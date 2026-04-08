#pragma once

#include "framebuffer.h"
#include "texture.h"
#include "vertex.h"

void rasterizer_FillRect(framebuffer_t* fb, int x, int y, int w, int h, pixel_t fill_color);
void rasterizer_FillText(framebuffer_t* fb, int x, int y, char* text, int scale, pixel_t color);
void rasterizer_DrawLine(framebuffer_t* fb, int x0, int y0, int x1, int y1, pixel_t color);
void rasterizer_FillTriangle(framebuffer_t* fb, vertex_t v1, vertex_t v2, vertex_t v3, 
                                                texture_t* texture);
void rasterizer_FillQuad(framebuffer_t* fb, vertex_t v1, vertex_t v2, vertex_t v3, vertex_t v4,
                                            texture_t* texture);
