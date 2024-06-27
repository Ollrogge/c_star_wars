
#ifndef _SCENE_H
#define _SCENE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "list.h"

typedef struct {
    int x;
    int y;
} movement_vec_t;

typedef struct object object_t;
typedef struct scene scene_t;

typedef struct {
    int (*init) (object_t* obj, scene_t*);
    int (*update) (object_t* obj, scene_t*, void* arg);
    int (*render) (object_t* obj, scene_t*);
    int (*destroy) (object_t* obj);
} object_handler_t;

struct object {
    list_head_t list;
    SDL_Texture* texture;
    int x;
    int y;
    unsigned speed;
    unsigned height;
    unsigned width;
    const object_handler_t* handler;
};

struct scene {
    SDL_Window* window;
    SDL_Renderer* renderer;
    unsigned speed;
    unsigned y;
    list_head_t objects;
};

int init_scene(scene_t* scene);
void destroy_scene(scene_t* scene);
int update_scene(scene_t* scene, movement_vec_t*);
void add_object_to_scene(scene_t* scene, object_t*  obj);


#endif