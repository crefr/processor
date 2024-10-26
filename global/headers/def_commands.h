#define PUSH_POP_ARG        scanPushPopArgs(prog)
#define JMP_ARG             handleLableInJmps(prog, &labels)
#define NO_ARGS

#define TO_FLOAT(arg)       (((float)(arg)) / ACC_COEF)
#define TO_INT(arg)         ((int)((arg) * ACC_COEF))


#define PUSH(arg)           stackPush(proc->stk, (arg))
#define POP                 stackPop (proc->stk)

#define CALL_PUSH(arg)      stackPush(proc->call_stk, (arg))
#define CALL_POP            stackPop (proc->call_stk)

#define PUSHORPOP_ARG       *GetPushPopArg(proc)

#define IP                  (proc->ip)
#define IP_REL              (int)((proc->ip) - (proc->prog))
#define IP_REL_TO_ABS(arg)  (proc->prog + (arg))
#define SET_IP(arg)         proc->ip = proc->prog + (arg)

#define ARG(n)              *(proc->ip + (n))
#define DRAW                drawRAM(proc, DRAW_WIDTH)

#define COND_JUMP(cond)              \
    int a = POP;                     \
    int b = POP;                     \
    if (b cond a){                   \
        logPrint(LOG_DEBUG, "jumping on %zu\n", ARG(1));    \
        SET_IP(ARG(1));              \
    }                                \
    else                             \
        IP += 2;
#define MATH_TWO_ARGS(sign)          \
    float a = TO_FLOAT(POP);         \
    float b = TO_FLOAT(POP);         \
    PUSH(TO_INT(b sign a));          \
    IP++;
#define MATH_ONE_ARG(func)           \
    float arg = TO_FLOAT(POP);       \
    PUSH(TO_INT(func(arg)));         \
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
    printf("%g\n", TO_FLOAT(POP));
    IP++;
})
DEF_CMD_(IN,   8,   NO_ARGS,
{
    float a = 0;
    scanf("%g", &a);
    PUSH(TO_INT(a));
    IP++;
})

DEF_CMD_(SQRT, 9,   NO_ARGS, {MATH_ONE_ARG(sqrt)})
DEF_CMD_(SIN,  10,  NO_ARGS, {MATH_ONE_ARG(sin) })
DEF_CMD_(COS,  11,  NO_ARGS, {MATH_ONE_ARG(cos) })

DEF_CMD_(DUMP, 12,  PUSH_POP_ARG,
{
    logPrint(LOG_RELEASE, "--------DUMP #%d--------\n", ARG(1));
    processorDump(proc);
    logPrint(LOG_RELEASE, "--------DUMP #%d END----\n", ARG(1));

    IP+=2;
})

DEF_CMD_(JMP,  13,  JMP_ARG,
{
    logPrint(LOG_DEBUG, "jumping on %zu\n", ARG(1));
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
    CALL_PUSH(IP_REL + 2);
    SET_IP(ARG(1));
})
DEF_CMD_(RET,  21,  NO_ARGS,
{
    SET_IP(CALL_POP);
})
DEF_CMD_(DRAW, 22,  NO_ARGS,
{
    DRAW;
    usleep(30 * 1000);
    IP++;
})
DEF_CMD_(NOCMD, 23,  NO_ARGS,
{
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
