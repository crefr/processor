#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "logger.h"
#include "processor.h"
#include "mystack.h"
#include "../../comands.h"

const size_t MAXCMDLEN = 50;
const size_t MAXPROGLEN = 50000;

static int procGetArg(processor_t * proc);
static void calcTwoArgs(processor_t * proc);
void processorRun(processor_t * proc)
{
    int quit = 0;
    while (proc->ip < proc->ip + proc->prog_size && quit != 1){
        processorDump(proc);
        switch ((*(proc->ip)) & CMDNUM_MASK){
        case PUSH_CMD:{
            stackPush(proc->stk, procGetArg(proc));
            break;
        }
        case POP_CMD:{
            proc->ip++;
            proc->reg[*(proc->ip)] = stackPop(proc->stk);
            break;
        }
        case ADD_CMD: case SUB_CMD: case MUL_CMD: case DIV_CMD:{
            calcTwoArgs(proc);
            break;
        }
        case JMP_CMD:{
            proc->ip = proc->prog + *(proc->ip + 1);
            continue;
        }
        case JA_CMD:{
            int a = stackPop(proc->stk);
            int b = stackPop(proc->stk);
            if (a > b){
                proc->ip = proc->prog + *(proc->ip + 1);
                continue;
            }
            else
                proc->ip++;
            break;
        }
        case OUT_CMD:{
            int out_elem = stackPop(proc->stk);
            printf("%d\n", out_elem);
            break;
        }
        case IN_CMD:{
            int in_elem = 0;
            scanf("%d", &in_elem);
            stackPush(proc->stk, in_elem);
            break;
        }
        case HLT_CMD:{
            quit = 1;
            break;
        }
        default:
            quit = 1;
            PRINTFANDLOG(LOG_RELEASE, "invalid instruction: %02X ", (unsigned int) *(proc->ip));
            break;
        }
        proc->ip++;
    }
}
static void calcTwoArgs(processor_t * proc)
{
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
    //fscanf(prog_file, "%zu", &(proc->prog_size));

    //making stack
    proc->stk = (stack_t *)calloc(1, sizeof(stack_t));
    *(proc->stk) = stackCtor(0);

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

void processorDump(processor_t * proc)
{
    logPrint(LOG_DEBUG, "\n-------------PROCESSOR_DUMP-------------\n");
    logPrint(LOG_DEBUG, "processor at %p\n", proc);
    logPrint(LOG_DEBUG, "\tprog [%p]:\n", proc->prog);
    logPrint(LOG_DEBUG, "\t\t");
    for (size_t cmd_index = 0; cmd_index < proc->prog_size; cmd_index++)
        logPrint(LOG_DEBUG, "%4zu ", cmd_index);
    logPrint(LOG_DEBUG, "\n");
    logPrint(LOG_DEBUG, "\t\t");
    for (size_t cmd_index = 0; cmd_index < proc->prog_size; cmd_index++)
        logPrint(LOG_DEBUG, "%4d ", proc->prog[cmd_index]);
    logPrint(LOG_DEBUG, "\n\t\t");
    for (size_t cmd_skip = 0; cmd_skip < (size_t)((proc->ip) - (proc->prog)); cmd_skip++)
        logPrint(LOG_DEBUG, "     ");
    logPrint(LOG_DEBUG, "   ^ ip = %zu\n", proc->ip - proc->prog);

    logPrint(LOG_DEBUG, "\tregisters: \n");
    for (size_t reg_index = 0; reg_index < REGNUM; reg_index++){
        logPrint(LOG_DEBUG, "\t\treg 0x%02X: %d\n", reg_index, proc->reg[reg_index]);
    }
    logPrint(LOG_DEBUG, "-----------PROCESSOR_DUMP_END-----------\n\n");
}

