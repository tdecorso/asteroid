#include "transform2D.h"
#include <math.h>

v4_t transform2D_apply(transform2D_t* t, v4_t p) {
    float x = p.x * t->scale.x;
    float y = p.y * t->scale.y;

    float c = cosf(t->rotation);
    float s = sinf(t->rotation);

    float rx = x * c - y * s;
    float ry = x * s + y * c;

    rx += t->position.x;
    ry += t->position.y;

    v4_t r = p;
    r.x = rx;
    r.y = ry;
    r.z = t->position.z;

    return r;
}
