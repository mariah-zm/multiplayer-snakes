#include <stdlib.h>

#include "gameserver/gameserver.h"
#include "core/core.h"

int main(void)
{
    srand(0);

    // Create game
    game_t *game = create_game();
    game_server_data_t game_server;
    game_server.game = game;

    // Initialise server
    init_game_server(&game_server);

    start_game_server(&game_server);

    // Close server socket on termination
    close_game_server(&game_server);  
    
    return 0; 
}