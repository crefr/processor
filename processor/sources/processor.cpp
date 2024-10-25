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
const size_t RAM_SIZE = 96 * 36;

static int * GetPushPopArg(processor_t * proc);

static void drawRAM(processor_t * proc, size_t width);


#define DEF_CMD_(cmd_name, num, arg, ...)   \
    case cmd_name ## _CMD:                  \
    __VA_ARGS__;                            \
    break;
void processorRun(processor_t * proc)
{
    assert(proc);
    int quit = 0;
    while (proc->ip < proc->ip + proc->prog_size && quit != 1){
        processorDump(proc);
        switch ((*(proc->ip)) & CMDNUM_MASK){
/**************************CRINGE_START*******************************/
            #include "def_commands.h"
            default:
                quit = 1;
                PRINTFANDLOG(LOG_RELEASE, "invalid instruction: %02X ", (unsigned int) *(proc->ip));
                break;
/**************************CRINGE_END*********************************/
        }
    }
}
#undef DEF_CMD_

static proc_status_t processorGetProgFromCode(processor_t * proc, FILE * prog_file);
proc_status_t processorCtor(processor_t * proc, FILE * prog_file)
{
    assert(proc);
    assert(prog_file);

    //making stack
    proc->stk = (stack_t *)calloc(1, sizeof(stack_t));
    *(proc->stk) = stackCtor(0);

    //making call stack for functions
    proc->call_stk = (stack_t *)calloc(1, sizeof(stack_t));
    *(proc->call_stk) = stackCtor(0);

    //getting RAM
    proc->RAM = (int *)calloc(RAM_SIZE, sizeof(int));
    proc->RAM_size = RAM_SIZE;

    //getting the program
    if (processorGetProgFromCode(proc, prog_file) != PROC_SUCCESS){
        return PROC_PROG_ERROR;
    }

    //initializing ip
    proc->ip = proc->prog;
    return PROC_SUCCESS;
}

static bool readHeader(processor_t * proc, FILE * prog_file);
static proc_status_t processorGetProgFromCode(processor_t * proc, FILE * prog_file)
{
    assert(proc);
    assert(prog_file);
    if (!readHeader(proc, prog_file)){
        PRINTFANDLOG(LOG_RELEASE, "incorrect signature\n");
        return PROC_PROG_ERROR;
    }
    proc->prog = (int *)calloc(proc->prog_size, sizeof(int));
    fread(proc->prog, sizeof(int), proc->prog_size, prog_file);
    return PROC_SUCCESS;
}

static bool readHeader(processor_t * proc, FILE * prog_file)
{
    assert(proc);
    assert(prog_file);
    header_t head = {};
    fread(&head, sizeof(head), 1, prog_file);

    proc->prog_size = head.size;
    if (head.sign != SIGNATURE || head.version != COMMAND_VERSION)
        return false;
    return true;;
}

void processorDtor(processor_t * proc)
{
    assert(proc);
    free(proc->prog);
    proc->prog = NULL;

    stackDtor(proc->call_stk);
    free(proc->call_stk);

    stackDtor(proc->stk);
    free(proc->stk);

    free(proc->RAM);
    proc->RAM = NULL;

    proc->stk = NULL;
    proc->call_stk = NULL;
}

static int * GetPushPopArg(processor_t * proc)
{
    assert(proc);
    int cmd = *(proc->ip);

    const bool imm = cmd & IMM_MASK;
    const bool reg = cmd & REG_MASK;
    const bool mem = cmd & MEM_MASK;

    proc->ip++;
    int * result = NULL;

    if (mem){
        result = proc->RAM;
        if (reg)
            result += (proc->reg[*(proc->ip++)]) / ACC_COEF;
        if (imm)
            result += *(proc->ip) / ACC_COEF;
        return result;
    }
    if (imm){
        proc->reg[0] = 0;
        if (reg)
            proc->reg[0] += (proc->reg[*(proc->ip++)]);
        proc->reg[0] += *(proc->ip);
        result = proc->reg + 0;
        return result;
    }
    if (reg){
        result = proc->reg + *(proc->ip);
    }
    return result;
}

void processorDump(processor_t * proc)
{
    assert(proc);
    if (logGetLevel() >= LOG_DEBUG){
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

        logPrint(LOG_DEBUG, "RAM: \n");
        for (size_t index = 0; index < RAM_SIZE; index++)
            logPrint(LOG_DEBUG, "\t%04zu: %04d\n", index, proc->RAM[index]);

        logPrint(LOG_DEBUG, "registers: \n");
        for (size_t reg_index = 0; reg_index < REGNUM; reg_index++){
            logPrint(LOG_DEBUG, "\treg 0x%02X: %d\n", reg_index, proc->reg[reg_index]);
        }
        logPrint(LOG_DEBUG, "-----------PROCESSOR_DUMP_END-----------\n\n");
    }
}

const char WHITE_CHAR = '*';
const char BLACK_CHAR = '.';
// const char * RET_STRING = "\033[96D\033[36A";
const char * RET_STRING = "\033[3J\r";
static void drawRAM(processor_t * proc, size_t width)
{
    logPrint(LOG_DEBUG_PLUS, "drawing...\n");
    fputs(RET_STRING, stdout);
    for (size_t mem_index = 0; mem_index < proc->RAM_size; mem_index++){
        if (mem_index % width == 0 && mem_index != 0)
            putchar('\n');
        if (proc->RAM[mem_index] == 0)
            putchar(BLACK_CHAR);
        else
            putchar(WHITE_CHAR);
    }
    //printf("\033[%zuD\033[%zuA", width, proc->RAM_size / width);
    fflush(stdout);
}

