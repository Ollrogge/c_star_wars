#ifndef _ENEMY_H
#define _ENEMY_H
#include "scene.h"

typedef struct {
    object_t obj;
} enemy_t;

int enemy_init(enemy_t* beam, scene_t* scene, int x, int y);

#endif // _ENEMY_H