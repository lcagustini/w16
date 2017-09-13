#include <stdlib.h>
#include <stdio.h>

void printRegisters(int16_t r[], int16_t sr){
    for(int i = 0; i < 10; i++)
        printf("r%d: %d\n", i, r[i]);
    printf("%d\n\n", sr);
}

void loadRAM(int16_t *ram){
    ram[0] = 0x0009 | 0x0000;
    ram[1] = 0x0008;

    ram[2] = 0x0009 | 0x0100;
    ram[3] = 0x0009;

    ram[4] = 0x0002 | 0x0000 | 0x1000;
    ram[5] = 0x0004 | 0x0000 | 0x1000;
    ram[6] = 0x000B | 0x0200 | 0x0000;

    ram[7] = 0x0001;

    ram[8] = 13;
    ram[9] = 39;
}

/*
 * 01 - HALT
 * 02 - ADD
 * 03 - SUB
 * 04 - MUL
 * 05 - DIV
 * 06 - OR
 * 07 - AND
 * 08 - NOT
 * 09 - LOAD
 * 0A - SAVE
 * 0B - COPY
 * */

int main(){
    int16_t *ram = malloc(62000);
    int16_t r[10] = {};
    int16_t sr;

    loadRAM(ram);

    int pc = 0;
    while(1){
        int16_t cur = ram[pc];
        int inst = cur & 0x00FF;
        int ir1 = (cur & 0x0F00) >> 8;
        int ir2 = (cur & 0xF000) >> 12;

        if(inst == 0x0001)
            break;
        else if(inst == 0x0002)
            r[ir1] = r[ir1] + r[ir2];
        else if(inst == 0x0003)
            r[ir1] = r[ir1] - r[ir2];
        else if(inst == 0x0004)
            r[ir1] = r[ir1] * r[ir2];
        else if(inst == 0x0005)
            r[ir1] = r[ir1] / r[ir2];
        else if(inst == 0x0006)
            r[ir1] = r[ir1] | r[ir2];
        else if(inst == 0x0007)
            r[ir1] = r[ir1] & r[ir2];
        else if(inst == 0x0008)
            r[ir1] = !r[ir1];
        else if(inst == 0x0009)
            r[ir1] = ram[ram[++pc]];
        else if(inst == 0x000A)
            ram[ram[++pc]] = r[ir1];
        else if(inst == 0x000B)
            r[ir1] = r[ir2];

        printRegisters(r);

        pc++;
    }

    return 0;
}
