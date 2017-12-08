#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#define RAM_SIZE 30

int16_t output[RAM_SIZE];

typedef struct{
    char *name;
    int16_t memory;
} Label;

void printLabels(Label labels[], int labelSize){
    printf("Labels: \n");
    for(int i = 0; i < labelSize; i++)
        printf("%s: %d\n", labels[i].name, labels[i].memory);
}

void printOutput(char *path){
    if(!path){
        printf("Output: \n");
        for(int i = 0; i < RAM_SIZE; i++)
            printf("%2d: 0x%04x\n", i, output[i]);
    }
    else{
        FILE *out = fopen(path, "w");

        for(int i = 0; i < RAM_SIZE; i++)
            fprintf(out, "%04x", output[i]);

        fclose(out);
    }
}

int match(char *string, regex_t *pattern){
    if(!regexec(pattern, string, 0, (regmatch_t*) NULL, 0))
        return 1;
    return 0;
}

int getLine(char **line, FILE *input){
    int i, cur;

    *line = malloc(128*sizeof(char));

    i = 0;
    while((cur = fgetc(input)) != '\n' && cur != EOF)
        (*line)[i++] = cur;

    (*line)[i] = '\0';

    if(cur == EOF)
        return 0;
    return 1;
}

void processLabel(char *line, Label *labels, int *labelSize, int memoryLocation, int *i){
    char *name = malloc(64*sizeof(char));
    int j = 0;

    while(line[*i] != ':'){
        name[j] = line[*i];
        (*i)++;
        j++;
    }

    name[j] = '\0';

    Label label = {.name = name, .memory = memoryLocation};

    for(j = 0; j < *labelSize; j++){
        if(!strcmp(label.name, labels[j].name))
            printf("Multiple labels with same name\n");
    }

    char *num = malloc(8*sizeof(char));
    j = 0;

    (*i)++;

    while(line[(*i)++] == ' ');
    (*i)--;

    while(line[*i] != ' ' && line[*i] != '#' && line[*i] != '\0'){
        num[j] = line[*i];
        (*i)++;
        j++;
    }

    num[j] = '\0';


    if(num[0]){
        output[memoryLocation] = (int16_t)strtol(num, NULL, 0);
    }

    labels[*labelSize] = label;
    (*labelSize)++;
}

