#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "logger.h"
#include "disasm.h"
#include "mystack.h"
#include "comands.h"

const size_t MAXCMDLEN = 50;
const size_t RAM_SIZE = 96 * 36;

static disasm_status_t readHeader(program_t * prog, FILE * code_file);

static disasm_status_t handlePushPopArgs(program_t * prog);

static disasm_status_t handleJmpArg(program_t * prog, int * labels);

static disasm_status_t readHeader(program_t * prog, FILE * code_file)
{
    assert(prog);
    assert(code_file);
    header_t head = {};
    fread(&head, sizeof(head), 1, code_file);

    prog->prog_size = head.size;
    if (head.sign != SIGNATURE || head.version != COMMAND_VERSION)
        return DASM_HEADER_ERROR;
    return DASM_SUCCESS;
}

disasm_status_t progCtor(program_t * prog, FILE * code_file, FILE * out_file)
{
    assert(prog);
    if (readHeader(prog, code_file) == DASM_HEADER_ERROR)
        return DASM_HEADER_ERROR;

    prog->program = (int *)calloc(prog->prog_size, sizeof(int));
    if (prog->program == NULL)
        return DASM_CTOR_CALLOC_ERROR;
    prog->ip = prog->program;

    size_t read_size = fread(prog->program, sizeof(int), prog->prog_size, code_file);
    if (read_size != prog->prog_size)
        return DASM_CTOR_READ_ERROR;

    prog->out_file = out_file;

    return DASM_SUCCESS;
}

disasm_status_t progDtor(program_t * prog)
{
    if (prog == NULL)
        return DASM_DOUBLE_DTOR;

    free(prog->program);
    prog->program = NULL;

    return DASM_SUCCESS;
}

#define DEF_CMD_(name, num, handle_arg, ...)    \
    case name ## _CMD:{                         \
        fprintf(prog->out_file, #name);         \
        handle_arg;                             \
        fputc('\n', prog->out_file);            \
        prog->ip++;                             \
        break;                                  \
    }

disasm_status_t disAsm(program_t * prog)
{
    assert(prog);
    int labels = 0; //TODO: this should be removed
    while (prog->ip < prog->program + prog->prog_size){
        int cmd_code = *(prog->ip) & CMDNUM_MASK;
        switch (cmd_code){
            #include "def_commands.h"
            default:
                fprintf(stderr, "undefined command code: %02X\n", cmd_code);
                return DASM_UNDEF_CMD_CODE;
        }
    }
    return DASM_SUCCESS;
}

#undef DEF_CMD_

static disasm_status_t handlePushPopArgs(program_t * prog)
{
    assert(prog);
    int cmd = *(prog->ip);

    const bool imm = cmd & IMM_MASK;
    const bool reg = cmd & REG_MASK;
    const bool mem = cmd & MEM_MASK;

    int * result = NULL;

    fputc(' ', prog->out_file);
    if (mem)
        fprintf(prog->out_file, "[");
    if (reg){
        prog->ip++;
        fprintf(prog->out_file, "r%cx ", *(prog->ip) + 'a' - 1);
    }
    if (imm){
        prog->ip++;
        fprintf(prog->out_file, "%d ",*(prog->ip));
    }
    if (mem)
        fprintf(prog->out_file, "]");
    return DASM_SUCCESS;
}

static disasm_status_t handleJmpArg(program_t * prog, int * labels)
{
    labels += 1; //TODO: this should be removed

    prog->ip++;
    fprintf(prog->out_file, " %d", *(prog->ip));
    return DASM_SUCCESS;
}
