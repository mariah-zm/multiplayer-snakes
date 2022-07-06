#include "core.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void log(log_type_t type, char *msg)
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
    log(ERROR, msg);
    fflush(stdout);
    exit(1);
}

void make_detached_thread(void* (*fn)(void *), void* arg)
{
    pthread_t       tid;
    pthread_attr_t  attr;

    if (pthread_attr_init(&attr) != 0)
        print_error("Failed to initialise thread");

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) == 0)
        pthread_create(&tid, &attr, fn, arg);
    else
        pthread_attr_destroy(&attr);
}