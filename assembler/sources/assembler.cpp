#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include "assembler.h"
#include "../../comands.h"
#include "logger.h"

const size_t MAXCMDLEN = 50;
void assembleRun(FILE * in_file, FILE * out_file)
{
    fprintf(out_file, "%8d\n", 0);

    size_t cmdcount = 0;
    while (1){
        cmdcount++;
        char cmd[MAXCMDLEN] = "";
        fscanf(in_file, "%s", cmd);

        if (strcmp(cmd, "push") == 0){
            int digit_arg = 0;
            int reg_arg   = 0;
            char arg1_str[ARGMAXLEN + 1] = "";
            int push_cmd_code = PUSH_CMD;

            fscanf(in_file, " %s ", arg1_str);
            if (!isdigit(arg1_str[0]) && arg1_str[0] != '-'){
                push_cmd_code |= REG_MASK;
                switch(arg1_str[1]){
                    case 'a': case 'A':
                        reg_arg = RAX;
                        break;
                    case 'b': case 'B':
                        reg_arg = RBX;
                        break;
                    case 'c': case 'C':
                        reg_arg = RCX;
                        break;
                    case 'd': case 'D':
                        reg_arg = RDX;
                        break;
                    default:
                        PRINTFANDLOG(LOG_RELEASE, "invalid argument at word: %zu", cmdcount);
                        break;
                }
                if (fscanf(in_file, "%d", &digit_arg) != 0){
                    push_cmd_code |= DIG_MASK;
                    fprintf(out_file, "%d %d %d\n", push_cmd_code, reg_arg, digit_arg);
                    cmdcount += 2;
                }
                else{
                    fprintf(out_file, "%d %d\n", push_cmd_code, reg_arg);
                    cmdcount += 1;
                }
            }
            else{
                push_cmd_code |= DIG_MASK;
                fprintf(out_file, "%d %s\n", push_cmd_code, arg1_str);
                cmdcount += 1;
            }
            continue;
        }
        if (strcmp(cmd, "pop") == 0){
            int pop_cmd_code = POP_CMD | REG_MASK;
            char reg_str[ARGMAXLEN + 1] = "";
            fscanf(in_file, "%s", reg_str);
            int reg_num = reg_str[1] - 'a' + 1;
            fprintf(out_file, "%d %d\n", pop_cmd_code, reg_num);
            cmdcount++;
            continue;
        }
        if (strcmp(cmd, "add") == 0){
            fprintf(out_file, "%d\n", ADD_CMD);
            continue;
        }
        if (strcmp(cmd, "add") == 0){
            fprintf(out_file, "%d\n", ADD_CMD);
            continue;
        }
        if (strcmp(cmd, "sub") == 0){
            fprintf(out_file, "%d\n", SUB_CMD);
            continue;
        }
        if (strcmp(cmd, "out") == 0){
            fprintf(out_file, "%d\n", OUT_CMD);
            continue;
        }
        if (strcmp(cmd, "hlt") == 0){
            fprintf(out_file, "%d\n", HLT_CMD);
            break;
        }
        PRINTFANDLOG(LOG_RELEASE, "SYNTAX ERROR: %s", cmd);
    }
    fseek(out_file, 0, SEEK_SET);
    fprintf(out_file, "%08lu", cmdcount);
}
