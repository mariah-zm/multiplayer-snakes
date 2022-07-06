#include "servercore.h"

#include <stdlib.h>

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
        case 3:    
        default:            // Required for no warnings       
            return RIGHT;
    }
}

coordinate_t get_random_coordinate(void)
{
    coordinate_t coord;
    // Generating random location away from border
    coord.x = rand() % (MAP_WIDTH - 4) + 2;
    coord.y = rand() % (MAP_HEIGHT - 4) + 2;
    coord.direction = get_random_direction();

    return coord;
}