#ifndef INCLUDED_WINDOW_
#define INCLUDED_WINDOW

#include <ncurses.h>
// #include <curses.h>

#include "../../core/core.h"

void init_ncurses();

WINDOW *create_game_window();

size_t show_menu(WINDOW *window);

void show_instructions(WINDOW *window);

void show_winner(WINDOW *window, int player_num);

void show_losing_screen(WINDOW *window);

void update_window(WINDOW *window, game_map_t *map);

void destroy_window(WINDOW *window);

#endif