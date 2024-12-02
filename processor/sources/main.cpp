#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "logger.h"

int main(int argc, const char ** argv)
{
    logStart("spulog.txt", LOG_DEBUG);
    logCancelBuffer();

    FILE * prog_file = fopen((argc > 1) ? argv[1] : "program.bin", "rb");
    if (prog_file == NULL){
        fprintf(stderr, "Program file can`t be opened\n");
        return 1;
    }

    processor_t proc = {};
    if (processorCtor(&proc, prog_file) != PROC_SUCCESS){
        fprintf(stderr, "Processor can`t be constructed\n");
        processorDtor(&proc);
        return 1;
    }
    processorRun (&proc);
    processorDtor(&proc);
    logExit();

    fclose(prog_file);
    return 0;
}
