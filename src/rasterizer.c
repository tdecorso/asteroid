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

static inline float edge(int px, int py, int ax, int ay, int bx, int by) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

void rasterizer_FillTriangle(framebuffer_t* fb, vertex_t v1, vertex_t v2, vertex_t v3,
                             texture_t* texture) {
    if (!fb) return;

    // --- Bounding box ---
    int xmin = min3(v1.pos.x, v2.pos.x, v3.pos.x);
    int ymin = min3(v1.pos.y, v2.pos.y, v3.pos.y);
    int xmax = max3(v1.pos.x, v2.pos.x, v3.pos.x);
    int ymax = max3(v1.pos.y, v2.pos.y, v3.pos.y);

    xmin = max2(0, xmin);
    ymin = max2(0, ymin);
    xmax = min2(xmax, fb->width);
    ymax = min2(ymax, fb->height);

    float area = edge(v1.pos.x, v1.pos.y,
                      v2.pos.x, v2.pos.y,
                      v3.pos.x, v3.pos.y);

    if (area == 0.0f) return;

    if (area < 0) {
        vertex_t tmp = v2;
        v2 = v3;
        v3 = tmp;
        area = -area;
    }

    float inv_area = 1.0f / area;

    for (int y = ymin; y < ymax; ++y) {
        for (int x = xmin; x < xmax; ++x) {

            float w1 = edge(x, y,
                            v2.pos.x, v2.pos.y,
                            v3.pos.x, v3.pos.y);

            float w2 = edge(x, y,
                            v3.pos.x, v3.pos.y,
                            v1.pos.x, v1.pos.y);

            float w3 = edge(x, y,
                            v1.pos.x, v1.pos.y,
                            v2.pos.x, v2.pos.y);

            if (w1 >= 0 && w2 >= 0 && w3 >= 0) {

                w1 *= inv_area;
                w2 *= inv_area;
                w3 *= inv_area;

                int index = x + y * fb->width;

                float z = w1 * v1.pos.z +
                          w2 * v2.pos.z +
                          w3 * v3.pos.z;

                if (z < fb->depth[index]) {
                    fb->depth[index] = z;

                    float inv_w =
                        w1 * v1.inv_w +
                        w2 * v2.inv_w +
                        w3 * v3.inv_w;

                    if (inv_w == 0.0f) continue;

                    float recip = 1.0f / inv_w;

                    float r = (w1 * v1.color.r +
                               w2 * v2.color.r +
                               w3 * v3.color.r) * recip;

                    float g = (w1 * v1.color.g +
                               w2 * v2.color.g +
                               w3 * v3.color.g) * recip;

                    float b = (w1 * v1.color.b +
                               w2 * v2.color.b +
                               w3 * v3.color.b) * recip;

                    float a = (w1 * v1.color.a +
                               w2 * v2.color.a +
                               w3 * v3.color.a) * recip;

                    if (!texture) {
                        fb->pixels[index].r = (uint8_t)(r * 255.0f);
                        fb->pixels[index].g = (uint8_t)(g * 255.0f);
                        fb->pixels[index].b = (uint8_t)(b * 255.0f);
                        fb->pixels[index].a = (uint8_t)(a * 255.0f);
                        continue;
                    }

                    float u = (w1 * v1.uv.x +
                               w2 * v2.uv.x +
                               w3 * v3.uv.x) * recip;

                    float v = (w1 * v1.uv.y +
                               w2 * v2.uv.y +
                               w3 * v3.uv.y) * recip;

                    int tx = (int)(u * (texture->width - 1));
                    int ty = (int)(v * (texture->height - 1));

                    unsigned char* tpixel =
                        &texture->data[(tx + ty * texture->width) * 4];

                    float src_r = tpixel[0] / 255.0f;
                    float src_g = tpixel[1] / 255.0f;
                    float src_b = tpixel[2] / 255.0f;
                    float src_a = tpixel[3] / 255.0f;

                    pixel_t dst = fb->pixels[index];

                    float dst_r = dst.r / 255.0f;
                    float dst_g = dst.g / 255.0f;
                    float dst_b = dst.b / 255.0f;
                    float dst_a = dst.a / 255.0f;

                    src_r *= r;
                    src_g *= g;
                    src_b *= b;

                    float out_a = src_a + dst_a * (1.0f - src_a);
                    float out_r = (src_r * src_a + dst_r * dst_a * (1.0f - src_a));
                    float out_g = (src_g * src_a + dst_g * dst_a * (1.0f - src_a));
                    float out_b = (src_b * src_a + dst_b * dst_a * (1.0f - src_a));

                    fb->pixels[index].r = (uint8_t)(out_r * 255.0f);
                    fb->pixels[index].g = (uint8_t)(out_g * 255.0f);
                    fb->pixels[index].b = (uint8_t)(out_b * 255.0f);
                    fb->pixels[index].a = (uint8_t)(out_a * 255.0f);
                }
            }
        }
    }
}

void rasterizer_FillQuad(framebuffer_t* fb, vertex_t v1, vertex_t v2, vertex_t v3, vertex_t v4,
                         texture_t* texture) {
    rasterizer_FillTriangle(fb, v1, v2, v3, texture);
    rasterizer_FillTriangle(fb, v1, v3, v4, texture);
}
