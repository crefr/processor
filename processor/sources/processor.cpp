#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "logger.h"
#include "processor.h"
#include "mystack.h"
#include "comands.h"

const size_t MAXCMDLEN = 50;
const size_t MAXPROGLEN = 50000;

static int procGetArg(processor_t * proc);
static void calcTwoArgs(processor_t * proc);
static void condJump(processor_t * proc);
void processorRun(processor_t * proc)
{
    int quit = 0;
    while (proc->ip < proc->ip + proc->prog_size && quit != 1){
        processorDump(proc);
        switch ((*(proc->ip)) & CMDNUM_MASK){
            case PUSH_CMD:{
                stackPush(proc->stk, procGetArg(proc));
                proc->ip++;
                break;
            }
            case POP_CMD:{
                proc->ip++;
                proc->reg[*(proc->ip)] = stackPop(proc->stk);
                proc->ip++;
                break;
            }
            case ADD_CMD: case SUB_CMD: case MUL_CMD: case DIV_CMD:{
                calcTwoArgs(proc);
                proc->ip++;
                break;
            }
            case JMP_CMD:{
                proc->ip = proc->prog + *(proc->ip + 1);
                break;
            }
            case JA_CMD: case JB_CMD: case JAE_CMD: case JBE_CMD:{
                condJump(proc);
                break;
            }
            case CALL_CMD:{
                stackPush(proc->call_stk, (int)(proc->ip + 2 - proc->prog));
                proc->ip = proc->prog + *(proc->ip + 1);
                break;
            }
            case RET_CMD:{
                proc->ip = proc->prog + stackPop(proc->call_stk);
                break;
            }
            case OUT_CMD:{
                int out_elem = stackPop(proc->stk);
                printf("%d\n", out_elem);
                proc->ip++;
                break;
            }
            case IN_CMD:{
                int in_elem = 0;
                scanf("%d", &in_elem);
                stackPush(proc->stk, in_elem);
                proc->ip++;
                break;
            }
            case HLT_CMD:{
                quit = 1;
                proc->ip++;
                break;
            }
            default:
                quit = 1;
                PRINTFANDLOG(LOG_RELEASE, "invalid instruction: %02X ", (unsigned int) *(proc->ip));
                break;
        }
    }
}
static void calcTwoArgs(processor_t * proc)
{
    assert(proc);
    int a = stackPop(proc->stk);
    int b = stackPop(proc->stk);
    int ans = 0;
    switch((*(proc->ip)) & CMDNUM_MASK){
        case ADD_CMD:{
            ans = a + b;
            break;
        }
        case SUB_CMD:{
            ans = b - a;
            break;
        }
        case MUL_CMD:{
            ans = a * b;
            break;
        }
        case DIV_CMD:{
            ans = b / a;
            break;
        }
        default:
            PRINTFANDLOG(LOG_RELEASE, "calcTwoArgs: wrong command\n");
            break;
    }
    stackPush(proc->stk, ans);
}


static proc_status_t processorGetProgFromCode(processor_t * proc, FILE * prog_file);
void processorCtor(processor_t * proc, FILE * prog_file)
{
    assert(proc      != NULL);
    assert(prog_file != NULL);

    //making stack
    proc->stk = (stack_t *)calloc(1, sizeof(stack_t));
    *(proc->stk) = stackCtor(0);

    //making call stack for functions
    proc->call_stk = (stack_t *)calloc(1, sizeof(stack_t));
    *(proc->call_stk) = stackCtor(0);

    //getting the program
    processorGetProgFromCode(proc, prog_file);

    //initializing ip
    proc->ip = proc->prog;
}

