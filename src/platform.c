#include "platform.h"

#include <stdio.h>

static void sdl_clock_init(sdl_clock_t* clock) {
    clock->freq_inv = 1.0f / (double) SDL_GetPerformanceFrequency(); 
}

bool sdl_platform_Init(sdl_platform_t* platform) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;
    SDL_srand(SDL_GetTicks());
    sdl_clock_init(&platform->clock);
    return true;
}

bool sdl_platform_CreateWindow(sdl_platform_t* platform, const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(title, width, height, SDL_WINDOW_HIDDEN);
    if (!window) return false;

    SDL_Surface* framebuffer = SDL_GetWindowSurface(window);
    if (!framebuffer) return false;

    platform->window = window;
    platform->framebuffer = framebuffer;

    SDL_ShowWindow(window);

    return true;
}

bool sdl_platform_WindowShouldClose(sdl_platform_t* platform) {
    return platform->should_close;
}

static void keyboard_clear(sdl_keyboard_t* keyboard) {
    memset(keyboard->key_pressed, 0, sizeof(keyboard->key_pressed));
    memset(keyboard->key_released, 0, sizeof(keyboard->key_released));
}

static void keyboard_update(sdl_keyboard_t* keyboard, SDL_KeyboardEvent*e ) {
    SDL_Scancode sc = e->scancode;
    if (e->type == SDL_EVENT_KEY_DOWN) {
        if (!keyboard->key_down[sc]) {
            keyboard->key_pressed[sc] = true;
        }
        keyboard->key_down[sc] = true;
    }
    else if (e->type == SDL_EVENT_KEY_UP) {
        keyboard->key_down[sc] = false;
        keyboard->key_released[sc] = true;
    }
}

static void sdl_platform_poll_events(sdl_platform_t* platform) {
    keyboard_clear(&platform->keyboard);
    while (SDL_PollEvent(&platform->event)) {
        switch (platform->event.type) {
        case SDL_EVENT_QUIT:
            platform->should_close = true;
            return;
        case SDL_EVENT_KEY_UP:
            keyboard_update(&platform->keyboard, &platform->event.key);
            break;
        case SDL_EVENT_KEY_DOWN:
            keyboard_update(&platform->keyboard, &platform->event.key);
            break;
        default:
            break;
        }
    }
}

void sdl_platform_BlitFramebuffer(sdl_platform_t* platform, framebuffer_t* fb) {
    uint8_t* dest_pixels = platform->framebuffer->pixels;
    size_t buffer_size = platform->framebuffer->pitch * platform->framebuffer->h;
    memcpy(dest_pixels, fb->pixels, buffer_size);
    SDL_UpdateWindowSurface(platform->window);
}

void sdl_platform_Quit(sdl_platform_t* platform) {
    SDL_DestroySurface(platform->framebuffer);
    SDL_DestroyWindow(platform->window);
    SDL_Quit();
}

bool sdl_platform_KeyDown(sdl_platform_t* platform, int scancode) {
    return platform->keyboard.key_down[scancode];
}

bool sdl_platform_KeyPressed(sdl_platform_t* platform, int scancode) {
    return platform->keyboard.key_pressed[scancode];
}

bool sdl_platform_KeyReleased(sdl_platform_t* platform, int scancode) {
    return platform->keyboard.key_released[scancode];
}

static void sdl_clock_tick(sdl_clock_t* clock) {
    double current = SDL_GetPerformanceCounter();
    clock->dt = (current - clock->last) * clock->freq_inv;
    clock->last = current;
}

double sdl_platform_BeginFrame(sdl_platform_t* platform) {
    sdl_platform_poll_events(platform);
    sdl_clock_tick(&platform->clock);
    return platform->clock.dt;
}

double sdl_platform_DeltaTime(sdl_platform_t* platform) {
    return platform->clock.dt;
}

double sdl_platform_GetTime(sdl_platform_t* platform) {
    return SDL_GetPerformanceCounter() * platform->clock.freq_inv;
}

float sdl_platform_RandF(sdl_platform_t* platform) {
    return SDL_randf();
}