void processInstruction(char *line, Label *labels, int labelSize, int *memory, int *i){
    char *inst = malloc(8*sizeof(char));
    int j = 0;

    while(line[(*i)++] == ' ');
    (*i)--;

    while(line[*i] != ' ' && line[*i] != '\0'){
        inst[j] = line[*i];
        (*i)++;
        j++;
    }

    inst[j] = '\0';

    if(!strcmp(inst, "LOAD")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int r = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] == ' ');
        (*i)--;
        if(line[*i] == '['){
            (*i)++;
            output[(*memory)++] = 0x0009 | (r << 8);
            char *rot = malloc(64*sizeof(char));
            j = 0;

            while(line[*i] != ']'){
                rot[j] = line[*i];
                (*i)++;
                j++;
            }

            rot[j] = '\0';

            for(j = 0; j < labelSize; j++){
                if(!strcmp(labels[j].name, rot)){
                    output[(*memory)++] = labels[j].memory;
                    break;
                }
            }
            if(j == labelSize)
                printf("Label not found\n");
        }
        else{
            output[(*memory)++] = 0x0009 | (r << 8) | 0x1000;

            char *num = malloc(8*sizeof(char));
            j = 0;

            while(line[*i] != ' ' && line[*i] != '#' && line[*i] != '\0'){
                num[j] = line[*i];
                (*i)++;
                j++;
            }

            num[j] = '\0';

            output[(*memory)++] = (int16_t)strtol(num, NULL, 0);
        }
    }
    else if(!strcmp(inst, "SAVE")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int r = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] == ' ');

        output[(*memory)++] = 0x000A | (r << 8);
        char *rot = malloc(64*sizeof(char));
        j = 0;

        while(line[*i] != ']'){
            rot[j] = line[*i];
            (*i)++;
            j++;
        }

        rot[j] = '\0';

        for(j = 0; j < labelSize; j++){
            if(!strcmp(labels[j].name, rot)){
                output[(*memory)++] = labels[j].memory;
                break;
            }
        }
        if(j == labelSize)
            printf("Label not found\n");
    }
    else if(!strcmp(inst, "ADD")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0002 | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "SUB")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0003 | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "MUL")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0004 | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "DIV")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0005 | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "OR")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0006 | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "AND")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0007 | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "NOT")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x0008 | (r << 8);
    }
    else if(!strcmp(inst, "COPY")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] == ' ');
        (*i)--;

        if(line[*i] == 'r'){
            int16_t r2 = line[++(*i)] - '0';
            (*i)++;

            output[(*memory)++] = 0x000B | (r1 << 8) | (r2 << 12);
        }
        else{
            char *num = malloc(8*sizeof(char));
            j = 0;

            while(line[*i] != ' ' && line[*i] != '#' && line[*i] != '\0'){
                num[j] = line[*i];
                (*i)++;
                j++;
            }

            num[j] = '\0';

            int8_t r2 = (int8_t)strtol(num, NULL, 0);

            output[(*memory)++] = 0x000B | (r1 << 8) | (r2 << 12);
        }
    }
    else if(!strcmp(inst, "TEST")){
        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r1 = line[++(*i)] - '0';
        (*i)++;

        while(line[(*i)++] != 'r');
        (*i)--;
        int16_t r2 = line[++(*i)] - '0';
        (*i)++;

        output[(*memory)++] = 0x000C | (r1 << 8) | (r2 << 12);
    }
    else if(!strcmp(inst, "POP")){
        while(line[(*i)++] == ' ');
        (*i)--;

        if(line[*i] == 'r'){
            int s = line[++(*i)] - '0';
            if(line[++(*i)] == '-'){
                int f = line[*i+2] - '0';
                for(int j = s; j <= f; j++){
                    char reg[1] = {j+'0'};
                    char inst[80] = "LOAD r";
                    strcat(inst, reg);
                    strcat(inst, " [r13]");

                    *i = 0;
                    processInstruction("SUB r13 1", labels, labelSize, memory, i);
                    *i = 0;
                    processInstruction(inst, labels, labelSize, memory, i);
                }
            }
            else{
                
            }
        }
        else{
            if(line[*i] == 'l'){
            }
            else if(line[*i] == 'p'){
            }
            else{
                (*i)++;
                if(line[*i] == 'r'){
                }
                else{
                }
            }
        }
    }
    else if(!strcmp(inst, "PUSH")){
    }
    else if(!strcmp(inst, "JUMP")){
        int r = 0;

        while(line[(*i)++] == ' ');
        (*i)--;

        if(line[*i] == 'L'){
            r = 0x0002;
            (*i) += 2;
        }
        else if(line[*i] == 'E'){
            r = 0x0001;
            (*i) += 2;
        }
        else if(line[*i] == 'G'){
            r = 0x0004;
            (*i) += 2;
        }
        else
            r = 0x0000;

        while(line[(*i)++] == ' ');
        (*i)--;

        output[(*memory)++] = 0x000D | (r << 8);
        if(line[*i] == '['){
            (*i)++;
            char *rot = malloc(64*sizeof(char));
            j = 0;

            while(line[*i] != ']'){
                rot[j] = line[*i];
                (*i)++;
                j++;
            }

            rot[j] = '\0';

            for(j = 0; j < labelSize; j++){
                if(!strcmp(labels[j].name, rot)){
                    output[(*memory)++] = labels[j].memory;
                    break;
                }
            }
            if(j == labelSize)
                printf("Label not found\n");
        }
        else{
            char *num = malloc(8*sizeof(char));
            j = 0;

            while(line[*i] != ' ' && line[*i] != '#' && line[*i] != '\0'){
                num[j] = line[*i];
                (*i)++;
                j++;
            }

            num[j] = '\0';

            output[(*memory)++] = (int16_t)strtol(num, NULL, 0);
        }
    }
    else if(!strcmp(inst, "HALT")){
        output[(*memory)++] = 1;
    }
}

