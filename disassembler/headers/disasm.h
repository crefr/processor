#ifndef PROCESSOR_INCLUDED
#define PROCESSOR_INCLUDED

#include "mystack.h"
#include "comands.h"

typedef struct {
    FILE * out_file;
    int * program;
    int * ip;
    size_t prog_size;
} program_t;

typedef enum
{
    DASM_SUCCESS = 0,
    DASM_HEADER_ERROR,
    DASM_CTOR_CALLOC_ERROR,
    DASM_CTOR_READ_ERROR,
    DASM_UNDEF_CMD_CODE,
    DASM_DOUBLE_DTOR,
    DASM_OTHER_ERROR
} disasm_status_t;

disasm_status_t progCtor(program_t * prog, FILE * code_file, FILE * out_file);

disasm_status_t progDtor(program_t * prog);

disasm_status_t disAsm(program_t * prog);

#endif
