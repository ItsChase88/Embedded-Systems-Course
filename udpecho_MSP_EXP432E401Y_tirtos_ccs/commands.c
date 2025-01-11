#include <globals.h>

//About command
void aboutCommand() {
    char output[128] = "";
    snprintf(output, sizeof(output), "Robert Chase Bonham - Assignment %s - Version %s - Compiled: %s - %s", global.assignmentNumber, global.version, __TIME__, __DATE__);
    writeUART(output, 1);
    return;
}

void audioCommand() {

}

//Callback command
void callbackCommand(char* params) {
    int length = strlen(params);

    char output[256] = "";

    if(length == 0) { // -callback
        int i;
        for(i = 0; i < 3; i++) {
            char localOutput[100] = "";
            snprintf(localOutput, sizeof(localOutput), "Callback %d C %d: %s\r\n", i, callback[i].count, callback[i].payload);
            strcat(output, localOutput);
        }
        writeUART(output, 1);
        return;
    } else if(length == 1) { // -callback #
        int callbackIndex = atoi(params);
        if(callbackIndex > 2) {
            throwError("invalidCallback");
            return;
        }

        snprintf(output, sizeof(output), "Callback %d C %d: %s\r\n", callbackIndex, callback[callbackIndex].count, callback[callbackIndex].payload);
        writeUART(output, 1);
        return;
    } else {

        char cmd[10][128];
        char cmdValue[128] = "";
        int cmdParts;
        int i;

        char paramsCopy[256];
        strncpy(paramsCopy, params, sizeof(paramsCopy) - 1);
        paramsCopy[255] = '\0'; // Null-terminate the copy
        splitStringBySpaces(paramsCopy, cmd, &cmdParts);

        //splitStringBySpaces(params, cmd, &cmdParts);

        for(i = 2; i < cmdParts; i++) {
            strcat(cmdValue, cmd[i]);
            if(i + 1 != cmdParts) {
                strcat(cmdValue, " ");
            }
        }

        int callbackIndex = atoi(cmd[0]);
        int callbackCount = atoi(cmd[1]);

        if(callbackIndex > 2) {
            throwError("invalidCallback");
            return;
        }

        strcpy(callback[callbackIndex].payload, cmdValue);
        callback[callbackIndex].count = callbackCount;
        callback[callbackIndex].counter = 0;

        snprintf(output, sizeof(output), "Callback: %s Count: %s Command: %s", cmd[0], cmd[1], cmdValue);
        writeUART(output, 1);

        if (callbackIndex == 0) { //Timer
            if(global.timer0.running == 0) {
                if (Timer_start(global.timer0.Handle) == Timer_STATUS_ERROR) {
                    //Timer_start() failed
                    while (1);
                }
                global.timer0.running = 1;
            }

            return;
        }

        return;
    }
}

//Error command
void errorCommand(char* params) {
    char output[1000] = "";
    if(strlen(params) > 0) {
        if(strcmp(params, "clear") == 0) {
            int i;
            for(i = 0; i < errorCount; i++) {
                error[i].count = 0;
            }
            writeUART("Cleared the error counts.", 1);
            return;
        } else if(isNumber(params)) {
            int index = strtol(params, NULL, 10);
            if(index < errorCount) {
                char outputStr[128] = "";
                strcat(output, "Count | Error\r\n");
                snprintf(outputStr, sizeof(outputStr), "%d     | %s \r\n", error[index].count, error[index].errorDesc);
                strcat(output, outputStr);
                writeUART(output, 1);
            } else {
                throwError("errorCommandNotFound");
            }
        } else {
            throwError("errorCommandNotFound");
        }
    } else {
        int i;
        strcat(output, "Index  | Count | Error\r\n");
        for(i = 0; i < errorCount; i++) {
            char outputStr[128] = "";
            if(i < 10) {
                snprintf(outputStr, sizeof(outputStr), "%d      | %d     | %s\r\n", i, error[i].count, error[i].errorDesc);
            } else {
                snprintf(outputStr, sizeof(outputStr), "%d     | %d     | %s\r\n", i, error[i].count, error[i].errorDesc);
            }
            strcat(output, outputStr);
        }

        writeUART(output, 1);
    }
    return;
}

