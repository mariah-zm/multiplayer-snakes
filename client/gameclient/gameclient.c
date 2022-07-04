#include "gameclient.h"

#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>

// Private functions not part of API
void write_to_server(int client_socket, char key);
network_data_t receive_from_server(int client_socket);
void *update_game(void *arg);
bool is_key_valid(char key);

int open_client_connection(char *hostname)
{
    // Pointers for quick access
    int socket_fd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Creating client socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("Could not open client socket");
     
    // Resolving server name
    if ((server = gethostbyname(hostname)) == NULL) {
		close(socket_fd);

        char err_msg[30] = "No such host ";
        strcat(err_msg, hostname);
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
    // Receive player_number
    network_data_t num_data = receive_from_server(client_socket);
    if (num_data.type != P_NUM_DATA)
    {
        print_error("Expected player number from server");
        return;
    }

    // Receive map
    network_data_t map_data = receive_from_server(client_socket);
    if (num_data.type != MAP_DATA)
    {
        print_error("Expected game map from server");
        return;
    }

    // Display intial game window
    show_game(window, map_data.data.map, num_data.data.player_num);

    // Populate data required for gameplay
    game_data_t game_data;
    game_data.client_socket = client_socket;
    game_data.map = map_data.data.map;
    game_data.player_num = num_data.data.player_num;
    game_data.score = 0;
    game_data.window = window;
    game_data.game_status = ONGOING;

    // Start thread to receive updates from game server
    pthread_t gameThread;
    void *threadRet;

    if (pthread_create(&gameThread, NULL, update_game, &game_data) != 0)
    {
        print_error("Failed to start game thread.");
        return;
    }

    // Getting user key input
    char key_pressed;

    do
    {
        key_pressed = toupper(wgetch(window));

        if (is_key_valid(key_pressed))
            write_to_server(client_socket, key_pressed);
        
    } while (key_pressed != QUIT && game_data.game_status == ONGOING);

    game_data.game_status = END;
    
    // Disconnect client
    
    // Wait for game thread to finish
    if (pthread_join(gameThread, &threadRet) != 0)
        print_error("Failed to join game thread");
}

void *update_game(void *arg)
{
    game_data_t *game_data = (game_data_t *) arg;
    WINDOW *window = game_data->window;

    while (game_data->game_status == ONGOING)
    {
        network_data_t server_data = receive_from_server(game_data->client_socket);

        switch (server_data.type)
        {
            case MAP_DATA:
                update_game_map(window, server_data.data.map, 
                                                        game_data->player_num);
                break;

            case SCORE_DATA:
                update_score(window, ++game_data->score);
                break;

            case ENDGAME_DATA:
                if (server_data.data.winner == game_data->player_num)
                    show_winning_screen(window);
                else 
                    show_losing_screen(window, server_data.data.winner);
                game_data->game_status = END;
                break;

            case DEAD_DATA:
                show_dead_screen(window);
                game_data->game_status = END;
                break;

            case MSG_DATA:
                show_message(window, server_data.data.msg, game_data->score);
                break;
        }

    }
    
}

void write_to_server(int client_socket, char key)
{
    if (write(client_socket, &key, 1) < 0) 
        print_error("Could not write to socket");
}

network_data_t receive_from_server(int client_socket)
{
    network_data_t data;
    char *buffer = (char *) &data.type;
    size_t const SIZE = sizeof(data.type);

    size_t bytes_read = 0;

    while (bytes_read < SIZE)
        bytes_read += read(client_socket, buffer + bytes_read, SIZE-bytes_read);

    // Reset bytes_read for actual data
    bytes_read = 0;

    char *data_buffer;
    size_t data_size;
        
    switch (data.type)
    {
        case MAP_DATA:
        {
            data_buffer = (char *) &data.data.map;
            data_size = MAP_SIZE * sizeof(data.data.map[0][0]);
            break;
        }
        
        case P_NUM_DATA:
        {
            data_buffer = (char *) &data.data.player_num;
            data_size = sizeof(data.data.player_num);
            break;
        }

        case ENDGAME_DATA:
        {
            data_buffer = (char *) &data.data.winner;
            data_size = sizeof(data.data.winner);
            break;
        }

        case MSG_DATA:
        {
            data_buffer = (char *) &data.data.msg;
            data_size = MSG_SIZE;
            break;
        }

        // These messages do not have data in them
        case DEAD_DATA:
        case SCORE_DATA:
        default:            // default required for no warnings
            return data;  
    }

    while (bytes_read < SIZE)
        bytes_read += read(client_socket,
                            data_buffer + bytes_read, 
                            data_size-bytes_read);
    
    return data;
}

bool is_key_valid(char key)
{
    return key == UP_KEY || key == DOWN_KEY || key == LEFT_KEY 
        || key == RIGHT_KEY || key == QUIT;
}