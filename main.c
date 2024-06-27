#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include "scene.h"
#include "background.h"
#include "player.h"

static volatile bool g_running = true;

void int_handler() {
    g_running = false;
}

int main(void) {
    int ret = 0;

    signal(SIGINT, int_handler);

    scene_t scene = {0};

    ret = init_scene(&scene);
    if (ret < 0) {
        return 0;
    }

    background_t background = {0};
    ret = background_init(&background, &scene);
    if (ret < 0) {
        return 0;
    }

    add_object_to_scene(&scene, &background.obj);

    player_t player = {0};
    ret = player_init(&player, &scene);
    if (ret < 0) {
        return 0;
    }

    add_object_to_scene(&scene, &player.obj);

    puts("Objects initialized, entering game loop");

    SDL_Event e;

    while (g_running) {
        movement_vec_t vec = {0};
        while (SDL_PollEvent(&e) != 0) {
             if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        vec.y -= player.obj.speed;
                        break;
                    case SDLK_s:
                        vec.y += player.obj.speed;
                        break;
                    case SDLK_a:
                        vec.x -= player.obj.speed;
                        break;
                    case SDLK_d:
                        vec.x += player.obj.speed;
                        break;
                }
            }
        }
        update_scene(&scene, &vec);
        SDL_Delay(10);
    }

    destroy_scene(&scene);
}
