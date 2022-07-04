#include "gameserver.h"

#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// PRIVATE FUNCTIONS NOT INCLUDED IN THE API
void *fruit_thread_fn(void *arg);
void *move_thread_fn(void *arg);
void *accept_clients(void *arg);
bool send_to_client(network_data_t *data, int client_fd);
void *send_map_to_all_clients(void *arg);
void *handle_client_connection(void *player_fd);
bool send_buffer_to_client(char *buffer, size_t buffer_size, int client_fd);
bool send_value_to_client(size_t value, int client_fd);
bool send_map_to_client(game_map_t *map, int client_fd);

void init_game_server(game_server_data_t *server_data)
{
    // Create server socket
    if ((server_data->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("Could not open server socket");

    // Initialize socket structure
    memset(&server_data->server_addr, 0, sizeof(server_data->server_addr));
    server_data->server_addr.sin_family = AF_INET;
    server_data->server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_data->server_addr.sin_port = htons(PORT);

    // Bind the host address
    if (bind(server_data->server_socket_fd, 
                (struct sockaddr *) &server_data->server_addr, 
                sizeof(server_data->server_addr)) < 0) 
    {
		close(server_data->server_socket_fd);
        exit_error("Could not bind server socket");
	}

    // Start listening for clients
    listen(server_data->server_socket_fd, 5);

    printf("INFO: Game server is running on port %d\n", PORT);
}

void close_game_server(game_server_data_t *server_data)
{
    close(server_data->server_socket_fd);
    // TODO close client connections
}

void start_game_server(game_server_data_t *server_data)
{
    game_t *game = server_data->game;

    // Game server is always on
    while (true)
    {
        pthread_t fruitThread, moveThread, networkThread;
        void *threadRet;

        // Start thread to add fruit at random intervals
        if (pthread_create(&fruitThread, NULL, fruit_thread_fn, game) != 0)
        {
            print_error("Failed to start fruit thread in game server");
            return;
        }

        // Start thread to move snakes every 0.8s
        if (pthread_create(&moveThread, NULL, move_thread_fn, server_data) != 0)
        {
            print_error("Failed to start move thread in game server");

            if (pthread_join(fruitThread, &threadRet) != 0)
                print_error("Failed to join fruit thread in game server");

            return;
        }

        // Start thread to accept incoming connections
        if (pthread_create(&networkThread, NULL, accept_clients, server_data) != 0)
        {
            print_error("Failed to start network thread in game server");

            if (pthread_join(fruitThread, &threadRet) != 0)
                print_error("Failed to join fruit thread in game server");

            if (pthread_join(moveThread, &threadRet) != 0)
                print_error("Failed to join fruit thread in game server");

            return;
        }

        /* The threads will finish when game.is_running becomes false, i.e
         * someone wins */

        // Wait for fruit thread to finish
        if (pthread_join(fruitThread, &threadRet) != 0)
            print_error("Failed to join fruit thread in game server");

        // Wait for move thread to finish
        if (pthread_join(moveThread, &threadRet) != 0)
            print_error("Failed to join move thread in game server");

        // Wait for network thread to finish
        if (pthread_join(networkThread, &threadRet) != 0)
            print_error("Failed to join network thread in game server");

        // TODO send winner + game ended

        // TODO reset game

        // TODO readd all snakes for connected clients

        // TODO send new map
    }
}

void *fruit_thread_fn(void *arg)
{
    printf("INFO: Started thread for adding fruit\n");
    game_t *game = (game_t *) arg;

    while (game->num_players < 1)
    {
        // wait until players have joined
        sleep(5);
    }
    
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

    return 0;
}

void *move_thread_fn(void *arg)
{
    printf("INFO: Started thread for moving players\n");
    game_server_data_t *server_data = (game_server_data_t *) arg;
    game_t *game = server_data->game;

    while (game->num_players < 1)
    {
        // wait until players have joined
        sleep(5);
    }

    while (game->is_running)
    {
        for (size_t i = 0; i < game->snakes_size; ++i)
        {    
            snake_t *snake = game->snakes[i];
            if (snake != NULL)
            {
                move_player(game, snake);

                if (snake->status == WINNER)
                    game->is_running = false;
            }
        }
        
        // TODO make this as a thread
        // Send updated map to each client after all of them move
        send_map_to_all_clients(server_data);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 800000000L; // every 0.8s, snakes move
        nanosleep(&ts, NULL);
    }

    return 0;
}

void *accept_clients(void *arg)
{ 
    printf("INFO: Waiting for clients to connect...\n");

    game_server_data_t *server_data = (game_server_data_t *) arg;
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
            print_error("Could not accept connection request");
        else 
        {
            printf("INFO: Player %ld has joined\n", client_num+1);

            if (client_num < MAX_PLAYERS)
            {
                client_conn_data_t *conn_data = malloc(sizeof(conn_data));
                conn_data->client_socket_fd = server_data->client_socket_fds[client_num];
                conn_data->player_num = client_num;
                conn_data->game = server_data->game;

                // Handle game loop for player in separate detached thread
                make_detached_thread(&handle_client_connection, conn_data); 

                ++client_num;
            }
            else
            {
                // GAME IS FULL
            }
        }
    }

    return 0;
}

void *handle_client_connection(void *arg)
{
    char key_buffer;
    direction_t new_dir;

    client_conn_data_t *conn_data = (client_conn_data_t *) arg;
    int client_fd = conn_data->client_socket_fd;
    game_t *game = conn_data->game;

    snake_t *snake = add_player(game, client_fd);

    // Send player num
    network_data_t player_num_data;
    player_num_data.type = P_NUM_DATA;
    player_num_data.data.player_num = conn_data->player_num;

    // Send map to client
    network_data_t map_data;
    map_data.type = MAP_DATA;
    map_data.data.map = &game->map;

    while (game->is_running && snake->status != DEAD)
    {
        // Reading key from client
        if (read(client_fd, &key_buffer, 1) < 0)
        {
            remove_player(game, snake);
            close(client_fd);
            print_error("Could not read from socket");
            break;
        }

        new_dir = toupper(key_buffer);
        
        // Disconnect player
        if (new_dir == 'Q')
            break;

        // Changing direction if entered key is valid
        else if (is_direction_valid(snake->head.direction, new_dir))
            change_snake_direction(snake, new_dir);
    }

    // Point reached if disconnected, dead, or game is won

    if (snake->status == DEAD)
    {
        // Informing client that they died
        network_data_t data;
        data.type = DEAD_DATA;
        send_to_client(&data, client_fd);

        remove_player(game, snake);
        close(client_fd);
    }

    // Clean up of network and player will be done in another function in case 
    // the game has been won

    // In any case, free allocated memory
    free(conn_data);

    return 0;
}

bool send_to_client(network_data_t *data, int client_fd)
{
    // Sending type of data being sent first
    if (!send_value_to_client(data->type, client_fd))
        return false;

    switch (data->type)
    {
        case MAP_DATA:
            return send_map_to_client(data->data.map, client_fd);
        case P_NUM_DATA:
            return send_value_to_client(data->data.player_num, client_fd);
        case SCORE_DATA:
        case DEAD_DATA:
            return true;
        case ENDGAME_DATA:
            return send_value_to_client(data->data.winner, client_fd);
        case MSG_DATA:
            return send_buffer_to_client(data->data.msg, MSG_SIZE, client_fd);
        default: return false;  // Required for no warnings
    }
}

bool send_buffer_to_client(char *buffer, size_t buffer_size, int client_fd)
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

bool send_map_to_client(game_map_t *map, int client_fd)
{
    size_t const SIZE = MAP_SIZE * sizeof(map[0][0]);
    char map_buffer[SIZE];
    memcpy(map_buffer, map, SIZE);

    return send_buffer_to_client(map_buffer, SIZE, client_fd);
}

bool send_value_to_client(size_t value, int client_fd)
{
    size_t const SIZE = sizeof(value);
    char buffer[SIZE];
    memset(buffer, 0, SIZE);
    memcpy(buffer, &value, SIZE);

    return send_buffer_to_client(buffer, SIZE, client_fd);
}

void *send_map_to_all_clients(void *arg)
{
    // TODO implement

    return 0;
}