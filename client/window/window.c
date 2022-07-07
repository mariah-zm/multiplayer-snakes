#include "window.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../core/core.h"
#include "../clientcore/clientcore.h"

// Private constants for centering vertically on screen
size_t const WELCOME_HEIGTH = 20;
size_t const MID_Y = (WIN_HEIGHT-WELCOME_HEIGTH)/2;

// Private constants for centering menu options horizontally on screen
size_t const MENU_WIDTH = 13;
size_t const MENU_MID_X = (WIN_WIDTH-MENU_WIDTH)/2;

void print_title(WINDOW *window);
void print_menu(WINDOW *window, size_t option);
void print_instructions(WINDOW *window);
void update_menu(WINDOW *window, size_t option);

void init_ncurses()
{
    initscr();      // Start curses mode

    noecho();       // Switching off echo so that WASD are not displayed
    cbreak();       // Disables line buffering

    start_color();
    use_default_colors();    
    curs_set(0);

    // Initialise colour pairs
    init_pair(MAIN_COL, COLOR_WHITE, COLOR_BLACK);
    init_pair(TITLE_COL, COLOR_GREEN, COLOR_BLACK);
    init_pair(OPTIONS_COL, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SCORE_COL, COLOR_WHITE, COLOR_MAGENTA);

    // Snake Colours
    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_RED);
    init_pair(5, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);
    init_pair(7, COLOR_BLACK, COLOR_WHITE);
}

WINDOW *create_game_window()
{
    WINDOW *game_window = newwin(WIN_HEIGHT, WIN_WIDTH, 0, 0);
    box(game_window, 0 , 0);
    wbkgd(game_window, COLOR_PAIR(MAIN_COL));
    wrefresh(game_window);

    return game_window;
}

void destroy_game_window(WINDOW *window)
{
    werase(window);
    delwin(window);
    echo(); 
    curs_set(1);  
    endwin();
}

size_t show_menu(WINDOW *window)
{
    bool exit_menu = false;
    size_t option = 1;
    char key_pressed;

    print_menu(window, option);

    do
    {
        key_pressed = wgetch(window);

        if (key_pressed == ENTER)
            exit_menu = true;
        else if (key_pressed = '\033')
        {
            wgetch(window); // skip '['

            key_pressed = wgetch(window);
            
            if (key_pressed == UP_ARROW)
            {
                if (option > 1) update_menu(window, --option);
            }
            else if (key_pressed == DOWN_ARROW)
                if (option < 3) update_menu(window, ++option);
        }
    } while (!exit_menu);

    return option;
}

void show_instructions(WINDOW *window)
{
    bool exit_menu = false;
    char key_pressed;

    print_instructions(window);

    do
    {
        key_pressed = wgetch(window);

        if (key_pressed == BACKSPACE || key_pressed == DELETE)
            exit_menu = true;
    } while (!exit_menu);
}

void show_winner(int game_status)
{

}

void show_game_screen(WINDOW *window, game_map_t map)
{    
    werase(window);

    // Print header with score
    wattron(window, COLOR_PAIR(SCORE_COL));
    mvwprintw(window, 0, 0, "Score: %-73d", map[DATA_ROW][SCORE_IDX]);
    wattroff(window, COLOR_PAIR(SCORE_COL));

    // Printing values in map (ignoring data row)
    for (size_t y = 0; y < MAP_HEIGHT; ++y)
    {
        for (size_t x = 0; x < MAP_WIDTH; ++x)
        {
            int current_value = map[y][x];
            int colour = abs(current_value) % 7;

            if (current_value == FRUIT)
            {
                wattron(window, COLOR_PAIR(COLOR_BLACK));
                mvwprintw(window, y+1, x, "o"); 
                wattroff(window, COLOR_PAIR(COLOR_BLACK));
            }
            else if (current_value == EMPTY)
            {
                wattron(window, COLOR_PAIR(COLOR_BLACK));
                mvwprintw(window, y+1, x, " "); 
                wattroff(window, COLOR_PAIR(COLOR_BLACK));
            } 
            else // Player
            {
                wattron(window, COLOR_PAIR(colour));

                // Checking if snake head
                if (map[y][x] < 0)
                    mvwprintw(window, y+1, x, ".");
                else 
                    mvwprintw(window, y+1, x, " "); 

                wattroff(window, COLOR_PAIR(colour));
            }
        }
    }

    wrefresh(window);
}

/*****************************************************************************
 * PRIVATE FUNCTIONS
 *****************************************************************************/

