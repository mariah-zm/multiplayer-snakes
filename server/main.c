#include "../core/core.h"
#include "gameserver/gameserver.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main(void)
{
    srand(time(NULL));                          // Random seed
    signal(SIGINT, server_signal_handler);      // Signal handler for Ctrl+C

    // Initialise server data and game
    game_server_data_t game_server;
    
    // Initialising server by opening connection
    init_game_server(&game_server);

    // Start game server and start accepting connections
    start_game_server(&game_server);

    return 0; 
}
