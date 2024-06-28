#ifndef _PLAYER_H
#define _PLAYER_H
#include <stdbool.h>
#include "scene.h"

typedef struct {
    object_t obj;
    // scene ticks are faster then keyboard events so implement sth like a delay
    // before we accept scene movement again
    int moving_delay;
    int shooting_delay;
} player_t;

int player_init(player_t*, scene_t*);

#endif // _PLAYER_H