//GPIO command
void gpioCommand(char* params) {
    int length = strlen(params);
    char output[500] = "";

    if(length == 0) { //-gpio
        int i;
        for(i = 0; i < 8; i++) {
            char localOutput[128] = "";
            int gpioState;
            gpioState = GPIO_read(i);

            snprintf(localOutput, sizeof(localOutput), "GPIO Pin: %d is set to the value: %d\r\n", i, gpioState);
            strcat(output, localOutput);
        }
        writeUART(output, 1);
        return;
    } else if(length == 1) { //-gpio #
        int gpioPin = atoi(params);
        if(gpioPin >= 0) {
            int gpioState;
            gpioState = GPIO_read(gpioPin);
            if(gpioState == 0) {
                writeUART("GPIO is set to state OFF.", 1);
            } else {
                writeUART("GPIO is set to state ON.", 1);
            }
        } else {
            throwError("invalidGPIO");
        }
    } else if(length == 3) { //-gpio # t
        if(strcmp(&params[2], "t") == 0) {
            int gpioPin = atoi(&params[0]);
            int gpioState;
            gpioState = GPIO_read(gpioPin);
            if(gpioState == 0) {
                GPIO_write(gpioPin, 1);
                writeUART("GPIO Pin toggled to state ON.", 1);
            } else {
                GPIO_write(gpioPin, 0);
                writeUART("GPIO Pin toggled to state OFF.", 1);
            }
        } else {
            throwError("invalidGPIO");
        }
    } else if(length > 3) { //-gpio # w VAL
        char* valueStr = (char *)params+4;
        char gpioPinStr = *params;
        char* cmdStr = (char *)params+2;
        char cmd = *cmdStr;

        int gpioPin = atoi(&gpioPinStr);
        int gpioVal = atoi(valueStr);
        if(cmd == 'w') {
            char localOutput[128] = "";
            snprintf(localOutput, sizeof(localOutput), "%s %d %s %d", "GPIO Pin:", gpioPin, "value set to:", gpioVal);

            GPIO_write(gpioPin, gpioVal);
            strcat(output, localOutput);
            writeUART(output, 1);
        } else {
            throwError("invalidGPIO");
        }

        return;
    } else {
        throwError("invalidGPIO");
    }
    return;
}