void print_title(WINDOW *window)
{
    // Constants for centering horizontally
    size_t const TITLE_WIDTH = 35;
    size_t const TITLE_MID_X = (WIN_WIDTH-TITLE_WIDTH)/2;

    wattron(window, COLOR_PAIR(TITLE_COL)); 
    mvwprintw(window, MID_Y, TITLE_MID_X, 
                        "   _____             __");
    mvwprintw(window, MID_Y + 1, TITLE_MID_X, 
                        "  / ___/____  ____ _/ /_____  _____");
    mvwprintw(window, MID_Y + 2, TITLE_MID_X, 
                        "  \\__ \\/ __ \\/ __ `/ //_/ _ \\/ ___/");
    mvwprintw(window, MID_Y + 3, TITLE_MID_X, 
                        " ___/ / / / / /_/ / ,< /  __(__  )");
    mvwprintw(window, MID_Y + 4, TITLE_MID_X, 
                        "/____/_/ /_/\\__,_/_/|_|\\___/____/");
    wattroff(window, COLOR_PAIR(TITLE_COL));
}

void print_menu(WINDOW *window, size_t option)
{
    // Constants for centering horizontally
    size_t const MENU_INS1_WIDTH = 49;   
    size_t const MENU_INS1_MID_X = (WIN_WIDTH-MENU_INS1_WIDTH)/2;
    size_t const MENU_INS2_WIDTH = 28;   
    size_t const MENU_INS2_MID_X = (WIN_WIDTH-MENU_INS2_WIDTH)/2;

    werase(window);
    print_title(window);
    
    // Menu
    wattron(window, COLOR_PAIR(OPTIONS_COL));
    mvwprintw(window, MID_Y + 7, MENU_MID_X, "> Join Game");
    wattroff(window, COLOR_PAIR(OPTIONS_COL));
    mvwprintw(window, MID_Y + 8, MENU_MID_X, "  How to Play");
    mvwprintw(window, MID_Y + 9, MENU_MID_X, "  Exit");

    // Menu Instructions
    mvwprintw(window, MID_Y + 12, MENU_INS1_MID_X, 
                        "Navigate options using the UP and DOWN arrow keys");
    mvwprintw(window, MID_Y + 13, MENU_INS2_MID_X, 
                        "Press ENTER to choose option");
    
    wrefresh(window);
}

void print_instructions(WINDOW *window)
{
    // Constants for centering horizontally on screen
    size_t const INSTRUCTIONS_WIDTH = 58;
    size_t const INS_MID_X = (WIN_WIDTH-INSTRUCTIONS_WIDTH)/2;
    size_t const INS_INS_WIDTH = 26;
    size_t const INS_INS_MID_X = (WIN_WIDTH-INS_INS_WIDTH)/2;

    werase(window);
    print_title(window);

    mvwaddstr(window, MID_Y + 7, INS_MID_X, 
                "How to Play");
    mvwprintw(window, MID_Y + 9, INS_MID_X, 
                "- Change the direction of movement by pressing the");
    mvwprintw(window, MID_Y + 10, INS_MID_X, 
                "  W, A, S, D keys.");
    mvwprintw(window, MID_Y + 11, INS_MID_X, 
                "- Fruit will appear randomly on the screen. Eat them to");
    mvwprintw(window, MID_Y + 12, INS_MID_X, 
                "  grow and earn points.");
    mvwprintw(window, MID_Y + 13, INS_MID_X, 
                "- If you reach 13 points first, you win.");
    mvwprintw(window, MID_Y + 14, INS_MID_X, 
                "- If you collide with the walls, other snakes or yourself,"); 
    mvwprintw(window, MID_Y + 15, INS_MID_X, 
                "  you die.");        
    mvwprintw(window, MID_Y + 16, INS_MID_X, 
                "- To quit, press Q at any time during the game.");

     mvwprintw(window, MID_Y + 19, INS_INS_MID_X, 
                "Press BACKSPACE to go back");
    
    wrefresh(window);
}

void update_menu(WINDOW *window, size_t option)
{
    if (option == 1)
    {   
        wattron(window, COLOR_PAIR(OPTIONS_COL)); 
        mvwprintw(window, MID_Y + 7, MENU_MID_X, "> Join Game");
        wattroff(window, COLOR_PAIR(OPTIONS_COL)); 
        mvwprintw(window, MID_Y + 8, MENU_MID_X, "  How to Play");
        mvwprintw(window, MID_Y + 9, MENU_MID_X, "  Exit");
    }
    else if (option == 2)
    {
        mvwprintw(window, MID_Y + 7, MENU_MID_X, "  Join Game");
        wattron(window, COLOR_PAIR(OPTIONS_COL));
        mvwprintw(window, MID_Y + 8, MENU_MID_X, "> How to Play");
        wattroff(window, COLOR_PAIR(OPTIONS_COL));
        mvwprintw(window, MID_Y + 9, MENU_MID_X, "  Exit");
    }
    else if (option == 3)
    {
        mvwprintw(window, MID_Y + 7, MENU_MID_X, "  Join Game");
        mvwprintw(window, MID_Y + 8, MENU_MID_X, "  How to Play");
        wattron(window, COLOR_PAIR(OPTIONS_COL));
        mvwprintw(window, MID_Y + 9, MENU_MID_X, "> Exit");
        wattroff(window, COLOR_PAIR(OPTIONS_COL));
    }

    wrefresh(window);
}