#include "core.h"

#include <stdio.h>
#include <stdlib.h>

void print_error(char *msg)
{
    perror(msg);
}

void exit_error(char *msg)
{
    print_error(msg);
    fflush(stdout);
    exit(1);
}