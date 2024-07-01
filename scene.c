#include <time.h>
#include "scene.h"
#include "constants.h"
#include "util.h"
#include "enemy.h"

static const unsigned SPEED = 2;
static void spawn_enemies(game_state_t* game_state, int enemies_cnt, int width, int height);
static game_status_t _handle_collision(object_t* obj1, object_t* obj2, game_state_t* state);
// surface = collection of pixels used in software blitting
// bit blit = bit block transfer
//  data operation commonly used in computer graphics in which several bitmaps
//  are combined into one using a boolean function

int update_scene(scene_t* scene, game_state_t* game_state) {
    game_status_t ret;

    int height;
    SDL_GetWindowSize(scene->window, NULL, &height);

    // Clear the screen
    SDL_RenderClear(scene->renderer);

    list_head_t *tmp = NULL;
    list_head_t* pos = NULL;
    object_t* obj = NULL;

    list_for_each_safe(pos, tmp, &scene->objects) {
        obj = container_of(pos, object_t, list);
        ret = obj->handler->update(obj, game_state);

        if (ret == DESTROY) {
            obj->handler->destroy(obj, game_state);
            list_del(&obj->list);
            free(obj);
            continue;
        }
        //obj->handler->render(obj, scene);
    }


    object_t** to_del = calloc(scene->objects_cnt, sizeof(object_t*));
    unsigned to_del_idx = 0;
    if (to_del == NULL) {
        puts("Calloc failed");
        return ERROR;
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

            if (obj2->type == OTHER || obj == obj2) {
                continue;
            }

            // assumption: max 1 overlap at a time
            if (rectangle_overlap(obj, obj2)) {
                ret = _handle_collision(obj, obj2, game_state);

                if (ret == GAME_OVER) {
                    return GAME_OVER;
                }

                if (ret == DESTROY) {
                    to_del[to_del_idx++] = obj;
                }

                break;
            }

        }
    }

    // need to delete the objects after the loop in order to handle the collision
    // of each object correctly. (need to call collision for both objects)
    for (unsigned i = 0; i < to_del_idx; i++) {
        del_object_from_scene(to_del[i], scene, game_state);
    }

    free(to_del);

    list_for_each(pos, &scene->objects) {
        obj = container_of(pos, object_t, list);
        obj->handler->render(obj, game_state);
    }

    Uint32 now = SDL_GetTicks();
    int ticks_passed = MS_TO_TICKS(now - scene->last_enemy_spawn);
    int ticks_per_background = SCREEN_HEIGHT / scene->speed;
    if (ticks_passed > ticks_per_background) {
        spawn_enemies(game_state, 20, SCREEN_WIDTH, SCREEN_HEIGHT);
        scene->last_enemy_spawn = now;
    }

    SDL_RenderPresent(scene->renderer);

    return 0;
}

static game_status_t _handle_collision(object_t* obj1, object_t* obj2, game_state_t* state) {
    game_status_t ret = OK;
    scene_t* scene = state->scene;

    ret = obj1->handler->collision(obj1, obj2, state);
    if (ret < 0) {
        return ret;
    }

    if (obj1->type == PLAYER && obj2->type == ENEMY) {
        return GAME_OVER;
    }

    if (obj1->type == BEAM && obj2->type == ENEMY) {
        scene->score.val++;
        sprintf(scene->score.buf, "Score: %d", scene->score.val);
        text_update(scene->score.text, state, scene->score.buf);
    }

    return ret;
}

game_status_t add_object_to_scene(object_t* obj, scene_t* scene, game_state_t* game_state) {
    list_add_tail(&scene->objects, &obj->list);

    scene->objects_cnt++;

    return obj->handler->init(obj, game_state);
}

game_status_t del_object_from_scene(object_t* obj, scene_t* scene, game_state_t* state) {
    list_del(&obj->list);

    scene->objects_cnt--;

    return obj->handler->destroy(obj, state);
}

game_status_t init_scene(scene_t* scene, game_state_t* game_state) {
    srand(time(NULL));
    list_init(&scene->objects);

    scene->speed = SPEED;
    scene->score.val = 0;
    scene->last_enemy_spawn = SDL_GetTicks();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return ERROR;
    }

    if (TTF_Init() < 0) {
        printf("Failed to initialize SDL_ttf, error: %s \n", TTF_GetError());
        return ERROR;
    }

    // Initialize PNG loading
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return ERROR;
    }

    // Create window
    scene->window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (scene->window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return ERROR;
    }

    scene->renderer = SDL_CreateRenderer(scene->window, -1, SDL_RENDERER_ACCELERATED);
    if (scene->renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return ERROR;
    }

    return OK;
}

// need to this happen after background has been initialized, otherwise
// the text will be behind the background
game_status_t init_text_for_scene(scene_t* scene, game_state_t* state) {
    text_t* score_text = calloc(1, sizeof(text_t));
    if (score_text == NULL) {
        printf("init_scene: calloc fail \n");
        return ERROR;
    }

    sprintf(scene->score.buf, "Score: %d", scene->score.val);
    int point_size = 15;

    if (text_init(score_text, state, SCREEN_WIDTH - 85, 20, point_size, scene->score.buf) < 0) {
        printf("text_init failed \n");
        return ERROR;
    }

    scene->score.text = score_text;

    if (add_object_to_scene(&score_text->obj, scene, state) < 0) {
        printf("init_text_for_scene: add_object failed \n");
        return ERROR;
    }

    return OK;
}

void destroy_scene(scene_t* scene, game_state_t* game_state) {
    object_t* obj;
    list_head_t* pos;
    list_head_t* tmp;
    list_for_each_safe(pos, tmp, &scene->objects) {
        obj = container_of(pos, object_t, list);
        obj->handler->destroy(obj, game_state);

        free(obj);
    }
    SDL_DestroyWindow(scene->window);
    IMG_Quit();
    SDL_Quit();
}

static void spawn_enemies(game_state_t* game_state, int enemies_cnt, int width, int height) {
    enemy_t** enemies = calloc(1, sizeof(void*) * enemies_cnt);

    for (int i = 0; i < enemies_cnt; i++) {
        enemy_t* enemy = (enemy_t*)calloc(1, sizeof(enemy_t));
        object_t* obj = &enemy->obj;
        enemy_init(enemy, game_state, 0, 0);
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
                add_object_to_scene(obj, game_state->scene, game_state);
            }
            place_tries--;
        }
    }

    free(enemies);
}