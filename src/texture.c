#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool texture_Create(texture_t* t, const char* path) {
    int w, h, channels;
    unsigned char* data = stbi_load(path, &w, &h, &channels, 4);
    if (!data) {
        return false;
    }

    t->width = w;
    t->height = h;
    t->data = data;
    return true;
}
