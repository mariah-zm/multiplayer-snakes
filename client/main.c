#include "clientcore/clientcore.h"
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

        if (option == OPT_PLAY)
        {
            client_data_t client_data;
            open_client_connection(&client_data, argv[1]);

            handle_client_connection(&client_data);

            close_client_connection(&client_data);
        }
        else if (option == OPT_INS)
            show_instructions(game_window);
        else        // option is EXIT
            break;             
    }
    
    destroy_game_window(game_window);

    return 0;
}