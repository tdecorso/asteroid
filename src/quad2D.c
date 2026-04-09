#include "quad2D.h"
#include "projection.h"

quad2D_t quad2D_Template() {
    quad2D_t q = {0};
    q.v1.pos = (v4_t) {-0.5f, -0.5f, 0.0f, 1.0f}; 
    q.v1.color = (color_t) { 1.0f, 1.0f, 1.0f, 1.0f}; 
    q.v1.uv = (v2_t) { 0.0f, 0.0f}; 

    q.v2.pos = (v4_t) { 0.5f, -0.5f, 0.0f, 1.0f}; 
    q.v2.color = (color_t) { 1.0f, 1.0f, 1.0f, 1.0f}; 
    q.v2.uv = (v2_t) { 1.0f, 0.0f}; 

    q.v3.pos = (v4_t) { 0.5f, 0.5f, 0.0f, 1.0f}; 
    q.v3.color = (color_t) { 1.0f, 1.0f, 1.0f, 1.0f};
    q.v3.uv = (v2_t) { 1.0f, 1.0f};

    q.v4.pos = (v4_t) {-0.5f, 0.5f, 0.0f, 1.0f};
    q.v4.color = (color_t) { 1.0f, 1.0f, 1.0f, 1.0f};
    q.v4.uv = (v2_t) { 0.0f, 1.0f};
    
    return q;
}

void quad2D_Transform2D(quad2D_t* q, transform2D_t* t) {
    q->v1.pos = transform2D_apply(t, q->v1.pos);
    q->v2.pos = transform2D_apply(t, q->v2.pos);
    q->v3.pos = transform2D_apply(t, q->v3.pos);
    q->v4.pos = transform2D_apply(t, q->v4.pos);
}

void quad2D_OrthoProject(quad2D_t* q, float left, float right, float bottom, float top) {
    q->v1.pos = ortho_project(q->v1.pos, left, right, bottom, top);
    q->v2.pos = ortho_project(q->v2.pos, left, right, bottom, top);
    q->v3.pos = ortho_project(q->v3.pos, left, right, bottom, top);
    q->v4.pos = ortho_project(q->v4.pos, left, right, bottom, top);
}

void quad2D_View(quad2D_t* q, camera2D_t* cam) {
    q->v1.pos = camera2D_apply(cam, q->v1.pos);
    q->v2.pos = camera2D_apply(cam, q->v2.pos);
    q->v3.pos = camera2D_apply(cam, q->v3.pos);
    q->v4.pos = camera2D_apply(cam, q->v4.pos);
}
