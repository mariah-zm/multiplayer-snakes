/**
 * @file   gameclient.h
 * @author Mariah Zammit
 * @brief  Contains definitions and functions to open and handle a client 
 *         connection with the server, as well as the logic required to listen 
 *         to user input and updating the game window
 */

#ifndef INCLUDED_GAMECLIENT_
#define INCLUDED_GAMECLIENT_

#include "../window/window.h"

int open_client_connection(char *hostname);

void close_client_connection(int client_socket);

void handle_client_connection(int client_socket, WINDOW *window);

#endif