#include "gameclient.h"

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <time.h>

#include "../../core/core.h"
#include "../window/window.h"
#include "../clientcore/clientcore.h"

// Private functions declarations
void *update_game(void *arg);
void write_to_server(int client_socket, char key);
bool read_from_server(int client_socket, game_map_t *map);
bool is_key_valid(char key);

int open_client_connection(char *hostname)
{
    int socket_fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Creating client socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("Could not open client socket");
     
    // Resolving server name
    if ((server = gethostbyname(hostname)) == NULL) {
		close(socket_fd);
        char err_msg[30];
        sprintf(err_msg, "No such host %s", hostname);
        print_error(err_msg);
    }

    // Populating serv_addr structure 
    memset(&serv_addr, 0, sizeof(serv_addr));
    // Setting to AF_INET
    serv_addr.sin_family = AF_INET;    
    // Setting server address
    memcpy(&serv_addr.sin_addr.s_addr,
           server->h_addr,             
           server->h_length);

    // Setting port (convert to network byte ordering)
    serv_addr.sin_port = htons(PORT); 

    // Connecting to the server 
    if (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
		close(socket_fd);
        exit_error("Could not connect to server");
    }

    return socket_fd;
}

void close_client_connection(int client_socket)
{
    close(client_socket);
}

void handle_client_connection(int client_socket, WINDOW *window)
{
    // Populate data required for gameplay
    game_data_t game_data;
    game_data.client_socket = client_socket;
    game_data.score = 0;
    game_data.window = window;
    game_data.game_status = ONGOING;

    char key_buffer;

    // Start updating the screen from received map data
    pthread_t updateThread;
    void *threadRet;

    if (pthread_create(&updateThread, NULL, update_game, &game_data) != 0)
    {
        print_error("Failed to start input thread for player in game server");
        return;
    }

    // Getting user key input
    char key_pressed;

    do
    {
        memset(&key_buffer, 0, 1);

        // Getting user input and only sending valid keys
        key_pressed = toupper(wgetch(window));

        if (is_key_valid(key_pressed))
            write_to_server(client_socket, key_pressed);
        
    } while (key_pressed != QUIT && game_data.game_status == ONGOING);

    // Wait for update thread to finish
    if (pthread_join(updateThread, &threadRet) != 0)
        print_error("Failed to join update thread in game server");

    if (key_pressed != QUIT)
        show_winner(game_data.game_status);
}

/*****************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

void *update_game(void *arg)
{
    game_data_t *game = (game_data_t *) arg;

    while (game->game_status == ONGOING)
    {
        // Recieve updated map from server
        if (!read_from_server(game->client_socket, &game->map))
        {
            game->game_status = END;
            break;
        }

        show_game_screen(game->window, game->map, game->score);
    }

    return 0;
}

void write_to_server(int client_socket, char key)
{
    if (write(client_socket, &key, 1) < 0) 
        exit_error("ERROR writing to socket.");
}

bool read_from_server(int client_socket, game_map_t *map)
{
    char map_buffer[MAP_SIZE];
    int bytes_read = 0, n;

    while(bytes_read < MAP_SIZE){
        n = read(client_socket, map_buffer + bytes_read, MAP_SIZE - bytes_read);
        if(n <= 0)
            return false;
        bytes_read += n;
    }

    memcpy(map, map_buffer, MAP_SIZE);

    return true;
}

bool is_key_valid(char key)
{
    return key == UP_KEY || key == DOWN_KEY || key == LEFT_KEY 
        || key == RIGHT_KEY || key == QUIT;
}