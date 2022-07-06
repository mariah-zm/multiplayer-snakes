#ifndef INCLUDED_GAME_
#define INCLUDED_GAME_

#include <stdbool.h>
#include <pthread.h>

#include "../../core/core.h"
#include "../servercore/servercore.h"
#include "snake.h"

typedef struct game
{
    game_map_t map;
    pthread_mutex_t lock;
    bool is_running;
    int winner;
} game_t;

game_t *create_game(void);

void add_fruit(game_t *game);

snake_t *add_player(game_t *game, size_t player_num);

void remove_player(game_t *game, snake_t *snake);

void move_player(game_t *game, snake_t *snake);

void feed_player(game_t *game, snake_t *snake);

bool is_direction_valid(direction_t current_dir, direction_t next_dir);

#endif