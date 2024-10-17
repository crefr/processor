#ifndef COMANDS_INCLUDED
#define COMANDS_INCLUDED

enum commands
{
    ERROR_CMD = -1,

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
    {"add"  , ADD_CMD  },
    {"sub"  , SUB_CMD  },
    {"mul"  , MUL_CMD  },
    {"div"  , DIV_CMD  },
    {"out"  , OUT_CMD  },
    {"in"   , IN_CMD   },
    {"hlt"  , HLT_CMD  }
};
const size_t Cmd_Num = sizeof(Commands) / sizeof(command_t);

const size_t ARGMAXLEN = 100;

const int CMDNUM_MASK = 0x1f;
const int REG_MASK = 0x40;
const int DIG_MASK = 0x20;

const char SIGNATURE[] = "crefr";

#endif
