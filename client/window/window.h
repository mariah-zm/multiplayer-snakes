#ifndef INCLUDED_WINDOW_
#define INCLUDED_WINDOW

#include <ncurses.h>

#include "../core/core.h"

WINDOW *create_window();

void show_welcome_screen(WINDOW *window);

void show_winner(WINDOW *window, int player_num);

void show_losing_screen(WINDOW *window);

void update_window(WINDOW *window, game_map_t *map);

void destroy_window(WINDOW *window);

#endif