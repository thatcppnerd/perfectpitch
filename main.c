#include <ncurses.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>

#include "lib/pp.h"

#define MIN_COLS 40
#define MIN_ROWS 20

#define MAX_DIFFICULTY 5


WINDOW* win = NULL;     // containing window that holds the border/title 
WINDOW* content = NULL; // where the content goes :>

int difficulty = -1;

int cols = -1; // width
int rows = -1; // height

settings_t settings_data =  {
                                .volume = 100.f
                            };


int init();

int main_menu();
int play_menu();
int game_start(gamestate_t game_state);
int settings_menu();
int sound_test(float freq);
int about_menu();

void quit();


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
    else
    {
        difficulty = 0;
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

        // goto main menu
        main_menu();

        // delete content window on exit
        delwin(content);
    }

    // end the show
    quit();

    return 0;
}

int init()
{
    // start the show
    win = initscr();

    if (win == NULL) // check if the star roles aren't vacant
    {
        printf("Ncurses failed to initialize.\n"); // She left?!
        return 1;
    }
    else if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        printf("SDL failed to intiialize.\n");
        return 1;
    }
    else if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        printf("Mixer failed to initialize.\n");
    }

    // do extra stuff for setup
    curs_set(0);            // make cursor invisible
    keypad(stdscr, TRUE);   // enable arrow keys
    noecho();               // keep user keypresses from making junk on screen
    cbreak();               // 

    
    // initialize color pairs
    start_color();
        
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    
    return 0;
}

int main_menu()
{
    // const char* title = "PERFECT PITCH"; // 13 chars wide
    const unsigned int main_menu_num_items = 4;
    const char* main_menu_item[] = { "Play", "Settings", "About", "Quit" };

    unsigned int select = 0;

    // draw screen border with 'PERFECT PITCH' top-center
    border('|', '|', '-', '-', '+', '+', '+', '+');

    wmove(win, 0, ceil(cols / 2) - 7);
    wprintw(win, "PERFECT PITCH");

  
    
    while(1) // menu loop
    {
        wmove(content, 0, 0);
        wprintw(content, "== MAIN MENU =="); 
        
        // print menu items
        for (int i = 0 ; i < main_menu_num_items ; i++)
        {
            wmove(content, 2 + i, 0);
            if (select == i) // highlight if selected
            {
                wattron(content, COLOR_PAIR(2));
            }
            // if not, print normally
            
            wprintw(content, "%s", main_menu_item[i]);
            wattroff(content, COLOR_PAIR(2));
        }
        
        // show 
        wrefresh(content);
        
        // wait for keyboard input
        int input = getch();
        
        // check if 
        if (input == KEY_UP) select--;
        else if (input == KEY_DOWN) select++;
        else if(input == KEY_ENTER || input == '\n') // option has been chosen
        {
            switch(select)
            {
                case 0: // play_menu
                    play_menu();
                break;

                case 1: // settings
                    settings_menu();
                break;

                case 2: // about_menu 
                    about_menu();
                break;

                case 3: // quit
                    return 0;
                break;
            }
        }

        // keep select in bounds
        select %= main_menu_num_items;

        werase(content);
    }

    return 0;
}

int play_menu()
{
    const int play_menu_num_items = 3;
    const char* play_menu_item[3] = {"Start Game", "Difficulty", "Back"};

    const char* difficulty_desc[MAX_DIFFICULTY] = {"Naturals", "Accidentals", "Naturals & Accidentals", "Chords", "Advanced Chords"};
    unsigned int select = 0;

    werase(content);

    // menu loop
    while(1)
    {
        wmove(content, 0, 0);

        wprintw(content, "== PLAY ==");

        for (int i = 0 ; i < play_menu_num_items ; i++)
        {
            wmove(content, 2 + i, 0);

            // highlight if selected
            if(select == i)
            {
                wattron(content, COLOR_PAIR(2));
            }

            wprintw(content, "%s", play_menu_item[i]);

            wattroff(content, COLOR_PAIR(2));

            switch(i)
            {
                case 1: // difficulty
                    wprintw(content, ": %u (%s)", difficulty, difficulty_desc[difficulty]);
                break;

                default: break;
            }
        }

        wrefresh(content);

        // inputs
        int input = getch();

        if (input == KEY_UP) select--;
        else if (input == KEY_DOWN) select++;
        else if (input == KEY_ENTER || input == '\n')
        {   
            switch(select)
            {
                case 0: // start game
                    gamestate_t game_state =    {
                                                    .difficulty = difficulty,
                                                    .score = 0,
                                                    .correct = 0,
                                                    .incorrect = 0
                                                };
                    game_start(game_state);
                break;

                case 1: // difficulty

                    // difficulty edit loop
                    while(1)
                    {
                        wmove(content, 3, 0);   // move to difficulty
                        wclrtoeol(content);     // clear line

                        wprintw(content, "%s: ", play_menu_item[1]);

                        wattron(content, COLOR_PAIR(2));
                        wprintw(content, "%u (%s)", difficulty, difficulty_desc[difficulty]);
                        wattroff(content, COLOR_PAIR(2));

                        wrefresh(content);

                        int input = getch();

                        if (input == KEY_LEFT && difficulty > 0) 
                        {
                            difficulty--;
                        }
                        else if (input == KEY_RIGHT && difficulty < (MAX_DIFFICULTY - 1)) 
                        {
                            difficulty++;
                        }
                        else if (input == KEY_ENTER || input == '\n') 
                        {
                            break; 
                        }
                    }
                break;

                case 2: // back
                    return 0;
                break;
            }
        }

        // keep in bounds
        select %= play_menu_num_items;

        werase(content);

    }
    return 0;
}