//Help command
void helpCommand(char* params) {
    //Check to see if there are any params.
    if(strlen(params)<1) {
        writeUART("\r\nList of Commands:\r\n-about\r\n-audio\r\n-callback\r\n-clear\r\n-error\r\n-gpio\r\n-help\r\n-if\r\n-memr\r\n-netudp\r\n-print\r\n-reg\r\n-rem\r\n-script\r\n-sine\r\n-stream\r\n-ticker\r\n-timer\r\n-uart\r\n-help", 1);
        return;
    }

    //Check which command the user requested help for.
    if(strcmp(params, "about") == 0) {
        writeUART("\r\nThe about command is used to print the authors name, the assignment number, the current version, and the date / time of the compile.", 1);
    } else if(strcmp(params, "audio") == 0) {
        writeUART("\r\nThe audio command allows you to play audio out of the speaker from a source.\r\n", 1);
    } else if(strcmp(params, "callback") == 0) {
        writeUART("\r\nThe callback command allows you to tie commands to a callback index.\r\nCallback 0: Timer\r\nCallback 1: SW1 (Right switch)\r\nCallback 2: SW2 (Left Switch)", 1);
    } else if(strcmp(params, "clear") == 0) {
        writeUART("\r\nThe clear command is used to clear the console.", 1);
    } else if(strcmp(params, "error") == 0) {
        writeUART("\r\nThe error command is used to print the error count of the various error types.\r\nUse -error clear to clear the count of all errors.", 1);
    } else if(strcmp(params, "gpio") == 0) {
        writeUART("\r\n-gpio - show all gpio pins\r\n-gpio 0 - show gpio 0 - LED1 - PN1\r\n-gpio 1 - show gpio 1 - LED2 - PN0\r\n-gpio 2 - show gpio 2 - LED3 - PF4\r\n-gpio 3 - show gpio 3 - LED4 - PF0\r\n-gpio 5 - show gpio 5 - PK5\r\n-gpio 6 - show gpio 6 - PD4\r\n-gpio 6 - show gpio 6 - SW1 - PJ0\r\n-gpio 7 - show gpio 7 - SW2 - PJ1\r\n-gpio # t - toggle gpio #\r\n-gpio # w VAL - write VAL to gpio #", 1);
    } else if(strcmp(params, "help") == 0) {
        writeUART("\r\nThe help command is used to view a list of commands along with help with commands.", 1);
    } else if(strcmp(params, "if") == 0) {
        writeUART("\r\nThe if command is used to check conditionals between two regs or imm values.\r\n-if [REG1/IMM1] [</>/=] [REG2/IMM2] ? [TRUE COMMAND] : [FALSE COMMAND]", 1);
    } else if(strcmp(params, "memr") == 0) {
        writeUART("\r\nThe memr command is used to read memory locations.\r\nValid Memory Ranges:\r\nFLASH: 0x00000000-0x000FFFFF\r\nSRAM: 0x20000000-0x2003FFFF", 1);
    } else if(strcmp(params, "netudp") == 0) {
        writeUART("\r\nThe netudp command is used to send commands to an IP and a port.\r\n-netudp IPADDR:PORT payload", 1);
    } else if(strcmp(params, "print") == 0) {
        writeUART("\r\nThe print command echos back user input.", 1);
    } else if(strcmp(params, "reg") == 0) {
        writeUART("\r\nThe reg command is used to create and view registers.\r\n-reg [OPERATION] [REG1 (DEST)] [Optional REG2]", 1);
    } else if(strcmp(params, "rem") == 0) {
        writeUART("\r\nThe rem command is used to make remarks.", 1);
    } else if(strcmp(params, "script") == 0) {
        writeUART("\r\nThe script command is used to create scripts to deliver multiple commands at once.\r\nValid Script Lines: 0-63\r\n-script - Shows all inputed lines\r\n-script clear - Clear the script lines.", 1);
    } else if(strcmp(params, "sine") == 0) {
        writeUART("\r\nThe sine command is used to play a sine wave from a given frequency through the audio port.\r\n-sine FREQ", 1);
    } else if(strcmp(params, "stream") == 0) {
        writeUART("\r\nThe stream command is used to setup a stream.", 1);
    } else if(strcmp(params, "ticker") == 0) {
        writeUART("\r\nThe ticker command is used to allow forward scheduling of actions.\r\n-ticker - Shows all tickers (0-15)\r\n-ticker # - Shows a specific ticker\r\n-ticker # clear - Clear a specific ticker\r\n-ticker # delay repeat count payload", 1);
    } else if(strcmp(params, "timer") == 0) {
        writeUART("\r\nThe timer command is used to change the frequency of the timer in microseconds. Less than 1,000 microseconds Invalid.", 1);
    } else if(strcmp(params, "uart") == 0) {
        writeUART("\r\nThe uart command is used to send commands to a different UART.\r\nPin PC4: Input\r\nPin PC5: Output", 1);
    } else {
        throwError("helpCommandNotFound");
    }

    return;
}

