#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "assembler.h"
#include "../../comands.h"
#include "logger.h"

const size_t MAXCMDLEN  = 50;

static void scanPushArgs(program_t * prog);

size_t assembleRun(program_t * prog)
{
    while (1){
        prog->ip++;
        char cmd[MAXCMDLEN] = "";
        fscanf(prog->in_file, "%s", cmd);

        if (strcmp(cmd, "push") == 0){
            scanPushArgs(prog);
            continue;
        }
        if (strcmp(cmd, "pop") == 0){
            int pop_cmd_code = POP_CMD | REG_MASK;
            char reg_str[ARGMAXLEN + 1] = "";
            fscanf(prog->in_file, "%s", reg_str);
            int reg_num = reg_str[1] - 'a' + 1;
            *(prog->ip++) = pop_cmd_code;
            *prog->ip     = reg_num;
            continue;
        }
        if (strcmp(cmd, "jmp") == 0){
            int jmp_address = 0;
            fscanf(prog->in_file, "%d", &jmp_address);
            *(prog->ip++) = JMP_CMD;
            *prog->ip     = jmp_address;
            continue;
        }
        if (strcmp(cmd, "ja") == 0){
            int jmp_address = 0;
            fscanf(prog->in_file, "%d", &jmp_address);
            *(prog->ip++) = JA_CMD;
            *prog->ip     = jmp_address;
            continue;
        }
        if (strcmp(cmd, "add") == 0){
            *prog->ip = ADD_CMD;
            continue;
        }
        if (strcmp(cmd, "sub") == 0){
            *prog->ip = SUB_CMD;
            continue;
        }
        if (strcmp(cmd, "mul") == 0){
            *prog->ip = MUL_CMD;
            continue;
        }
        if (strcmp(cmd, "div") == 0){
            *prog->ip = DIV_CMD;
            continue;
        }
        if (strcmp(cmd, "out") == 0){
            *prog->ip = OUT_CMD;
            continue;
        }
        if (strcmp(cmd, "in") == 0){
            *prog->ip = IN_CMD;
            continue;
        }
        if (strcmp(cmd, "hlt") == 0){
            *prog->ip = HLT_CMD;
            break;
        }
        PRINTFANDLOG(LOG_RELEASE, "SYNTAX ERROR: %s\n", cmd);
    }
    prog->size = (size_t)(prog->ip - prog->program + 1);
    return prog->size;
}


static void scanPushArgs(program_t * prog)
{
    int digit_arg = 0;
    int reg_arg   = 0;
    char reg_str[ARGMAXLEN + 1] = "";
    int push_cmd_code = PUSH_CMD;

    if (fscanf(prog->in_file, "%d", &digit_arg) != 0){
        push_cmd_code |= DIG_MASK;
        *(prog->ip++) = push_cmd_code;
        *prog->ip     = digit_arg;
    }
    else{
        fscanf(prog->in_file, " %s ", reg_str);
        push_cmd_code |= REG_MASK;
        reg_arg = toupper(reg_str[1]) - 'A' + 1;
        if (fscanf(prog->in_file, "%d", &digit_arg) != 0){
            push_cmd_code |= DIG_MASK;
            *(prog->ip++) = push_cmd_code;
            *(prog->ip++) = reg_arg;
            *prog->ip     = digit_arg;
        }
        else{
            *(prog->ip++) = push_cmd_code;
            *prog->ip     = reg_arg;
        }
    }
}


static void printSignature(FILE * out_file, size_t prog_size)
{
    return;
}
void progToText(program_t * prog)
{
    fprintf(prog->out_text_file, "%08zu\n", prog->size);
    for (size_t ip = 0; ip < prog->size; ip++){
        fprintf(prog->out_text_file, "%d ", prog->program[ip]);
    }
}

void progToCode(program_t * prog)
{
    fwrite(&prog->size, sizeof(size_t), 1, prog->out_file);
    fwrite(prog->program, sizeof(int), prog->size, prog->out_file);
}

program_t progCtor(int * program, FILE * in_file, FILE * out_file, FILE * out_text_file)
{
    program_t prog = {};
    prog.program = program;
    prog.ip = program - 1;
    prog.in_file  = in_file;
    prog.out_file = out_file;
    prog.out_text_file = out_text_file;
    return prog;
}
