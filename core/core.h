/**
 * @file core.h
 * @author Mariah Zammit
 * @brief For data shared between server and client
 * 
 */
#ifndef INCLUDED_CORE_
#define INCLUDED_CORE_

#define PORT            5566

#define MAP_HEIGHT      24    // default size of terminal window in num of chars
#define MAP_WIDTH       80
#define MAP_SIZE        (MAP_HEIGHT*MAP_WIDTH)

#define UP_KEY          'W'
#define LEFT_KEY        'A'
#define DOWN_KEY        'S'
#define RIGHT_KEY       'D'

void print_error(char *msg);

void exit_error(char *msg);

#endif