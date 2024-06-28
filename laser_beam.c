#include "laser_beam.h"
#include "util.h"

static object_status_t init(object_t*, scene_t*);
static object_status_t update(object_t*, scene_t*, void* arg);
static object_status_t render(object_t*, scene_t*);
static object_status_t destroy(object_t*);
static object_status_t collision(object_t*, object_t*);

#define IMG_PATH "./images/laser_beam.png"

static SDL_Surface* g_img = NULL;

#define WIDTH 5
#define HEIGHT 10
#define SPEED 2

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy,
    .collision = collision
};

int laser_beam_init(laser_beam_t* beam, scene_t* scene, int x, int y) {
    object_t* obj = &beam->obj;
    list_init(&obj->list);

    obj->handler = &g_handler;
    obj->x = x;
    obj->y = y;
    obj->speed = -SPEED;
    obj->type = BEAM;

    if (g_img == NULL) {
        g_img = IMG_Load(IMG_PATH);
        if (g_img == NULL) {
            printf("Enemy init: failed to load img: %s \n", IMG_GetError());
            return ERROR;
        }
    }

    return 0;
}

static object_status_t init(object_t* obj, scene_t* scene) {
    return load_and_scale_texture(obj, scene, g_img, WIDTH, HEIGHT);
}

static object_status_t render(object_t* obj, scene_t* scene) {
    object_status_t ret = OK;

    SDL_Rect src_rect1 = { 0, 0, obj->width, obj->height};
    SDL_Rect dst_rect1 = {obj->x, obj->y, obj->width, obj->height};
    ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect1, &dst_rect1);
    if (ret < 0) {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ERROR;
    }

    return ret;
}

static object_status_t update(object_t* obj, scene_t* scene, void* arg) {
    (void)scene;
    (void)arg;

    obj->y += obj->speed;

    if (obj->y + obj->height <= 0) {
        return DESTROY;
    }

    return OK;
}

static object_status_t destroy(object_t* obj) {
    SDL_DestroyTexture(obj->texture);
    return 0;
}

static object_status_t collision(object_t* me, object_t* other) {
    if (other->type == ENEMY) {
        return DESTROY;
    }

    return OK;
}