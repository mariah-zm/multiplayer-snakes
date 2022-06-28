#include "servercore.h"

direction_t get_random_direction()
{
    int rand_num = rand() % 4;

    switch (rand_num)
    {
        case 0:
            return UP;
        case 1:
            return LEFT;
        case 2:
            return DOWN;
        default:
            return RIGHT;
    }
}

void set_random_coordinate(coordinate_t *coord)
{
    coord->x = rand() % GAME_WIDTH;
    coord->y = rand() % GAME_HEIGHT;
    coord->direction = get_random_direction();
}