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
    size_t map[MAP_HEIGHT][MAP_WIDTH];
    size_t num_players;
    bool is_running;
    snake_t *snakes[MAX_PLAYERS];
    size_t snakes_size;
} game_t;

typedef enum player_status
{
    PLAYING,
    WINNER,
    DEAD
} player_status_t;

game_t *create_game(void);

void remove_player(game_t *game, snake_t *snake);

snake_t *add_player(game_t *game, int player_num);

player_status_t move_player(game_t *game, snake_t *snake);

void add_fruit(game_t *game);

void update_map_coordinate(game_t *game, coordinate_t const *coord, int value);

int value_at_coordindate(game_t const *game, coordinate_t const *coord);

bool is_collided(game_t *game, coordinate_t *snake_head);

bool is_outside_border(coordinate_t *coord);

bool is_coord_valid(game_t const *game, coordinate_t const *coord, int type);

bool is_isolated(game_t const *game, coordinate_t const *coord);

bool is_direction_valid(direction_t current_dir, direction_t next_dir);

void reset_game(game_t *game);

#endif