#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "assembler.h"
#include "logger.h"

const size_t MAXPROGLEN = 10000;
int main()
{
    logStart("log.txt", LOG_DEBUG_PLUS);
    FILE * in_file = fopen("../program.asm", "r");
    assert(in_file != NULL);

    FILE * out_file = fopen("../program_code.txt", "w");

    int program[MAXPROGLEN] = {};
    size_t prog_len = assembleRun(in_file, program);
    progToText(out_file, program, prog_len);
    logExit();
    fclose(in_file);
    fclose(out_file);
    return 0;
}
