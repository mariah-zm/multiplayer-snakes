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
void exit_client(WINDOW *window, char *msg);

int open_client_connection(WINDOW *window, char *hostname)
{
    int socket_fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Creating client socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_client(window, "Could not open client socket");
     
    // Resolving server name
    if ((server = gethostbyname(hostname)) == NULL) {
		close(socket_fd);
        char err_msg[30];
        sprintf(err_msg, "No such host %s", hostname);
        exit_client(window, err_msg);
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
        exit_client(window, "Could not connect to server");
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
    game_data.window = window;
    game_data.player_status = PLAYING;

    char key_buffer;

    // Start updating the screen from received map data
    pthread_t update_thread;
    void *threadRet;

    if (pthread_create(&update_thread, NULL, update_game, &game_data) != 0)
    {
        logger(ERROR, "Failed to start input thread for player in game server");
        return;
    }

    // Getting user key input
    char key_pressed;

    do
    {
        memset(&key_buffer, 0, 1);

        // Getting user input and only sending valid keys
        key_pressed = toupper(wgetch(window));

        // DO not send if no longer in game
        if (is_key_valid(key_pressed) && game_data.player_status == PLAYING)
            write_to_server(client_socket, key_pressed);
        
        if (key_pressed == QUIT)
            game_data.player_status = DISCONNECTED;
        
    } while (key_pressed != QUIT && game_data.player_status == PLAYING);

    // Wait for update thread to finish
    if (pthread_join(update_thread, &threadRet) != 0)
        logger(ERROR, "Failed to join update thread in game server");
}

/*****************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

void *update_game(void *arg)
{
    game_data_t *game = (game_data_t *) arg;

    while (game->player_status == PLAYING)
    {
        // Recieve updated map from server
        if (!read_from_server(game->client_socket, &game->map))
        {
            game->player_status = DISCONNECTED;
            break;
        }

        game->player_num = game->map[DATA_ROW][PNUM_IDX];

        // Setting player status according to data received
        if (game->player_num  == DEAD)
            game->player_status = DEAD;
        
        // Checking if game is finished because someone won
        int winner = game->map[DATA_ROW][WINNER_IDX];
        if (winner > 0)
        {
            if (winner == game->player_num)
                game->player_status = WINNER;
            else
                game->player_status = LOSER;

            // Setting player_num to winner to display message later
            game->player_num = winner;
        }

        show_game(game->window, game->map);
    }

    if (game->player_status == WINNER)
        show_message(game->window, "You win!");
    else if (game->player_status == LOSER)
    {
        char msg[80];
        sprintf(msg, " You lose! Player %ld won.", game->player_num);
        show_message(game->window, msg);
    }
    else if (game->player_status == DEAD)
        show_message(game->window, "You lose!");

    return 0;
}

void write_to_server(int client_socket, char key)
{
    if (write(client_socket, &key, 1) < 0) 
        exit_error("Could not write to socket");
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

void exit_client(WINDOW *window, char *msg)
{
    destroy_game_window(window);
    exit_error(msg);
}