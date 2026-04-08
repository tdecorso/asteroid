#include "projection.h"

v4_t ortho_project(v4_t p, float left, float right, float bottom, float top) {
    v4_t r;

    r.x = 2.0f * (p.x - left) / (right - left) - 1.0f;
    r.y = 2.0f * (p.y - bottom) / (top - bottom) - 1.0f;

    r.z = p.z;
    r.w = 1.0f;

    return r;
}