void ifCommand(char* params) { //-if 10 > #0 ? -script 5 : -print FALSE
    if(strlen(params) == 0) {
        throwError("ifMissingParams");
    }

    char cmd[10][128];
    int cmdParts;

    splitStringBySpaces(params, cmd, &cmdParts);

    if(cmdParts < 7) {
        throwError("ifMissingParams");
        return;
    }

    if(strcmp(cmd[3], "?") != 0) {
        throwError("ifInvalidParams");
        return;
    }

    int i;
    int colonFound = 0;
    char cmdTrue[128] = "";
    char cmdFalse[128] = "";
    for(i = 4; i < cmdParts; i++) {
        if(colonFound == 0) {
            if(strcmp(cmd[i], ":") == 0) {
                colonFound = 1;
                continue;
            } else if(strcmp(cmd[i+1], ":") == 0) {
                strcat(cmdTrue, cmd[i]);
            } else {
                strcat(cmdTrue, cmd[i]);
                strcat(cmdTrue, " ");
            }
        } else {
            if(strcmp(cmd[i+1], "") == 0) {
                strcat(cmdFalse, cmd[i]);
            } else {
                strcat(cmdFalse, cmd[i]);
                strcat(cmdFalse, " ");
            }
        }
    }

    if(colonFound == 0) {
        throwError("ifInvalidParams");
        return;
    }

    int reg1;
    int reg2;

    if(strstr(cmd[0], "#") != NULL) {
        memmove(cmd[0], cmd[0]+1, strlen(cmd[0]));
        reg1 = atoi(cmd[0]);
    } else {
        reg1 = atoi(cmd[0]);
        reg1 = registers[reg1];
    }

    if(strstr(cmd[2], "#") != NULL) {
        memmove(cmd[2], cmd[2]+1, strlen(cmd[2]));
        reg2 = atoi(cmd[2]);
    } else {
        reg2 = atoi(cmd[2]);
        reg2 = registers[reg2];
    }

    if(strcmp(cmd[1], ">") == 0) {
        if(reg1 > reg2) {
            commandHandler(cmdTrue);
        } else {
            commandHandler(cmdFalse);
        }
    } else if(strcmp(cmd[1], "<") == 0) {
        if(reg1 < reg2) {
            commandHandler(cmdTrue);
        } else {
            commandHandler(cmdFalse);
        }
    } else if(strcmp(cmd[1], "=") == 0) {
        if(reg1 == reg2) {
            commandHandler(cmdTrue);
        } else {
            commandHandler(cmdFalse);
        }
    } else {
        throwError("ifInvalidParams");
    }
}

//Memr command
void memrCommand(char* params) {
    uint32_t* memoryAddress;
    int32_t memoryValue;
    long hexInput;

    //Mask the hex input and conver to a long.
    hexInput = 0xFFFFFFFC & strtol(params, NULL, 16);

    //Check to make sure the memory address is in valid range.
    if (hexInput > 0x100000 && hexInput < 0x20000000) {
        throwError("memoryAddressOOR");
        return;
    } else if(hexInput > 0x20040000 && hexInput < 40000000) {
        throwError("memoryAddressOOR");
        return;
    } else if(hexInput >= 0x22800000) {
        throwError("memoryAddressOOR");
        return;
    } else {
        char output[300] = "";

        //Convert the memory address into a memory value.
        memoryAddress = (uint32_t*)hexInput;
        memoryValue = *memoryAddress;

        //Convert the memory value to a string.
        snprintf(output, sizeof(output), "\r\n0x%08X", memoryValue);

        writeUART(output, 1);
        return;
    }
}

//NetUDP command
void netudpCommand(char* params) {
    if(strlen(params) == 0) { //-netudp
        throwError("netudpMissingParams");
        return;
    }

    udpEnqueue(params);
    writeUART("Attempting to send payload...", 1);
}

//Print command
void printCommand(char* params) {
    writeUART(params, 1);
    return;
}

//Reg command
void regCommand(char* params) { //-reg [OPERATION] [REG1] [REG2]
    char output[1000] = "";
    if(strlen(params) == 0) { //-reg
        int i;
        char localOutput[128] = "";
        strcat(output, "Registers:\r\n");
        for(i = 0; i < 32; i++) {
            sprintf(localOutput, "Reg %d: %d | Reg: %d: %d\r\n", i, registers[i], i+1, registers[i+1]);
            strcat(output, localOutput);
            i++;
        }
        writeUART(output, 1);
        return;
    }

    char cmd[10][128];
    int cmdParts;

    splitStringBySpaces(params, cmd, &cmdParts);

    if(cmdParts < 2) {
        throwError("regMissingParams");
        return;
    }

    int reg1;
    int reg2;
    int immValue = 0;

    if(cmdParts == 2) {
        if(strstr(cmd[1], "#") != NULL) {
            throwError("regImmNotDest");
            return;
        }
        reg1 = atoi(cmd[1]);
    } else {
        if(strstr(cmd[1], "#") != NULL) {
            throwError("regImmNotDest");
            return;
        }
        reg1 = atoi(cmd[1]);
        if(strstr(cmd[2], "#") != NULL) {
            memmove(cmd[2], cmd[2]+1, strlen(cmd[2]));
            immValue = 1;
            reg2 = atoi(cmd[2]);
        } else {
            reg2 = atoi(cmd[2]);
            if(reg2 > 31) {
                throwError("regInvalidIndex");
                return;
            }
        }
    }

    if(reg1 > 31) {
        throwError("regInvalidIndex");
        return;
    }

    if(strcmp(cmd[0], "MOV") == 0) {
        MOV(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "XCHG") == 0) {
        XCHG(reg1, reg2);
    } else if(strcmp(cmd[0], "INC") == 0) {
        INC(reg1);
    } else if(strcmp(cmd[0], "DEC") == 0) {
        DEC(reg1);
    } else if(strcmp(cmd[0], "ADD") == 0) {
        ADD(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "SUB") == 0) {
        SUB(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "NEG") == 0) {
        NEG(reg1);
    } else if(strcmp(cmd[0], "NOT") == 0) {
        NOT(reg1);
    } else if(strcmp(cmd[0], "AND") == 0) {
        AND(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "IOR") == 0) {
        IOR(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "XOR") == 0) {
        XOR(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "MUL") == 0) {
        MUL(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "DIV") == 0) {
        DIV(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "REM") == 0) {
        REM(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "MAX") == 0) {
        MAX(reg1, reg2, immValue);
    } else if(strcmp(cmd[0], "MIN") == 0) {
        MIN(reg1, reg2, immValue);
    } else {
        throwError("regInvalidOp");
        return;
    }

    sprintf(output, "Operation %s performed.", cmd[0]);
    writeUART(output, 1);

    return;
}

