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
}

void close_game_server(void)
{
    close(server_socket_fd);
}

void accept_clients(void)
{ 
	// Start listening for clients
    listen(server_socket_fd, 5);

    size_t client_num = 0;
    socklen_t client_len = sizeof(struct sockaddr_in);

    while (true)
    {
        // Accept incoming connection
        client_socket_fds[client_num] = accept(server_socket_fd, 
                (struct sockaddr *) &client_socket_fds[client_num], &client_len);
        
        if (client_socket_fds[client_num] < 0) 
            print_error("ERR: could not accept connection request");
        else 
        {
            // Check winner

            // Handle game loop for player in separate thread
            make_detached_thread(&game_loop, &client_socket_fds[client_num]); 

            ++client_num;
        }
    }
}

void *game_loop(void *player_fd)
{
    int player_num = (int) player_fd;

    add_player(game, player_num);

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