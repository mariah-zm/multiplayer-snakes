/**
 * @file   core.h
 * @author Mariah Zammit
 * @brief  For data, types and functions shared between server and client
 */
#ifndef INCLUDED_CORE_
#define INCLUDED_CORE_

#define PORT            7000

#define MAP_HEIGHT      23    // default height of window - one line for score
#define MAP_WIDTH       80
#define MAP_SIZE        (MAP_HEIGHT * MAP_WIDTH * sizeof(int))

// Keyboard keys for directions
#define UP_KEY          'W'
#define LEFT_KEY        'A'
#define DOWN_KEY        'S'
#define RIGHT_KEY       'D'

// Game Map Values
#define EMPTY           0
#define FRUIT           100 

#include <stddef.h>

typedef int game_map_t[MAP_HEIGHT][MAP_WIDTH];

void print_error(char *msg);

void exit_error(char *msg);

void make_detached_thread(void* (*fn)(void *), void* arg);

#endif