static bool checkSignature(FILE * prog_file);
static size_t getCodeSize(FILE * prog_file);
static proc_status_t processorGetProgFromCode(processor_t * proc, FILE * prog_file)
{
    if (!checkSignature(prog_file)){
        PRINTFANDLOG(LOG_RELEASE, "incorrect signature\n");
        logExit();
        return PROC_PROG_ERROR;
    }
    proc->prog_size = getCodeSize(prog_file);
    proc->prog = (int *)calloc(proc->prog_size, sizeof(int));
    fread(proc->prog, sizeof(int), proc->prog_size, prog_file);
    return PROC_SUCCESS;
}

static bool checkSignature(FILE * prog_file)
{
    char sign[sizeof(SIGNATURE)] = "";
    fread(sign, sizeof(char), sizeof(SIGNATURE), prog_file);

    if (strcmp(sign, SIGNATURE) == 0)
        return true;
    return false;
}

static size_t getCodeSize(FILE * prog_file)
{
    size_t size = 0;
    fread(&size, sizeof(size_t), 1, prog_file);
    return size;
}

void processorDtor(processor_t * proc)
{
    assert(proc != NULL);
    free(proc->prog);
    proc->prog = NULL;

    stackDtor(proc->call_stk);
    free(proc->call_stk);

    stackDtor(proc->stk);
    free(proc->stk);

    proc->stk = NULL;
}

static int procGetArg(processor_t * proc)
{
    int result = 0;
    int cmd = *(proc->ip);
    if (cmd & REG_MASK){
        proc->ip++;
        result += proc->reg[*(proc->ip)];
    }
    if (cmd & DIG_MASK){
        proc->ip++;
        result += *(proc->ip);
    }
    return result;
}

static void condJump(processor_t * proc)
{
    int a = stackPop(proc->stk);
    int b = stackPop(proc->stk);
    bool condition = false;

    int op_code = *(proc->ip) & CMDNUM_MASK;
    switch(op_code){
        case JA_CMD:  condition = a >  b; break;
        case JB_CMD:  condition = a <  b; break;
        case JAE_CMD: condition = a >= b; break;
        case JBE_CMD: condition = a <= b; break;
        default:
            logPrint(LOG_DEBUG, "invalid cmd for condJump: %d\n", op_code);
            break;
    }
    if (condition){
        proc->ip = proc->prog + *(proc->ip + 1);
        return;
    }
    else
        proc->ip += 2;
}

void processorDump(processor_t * proc)
{
    logPrint(LOG_DEBUG, "\n-------------PROCESSOR_DUMP-------------\n");
    logPrint(LOG_DEBUG, "processor at %p\n", proc);
    logPrint(LOG_DEBUG, "prog [%p]:\n", proc->prog);
    logPrint(LOG_DEBUG, "\t");
    for (size_t cmd_index = 0; cmd_index < proc->prog_size; cmd_index++)
        logPrint(LOG_DEBUG, "%4zu ", cmd_index);
    logPrint(LOG_DEBUG, "\n");
    logPrint(LOG_DEBUG, "\t");
    for (size_t cmd_index = 0; cmd_index < proc->prog_size; cmd_index++)
        logPrint(LOG_DEBUG, "%4d ", proc->prog[cmd_index]);
    logPrint(LOG_DEBUG, "\n\t");
    for (size_t cmd_skip = 0; cmd_skip < (size_t)((proc->ip) - (proc->prog)); cmd_skip++)
        logPrint(LOG_DEBUG, "     ");
    logPrint(LOG_DEBUG, "   ^ ip = %zu\n", proc->ip - proc->prog);

    logPrint(LOG_DEBUG, "call stack: \n");
    for (size_t index = 0; index < proc->call_stk->size; index++)
        logPrint(LOG_DEBUG, "\t%zu: %04d\n", index, proc->call_stk->data[index]);

    logPrint(LOG_DEBUG, "registers: \n");
    for (size_t reg_index = 0; reg_index < REGNUM; reg_index++){
        logPrint(LOG_DEBUG, "\treg 0x%02X: %d\n", reg_index, proc->reg[reg_index]);
    }
    logPrint(LOG_DEBUG, "-----------PROCESSOR_DUMP_END-----------\n\n");
}

