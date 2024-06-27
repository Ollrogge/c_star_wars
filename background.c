#include "background.h"
#include "constants.h"

#define IMG_PATH "./images/background.png"

static int init(object_t*, scene_t*);
static int update(object_t*, scene_t*);
static int render(object_t*, scene_t*);
static int destroy(object_t*);

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy
};

int background_init(background_t* background, scene_t* scene) {
    background->obj.handler = &g_handler;
    background->obj.speed = 2;
    background->obj.x = 0;
    background->obj.y = 0;

    return 0;
}

static int init(object_t* obj, scene_t* scene) {
    background_t* background = (background_t *)obj;

    SDL_Surface* img = IMG_Load(IMG_PATH);
    if (img == NULL) {
        printf("Background.c: unable to load image!: %s \n", IMG_GetError());
        return -1;
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
        printf("Scaled surface failed\n");
        return -1;
    }

    SDL_BlitScaled(img, NULL, scaled_surface, &dstRect);

    background->obj.texture = SDL_CreateTextureFromSurface(scene->renderer, scaled_surface);

    SDL_FreeSurface(img);
    SDL_FreeSurface(scaled_surface);

    if (background->obj.texture == NULL) {
        printf("Failed to create tecture from surface for background: %s \n", SDL_GetError());

        return -1;
    }

    return 0;
}

static int update(object_t* obj, scene_t* scene) {
    obj->y = (obj->speed + obj->y) % obj->height;
    return 0;
}

static int render(object_t* obj, scene_t* scene) {
    int ret = 0;

    // Render the first part of the background image
    SDL_Rect src_rect1 = { 0, 0, obj->width, obj->height - obj->y};
    SDL_Rect dst_rect1 = { 0, obj->y, obj->width, obj->height - obj->y };
    ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect1, &dst_rect1);
    if (ret < 0) {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ret;
    }

    // Render the second part of the background image if needed
    // (the part that wraps around)
    if (obj->y > 0) {
        SDL_Rect src_rect2 = { 0, obj->height - obj->y, obj->width, obj->y };
        SDL_Rect dst_rect2 = { 0, 0, obj->width, obj->y};
        ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect2, &dst_rect2);
        if (ret < 0) {
            printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
            return ret;
        }
    }

    return ret;
}

static int destroy(object_t* obj) {
    SDL_DestroyTexture(obj->texture);
    return 0;
}

