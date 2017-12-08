#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define RAM_SIZE 30

void printRegisters(int16_t r[]){
    for(int i = 0; i < 10; i++)
        printf("r%d: %d\n", i, r[i]);
    printf("sr: %d\n", r[10]);
    printf("pc: %d\n", r[11]);
    printf("lr: %d\n", r[12]);
    printf("sp: %d\n\n", r[13]);
}

void loadRAM(char *path, int16_t *ram){
    FILE *input;
    int lineNumber = 0;

    if(path != NULL){
        input = fopen(path, "r");

        if(!input)
            printf("Error opening file");
        else{
            int16_t inst;
            while(!feof(input) && fscanf(input, "%4hx", &inst)){
                ram[lineNumber++] = inst;
                printf("%d - %x\n", lineNumber-1, inst);
            }
            fclose(input);
        }
    }
    else
        printf("No file found");

    /*
    ram[0] = 0x0009 | 0x0000 | 0x0000;
    ram[1] = 0x000A;

    ram[2] = 0x0009 | 0x0100 | 0x0000;
    ram[3] = 0x000B;

    ram[4] = 0x000D | 0x0000 | 0x1000;
    ram[5] = 0x0009;

    ram[6] = 0x0002 | 0x0000 | 0x1000;
    ram[7] = 0x0004 | 0x0000 | 0x1000;
    ram[8] = 0x000C | 0x0100 | 0x0000;

    ram[9] = 0x0001;

    ram[10] = 13;
    ram[11] = 39;
    */
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
 * 0C - TEST
 * 0D - JUMP
 * */

int main(int argc, char **argv){
    int16_t ram[RAM_SIZE] = {};
    int16_t r[14] = {}; /*r0-r9 + sr + pc + lr + sp*/

    loadRAM(argv[1], ram);

    while(1){
        int16_t cur = ram[r[11]];
        int16_t inst = cur & 0x00FF;
        int16_t ir1 = (cur & 0x0F00) >> 8;
        int16_t ir2 = (cur & 0xF000) >> 12;

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
        else if(inst == 0x0009){
            if(!ir2)
                r[ir1] = ram[ram[++r[11]]];
            else
                r[ir1] = ram[++r[11]];
        }
        else if(inst == 0x000A)
            ram[ram[++r[11]]] = r[ir1];
        else if(inst == 0x000B){
            r[ir1] = r[ir2];
        }
        else if(inst == 0x000C){
            if(r[ir1] - r[ir2] == 0) r[10] = 0x0001;
            else if(r[ir1] - r[ir2] < 0) r[10] = 0x0002;
            else r[10] = 0x0004;
        }
        else if(inst == 0x000D){
            r[11]++;
            if((r[10] & ir1) || (!ir1)){
                r[11] = ram[r[11]];
                r[11] -= 2;
            }
        }

        printRegisters(r);

        r[11]++;
    }

    return 0;
}
