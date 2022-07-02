#include "gameserver.h"

#include <pthread.h>
#include <time.h>

// PRIVATE FUNCTIONS NOT INCLUDED IN THE API
void *fruit_thread_fn(void *arg);
void *move_thread_fn(void *arg);
void accept_clients(game_server_data_t *server_data);
bool send_one(network_data_t *data, int client_fd);
void send_all(game_server_data_t *server_data);
void *handle_client_connection(void *player_fd);
void make_detached_thread(void* (*fn)(void *), void* arg);
bool send_to_client(char *buffer, size_t buffer_size, int client_fd);
bool send_value(int32_t value, int client_fd);
bool send_map(game_map_t *map, int client_fd);

void init_game_server(game_server_data_t *server_data)
{
    // Create server socket
    if ((server_data->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("ERR: could not open server socket");

    // Initialize socket structure
    memset(&server_data->server_addr, 0, sizeof(server_data->server_addr));
    server_data->server_addr.sin_family = AF_INET;
    server_data->server_addr.sin_addr.s_addr = INADDR_ANY;
    server_data->server_addr.sin_port = htons(PORT);

    // Bind the host address
    if (bind(server_data->server_socket_fd, 
                (struct sockaddr *) &server_data->server_socket_fd, 
                sizeof(server_data->server_socket_fd)) < 0) 
    {
		close(server_data->server_socket_fd);
        exit_error("ERR: could not bind server socket");
	}

    // Start listening for clients
    listen(server_data->server_socket_fd, 5);
}

void close_game_server(game_server_data_t *server_data)
{
    close(server_data->server_socket_fd);
}

void start_game(game_server_data_t *server_data)
{
    game_t *game = server_data->game;

    // Start thread to add fruit at random intervals
    make_detached_thread(&fruit_thread_fn, game);
    // Start thread to move snakes every 1s
    make_detached_thread(&move_thread_fn, game);

    while (true)
    {
        // Accept new clients while game is running
        while (game->is_running)
        {
            accept_clients(server_data);
        }

        // If game stops running it means someone has wonunion data 
    {
        
    }
        // TODO send winner

        // Game needs to be reset
        reset_game(game);

        // TODO re add snakes for all connected clients

        // TODO send winner

        // Resend new copy of map to all connected clients
        send_all(server_data);
    }
}

void *fruit_thread_fn(void *arg)
{
    game_t *game = (game_t *) arg;

    while (game->is_running)
    {
        add_fruit(game);

        // Creating random intervals between 0 and 2 seconds
        int milli_seconds = rand() % 2000;
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = milli_seconds * 1000000L;
        nanosleep(&ts, NULL);
    }
}

void *move_thread_fn(void *arg)
{
    game_server_data_t *server_data = (game_server_data_t *) arg;
    game_t *game = server_data->game;

    while (game->is_running)
    {
        for (size_t i = 0; i < game->snakes_size; ++i)
        {    
            snake_t *snake = game->snakes[i];
            if (snake != NULL)
            {
                player_status_t status = move_player(game, snake);

                if (status == WINNER)
                    game->is_running = false;
                else if (status == DEAD)
                    remove_player(game, snake);
            }
        }
        
        // Send updated map to each client after all of them move
        send_all(server_data);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 800000000L; // every 0.8s, snakes move
        nanosleep(&ts, NULL);
    }
}

void accept_clients(game_server_data_t *server_data)
{ 
    size_t client_num = 0;
    socklen_t client_len = sizeof(struct sockaddr_in);

    while (server_data->game->is_running)
    {
        // Accept incoming connection
        server_data->client_socket_fds[client_num] 
            = accept(server_data->server_socket_fd, 
                (struct sockaddr *) &server_data->client_socket_fds[client_num], 
                &client_len);
        
        if (server_data->client_socket_fds[client_num] < 0) 
            print_error("ERR: could not accept connection request");
        else 
        {
            // Handle game loop for player in separate thread
            make_detached_thread(&handle_client_connection, 
                                    &server_data->client_socket_fds[client_num]); 

            ++client_num;
        }
    }
}

bool send_to_client(char *buffer, size_t buffer_size, int client_fd)
{
    size_t bytes_sent = 0;

    while (bytes_sent < buffer_size)
    {
        bytes_sent += write(client_fd, buffer, buffer_size);

        if (bytes_sent < 0)
            return false;
    }

    return true;
}

bool send_map(game_map_t *map, int client_fd)
{    
    size_t const SIZE = MAP_SIZE * sizeof(map[0][0]);
    char map_buffer[SIZE];
    memcpy(map_buffer, map, SIZE);

    return send_to_client(map_buffer, SIZE, client_fd);
}

bool send_value(int32_t value, int client_fd)
{
    size_t const SIZE = sizeof(value);
    char buffer[SIZE];
    memset(buffer, 0, SIZE);
    memcpy(buffer, value, SIZE);

    return send_to_client(buffer, SIZE, client_fd);
}

bool send_one(network_data_t *data, int client_fd)
{
    // Sending type of data being sent first
    if (!send_value(data->type, client_fd))
        return false;

    switch (data->type)
    {
        case MAP_DATA:
            return send_map(data->data.map, client_fd);
        case PLAYER_NUM:
            return send_value(data->data.player_num, client_fd);
        case SCORE_DATA:
            return send_value(data->data.score, client_fd);
        case ENDGAME_DATA:
            return send_value(data->data.is_winner, client_fd);
        case MSG_DATA:
            return send_to_client(data->data.msg, MSG_SIZE, client_fd);
        default: return false;  // Required for no warnings
    }
}

void send_all(game_server_data_t *server_data)
{
    char map_buffer[MAP_SIZE];
    memcpy(map_buffer, server_data->game->map, MAP_SIZE);

    for (int i = 0; i < server_data->game->snakes_size; ++i)
    {
        if (server_data->game->snakes[i] != NULL)
            send_one(map_buffer, server_data->client_socket_fds[i]);
    }
}

void *handle_client_connection(void *arg)
{
    char key_buffer;
    direction_t new_dir;

    client_conn_data_t conn_data = *(client_conn_data_t *) arg;
    int client_fd = conn_data.client_socket_fd;
    game_t *game = conn_data.game;

    snake_t *snake = add_player(game, client_fd);

    // Send map to client (player)
    char map_buffer[MAP_SIZE];
    memcpy(map_buffer, game->map, MAP_SIZE);
    send_one(map_buffer, client_fd);

    while (true)
    {
        memset(key_buffer, 0, 1);

        // Reading key from client
        if (read(client_fd, key_buffer, 1) < 0)
        {
            remove_player(game, snake);
            close(client_fd);
            print_error("ERROR reading from socket");
            break;
        }

        new_dir = toupper(key_buffer);
        
        // Disconnect player
        if (new_dir == 'Q')
        {
            remove_player(game, snake);
            close(client_fd);
            break;
        }
        // Changing direction if entered key is valid
        else if (is_direction_valid(snake->head.direction, new_dir))
            change_snake_direction(snake, new_dir);
    }
}

void make_detached_thread(void* (*fn)(void *), void* arg)
{
    pthread_t       tid;
    pthread_attr_t  attr;

    if (pthread_attr_init(&attr) != 0)
        print_error("ERR: failed to initialise thread");

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) == 0)
        pthread_create(&tid, &attr, fn, arg);
    else
        pthread_attr_destroy(&attr);
}