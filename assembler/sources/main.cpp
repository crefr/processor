#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "assembler.h"
#include "logger.h"

int main()
{
    logStart("log.txt", LOG_DEBUG_PLUS);
    FILE * in_file = fopen("../program.asm", "r");
    assert(in_file != NULL);

    FILE * out_file = fopen("../program_code.txt", "w");

    assembleRun(in_file, out_file);
    logExit();
    return 0;
}
