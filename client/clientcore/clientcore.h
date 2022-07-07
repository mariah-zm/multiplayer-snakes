/**
 * @file   clientcore.h
 * @author Mariah Zammit
 * @brief  Contains type definitions and functions that are available to all 
 *         files in the game client
 */

#ifndef INCLUDED_CLIENTCORE_
#define INCLUDED_CLIENTCORE_

#include "../../core/core.h"

#include <ncurses.h>

typedef struct game_data
{
    int client_socket;
    game_map_t map;
    int player_status;
    size_t player_num;
    WINDOW *window;
} game_data_t;

#endif