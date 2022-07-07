#include "gameserver.h"

#include "../models/game.h"
#include "../../core/core.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Global variable for signal handler
int server_socket;

// Private functions declarations
void make_detached_thread(void* (*fn)(void *), void* arg);
bool write_to_client(game_map_t map, int client_socket);
bool write_value_to_client(size_t value, int client_socket);
bool write_map_to_client(game_map_t map, int client_socket);
void *fruit_thread_fn(void *arg);
void add_player_data(game_t *game, snake_t *snake, game_map_t *client_map);
void *handle_client_connection(void *arg);

void init_game_server(game_server_data_t *server_data)
{
    struct sockaddr_in server_addr;

    // Create server socket
    if ((server_data->server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("Could not open server socket");

    // Initialize socket structure
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the host address
    if (bind(server_data->server_socket_fd, 
                (struct sockaddr *) &server_addr, 
                sizeof(server_addr)) < 0) 
    {
		close(server_data->server_socket_fd);
        exit_error("Could not bind server socket");
	}

    // Start listening for clients
    listen(server_data->server_socket_fd, 5);

    server_socket = server_data->server_socket_fd;

    char msg[100];
    sprintf(msg, "Game server is running on port %d", PORT);
    logger(INFO, msg);
}

void close_game_server(game_server_data_t *server_data)
{
    close(server_data->server_socket_fd);
}

void start_game_server(game_server_data_t *server_data)
{
    socklen_t client_len = sizeof(server_data->server_socket_fd);

    game_t *game = create_game();

    // Start thread for adding fruit at random intervals
    pthread_t fruitThread;
    void *threadRet;

    if (pthread_create(&fruitThread, NULL, fruit_thread_fn, game) != 0)
        exit_error("Failed to start fruit thread in game server");

    for (int i = 0; ; ++i)
    {
        // Accepting an incoming connection request
        server_data->client_socket_fds[i] =
            accept(server_data->server_socket_fd, 
                (struct sockaddr *) &server_data->client_addrs[i], &client_len);

        if (server_data->client_socket_fds[i] < 0) 
            logger(ERROR, "Could not accept client");

        // Handling client connection in separate thread
        client_conn_data_t *conn_data = (client_conn_data_t *) malloc(sizeof(client_conn_data_t));
        conn_data->game = game;
        conn_data->snake = NULL;
        conn_data->client_socket = server_data->client_socket_fds[i];

        make_detached_thread(handle_client_connection, conn_data); 
    }

    // Wait for fruit thread to finish
    if (pthread_join(fruitThread, &threadRet) != 0)
        logger(ERROR, "Failed to join fruit thread in game server");

    free(game);
}

void server_signal_handler()
{
    logger(ERROR, "Server interrupted. Closing...");
    close(server_socket);
    exit(0);
}

/*****************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

void make_detached_thread(void* (*fn)(void *), void* arg)
{
    pthread_t       tid;
    pthread_attr_t  attr;

    if (pthread_attr_init(&attr) != 0)
        logger(ERROR, "Failed to initialise thread");

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) == 0)
        pthread_create(&tid, &attr, fn, arg);
    else
        pthread_attr_destroy(&attr);
}

bool write_to_client(game_map_t map, int client_socket)
{
    char buffer[MAP_SIZE];
    memcpy(buffer, map, MAP_SIZE);

        size_t bytes_sent = 0;

    while (bytes_sent < MAP_SIZE)
    {
        bytes_sent += write(client_socket, buffer, MAP_SIZE);

        if (bytes_sent < 0)
            return false;
    }

    return true;
}

void *fruit_thread_fn(void *arg)
{
    logger(INFO, "Started thread for adding fruit");
    game_t *game = (game_t *) arg;
    
    while (true)
    {
        // Only adding fruit if there are players in the game
        while (game->num_players > 0)
        {        
            add_fruit(game);

            // Creating random intervals between 3 and 6 seconds
            int milli_seconds = (rand() % 3000) + 3000;
            struct timespec ts;
            ts.tv_sec = milli_seconds / 1000;
            ts.tv_nsec = (milli_seconds % 1000) * 1000000L; // range 0 to 999999999
            nanosleep(&ts, NULL);
        }
    }
}

void *handle_client_input(void *arg)
{
    client_conn_data_t *data = (client_conn_data_t *) arg;

    // For quick access
    game_t *game = data->game;
    snake_t *snake = data->snake;
    int fd = data->client_socket;
    int player_num = snake->player_num;

    // Values for client input
    char key_buffer, new_dir;

    // Wait for snake to become available
    while (snake == NULL); 

    while (game->is_running && snake->status == PLAYING) // TODO add player status
    {
        // Reading key from client
        if (read(fd, &key_buffer, 1) < 0)
        {
            char msg[100];
            sprintf(msg, "Could not read input from player %d", player_num);
            logger(ERROR, msg);
            snake->status = DISCONNECTED;
            break;
        }

        new_dir = toupper(key_buffer);
        
        // Disconnect player
        if (new_dir == 'Q')
        {
            snake->status = DISCONNECTED;
            break;
        }

        // Changing direction if entered key is valid
        else if (is_direction_valid(snake->head.direction, new_dir))
            change_snake_direction(snake, new_dir);
    }

    return 0;
}

void add_player_data(game_t *game, snake_t *snake, game_map_t *client_map)
{
    memcpy(client_map, game->map, MAP_SIZE);

    // Setting score
    (*client_map)[DATA_ROW][SCORE_IDX] = snake->length - 2;

    // Setting player's number
    if (snake->status == DEAD)
        (*client_map)[DATA_ROW][PNUM_IDX] = -1;
    else 
        (*client_map)[DATA_ROW][PNUM_IDX] = snake->player_num;

    // Setting winner's number, if any
    (*client_map)[DATA_ROW][WINNER_IDX] = game->winner;
}

void *handle_client_connection(void *arg)
{
    client_conn_data_t *data = (client_conn_data_t *) arg;
    
    // For quick access
    game_t *game = data->game;
    int client_socket = data->client_socket;
    int player_num = client_socket-3;

    char msg[100];
    sprintf(msg, "Player %d has joined", player_num);
    logger(INFO, msg);

    // Start thread to receive client input
    pthread_t inputThread;
    void *threadRet;

    if (pthread_create(&inputThread, NULL, handle_client_input, data) != 0)
    {
        logger(ERROR, "Failed to start input thread for player in game server");
        return 0;
    }
    
    // Create snake structure
    data->snake = add_player(game, player_num);
    snake_t *snake = data->snake;

    // Time structure for sleep
    struct timespec ts;
    ts.tv_sec = 0;
    // ts.tv_nsec = 0;
    ts.tv_nsec = PLAYING_SPEED * 10000000L; // every 0.15s, snakes move

    do 
    {
        move_player(game, snake);

        game_map_t map_to_send;
        add_player_data(game, snake, &map_to_send);

        if (!write_to_client(map_to_send, client_socket))
        {
            snake->status = DISCONNECTED;
            break;
        }

        nanosleep(&ts, NULL);

    } while (snake->status == PLAYING);

    // Reset game if we have a winner
    if (snake->status == WINNER)
    {
        char msg[100];
        sprintf(msg, "Player %d won!", player_num);
        logger(INFO, msg);

        reset_game(game);
    }
    else if (snake->status == DISCONNECTED)
    {
        char msg[100];
        sprintf(msg, "Player %d has left the game", player_num);
        logger(INFO, msg);
    }    
    else if (snake->status == DEAD)
    {
        char msg[100];
        sprintf(msg, "Player %d died", player_num);
        logger(INFO, msg);
    }    

    // Wait for input thread to finish
    if (pthread_join(inputThread, &threadRet) != 0)
        logger(ERROR, "Failed to join fruit thread in game server");

    // Clean up player data
    remove_player(game, snake);
    close(client_socket);
    free(data);

    return 0;
}