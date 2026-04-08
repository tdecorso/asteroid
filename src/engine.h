#include "platform.h"
#include "framebuffer.h"

typedef struct {
    sdl_platform_t platform;
    framebuffer_t fb;
    double dt;
    double frame_start;
    double target_dt;
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
