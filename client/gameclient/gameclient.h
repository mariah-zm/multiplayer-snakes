/**
 * @file   gameclient.h
 * @author Mariah Zammit
 * @brief  Contains definitions and functions to open and handle a client 
 *         connection with the server, as well as the logic required to listen 
 *         to user input and updating the game window
 */

#ifndef INCLUDED_GAMECLIENT_
#define INCLUDED_GAMECLIENT_

#define DEAD            -1
#define PLAYING         1
#define WINNER          2
#define LOSER           3
#define DISCONNECTED    4

#include "../window/window.h"

int open_client_connection(WINDOW *window, char *hostname);

void close_client_connection(int client_socket);

void handle_client_connection(int client_socket, WINDOW *window);

#endif