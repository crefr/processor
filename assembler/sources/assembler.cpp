#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "../../comands.h"
#include "logger.h"

typedef struct {
    char name[ARGMAXLEN];
    int * ip;
} label_t;

typedef struct {
    int * ip;
    size_t label_id;
} fixup_label_t;

const size_t MAX_LABELS_NUM = 10;
const size_t MAX_FIXUPS_NUM = 100;
typedef struct {
    label_t labels[MAX_LABELS_NUM];
    fixup_label_t fixups[MAX_FIXUPS_NUM];
    size_t fixup_top;
    size_t top;
} label_list_t;

const size_t MAXCMDLEN  = 50;

program_t progCtor(int * program, FILE * in_file, FILE * out_file, FILE * out_text_file)
{
    assert(program);
    assert(in_file);
    assert(out_file);
    assert(out_text_file);
    program_t prog = {};
    prog.program = program;
    prog.ip = program;
    prog.in_file  = in_file;
    prog.out_file = out_file;
    prog.out_text_file = out_text_file;
    return prog;
}

static void scanPushArgs(program_t * prog);
static enum commands getCmdByStr(const char * str);
static void handleLableInJmps(program_t * prog, label_list_t * labels);
static void handleLabelInCode(program_t * prog, label_list_t * labels, const char * label_name);
static void fixupLabels(program_t * prog, label_list_t * labels);
static void labelDump(program_t * prog, label_list_t * labels);
size_t assembleRun(program_t * prog)
{
    assert(prog);
    label_list_t labels = {};
    char cmd[MAXCMDLEN] = "";
    while (fscanf(prog->in_file, "%s", cmd) > 0){
        if (strchr(cmd, ':') != NULL){
            handleLabelInCode(prog, &labels, cmd);
            continue;
        }
        enum commands cmd_id = getCmdByStr(cmd);
        switch(cmd_id){
        case PUSH_CMD:{
            scanPushArgs(prog);
            prog->ip++;
            continue;
        }
        case POP_CMD:{
            int pop_cmd_code = POP_CMD | REG_MASK;
            char reg_str[ARGMAXLEN + 1] = "";
            fscanf(prog->in_file, "%s", reg_str);
            int reg_num = reg_str[1] - 'a' + 1;
            *(prog->ip++) = pop_cmd_code;
            *(prog->ip++)     = reg_num;
            continue;
        }
        case JMP_CMD: case JA_CMD:{
            *(prog->ip++) = cmd_id;
            handleLableInJmps(prog, &labels);
            continue;
        }
        case ADD_CMD: case SUB_CMD: case MUL_CMD: case DIV_CMD:{
            *(prog->ip++) = (int) cmd_id;
            continue;
        }
        case OUT_CMD: case IN_CMD:{
            *(prog->ip++) = (int) cmd_id;
            continue;
        }
        case HLT_CMD:
            *(prog->ip++) = (int) cmd_id;
            continue;
        default:
            PRINTFANDLOG(LOG_RELEASE, "SYNTAX ERROR: \"%s\" in command %zu, (scanned as %d)\n",
                        cmd, (size_t)(prog->ip - prog->program), cmd_id);
            return 0;
        }
    }
    labelDump(prog, &labels);
    fixupLabels(prog, &labels);
    labelDump(prog, &labels);
    prog->size = (size_t)(prog->ip - prog->program);
    return prog->size;
}

static enum commands getCmdByStr(const char * str)
{
    assert(str);
    for (size_t cmd_index = 0; cmd_index < Cmd_Num; cmd_index++){
        if (strcmp(str, Commands[cmd_index].name) == 0)
            return Commands[cmd_index].id;
    }
    return ERROR_CMD;
}

static void scanPushArgs(program_t * prog)
{
    assert(prog);
    int digit_arg = 0;
    int reg_arg   = 0;
    char reg_str[ARGMAXLEN + 1] = "";
    int push_cmd_code = PUSH_CMD;

    if (fscanf(prog->in_file, "%d", &digit_arg) != 0){
        push_cmd_code |= DIG_MASK;
        *(prog->ip++) = push_cmd_code;
        *prog->ip     = digit_arg;
    }
    else{
        fscanf(prog->in_file, " %s ", reg_str);
        push_cmd_code |= REG_MASK;
        reg_arg = toupper(reg_str[1]) - 'A' + 1;
        if (fscanf(prog->in_file, "%d", &digit_arg) != 0){
            push_cmd_code |= DIG_MASK;
            *(prog->ip++) = push_cmd_code;
            *(prog->ip++) = reg_arg;
            *prog->ip     = digit_arg;
        }
        else{
            *(prog->ip++) = push_cmd_code;
            *prog->ip     = reg_arg;
        }
    }
}


/***********************************
HEADER FORMAT (byte representation)
c r e f r 00 25 00 00 00 00 00 00 00
^            ^
|            |
signature    num of tokens (size_t)
************************************/
static void writeHeader(FILE * out_file, size_t prog_size)
{
    assert(out_file);
    fwrite(SIGNATURE, sizeof(SIGNATURE), 1, out_file);
    fwrite(&prog_size, sizeof(prog_size), 1, out_file);
}

void progToText(program_t * prog)
{
    assert(prog);
    fprintf(prog->out_text_file, "%08zu\n", prog->size);
    for (size_t ip = 0; ip < prog->size; ip++){
        fprintf(prog->out_text_file, "%d ", prog->program[ip]);
    }
}

