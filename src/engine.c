#include "engine.h"
#include "framebuffer.h"
#include "platform.h"

bool engine_Init(engine_t* e) {
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
