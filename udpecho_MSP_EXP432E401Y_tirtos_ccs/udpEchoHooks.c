/*
 *    ======== udpEchoHooks.c ========
 */
#include <globals.h>

#include <ti/ndk/inc/netmain.h>

#include <ti/ndk/slnetif/slnetifndk.h>
#include <ti/net/slnet.h>
#include <ti/net/slnetif.h>
#include <ti/net/slnetutils.h>

#include <ti/drivers/emac/EMACMSP432E4.h>

#define UDPHANDLERSTACK 4196
#define IFPRI  4   /* Ethernet interface priority */

/* Prototypes */
extern void *UDPrxFxn(void *arg0);
extern void *UDPtxFxn(void *arg0);
/*
 *  ======== netIPAddrHook ========
 *  user defined network IP address hook
 */
void netIPAddrHook(uint32_t IPAddr, unsigned int IfIdx, unsigned int fAdd)
{
    pthread_t          thread;
    pthread_attr_t     attrs;
    struct sched_param priParam;
    int                retc;
    int                detachState;
    uint32_t           hostByteAddr;
    static uint16_t    udpPort = 1000;
    static bool        createTask = true;
    int32_t            status = 0;
    char txbuf[300];

    /* print the IP address that was added/removed */
    hostByteAddr = NDK_ntohl(IPAddr);
    //glo.REGISTERS[IP_SHADOW_REG] = hostByteAddr;

    sprintf(txbuf, "-print [Network Service] IP Assigned: %d.%d.%d.%d", (uint8_t)(hostByteAddr>>24)&0xFF, (uint8_t)(hostByteAddr>>16)&0xFF,
            (uint8_t)(hostByteAddr>>8)&0xFF, (uint8_t)hostByteAddr&0xFF);

    enqueue(txbuf);

    /* initialize SlNet interface(s) */
    status = ti_net_SlNet_initConfig();
    if (status < 0)
    {
        throwError("netudpError");
    }

    if (fAdd && createTask) {
        /*
         *  Create the Task that handles incoming UDP packets.
         *  udpPort will be the port that this task listens to.
         */

        /* Set priority and stack size attributes */
        pthread_attr_init(&attrs);
        priParam.sched_priority = 1;

        detachState = PTHREAD_CREATE_DETACHED;
        retc = pthread_attr_setdetachstate(&attrs, detachState);
        if (retc != 0) {
            throwError("netudpError");
        }

        pthread_attr_setschedparam(&attrs, &priParam);

        retc |= pthread_attr_setstacksize(&attrs, UDPHANDLERSTACK);
        if (retc != 0) {
            throwError("netudpError");
        }

        retc = pthread_create(&thread, &attrs, UDPrxFxn, (void *)&udpPort);
        if (retc != 0) {
            throwError("netudpError");
        }

        retc = pthread_create(&thread, &attrs, UDPtxFxn, (void *)&udpPort);
        if (retc != 0) {
            throwError("netudpError");
        }
        else // retc == 0
        {
            sprintf(txbuf, "-print [Network Service] UDP Port: %d", udpPort);
            enqueue(txbuf);
        }
        createTask = false;
    }
}

/*
 *  ======== serviceReport ========
 *  NDK service report.  Initially, just reports common system issues.
 */
void serviceReport(uint32_t item, uint32_t status, uint32_t report, void *h)
{
    static char *taskName[] = {"Telnet", "", "NAT", "DHCPS", "DHCPC", "DNS"};
    static char *reportStr[] = {"Starting", "Running", "Updated", "Complete", "Fault"};
    static char *statusStr[] = {"Disabled", "Waiting", "IPTerm", "Failed","Enabled"};
    char txbuf[300]= {0};

    sprintf(txbuf, "-print [Network Service] %s - %s - %s\n",
            taskName[item - 1], statusStr[status], reportStr[report / 256]);
    enqueue(txbuf);

    /* report common system issues */
    if ((item == CFGITEM_SERVICE_DHCPCLIENT) &&
            (status == CIS_SRV_STATUS_ENABLED) &&
            (report & NETTOOLS_STAT_FAULT))
    {
        throwError("netudpError");
    }
}
