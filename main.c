#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include "scene.h"
#include "background.h"

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

    puts("Objects initialized, entering game loop");

    while (g_running) {
        update_scene(&scene);
        SDL_Delay(10);
    }

    destroy_scene(&scene);
}
