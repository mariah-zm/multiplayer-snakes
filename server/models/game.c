#include "game.h"

game_t *create_game(void)
{
    // Locks are not rquired in this function as this new instance only exists
    // in this function

    game_t *game = malloc(sizeof(game_t));
    game->num_players = 0;
    game->is_running = true;

    if (pthread_mutex_init(&game->lock, NULL) != 0)
        exit_error("ERR: failed to initialise mutex for game map");

    // Setting places in map as all empty
    for (size_t y = 0; y < GAME_HEIGHT; ++y)
        for (size_t x = 0; x < GAME_WIDTH; ++x)
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
    pthread_mutex_unlock(&game->lock);

    return snake;
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

bool is_coord_valid(game_t const *game, coordinate_t const *coord, int type)
{
    int value = value_at_coordindate(game, coord);

    if (value != EMPTY)
        return false;
    // Otherwise it's empty so good
    else if (type == FRUIT)
    {
        // Fruit must not be at border and away from 
        if (coord->x == 0 || coord->x == GAME_WIDTH 
            || coord->y == 0 || coord->y == GAME_HEIGHT
            || !is_isolated(game, coord))
            return false;
    }
    // Otherwise type is a player's number which means coordinate is head
    else 
    {
        // Snake's head must not be facing a border and must be away from
        // other snakes/fruits
        if ((coord->direction == UP && coord->y == 0)
            || (coord->direction == DOWN && coord->y == GAME_HEIGHT)
            || (coord->direction == LEFT && coord->x == 0)
            || (coord->direction == RIGHT && coord->x == GAME_WIDTH)
            || !is_isolated(game, coord))
            return false;
    }

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
    while (y < GAME_HEIGHT && y <= coord->y + 2)
    {
        while (x < GAME_WIDTH && x <= coord->x + 2)
        {
            if (game->map[y][x] != EMPTY)
                return false;
            ++x;
        }
        ++y;
    }
}

void reset_game(game_t *game)
{
    pthread_mutex_lock(&game->lock);

    game->num_players = 0;
    game->is_running = true;
    
    for (size_t y = 0; y < GAME_HEIGHT; ++y)
        for (size_t x = 0; x < GAME_WIDTH; ++x)
            game->map[y][x] = EMPTY;   

    pthread_mutex_unlock(&game->lock);
}