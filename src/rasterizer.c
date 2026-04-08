#include "rasterizer.h"
#include "font8x8/font8x8.h"
#include <stdio.h>

#define abs(x) ((x) > 0 ? (x) : -(x))
#define max2(a, b) ((a) > (b) ? (a) : (b))
#define min2(a, b) ((a) < (b) ? (a) : (b))
#define max3(a, b, c) (max2((a), max2((b), (c))))
#define min3(a, b, c) (min2((a), min2((b), (c))))

static void put_pixel(framebuffer_t* fb, int x, int y, pixel_t p) {
    if (x < 0 || y < 0) return;
    if (x >= fb->width || y >= fb->height) return;
    fb->pixels[x + y * fb->width] = p;
}

void rasterizer_FillRect(framebuffer_t* fb, int x0, int y0, int w, int h, pixel_t fill_color) {
    if (!fb) return;

    for (int y = y0; y < y0 + h; ++y) {
        for (int x = x0; x < x0 + w; ++x) {
            put_pixel(fb, x, y, fill_color);
        }
    }
}

static void rasterizer_FillChar(framebuffer_t* fb, int x0, int y0, char c, int scale, pixel_t color) {
    if (!fb) return;
    const char* glyph = font8x8_basic[(int)c];
    int set = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            set = glyph[y] & (1 << x); 
            if (!set) continue; 
            for (int dy = 0; dy < scale; ++dy) {
                for (int dx = 0; dx < scale; ++dx) {
                    put_pixel(fb, x0 + x * scale + dx, y0 + y * scale + dy, color);
                }
            }
        }
    }
}

void rasterizer_FillText(framebuffer_t* fb, int x, int y, char* text, int scale, pixel_t color) {
    if (!fb) return;
    while (*text) {
        rasterizer_FillChar(fb, x, y, *text, scale, color);
        x += 8 * scale;
        text++;
    }
}


void rasterizer_DrawLine(framebuffer_t* fb, int x0, int y0, int x1, int y1, pixel_t color)
{
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        put_pixel(fb, x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

float triangle_signed_area(int x0, int y0, int x1, int y1, int x2, int y2) {
    return x0*y1 + x1*y2 + x2*y0 - x0*y2 - x1*y0 - x2*y1;
}

static inline float edge(int px, int py, int ax, int ay, int bx, int by) {
    return (px-ax)*(by-ay) - (py-ay)*(bx-ax);
}

void rasterizer_FillTriangle(framebuffer_t* fb, vertex_t v1, vertex_t v2, vertex_t v3) {
    if (!fb) return;

    int xmin = min3(v1.pos.x, v2.pos.x, v3.pos.x);
    int ymin = min3(v1.pos.y, v2.pos.y, v3.pos.y);

    int xmax = max3(v1.pos.x, v2.pos.x, v3.pos.x);
    int ymax = max3(v1.pos.y, v2.pos.y, v3.pos.y);

    xmin = max2(0, xmin);
    ymin = max2(0, ymin);

    xmax = min2(xmax, fb->width);
    ymax = min2(ymax, fb->height);

    int e1_dx = v3.pos.y - v2.pos.y;
    int e1_dy = v2.pos.x - v3.pos.x;

    int e2_dx = v1.pos.y - v3.pos.y;
    int e2_dy = v3.pos.x - v1.pos.x;

    int e3_dx = v2.pos.y - v1.pos.y;
    int e3_dy = v1.pos.x - v2.pos.x;

    float area = edge(v1.pos.x, v1.pos.y, v2.pos.x, v2.pos.y, v3.pos.x, v3.pos.y);
    if (area <= 1e-6f) return;
    float inv_area = 1.0f / area;

    float w1_row = edge(xmin, ymin, v2.pos.x, v2.pos.y, v3.pos.x, v3.pos.y);
    float w2_row = edge(xmin, ymin, v3.pos.x, v3.pos.y, v1.pos.x, v1.pos.y);
    float w3_row = edge(xmin, ymin, v1.pos.x, v1.pos.y, v2.pos.x, v2.pos.y);

    for (int y = ymin; y < ymax; ++y) {
        float w1 = w1_row; 
        float w2 = w2_row; 
        float w3 = w3_row; 

        for (int x = xmin; x < xmax; ++x) {
            if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                float z = 
                    (w1 * v1.pos.z +
                     w2 * v2.pos.z +
                     w3 * v3.pos.z) * inv_area;

                int index = x + y * fb->width;
                
                if (z < fb->depth[index]) {
                    fb->depth[index] = z;

                    float inv_w =
                        (w1 * v1.inv_w +
                         w2 * v2.inv_w +
                         w3 * v3.inv_w) * inv_area;

                    float r =
                        (w1 * v1.color.r +
                         w2 * v2.color.r +
                         w3 * v3.color.r) * inv_area;

                    float g =
                        (w1 * v1.color.g +
                         w2 * v2.color.g +
                         w3 * v3.color.g) * inv_area;

                    float b =
                        (w1 * v1.color.b +
                         w2 * v2.color.b +
                         w3 * v3.color.b) * inv_area;

                    r /= inv_w;
                    g /= inv_w;
                    b /= inv_w;

                    pixel_t color = {
                        (uint8_t) (r * 255.0f),
                        (uint8_t) (g * 255.0f),
                        (uint8_t) (b * 255.0f),
                        255};

                    fb->pixels[index] = color;
                }
            }

            w1 += e1_dx;
            w2 += e2_dx;
            w3 += e3_dx;
        }

        w1_row += e1_dy;
        w2_row += e2_dy;
        w3_row += e3_dy;
    }
}