void progToCode(program_t * prog)
{
    assert(prog);
    writeHeader(prog->out_file, prog->size);
    fwrite(prog->program, sizeof(int), prog->size, prog->out_file);
}

static label_t * findLabelInList(label_list_t * labels, const char * name);
static void handleLableInJmps(program_t * prog, label_list_t * labels)
{
    assert(prog);
    assert(labels);
    char label_str[ARGMAXLEN] = "";
    fscanf(prog->in_file, " %s ", label_str);

    if (strchr(label_str, ':') != NULL){
        logPrint(LOG_DEBUG, "found lable in jmp: %s\n", label_str);
        label_t * label = NULL;
        if ((label = findLabelInList(labels, label_str)) != NULL){
            logPrint(LOG_DEBUG, "\tfound lable %s in list\n", label_str);
            *prog->ip = (int) (label->ip - prog->program);
            prog->ip++;
        }
        else {
            logPrint(LOG_DEBUG, "\tdid not find lable %s in list, making new (top = %zu)\n", label_str, labels->top);
            *prog->ip = -1;
            strcpy(labels->labels[labels->top].name, label_str);
            labels->labels[labels->top].ip = NULL;

            labels->fixups[labels->fixup_top].ip = prog->ip;
            labels->fixups[labels->fixup_top].label_id = labels->top;

            labels->fixup_top++;
            labels->top++;
            prog->ip++;

        }
    }
    else {
        logPrint(LOG_DEBUG, "found address %s\n");
        char * end = NULL;
        int addr_num = (int) strtol(label_str, &end, 10);
        if (*end != '\0')
            LOGPRINTERROR(LOG_DEBUG, "incorrect address: %s\n", label_str);
        *prog->ip = addr_num;
        prog->ip++;
    }
}
static void handleLabelInCode(program_t * prog, label_list_t * labels, const char * label_name)
{
    assert(prog);
    assert(labels);
    assert(label_name);
    assert(strchr(label_name, ':') != NULL);

    logPrint(LOG_DEBUG, "found label in code: %s, ip = %zu\n", label_name, (size_t)(prog->ip - prog->program));
    label_t * label = findLabelInList(labels, label_name);
    if (label != NULL){
        label->ip = prog->ip;
    }
    else {
        assert(labels->top != MAX_LABELS_NUM);
        strcpy(labels->labels[labels->top].name, label_name);
        labels->labels[labels->top].ip = prog->ip;
        labels->top++;
    }
}
static label_t * findLabelInList(label_list_t * labels, const char * name)
{
    assert(labels);
    assert(name);
    for (size_t index = 0; index < labels->top; index++){
        if (strcmp(labels->labels[index].name, name) == 0)
            return &(labels->labels[index]);
    }
    return NULL;
}

static void fixupLabels(program_t * prog, label_list_t * labels)
{
    assert(prog);
    assert(labels);
    logPrint(LOG_DEBUG, "making fixups, total: %zu\n", labels->fixup_top);
    for (size_t fixup_index = 0; fixup_index < labels->fixup_top; fixup_index++){
        logPrint(LOG_DEBUG, "\tmaking fixup #%zu / %zu, label %s\n",
                fixup_index + 1, labels->fixup_top, labels->labels[labels->fixups[fixup_index].label_id].name);
        int * addr_to_fix = labels->fixups[fixup_index].ip;
        int * new_addr = labels->labels[labels->fixups[fixup_index].label_id].ip;

        *(addr_to_fix) = (int)(new_addr - prog->program);
    }
    logPrint(LOG_DEBUG, "end of fixups\n\n", labels->fixup_top);
}

static void labelDump(program_t * prog, label_list_t * labels)
{
    logPrint(LOG_DEBUG, "\n-----LABELS_DUMP-----\n");
    logPrint(LOG_DEBUG, "num of labels: %zu\n", labels->top);
    logPrint(LOG_DEBUG, "labels:\n");
    for (size_t label_index = 0; label_index < labels->top; label_index++){
        logPrint(LOG_DEBUG, "\tlabel #%zu:\n", label_index + 1);
        logPrint(LOG_DEBUG, "\t\tname: %s\n",  labels->labels[label_index].name);
        logPrint(LOG_DEBUG, "\t\tip:   %d\n", (int)(labels->labels[label_index].ip - prog->program));
    }
    logPrint(LOG_DEBUG, "num of fixups: %zu\n", labels->fixup_top);
    if (labels->fixup_top > 0){
        logPrint(LOG_DEBUG, "fixups:\n");
        for (size_t fixup_index = 0; fixup_index < labels->fixup_top; fixup_index++){
            logPrint(LOG_DEBUG, "\tfixup #%zu:\n", fixup_index + 1);
            logPrint(LOG_DEBUG, "\t\tip:         %d\n", (int)(labels->fixups[fixup_index].ip - prog->program));
            logPrint(LOG_DEBUG, "\t\tlabel id:   %zu\n", labels->fixups[fixup_index].label_id);
            logPrint(LOG_DEBUG, "\t\tlabel name: %s\n" , labels->labels[labels->fixups[fixup_index].label_id].name);
        }
    }

    logPrint(LOG_DEBUG, "---LABELS_DUMP_END---\n\n");
}
