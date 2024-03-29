#include "game.h"

#include <string.h>
#include <stdlib.h>

// Private functions declarations
void update_map_coordinate(game_t *game, coordinate_t coord, int value);
int value_at_coordindate(game_map_t map, coordinate_t coord);
direction_t get_random_direction(void);
coordinate_t get_random_coordinate(void);
bool is_coord_valid(game_map_t map, coordinate_t coord, int type);
bool is_head_valid(game_map_t map, coordinate_t head);
bool is_collided(game_map_t map, coordinate_t snake_head);
bool is_outside_border(coordinate_t coord);

game_t *create_game(void)
{
    game_t *game = (game_t *) malloc(sizeof(game_t));

    // Fill map with empty values
    memset(&game->map, EMPTY, MAP_SIZE);

    game->is_running = true;
    game->winner = 0;
    game->num_players = 0;

    if (pthread_mutex_init(&game->lock, NULL) != 0)
        exit_error("Failed to initialise mutex for game map");

    return game;
}

void destroy_game(game_t *game)
{
    pthread_mutex_destroy(&game->lock);
    free(game);
    game = NULL;
}

void reset_game(game_t *game)
{
    // Resetting map with empty values
    memset(&game->map, EMPTY, MAP_SIZE);

    game->is_running = true;
    game->winner = 0;
    game->num_players = 0;
}

void add_fruit(game_t *game)
{
    coordinate_t coordinate = get_random_coordinate();

    while (!is_coord_valid(game->map, coordinate, FRUIT))
        coordinate = get_random_coordinate();
    
    update_map_coordinate(game, coordinate, FRUIT);
}


snake_t *add_player(game_t *game, int player_num)
{
    // Getting a valid position of snake's head
    coordinate_t head = get_random_coordinate();

    while (!is_coord_valid(game->map, head, player_num)
            || !is_head_valid(game->map, head))
        head = get_random_coordinate();

    snake_t *snake = create_snake(player_num, head);

    // Placing snake on map
    update_map_coordinate(game, snake->head, -snake->player_num);
    update_map_coordinate(game, snake->tail, snake->player_num);

    pthread_mutex_lock(&game->lock);
    ++game->num_players;
    pthread_mutex_unlock(&game->lock);

    return snake;
}

void remove_player(game_t *game, snake_t *snake)
{
    // Removing snake from map
    if (!is_outside_border(snake->head)) 
        update_map_coordinate(game, snake->head, EMPTY);
    update_map_coordinate(game, snake->tail, EMPTY);
    for (size_t idx = 0; idx < snake->length-2; ++idx)
        update_map_coordinate(game, snake->body[idx], EMPTY);

    destroy_snake(snake);

    pthread_mutex_lock(&game->lock);
    --game->num_players;
    pthread_mutex_unlock(&game->lock);
}

void move_player(game_t *game, snake_t *snake)
{
    coordinate_t old_tail = snake->tail;
    move_snake(snake);

    if (is_collided(game->map, snake->head))
    {
        snake->status = DEAD;
        update_map_coordinate(game, old_tail, EMPTY);
        return;
    }
    else if (value_at_coordindate(game->map, snake->head) == FRUIT)
        feed_snake(snake);
    else
        // Updating value of old tail if it hasn't eaten, rest of body is same
        update_map_coordinate(game, old_tail, EMPTY);

    // Updating value at new and old head in any case because always moves forward
    update_map_coordinate(game, snake->head, -snake->player_num);
    update_map_coordinate(game, snake->body[0], snake->player_num);
    update_map_coordinate(game, old_tail, EMPTY);

    // Checking if player is a winner
    if (snake->length == MAX_SNAKE_LEN)
    {
        snake->status = WINNER;
        game->winner = snake->player_num;
        game->is_running = false;
    }       
}

// Function for a snake to eat a fruit in front of it
void feed_player(game_t *game, snake_t *snake)
{
    // TODO Change logic for how this is called after server communication is done
    move_snake(snake);
    feed_snake(snake);

    update_map_coordinate(game, snake->head, -snake->player_num);
    update_map_coordinate(game, snake->body[0], snake->player_num);
}

/*****************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

void update_map_coordinate(game_t *game, coordinate_t coord, int value)
{
    pthread_mutex_lock(&game->lock);
    game->map[coord.y][coord.x] = value;
    pthread_mutex_unlock(&game->lock);
}

int value_at_coordindate(game_map_t map, coordinate_t coord)
{
    return map[coord.y][coord.x];
}

direction_t get_random_direction(void)
{
    int rand_num = rand() % 4;

    switch (rand_num)
    {
        case 0:
            return UP;
        case 1:
            return LEFT;
        case 2:
            return DOWN;
        case 3:    
        default:            // Required for no warnings       
            return RIGHT;
    }
}

coordinate_t get_random_coordinate(void)
{
    coordinate_t coord;
    // Generating random location away from border
    coord.x = rand() % (MAP_WIDTH - 4) + 2;
    coord.y = rand() % (MAP_HEIGHT - 4) + 2;
    coord.direction = get_random_direction();

    return coord;
}

bool is_coord_valid(game_map_t map, coordinate_t coord, int type)
{
    int value = value_at_coordindate(map, coord);

    if (value != EMPTY)
        return false; 

    return true;
}

bool is_head_valid(game_map_t map, coordinate_t head)
{
    switch (head.direction)
    {
        case UP: 
            return map[head.y+1][head.x] == EMPTY;
        case DOWN: 
            return map[head.y-1][head.x] == EMPTY;
        case RIGHT: 
            return map[head.y][head.x-1] == EMPTY;
        case LEFT:
            return map[head.y][head.x+1] == EMPTY;
        default: 
            return false;      // Required for no warnings
    }
}

bool is_direction_valid(direction_t current_dir, direction_t next_dir)
{
    if ((current_dir == DOWN && next_dir != UP)
            || (current_dir == UP && next_dir != DOWN)
            || (current_dir == RIGHT && next_dir != LEFT)
            || (current_dir == LEFT && next_dir != RIGHT))
        return true;
    
    return false;
}

bool is_collided(game_map_t map, coordinate_t snake_head)
{
    if (is_outside_border(snake_head))
        return true;
    else 
    {
        int value_at_head = value_at_coordindate(map, snake_head);
        return value_at_head != FRUIT && value_at_head != EMPTY;
    }
        
}

bool is_outside_border(coordinate_t coord)
{
    return coord.x < 0 || coord.x >= MAP_WIDTH 
            || coord.y < 0 || coord.y >= MAP_HEIGHT;
}