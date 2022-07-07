#ifndef INCLUDED_GAMESRVER_
#define INCLUDED_GAMESRVER_

#define MAX_PLAYERS     1000

#include "../models/game.h"
#include "../../core/core.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

typedef struct game_server
{
    int server_socket_fd;
    int client_socket_fds[MAX_PLAYERS];
    struct sockaddr_in client_addrs[MAX_PLAYERS];
} game_server_data_t;

typedef struct client_conn_data
{
    game_t *game;
    snake_t *snake;
    int client_socket;
} client_conn_data_t;

void init_game_server(game_server_data_t *server_data);

void close_game_server(game_server_data_t *server_data);

void start_game_server(game_server_data_t *server_data);

void server_signal_handler();

#endif