#ifndef __PP_H__
#define __PP_H__

#include <stdbool.h>

struct __settings
{
    double  volume;
    bool    debug_enabled;
};  

struct __gamestate
{
    unsigned int difficulty;
    unsigned int score;
    
    unsigned int correct;
    unsigned int incorrect;

    unsigned int time;
};

typedef struct __settings   settings_t;
typedef struct __gamestate  gamestate_t;

#endif