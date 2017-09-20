#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int16_t output[62000];

typedef struct{
    char *name;
    int16_t memory;
} Label;

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
            printf("Multiple labels with same name");
    }

    labels[*labelSize] = label;
    (*labelSize)++;
}

void processInstruction(char *line, Label *labels, int labelSize, int *memory, int *i){
    char *inst = malloc(8*sizeof(char));
    int j;

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
                printf("Label not found");
        }
        else{
            output[(*memory)++] = 0x0009 | (r << 8) | 0x1000;

            char *num = malloc(8*sizeof(char));
            j = 0;

            while(line[*i] != ' ' || line[*i] != '#'){
                num[j] = line[*i];
                (*i)++;
                j++;
            }

            num[j] = '\0';

            output[(*memory)++] = (int16_t)strtol(num, NULL, 0);
        }
    }
    else if(!strcmp(inst, "ADD")){
    }
}

int main(int argc, char **argv){
    FILE *input;
    char *line;
    regex_t linePattern, labelPattern, instructionPattern;
    int i, lineNumber, memoryLocation, labelSize;
    Label labels[1024];

    if(regcomp(&linePattern, "(^([a-zA-Z_][a-zA-Z0-9_]*[:]){0,1} *(((HALT)|(ADD *r[0-9] *r[0-9])|(SUB *r[0-9] *r[0-9])|(MUL *r[0-9] *r[0-9])|(DIV *r[0-9] *r[0-9])|(OR *r[0-9] *r[0-9])|(AND *r[0-9] *r[0-9])|(NOT *r[0-9])|(LOAD *r[0-9] *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+)))|(SAVE *r[0-9] *\\[0x[0-9A-Fa-f]+\\])|(COPY *r[0-9] *r[0-9])|(TEST *r[0-9] *r[0-9])|(JUMP *(N|Z|U) *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+))))|(0x[0-9A-Za-z]+)){0,1}(#.*){0,1}$)", REG_EXTENDED))
        printf("Line RegEx error");
    if(regcomp(&labelPattern, "[a-zA-Z_][a-zA-Z0-9_]*[:]", REG_EXTENDED))
        printf("Label RegEx error");
    if(regcomp(&instructionPattern, "((HALT)|(ADD *r[0-9] *r[0-9])|(SUB *r[0-9] *r[0-9])|(MUL *r[0-9] *r[0-9])|(DIV *r[0-9] *r[0-9])|(OR *r[0-9] *r[0-9])|(AND *r[0-9] *r[0-9])|(NOT *r[0-9])|(LOAD *r[0-9] *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+)))|(SAVE *r[0-9] *\\[0x[0-9A-Fa-f]+\\])|(COPY *r[0-9] *r[0-9])|(TEST *r[0-9] *r[0-9])|(JUMP *(N|Z|U) *((\\[[a-zA-Z_][a-zA-Z0-9_]*\\])|(0x[0-9A-Fa-f]+))))", REG_EXTENDED))
        printf("Instruction RegEx error");

    if(argv[1] != NULL){
        input = fopen(argv[1], "r");
        if(!input)
            printf("Error opening file");

        lineNumber = 1;
        memoryLocation = 0;
        labelSize = 0;
        while(getLine(&line, input)){
            i = 0;
            if(match(line, &linePattern)){
                if(match(line, &labelPattern))
                    processLabel(line, labels, &labelSize, memoryLocation, &i);
            }
            else
                printf("Syntax error on line %d\n", lineNumber);

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
    }
    else
        printf("No file found");

    return 0;
}
