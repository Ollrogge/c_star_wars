#include "text.h"
#include "scene.h"

static game_status_t init(object_t*, game_state_t*);
static game_status_t update(object_t*, game_state_t*);
static game_status_t render(object_t*, game_state_t*);
static game_status_t destroy(object_t*, game_state_t*);
static game_status_t collision(object_t*, object_t*, game_state_t*);

#define FONT_PATH "./fonts/DejaVuSansMono.ttf"

static const object_handler_t g_handler = {
    .init = init,
    .update = update,
    .render = render,
    .destroy = destroy,
    .collision = collision
};

int text_init(text_t* text, game_state_t* state, int x, int y, int point_size, char* val) {
    object_t* obj = &text->obj;
    list_init(&obj->list);

    // default for now (white)
    text->color = (SDL_Color){255, 255, 255, 255};
    text->text = val;
    text->updated = false;

    obj->handler = &g_handler;
    obj->x = x;
    obj->y = y;
    obj->type = OTHER;

    text->font_handle = TTF_OpenFont(FONT_PATH, point_size);
    if (text->font_handle == NULL) {
        printf("text_init: failed to load font, error: %s \n", TTF_GetError());
        return ERROR;
    }

    return text_update(text, state, val);
}

int text_update(text_t* text, game_state_t* state, char* val) {
    scene_t* scene = state->scene;
    SDL_Surface* text_surface = TTF_RenderText_Solid(text->font_handle, val, text->color);

    if (text->obj.texture != NULL) {
        SDL_DestroyTexture(text->obj.texture);
    }

    if (text_surface == NULL) {
        printf("text_update: rendertext_solid failed: %s \n", TTF_GetError());
        return ERROR;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(scene->renderer, text_surface);

    text->obj.width = text_surface->w;
    text->obj.height = text_surface->h;
    SDL_FreeSurface(text_surface);

    if (text_texture == NULL) {
        printf("text_update: unable to create texture: %s \n", SDL_GetError());
        return ERROR;
    }

    text->obj.texture = text_texture;

    return OK;
}

static game_status_t render(object_t* obj, game_state_t* state) {
    game_status_t ret = OK;
    scene_t* scene = state->scene;

    SDL_Rect dst_rect1 = {obj->x, obj->y, obj->width, obj->height};
    ret = SDL_RenderCopy(scene->renderer, obj->texture, NULL, &dst_rect1);
    if (ret < 0) {
        printf("text.c, SDL_RenderCopy Error: %s\n", SDL_GetError());
        return ERROR;
    }

    return ret;
}

static game_status_t destroy(object_t* obj, game_state_t* state) {
    text_t* text = (text_t*)obj;
    TTF_CloseFont(text->font_handle);
    SDL_DestroyTexture(obj->texture);

    return OK;
}

static game_status_t init(object_t* obj, game_state_t* state) {
    return OK;
}

static game_status_t update(object_t*, game_state_t* state) {
    return OK;
}

static game_status_t collision(object_t*, object_t*, game_state_t* state) {
    return OK;
}