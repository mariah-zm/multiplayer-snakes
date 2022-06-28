#ifndef INCLUDED_SNAKE_
#define INCLUDED_SNAKE_

#include "../servercore/servercore.h"

typedef struct snake 
{
    size_t player_num;
    size_t length;
    coordinate_t head;
    coordinate_t tail;
    coordinate_t body[MAX_SNAKE_LEN-2];
} snake_t;

snake_t *create_snake(size_t player_num);

void destroy_snake(snake_t *snake);

void change_snake_direction(snake_t *snake, direction_t direction);

void move_snake(snake_t *snake);

void feed_snake(snake_t *snake);

#endif