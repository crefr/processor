#ifndef LOGGER_INCLUDED
#define LOGGER_INCLUDED

/// @brief different levels of logging, IT IS NECESSARY TO WRITE THEM IN ASCENDING ORDER
enum loglevels{LOG_RELEASE, LOG_DEBUG, LOG_DEBUG_PLUS};

#define LOGPRINTWITHTIME(loglevel, ...)          \
        do{                                      \
                logPrintTime(loglevel);          \
                logPrint(loglevel, __VA_ARGS__); \
        }while(0)

#define LOGPRINT(loglevel, ...)                  \
        do{                                      \
                logPrint(loglevel, __VA_ARGS__); \
        }while(0)

#define LOGPRINTERROR(loglevel, ...)             \
        do{                                      \
                logPrintTime(loglevel);          \
                logPrint(loglevel, __VA_ARGS__); \
                logExit();                       \
        }while(0)

#define PRINTFANDLOG(loglevel, ...)              \
        do{                                      \
                logPrint(loglevel, __VA_ARGS__); \
                printf(__VA_ARGS__);             \
                printf("\n");                    \
        }while(0)

int  logStart(const char * logfile, enum loglevels loglevel);
void logPrint(enum loglevels loglevel, const char * fmt, ...);
void logPrintTime(enum loglevels loglevel);
void logExit(void);

void logCancelBuffer();
enum loglevels logGetLevel();


#endif
