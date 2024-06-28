#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include "scene.h"
#include "background.h"
#include "player.h"
#include "constants.h"

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


    background_t* background = (background_t*)calloc(1, sizeof(background_t));
    ret = background_init(background, &scene);
    if (ret < 0) {
        return 0;
    }

    add_object_to_scene(&scene, &background->obj);

    player_t* player = (player_t*)calloc(1, sizeof(player_t));
    ret = player_init(player, &scene);
    if (ret < 0) {
        return 0;
    }

    add_object_to_scene(&scene, &player->obj);

    puts("Objects initialized, entering game loop");

    SDL_Event e;

    while (g_running) {
        game_state_t state = {0};
        while (SDL_PollEvent(&e) != 0) {
             if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        state.movement.y -= player->obj.speed;
                        break;
                    case SDLK_s:
                        state.movement.y += player->obj.speed;
                        break;
                    case SDLK_a:
                        state.movement.x -= player->obj.speed;
                        break;
                    case SDLK_d:
                        state.movement.x += player->obj.speed;
                        break;
                }
            }
            // todo: also need to capture button up to stop shooting
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                SDL_MouseButtonEvent *e_b = &e.button;
                if (e_b->button == SDL_BUTTON_LEFT) {
                    state.mouse_used = true;
                }
            }
        }
        ret = update_scene(&scene, &state);
        if (ret == GAME_OVER) {
            break;
        }
        SDL_Delay(MS_TO_TICKS(TICK_RATE));
    }

    puts("Game over");
    SDL_Delay(3000);

    destroy_scene(&scene);
}
