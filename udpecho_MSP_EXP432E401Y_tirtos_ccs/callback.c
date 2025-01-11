#include <globals.h>

void timer0Callback(Timer_Handle TimerHandle, int_fast16_t status) {
    Swi_post(global.swi0.Handle);
}

void SWITimer0(UArg uarg1, UArg uarg2) {
   if(callback[0].count < 0) {
       commandHandler(callback[0].payload);
   } else if(callback[0].counter < callback[0].count) {
       commandHandler(callback[0].payload);
       callback[0].counter++;
   } else {
       Timer_stop(global.timer0.Handle);
   }
}

void timer1Callback(Timer_Handle TimerHandle, int_fast16_t status) {
    int i;
    for(i = 0; i < 16; i++) {
        if(tickers[i].finished == 0) {
            continue;
        }

        if(tickers[i].initialDelayCounter < tickers[i].initialDelay) {
            tickers[i].initialDelayCounter = tickers[i].initialDelayCounter + 10;
            continue;
        }

        if(tickers[i].notInitialRun != 0) {
            if(tickers[i].repeatDelayCounter < tickers[i].repeatDelay) {
                tickers[i].repeatDelayCounter = tickers[i].repeatDelayCounter + 10;
                continue;
            }
        }

        commandHandler(tickers[i].payload);

        tickers[i].notInitialRun = 1;
        tickers[i].repeatDelayCounter = 0;

        if(tickers[i].repeatCount < 0) {
            continue;
        } else if(tickers[i].repeatCounter < tickers[i].repeatCount) {
            tickers[i].repeatCounter++;
        } else {
            tickers[i].finished = 0;
        }
    }
}

void sw1Callback(uint_least8_t index) {
    if(callback[1].count < 0) {
        commandHandler(callback[1].payload);
    } else if(callback[1].counter < callback[1].count) {
        commandHandler(callback[1].payload);
        callback[1].counter++;
    }
}

void sw2Callback(uint_least8_t index) {
    if(callback[2].count < 0) {
        commandHandler(callback[2].payload);
    } else if(callback[2].counter < callback[2].count) {
        commandHandler(callback[2].payload);
        callback[2].counter++;
    }
}

void ADC_BUFFER_CALLBACK_FXN(ADCBuf_Handle handle, ADCBuf_Conversion *conversion, void *completedADCBuffer, uint32_t completedChannel, int_fast16_t status) {
    global.adc.adcbufctrl.RX_completed = completedADCBuffer;
    global.adc.adcbufctrl.callback_count++;
    Semaphore_post(global.semaphore2.Handle);
}
