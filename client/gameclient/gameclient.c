#include "gameclient.h"

bool is_key_valid(char key);

void open_client_connection(client_data_t *client_data, char *hostname)
{
    // Pointers for quick access
    int *socket_fd = &client_data->socket_fd;
    struct sockaddr_in *serv_addr = &client_data->serv_addr;

    // Creating client socket
    if ((*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        exit_error("ERR: could not open client socket");
     
    // Resolving server name
    if ((client_data->server = gethostbyname(hostname)) == NULL) {
		close(*socket_fd);

        char *err_msg;
        scanf(err_msg, "ERR: no such host %s", hostname);
        print_error(err_msg);
    }

    // Populating serv_addr structure 
    memset(serv_addr, 0, sizeof(serv_addr));
    // Setting to AF_INET
    serv_addr->sin_family = AF_INET;    
    // Setting server address
    memcpy(&serv_addr->sin_addr.s_addr,
           client_data->server->h_addr,             
           client_data->server->h_length);

    // Setting port (convert to network byte ordering)
    serv_addr->sin_port = htons(PORT); 

    // Connecting to the server 
    if (connect(*socket_fd, (struct sockaddr *) serv_addr, sizeof(serv_addr)) < 0) 
    {
		close(*socket_fd);
        exit_error("ERR: could not connect to server");
    }
}

void close_client_connection(client_data_t *client_data)
{
    close(client_data->socket_fd);
}

void handle_connection(client_data_t *client_data, WINDOW *window)
{
    // Receive player_number
    network_data_t num_data;
    receive_from_server(client_data, &num_data);
    if (num_data.type != P_NUM_DATA)
    {
        print_error("ERR: Expected player number from server");
        return;
    }

    // Receive map
    network_data_t map_data;
    receive_from_server(client_data, &map_data);
    if (num_data.type != MAP_DATA)
    {
        print_error("ERR: Expected game map from server");
        return;
    }

    // Start gameplay
    game_data_t game_data;
    game_data.map = map_data.data.map;
    game_data.player_num = map_data.data.player_num;
    game_data.score = 0;
    game_data.window = window;

    show_game(&game_data);

    // Start thread to receive updates from game server

    char key_pressed;

    do
    {
        key_pressed = toupper(wgetch(window));

        if (is_key_valid(key_pressed))
            write_to_server(client_data, key_pressed);
        
    } while (key_pressed != QUIT); // and game is ongoing
    
    // Disconnect client
    
}

void write_to_server(client_data_t *client_data, char key)
{
    if (write(client_data->socket_fd, &key, 1) < 0) 
        print_error("ERR: could not write to socket");
}

network_data_t receive_from_server(client_data_t *client_data)
{
    network_data_t data;
    char *buffer = (char *) &data.type;
    size_t const SIZE = sizeof(data.type);

    size_t bytes_read = 0;

    while (bytes_read < SIZE)
        bytes_read += read(client_data->socket_fd, buffer + bytes_read, 
                                                            SIZE-bytes_read);
        
    switch (data.type)
    {
        case MAP_DATA:

            break;
        case P_NUM_DATA:
            break;
        case SCORE_DATA:
            break;
        case ENDGAME_DATA:
            break;
        case MSG_DATA:
            break;
        default: break;  // Required for no warnings
    }
}

bool is_key_valid(char key)
{
    return key == UP_KEY || key == DOWN_KEY || key == LEFT_KEY 
        || key == RIGHT_KEY || key == QUIT;
}