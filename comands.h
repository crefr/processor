#ifndef COMANDS_INCLUDED
#define COMANDS_INCLUDED

enum commands
{
    PUSH_CMD = 1,
    POP_CMD,

    ADD_CMD,
    SUB_CMD,
    MUL_CMD,
    DIV_CMD,

    OUT_CMD,
    IN_CMD,

    SQRT_CMD,
    SIN_CMD,
    COS_CMD,

    DUMP_CMD,

    JMP_CMD,
    JA_CMD,
    HLT_CMD = 0
};
const int CMDNUM_MASK = 0x0f;

enum registers
{
    ZERO = 0,
    RAX = 1,
    RBX,
    RCX,
    RDX
};
const size_t REGNUM = 5;

const size_t ARGMAXLEN = 100;
const int REG_MASK = 0x20;
const int DIG_MASK = 0x10;

#endif
