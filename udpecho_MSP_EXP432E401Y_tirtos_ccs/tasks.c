#include <globals.h>

void mainThread() {
    parser();
    return;
}

void queueThread() {
    queue();
    return;
}

void uart7Thread() {
    uart7();
    return;
}

void streamThread() {
    stream();
    return;
}
