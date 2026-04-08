#pragma once

#include "v2.h"
#include "texture.h"

typedef struct {
    texture_t* texture;
    v2_t position;
    v2_t scale;
    float rotation;
} sprite_t;
