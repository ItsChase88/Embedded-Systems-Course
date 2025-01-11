#include <globals.h>

//Function to handle all commands and their outputs
void commandHandler(char* cmdFull) {
    char params[128] = "";
    char cmd[128] = "";

    //Make sure we have a valid command.
    if(strlen(cmdFull)<1) {
        writeUART("", 1);
        return;
    }

    //Check if there is any spaces and get the position of the first space.
    const char* spacePos = strchr(cmdFull, ' ');

    //Get the first half of the string which is our command
    if (spacePos != NULL) {
        //Get the params part of the string.
        strcpy(params, spacePos + 1);

        //Get the command part of the string.
        strcpy(cmd, cmdFull);
        int fullLen = strlen(cmdFull);
        int paramLen = strlen(params);
        int cmdLen = fullLen - paramLen - 1;
        int i;

        for(i = 0; i < fullLen-1; i++) {
            if(i > cmdLen-1) {
                cmd[i] = 0;
            }
        }
    } else {
        strcpy(cmd, cmdFull);
    }

    //Check the command typed.
    if(strcmp(cmd, "-about") == 0) {
        aboutCommand();
    } else if(strcmp(cmd, "-audio") == 0) {
        audioCommand(params);
    } else if(strcmp(cmd, "-callback") == 0) {
        callbackCommand(params);
    } else if(strcmp(cmd, "-clear") == 0) {
        writeUART("\033[2J\033[H\r\nScreen has been cleared!", 1);
    } else if(strcmp(cmd, "-error") == 0) {
        errorCommand(params);
    } else if(strcmp(cmd, "-gpio") == 0) {
        gpioCommand(params);
    } else if(strcmp(cmd, "-help") == 0) {
        helpCommand(params);
    } else if(strcmp(cmd, "-if") == 0) {
        ifCommand(params);
    } else if(strcmp(cmd, "-memr") == 0) {
        memrCommand(params);
    } else if(strcmp(cmd, "-netudp") == 0) {
        netudpCommand(params);
    } else if(strcmp(cmd, "-print") == 0) {
        printCommand(params);
    } else if(strcmp(cmd, "-reg") == 0) {
        regCommand(params);
    } else if(strcmp(cmd, "-rem") == 0) {
        return;
    } else if(strcmp(cmd, "-script") == 0) {
        scriptCommand(params);
    } else if(strcmp(cmd, "-sine") == 0) {
        sineCommand(params);
    } else if(strcmp(cmd, "-stream") == 0) {
        streamCommand(params);
    } else if(strcmp(cmd, "-ticker") == 0) {
        tickerCommand(params);
    } else if(strcmp(cmd, "-timer") == 0) {
        timerCommand(params);
    } else if(strcmp(cmd, "-uart") == 0) {
        uartCommand(params);
    } else {
        throwError("commandNotFound");
    }

    return;
}

//Function to interact with the uart and parse I/O
void parser() {
    char input;
    char inputString[128] = "";

    writeUART("Welcome! Please wait while we try to connect to the internet...", 1);

    //Infinite while loop to allow parser to run forever.
    while (1) {
        UART_read(global.uart0.Handle, &input, 1);

        if(input == 13) {
            //Run command and get output
            //commandHandler(inputString);
            enqueue(inputString);

            strcpy(inputString, "");

        } else if(input == 127) {
            //remove the last added char in string
            if(strlen(inputString) > 0) {
                inputString[strlen(inputString)-1] = 0;

                char modifiedInput[2] = "";
                snprintf(modifiedInput, 2, "%s", &input);
                writeUART(modifiedInput, 0);
            }
        } else {
            //Check to make sure the user doesn't type more than 128 chars
            if(strlen(inputString)+1 != 128) {
                //Add what the user is typing to our inputString
                strncat(inputString, &input, 1);

                //Force our input to be the char + null term only. Otherwise undefined behavior happens
                char modifiedInput[2] = "";
                snprintf(modifiedInput, 2, "%s", &input);
                writeUART(modifiedInput, 0);
            } else {
                //User typed more than 128 chars, so reset the line and inputString
                strcpy(inputString, "");
                //writeUART("\r\n", 0);
                throwError("bufferOverflow");
                //writeUART("\r\n", 0);
            }
        }
        input = 0;
    }
}
