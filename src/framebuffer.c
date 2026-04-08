#include "framebuffer.h"

#include <stdlib.h>

bool framebuffer_Init(framebuffer_t* fb, int width, int height) {
    pixel_t* pixels = malloc(sizeof(pixel_t) * width * height);
    if (!pixels) return false;

    float* depth = malloc(sizeof(float) * width * height);
    if (!depth) return false;

    fb->pixels = pixels;
    fb->depth = depth;
    fb->width = width;
    fb->height = height;

    return true;
}

void framebuffer_Destroy(framebuffer_t* fb) {
    if (!fb) return;
    free(fb->pixels);
}

void framebuffer_Clear(framebuffer_t* fb, pixel_t color) {
    for (int i = 0; i < fb->width * fb->height; ++i) {
        fb->pixels[i] = color;
        fb->depth[i] = 1.0f;
    }
}

