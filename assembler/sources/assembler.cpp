#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "assembler.h"
#include "../../comands.h"

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
            int arg = 0;
            fscanf(in_file, "%d", &arg);
            fprintf(out_file, "%d %d\n", PUSH_CMD, arg);
            cmdcount++;
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
    }
    fseek(out_file, 0, SEEK_SET);
    fprintf(out_file, "%08lu", cmdcount);
}
