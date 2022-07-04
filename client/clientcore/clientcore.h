#ifndef INCLUDED_CLIENTCORE_
#define INCLUDED_CLIENTCORE_

#include "../../core/core.h"

#include <ncurses.h>

typedef struct game_data
{
    game_map_t *map;
    size_t score;
    size_t player_num;
    WINDOW *window;
} game_data_t;

#endif