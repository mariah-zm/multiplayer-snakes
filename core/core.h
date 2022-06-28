/**
 * @file core.h
 * @author Mariah Zammit
 * @brief For data shared between server and client
 * 
 */
#ifndef INCLUDED_CORE_
#define INCLUDED_CORE_

#define PORT            5566

#define GAME_HEIGHT     24    // default size of terminal window in num of chars
#define GAME_WIDTH      80

#define UP_KEY          'W'
#define LEFT_KEY        'A'
#define DOWN_KEY        'S'
#define RIGHT_KEY       'D'

void print_error(char *msg);

void exit_error(char *msg);

#endif