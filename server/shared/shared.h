#ifndef INCLUDED_SHARED_
#define INCLUDED_SHARED_

#include <stddef.h>

#define GAME_HEIGHT     24    // default size of terminal window in num of chars
#define GAME_WIDTH      80

#define UP_KEY              'W'
#define LEFT_KEY            'A'
#define DOWN_KEY            'S'
#define RIGHT_KEY           'D'

#define MAX_SNAKE_LEN GAME_HEIGHT * GAME_WIDTH;

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