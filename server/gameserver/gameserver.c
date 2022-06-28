#include "gameserver.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

int server_socket_fd;
int client_socket_fds[MAX_PLAYERS];     
struct sockaddr_in server_addr;
struct sockaddr_in client_addrs[MAX_PLAYERS];

game_t *game;

void init_game_server(game_t *game_p)
{
    game = game_p;
    // Create server socket
    if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("ERR: could not open server socket");

    // Initialize socket structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the host address
    if (bind(server_socket_fd, (struct sockaddr *) &server_socket_fd, 
                sizeof(server_socket_fd)) < 0) 
    {
		close(server_socket_fd);
        exit_error("ERR: could not bind server socket");
	}

    // Start listening for clients
    listen(server_socket_fd, 5);
}

void close_game_server(void)
{
    close(server_socket_fd);
}

void accept_clients(void)
{ 
    size_t client_num = 0;
    socklen_t client_len = sizeof(struct sockaddr_in);

    while (game->is_running)
    {
        // Accept incoming connection
        client_socket_fds[client_num] = accept(server_socket_fd, 
                (struct sockaddr *) &client_socket_fds[client_num], &client_len);
        
        if (client_socket_fds[client_num] < 0) 
            print_error("ERR: could not accept connection request");
        else 
        {
            // Handle game loop for player in separate thread
            make_detached_thread(&handle_client_connection, 
                                    &client_socket_fds[client_num]); 

            ++client_num;
        }
    }
}

void send_one(char *map_buffer, int client_fd)
{
    size_t bytes_sent = 0;

    while(bytes_sent < MAP_SIZE)
    {         
        bytes_sent += write(client_fd, map_buffer, MAP_SIZE);
        if (bytes_sent < 0) 
            print_error("ERR: failed to write to client socket");
    } 
}

void send_all()
{
    char map_buffer[MAP_SIZE];
    memcpy(map_buffer, game->map, MAP_SIZE);

    for (int i = 0; i < game->snakes_size; ++i)
    {
        if (game->snakes[i] != NULL)
            send_one(map_buffer, client_socket_fds[i]);
    }
}

void *handle_client_connection(void *player_fd)
{
    char key_buffer;
    direction_t new_dir;
    int player_num = *(int *) player_fd;

    snake_t *snake = add_player(game, player_num);

    // Send map to client (player)
    char map_buffer[MAP_SIZE];
    memcpy(map_buffer, game->map, MAP_SIZE);
    send_one(map_buffer, player_num);

    while (true)
    {
        memset(key_buffer, 0, 1);

        if (read(player_fd, key_buffer, 255) < 0)
        {
            remove_player(game, snake);
            close(player_fd);
            print_error("ERROR reading from socket");
            break;
        }

        new_dir = toupper(key_buffer);
        
        if (is_direction_valid(snake->head.direction, new_dir))
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