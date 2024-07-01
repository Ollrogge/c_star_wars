#ifndef _TEXT_H
#define _TEXT_H
#include "object.h"
#include <SDL2/SDL_ttf.h>

typedef struct {
    object_t obj;
    SDL_Color color;
    TTF_Font* font_handle;
    char* text;
    bool updated;
} text_t;

int text_init(text_t*, game_state_t*, int, int, int, char*);
int text_update(text_t* text, game_state_t*, char* val);

#endif