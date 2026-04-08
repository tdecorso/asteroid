#pragma once

#include <SDL3/SDL.h>
#include "framebuffer.h"

typedef struct {
    bool key_down[SDL_SCANCODE_COUNT];
    bool key_pressed[SDL_SCANCODE_COUNT];
    bool key_released[SDL_SCANCODE_COUNT];
} sdl_keyboard_t;

typedef struct {
    double freq_inv;
    double dt;
    double last;
} sdl_clock_t;

typedef struct {
    SDL_Window* window;
    SDL_Surface* framebuffer;
    SDL_Event event;
    bool should_close;
    sdl_keyboard_t keyboard;
    sdl_clock_t clock;
} sdl_platform_t;

bool sdl_platform_Init(sdl_platform_t* platform);
bool sdl_platform_CreateWindow(sdl_platform_t* platform, const char* title, int width, int height);
bool sdl_platform_WindowShouldClose(sdl_platform_t* platform);
void sdl_platform_BlitFramebuffer(sdl_platform_t* platform, framebuffer_t* fb);
void sdl_platform_Quit(sdl_platform_t* platform);
bool sdl_platform_KeyDown(sdl_platform_t* platform, int scancode);
bool sdl_platform_KeyPressed(sdl_platform_t* platform, int scancode);
bool sdl_platform_KeyReleased(sdl_platform_t* platform, int scancode);
double sdl_platform_BeginFrame(sdl_platform_t* platform);
double sdl_platform_DeltaTime(sdl_platform_t* platform);
double sdl_platform_GetTime(sdl_platform_t* platform);
