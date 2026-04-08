#pragma once

#include "color.h"
#include "v2.h"
#include "v4.h"

typedef struct {
    v4_t    pos;
    color_t color;
    v2_t    uv;
    float   inv_w;
} vertex_t;
