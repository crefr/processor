#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "mystack.h"
#include "hash.h"
#include "logger.h"

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

IF_STACK_STRUCT_CANARIES_ON(
    /// @brief checks if struct canaries are OK, returns 1 if OK, 0 if not
    static canarystatus checkIfStructCanariesOK(stack_t * stk);
)
IF_STACK_HASH_ON(
    /// @brief calculates hash and updates it in stack_t struct
    static void stackUpdateHash(stack_t * stk);
)
IF_STACK_DATA_CANARIES_ON(
    /// @brief fills canaries at data
    static void fillDataCanaries(stack_t * stk);

    /// @brief checks if data canaries are ok
    static canarystatus checkIfDataCanariesOK(stack_t * stk);

    /// @brief gets size in canaries
    static size_t stackGetSizeInCanaries(stack_t * stk);
)

/// @brief poisons the rest of stack (from size to the end of capacity)
static void stackPoisonRest(stack_t * stk);

/// @brief reduces stack by 2 when size < capacity / 4
static void stackReduce(stack_t * stk);

/// @brief enlarges stack by 2 when size = capacity
static void stackEnlarge(stack_t * stk);

/// @brief resizes stack (is used by stackReduce and stackEnlarge)
static void stackResize(stack_t * stk, size_t newcap);

stack_t stackCtor(size_t start_cap)
{
    stack_t stk = {};
    stk.size = 0;
    stk.capacity = start_cap;
    stk.errNo = STACK_OK;
    IF_STACK_STRUCT_CANARIES_ON(stk.structcanary1 = CANARY1;)

  #ifndef STACK_DATA_CANARIES_ON
    stk.data = (stack_elem_t *) calloc(start_cap, sizeof(stack_elem_t));
  #else
    size_t startcapincanaries = stackGetSizeInCanaries(&stk);
    void * datawithcanaries = calloc(startcapincanaries + 2, sizeof(canary_t));
    datawithcanaries = (canary_t *)datawithcanaries + 1;
    stk.data = (stack_elem_t *)datawithcanaries;
    fillDataCanaries(&stk);
  #endif

    stackPoisonRest(&stk);
    IF_STACK_STRUCT_CANARIES_ON(stk.structcanary2 = CANARY2;)
    IF_STACK_HASH_ON(stackUpdateHash(&stk);)
    STACKASSERT(&stk, stackOK(&stk) == STACK_OK);
    LOGPRINTWITHTIME(LOG_DEBUG_PLUS, "STACK CONSTRUCTED, start_cap = %zu", start_cap);
    return stk;
}

void stackDtor(stack_t * stk)
{
    assert(stk != NULL);
    STACKASSERT(stk, stackOK(stk) == STACK_OK);
  #ifndef STACK_DATA_CANARIES_ON
    free(stk->data);
  #else
    free((canary_t *)(stk->data) - 1);
  #endif
    LOGPRINTWITHTIME(LOG_DEBUG_PLUS, "STACK DESTRUCTED");
}

static void stackResize(stack_t * stk, size_t newcap)
{
    assert(stk != NULL);
    LOGPRINTWITHTIME(LOG_DEBUG_PLUS, "stack RESIZE from %zu to %zu", stk->capacity, newcap);
    stk->capacity = newcap;
  #ifndef STACK_DATA_CANARIES_ON
    stk->data = (stack_elem_t *) realloc(stk->data, newcap * sizeof(stack_elem_t));
  #else
    size_t newsizeincanaries = stackGetSizeInCanaries(stk) + 2;
    void * datawithcanaries = realloc((canary_t *)stk->data - 1, newsizeincanaries * sizeof(canary_t));
    stk->data = (stack_elem_t *) ((canary_t *)datawithcanaries + 1);
    fillDataCanaries(stk);
  #endif
}

static void stackEnlarge(stack_t * stk)
{
    assert(stk != NULL);
    stackResize(stk, MAX(MINSTACKDIFF, 2 * stk->capacity));
    stackPoisonRest(stk);
}

static void stackReduce(stack_t * stk)
{
    assert(stk != NULL);
    if (stk->capacity > MINSTACKDIFF)
        stackResize(stk, stk->capacity / 2);
}

stack_elem_t stackPop(stack_t * stk)
{
    assert(stk != NULL);
    STACKASSERT(stk, stk->size != 0);
    LOGPRINTWITHTIME(LOG_DEBUG_PLUS, "stack POP, size: %zu, val: %lg", stk->size, stk->data[stk->size - 1]);
    stack_elem_t val = stk->data[stk->size - 1];
    if (stk->size - 1 <= stk->capacity / 4)
        stackReduce(stk);
    stk->data[stk->size - 1] = stackpoison;
    stk->size--;
    STACKASSERT(stk, (stk->size) != (stk->capacity));
    IF_STACK_HASH_ON(stackUpdateHash(stk));
    STACKASSERT(stk, stackOK(stk) == STACK_OK);
    return val;
}

