#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "assembler.h"
#include "logger.h"

const size_t MAXPROGLEN = 10000;
int main(int argc, char ** argv)
{
    logStart("asmlog.txt", LOG_DEBUG_PLUS);
    FILE * in_file = fopen((argc > 1) ? argv[1] : ("program.asm"), "r");
    assert(in_file != NULL);

    FILE * code_text_file = fopen("program_code_text.txt", "w");
    FILE * code_file = fopen("program_code.txt", "wb");

    int program[MAXPROGLEN] = {};
    program_t prog = progCtor(program, in_file, code_file, code_text_file);

    size_t prog_len = assembleRun(&prog);
    progToText(&prog);
    progToCode(&prog);
    logExit();
    fclose(in_file);
    fclose(code_text_file);
    fclose(code_file);
    return 0;
}
