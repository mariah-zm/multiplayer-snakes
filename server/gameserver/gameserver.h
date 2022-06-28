#ifndef INCLUDED_GAMESRVER_
#define INCLUDED_GAMESRVER_

#include "../models/game.h"

void init_game_server(game_t *game_p);

void close_game_server(void);

void accept_clients(void);

void *game_loop(void *player_fd);

void make_detached_thread(void* (*fn)(void *), void* arg);

#endif