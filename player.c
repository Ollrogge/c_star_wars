#include "player.h"
#include "constants.h"
#include "util.h"
#include "laser_beam.h"

static object_status_t init(object_t*, scene_t*);
static object_status_t update(object_t*, scene_t*, void* arg);
static object_status_t render(object_t*, scene_t*);
static object_status_t destroy(object_t*);
static object_status_t collision(object_t*, object_t*);

#define IMG_PATH "./images/space-ship.png"

static SDL_Surface* g_img = NULL;

#define WIDTH 50
#define HEIGHT 50
#define SPEED 3

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy,
    .collision = collision
};

int player_init(player_t* player, scene_t* scene) {
    object_t* obj = (object_t*)player;
    int height, width;
    SDL_GetWindowSize(scene->window, &width, &height);
    list_init(&obj->list);
    obj->handler = &g_handler;
    obj->speed = SPEED;
    obj->x = width / 2 - WIDTH / 2;
    obj->y = height - HEIGHT;
    obj->type = PLAYER;

    if (g_img == NULL) {
        g_img = IMG_Load(IMG_PATH);
        if (g_img == NULL) {
            printf("Player init: failed to load img: %s \n", IMG_GetError());
            return ERROR;
        }
    }

    return OK;
}

static object_status_t init(object_t* obj, scene_t* scene) {
    return load_and_scale_texture(obj, scene, g_img, WIDTH, HEIGHT);
}

static object_status_t render(object_t* obj, scene_t* scene) {
    int ret = OK;

    SDL_Rect src_rect1 = { 0, 0, obj->width, obj->height};
    SDL_Rect dst_rect1 = {obj->x, obj->y, obj->width, obj->height};
    ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect1, &dst_rect1);
    if (ret < 0) {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ERROR;
    }

    return ret;
}

static void _handle_movement(player_t *player, scene_t* scene, movement_vec_t* movement) {
    int height, width;
    SDL_GetWindowSize(scene->window, &width, &height);
    object_t *obj = &player->obj;

    if (movement->y != 0 || movement->x != 0) {
        player->moving_delay = TICK_RATE / 10;
    }
    else {
        player->moving_delay -= 1;
    }

    // update position
    if (player->moving_delay >= 0) {
        // turn of scene movement when we are moving else player movement is jerky
        // and if tickrate high player wont even move
        obj->y += movement->y * obj->speed;
    }
    else {
        obj->y += scene->speed;
    }
    obj->x = obj->x + movement->x * obj->speed;

    if (obj->y + obj->height >= height) {
        obj->y = height - obj->height;
    }
    else if (obj->y <= 0) {
        obj->y = 0;
    }

    if (obj->x + obj->width >= width) {
        obj->x = width - obj->width;
    }
    else if (obj->x <= 0) {
        obj->x = 0;
    }
}

static object_status_t _handle_shooting(player_t* player, scene_t* scene, bool mouse_used) {
    if (player->shooting_delay >= 0) {
        player->shooting_delay -= 1;
        return OK;
    }

    if (!mouse_used) {
        return OK;
    }

    object_t* obj = &player->obj;
    player->shooting_delay = TICK_RATE / 2;
    int ret = 0;
    int x = 0;

    laser_beam_t* laser1 = (laser_beam_t*)calloc(1, sizeof(laser_beam_t));
    x = obj->x;
    laser_beam_init(laser1, scene, x, obj->y);

    laser_beam_t* laser2 = (laser_beam_t*)calloc(1, sizeof(laser_beam_t));
    x = obj->x + obj->width - obj->width / 8;
    laser_beam_init(laser2, scene, x, obj->y);

    ret = add_object_to_scene(scene, &laser1->obj);
    ret = add_object_to_scene(scene, &laser2->obj);

    return ret;
}

static object_status_t update(object_t* obj, scene_t* scene, void* arg) {
    game_state_t* game_state = (game_state_t*)arg;
    movement_vec_t* movement = &game_state->movement;
    player_t* player = (player_t*)obj;

    int ret;
    _handle_movement(player, scene, movement);
    ret = _handle_shooting(player, scene, game_state->mouse_used);
    return ret;
}

static object_status_t destroy(object_t* obj) {
    SDL_DestroyTexture(obj->texture);
    return OK;
}

static object_status_t collision(object_t* me, object_t* other) {
    if (other->type == ENEMY) {
        return DESTROY;
    }
    return OK;
}