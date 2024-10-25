#ifndef PROCESSOR_INCLUDED
#define PROCESSOR_INCLUDED

#include "mystack.h"
#include "comands.h"

/// @brief processor struct
typedef struct
{
    int * prog;
    size_t prog_size;
    stack_t * stk;
    stack_t * call_stk;
    int reg[REGNUM];
    int * RAM;
    size_t RAM_size;
    int * ip;
} processor_t;

typedef enum {
    PROC_SUCCESS,
    PROC_PROG_ERROR
} proc_status_t;

/// @brief  constructs processor struct
proc_status_t processorCtor(processor_t * proc, FILE * prog_file);

/// @brief  destructs processor struct
void processorDtor(processor_t * proc);

/// @brief  runs program from processor struct
void processorRun(processor_t * proc);

/// @brief  dums processor
void processorDump(processor_t * proc);

const size_t DRAW_WIDTH = 96;

#endif
