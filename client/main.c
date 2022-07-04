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
            int client_socket_fd = open_client_connection(argv[1]);

            handle_client_connection(client_socket_fd, game_window);

            close_client_connection(client_socket_fd);
        }
        else if (option == OPT_INS)
            show_instructions(game_window);
        else        // option is EXIT
            break;             
    }
    
    destroy_game_window(game_window);

    return 0;
}