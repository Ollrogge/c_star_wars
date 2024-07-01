
#ifndef _SCENE_H
#define _SCENE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "list.h"
#include "object.h"
#include "text.h"

typedef struct scene scene_t;

#define SCORE_BUF_SIZE 0x40

typedef struct {
    int val;
    char buf[SCORE_BUF_SIZE];
    text_t* text;
} score_t;

struct scene {
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint32 last_enemy_spawn;
    unsigned speed;
    unsigned y;
    score_t score;
    list_head_t objects;
    unsigned objects_cnt;
};


game_status_t init_scene(scene_t*, game_state_t*);
void destroy_scene(scene_t*, game_state_t*);
int update_scene(scene_t*, game_state_t*);
game_status_t add_object_to_scene(object_t*, scene_t*, game_state_t*);
game_status_t del_object_from_scene(object_t* obj, scene_t* scene, game_state_t* state);
game_status_t init_text_for_scene(scene_t*, game_state_t*);


#endif