#pragma once

#include "v2.h"
#include "v4.h"

typedef struct {
    v2_t position;
    v2_t scale;
    float rotation;
} transform2D_t;

v4_t transform2D_apply(transform2D_t* t, v4_t p);
