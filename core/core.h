/**
 * @file   core.h
 * @author Mariah Zammit
 * @brief  For data, types and functions shared between server and client
 */
#ifndef INCLUDED_CORE_
#define INCLUDED_CORE_

#define PORT            5566

#define MAP_HEIGHT      23    // default height of window - one line for score
#define MAP_WIDTH       80
#define MAP_SIZE        (MAP_HEIGHT*MAP_WIDTH)

#define UP_KEY          'W'
#define LEFT_KEY        'A'
#define DOWN_KEY        'S'
#define RIGHT_KEY       'D'

// Game Map Values
#define EMPTY           0
#define FRUIT           100 

#include <stddef.h>

typedef int game_map_t[MAP_HEIGHT][MAP_WIDTH];

typedef struct game_data
{
    game_map_t map;
    size_t player_num;
    size_t score;
} game_data_t;

void print_error(char *msg);

void exit_error(char *msg);

#endif