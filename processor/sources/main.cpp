#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "logger.h"

int main()
{
    logStart("spulog.txt", LOG_DEBUG);
    FILE * prog_file = fopen("program_code.txt", "r");

    processor_t proc = {};

    processorCtor(&proc, prog_file);
    processorRun (&proc);
    processorDtor(&proc);
    logExit();

    fclose(prog_file);
    return 0;
}
