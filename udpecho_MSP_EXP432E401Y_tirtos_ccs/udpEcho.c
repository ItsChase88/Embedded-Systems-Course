#include <globals.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include <ti/net/slnetutils.h>

#define UDPPACKETSIZE 1472
#define MAXPORTLEN    6

extern void fdOpenSession();
extern void fdCloseSession();
extern void *TaskSelf();

char *UDPParse(char *buff, struct sockaddr_in *clientAddr); // UDP payload parser

/*
 *  Receives UDP packets.
 *
 */
void *UDPrxFxn(void *arg0)
{
    int                bytesRcvd;
    int                status;
    int                server = -1;
    fd_set             readSet;
    struct addrinfo    hints;
    struct addrinfo    *res, *p;
    struct sockaddr_in clientAddr;
    socklen_t          addrlen;
    char               buffer[UDPPACKETSIZE + 1];   // adding 0 @ the end of str
    char               portNumber[MAXPORTLEN];
    char               txbuf[300];

    fdOpenSession(TaskSelf());

    enqueue("-print [Network Service] UDP RX started");

    sprintf(portNumber, "%d", *(uint16_t *)arg0);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    /* Obtain addresses suitable for binding to */
    status = getaddrinfo(NULL, portNumber, &hints, &res);
    if (status != 0) {
        throwError("netudpError");
        goto shutdown;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        server = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server == -1) {
            continue;
        }

        status = bind(server, p->ai_addr, p->ai_addrlen);
        if (status != -1) {
            break;
        }

        close(server);
    }

    if (server == -1) {
        throwError("netudpError");
        goto shutdown;
    } else if (p == NULL) {
        throwError("netudpError");
        goto shutdown;
    } else {
        freeaddrinfo(res);
        res = NULL;
    }

    do {
        /*
         *  readSet and addrlen are value-result arguments, which must be reset
         *  in between each select() and recvfrom() call
         */
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);
        addrlen = sizeof(clientAddr);

        /* Wait forever for the reply */
        status = select(server + 1, &readSet, NULL, NULL, NULL);
        if (status > 0) {
            if (FD_ISSET(server, &readSet)) {
                bytesRcvd = recvfrom(server, buffer, UDPPACKETSIZE, 0,
                        (struct sockaddr *)&clientAddr, &addrlen);

                if (bytesRcvd > 0)
                {

                    buffer[bytesRcvd] = 0; // one extra byte in buffer so this is safe for strlen
                    sprintf(txbuf, "-print [UDP %d.%d.%d.%d:%d] ",
                              (uint8_t)(clientAddr.sin_addr.s_addr) &0xFF, (uint8_t)(clientAddr.sin_addr.s_addr>>8) &0xFF,
                              (uint8_t)(clientAddr.sin_addr.s_addr>>16) &0xFF, (uint8_t)(clientAddr.sin_addr.s_addr>>24) &0xFF,
                              clientAddr.sin_port);
                    enqueue(txbuf);
                    enqueue(buffer);
                }
            }
        }
    } while (status > 0);

shutdown:
    if (res) {
        freeaddrinfo(res);
    }

    if (server != -1) {
        close(server);
    }

    fdCloseSession(TaskSelf());

    return (NULL);
}

/*
 *  Transmits UDP packets.
 *
 */
