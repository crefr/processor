#ifndef PROCESSOR_INCLUDED
#define PROCESSOR_INCLUDED

#include "mystack.h"
#include "../../comands.h"

typedef struct
{
    int * prog;
    size_t prog_size;
    stack_t * stk;
    int reg[REGNUM];
    int * ip;
} processor_t;

typedef enum {
    PROC_SUCCESS,
    PROC_PROG_ERROR
} proc_status_t;

void processorCtor(processor_t * proc, FILE * prog_file);
void processorRun(processor_t * proc);
void processorDtor(processor_t * proc);

void processorDump(processor_t * proc);

#endif
