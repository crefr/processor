#ifndef PROCESSOR_INCLUDED
#define PROCESSOR_INCLUDED

typedef struct
{
    int * prog;
    size_t prog_size;
} processor_t;

void processorGetProg(processor_t * proc, FILE * prog_file);
void processorRun(processor_t * proc);
void processorDtor(processor_t * proc);

#endif