//Script command
void scriptCommand(char* params) {
    char output[1000] = "";
    if(strlen(params) == 0) { //-script
        int i;
        strcat(output, "Script Space:\r\n");
        int scriptCount = 0;
        for(i = 0; i < 64; i++) {
            char localOutput[200];
            if(strcmp(scripts[i].payload, "") != 0) {
                sprintf(localOutput, "Script Line %d = %s\r\n", i, scripts[i].payload);
                strcat(output, localOutput);
                scriptCount++;
            }
        }
        if(scriptCount == 0) {
            strcat(output, "Currently there are no scripts written for lines 0 to 63.");
        }
        writeUART(output, 1);
        return;
    } else if(strlen(params) > 0) { //-script # OR -script # payload
        char cmd[10][128];
        int cmdParts;
        int index;

        splitStringBySpaces(params, cmd, &cmdParts);

        if(strcmp(cmd[0], "clear") == 0) {
            int i;
            for(i = 0; i < 64; i++) {
                strcpy(scripts[i].payload, "");
            }
            writeUART("All script lines have been cleared.", 1);
            return;
        }

        index = atoi(cmd[0]);

        if(index < 0 || index > 63) {
            throwError("invalidScriptRange");
            return;
        }

        if(cmdParts == 1) { //-script #
            if(strcmp(scripts[index].payload, "") == 0) {
                sprintf(output, "Script Line %d is set to NULL. No script will run.", index);
                writeUART(output, 1);
            } else {
                int i;
                for(i = index; i < 65; i++) {
                    if((strcmp(scripts[index].payload, "") == 0) || i == 64) {
                        sprintf(output, "Script %s has concluded.", cmd[0]);
                        writeUART(output, 1);
                        return;
                    }

                    commandHandler(scripts[index].payload);
                    index++;
                }
            }

        } else { //-script # payload
            int i;
            char payload[128] = "";
            for(i = 1; i < cmdParts; i++) {
                strcat(payload, cmd[i]);
                if(i + 1 != cmdParts) {
                    strcat(payload, " ");
                }
            }

            char* compare = malloc(strlen(payload)+1);
            memset(compare, 0, strlen(payload)+1);
            sprintf(compare, "-script %d", index);

            if(strcmp(payload, compare) == 0) {
                throwError("scriptPayloadLoop");
                free(compare);
                return;
            }

            free(compare);

            strcpy(scripts[index].payload, payload);
            sprintf(output, "Script Line %d = %s", index, payload);
            writeUART(output, 1);
        }

        return;
    }
}

void sineCommand(char* params) {
    if(strlen(params) == 0) {
        sineHandler();
        return;
    }

    int freq;

    freq = atoi(params);

    if(freq < 0 || freq > 3999) {
        throwError("invalidSineFreq");
        return;
    }

    timerCommand("125"); //Set timer to 125

    global.lut.lutDelta = (double)freq * (double)256 * (double)125/1000000.0;

    if (global.lut.lutDelta >= 256 / 2) {
        throwError("nyquistViolation");
        return;
    } else if (global.lut.lutDelta <= 0) {
        timerCommand("0");
        writeUART("Timer is off.", 1);
        return;
    }

    callbackCommand("0 -1 -sine");

    sineHandler();
}

