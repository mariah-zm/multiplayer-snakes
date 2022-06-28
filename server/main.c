#include <stdlib.h>

#include "gameserver/gameserver.h"
#include "core/core.h"

int main(void)
{
    srand(0);

    // Create game
    game_t *game = create_game();

    // Initialise server
    init_game_server(game);

    // Accept incoming connections
    accept_clients();

    // Close server socket on termination
    close_game_server();  
    
    return 0; 
}