void stackPush(stack_t * stk, stack_elem_t val)
{
    assert(stk != NULL);
    LOGPRINTWITHTIME(LOG_DEBUG_PLUS, "stack PUSH, size: %zu, val: %lg", stk->size, val);
    STACKASSERT(stk, stackOK(stk) == STACK_OK);
    if (stk->size == stk->capacity)
        stackEnlarge(stk);
    stk->data[stk->size] = val;
    stk->size++;
    IF_STACK_HASH_ON(stackUpdateHash(stk));
    STACKASSERT(stk, stackOK(stk) == STACK_OK);
}

 static void stackPoisonRest(stack_t * stk)
{
    for (size_t index = stk->size; index < stk->capacity; index++)
        stk->data[index] = (stack_elem_t) stackpoison;
}

void stackDump(stack_t * stk)
{
    assert(stk != NULL);
    logPrint(LOG_DEBUG, "-----------STACK DUMP-----------");
    logPrint(LOG_DEBUG, "stack_t[%p]{", stk);
    logPrint(LOG_DEBUG, "\terrNo = %d", stk->errNo);
    logPrint(LOG_DEBUG, "\tsize = %lu", stk->size);
    logPrint(LOG_DEBUG, "\tcapacity = %lu", stk->capacity);
IF_STACK_HASH_ON(
    logPrint(LOG_DEBUG, "\thash = %08X", stk->hash);
)
    logPrint(LOG_DEBUG, "\tdata[%p]", stk->data);
    if (stk->data != NULL){
        logPrint(LOG_DEBUG, "\t{");
        for (size_t index = 0; index < stk->capacity; index++){
            logPrint(LOG_DEBUG, "\t\t[%lu] = %d", index, stk->data[index]);
        }
        logPrint(LOG_DEBUG, "\t}");
    }
    logPrint(LOG_DEBUG, "}\n---------STACK DUMP END---------");
}

stackstatus stackOK(stack_t * stk)
{
    assert(stk != NULL);
    stackstatus errNo = STACK_OK;
    if (errNo == STACK_OK && stk->data == NULL)
        errNo = STACK_DATA_ERROR;
    if (errNo == STACK_OK && stk->size > stk->capacity)
        errNo = STACK_SIZE_TOOBIG;
IF_STACK_STRUCT_CANARIES_ON(
    if (errNo == STACK_OK && checkIfStructCanariesOK(stk) != STACK_CANARIES_OK)
        errNo = STACK_STRUCT_CANARY_ERROR;
)
IF_STACK_DATA_CANARIES_ON(
    if (errNo == STACK_OK && checkIfDataCanariesOK(stk) != STACK_CANARIES_OK)
        errNo = STACK_DATA_CANARY_ERROR;
)
IF_STACK_HASH_ON(
    if (errNo == STACK_OK && stk->hash != stackGetHash(stk))
        errNo = STACK_HASH_ERROR;
)
    stk->errNo = errNo;
    return errNo;
}


IF_STACK_HASH_ON(
hash_t stackGetHash(stack_t * stk)
{
    assert(stk != NULL);
    uint32_t datahash   = MurMur32Hash(stk->data, stk->capacity * sizeof(stack_elem_t), 0);
    char * structstartptr = (char *)stk + sizeof(hash_t);
    uint32_t structhash = MurMur32Hash(structstartptr, sizeof(stack_t) - sizeof(hash_t), 0);
    uint32_t hash = (datahash >> 1) + (structhash >> 1);
    return hash;
}
)

IF_STACK_HASH_ON(
static void stackUpdateHash(stack_t * stk)
{
    assert(stk != NULL);
    stk->hash = stackGetHash(stk);
}
)

IF_STACK_STRUCT_CANARIES_ON(
static canarystatus checkIfStructCanariesOK(stack_t * stk)
{
    assert(stk != NULL);
    if (stk->structcanary1 != CANARY1)
        return STACK_LEFT_CANARY_CORRUPTED;
    if (stk->structcanary2 != CANARY2)
        return STACK_RIGHT_CANARY_CORRUPTED;
    return STACK_CANARIES_OK;
}
)

IF_STACK_DATA_CANARIES_ON(
static void fillDataCanaries(stack_t * stk)
{
    assert(stk != NULL);
    *((canary_t *)(stk->data) - 1) = CANARY1;
    *((canary_t *)(stk->data) + stackGetSizeInCanaries(stk)) = CANARY2;
}
)

IF_STACK_DATA_CANARIES_ON(
static canarystatus checkIfDataCanariesOK(stack_t * stk)
{
    assert(stk != NULL);
    canary_t * leftcanaryptr  = (canary_t *)(stk->data) - 1;
    canary_t * rightcanaryptr = (canary_t *)(stk->data) + stackGetSizeInCanaries(stk);
    if (*leftcanaryptr  != CANARY1)
        return STACK_LEFT_CANARY_CORRUPTED;
    if (*rightcanaryptr != CANARY2)
        return STACK_RIGHT_CANARY_CORRUPTED;
    return STACK_CANARIES_OK;
}
)

IF_STACK_DATA_CANARIES_ON(
static size_t stackGetSizeInCanaries(stack_t * stk)
{
    assert(stk != NULL);
    size_t size_in_canaries = ((stk->capacity) * sizeof(stack_elem_t)) / sizeof(canary_t);
    if (size_in_canaries % sizeof(canary_t) != 0)
        size_in_canaries++;
    return size_in_canaries;
}
)
