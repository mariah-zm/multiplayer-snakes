#include "gameclient/gameclient.h"
#include "window/window.h"

int main(int argc, char *argv[])
{
    // Checking for server name
    if (argc != 2) {
        char msg[30];
        scanf(msg, "usage %s <hostname>\n", argv[0]);
        exit_error(msg);
    }

    WINDOW *window = create_window();
    show_welcome_screen(window);

    client_data_t client_data;
    open_client_connection(&client_data, argv[1]);

    // Start gameplay

    close_client_connection(&client_data);
    destroy_window(window);

    return 0;
}