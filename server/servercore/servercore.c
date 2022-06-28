#include "servercore.h"

direction_t get_random_direction(void)
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
        default:            // case 3
            return RIGHT;
    }
}

coordinate_t get_random_coordinate(void)
{
    coordinate_t coord;
    coord.x = rand() % GAME_WIDTH;
    coord.y = rand() % GAME_HEIGHT;
    coord.direction = get_random_direction();

    return coord;
}