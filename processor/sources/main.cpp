#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "logger.h"

int main()
{
    logStart("spulog.txt", LOG_RELEASE);
    //logCancelBuffer();

    FILE * prog_file = fopen("program_code.txt", "r");

    processor_t proc = {};

    if (processorCtor(&proc, prog_file) != PROC_SUCCESS){
        processorDtor(&proc);
        return 1;
    }
    processorRun (&proc);
    processorDtor(&proc);
    logExit();

    fclose(prog_file);
    return 0;
}
