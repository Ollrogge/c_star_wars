#ifndef _BACKGROUND_H
#define _BACKGROUND_H
#include "object.h"

typedef struct {
    object_t obj;
} background_t;

int background_init(background_t*, game_state_t*);

#endif // _BACKGROUND_H