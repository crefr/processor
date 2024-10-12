#ifndef ASSEMBLER_INCLUDED
#define ASSEMBLER_INCLUDED

size_t assembleRun(FILE * _file, int * program);
void progToText(FILE * out_file, int * program, size_t prog_size);

#endif
