#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "assembler.h"
#include "logger.h"

const size_t MAXPROGLEN = 400000000;
int main(int argc, char ** argv)
{
    logStart("asmlog.txt", LOG_RELEASE);
    FILE * in_file = fopen((argc > 1) ? argv[1] : ("program.asm"), "r");
    assert(in_file != NULL);

    FILE * code_text_file = fopen("program_code_text.txt", "w");
    FILE * code_file = fopen("program_code.txt", "wb");

    int *program = (int *)calloc(MAXPROGLEN, sizeof(int));
    program_t prog = progCtor(program, in_file, code_file, code_text_file);

    size_t prog_len = assembleRun(&prog);
    printf("program length in tokens: %zu\n", prog_len);
    //progToText(&prog);
    progToCode(&prog);
    logExit();
    free(program);
    fclose(in_file);
    fclose(code_text_file);
    fclose(code_file);
    return 0;
}
