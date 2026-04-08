#include "engine.h"
#include "rasterizer.h"
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600

const pixel_t BG_COLOR     = { 24,  22,  12, 255};
const pixel_t UI_COLOR     = {190, 200, 200, 255};
const pixel_t RED          = {  0,   0, 255, 255};

typedef enum {
    MENU,
    PLAY,
} scene_e;

vertex_t perspective_divide(vertex_t v) {
    float inv_w = 1.0f / v.pos.w;
    vertex_t r = {0};
    r.pos.x = v.pos.x * inv_w;
    r.pos.y = v.pos.y * inv_w;
    r.pos.z = v.pos.z * inv_w;
    r.color.r = v.color.r * inv_w;
    r.color.g = v.color.g * inv_w;
    r.color.b = v.color.b * inv_w;
    r.color.a = v.color.a * inv_w;
    r.inv_w = inv_w;
    return r;
}

vertex_t viewport_transform(float left, float width, float top, float height, vertex_t ndc) {
    vertex_t v = {0};
    v.pos.x = left + (ndc.pos.x + 1.0f) * width * 0.5f;
    v.pos.y =  top + (1.0f - ndc.pos.y) * height * 0.5f;
    v.pos.z = (ndc.pos.z + 1.0f) * 0.5f;
    v.color = ndc.color;
    v.inv_w = ndc.inv_w;
    return v;
}

int main(void) {
    engine_t engine = {0};

    if (!engine_Init(&engine)) {
        printf("[ERROR] Could not initialize engine.\n");
        return 1;
    }

    if (!engine_CreateWindow(&engine, SCREEN_W, SCREEN_H, "Pong")) {
        printf("[ERROR] Could not create window.\n");
        return 1;
    }

    engine_SetTargetFPS(&engine, 60);

    scene_e scene = MENU;

    char text[64];
    double fps_smooth = 0.0;

    vertex_t v1 = {.pos = {-0.5f, -0.5f, 0.0f, 1.0f}, .color = {1.0f, 0.0f, 0.0f, 1.0f}};
    vertex_t v2 = {.pos = { 0.5f, -0.5f, 0.0f, 1.0f}, .color = {0.0f, 1.0f, 0.0f, 1.0f}};
    vertex_t v3 = {.pos = { 0.0f,  0.5f, 0.0f, 1.0f}, .color = {0.0f, 0.0f, 1.0f, 1.0f}};

    vertex_t v1_tmp = perspective_divide(v1);
    vertex_t v2_tmp = perspective_divide(v2);
    vertex_t v3_tmp = perspective_divide(v3);

    v1_tmp = viewport_transform(0, (float)SCREEN_W, 0, (float)SCREEN_H, v1_tmp);
    v2_tmp = viewport_transform(0, (float)SCREEN_W, 0, (float)SCREEN_H, v2_tmp);
    v3_tmp = viewport_transform(0, (float)SCREEN_W, 0, (float)SCREEN_H, v3_tmp);
    
    while (!engine_WindowShouldClose(&engine)) {
        engine_BeginFrame(&engine);

        double fps = 1.0 / engine.dt;
        fps_smooth = fps_smooth * 0.9 + fps * 0.1;

        switch (scene) {
            case MENU: {
                if (engine_IsKeyPressed(&engine, SDL_SCANCODE_SPACE)) {
                    scene = PLAY;
                }
                break;
            }

            case PLAY: {
                break;
            }
        }

        engine_ClearBackground(&engine, BG_COLOR);

        switch (scene) {
            case MENU:
                rasterizer_FillText(&engine.fb, 240, 250,
                    "Press SPACE to start", 2, UI_COLOR);
                break;

            case PLAY:
                snprintf(text, sizeof(text), "FPS: %.2f", fps_smooth);
                rasterizer_FillText(&engine.fb, 5, 5, text, 1, UI_COLOR);
                rasterizer_FillTriangle(&engine.fb, v1_tmp, v2_tmp, v3_tmp);
                break;
        }

        engine_EndFrame(&engine);
    }

    engine_Quit(&engine);
    return 0;
}
