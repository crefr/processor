#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "assembler.h"
#include "comands.h"
#include "logger.h"

typedef struct {
    char name[ARGMAXLEN];
    int * ip;
} label_t;

typedef struct {
    int * ip;
    size_t label_id;
} fixup_label_t;

const size_t MAX_LABELS_NUM  = 100;
const size_t MAX_FIXUPS_NUM  = 100;

const size_t START_PROG_LEN  = 256;
const size_t SIZE_MULTIPLIER = 2;

const size_t MAX_CMD_LEN = 100;

typedef struct {
    label_t * labels;
    fixup_label_t * fixups;
    size_t fixup_top;
    size_t top;
} label_list_t;

const size_t MAXCMDLEN  = 50;

/// @brief  handles args of push-pop type (ex. 1, [21], rax 5)
static void handlePushPopArgs(program_t * prog);

/// @brief  handles args of jmp commands (jmp, ja, jb, etc.)
static void handleJmpArg(program_t * prog, label_list_t * labels);

/// @brief  handles label that may appear somewhere in code
static void handleLabelInCode(program_t * prog, label_list_t * labels, const char * label_name);

/// @brief  fixing labels as args in jmp commands
static void fixupLabels(program_t * prog, label_list_t * labels);

/// @brief  dumps label list
static void labelDump(program_t * prog, label_list_t * labels);

/// @brief  cuts comments from other string
static void cutCommentIfNeed(char * line, char com_char);

/// @brief  like stricmp but mine
static int myStricmp(const char * first_string, const char * second_string);

/// @brief  resizes array with prog if needed
static void progResizeIfNeed(program_t * prog);

/// @brief  constructs label_list_t
static label_list_t labelListCtor(size_t labels_num, size_t fixups_num);

/// @brief  destructs label_list_t
static void labelListDtor(label_list_t * list);

/// @brief  finds label in list and returns ptr to it
static label_t * findLabelInList(label_list_t * labels, const char * name);

/// @brief  writes header to binary file
static void writeHeader(FILE * out_file, size_t prog_size);

program_t progCtor(FILE * in_file, FILE * out_file, FILE * out_text_file)
{
    assert(in_file);
    assert(out_file);
    assert(out_text_file);
    program_t prog = {};
    prog.program = (int *)calloc(START_PROG_LEN, sizeof(int));
    prog.size = START_PROG_LEN;
    prog.ip = prog.program;
    prog.in_file  = in_file;
    prog.out_file = out_file;
    prog.out_text_file = out_text_file;
    prog.line_ptr = NULL;
    return prog;
}

void progDtor(program_t * prog)
{
    free(prog->program);
    prog->program = NULL;
}

