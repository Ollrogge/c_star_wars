#include "scene.h"
#include "constants.h"
#include "util.h"

static const unsigned SPEED = 2;

// surface = collection of pixels used in software blitting
// bit blit = bit block transfer
//  data operation commonly used in computer graphics in which several bitmaps
//  are combined into one using a boolean function

int update_scene(scene_t* scene, movement_vec_t* vec) {
    int ret;
    object_t* obj;
    list_head_t* pos;

    int height;
    SDL_GetWindowSize(scene->window, NULL, &height);

    list_for_each(pos, &scene->objects) {
        obj = container_of(pos, object_t, list);
        obj->handler->update(obj, scene, vec);
    }

    // Clear the screen
    SDL_RenderClear(scene->renderer);

    list_for_each(pos, &scene->objects) {
        obj = container_of(pos, object_t, list);
        ret = obj->handler->render(obj, scene);
        if (ret < 0) {
            puts("Object render failed");
        }
    }

    SDL_RenderPresent(scene->renderer);

    return 0;
}

void add_object_to_scene(scene_t *scene, object_t *obj) {
    list_add_tail(&scene->objects, &obj->list);

    obj->handler->init(obj, scene);
}

int init_scene(scene_t* scene) {
    list_init(&scene->objects);
    scene->speed = SPEED;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create window
    scene->window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (scene->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    scene->renderer = SDL_CreateRenderer(scene->window, -1, SDL_RENDERER_ACCELERATED);
    if (scene->renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return -1;
    }

    return 0;
}

void destroy_scene(scene_t* scene) {
    object_t* obj;
    list_head_t* pos;
    list_for_each(pos, &scene->objects) {
        obj = container_of(pos, object_t, list);
        obj->handler->destroy(obj);
    }
    SDL_DestroyWindow(scene->window);
    IMG_Quit();
    SDL_Quit();
}