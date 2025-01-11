/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_tirtos.c ========
 */
#include <globals.h>

extern void mainThread();
extern void queueThread();
extern void uart7Thread();
extern void streamThread();

extern Semaphore_Handle semaphore0;
extern Semaphore_Handle semaphore1;
extern Semaphore_Handle semaphore2;

extern void ti_ndk_config_Global_startupFxn();


/*
 *  ======== main ========
 */
void Sys_init() {
    /* Call driver init functions */
    Board_init();
    GPIO_init();
    UART_init();
    Timer_init();
    SPI_init();
    ADC_init();
    ADCBuf_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_PK5, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    GPIO_write(CONFIG_GPIO_PK5, 0);
    GPIO_write(CONFIG_GPIO_PD4, 1);

    GPIO_setCallback(CONFIG_GPIO_SW_1, sw1Callback);
    GPIO_setCallback(CONFIG_GPIO_SW_2, sw2Callback);

    GPIO_enableInt(CONFIG_GPIO_SW_1);
    GPIO_enableInt(CONFIG_GPIO_SW_2);

    Timer_Params_init(&global.timer0.Params);
    global.timer0.Params.periodUnits = Timer_PERIOD_US;
    global.timer0.Params.period = 2500000;
    global.timer0.Length = global.timer0.Params.period;
    global.timer0.Params.timerMode  = Timer_CONTINUOUS_CALLBACK;
    global.timer0.Params.timerCallback = timer0Callback;
    global.timer0.Handle = Timer_open(CONFIG_TIMER_0, &global.timer0.Params);
    if (global.timer0.Handle == NULL) {
        // Timer_open() failed
        while (1);
    }

    Timer_Params_init(&global.timer1.Params);
    global.timer1.Params.periodUnits = Timer_PERIOD_US;
    global.timer1.Params.period = 100000;
    global.timer1.Length = global.timer1.Params.period;
    global.timer1.Params.timerMode  = Timer_CONTINUOUS_CALLBACK;
    global.timer1.Params.timerCallback = timer1Callback;
    global.timer1.Handle = Timer_open(CONFIG_TIMER_1, &global.timer1.Params);
    if (global.timer1.Handle == NULL) {
        // Timer_open() failed
        while (1);
    }
    if (Timer_start(global.timer1.Handle) == Timer_STATUS_ERROR) {
        //Timer_start() failed
        while (1);
    }

    global.semaphore0.Handle = semaphore0;
    global.semaphore1.Handle = semaphore1;
    global.semaphore2.Handle = semaphore2;

    Swi_Params_init(&global.swi0.Params);
    global.swi0.Handle = Swi_create(SWITimer0, &global.swi0.Params, NULL);

    /* Create a UART with data processing off. */
    UART_Params_init(&global.uart0.Params);
    global.uart0.Params.writeDataMode = UART_DATA_BINARY;
    global.uart0.Params.readDataMode = UART_DATA_BINARY;
    global.uart0.Params.readReturnMode = UART_RETURN_FULL;
    global.uart0.Params.baudRate = 115200;

    global.uart0.Handle = UART_open(CONFIG_UART_0, &global.uart0.Params);

    UART_Params_init(&global.uart7.Params);
    global.uart7.Params.writeDataMode = UART_DATA_BINARY;
    global.uart7.Params.readDataMode = UART_DATA_TEXT;
    global.uart7.Params.readReturnMode = UART_RETURN_NEWLINE;
    global.uart7.Params.baudRate = 115200;
    global.uart7.Params.readEcho = UART_ECHO_OFF;

    global.uart7.Handle = UART_open(CONFIG_UART_1, &global.uart7.Params);


    SPI_Params_init(&global.spi0.Params);
    global.spi0.Params.dataSize = 16;
    global.spi0.Params.frameFormat = SPI_POL0_PHA1;
    global.spi0.Handle = SPI_open(CONFIG_SPI_0, &global.spi0.Params);

    if (global.spi0.Handle == NULL){
        while (1);
    }

    ti_ndk_config_Global_startupFxn();


    ADC_Handle adc0_handle, adc1_handle;
    ADC_Params adcParams;

    ADCBuf_Handle adcbuf_handle;
    ADCBuf_Params adcbuf_params;

    ADC_Params_init(&adcParams);
    adcParams.isProtected = true;

    adc0_handle = ADC_open(CONFIG_ADC_0, &adcParams);
    if (adc0_handle == NULL)
        while (1);
    adc1_handle = ADC_open(CONFIG_ADC_1, &adcParams);
    if (adc1_handle == NULL)
        while (1);

    ADCBuf_Params_init(&adcbuf_params);
    adcbuf_params.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
    adcbuf_params.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
    adcbuf_params.callbackFxn = ADC_BUFFER_CALLBACK_FXN;
    adcbuf_params.samplingFrequency = 8000;

    adcbuf_handle = ADCBuf_open(CONFIG_ADCBUF_0, &adcbuf_params);
    if(adcbuf_handle == NULL)
        while(1);

    global.adc.adc[0] = adc0_handle;
    global.adc.adc[1] = adc1_handle;
    global.adc.adcBuf = adcbuf_handle;
}


int main(void) {
    Sys_init();

    BIOS_start();

    return (0);
}
