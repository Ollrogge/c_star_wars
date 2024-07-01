#ifndef _OBJECT_H
#define _OBJECT_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "list.h"

struct scene;
typedef struct scene scene_t;

typedef struct {
    int x;
    int y;
} movement_vec_t;

typedef struct {
    movement_vec_t movement;
    bool mouse_used;
    scene_t* scene;
} game_state_t;

typedef enum {
    ERROR = -1,
    OK,
    DESTROY,
    GAME_OVER
} game_status_t;

typedef enum {
    OTHER,
    PLAYER,
    ENEMY,
    BEAM,
} object_type_t;

typedef struct object object_t;

typedef struct {
    game_status_t (*init) (object_t* obj, game_state_t* arg);
    game_status_t (*update) (object_t* obj, game_state_t* arg);
    game_status_t (*render) (object_t* obj, game_state_t* arg);
    game_status_t (*destroy) (object_t* obj, game_state_t* arg);
    game_status_t (*collision) (object_t*, object_t*, game_state_t* arg);
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

#endif