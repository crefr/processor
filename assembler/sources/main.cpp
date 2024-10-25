#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "assembler.h"
#include "logger.h"

const size_t MAX_FILE_NAME = 100;
const char * OUT_FILE_EXT = "bin";
const char * OUT_FILE_TEXT_EXT = "txt";

void changeExtension(char * new_name, const char * old_name, const char * ext_str);

int main(int argc, char ** argv)
{
    logStart("asmlog.txt", LOG_DEBUG_PLUS);
    const char * input_file_name = (argc > 1) ? argv[1] : ("program.asm");
    FILE * in_file = fopen((argc > 1) ? argv[1] : ("program.asm"), "r");
    assert(in_file != NULL);

    char code_file_name[MAX_FILE_NAME] = "";
    char code_text_file_name[MAX_FILE_NAME] = "";

    changeExtension(code_file_name, input_file_name, OUT_FILE_EXT);
    changeExtension(code_text_file_name, input_file_name, OUT_FILE_TEXT_EXT);

    FILE * code_text_file = fopen(code_text_file_name, "w");
    FILE * code_file = fopen(code_file_name, "wb");

    program_t prog = progCtor(in_file, code_file, code_text_file);

    size_t prog_len = assembleRun(&prog);
    printf("program length in tokens: %zu\n", prog_len);
    progToText(&prog);
    progToCode(&prog);
    progDtor(&prog);
    logExit();
    fclose(in_file);
    fclose(code_text_file);
    fclose(code_file);
    return 0;
}

void changeExtension(char * new_name, const char * old_name, const char * ext_str)
{
    assert(new_name);
    assert(old_name);
    assert(ext_str );

    strcpy(new_name, old_name);
    char * dot = strrchr(new_name, '.');

    if (dot != NULL){
        strcpy(dot + 1, ext_str);
    }
    else {
        size_t len = strlen(new_name);
        *(new_name + len) = '.';
        strcpy(new_name + len + 1, ext_str);
    }
}
