#include <globals.h>

void MOV(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] = src;
    } else {
        registers[dest] = registers[src];
    }
}

void XCHG(int reg1, int reg2) {
    int32_t temp = registers[reg1];
    registers[reg1] = registers[reg2];
    registers[reg2] = temp;
}

void INC(int reg) {
    registers[reg]++;
}

void DEC(int reg) {
    registers[reg]--;
}

void ADD(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] = registers[dest] + src;
    } else {
        registers[dest] = registers[dest] + registers[src];
    }
}

void SUB(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] = registers[dest] - src;
    } else {
        registers[dest] = registers[dest] - registers[src];
    }
}

void NEG(int reg) {
    registers[reg] = -registers[reg];
}

void NOT(int reg) {
    registers[reg] = ~registers[reg];
}

void AND(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] &= src;
    } else {
        registers[dest] &= registers[src];
    }
}

void IOR(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] |= src;
    } else {
        registers[dest] |= registers[src];
    }
}

void XOR(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] ^= src;
    } else {
        registers[dest] ^= registers[src];
    }
}

void MUL(int dest, int src, int isImm) {
    if(isImm) {
        registers[dest] *= src;
    } else {
        registers[dest] *= registers[src];
    }
}

void DIV(int dest, int src, int isImm) {
    int divisor;
    if(isImm) {
        divisor = src;
    } else {
        divisor = registers[src];
    }
    if (divisor != 0) {
        registers[dest] /= divisor;
    } else {
        //printf("Error: Division by zero!\n");
    }
}

void REM(int dest, int src, int isImm) {
    int divisor;
    if(isImm) {
        divisor = src;
    } else {
        divisor = registers[src];
    }
    if (divisor != 0) {
        registers[dest] %= divisor;
    } else {
        //printf("Error: Division by zero!\n");
    }
}

void MAX(int dest, int src, int isImm) {
    int value;
    if(isImm) {
        value = src;
    } else {
        value = registers[src];
    }
    if (value > registers[dest]) {
        registers[dest] = value;
    }
}

void MIN(int dest, int src, int isImm) {
    int value;
    if(isImm) {
        value = src;
    } else {
        value = registers[src];
    }
    if (value > registers[dest]) {
        registers[dest] = value;
    }
}
