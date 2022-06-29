#include "game.h"

game_t *create_game(void)
{
    // Locks are not rquired in this function as this new instance only exists
    // in this function

    game_t *game = malloc(sizeof(game_t));
    game->num_players = 0;
    game->snakes_size = 0;
    game->is_running = true;

    if (pthread_mutex_init(&game->lock, NULL) != 0)
        exit_error("ERR: failed to initialise mutex for game map");

    // Setting places in map as all empty
    for (size_t y = 0; y < MAP_HEIGHT; ++y)
        for (size_t x = 0; x < MAP_WIDTH; ++x)
            game->map[y][x] = EMPTY;

    // Adding 3 fruit
    for (size_t i = 0; i < 3; ++i)
        add_fruit(game);

    return game;
}

void remove_player(game_t *game, snake_t *snake)
{
    // Removing snake from map
    update_map_coordinate(game, &snake->head, EMPTY);
    update_map_coordinate(game, &snake->tail, EMPTY);
    for (size_t idx = 0; idx < snake->length-2; ++idx)
        update_map_coordinate(game, &snake->body[idx], EMPTY);

    destroy_snake(snake);

    pthread_mutex_lock(&game->lock);
    --game->num_players;
    pthread_mutex_unlock(&game->lock);
}

snake_t *add_player(game_t *game, int player_num)
{
    // Getting position of snake's head
    coordinate_t head = get_random_coordinate();

    while (is_coord_valid(game, &head, player_num))
        head = get_random_coordinate();

    // Creating snake
    snake_t *snake = create_snake(player_num, &head);

    // Placing snake on map
    update_map_coordinate(game, &snake->head, snake->player_num);
    update_map_coordinate(game, &snake->tail, snake->player_num);

    pthread_mutex_lock(&game->lock);
    ++game->num_players;
    game->snakes[game->snakes_size] = snake;
    ++game->snakes_size;
    pthread_mutex_unlock(&game->lock);

    return snake;
}

player_status_t move_player(game_t *game, snake_t *snake)
{
    coordinate_t old_tail = snake->tail;

    move_snake(snake);

    if (is_collided(game, &snake->head))
    {
        remove_player(game, snake);
        return DEAD;
    }
    else if (value_at_coordindate(game, &snake->head) == FRUIT)
        feed_snake(snake);
    else
        // Updating value of old tail if it hasn't eaten, rest of body is same
        update_map_coordinate(game, &old_tail, EMPTY);

    // Updating value at new head in any case because always moves forward
    update_map_coordinate(game, &snake->head, snake->player_num);

    if (snake->length == 15)
        return WINNER;

    return PLAYING;    
}

void add_fruit(game_t *game)
{
    coordinate_t coordinate = get_random_coordinate();

    while (!is_coord_valid(game, &coordinate, FRUIT))
        coordinate = get_random_coordinate();
    
    update_map_coordinate(game, &coordinate, FRUIT);
}

void update_map_coordinate(game_t *game, coordinate_t const *coord, int value)
{
    pthread_mutex_lock(&game->lock);
    game->map[coord->y][coord->x] = value;
    pthread_mutex_unlock(&game->lock);
}

int value_at_coordindate(game_t const *game, coordinate_t const *coord)
{
    return game->map[coord->y][coord->x];
}

bool is_collided(game_t *game, coordinate_t *snake_head)
{
    if (is_outside_border(&snake_head))
        return true;
    else 
    {
        int value_at_head = value_at_coordindate(game, snake_head);
        return value_at_head != FRUIT && value_at_head != EMPTY;
    }
        
}

bool is_outside_border(coordinate_t *coord)
{
    return coord->x < 0 || coord->x >= MAP_WIDTH 
            || coord->y < 0 || coord->y >= MAP_HEIGHT;
}

bool is_coord_valid(game_t const *game, coordinate_t const *coord, int type)
{
    int value = value_at_coordindate(game, coord);

    if (value != EMPTY)
        return false;
    // Otherwise it's empty so good
    else 
        // fruit/players must be away from other fruit/players
        return is_isolated(game, coord); 

    return true;
}

bool is_isolated(game_t const *game, coordinate_t const *coord)
{
    size_t y = coord->y;
    size_t x = coord->x;

    // Checking left/up
    while (y >= 0 && y >= coord->y - 2)
    {
        while (x >= 0 && x >= coord->x - 2)
        {
            if (game->map[y][x] != EMPTY)
                return false;
            --x;
        }
        --y;
    }

    y = coord->y;
    x = coord->x;

    // Checking right/down
    while (y < MAP_HEIGHT && y <= coord->y + 2)
    {
        while (x < MAP_WIDTH && x <= coord->x + 2)
        {
            if (game->map[y][x] != EMPTY)
                return false;
            ++x;
        }
        ++y;
    }

    return true;
}

bool is_direction_valid(direction_t current_dir, direction_t next_dir)
{
    if ((current_dir == DOWN && !next_dir == UP)
            || (current_dir == UP && !next_dir == DOWN)
            || (current_dir == RIGHT && !next_dir == LEFT)
            || (current_dir == LEFT && !next_dir == RIGHT))
        return true;
    
    return false;
}

void reset_game(game_t *game)
{
    pthread_mutex_lock(&game->lock);

    game->num_players = 0;
    game->is_running = true;
    
    for (size_t y = 0; y < MAP_HEIGHT; ++y)
        for (size_t x = 0; x < MAP_WIDTH; ++x)
            game->map[y][x] = EMPTY;

    for (size_t i = 0; i < game->snakes_size; ++i)
        free(game->snakes[i]);

    game->snakes_size = 0;

    pthread_mutex_unlock(&game->lock);
}