int main(int argc, char **argv){
    FILE *input;
    char *line;
    regex_t linePattern, labelPattern, instructionPattern, memoryPattern;
    int i, lineNumber, memoryLocation, labelSize;
    Label labels[1024];

    if(regcomp(&linePattern, "(^([a-zA-Z_][a-zA-Z0-9_]*[:])? *(((HALT)|(PUSH *((r[0-9](-r[0-9])?)|(lr)|(sp)|(sr)|(pc)))|(POP *((r[0-9](-r[0-9])?)|(lr)|(sp)|(sr)|(pc)))|(ADD *r[0-9] *r[0-9])|(SUB *r[0-9] *r[0-9])|(MUL *r[0-9] *r[0-9])|(DIV *r[0-9] *r[0-9])|(OR *r[0-9] *r[0-9])|(AND *r[0-9] *r[0-9])|(NOT *r[0-9])|(LOAD *r[0-9] *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+)))|(SAVE *r[0-9] *\\[0x[0-9A-Fa-f]+\\])|(COPY *r[0-9] *r[0-9])|(TEST *r[0-9] *r[0-9])|(JUMP *(LT|EQ|GT)? *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+))))|(0x[0-9A-Za-z]+))?(#.*)?$)", REG_EXTENDED))
        printf("Line RegEx error\n");
    if(regcomp(&labelPattern, "[a-zA-Z_][a-zA-Z0-9_]*[:]", REG_EXTENDED))
        printf("Label RegEx error\n");
    if(regcomp(&instructionPattern, "((HALT)|(PUSH *((r[0-9](-r[0-9])?)|(lr)|(sp)|(sr)|(pc)))|(POP *((r[0-9](-r[0-9])?)|(lr)|(sp)|(sr)|(pc)))|(ADD *r[0-9] *r[0-9])|(SUB *r[0-9] *r[0-9])|(MUL *r[0-9] *r[0-9])|(DIV *r[0-9] *r[0-9])|(OR *r[0-9] *r[0-9])|(AND *r[0-9] *r[0-9])|(NOT *r[0-9])|(LOAD *r[0-9] *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+)))|(SAVE *r[0-9] *\\[0x[0-9A-Fa-f]+\\])|(COPY *r[0-9] *r[0-9])|(TEST *r[0-9] *r[0-9])|(JUMP *(LT|EQ|GT)? *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+))))", REG_EXTENDED))
        printf("Instruction RegEx error\n");
    if(regcomp(&memoryPattern, "(LOAD|SAVE|JUMP)", REG_EXTENDED))
        printf("Memory RegEx error\n");

    if(argv[1]){
        input = fopen(argv[1], "r");
        if(!input)
            printf("Error opening file\n");
        else{
            lineNumber = 1;
            memoryLocation = 0;
            labelSize = 0;
            while(getLine(&line, input)){
                i = 0;
                if(match(line, &linePattern)){
                    if(match(line, &labelPattern))
                        processLabel(line, labels, &labelSize, memoryLocation, &i);
                }
                else{
                    printf("Syntax error on line %d\n", lineNumber);
                    return 1;
                }

                if(match(line, &memoryPattern))
                    memoryLocation++;

                if(line[0] != '\0')
                    memoryLocation++;
                lineNumber++;
            }

            fseek(input, 0, SEEK_SET);
            lineNumber = 1;
            memoryLocation = 0;
            while(getLine(&line, input)){
                i = 0;
                if(match(line, &instructionPattern))
                    processInstruction(line, labels, labelSize, &memoryLocation, &i);
                lineNumber++;
            }

            printLabels(labels, labelSize);
            printOutput(argv[2]);
        }
    }
    else
        printf("No file found\n");

    return 0;
}
