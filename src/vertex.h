#pragma once

#include "color.h"

typedef struct {
    float x, y, z, w;
} v4_t;

typedef struct {
    v4_t pos;
    color_t color;
    float inv_w;
} vertex_t;
