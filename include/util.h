#ifndef _UTIL_H
#define _UTIL_H
#include <stddef.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "scene.h"

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})


game_status_t load_and_scale_texture(object_t* obj, scene_t* scene, SDL_Surface*, int width, int height);

bool rectangle_overlap(object_t* obj1, object_t* obj2);

#endif // _UTIL_H