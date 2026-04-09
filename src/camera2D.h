#pragma once

#include "v2.h"
#include "v4.h"

typedef struct {
    v2_t position;
    float zoom;
} camera2D_t;

v4_t camera2D_apply(camera2D_t* cam, v4_t p);

