#include "game.h"

// TODO synchronise game for different threads

void game_loop(game_t *game)
{
    
}

game_t *create_game()
{
    game_t *game = malloc(sizeof(game_t));
    game->num_players = 0;
    game->is_running = true;

    return game;
}

void remove_snake(game_t *game, snake_t *snake)
{
    // Removing snake from map
    game->map[snake->head.y][snake->head.x] = 0;
    game->map[snake->tail.y][snake->tail.x] = 0;
    for (size_t idx = 0; idx < snake->length-2; ++idx)
        game->map[snake->body[idx].y][snake->body[idx].x] = 0;

    destroy_snake(snake);

    --game->num_players;
}

void place_snake(game_t *game, snake_t *snake)
{
    // Adding snake to map
    game->map[snake->head.y][snake->head.x] = snake->player_num;
    game->map[snake->tail.y][snake->tail.x] = snake->player_num;
    for (size_t idx = 0; idx < snake->length-2; ++idx)
        game->map[snake->body[idx].y][snake->body[idx].x] = snake->player_num;
}

void update_map_coordinate(game_t *game, coordinate_t *coord, int value)
{

}

void reset_game(game_t *game)
{
    game->num_players = 0;
    game->is_running = true;
}