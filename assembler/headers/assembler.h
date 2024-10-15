#ifndef ASSEMBLER_INCLUDED
#define ASSEMBLER_INCLUDED

typedef struct {
    FILE * in_file;
    FILE * out_file;
    FILE * out_text_file;
    int * ip;
    size_t size;
    int * program;
}program_t;

program_t progCtor(int * program, FILE * in_file, FILE * out_file, FILE * out_text_file);
void progToCode(program_t * prog);
void progToText(program_t * prog);
size_t assembleRun(program_t * prog);

#endif
