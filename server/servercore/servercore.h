#ifndef INCLUDED_SERVERCORE_
#define INCLUDED_SERVERCORE_

#include <stddef.h>

#include "../core/core.h"

#define MAX_SNAKE_LEN   (GAME_HEIGHT*GAME_WIDTH)

typedef enum direction
{
    UP = UP_KEY, 
    LEFT = LEFT_KEY,
    DOWN = DOWN_KEY,  
    RIGHT = RIGHT_KEY
} direction_t;

typedef struct coordinate 
{
    size_t x;
    size_t y;
    direction_t direction;
} coordinate_t;

direction_t get_random_direction();

coordinate_t get_random_coordinate();

#endif