/**
 * @file   servercore.h
 * @author Mariah Zammit
 * @brief  Contains type definitions and functions that are available to all 
 *         files in the game server
 */

#ifndef INCLUDED_SERVERCORE_
#define INCLUDED_SERVERCORE_

#include <stddef.h>

#include "../../core/core.h"

typedef enum direction
{
    UP = UP_KEY, 
    LEFT = LEFT_KEY,
    DOWN = DOWN_KEY,  
    RIGHT = RIGHT_KEY
} direction_t;

typedef struct coordinate 
{
    size_t x;
    size_t y;
    direction_t direction;
} coordinate_t;

typedef enum player_status
{
    PLAYING,
    WINNER,
    DEAD,
    DISCONNECTED
} player_status_t;

#endif