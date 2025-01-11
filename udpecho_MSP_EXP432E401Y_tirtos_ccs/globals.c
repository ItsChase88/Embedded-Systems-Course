#include <globals.h>

GLOBAL global = {
    .assignmentNumber = "12",
    .version = "1.11"
};

ERROR error[24] = {
    {.errorName = "commandNotFound", .errorDesc = "ERROR: Command not found.", .count = 0},
    {.errorName = "bufferOverflow", .errorDesc = "ERROR: Buffer overflow!", .count = 0},
    {.errorName = "helpCommandNotFound", .errorDesc = "ERROR: Unknown help command.", .count = 0},
    {.errorName = "memoryAddressOOR", .errorDesc = "ERROR: The memory address you tried to access is out of range!", .count = 0},
    {.errorName = "errorCommandNotFound", .errorDesc = "ERROR: Unknown error command.", .count = 0},
    {.errorName = "invalidGPIO", .errorDesc = "ERROR: Invalid GPIO command.", .count = 0},
    {.errorName = "invalidCallback", .errorDesc = "ERROR: Invalid callback index.", .count = 0},
    {.errorName = "invalidTimerRange", .errorDesc = "ERROR: Invalid timer range.", .count = 0},
    {.errorName = "invalidScriptRange", .errorDesc = "ERROR: Invalid script index range.", .count = 0},
    {.errorName = "scriptPayloadLoop", .errorDesc = "ERROR: Script payload can not cause infinite loop.", .count = 0},
    {.errorName = "invalidTickerRange", .errorDesc = "ERROR: Invalid ticker index range.", .count = 0},
    {.errorName = "tickerMissingParams", .errorDesc = "ERROR: Ticker command missing params.", .count = 0},
    {.errorName = "regMissingParams", .errorDesc = "ERROR: Register command missing params.", .count = 0},
    {.errorName = "regInvalidOp", .errorDesc = "ERROR: Invalid register operation.", .count = 0},
    {.errorName = "regInvalidIndex", .errorDesc = "ERROR: Invalid register index.", .count = 0},
    {.errorName = "regImmNotDest", .errorDesc = "ERROR: Destination register can not be an immediate value.", .count = 0},
    {.errorName = "ifMissingParams", .errorDesc = "ERROR: Conditional command missing params.", .count = 0},
    {.errorName = "ifInvalidParams", .errorDesc = "ERROR: Conditional command invalid params.", .count = 0},
    {.errorName = "invalidSineFreq", .errorDesc = "ERROR: Invalid sine frequency (FREQ<4000).", .count = 0},
    {.errorName = "nyquistViolation", .errorDesc = "ERROR: Nyquist violation.", .count = 0},
    {.errorName = "audioBuffer", .errorDesc = "ERROR: Audio buffer error.", .count = 0},
    {.errorName = "netudpMissingParams", .errorDesc = "ERROR: NetUDP command missing params.", .count = 0},
    {.errorName = "netudpError", .errorDesc = "ERROR: NetUDP error occurred.", .count = 0},
    {.errorName = "streamError", .errorDesc = "ERROR: Stream error occurred.", .count = 0}
};

int errorCount = 24;

CALLBACK callback[3] = {
    {.payload = "NULL", .count = 0, .counter = 0},
    {.payload = "NULL", .count = 0, .counter = 0},
    {.payload = "NULL", .count = 0, .counter = 0}
};

CALLBACK scripts[64];
TICKER tickers[16];
int32_t registers[32];

