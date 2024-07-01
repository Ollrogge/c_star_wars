#include "background.h"
#include "constants.h"
#include "scene.h"

#define IMG_PATH "./images/background.png"

static game_status_t init(object_t*, game_state_t* arg);
static game_status_t update(object_t*, game_state_t* arg);
static game_status_t render(object_t*, game_state_t* arg);
static game_status_t destroy(object_t*, game_state_t* arg);
static game_status_t collision(object_t*, object_t*, game_state_t* arg);

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy,
    .collision = collision
};

int background_init(background_t* background, game_state_t* state) {
    background->obj.handler = &g_handler;
    background->obj.speed = 0;
    background->obj.x = 0;
    background->obj.y = 0;

    return 0;
}

static game_status_t init(object_t* obj, game_state_t* state) {
    int ret;
    background_t* background = (background_t *)obj;
    scene_t* scene = state->scene;

    SDL_Surface* img = IMG_Load(IMG_PATH);
    if (img == NULL) {
        printf("Background.c: unable to load image!: %s \n", IMG_GetError());
        return ERROR;
    }

    int height, width;
    SDL_GetWindowSize(scene->window, &width, &height);

    // Calculate the scaling factor to fit the image within the window
    float scaleX = (float)width / img->w;
    float scaleY = (float)height / img->h;

    // Calculate the new dimensions for the scaled image
    int new_width = (int)(img->w * scaleX);
    int new_height = (int)(img->h * scaleY);

    printf("Scaled dimensions: %d %d %f %f\n", new_width, new_height, scaleY, scaleX);

    SDL_Rect dstRect;
    dstRect.x = (width - new_width) / 2;  // Center the image horizontally
    dstRect.y = (height - new_height) / 2;  // Center the image vertically
    dstRect.w = new_width;
    dstRect.h = new_height;

    obj->height = new_height;
    obj->width = new_width;

    SDL_Surface* scaled_surface = SDL_CreateRGBSurface(0, obj->height, obj->width, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (scaled_surface == NULL) {
        SDL_FreeSurface(img);
        printf("Scaled surface failed, error: %s \n", SDL_GetError());
        return ERROR;
    }

    ret = SDL_BlitScaled(img, NULL, scaled_surface, &dstRect);
    if (ret < 0) {
        printf("background.c, blitscaled failed \n");
        SDL_FreeSurface(img);
        SDL_FreeSurface(scaled_surface);
        return ERROR;
    }

    background->obj.texture = SDL_CreateTextureFromSurface(scene->renderer, scaled_surface);

    SDL_FreeSurface(img);
    SDL_FreeSurface(scaled_surface);

    if (background->obj.texture == NULL) {
        printf("Failed to create txxture from surface for background: %s \n", SDL_GetError());

        return ERROR;
    }

    return OK;
}

static game_status_t update(object_t* obj, game_state_t* state) {
    scene_t* scene = state->scene;
    obj->y = (scene->speed + obj->y) % obj->height;
    return OK;
}

static game_status_t render(object_t* obj, game_state_t* state) {
    scene_t* scene = state->scene;
    int ret = OK;

    // Render the first part of the background image
    SDL_Rect src_rect1 = { 0, 0, obj->width, obj->height - obj->y};
    SDL_Rect dst_rect1 = { 0, obj->y, obj->width, obj->height - obj->y };
    ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect1, &dst_rect1);
    if (ret < 0) {
        printf("background.c (1), SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ERROR;
    }

    // Render the second part of the background image if needed
    // (the part that wraps around)
    if (obj->y > 0) {
        SDL_Rect src_rect2 = { 0, obj->height - obj->y, obj->width, obj->y };
        SDL_Rect dst_rect2 = { 0, 0, obj->width, obj->y};
        ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect2, &dst_rect2);
        if (ret < 0) {
            printf("background.c (2), SDL_RenderCopy Error: %s\n", SDL_GetError());
            return ERROR;
        }
    }

    return ret;
}

static game_status_t destroy(object_t* obj, game_state_t* state) {
    (void)state;
    SDL_DestroyTexture(obj->texture);
    return OK;
}


static game_status_t collision(object_t* me, object_t* other, game_state_t* state) {
    (void)me;
    (void)other,
    (void)state;
    return OK;
}