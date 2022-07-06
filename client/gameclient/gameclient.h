#ifndef INCLUDED_GAMECLIENT_
#define INCLUDED_GAMECLIENT_

#include "../window/window.h"

int open_client_connection(char *hostname);

void close_client_connection(int client_socket);

void handle_client_connection(int client_socket, WINDOW *window);

#endif