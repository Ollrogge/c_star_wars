#ifndef _ENEMY_H
#define _ENEMY_H
#include "scene.h"

typedef struct {
    object_t obj;
} enemy_t;

int enemy_init(enemy_t*, game_state_t*, int, int);

#endif // _ENEMY_H