#include "camera2D.h"

v4_t camera2D_apply(camera2D_t* cam, v4_t p) {
    p.x = (p.x - cam->position.x) * cam->zoom;
    p.y = (p.y - cam->position.y) * cam->zoom;
    return p;
}
