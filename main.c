#include <stdlib.h>
#include <stdio.h>

void printRegisters(int16_t r[], int16_t sr, int16_t pc){
    for(int i = 0; i < 10; i++)
        printf("r%d: %d\n", i, r[i]);
    printf("sr: %d\n", sr);
    printf("pc: %d\n\n", pc);
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
            while(!feof(input) && fscanf(input, "%4hx", &inst))
                ram[lineNumber++] = inst;
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
    int16_t *ram = malloc(62000);
    int16_t r[10] = {};
    int16_t sr = 0;

    loadRAM(argv[1], ram);

    int16_t pc = 0;
    while(1){
        int16_t cur = ram[pc];
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
                r[ir1] = ram[ram[++pc]];
            else
                r[ir1] = ram[++pc];
        }
        else if(inst == 0x000A)
            ram[ram[++pc]] = r[ir1];
        else if(inst == 0x000B){
            if(ir2 > 9)
                r[ir1] = ir2;
            else
                r[ir1] = r[ir2];
        }
        else if(inst == 0x000C){
            if(r[ir1] - r[ir2] == 0) sr = 0x0001;
            else if(r[ir1] - r[ir2] < 0) sr = 0x0002;
            else sr = 0x0004;
        }
        else if(inst == 0x000D){
            pc++;
            if((sr & ir1) || (!ir1)){
                if(!ir2)
                    pc = ram[ram[pc]];
                else
                    pc = ram[pc];
            }
            pc--;
        }

        printRegisters(r, sr, pc);

        pc++;
    }

    return 0;
}
