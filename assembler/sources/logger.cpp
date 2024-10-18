#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "logger.h"

static int LOGlevel = 0;
static FILE * LOGfile = NULL;

int logStart(const char * logfilename, int loglevel)
{
    LOGlevel = loglevel;
    LOGfile = fopen(logfilename, "a+");
    if (LOGfile == NULL){
        printf(">>> logger ERROR: cannot open logfile\n");
        return 0;
    }
    logPrint(0, "\n<-----------STARTED----------->\n");
    return 1;
}

void logPrint(int loglevel, const char * fmt, ...)
{
    if (loglevel <= LOGlevel){
        //logPrintTime();
        va_list va = {};
        va_start(va, fmt);
        vfprintf(LOGfile, fmt, va);
        //fprintf(LOGfile, "\n");
        va_end(va);
    }
}

void logPrintTime(int loglevel)
{
    if (loglevel <= LOGlevel){
        time_t time_0= time(NULL);
        struct tm *calctime = localtime(&time_0);

        const size_t timestrlen = 100;
        char timestr[timestrlen] = {};

        strftime(timestr, timestrlen, "[%d.%m.%G %H:%M:%S] ", calctime);
        fprintf(LOGfile, "%s", timestr);
    }
}

void logExit()
{
    logPrint(0, "<-----------ENDING------------>\n");
    fclose(LOGfile);
}
