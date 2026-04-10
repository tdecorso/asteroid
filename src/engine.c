#include "engine.h"
#include "framebuffer.h"
#include "platform.h"
#include "rasterizer.h"
#include "quad2D.h"
#include "transform2D.h"
#include <stdio.h>

bool engine_Init(engine_t* e) {
    e->camera2D.position = (v2_t) {0};
    e->camera2D.zoom = 1.0f;
    return sdl_platform_Init(&e->platform);
}

bool engine_CreateWindow(engine_t* e, int w, int h, const char* title) {
    if (!sdl_platform_CreateWindow(&e->platform, title, w, h)) return false;
    if (!framebuffer_Init(&e->fb, w, h)) return false;
    return true;
}

bool engine_WindowShouldClose(engine_t* e) {
    return sdl_platform_WindowShouldClose(&e->platform);
}

void engine_Quit(engine_t* e) {
    framebuffer_Destroy(&e->fb);
    sdl_platform_Quit(&e->platform);
}

void engine_ClearBackground(engine_t* e, pixel_t color) {
    framebuffer_Clear(&e->fb, color);
}

void engine_BeginFrame(engine_t* e) {
    e->dt = sdl_platform_BeginFrame(&e->platform);
    e->frame_start = sdl_platform_GetTime(&e->platform);
}

void engine_EndFrame(engine_t* e) {
    sdl_platform_BlitFramebuffer(&e->platform, &e->fb);
    double frame_time = sdl_platform_GetTime(&e->platform) - e->frame_start;
    double remaining = e->target_dt - frame_time;
    if (remaining > 0.0) {
        if (remaining > 0.002) {
            SDL_Delay((uint32_t)((remaining - 0.001) * 1000.0));
        }
    }

    while (sdl_platform_GetTime(&e->platform) - e->frame_start < e->target_dt) {
        // spin for precision
    }
}

bool engine_IsKeyPressed(engine_t* e, int scancode) {
    return sdl_platform_KeyPressed(&e->platform, scancode);
}

bool engine_IsKeyReleased(engine_t* e, int scancode) {
    return sdl_platform_KeyReleased(&e->platform, scancode);
}

bool engine_IsKeyDown(engine_t* e, int scancode) {
    return sdl_platform_KeyDown(&e->platform, scancode);
}

void engine_SetTargetFPS(engine_t* e, int fps) {
    e->target_dt = 1 / (double) fps;
}

void engine_DrawText(engine_t* e, int x, int y, char* text, int scale, pixel_t color) {
    rasterizer_FillText(&e->fb, x, y, text, scale, color);
}

static vertex_t perspective_divide(vertex_t v) {
    float inv_w = 1.0f / v.pos.w;
    vertex_t r = {0};

    r.pos.x = v.pos.x * inv_w;
    r.pos.y = v.pos.y * inv_w;
    r.pos.z = v.pos.z * inv_w;
    r.pos.w = v.pos.w;

    r.color.r = v.color.r * inv_w;
    r.color.g = v.color.g * inv_w;
    r.color.b = v.color.b * inv_w;
    r.color.a = v.color.a * inv_w;

    r.uv.x = v.uv.x * inv_w;
    r.uv.y = v.uv.y * inv_w;

    r.inv_w = inv_w;

    return r;
}

static vertex_t viewport_transform(float left, float width, float top, float height, vertex_t ndc) {
    vertex_t v = {0};
    v.pos.x = left + (ndc.pos.x + 1.0f) * width * 0.5f;
    v.pos.y =  top + (1.0f - ndc.pos.y) * height * 0.5f;
    v.pos.z = (ndc.pos.z + 1.0f) * 0.5f;
    v.pos.w = ndc.pos.w;
    v.color = ndc.color;
    v.uv    = ndc.uv;
    v.inv_w = ndc.inv_w;
    return v;
}

static void dump_vertex(const char* label, vertex_t v) {
    printf("%s: pos=(%f, %f, %f, %f) inv_w=%f uv=(%f, %f)\n",
        label,
        v.pos.x, v.pos.y, v.pos.z, v.pos.w,
        v.inv_w,
        v.uv.x, v.uv.y
    );
}

void engine_DrawSprite(engine_t* e, sprite_t* sprite) {
    quad2D_t quad = quad2D_Template();

    transform2D_t transform = {
        sprite->pos,
        sprite->scale,
        sprite->rot
    };

    quad2D_Transform2D(&quad, &transform);

    quad2D_View(&quad, &e->camera2D);

    float left   = -e->fb.width  * 0.5f;
    float right  =  e->fb.width  * 0.5f;
    float bottom = -e->fb.height * 0.5f;
    float top    =  e->fb.height * 0.5f;

    quad2D_OrthoProject(&quad, left, right, bottom, top);

    quad.v1 = perspective_divide(quad.v1);
    quad.v2 = perspective_divide(quad.v2);
    quad.v3 = perspective_divide(quad.v3);
    quad.v4 = perspective_divide(quad.v4);

    quad.v1 = viewport_transform(0, (float)e->fb.width, 0, (float)e->fb.height, quad.v1);
    quad.v2 = viewport_transform(0, (float)e->fb.width, 0, (float)e->fb.height, quad.v2);
    quad.v3 = viewport_transform(0, (float)e->fb.width, 0, (float)e->fb.height, quad.v3);
    quad.v4 = viewport_transform(0, (float)e->fb.width, 0, (float)e->fb.height, quad.v4);

    rasterizer_FillQuad2D(
        &e->fb,
        quad.v1,
        quad.v2,
        quad.v3,
        quad.v4,
        sprite->texture
    );
}

float engine_RandF(engine_t* e, float low, float high) {
    return sdl_platform_RandF(&e->platform) * (high - low) + low;
}
