#ifndef INCLUDED_CLIENTCORE_
#define INCLUDED_CLIENTCORE_

#include "../../core/core.h"

#include <ncurses.h>

#define DEAD            -1
#define PLAYING         1
#define WINNER          2
#define LOSER           3
#define DISCONNECTED    4

typedef struct game_data
{
    int client_socket;
    game_map_t map;
    int player_status;
    size_t score;
    size_t player_num;
    WINDOW *window;
} game_data_t;

#endif