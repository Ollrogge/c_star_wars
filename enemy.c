#include "enemy.h"
#include "util.h"

static game_status_t init(object_t*, game_state_t*);
static game_status_t update(object_t*, game_state_t*);
static game_status_t render(object_t*, game_state_t*);
static game_status_t destroy(object_t*, game_state_t*);
static game_status_t collision(object_t*, object_t*, game_state_t*);

#define IMG_PATH "./images/asteroid.png"

#define WIDTH 25
#define HEIGHT 25
#define SPEED 2

static SDL_Surface* g_img = NULL;

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy,
    .collision = collision
};

int enemy_init(enemy_t* enemy, game_state_t* game_state, int x, int y) {
    object_t* obj = &enemy->obj;

    obj->handler = &g_handler;
    obj->x = x;
    obj->y = y;
    obj->speed = SPEED;
    obj->width = WIDTH;
    obj->height = HEIGHT;
    obj->type = ENEMY;

    if (g_img == NULL) {
        g_img = IMG_Load(IMG_PATH);
        if (g_img == NULL) {
            printf("Enemy init: failed to load img: %s \n", IMG_GetError());
            return ERROR;
        }
    }

    return 0;
}

static game_status_t init(object_t* obj, game_state_t* game_state) {
    scene_t* scene = game_state->scene;
    return load_and_scale_texture(obj, scene, g_img, WIDTH, HEIGHT);
}

static game_status_t render(object_t* obj, game_state_t* game_state) {
    scene_t* scene = game_state->scene;
    game_status_t ret = OK;

    SDL_Rect src_rect1 = { 0, 0, obj->width, obj->height};
    SDL_Rect dst_rect1 = {obj->x, obj->y, obj->width, obj->height};
    ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect1, &dst_rect1);
    if (ret < 0) {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ERROR;
    }

    return ret;
}

static game_status_t update(object_t* obj, game_state_t* game_state) {
    scene_t* scene = game_state->scene;

    int height;
    SDL_GetWindowSize(scene->window, NULL, &height);

    obj->y += obj->speed;

    if (obj->y >= height) {
        return DESTROY;
    }

    return OK;
}

static game_status_t destroy(object_t* obj, game_state_t* game_state) {
    SDL_DestroyTexture(obj->texture);
    return 0;
}

static game_status_t collision(object_t* me, object_t* other, game_state_t* game_state) {
    if (other->type == BEAM) {
        return DESTROY;
    }
    return OK;
}