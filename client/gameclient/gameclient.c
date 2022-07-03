#include "gameclient.h"

void open_client_connection(client_data_t *client_data, char *hostname)
{

}

void close_client_connection(client_data_t *client_data)
{

}

void *handle_connection(void *arg)
{

}

void write_to_server(client_data_t *client_data, char key)
{

}

void receive_from_server(client_data_t *client_data, game_map_t *map)
{
    // Read type first
    int32_t data_type;
    char *buffer = (char*)&data_type;
    size_t const SIZE = sizeof(data_type);

    size_t bytes_read = 0;

    while (bytes_read < SIZE)
        bytes_read += read(client_data->socket_fd, buffer + bytes_read, 
                                                            SIZE-bytes_read);    

    switch (data_type)
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