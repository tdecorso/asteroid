#include "platform.h"
#include "framebuffer.h"
#include "sprite.h"
#include "camera2D.h"
#include "audio.h"

typedef struct {
    sdl_platform_t platform;
    framebuffer_t fb;
    double dt;
    double frame_start;
    double target_dt;
    camera2D_t camera2D;
    ma_engine audio;
} engine_t;

bool engine_Init(engine_t* e);
bool engine_CreateWindow(engine_t* e, int w, int h, const char* title);
bool engine_WindowShouldClose(engine_t* e);
void engine_Quit(engine_t* e);
void engine_ClearBackground(engine_t* e, pixel_t color);
void engine_BeginFrame(engine_t* e);
void engine_EndFrame(engine_t* e);
bool engine_IsKeyPressed(engine_t* e, int scancode);
bool engine_IsKeyReleased(engine_t* e, int scancode);
bool engine_IsKeyDown(engine_t* e, int scancode);
void engine_SetTargetFPS(engine_t* e, int fps);
void engine_DrawText(engine_t* e, int x, int y, char* text, int scale, pixel_t color);
void engine_DrawSprite(engine_t* e, sprite_t* sprite);
float engine_RandF(engine_t* e, float low, float high);
bool engine_LoadSound(engine_t* e, const char* path, sound_t* s);
bool engine_LoadLoop(engine_t* e, const char* path, loop_t* l);
void engine_PlaySound(engine_t* e, sound_t* s, float volume);
void engine_StartLoop(engine_t* e, loop_t* l, float volume);
void engine_StopLoop(engine_t* e, loop_t* l);
