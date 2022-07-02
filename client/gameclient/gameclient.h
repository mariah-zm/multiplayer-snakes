#ifndef INCLUDED_GAMECLIENT_
#define INCLUDED_GAMECLIENT_

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "../../core/core.h"

typedef struct client_data
{
    int socket_fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;
} client_data_t;

void open_client_connection(client_data_t *client_data, char *hostname);

void close_client_connection(client_data_t *client_data);

void *handle_connection(void *arg);

void write_to_server(client_data_t *client_data, char key);

void receive_from_server(client_data_t *client_data, game_map_t *map);

#endif