#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
} pixel_t;

typedef struct {
    pixel_t* pixels;
    int width;
    int height;
    float* depth;
} framebuffer_t;

bool framebuffer_Init(framebuffer_t* fb, int width, int height);
void framebuffer_Destroy(framebuffer_t* fb);
void framebuffer_Clear(framebuffer_t* fb, pixel_t color);