#define DEF_CMD_(cmd_name, num, handle_arg, ...)        \
    if (strcasecmp(#cmd_name, cmd_buf) == 0){           \
        *(prog->ip) = num;                              \
        handle_arg;                                     \
        prog->ip++;                                     \
    }                                                   \
    else

size_t assembleRun(program_t * prog)
{
    assert(prog);
    label_list_t labels = labelListCtor(MAX_LABELS_NUM, MAX_FIXUPS_NUM);
    char curr_str[MAX_LINE_LEN] = "";
    size_t line_count = 1;
    while (1){
        char cmd_buf[MAX_LINE_LEN] = "";
        int scanned_chars = 0;
        if (prog->line_ptr == NULL || sscanf(prog->line_ptr, " %s %n", cmd_buf, &scanned_chars) <= 0){
            int first_nl = 0;
            int last_nl = 0;
            if (fscanf(prog->in_file, "%[^\n]%n%*[\n]%n", curr_str, &first_nl, &last_nl) > 0){
                logPrint(LOG_DEBUG_PLUS, "scanned str #%04zu: \"%s\"\n", line_count, curr_str);
                cutCommentIfNeed(curr_str, COMMENT_CHAR);
                prog->line_ptr = curr_str;
                line_count += (size_t)(last_nl - first_nl);
                continue;
            }
            else
                break;
        }
        prog->line_ptr += scanned_chars;
        progResizeIfNeed(prog);

        if (strchr(cmd_buf, ':') != NULL){
            handleLabelInCode(prog, &labels, cmd_buf);
            continue;
        }
/**************************CRINGE_START*******************************/
        #include "def_commands.h"
        /*else*/{
            PRINTFANDLOG(LOG_RELEASE, "SYNTAX ERROR: \"%s\" in command %zu on the line %zu\n",
                        cmd_buf, (size_t)(prog->ip - prog->program), line_count);
            return 0;
        }
/**************************CRINGE_END*********************************/
    }
    labelDump(prog, &labels);
    fixupLabels(prog, &labels);
    labelDump(prog, &labels);

    labelListDtor(&labels);
    prog->size = (size_t)(prog->ip - prog->program);
    return prog->size;
}
#undef DEF_CMD_

static void cutCommentIfNeed(char * line, char com_char)
{
    assert(line);
    char * com_ptr = strchr(line, com_char);
    if (com_ptr != NULL)
        *com_ptr = '\0';
}

static void handlePushPopArgs(program_t * prog)
{
    assert(prog);
    int reg_arg   = 0;

    const size_t MAX_STR_LEN = 511;
    char scanned_str[MAX_STR_LEN + 1] = "";
    char str[MAX_STR_LEN + 1] = "";
    char reg_str[ARGMAXLEN + 1] = "";
    int cmd_code = *(prog->ip);

    int scanned_chars = 0;
    // sscanf(prog->line_ptr, "%[^] %n", scanned_str, &scanned_chars);
    strcpy(scanned_str, prog->line_ptr);
    scanned_chars = strlen(prog->line_ptr);
    prog->line_ptr += scanned_chars;

    if (sscanf(scanned_str, " [%[^]] ", str) > 0) {;
        cmd_code |= MEM_MASK;
        strcpy(scanned_str, str);
    }
    int scanned_chs = 0;
    float fl_imm_arg = 0;
    if (sscanf(scanned_str, "%g%n", &fl_imm_arg, &scanned_chs) > 0){
        cmd_code |= IMM_MASK;
        *(prog->ip++) = cmd_code;
        *prog->ip     = (int)(fl_imm_arg * ACC_COEF);
    }
    else{
        sscanf(scanned_str, " %s%n ", reg_str, &scanned_chs);
        cmd_code |= REG_MASK;
        reg_arg = toupper(reg_str[1]) - 'A' + 1;
        if (sscanf(scanned_str + scanned_chs, "%g", &fl_imm_arg) > 0){
            cmd_code |= IMM_MASK;
            *(prog->ip++) = cmd_code;
            *(prog->ip++) = reg_arg;
            *prog->ip     = (int)(fl_imm_arg * ACC_COEF);
        }
        else{
            *(prog->ip++) = cmd_code;
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
    header_t head = {};
    head.sign = SIGNATURE;
    head.size = prog_size;
    head.version = COMMAND_VERSION;
    fwrite(&head, sizeof(head), 1, out_file);
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

static label_list_t labelListCtor(size_t labels_num, size_t fixups_num)
{
    label_list_t list = {};
    list.labels =       (label_t *)calloc(labels_num, sizeof(label_t));
    list.fixups = (fixup_label_t *)calloc(fixups_num, sizeof(fixup_label_t));

    return list;
}

static void labelListDtor(label_list_t * list)
{
    assert(list);
    assert(list->labels);
    assert(list->fixups);

    free(list->labels);
    list->labels = NULL;

    free(list->fixups);
    list->fixups = NULL;
}

static void handleJmpArg(program_t * prog, label_list_t * labels)
{
    assert(prog);
    assert(labels);
    char label_str[ARGMAXLEN] = "";

    int scanned_chars = 0;
    sscanf(prog->line_ptr, " %s %n", label_str, &scanned_chars);

    prog->line_ptr += scanned_chars;

    prog->ip++;
    if (strchr(label_str, ':') != NULL){
        logPrint(LOG_DEBUG, "found lable in jmp: %s\n", label_str);
        label_t * label = NULL;
        if ((label = findLabelInList(labels, label_str)) != NULL){
            logPrint(LOG_DEBUG, "\tfound lable %s in list\n", label_str);
            *prog->ip = (int) (label->ip - prog->program);
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
    assert(prog);
    assert(labels);
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

static int myStricmp(const char * first_string, const char * second_string)
{
    assert(first_string);
    assert(second_string);
    int res = 0;
    while ((res = toupper(*first_string) - toupper(*second_string)) == 0 && *first_string != '\0'){
         first_string++;
        second_string++;
    }
    return res;
}

static void progResizeIfNeed(program_t * prog)
{
    assert(prog);
    size_t ip_index = (size_t)(prog->ip - prog->program);
    if (prog->size - ip_index < MAX_CMD_LEN){
        prog->size *= SIZE_MULTIPLIER;
        prog->program = (int *)realloc(prog->program, prog->size * sizeof(int));
        prog->ip = prog->program + ip_index;
    }
}
