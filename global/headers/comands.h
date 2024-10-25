#ifndef COMANDS_INCLUDED
#define COMANDS_INCLUDED

#include <stdint.h>

#define DEF_CMD_(cmd, num, ...)     \
    cmd ## _CMD = num,
enum commands
{
    #include "def_commands.h"
    ERROR_CMD = -1
};
#undef DEF_CMD_

enum registers
{
    ZERO = 0,
    RAX = 1,
    RBX,
    RCX,
    RDX
};
const size_t REGNUM = 5;

typedef struct {
    const char * name;
    enum commands id;
} command_t;

const command_t Commands[] =
{
    {"push" , PUSH_CMD },
    {"pop"  , POP_CMD  },
    {"jmp"  , JMP_CMD  },
    {"ja"   , JA_CMD   },
    {"jb"   , JB_CMD   },
    {"jae"  , JAE_CMD  },
    {"jbe"  , JBE_CMD  },
    {"je"   , JE_CMD   },
    {"jne"  , JNE_CMD  },
    {"add"  , ADD_CMD  },
    {"sub"  , SUB_CMD  },
    {"mul"  , MUL_CMD  },
    {"div"  , DIV_CMD  },
    {"out"  , OUT_CMD  },
    {"in"   , IN_CMD   },
    {"hlt"  , HLT_CMD  },
    {"call" , CALL_CMD },
    {"ret"  , RET_CMD  },
    {"draw" , DRAW_CMD }
};
const size_t Cmd_Num = sizeof(Commands) / sizeof(command_t);

const size_t ARGMAXLEN = 100;

const int CMDNUM_MASK = 0x1f;
const int IMM_MASK = 0x20;
const int REG_MASK = 0x40;
const int MEM_MASK = 0x80;

const int ACC_COEF = 1000;

const uint64_t SIGNATURE = * (const uint64_t *)"*crefr++";
const uint64_t COMMAND_VERSION = 3;
typedef struct {
    uint64_t sign;
    uint32_t version;
    uint64_t size;
} header_t;

#endif
