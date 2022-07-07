/**
 * @file   core.h
 * @author Mariah Zammit
 * @brief  For data, types and functions shared between server and client
 */
#ifndef INCLUDED_CORE_
#define INCLUDED_CORE_

#define PORT            5566

#define MAP_D_HEIGHT    24      // extra row to hold data in it
#define MAP_HEIGHT      23      // default height of window - one row for data
#define MAP_WIDTH       80      // default width of window
#define MAP_SIZE        (MAP_D_HEIGHT * MAP_WIDTH * sizeof(int))

// Keyboard keys for directions
#define UP_KEY          'W'
#define LEFT_KEY        'A'
#define DOWN_KEY        'S'
#define RIGHT_KEY       'D'

// Game Map Values
#define EMPTY           0
#define FRUIT           100 

// Indices for data elements
#define DATA_ROW            (MAP_HEIGHT) // 23
#define SCORE_IDX           0            
#define PNUM_IDX            1           // Dead if value at idx is -1
#define WINNER_IDX          2

#include <stddef.h>

typedef int game_map_t[MAP_D_HEIGHT][MAP_WIDTH];

typedef enum log_type
{
    ERROR,
    INFO
} log_type_t;

void exit_error(char *msg);

void make_detached_thread(void* (*fn)(void *), void* arg);

void logger(log_type_t type, char *msg);

#endif