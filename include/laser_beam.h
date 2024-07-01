#ifndef _LASER_H
#define _LASER_H
#include "object.h"

typedef struct {
    object_t obj;
} laser_beam_t;

int laser_beam_init(laser_beam_t* , game_state_t*, int x, int y);

#endif // _LASER_H