#include <globals.h>

void queue() {
    uint32_t gatekey;
    while(1) {
        Semaphore_pend(global.semaphore0.Handle, BIOS_WAIT_FOREVER);

        //UART_write(global.uart0.Handle, global.queue.payloads[global.queue.front], strlen(global.queue.payloads[global.queue.front]));
        commandHandler(global.queue.payloads[global.queue.front]);

        gatekey = GateSwi_enter(global.gateSwi0.Handle);

        free(global.queue.payloads[global.queue.front]);
        global.queue.front++;

        if(global.queue.front == 100) {
            global.queue.front = 0;
            global.queue.rear = 0;
        }

        GateSwi_leave(global.gateSwi0.Handle, gatekey);
    }
}

void enqueue(char* value) {
    uint32_t gatekey;

    if (!value || value[0] == 0){
        return;
    }

    gatekey = GateSwi_enter(global.gateSwi0.Handle);

    int len = (strlen(value)+1);
    global.queue.payloads[global.queue.rear] = malloc(len);
    memset(global.queue.payloads[global.queue.rear], 0, len);
    strcpy(global.queue.payloads[global.queue.rear], value);
    global.queue.rear++;

    GateSwi_leave(global.gateSwi0.Handle, gatekey);
    Semaphore_post(global.semaphore0.Handle);
}
