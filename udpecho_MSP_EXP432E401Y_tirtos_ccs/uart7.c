#include <globals.h>

void uart7() {
    char input[320];
    int_fast32_t len;
    while(1) {
        len = UART_read(global.uart7.Handle, input, 320); //3rd param is causing issues bc it waits for that amount of chars before returning from semaphore.

        input[len - 1] = 0;
        enqueue("-print [UART7]");
        enqueue(input);
    }
}
