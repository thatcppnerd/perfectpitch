#include <ncurses.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>

#include <pthread.h>

#include "pp.h"
#include "notes.h"

#define MIN_COLS 40
#define MIN_ROWS 20

#define MAX_DIFFICULTY 5


WINDOW* win = NULL;     // containing window that holds the border/title 
WINDOW* content = NULL; // where the content goes :>

int difficulty = -1;

int cols = -1; // width
int rows = -1; // height

settings_t settings_data =  {
                                .volume = 100.f,
                                .debug_enabled = false
                            };


int init();
void quit();


int main_menu();
int play_menu();
int game_start(gamestate_t game_state);
int settings_menu();
int about_menu();

int play_tone(float freq);



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


int game_start(gamestate_t game_state)
{
    const char* difficulty_desc[MAX_DIFFICULTY] = {"Naturals", "Accidentals", "Naturals & Accidentals", "Chords", "Advanced Chords"};

    // game start screen 
    werase(content);
    wmove(content, 0, 0);

    wprintw(content, "You are playing on %s difficulty.\nYou will answer 20 questions about %s based on their sound.\n\nDo you have a GENIUS ear?", difficulty_desc[game_state.difficulty], difficulty_desc[game_state.difficulty]);
    mvwprintw(content, rows - 1, 0, "Press ENTER to start the game.");

    wrefresh(content);

    int input = 0;

    // wait for enter
    do
    {
        input = getch();
    }
    while(input != KEY_ENTER && input != '\n');

    switch(game_state.difficulty)
    {
        case 0: // Naturals
            while(1)
            {
                wmove(content, 0, 0);

                int questions = 20;

                // game loop
                for (int question = 1 ; question <= questions ; question++)
                {
                    werase(content);
                    wprintw(content, "Q%d/%d: What note is this?", question, questions);

                    wrefresh(content);

                    sleep(1);

                    play_tone(391.995);
                }
            }
        break;
    }
    return 0;
}

int play_tone(float freq)
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

void quit()
{
    // end the show
    delwin(win);
    endwin();
    refresh();
}

