#pragma once

#include "vertex.h"
#include "transform2D.h"

typedef struct {
    vertex_t v1;
    vertex_t v2;
    vertex_t v3;
    vertex_t v4;
} quad_t;

quad_t quad_Template();
void quad_Transform2D(quad_t* q, transform2D_t* t);
void quad_OrthoProject(quad_t* q, float left, float right, float bottom, float top);
