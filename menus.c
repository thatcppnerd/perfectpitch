#include "main.c"


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
                                                    .incorrect = 0,
                                                    .time = 0
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

int settings_menu()
{
    const unsigned int settings_menu_num_items = 4;
    const char* settings_menu_item[4] = { "Volume", "Sound Test", "Debug Mode", "Back" };
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

                case 2: // debug mode
                    wprintw(content, ": %s", settings_data.debug_enabled ? "True" : "False");
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
                    play_tone(440.f);
                break;

                case 2: // debug mode toggle
                    settings_data.debug_enabled = !settings_data.debug_enabled;
                break;

                case 3: // quit
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