int game_start(gamestate_t game_state)
{

    return 0;
}

int settings_menu()
{
    const unsigned int settings_menu_num_items = 3;
    const char* settings_menu_item[3] = { "Volume", "Sound Test", "Back" };
    unsigned int select = 0;



    // menu loop
    while(1)
    {   
        // reset screen 
        werase(content);
        wmove(content, 0, 0);

        // menu titles
        wprintw(content, "== SETTINGS ==");

        // print menu
        for (int i = 0 ; i < settings_menu_num_items ; i++)
        {
            wmove(content, i + 2, 0);

            // highlight selected item
            if (select == i)
            {
                wattron(content, COLOR_PAIR(2));
            }

            // print setting name
            wprintw(content, "%s", settings_menu_item[i]);
            wattroff(content, COLOR_PAIR(2));

            // print value of setting
            switch(i)
            {
                case 0: // volume
                    wprintw(content, ": %.1f", settings_data.volume);
                break;

                default: break;
            }
        }

        wrefresh(content);

        // usleep(50000);

        int input = getch();

        // get inputs
        if (input == KEY_UP) select--;
        else if (input == KEY_DOWN) select++;
        else if (input == KEY_ENTER || input == '\n')
        {   
            // reprint selected item as normal and highlight the value to show it is being edited
            switch(select)
            {
                case 0: // volume

                    // value edit loop
                    while(1)
                    {
                        wclrtoeol(content); // clear line
                        mvwprintw(content, 2, 0, "%s: ", settings_menu_item[0]); // reprint name with un-highlighted version

                        // reprint value with highlighted version
                        wattron(content, COLOR_PAIR(2));
                        wprintw(content, "%.1f", settings_data.volume);
                        wattroff(content, COLOR_PAIR(2));

                        wrefresh(content);

                        int input = getch();

                        // get inputs
                        if (input == KEY_LEFT && 0.f < settings_data.volume) // decrease
                        {
                            settings_data.volume -= .5;
                        }
                        else if (input == KEY_RIGHT && 100.f > settings_data.volume) // increase
                        {
                            settings_data.volume += .5;
                        }
                        else if (input == KEY_ENTER || input == '\n') // done editing
                        {
                            break; // exit edit loop
                        }
                    }
                break;
                
                case 1:
                    sound_test(440.f);
                break;

                case 2: // quit
                    wrefresh(content);

                    return 0; // 
                break;
                    
            }
        }
        
        // keep select in bounds
        select %= settings_menu_num_items;

    }

    return 0;
}

int sound_test(float freq)
{
    SDL_AudioDeviceID audio_device;

    SDL_AudioSpec audio_spec;
    SDL_zero(audio_spec);

    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 1;
    audio_spec.samples = 2048;
    audio_spec.callback = NULL;

    audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

    double x = 0.;
    for(int s = 0 ; s < (audio_spec.freq * 3) ; s++)
    {
        x += freq/44100.f;

        short sample = sin((2. * M_PI) * x) * (300. * settings_data.volume);
        SDL_QueueAudio(audio_device, &sample, sizeof(short));
    }

    SDL_PauseAudioDevice(audio_device, 0);

    SDL_Delay(3000);

    SDL_CloseAudioDevice(audio_device);

    return 0;
}

int about_menu()
{ 
    werase(content);
    wmove(content, 0, 0);

    waddstr(content, "Perfect Pitch was written as a quick, easy way for me to ear train myself, while also giving me an excuse to program something using ncurses, SDL, and C, haha.\n\n");
    waddstr(content, "Perfect Pitch was written between 8/2/24 to 8/5/24.\n\n");

    // draw 'Back' selection(it is the only selection)
    wattron(content, COLOR_PAIR(2));
    wprintw(content, "Back");
    wattroff(content, COLOR_PAIR(2));

    wrefresh(content);

    // usleep(500000); // give small delay in input (0.4s)

    int input = 0;
    do
    {
        input = getch();
    }
    while(input != KEY_ENTER && input != '\n');

    werase(content);

    wrefresh(content);

    return 0;
}

void quit()
{
    // end the show
    delwin(win);
    endwin();
    refresh();
}