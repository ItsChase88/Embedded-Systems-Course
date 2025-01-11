#include <globals.h>

void sineHandler() {
    double lowerW, upperW;
    double answer;
    uint32_t loweridx, upperidx;
    uint16_t outval;
    bool transferOK;
    SPI_Transaction spiTransaction;

    // Get indexes and weights for the lut
    loweridx = (uint32_t)global.lut.lutPos;
    upperidx = (loweridx + 1);
    upperW = global.lut.lutPos - (double)loweridx;
    lowerW = 1.0 - upperW;
    answer = (double)sinTable[loweridx] * lowerW + (double)sinTable[upperidx] * upperW;
    outval = round(answer);

    spiTransaction.count = 1;
    spiTransaction.txBuf = (void *)&outval;
    spiTransaction.rxBuf = (void *)NULL;

    transferOK = SPI_transfer(global.spi0.Handle, &spiTransaction);
    if (!transferOK){
        throwError("audioBuffer");
        return;
    }

    // Set position relative to delta
    global.lut.lutPos += global.lut.lutDelta;
    while(global.lut.lutPos >= (double)256){
        global.lut.lutPos -= (double)256;
    }
}
