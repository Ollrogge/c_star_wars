
#ifndef _SCENE_H
#define _SCENE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "list.h"

typedef enum {
    ERROR = -1,
    OK,
    DESTROY,
    GAME_OVER
} object_status_t;

typedef struct {
    int x;
    int y;
} movement_vec_t;

typedef struct {
    movement_vec_t movement;
    bool mouse_used;
} game_state_t;

typedef enum {
    OTHER,
    PLAYER,
    ENEMY,
    BEAM,
} object_type_t;

typedef struct object object_t;
typedef struct scene scene_t;

typedef struct {
    object_status_t (*init) (object_t* obj, scene_t*);
    object_status_t (*update) (object_t* obj, scene_t*, void* arg);
    object_status_t (*render) (object_t* obj, scene_t*);
    object_status_t (*destroy) (object_t* obj);
    object_status_t (*collision) (object_t*, object_t*);
} object_handler_t;

struct object {
    list_head_t list;
    SDL_Texture* texture;
    object_type_t type;
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
    Uint32 last_enemy_spawn;
    unsigned speed;
    unsigned y;
    list_head_t objects;
};


int init_scene(scene_t* scene);
void destroy_scene(scene_t* scene);
int update_scene(scene_t* scene, game_state_t*);
object_status_t add_object_to_scene(scene_t* scene, object_t*  obj);


#endif