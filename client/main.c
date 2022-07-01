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

    init_ncurses();
    WINDOW *game_window = create_game_window();

    // Display menu
    while (true)
    {
        size_t option = show_menu(game_window);

        if (option == 1 || option == 3)
            break;
        else 
            show_instructions(game_window);
    }

    client_data_t client_data;
    // open_client_connection(&client_data, argv[1]);

    // Start gameplay

    // close_client_connection(&client_data);
    destroy_window(game_window);

    return 0;
}