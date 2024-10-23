#define PUSH_POP_ARG    scanPushPopArgs(prog)
#define JMP_ARG         handleLableInJmps(prog, &labels)
#define NO_ARGS

#define PUSH(arg)       stackPush(proc->stk, arg)
#define POP             stackPop (proc->stk)

#define CALL_PUSH(arg)  stackPush(proc->call_stk, arg)
#define CALL_POP        stackPop (proc->call_stk)

#define PUSHORPOP_ARG   *GetPushPopArg(proc)

#define IP              proc->ip
#define SET_IP(arg)     proc->ip = proc->prog + arg;

#define ARG(n)          *(proc->ip + n)
#define DRAW            drawRAM(proc, DRAW_WIDTH)

#define COND_JUMP(cond)              \
    int a = POP;                     \
    int b = POP;                     \
    if ((a) cond (b))              \
        SET_IP(ARG(1));              \
    IP++;
#define MATH_TWO_ARGS(sign)          \
    int a = POP;                     \
    int b = POP;                     \
    PUSH((a) sign (b));            \
    IP++;

DEF_CMD_(HLT,  0,   NO_ARGS,
{
    quit = 1;
})

DEF_CMD_(PUSH, 1,   PUSH_POP_ARG,
{
    PUSH(PUSHORPOP_ARG);
    IP++;
})

DEF_CMD_(POP,  2,   PUSH_POP_ARG,
{
    PUSHORPOP_ARG = POP;
    IP++;
})

DEF_CMD_(ADD,  3,   NO_ARGS, {MATH_TWO_ARGS(+)})
DEF_CMD_(SUB,  4,   NO_ARGS, {MATH_TWO_ARGS(-)})
DEF_CMD_(MUL,  5,   NO_ARGS, {MATH_TWO_ARGS(*)})
DEF_CMD_(DIV,  6,   NO_ARGS, {MATH_TWO_ARGS(/)})

DEF_CMD_(OUT,  7,   NO_ARGS,
{
    printf("%d\n", POP);
    IP++;
})
DEF_CMD_(IN,   8,   NO_ARGS,
{
    int a = 0;
    scanf("%d", &a);
    PUSH(a);
    IP++;
})

// DEF_CMD_(SQRT, 9,   NO_ARGS )
// DEF_CMD_(SIN,  10,  NO_ARGS )
// DEF_CMD_(COS,  11,  NO_ARGS )

// DEF_CMD_(DUMP, 12,  NO_ARGS )

DEF_CMD_(JMP,  13,  JMP_ARG,
{
    SET_IP(ARG(1));
})

DEF_CMD_(JA,   14,  JMP_ARG, {COND_JUMP(>); })
DEF_CMD_(JB,   15,  JMP_ARG, {COND_JUMP(<); })
DEF_CMD_(JAE,  16,  JMP_ARG, {COND_JUMP(>=);})
DEF_CMD_(JBE,  17,  JMP_ARG, {COND_JUMP(<=);})
DEF_CMD_(JE,   18,  JMP_ARG, {COND_JUMP(==);})
DEF_CMD_(JNE,  19,  JMP_ARG, {COND_JUMP(!=);})

DEF_CMD_(CALL, 20,  JMP_ARG,
{
    SET_IP(ARG(1));
    CALL_PUSH(ARG(1));
})
DEF_CMD_(RET,  21,  NO_ARGS,
{
    SET_IP(CALL_POP);
})
DEF_CMD_(DRAW, 22,  NO_ARGS,
{
    DRAW;
    IP++;
})

#undef PUSH_POP_ARG
#undef JMP_ARG
#undef NO_ARGS
#undef PUSH
#undef POP
#undef CALL_PUSH
#undef CALL_POP
#undef PUSH_POP_ARG
#undef IP
#undef SET_IP
#undef ARG
#undef DRAW
#undef COND_JUMP
#undef MATH_TWO_ARGS
