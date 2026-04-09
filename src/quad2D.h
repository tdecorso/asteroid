#pragma once

#include "vertex.h"
#include "transform2D.h"
#include "camera2D.h"

typedef struct {
    vertex_t v1;
    vertex_t v2;
    vertex_t v3;
    vertex_t v4;
} quad2D_t;

quad2D_t quad2D_Template();
void quad2D_Transform2D(quad2D_t* q, transform2D_t* t);
void quad2D_OrthoProject(quad2D_t* q, float left, float right, float bottom, float top);
void quad2D_View(quad2D_t* q, camera2D_t* cam);
