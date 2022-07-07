/**
 * @file   window.h
 * @author Mariah Zammit
 * @brief  Contains functions related to the game graphics and menus, and 
 *         modifying the game window
 */

#ifndef INCLUDED_WINDOW_
#define INCLUDED_WINDOW

#include <ncurses.h>

#include "../../core/core.h"

#define OPT_PLAY    1
#define OPT_INS     2
#define OPT_EXIT    3

#define WIN_HEIGHT  (MAP_HEIGHT + 1)        // first row is for header
#define WIN_WIDTH   (MAP_WIDTH)

// Values of Keys
#define ENTER       10
#define UP_ARROW    'A'      
#define DOWN_ARROW  'B'
#define BACKSPACE   8
#define DELETE      127
#define QUIT        'Q'

// Colour indices
#define MAIN_COL    8
#define TITLE_COL   9
#define OPTIONS_COL 10
#define SCORE_COL   11

void init_ncurses();

WINDOW *create_game_window();

void destroy_game_window(WINDOW *window);

size_t show_menu(WINDOW *window);

void show_instructions(WINDOW *window);

void show_game(WINDOW *window, game_map_t map);

void show_message(WINDOW *window, char *msg);

#endif