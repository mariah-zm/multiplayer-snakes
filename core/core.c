#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void logger(log_type_t type, char *msg)
{
    switch (type)
    {
        case ERROR:
            fprintf(stderr, "ERR: %s\n", msg);
            break;

        case INFO:
            printf("INFO: %s\n", msg);
            break;
        
        default:
            break;
    }
}

void exit_error(char *msg)
{
    logger(ERROR, msg);
    fflush(stdout);
    exit(1);
}