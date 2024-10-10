#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "logger.h"

int main()
{
    logStart("log.txt", LOG_DEBUG_PLUS);
    FILE * prog_file = fopen("../program_code.txt", "r");

    processor_t proc = {};

    processorGetProg(&proc, prog_file);
    processorRun(&proc);
    logExit();

    fclose(prog_file);
    return 0;
}
