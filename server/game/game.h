#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_

#include "../shared/shared.h"
#include "../snake/snake.h"

#define FRUIT           100 
#define MAX_PLAYERS     1000

typedef struct game 
{
    size_t game_map[GAME_HEIGHT][GAME_WIDTH];
    size_t num_players;
    snake_t *players_snakes;
} game_t;

game_t *create_game();

void reset_game(game_t *);

#endif