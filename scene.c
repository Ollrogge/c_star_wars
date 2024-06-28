#include <time.h>
#include "scene.h"
#include "constants.h"
#include "util.h"
#include "enemy.h"

static const unsigned SPEED = 2;
static void spawn_enemies(scene_t* scene, int enemies_cnt, int width, int height);

// surface = collection of pixels used in software blitting
// bit blit = bit block transfer
//  data operation commonly used in computer graphics in which several bitmaps
//  are combined into one using a boolean function

int update_scene(scene_t* scene, game_state_t* state) {
    object_status_t ret;

    int height;
    SDL_GetWindowSize(scene->window, NULL, &height);

    // Clear the screen
    SDL_RenderClear(scene->renderer);

    list_head_t *tmp = NULL;
    list_head_t* pos = NULL;
    object_t* obj = NULL;

    list_for_each_safe(pos, tmp, &scene->objects) {
        obj = container_of(pos, object_t, list);
        ret = obj->handler->update(obj, scene, state);

        if (ret == DESTROY) {
            obj->handler->destroy(obj);
            list_del(&obj->list);
            free(obj);
            continue;
        }
        //obj->handler->render(obj, scene);
    }

    list_head_t *tmp2 = NULL;
    list_head_t* pos2 = NULL;
    object_t* obj2 = NULL;
    list_for_each_safe(pos, tmp, &scene->objects) {
        obj = container_of(pos, object_t, list);
        if (obj->type == OTHER) {
            continue;
        }
        list_for_each_safe(pos2, tmp2, &scene->objects) {
            obj2 = container_of(pos2, object_t, list);

            // assumption: at max 1 overlap at a time
            if (rectangle_overlap(obj, obj2)) {
                ret = obj->handler->collision(obj, obj2);
                if (ret == DESTROY) {
                    obj->handler->destroy(obj);
                    list_del(&obj->list);
                    if (obj->type == PLAYER) {
                        return GAME_OVER;
                    }
                    free(obj);
                    break;
                }
            }

        }
    }

    list_for_each(pos, &scene->objects) {
        obj = container_of(pos, object_t, list);
        obj->handler->render(obj, scene);
    }

    Uint32 now = SDL_GetTicks();
    int ticks_passed = MS_TO_TICKS(now - scene->last_enemy_spawn);
    int ticks_per_background = SCREEN_HEIGHT / scene->speed;
    if (ticks_passed > ticks_per_background) {
        spawn_enemies(scene, 20, SCREEN_WIDTH, SCREEN_HEIGHT);
        scene->last_enemy_spawn = now;
    }

    SDL_RenderPresent(scene->renderer);

    return 0;
}

object_status_t add_object_to_scene(scene_t *scene, object_t *obj) {
    list_add_tail(&scene->objects, &obj->list);

    return obj->handler->init(obj, scene);
}

int init_scene(scene_t* scene) {
    srand(time(NULL));
    list_init(&scene->objects);
    scene->speed = SPEED;

    scene->last_enemy_spawn = SDL_GetTicks();

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
    list_head_t* tmp;
    list_for_each_safe(pos, tmp, &scene->objects) {
        obj = container_of(pos, object_t, list);
        obj->handler->destroy(obj);

        free(obj);
    }
    SDL_DestroyWindow(scene->window);
    IMG_Quit();
    SDL_Quit();
}

static void spawn_enemies(scene_t* scene, int enemies_cnt, int width, int height) {
    enemy_t** enemies = calloc(1, sizeof(void*) * enemies_cnt);

    for (int i = 0; i < enemies_cnt; i++) {
        enemy_t* enemy = (enemy_t*)calloc(1, sizeof(enemy_t));
        object_t* obj = &enemy->obj;
        enemy_init(enemy, scene, 0, 0);
        int place_tries = 20;
        while (place_tries >= 0) {
            float x = ((float)rand() / RAND_MAX) * (width - obj->width);
            float y = ((float)rand() / RAND_MAX) * (height - obj->height);

            obj->x = (int)x;
            // spawn ouf of sight such that they start appearing at top
            obj->y = -(int)y;

            bool overlap = false;
            for (int j = 0; j < i; j++) {
                object_t* obj2 = &enemies[j]->obj;
                if (rectangle_overlap(obj, obj2)) {
                    overlap = true;
                    break;
                }
            }

            if (!overlap) {
                enemies[i] = enemy;
                place_tries = -1;
                add_object_to_scene(scene, obj);
            }
            place_tries--;
        }
    }

    free(enemies);
}