#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_

#include<stdbool.h>  

#include "../servercore/servercore.h"
#include "snake.h"

#define FRUIT           100 
#define MAX_PLAYERS     1000

typedef struct game 
{
    size_t map[GAME_HEIGHT][GAME_WIDTH];
    size_t num_players;
    bool is_running;
} game_t;

void game_loop(game_t *game);

game_t *create_game();

void remove_snake(game_t *game, size_t player_num);

void place_snake(game_t *game, snake_t *snake);

void add_fruit_callback();

void update_map_coordinate(game_t *game, coordinate_t *coord, int value);

void reset_game(game_t *game);

#endif