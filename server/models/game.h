#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_

#include <stdbool.h> 
#include <pthread.h> 

#include "../servercore/servercore.h"
#include "snake.h"

#define EMPTY           0
#define FRUIT           100 
#define MAX_PLAYERS     1000

typedef struct game 
{
    pthread_mutex_t lock;
    size_t map[GAME_HEIGHT][GAME_WIDTH];
    size_t num_players;
    bool is_running;
} game_t;

game_t *create_game(void);

void remove_player(game_t *game, snake_t *snake);

snake_t *add_player(game_t *game, int player_num);

void add_fruit(game_t *game);

void update_map_coordinate(game_t *game, coordinate_t const *coord, int value);

int value_at_coordindate(game_t const *game, coordinate_t const *coord);

bool is_coord_valid(game_t const *game, coordinate_t const *coord, int type);

bool is_isolated(game_t const *game, coordinate_t const *coord);

void reset_game(game_t *game);

#endif