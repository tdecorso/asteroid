#include "engine.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600

const pixel_t BG_COLOR     = {170, 170, 170, 255};
const pixel_t UI_COLOR     = { 60,  60,  60, 255};
const pixel_t RED          = {  0,   0, 255, 255};

typedef enum {
    MENU,
    PLAY,
} scene_e;

const char* tex_path = "/home/tony/dev/games/asteroid/build/assets/Fighter/Idle.png";

int main(void) {
    engine_t engine = {0};

    if (!engine_Init(&engine)) {
        printf("[ERROR] Could not initialize engine.\n");
        return 1;
    }

    if (!engine_CreateWindow(&engine, SCREEN_W, SCREEN_H, "Pong")) {
        printf("[ERROR] Could not create window.\n");
        return 1;
    }

    engine_SetTargetFPS(&engine, 60);

    scene_e scene = MENU;

    char fps_text[64];
    double fps_smooth = 0.0;

    texture_t idle_tex = {0};
    if (!texture_Create(&idle_tex, tex_path)) {
        printf("[ERROR] Could not load texture at %s\n", tex_path);
        return 1;
    }

    sprite_t player = {.texture = &idle_tex, 
                       .position = {0.0f, 0.0f}, 
                       .scale = {200.0f, 200.0f}, 
                       .rotation = 0};

    float vx = 0.0f;
    float vy = 0.0f;

    while (!engine_WindowShouldClose(&engine)) {
        engine_BeginFrame(&engine);

        double fps = 1.0 / engine.dt;
        fps_smooth = fps_smooth * 0.9 + fps * 0.1;

        switch (scene) {
            case MENU: {
                if (engine_IsKeyPressed(&engine, SDL_SCANCODE_SPACE)) {
                    scene = PLAY;
                }
                break;
            }

            case PLAY: {
                if (engine_IsKeyDown(&engine, SDL_SCANCODE_W)) {
                    vx += cosf(player.rotation) * 1000.0f * engine.dt;
                    vy += sinf(player.rotation) * 1000.0f * engine.dt;
                }
                if (engine_IsKeyDown(&engine, SDL_SCANCODE_S)) {
                    vx -= cosf(player.rotation) * 1000.0f * engine.dt;
                    vy -= sinf(player.rotation) * 1000.0f * engine.dt;
                }
                if (engine_IsKeyDown(&engine, SDL_SCANCODE_D)) {
                    player.rotation -= 6.0f * engine.dt;
                }
                if (engine_IsKeyDown(&engine, SDL_SCANCODE_A)) {
                    player.rotation += 6.0f * engine.dt;
                }

                player.position.x += vx * engine.dt;
                player.position.y += vy * engine.dt;

                vx *= 0.95f;
                vy *= 0.95f;
                break;
            }
        }

        engine_ClearBackground(&engine, BG_COLOR);

        switch (scene) {
            case MENU:
                engine_DrawText(&engine, 240, 250, "Press SPACE to start", 2, UI_COLOR);
                break;

            case PLAY:
                snprintf(fps_text, sizeof(fps_text), "FPS: %.2f", fps_smooth);
                engine_DrawText(&engine, 5, 5, fps_text, 1, UI_COLOR);
                engine_DrawSprite(&engine, &player);
                break;
        }

        engine_EndFrame(&engine);
    }

    engine_Quit(&engine);
    return 0;
}