uint16_t sinTable[257] = {
    8192,
    8393,
    8594,
    8795,
    8995,
    9195,
    9394,
    9593,
    9790,
    9987,
    10182,
    10377,
    10570,
    10762,
    10952,
    11140,
    11327,
    11512,
    11695,
    11875,
    12054,
    12230,
    12404,
    12575,
    12743,
    12909,
    13072,
    13232,
    13389,
    13543,
    13693,
    13841,
    13985,
    14125,
    14262,
    14395,
    14525,
    14650,
    14772,
    14890,
    15003,
    15113,
    15219,
    15320,
    15417,
    15509,
    15597,
    15681,
    15760,
    15835,
    15905,
    15971,
    16031,
    16087,
    16138,
    16185,
    16227,
    16263,
    16295,
    16322,
    16345,
    16362,
    16374,
    16382,
    16383,
    16382,
    16374,
    16362,
    16345,
    16322,
    16295,
    16263,
    16227,
    16185,
    16138,
    16087,
    16031,
    15971,
    15905,
    15835,
    15760,
    15681,
    15597,
    15509,
    15417,
    15320,
    15219,
    15113,
    15003,
    14890,
    14772,
    14650,
    14525,
    14395,
    14262,
    14125,
    13985,
    13841,
    13693,
    13543,
    13389,
    13232,
    13072,
    12909,
    12743,
    12575,
    12404,
    12230,
    12054,
    11875,
    11695,
    11512,
    11327,
    11140,
    10952,
    10762,
    10570,
    10377,
    10182,
    9987,
    9790,
    9593,
    9394,
    9195,
    8995,
    8795,
    8594,
    8393,
    8192,
    7991,
    7790,
    7589,
    7389,
    7189,
    6990,
    6791,
    6594,
    6397,
    6202,
    6007,
    5814,
    5622,
    5432,
    5244,
    5057,
    4872,
    4689,
    4509,
    4330,
    4154,
    3980,
    3809,
    3641,
    3475,
    3312,
    3152,
    2995,
    2841,
    2691,
    2543,
    2399,
    2259,
    2122,
    1989,
    1859,
    1734,
    1612,
    1494,
    1381,
    1271,
    1165,
    1064,
    967,
    875,
    787,
    703,
    624,
    549,
    479,
    413,
    353,
    297,
    246,
    199,
    157,
    121,
    89,
    62,
    39,
    22,
    10,
    2,
    0,
    2,
    10,
    22,
    39,
    62,
    89,
    121,
    157,
    199,
    246,
    297,
    353,
    413,
    479,
    549,
    624,
    703,
    787,
    875,
    967,
    1064,
    1165,
    1271,
    1381,
    1494,
    1612,
    1734,
    1859,
    1989,
    2122,
    2259,
    2399,
    2543,
    2691,
    2841,
    2995,
    3152,
    3312,
    3475,
    3641,
    3809,
    3980,
    4154,
    4330,
    4509,
    4689,
    4872,
    5057,
    5244,
    5432,
    5622,
    5814,
    6007,
    6202,
    6397,
    6594,
    6791,
    6990,
    7189,
    7389,
    7589,
    7790,
    7991,
    8192
};

void writeUART(char* output, int newLine) {
    if(newLine == 0) { // No newline
        int len = (strlen(output)+1);
        char* localOutput = malloc(len);
        memset(localOutput, 0, len);
        sprintf(localOutput, "%s", output);
        //TODO: add localOutput to the queue
        UART_write(global.uart0.Handle, localOutput, strlen(localOutput));
        //enqueue(localOutput);
        free(localOutput);
    } else { // Yes newline
        int len = (strlen(output)+5);
        char* localOutput = malloc(len);
        memset(localOutput, 0, len);
        sprintf(localOutput, "\r\n%s\r\n", output);
        //TODO: add localOutput to the queue
        UART_write(global.uart0.Handle, localOutput, strlen(localOutput));
        //enqueue(localOutput);
        free(localOutput);
    }
}

int isNumber(char *s) {
    while (*s) {
        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}

void throwError(char* errorName) {
    int i;
    for(i = 0; i < errorCount; i++) {
        if(strcmp(errorName, error[i].errorName) == 0) {
            error[i].count++;
            writeUART(error[i].errorDesc, 1);
            return;
        }
    }
}

void splitStringBySpaces(char *str, char words[10][128], int *wordCount) {
    int i;
    for(i = 0; i < 10; i++) {
        strcpy(words[i], "");
    }

    char *token;
    *wordCount = 0;

    token = strtok(str, " ");

    while (token != NULL) {
        strcpy(words[*wordCount], token);
        (*wordCount)++;
        token = strtok(NULL, " ");
    }
}
