#ifndef ASSEMBLER_INCLUDED
#define ASSEMBLER_INCLUDED

/// @brief struct the program is in
typedef struct {
    FILE * in_file;
    FILE * out_file;
    FILE * out_text_file;
    int * ip;
    size_t size;
    int * program;
    const char * line_ptr;
}program_t;

const size_t MAX_LINE_LEN = 256;

const char COMMENT_CHAR = ';';
#define COMMENT_CHAR_STR  ";"

/// @brief  constructs struct, the program will be in
program_t progCtor(FILE * in_file, FILE * out_file, FILE * out_text_file);

/// @brief  destructs struct with program
void progDtor(program_t * prog);

/// @brief  writes code from array to file in binary mode
void progToCode(program_t * prog);

/// @brief  writes code as text to file in text mode (for debug)
void progToText(program_t * prog);

/// @brief  function that assembles code from program_t struct
size_t assembleRun(program_t * prog);

#endif
