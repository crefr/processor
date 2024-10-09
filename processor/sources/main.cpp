#include <stdio.h>
#include <stdlib.h>

#include "processor.h"
#include "logger.h"

int main()
{
    logStart("log.txt", LOG_DEBUG_PLUS);
    processorRun(NULL);
    logExit();
    return 0;
}
