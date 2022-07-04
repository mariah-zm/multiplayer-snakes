#ifndef INCLUDED_CLIENTCORE_
#define INCLUDED_CLIENTCORE_

#include "../../core/core.h"

#include <ncurses.h>

#define END     0
#define ONGOING 1

typedef struct game_data
{
    int client_socket;
    game_map_t *map;
    int game_status;
    size_t score;
    size_t player_num;
    WINDOW *window;
} game_data_t;

#endif