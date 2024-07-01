#include "util.h"

game_status_t load_and_scale_texture(object_t* obj, scene_t* scene, SDL_Surface* img, int width, int height) {
    // Calculate the scaling factor to fit the image within the window
    float scaleX = (float)width / img->w;
    float scaleY = (float)height / img->h;

    // Calculate the new dimensions for the scaled image
    int new_width = (int)(img->w * scaleX);
    int new_height = (int)(img->h * scaleY);

    //printf("Scaled dimensions: %d %d %f %f\n", new_width, new_height, scaleY, scaleX);

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
        return ERROR;
    }

    SDL_BlitScaled(img, NULL, scaled_surface, &dstRect);

    obj->texture = SDL_CreateTextureFromSurface(scene->renderer, scaled_surface);

    SDL_FreeSurface(scaled_surface);

    if (obj->texture == NULL) {
        printf("Failed to create tecture from surface for background: %s \n", SDL_GetError());

        return ERROR;
    }

    return OK;
}

bool rectangle_overlap(object_t* obj1, object_t* obj2) {
    return !(obj1->x + obj1->width <= obj2->x || obj1->x >= obj2->x + obj2->width ||
             obj1->y + obj1->height <= obj2->y || obj1->y >= obj2->y + obj2->height);

}