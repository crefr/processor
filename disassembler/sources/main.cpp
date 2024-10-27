#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "disasm.h"
#include "logger.h"

void changeEnding(char * new_name, const char * old_name, const char * ext_str);

const size_t MAX_FILE_NAME = 64;

int main(int argc, const char ** argv)
{
    logStart("disasmlog.txt", LOG_DEBUG);
    logCancelBuffer();


    const char * input_file_name = (argc > 1) ? argv[1] : ("program_code.bin");
    FILE * code_file = fopen(input_file_name, "rb");

    char out_file_name[MAX_FILE_NAME] = "";
    changeEnding(out_file_name, input_file_name, "_DIS.asm");

    FILE * out_file = fopen(out_file_name, "w");
    program_t prog = {};
    progCtor(&prog, code_file, out_file);

    disAsm(&prog);

    progDtor(&prog);
    logExit();
    fclose(code_file);
    fclose(out_file );
    return 0;
}

void changeEnding(char * new_name, const char * old_name, const char * ext_str)
{
    assert(new_name);
    assert(old_name);
    assert(ext_str );

    strcpy(new_name, old_name);
    char * dot = strrchr(new_name, '.');

    if (dot != NULL){
        strcpy(dot, ext_str);
    }
    else {
        size_t len = strlen(new_name);
        strcpy(new_name + len, ext_str);
    }
}
