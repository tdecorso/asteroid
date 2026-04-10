#pragma once

#include "v2.h"
#include "v3.h"
#include "texture.h"

typedef struct {
    texture_t* texture;
    v3_t pos;
    v2_t scale;
    float rot;
} sprite_t;
