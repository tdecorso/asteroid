#pragma once
#include <stdbool.h>

typedef struct {
    int width;
    int height;
    unsigned char* data;
} texture_t;

bool texture_Create(texture_t* t, const char* path);
