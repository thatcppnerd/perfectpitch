#include <ncurses.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <assert.h>

#define MIN_COLS 40
#define MIN_ROWS 20  


WINDOW* win = NULL;     // containing window that holds the border/title 
WINDOW* content = NULL; // where the content goes :>

int difficulty = -1;

int cols = -1; // width
int rows = -1; // height

int init();
int main_menu();

int main(int argc, char* argv[])
{
    if (argc > 2) // check if arguments are valid
    {
        printf("Too many arguments\nValid usage:\n%s [difficulty]\n", argv[0]);
        return 1;
    }
    else if (argc == 2) // if user specified difficulty
    {
        difficulty = atoi(argv[1]);
    }


    if (init() != 0)
    {
        return 1;
    }

    // check if terminal dimensions are adequate
    getmaxyx(win, rows, cols);

    if (cols < MIN_COLS || rows < MIN_ROWS) // is adequate?
    {
        endwin(); // end the show due to technical difficulties :<
        printf  
        ( 
            "Size of terminal inadqeuate.\nMinimum size required is %d ROWS, & %d COLUMNS.\nYour size was %d ROWS, & %d COLUMNS.\nToo bad!\n",
            MIN_ROWS, MIN_COLS,
            cols, rows
        );

        // put "size helper" tool here?
        // it'd be cute

        return 1;
    }
    else // checks are done
    {
        // create content window, now error checking is complete
        content = subwin(win, rows - 2, cols - 2, 1, 1);

        // do extra stuff for setup
        curs_set(0);            // make cursor invisible
        keypad(stdscr, TRUE);   // enable arrow keys
        
        // goto main menu
        main_menu();

        // delete content window on exit
        delwin(content);
    }

    

    

    // end the show
    delwin(win);
    endwin();
    refresh();

    return 0;
}

int init()
{
    // start the show
    win = initscr();

    if (win == NULL) // check if the star role isn't vacant
    {
        printf("Ncurses failed to initialize.\n"); // She left?!
        return 1;
    }
    
    // initialize color pairs
    start_color();
        
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    
    return 0;
}

int main_menu()
{
    const char* title = "PERFECT PITCH"; // 13 chars wide

    // draw screen border with 'PERFECT PITCH' top-center
    border('|', '|', '-', '-', '+', '+', '+', '+');

    wmove(win, 0, ceil(cols / 2) - 10);
    wprintw(win, title);

    // draw select screen
    wmove(content, 0, 0);
    wprintw(content, "== MAIN MENU =="); 
    
    wmove(content, 2, 0);
    

    int select = 0;
    
    



    refresh();
    
    


    getch();
    return 0;
}