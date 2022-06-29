#ifndef INCLUDED_GAMESRVER_
#define INCLUDED_GAMESRVER_

#include "../models/game.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

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
    int client_socket_fd;
} client_conn_data_t;

void init_game_server(game_server_data_t *server_data);

void close_game_server(game_server_data_t *server_data);

void start_game(game_server_data_t *server_data);

void *fruit_thread_fn(void *arg);

void *move_thread_fn(void *arg);

void accept_clients(game_server_data_t *server_data);

void send_one(char *map_buffer, int client_fd);

void send_all(game_server_data_t *server_data);

void *handle_client_connection(void *player_fd);

void make_detached_thread(void* (*fn)(void *), void* arg);

#endif