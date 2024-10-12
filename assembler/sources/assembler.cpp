#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "assembler.h"
#include "../../comands.h"
#include "logger.h"

const size_t MAXCMDLEN  = 50;

static void scanPushArgs(FILE * in_file, int ** ip);

size_t assembleRun(FILE * in_file, int * program)
{
    int * ip = program - 1;
    while (1){
        ip++;
        char cmd[MAXCMDLEN] = "";
        fscanf(in_file, "%s", cmd);

        if (strcmp(cmd, "push") == 0){
            scanPushArgs(in_file, &ip);
            continue;
        }
        if (strcmp(cmd, "pop") == 0){
            int pop_cmd_code = POP_CMD | REG_MASK;
            char reg_str[ARGMAXLEN + 1] = "";
            fscanf(in_file, "%s", reg_str);
            int reg_num = reg_str[1] - 'a' + 1;
            *(ip++) = pop_cmd_code;
            *ip     = reg_num;
            continue;
        }
        if (strcmp(cmd, "jmp") == 0){
            int jmp_address = 0;
            fscanf(in_file, "%d", &jmp_address);
            *(ip++) = JMP_CMD;
            *ip     = jmp_address;
            continue;
        }
        if (strcmp(cmd, "ja") == 0){
            int jmp_address = 0;
            fscanf(in_file, "%d", &jmp_address);
            *(ip++) = JA_CMD;
            *ip     = jmp_address;
            continue;
        }
        if (strcmp(cmd, "add") == 0){
            *ip = ADD_CMD;
            continue;
        }
        if (strcmp(cmd, "sub") == 0){
            *ip = SUB_CMD;
            continue;
        }
        if (strcmp(cmd, "mul") == 0){
            *ip = MUL_CMD;
            continue;
        }
        if (strcmp(cmd, "div") == 0){
            *ip = DIV_CMD;
            continue;
        }
        if (strcmp(cmd, "out") == 0){
            *ip = OUT_CMD;
            continue;
        }
        if (strcmp(cmd, "in") == 0){
            *ip = IN_CMD;
            continue;
        }
        if (strcmp(cmd, "hlt") == 0){
            *ip = HLT_CMD;
            break;
        }
        PRINTFANDLOG(LOG_RELEASE, "SYNTAX ERROR: %s\n", cmd);
    }
    return ip - program + 1;
}


static void scanPushArgs(FILE * in_file, int ** ip)
{
    int digit_arg = 0;
    int reg_arg   = 0;
    char reg_str[ARGMAXLEN + 1] = "";
    int push_cmd_code = PUSH_CMD;

    if (fscanf(in_file, "%d", &digit_arg) != 0){
        push_cmd_code |= DIG_MASK;
        *((*ip)++) = push_cmd_code;
        **ip       = digit_arg;
    }
    else{
        fscanf(in_file, " %s ", reg_str);
        push_cmd_code |= REG_MASK;
        reg_arg = toupper(reg_str[1]) - 'A' + 1;
        if (fscanf(in_file, "%d", &digit_arg) != 0){
            push_cmd_code |= DIG_MASK;
            *((*ip)++) = push_cmd_code;
            *((*ip)++) = reg_arg;
            **ip       = digit_arg;
        }
        else{
            *((*ip)++) = push_cmd_code;
            **ip       = reg_arg;
        }
    }
}

void progToText(FILE * out_file, int * program, size_t prog_size)
{
    fprintf(out_file, "%08zu\n", prog_size);
    for (size_t ip = 0; ip < prog_size; ip++){
        fprintf(out_file, "%d ", program[ip]);
    }
}
