#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "argvprocessing.h"

int argvReceive(const int argc, char **argv, union fvals *fval)
{
    assert(argv != NULL);
    assert(fval != NULL);

    int argindex = 1;
    while(argindex < argc)
    {
        int flagindex = 0;
        for (; flagindex < ARGVNUM; flagindex++){
            if (strcmp(argv[argindex], args[flagindex].fname) == 0){
                if (args[flagindex].valtype == V_NOTDEF)
                    fval[flagindex].bl = 1;
                else{
                    argindex++;
                    if (argindex >= argc)
                        return ARGBAD;
                    if(fillFval(argv, argindex, fval, flagindex) == ARGBAD)
                        return ARGBAD;
                }
                break;
            }
        } if(flagindex == ARGVNUM) return ARGBAD;
        argindex++;
    }
    return ARGGOOD;
}

int fillFval(char **argv, const int argindex, union fvals *fval, const int flagindex)
{
    assert(argv != NULL);
    assert(fval != NULL);
    switch(args[flagindex].valtype)
    {
        case V_BOOL:
            if (strcmp("0", argv[argindex]) == 0)
                fval[flagindex].bl = 0;
            else if (strcmp("1", argv[argindex]) == 0)
                fval[flagindex].bl = 1;
            else return ARGBAD;
            break;
        case V_STRING:
            strcpy(fval[flagindex].str, argv[argindex]);
            break;
        case V_DOUBLE:
            if (sscanf(argv[argindex], "%lg", &(fval[flagindex].dbl)) != 1)
                return ARGBAD;
            break;
        case V_INT:
            if (sscanf(argv[argindex], "%d", &(fval[flagindex].in)) != 1)
                return ARGBAD;
            break;
        default:
            return ARGBAD;
            break;
    }
    return ARGGOOD;
}

void printHelp()
{
    printf("############################################################################\n");
    for (int flagi = 0; flagi < ARGVNUM; flagi++)
        printf("%10s\t%s\n", args[flagi].fname, args[flagi].help);
    printf("############################################################################\n");
}
