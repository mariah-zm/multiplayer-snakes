#ifndef INCLUDED_GAMESRVER_
#define INCLUDED_GAMESRVER_

#include "../models/game.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
// #include <stdlib.h>

typedef struct game_server
{
    int server_socket_fd;
    int client_socket_fds[MAX_PLAYERS];     
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addrs[MAX_PLAYERS];
    game_t *game;
} game_server_data_t;

typedef struct client_conn_data
{
    game_t *game;
    size_t player_num;
    int client_socket_fd;
} client_conn_data_t;

typedef struct data_to_send
{
    network_data_t data;
    int client_socket_fd;
} data_to_send_t;

void init_game_server(game_server_data_t *server_data);

void close_game_server(game_server_data_t *server_data);

void start_game_server(game_server_data_t *server_data);

#endif