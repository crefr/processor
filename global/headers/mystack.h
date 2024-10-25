#ifndef MYSTACK_INCLUDED
#define MYSTACK_INCLUDED

#include <stdint.h>

#define STACK_DEBUG
#define STACK_STRUCT_CANARIES_ON
#define STACK_DATA_CANARIES_ON
#define STACK_HASH_ON

#ifdef STACK_DEBUG
    #define STACKASSERT(stkptr, expr)                                                                                       \
        do{                                                                                                                 \
            if (!(expr)){                                                                                                   \
                LOGPRINTWITHTIME(LOG_RELEASE, "---\\/\n<<<<<<<<STACK ERROR>>>>>>>>");                                       \
                PRINTFANDLOG(LOG_RELEASE, "Assertion failed:\n\t{" #expr "}\n\tFILE %s, in FUNCTION \"%s\", LINE %d",       \
                        __FILE__, __PRETTY_FUNCTION__, __LINE__);                                                           \
                stackDump(stkptr);                                                                                          \
                logExit();                                                                                                  \
                exit(1);                                                                                                    \
        }                                                                                                                   \
        }while(0)
#else
    #define STACKASSERT(stkptr, expr)
#endif

// you can not redefine canary_t type
typedef uint64_t canary_t;
#define CANARY1 0xC9EF9228
#define CANARY2 0x228C9EF9

#ifdef STACK_STRUCT_CANARIES_ON
    #define IF_STACK_STRUCT_CANARIES_ON(...) __VA_ARGS__
#else
    #define IF_STACK_STRUCT_CANARIES_ON(...)
#endif

#ifdef STACK_DATA_CANARIES_ON
    #define IF_STACK_DATA_CANARIES_ON(...) __VA_ARGS__
#else
    #define IF_STACK_DATA_CANARIES_ON(...)
#endif

#ifdef STACK_HASH_ON
    #define IF_STACK_HASH_ON(...) __VA_ARGS__
    typedef uint32_t hash_t;
#else
    #define IF_STACK_HASH_ON(...)
#endif

/// @brief enum with errors that stackOK can return
typedef enum {
    STACK_OK = 0,
    STACK_UNDEF_ERROR,
    STACK_HASH_ERROR,
    STACK_DATA_ERROR,
    STACK_SIZE_TOOBIG,
    STACK_STRUCT_CANARY_ERROR,
    STACK_DATA_CANARY_ERROR
} stackstatus;

typedef enum {
    STACK_LEFT_CANARY_CORRUPTED = -1,
    STACK_CANARIES_OK = 0,
    STACK_RIGHT_CANARY_CORRUPTED = 1
} canarystatus;

typedef int stack_elem_t;
const stack_elem_t stackpoison = -545;

/// @brief struct with stack, do not move uint32_t hash, it must be first (to skip it while calculating hash)
typedef struct {
    IF_STACK_HASH_ON(hash_t hash);
    IF_STACK_STRUCT_CANARIES_ON(canary_t structcanary1);
    stack_elem_t * data;
    size_t size;
    size_t capacity;
    stackstatus errNo;
    IF_STACK_STRUCT_CANARIES_ON(canary_t structcanary2);
} stack_t;

#define MINSTACKDIFF 16

IF_STACK_HASH_ON(
    /// @brief calculates hash and returns it
    hash_t stackGetHash(stack_t * stk);
)

/// @brief constructs stack
stack_t stackCtor(size_t start_cap);

/// @brief destroys stack
void stackDtor(stack_t * stk);

/// @brief makes pop, returns popped elem
stack_elem_t stackPop(stack_t * stk);

/// @brief makes push to stack
void stackPush(stack_t * stk, stack_elem_t val);

/// @brief checks if stack is OK, returns stackstatus
stackstatus stackOK(stack_t * stk);

/// @brief dumps stack to log file
void stackDump(stack_t * stk);

#endif
