#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "processor.h"
#include "mystack.h"
#include "../../comands.h"

const size_t MAXCMDLEN = 50;
const size_t MAXPROGLEN = 50000;
void processorRun(processor_t * proc)
{
    int * ip = proc->prog;
    stack_t stk = stackCtor(0);
    int quit = 0;
    while (ip < ip + proc->prog_size && quit != 1){
        switch (*ip){
        case PUSH_CMD:{
            int arg = 0;
            ip++;
            arg = *ip;
            stackPush(&stk, arg);
            break;
        }
        case ADD_CMD:{
            int a = stackPop(&stk);
            int b = stackPop(&stk);
            stackPush(&stk, a + b);
            break;
        }
        case SUB_CMD:{
            int a = stackPop(&stk);
            int b = stackPop(&stk);
            stackPush(&stk, b - a);
            break;
        }
        case OUT_CMD:{
            int out_elem = stackPop(&stk);
            printf("%d\n", out_elem);
            break;
        }
        case HLT_CMD:{
            quit = 1;
            break;
        }
        }
        ip++;
    }
    stackDtor(&stk);
}

void processorGetProg(processor_t * proc, FILE * prog_file)
{
    assert(proc      != NULL);
    assert(prog_file != NULL);
    fscanf(prog_file, "%zu", &(proc->prog_size));
    proc->prog = (int *)calloc(proc->prog_size, sizeof(int));

    for (size_t index = 0; index < proc->prog_size; index++){
        fscanf(prog_file, "%d", proc->prog + index);
    }
}

void processorDtor(processor_t * proc)
{
    assert(proc != NULL);
    free(proc->prog);
    proc->prog = NULL;
}