void *UDPtxFxn(void *arg0)
{
    int                bytesSent;
    int                status;
    int                server = -1;
    fd_set             readSet;
    fd_set             writeSet;
    struct addrinfo    hints;
    struct addrinfo    *res, *p;
    struct sockaddr_in clientAddr;
    socklen_t          addrlen;
    char               portNumber[MAXPORTLEN];
    char               txbuf[300];
    char               *token;
    int32_t            payloadnext;
    int                bytesRequested;
    uint32_t gatekey;

    fdOpenSession(TaskSelf());
    enqueue("-print [Network Service] UDP TX started");
    enqueue("-print [Network Service] You are now connected!\r\n");
    enqueue("-print Please enter your command..");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    //Obtain addresses suitable for binding to
    status = getaddrinfo(NULL, portNumber, &hints, &res);
    if (status != 0) {
        throwError("netudpError");
        goto shutdown;
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        server = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server == -1)
            continue;
    }

    if (server == -1)
    {
        throwError("netudpError");
        goto shutdown;
    }
    else // server is valid
    {
        freeaddrinfo(res);
        res = NULL;
    }

    for(;;)
    {

        //readSet and addrlen are value-result arguments, which must be reset in between each select() and recvfrom() call
        FD_ZERO(&readSet);
        FD_SET(server, &readSet);
        addrlen = sizeof(clientAddr);

        // Wait forever for the reply
        Semaphore_pend(global.semaphore1.Handle, BIOS_WAIT_FOREVER);

        status = select(server + 1, NULL, &writeSet, NULL, NULL);

        if (status <= 0)
        {
            throwError("netudpError");
            continue;
        }

        token = UDPParse(global.udpQueue.payloads[global.udpQueue.front], &clientAddr);

        if (!token)
            continue;

        bytesRequested = strlen(token) + 129;

        bytesSent = sendto(server, token, bytesRequested, 0,
                (struct sockaddr *)&clientAddr, addrlen);

        if (bytesSent < 0 || bytesSent != bytesRequested)
            throwError("netudpError");

        gatekey = GateSwi_enter(global.gateSwi1.Handle);

        free(global.udpQueue.payloads[global.udpQueue.front]);
        global.udpQueue.front++;

        if(global.udpQueue.front == 100) {
            global.udpQueue.front = 0;
            global.udpQueue.rear = 0;
        }

        GateSwi_leave(global.gateSwi1.Handle, gatekey);
    }

shutdown:
    if (res) {
        freeaddrinfo(res);
    }

    if (server != -1) {
        close(server);
    }

    fdCloseSession(TaskSelf());

    return (NULL);
}

void udpEnqueue(char* value) {
    uint32_t gatekey;

    if (!value || value[0] == 0){
        return;
    }

    gatekey = GateSwi_enter(global.gateSwi1.Handle);

    int len = (strlen(value)+1);
    global.udpQueue.payloads[global.udpQueue.rear] = malloc(len);
    memset(global.udpQueue.payloads[global.udpQueue.rear], 0, len);
    strcpy(global.udpQueue.payloads[global.udpQueue.rear], value);
    global.udpQueue.rear++;

    GateSwi_leave(global.gateSwi1.Handle, gatekey);
    Semaphore_post(global.semaphore1.Handle);
}

// extract an ip, port, and payload from an incoming stream of chars
char *UDPParse(char *buff, struct sockaddr_in *clientAddr)
{
    char *token;
    char *colon;
    int32_t AddrByte;
    uint32_t PortWord;

    token = buff;
    if(!token) return token;

    // verify byte 1
    if(isdigit(*token) == 0)
    {
        throwError("netudpError");
        return NULL;
    }
    AddrByte = atoi(token);
    clientAddr->sin_addr.s_addr = AddrByte;

    //byte 2
    token = strchr(token, '.');
    if(!token) return token;
    token++;
    if(*token == 0) return NULL;
    if(isdigit(*token)==0) return NULL;
    AddrByte = atoi(token);
    clientAddr->sin_addr.s_addr |= AddrByte << 8;

    // byte 3
    token = strchr(token, '.');
    if(!token) return token;
    token++;
    if(*token == 0) return NULL;
    if(isdigit(*token)==0) return NULL;
    AddrByte = atoi(token);
    clientAddr->sin_addr.s_addr |= AddrByte << 16;

    // byte 4
    token = strchr(token, '.');
    if(!token) return token;
    token++;
    if(*token == 0) return NULL;
    if(isdigit(*token)==0) return NULL;
    AddrByte = atoi(token);
    clientAddr->sin_addr.s_addr |= AddrByte << 24;

    // get the port
    colon = strchr(token, ':');
    if(!colon)
    {
        PortWord = 1000;
        token = strchr(token, ' ');
    }
    else
    {
        token = colon;
        token++;
        if(*token == 0) return NULL;
        if(isdigit(*token) == 0) return NULL;
        PortWord = atoi(token);
    }
    clientAddr->sin_port = (PortWord & 0xFF00) >> 8;
    clientAddr->sin_port |= (PortWord & 0xFF) << 8;
    clientAddr->sin_family = AF_INET;

    // extract the payload, if any
    if(token)
        token = strchr(token, '-');
    return token;
}
