#ifndef INCLUDED_SNAKE_
#define INCLUDED_SNAKE_

#include "../shared/shared.h"

typedef struct snake 
{
    size_t player_num;
    size_t length;
    coordinate_t head;
    coordinate_t tail;
    coordinate_t body[MAX_SNAKE_LEN];
} snake_t;

snake_t *create_snake(size_t player_num);

void destroy_snake(snake_t *snake);

void move_snake(snake_t *snake, direction_t direction);

void feed_snake(snake_t *snake);

#endif