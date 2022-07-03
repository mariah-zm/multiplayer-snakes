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

#define MAP_DATA        0
#define P_NUM_DATA      1
#define SCORE_DATA      2
#define ENDGAME_DATA    3
#define DEAD_DATA       4
#define MSG_DATA        5

#define MSG_SIZE        25

#include <stddef.h>

typedef int32_t game_map_t[MAP_HEIGHT][MAP_WIDTH];

typedef union data 
{
    game_map_t *map;
    int32_t player_num;
    int32_t score; 
    int32_t is_winner;
    char msg[MSG_SIZE];
} data_t;

typedef struct network_data
{
    int32_t type;
    data_t data;
} network_data_t;

void print_error(char *msg);

void exit_error(char *msg);

#endif