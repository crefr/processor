#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "assembler.h"
#include "../../comands.h"

const size_t MAXCMDLEN = 50;
void assembleRun(FILE * in_file, FILE * out_file)
{
    while (1){
        char cmd[MAXCMDLEN] = "";
        fscanf(in_file, "%s", cmd);

        if (strcmp(cmd, "push") == 0){
            int arg = 0;
            fscanf(in_file, "%d", &arg);
            fprintf(out_file, "%d %d\n", PUSH_CMD, arg);
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
}
