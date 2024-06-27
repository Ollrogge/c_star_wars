#include "player.h"

static int init(object_t*, scene_t*);
static int update(object_t*, scene_t*, void* arg);
static int render(object_t*, scene_t*);
static int destroy(object_t*);

#define IMG_PATH "./images/space-ship.png"

#define WIDTH 50
#define HEIGHT 50
#define SPEED 5

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy
};

int player_init(player_t* player, scene_t* scene) {
    int height, width;
    SDL_GetWindowSize(scene->window, &width, &height);
    player->obj.handler = &g_handler;
    player->obj.speed = SPEED;
    player->obj.x = width / 2 - WIDTH / 2;
    player->obj.y = height - HEIGHT;

    return 0;
}

static int init(object_t* obj, scene_t* scene) {
    player_t* player = (player_t*) obj;

    SDL_Surface* img = IMG_Load(IMG_PATH);
    if (img == NULL) {
        printf("Player.c: unable to load image!: %s \n", IMG_GetError());
        return -1;
    }

    // Calculate the scaling factor to fit the image within the window
    float scaleX = (float)WIDTH / img->w;
    float scaleY = (float)HEIGHT / img->h;

    // Calculate the new dimensions for the scaled image
    int new_width = (int)(img->w * scaleX);
    int new_height = (int)(img->h * scaleY);

    printf("Scaled dimensions: %d %d %f %f\n", new_width, new_height, scaleY, scaleX);

    SDL_Rect dstRect;
    dstRect.x = (WIDTH - new_width) / 2;  // Center the image horizontally
    dstRect.y = (HEIGHT - new_height) / 2;  // Center the image vertically
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

    player->obj.texture = SDL_CreateTextureFromSurface(scene->renderer, scaled_surface);

    SDL_FreeSurface(img);
    SDL_FreeSurface(scaled_surface);

    if (player->obj.texture == NULL) {
        printf("Failed to create tecture from surface for background: %s \n", SDL_GetError());

        return -1;
    }

    return 0;
}

static int render(object_t* obj, scene_t* scene) {
    int ret = 0;

    SDL_Rect src_rect1 = { 0, 0, obj->width, obj->height};
    SDL_Rect dst_rect1 = {obj->x, obj->y, obj->width, obj->height};
    ret = SDL_RenderCopy(scene->renderer, obj->texture, &src_rect1, &dst_rect1);
    if (ret < 0) {
        printf("SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ret;
    }

    return ret;
}

static int update(object_t* obj, scene_t* scene, void* arg) {
    movement_vec_t* movement = (movement_vec_t*)arg;
    player_t* player = (player_t*)obj;
    int height, width;
    SDL_GetWindowSize(scene->window, &width, &height);
    if (movement->y != 0) {
        player->moving_cnt = 10;
    }
    else {
        player->moving_cnt -= 1;
    }

    if (player->moving_cnt >= 0) {
        // turn of scene movement when we are moving else player movement is jerky
        // and if tickrate high player wont even move
        obj->y += movement->y;
    }
    else {
        obj->y += scene->speed;
    }
    obj->x = obj->x + movement->x;

    //bool is_moving_up = movement->y < 0 ? true : false;
    //bool move_right = movement->x > 0 ? true : false;

    if (obj->y + obj->height >= height) {
        obj->y = height - obj->height;
    }
    else if (obj->y <= 0) {
        obj->y = 0;
    }

    if (obj->x + obj->width >= width) {
        obj->x = width - obj->width;
    }
    else if (obj->x <= 0) {
        obj->x = 0;
    }

    return 0;
}

static int destroy(object_t* obj) {
    SDL_DestroyTexture(obj->texture);
    return 0;
}