#ifndef GLOBALS_H
#define GLOBALS_H

/*Standard Headers*/
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

/* Driver Header files */
#include <ti/drivers/Board.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/TIMER.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/ADC.h>
#include <ti/drivers/ADCBuf.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/gates/GateSwi.h>

#include <ti/devices/msp432e4/driverlib/udma.h>

/* Driver configuration */
#include "ti_drivers_config.h"

typedef struct ERROR {
    char errorName[64];
    char errorDesc[256];
    int count;
} ERROR;

typedef struct CALLBACK {
    char payload[128];
    int count;
    int counter;
} CALLBACK;

typedef struct TICKER {
    int finished;
    int notInitialRun;
    int initialDelay;
    int initialDelayCounter;
    int repeatDelay;
    int repeatDelayCounter;
    int repeatCount;
    int repeatCounter;
    char payload[128];
} TICKER;

typedef struct UART {
    UART_Handle Handle;
    UART_Params Params;
} UART;

typedef struct SPI {
    SPI_Handle Handle;
    SPI_Params Params;
} SPI;

typedef struct TIMER {
    Timer_Handle Handle;
    Timer_Params Params;
    uint32_t Length;
    int running;
} TIMER;

typedef struct SEMAPHORE {
    Semaphore_Handle Handle;
    Semaphore_Params Params;
} SEMAPHORE;

typedef struct SWI {
    Swi_Handle Handle;
    Swi_Params Params;
} SWI;

typedef struct GATESWI {
    GateSwi_Handle Handle;
    GateSwi_Params Params;
} GATESWI;

typedef struct QUEUE {
    char* payloads[100];
    int front;
    int rear;
} QUEUE;

typedef struct LUT {
    double lutPos;
    double lutDelta;
} LUT;

typedef struct _bufctrl {
    ADCBuf_Conversion conversion;
    uint16_t *RX_completed;
    uint16_t *TX_completed;
    uint32_t converting;
    int32_t TX_index;
    int32_t TX_correction;
    uint32_t ping_count;
    uint32_t pong_count;
    uint32_t callback_count;
    uint32_t callback0_count;
    uint32_t sample_count;
    uint32_t missed_255ADCs;
    uint32_t doubled_255ADCs;
    uint32_t delay;
} ADCBufControl;

typedef struct ADC {
    ADC_Handle adc[2];
    ADCBuf_Handle adcBuf;
    ADCBufControl adcbufctrl;
} ADC;

typedef struct GLOBAL {
    UART uart0;
    UART uart7;
    SPI spi0;
    TIMER timer0;
    TIMER timer1;
    QUEUE queue;
    QUEUE udpQueue;
    SEMAPHORE semaphore0;
    SEMAPHORE semaphore1;
    SEMAPHORE semaphore2;
    GATESWI gateSwi0;
    GATESWI gateSwi1;
    SWI swi0;
    ADC adc;
    LUT lut;
    char version[20];
    char assignmentNumber[2];
} GLOBAL;

extern GLOBAL global;
extern ERROR error[24];
extern CALLBACK callback[3];
extern CALLBACK scripts[64];
extern TICKER tickers[16];

extern int errorCount;

extern int32_t registers[32];

extern uint16_t sinTable[257];

//PARSER
void parser();
void commandHandler(char*);

//QUEUE
void queue();
void enqueue(char *);
void udpEnqueue(char*);

//UART7
void uart7();

//STREAM
void stream();

//SINE
void sineHandler();

//COMMANDS
void aboutCommand();
void audioCommand();
void callbackCommand(char *);
void errorCommand(char*);
void gpioCommand(char*);
void helpCommand(char*);
void ifCommand(char*);
void netudpCommand(char*);
void memrCommand(char*);
void printCommand(char*);
void regCommand(char*);
void scriptCommand(char*);
void sineCommand(char*);
void streamCommand(char*);
void tickerCommand(char*);
void timerCommand(char*);
void uartCommand(char*);
void voiceCommand(char*);

//GLOBALS
void writeUART(char*, int);
void throwError(char *);
int isNumber(char *);
void splitStringBySpaces(char *, char [10][128], int *);

//CALLBACKS
void timer0Callback(Timer_Handle TimerHandle, int_fast16_t status);
void SWITimer0(UArg uarg1, UArg uarg2);
void timer1Callback(Timer_Handle TimerHandle, int_fast16_t status);
void sw1Callback(uint_least8_t index);
void sw2Callback(uint_least8_t index);
void ADC_BUFFER_CALLBACK_FXN(ADCBuf_Handle handle,ADCBuf_Conversion *conversion,void *completedADCBuffer,uint32_t completedChannel,int_fast16_t status);

//REGISTERS
void MOV(int dest, int src, int isImm);    // Move (with immediate)
void XCHG(int reg1, int reg2);             // Exchange
void INC(int reg);                         // Increment
void DEC(int reg);                         // Decrement
void ADD(int dest, int src, int isImm);    // Add (with immediate)
void SUB(int dest, int src, int isImm);    // Subtract (with immediate)
void NEG(int reg);                         // Negate
void NOT(int reg);                         // Bitwise NOT
void AND(int dest, int src, int isImm);    // Bitwise AND (with immediate)
void IOR(int dest, int src, int isImm);    // Bitwise OR (with immediate)
void XOR(int dest, int src, int isImm);    // Bitwise XOR (with immediate)
void MUL(int dest, int src, int isImm);    // Multiply (with immediate)
void DIV(int dest, int src, int isImm);    // Division (with immediate)
void REM(int dest, int src, int isImm);    // Remainder (with immediate)
void MAX(int dest, int src, int isImm);    // Maximum (with immediate)
void MIN(int dest, int src, int isImm);    // Minimum (with immediate)

#endif