void streamCommand(char* params) {

}

void tickerCommand(char* params) {
    char output[1000] = "";
    if(strlen(params) == 0) {// -ticker
        char localOutput[128] = "";
        int i;
        strcat(output, "Tickers:\r\n");
        for(i = 0; i < 16; i++) {
            sprintf(localOutput, "Ticker %d | Delay: %d Repeat: %d RepeatCount: %d Payload: %s\r\n", i, tickers[i].initialDelay, tickers[i].repeatDelay, tickers[i].repeatCount, tickers[i].payload);
            strcat(output, localOutput);
        }
        writeUART(output, 1);
        return;
    }

    char cmd[10][128];
    int cmdParts;
    int index;

    char paramsCopy[256];
    strncpy(paramsCopy, params, sizeof(paramsCopy) - 1);
    paramsCopy[255] = '\0'; // Null-terminate the copy
    splitStringBySpaces(paramsCopy, cmd, &cmdParts);

    //splitStringBySpaces(params, cmd, &cmdParts);

    index = atoi(cmd[0]);

    if(index < 0 || index > 15) {
        throwError("invalidTickerRange");
        return;
    }

    if(strcmp(cmd[1], "clear") == 0) {// -ticker # clear
        tickers[index].initialDelay = 0;
        tickers[index].initialDelayCounter = 0;
        tickers[index].repeatDelay = 0;
        tickers[index].repeatDelayCounter = 0;
        tickers[index].repeatCount = 0;
        tickers[index].repeatCounter = 0;
        strcpy(tickers[index].payload, "");
        sprintf(output, "Ticker %d has been cleared.", index);
        writeUART(output, 1);
        return;
    }

    if(cmdParts < 2) { // -ticker #
        sprintf(output, "Ticker %d | Delay: %d Repeat: %d RepeatCount: %d Payload: %s", index, tickers[index].initialDelay, tickers[index].repeatDelay, tickers[index].repeatCount, tickers[index].payload);
        writeUART(output, 1);
        return;
    } else if(cmdParts < 6) {
        throwError("tickerMissingParams");
        return;
    }

    // -ticker # delay repeat repeatCount payload
    tickers[index].finished = 1;
    tickers[index].notInitialRun = 0;
    tickers[index].initialDelay = atoi(cmd[1]);
    tickers[index].repeatDelay = atoi(cmd[2]);
    tickers[index].repeatCount = atoi(cmd[3]);
    tickers[index].initialDelayCounter = 0;
    tickers[index].repeatDelayCounter = 0;
    tickers[index].repeatCounter = 0;


    int i;
    char payload[128] = "";
    for(i = 4; i < cmdParts; i++) {
        strcat(payload, cmd[i]);
        if(i + 1 != cmdParts) {
            strcat(payload, " ");
        }
    }

    strcpy(tickers[index].payload, payload);
    sprintf(output, "Ticker %d | Delay: %s Repeat: %s RepeatCount: %s Payload: %s", index, cmd[1], cmd[2], cmd[3], payload);
    writeUART(output, 1);
}

void timerCommand(char* params) {
    if(strlen(params) == 0) {
        char output[128];
        snprintf(output, sizeof(output), "Timer length is set to: %d micro seconds.", global.timer0.Length);

        writeUART(output, 1);
        return;
    } else if(strlen(params) > 1) {
        char output[128];
        uint32_t length = atoi(params);
        global.timer0.Length = length;

        if(length < 100) {
            throwError("invalidTimerRange");
            return;
        }

        Timer_setPeriod(global.timer0.Handle, Timer_PERIOD_US, length);

        snprintf(output, sizeof(output), "Timer length has been updated to: %d micro seconds.", length);

        writeUART(output, 1);

        return;
    } else {
        throwError("invalidTimerRange");
        return;
    }
}

void uartCommand(char* params) {
    char localOutput[320] = "";
    sprintf(localOutput, "%s\n", params);
    UART_write(global.uart7.Handle, localOutput, sizeof(localOutput));
    writeUART("", 1);
}
