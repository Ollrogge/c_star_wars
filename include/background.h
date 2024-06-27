#ifndef _BACKGROUND_H
#define _BACKGROUND_H
#include "scene.h"

typedef struct {
    object_t obj;
} background_t;

int background_init(background_t*, scene_t*);

#endif // _BACKGROUND_H