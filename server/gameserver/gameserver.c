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
bool write_to_client(game_map_t map, int client_socket);
void *fruit_thread_fn(void *arg);
void *handle_client_connection(void *arg);
void *say_hello(void *arg);

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

    printf("INFO: Game server is running on port %d\n", PORT);
}

void close_game_server(game_server_data_t *server_data)
{
    close(server_data->server_socket_fd);
    // TODO close client connections
}

void start_game_server(game_server_data_t *server_data)
{
    socklen_t client_len = sizeof(server_data->server_socket_fd);

    game_t *game = create_game();

    // Start thread for adding fruit at random intervals
    pthread_t fruitThread;
    void *threadRet;

    if (pthread_create(&fruitThread, NULL, fruit_thread_fn, game) != 0)
    {
        print_error("Failed to start fruit thread in game server");
        return;
    }

    for (int i = 0; ; ++i)
    {
        // Accepting an incoming connection request
        server_data->client_socket_fds[i] =
            accept(server_data->server_socket_fd, 
                (struct sockaddr *) &server_data->client_addrs[i], &client_len);

        if (server_data->client_socket_fds[i] < 0) 
            exit_error("ERROR on accept");

        // Reset game if someone won
        if (!game->is_running)
        {
            printf("Il-logħba ġiet irrisettjata!\n");
            game->is_running = true;
        }

        client_conn_data_t *conn_data = (client_conn_data_t *) malloc(sizeof(client_conn_data_t));
        conn_data->game = game;
        conn_data->snake = NULL;
        conn_data->client_socket = server_data->client_socket_fds[i];

        make_detached_thread(handle_client_connection, conn_data); 
    }

        // Wait for fruit thread to finish
    if (pthread_join(fruitThread, &threadRet) != 0)
        print_error("Failed to join fruit thread in game server");


    free(game);
}

void server_signal_handler()
{
    printf("\nServer interrupted. Closing...\n");
    close(server_socket);
    exit(0);
}

/*****************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

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
    printf("INFO: Started thread for adding fruit\n");
    game_t *game = (game_t *) arg;
    
    while (true)
    {
        add_fruit(game);

        // Creating random intervals between 2 and 5 seconds
        int milli_seconds = (rand() % 3000) + 2000;
        struct timespec ts;
        ts.tv_sec = milli_seconds / 1000;
        ts.tv_nsec = (milli_seconds % 1000) * 1000000L; // range 0 to 999999999
        nanosleep(&ts, NULL);
    }
}

void *handle_client_input(void *arg)
{
    client_conn_data_t *data = (client_conn_data_t *) arg;

    // For quick access
    game_t *game = data->game;
    snake_t *snake = data->snake;
    int fd = data->client_socket;
    size_t player_num = snake->player_num;

    // Values for client input
    char key_buffer, new_dir;

    // Wait for snake to become available
    while (snake == NULL); 

    while (game->is_running) // TODO add player status
    {
        // Reading key from client
        if (read(fd, &key_buffer, 1) < 0)
        {
            printf("ERR: Could not read input from player %ld", player_num);
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

    return 0;
}

void *handle_client_connection(void *arg)
{
    client_conn_data_t *data = (client_conn_data_t *) arg;
    
    // For quick access
    game_t *game = data->game;
    int fd = data->client_socket;
    
    int player_num = fd-3;
    printf("INFO: Player %d has joined\n", player_num);

    // Start thread to receive client input
    make_detached_thread(handle_client_input, data); 
    
    // Create snake structure
    data->snake = add_player(game, player_num);
    snake_t *snake = data->snake;

    // Time structure for sleep
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 300000000L; // every 0.3s, snakes move

    do 
    {
        if (!write_to_client(game->map, fd))
        {
            snake->status = DISCONNECTED;
            break;
        }

        move_player(game, snake);

        nanosleep(&ts, NULL);

    } while (game->is_running && snake->status == PLAYING);

    if (snake->status == WINNER)
        printf("INFO: Player %d won!\n", player_num);
    else if (snake->status == DISCONNECTED)
        printf("INFO: Player %d has left the game\n", player_num);
    else if (snake->status == DEAD)
        printf("INFO: Player %d died\n", player_num);

    // Clean up player data
    remove_player(game, snake);
    close(fd);
    free(data);
}