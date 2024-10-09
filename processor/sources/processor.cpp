#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "processor.h"
#include "mystack.h"

const size_t MAXCMDLEN = 50;
void processorRun(processor_t * proc)
{
    stack_t stk = stackCtor(0);
    while (1){
        char cmd[MAXCMDLEN] = "";
        scanf("%s", cmd);

        if (strcmp(cmd, "push") == 0){
            int arg = 0;
            scanf("%d", &arg);
            stackPush(&stk, arg);
            continue;
        }
        if (strcmp(cmd, "add") == 0){
            int a = stackPop(&stk);
            int b = stackPop(&stk);
            stackPush(&stk, a + b);
            continue;
        }
        if (strcmp(cmd, "sub") == 0){
            int a = stackPop(&stk);
            int b = stackPop(&stk);
            stackPush(&stk, b - a);
            continue;
        }
        if (strcmp(cmd, "out") == 0){
            int out_elem = stackPop(&stk);
            printf("%d\n", out_elem);
            continue;
        }
        if (strcmp(cmd, "hlt") == 0){
            break;
        }
    }
    stackDtor(&stk);
}
