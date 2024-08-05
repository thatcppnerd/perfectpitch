#ifndef __PP_H__
#define __PP_H__

struct __settings
{
    double volume;
};

struct __gamestate
{
    unsigned int difficulty;
    unsigned int score;
    
    unsigned int correct;
    unsigned int incorrect;
};

typedef struct __settings   settings_t;
typedef struct __gamestate  gamestate_t;

#endif