#include "engine.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_W 800
#define SCREEN_H 600

#define MAX_ASTEROIDS 8
#define MAX_BULLETS 32

#define WRAP_MARGIN 80.0f
#define FIRE_COOLDOWN 0.25f

const pixel_t BG_COLOR  = { 22,   8,  12, 255};
const pixel_t UI_COLOR  = {255, 150, 200, 255};
const pixel_t DIM_COLOR = {180, 100, 140, 255};

typedef enum {
    MENU,
    PLAY,
    GAME_OVER,
    WIN,
} scene_e;

typedef enum {
    AST_BIG,
    AST_MED,
    AST_SMALL
} asteroid_size_e;

typedef struct {
    sprite_t sprite;
    v2_t vel;
    asteroid_size_e size;
    int active;
} asteroid_t;

typedef struct {
    sprite_t sprite;
    v2_t vel;
    float life;
    int active;
} bullet_t;

typedef struct {
    sprite_t sprite;
    v2_t vel;
    int lives;
    float fire_timer;
    int invincible_timer;
    bool thrusting;
} player_t;

const char* tex1 = "/home/tony/dev/asteroid/assets/ship_1.png";
const char* tex2 = "/home/tony/dev/asteroid/assets/meteor.png";
const char* tex3 = "/home/tony/dev/asteroid/assets/laser.png";

const char* sound1 = "/home/tony/dev/asteroid/assets/laser.mp3";
const char* sound2 = "/home/tony/dev/asteroid/assets/hit.mp3";
const char* sound3 = "/home/tony/dev/asteroid/assets/explosion.mp3";
const char* sound4 = "/home/tony/dev/asteroid/assets/thrust.mp3";
const char* sound5 = "/home/tony/dev/asteroid/assets/start.mp3";
const char* sound6 = "/home/tony/dev/asteroid/assets/gameover.mp3";
const char* sound7 = "/home/tony/dev/asteroid/assets/win.mp3";
const char* sound8 = "/home/tony/dev/asteroid/assets/gameloop.mp3";
const char* sound9 = "/home/tony/dev/asteroid/assets/dead.mp3";

static float radians(float d) { return M_PI * d / 180.0f; }

static int circle_collide(v3_t a, float r1, v3_t b, float r2) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx*dx + dy*dy <= (r1 + r2)*(r1 + r2);
}

static float asteroid_radius(asteroid_t* a) {
    switch (a->size) {
        case AST_BIG:   return 60.0f;
        case AST_MED:   return 40.0f;
        case AST_SMALL: return 20.0f;
    }
    return 20.0f;
}

static void wrap_position(v3_t* pos, camera2D_t* cam) {
    float hw = SCREEN_W * 0.5f + WRAP_MARGIN;
    float hh = SCREEN_H * 0.5f + WRAP_MARGIN;

    float left   = cam->position.x - hw;
    float right  = cam->position.x + hw;
    float top    = cam->position.y - hh;
    float bottom = cam->position.y + hh;

    float width  = right - left;
    float height = bottom - top;

    while (pos->x > right) pos->x -= width;
    while (pos->x < left)  pos->x += width;

    while (pos->y > bottom) pos->y -= height;
    while (pos->y < top)    pos->y += height;
}

static void update_camera(sprite_t* player, camera2D_t* cam, float dt) {
    float t = 5.0f * dt;
    cam->position.x += (player->pos.x - cam->position.x) * t;
    cam->position.y += (player->pos.y - cam->position.y) * t;
}

static void player_init(player_t* p, texture_t* t) {
    p->sprite = (sprite_t){
        .texture = t,
        .pos = {0,0,0},
        .scale = {100,100},
        .rot = radians(0)
    };
    p->vel = (v2_t){0};
    p->lives = 3;
    p->fire_timer = 0.0f;
    p->invincible_timer = 120;
}

static void bullets_init(bullet_t* b, texture_t* t) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        b[i].sprite = (sprite_t) {
            .texture = t,
            .pos = {0, 0, 0},
            .scale = {10,10},
            .rot = 0,
        };
        b[i].active = 0;
    }
}

static void bullet_spawn(bullet_t* b, player_t* p, engine_t* e, sound_t* shoot) {
    if (p->fire_timer > 0.0f) return;

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!b[i].active) {
            b[i].active = 1;
            b[i].sprite.pos = p->sprite.pos;

            float a = p->sprite.rot + radians(270);
            b[i].sprite.rot = p->sprite.rot;
            b[i].vel.x = cosf(a) * 600.0f;
            b[i].vel.y = sinf(a) * 600.0f;

            b[i].life = 1.5f;
            p->fire_timer = FIRE_COOLDOWN;

            engine_PlaySound(e, shoot, 0.2f);
            return;
        }
    }
}

static void bullets_update(bullet_t* b, camera2D_t* cam, float dt) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!b[i].active) continue;

        b[i].sprite.pos.x += b[i].vel.x * dt;
        b[i].sprite.pos.y += b[i].vel.y * dt;
        b[i].life -= dt;

        wrap_position(&b[i].sprite.pos, cam);

        if (b[i].life <= 0) b[i].active = 0;
    }
}

static void bullets_draw(engine_t* e, bullet_t* b) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!b[i].active) continue;
        engine_DrawSprite(e, &b[i].sprite);
    }
}

static void asteroid_split(engine_t* e, asteroid_t* asteroids, asteroid_t* a, sound_t* explosion) {
    if (a->size == AST_SMALL) {
        engine_PlaySound(e, explosion, 0.5f);
        a->active = 0;
        return;
    }

    asteroid_size_e ns = (a->size == AST_BIG) ? AST_MED : AST_SMALL;

    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (!asteroids[i].active) {
                asteroids[i] = *a;
                asteroids[i].size = ns;
                asteroids[i].active = 1;

                asteroids[i].vel.x = engine_RandF(e, -150, 150);
                asteroids[i].vel.y = engine_RandF(e, -150, 150);

                float s = (ns == AST_MED) ? 100.0f : 60.0f;
                asteroids[i].sprite.scale = (v2_t){s, s};
                break;
            }
        }
    }

    a->active = 0;
}

static void asteroids_init(engine_t* e, asteroid_t* a, texture_t* t) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        a[i].active = 1;
        a[i].size = AST_BIG;

        a[i].sprite = (sprite_t){
            .texture = t,
            .pos = {
                engine_RandF(e, -SCREEN_W, SCREEN_W),
                engine_RandF(e, -SCREEN_H, SCREEN_H),
                engine_RandF(e, 0.01f, 0.2f),
            },
            .scale = {150,150},
            .rot = engine_RandF(e, 0, M_PI*2)
        };

        a[i].vel = (v2_t){
            engine_RandF(e, -100, 100),
            engine_RandF(e, -100, 100)
        };
    }
}

static void asteroids_update(asteroid_t* a, camera2D_t* cam, float dt) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!a[i].active) continue;

        a[i].sprite.pos.x += a[i].vel.x * dt;
        a[i].sprite.pos.y += a[i].vel.y * dt;
        a[i].sprite.rot  += 0.4f * dt;

        wrap_position(&a[i].sprite.pos, cam);
    }
}

static void asteroids_draw(engine_t* e, asteroid_t* a) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (a[i].active)
            engine_DrawSprite(e, &a[i].sprite);
    }
}

static int asteroids_alive(asteroid_t* a) {
    for (int i = 0; i < MAX_ASTEROIDS; i++)
        if (a[i].active) return 1;
    return 0;
}

static void bullets_vs_asteroids(engine_t* e, bullet_t* b, asteroid_t* a, player_t* p, sound_t* hit, sound_t* explosion) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!b[i].active) continue;

        for (int j = 0; j < MAX_ASTEROIDS; j++) {
            if (!a[j].active) continue;

            if (circle_collide(b[i].sprite.pos, 4, a[j].sprite.pos, asteroid_radius(&a[j]))) {
                engine_PlaySound(e, hit, 1.0f);
                b[i].active = 0;
                asteroid_split(e, a, &a[j], explosion);
                break;
            }
        }
    }
}

static void player_vs_asteroids(player_t* p, asteroid_t* a, engine_t* e, sound_t* dead) {
    if (p->invincible_timer > 0) return;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!a[i].active) continue;

        if (circle_collide(p->sprite.pos, 40, a[i].sprite.pos, asteroid_radius(&a[i]))) {
            engine_PlaySound(e, dead, 1.0f);
            p->lives--;
            p->sprite.pos = (v3_t){0,0,0};
            p->vel = (v2_t){0,0};
            p->invincible_timer = 120;
            break;
        }
    }
}

/* Draw the menu screen — asteroids drift behind it */
static void draw_menu(engine_t* e, float blink_timer) {
    int cx = SCREEN_W / 2;

    /* Title */
    engine_DrawText(e, cx - 144, 140, "ASTEROIDS", 4, UI_COLOR);

    /* Controls table */
    const int label_x = cx - 136;
    const int bind_x  = label_x + 176;
    const int row_h   = 22;
    const int table_y = 228;

    engine_DrawText(e, label_x, table_y + row_h * 0, "Thrust",  2, DIM_COLOR);
    engine_DrawText(e, bind_x,  table_y + row_h * 0, "W",       2, UI_COLOR);

    engine_DrawText(e, label_x, table_y + row_h * 1, "Rotate",  2, DIM_COLOR);
    engine_DrawText(e, bind_x,  table_y + row_h * 1, "A / D",   2, UI_COLOR);

    engine_DrawText(e, label_x, table_y + row_h * 2, "Fire",    2, DIM_COLOR);
    engine_DrawText(e, bind_x,  table_y + row_h * 2, "SPACE",   2, UI_COLOR);

    /* Blinking prompt */
    if ((int)(blink_timer * 2.0f) % 2 == 0)
        engine_DrawText(e, cx - 152, table_y + row_h * 4 + 16,
                        "Press ENTER to play", 2, UI_COLOR);
}

int main(void) {
    engine_t engine = {0};

    if (!engine_Init(&engine)) {
        printf("[ERROR] Failed to init the engine: %s\n", SDL_GetError());
        return 1;
    }
    engine_CreateWindow(&engine, SCREEN_W, SCREEN_H, "Asteroids");
    engine_SetTargetFPS(&engine, 60);

    texture_t player_tex   = {0};
    texture_t asteroid_tex = {0};
    texture_t bullet_tex   = {0};

    if (!texture_Create(&player_tex,   tex1)) {
        printf("[ERROR] Failed to load texture %s\n", tex1);
        return 1;
    }
    if (!texture_Create(&asteroid_tex, tex2)) {
        printf("[ERROR] Failed to load texture %s\n", tex2);
        return 1;
    }
    if (!texture_Create(&bullet_tex,   tex3)) {
        printf("[ERROR] Failed to load texture %s\n", tex3);
        return 1;
    }

    sound_t     laser_sound = {0};
    sound_t       hit_sound = {0};
    sound_t explosion_sound = {0};
    sound_t     start_sound = {0};
    sound_t  gameover_sound = {0}; 
    sound_t       win_sound = {0};
    sound_t      dead_sound = {0};

    loop_t thrust_loop = {0};
    loop_t   game_loop = {0};

    if (!engine_LoadSound(&engine, sound1, &laser_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound1);
        return 1;
    }
    if (!engine_LoadSound(&engine, sound2, &hit_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound2);
        return 1;
    }
    if (!engine_LoadSound(&engine, sound3, &explosion_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound3);
        return 1;
    }
    if (!engine_LoadLoop(&engine, sound4, &thrust_loop)) {
        printf("[ERROR] Failed to load loop %s\n", sound4);
        return 1;
    }
    if (!engine_LoadSound(&engine, sound5, &start_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound5);
        return 1;
    }
    if (!engine_LoadSound(&engine, sound6, &gameover_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound6);
        return 1;
    }
    if (!engine_LoadSound(&engine, sound7, &win_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound7);
        return 1;
    }
    if (!engine_LoadLoop(&engine, sound8, &game_loop)) {
        printf("[ERROR] Failed to load loop %s\n", sound8);
        return 1;
    }
    if (!engine_LoadSound(&engine, sound9, &dead_sound)) {
        printf("[ERROR] Failed to load sound %s\n", sound9);
        return 1;
    }

    player_t player;
    player_init(&player, &player_tex);

    asteroid_t asteroids[MAX_ASTEROIDS] = {0};
    asteroids_init(&engine, asteroids, &asteroid_tex);

    bullet_t bullets[MAX_BULLETS];
    bullets_init(bullets, &bullet_tex);

    scene_e scene = MENU;

    float smooth_fps  = 60.0f;
    float blink_timer = 0.0f;
    const float FPS_ALPHA = 0.05f;

    engine_StartLoop(&engine, &game_loop, 1.0f);

    while (!engine_WindowShouldClose(&engine)) {
        engine_BeginFrame(&engine);

        if (engine.dt > 0.0f) {
            float inst_fps = 1.0f / engine.dt;
            smooth_fps = smooth_fps + FPS_ALPHA * (inst_fps - smooth_fps);
        }

        blink_timer += engine.dt;
        update_camera(&player.sprite, &engine.camera2D, engine.dt);
        asteroids_update(asteroids, &engine.camera2D, engine.dt);

        if (scene == MENU) {
            if (engine_IsKeyPressed(&engine, SDL_SCANCODE_RETURN) ||
                engine_IsKeyPressed(&engine, SDL_SCANCODE_KP_ENTER)) {
                engine_PlaySound(&engine, &start_sound, 1.0f);
                player_init(&player, &player_tex);
                asteroids_init(&engine, asteroids, &asteroid_tex);
                bullets_init(bullets, &bullet_tex);
                engine.camera2D.position = (v2_t){0};
                smooth_fps = 60.0f;
                scene = PLAY;
            }
        }
        else if (scene == PLAY) {
            bool thrusting = engine_IsKeyDown(&engine, SDL_SCANCODE_W);
            if (thrusting) {
                if (!player.thrusting) engine_StartLoop(&engine, &thrust_loop, 0.4f);
                float a = player.sprite.rot + radians(90);
                player.vel.x -= cosf(a) * 700 * engine.dt;
                player.vel.y -= sinf(a) * 700 * engine.dt;
            }

            if (!thrusting && player.thrusting) {
                engine_StopLoop(&engine, &thrust_loop);
            }

            player.thrusting = thrusting;

            if (engine_IsKeyDown(&engine, SDL_SCANCODE_A))
                player.sprite.rot += 6 * engine.dt;

            if (engine_IsKeyDown(&engine, SDL_SCANCODE_D))
                player.sprite.rot -= 6 * engine.dt;

            if (engine_IsKeyPressed(&engine, SDL_SCANCODE_SPACE) ||
                engine_IsKeyDown(&engine,    SDL_SCANCODE_SPACE)) {
                bullet_spawn(bullets, &player, &engine, &laser_sound);
            }

            player.sprite.pos.x += player.vel.x * engine.dt;
            player.sprite.pos.y += player.vel.y * engine.dt;

            player.vel.x *= 0.98f;
            player.vel.y *= 0.98f;

            wrap_position(&player.sprite.pos, &engine.camera2D);

            if (player.fire_timer > 0.0f)   player.fire_timer -= engine.dt;
            if (player.invincible_timer > 0) player.invincible_timer--;

            bullets_update(bullets, &engine.camera2D, engine.dt);

            bullets_vs_asteroids(&engine, bullets, asteroids, &player, &hit_sound, &explosion_sound);
            player_vs_asteroids(&player, asteroids, &engine, &dead_sound);

            if (player.lives <= 0) {
                engine_StopLoop(&engine, &game_loop);
                engine_PlaySound(&engine, &gameover_sound, 0.4f);
                scene = GAME_OVER;
            }
            else if (!asteroids_alive(asteroids)) {
                engine_StopLoop(&engine, &game_loop);
                engine_PlaySound(&engine, &win_sound, 0.8f);
                scene = WIN;
            }
        }
        else {
            if (engine_IsKeyPressed(&engine, SDL_SCANCODE_R)) {
                player_init(&player, &player_tex);
                asteroids_init(&engine, asteroids, &asteroid_tex);
                bullets_init(bullets, &bullet_tex);
                engine.camera2D.position = (v2_t){0};
                smooth_fps = 60.0f;
                engine_PlaySound(&engine, &start_sound, 0.6f);
                scene = PLAY;
                engine_StartLoop(&engine, &game_loop, 1.0f);
            }

            engine_StopLoop(&engine, &thrust_loop);
        }

        /* --- Draw --- */
        engine_ClearBackground(&engine, BG_COLOR);

        asteroids_draw(&engine, asteroids);

        if (scene != MENU) {
            bullets_draw(&engine, bullets);

            if (player.invincible_timer == 0 || (player.invincible_timer / 6) % 2 == 0)
                engine_DrawSprite(&engine, &player.sprite);

            /* HUD */
            char txt[128];

            snprintf(txt, sizeof(txt), "FPS: %d", (int)(smooth_fps + 0.5f));
            engine_DrawText(&engine, 10, 10, txt, 1, UI_COLOR);

            if (player.lives == 3) {
                snprintf(txt, sizeof(txt), "Lives: O O O");
            }
            else if (player.lives == 2) {
                snprintf(txt, sizeof(txt), "Lives: O O X");
            }
            else if (player.lives == 1) {
                snprintf(txt, sizeof(txt), "Lives: O X X");
            }

            if (player.lives > 0) engine_DrawText(&engine, 10, 30, txt, 1, UI_COLOR);
        }

        /* Overlays */
        if (scene == MENU) {
            draw_menu(&engine, blink_timer);
        }
        else if (scene == GAME_OVER) {
            engine_DrawText(&engine, SCREEN_W/2 - 144, SCREEN_H/2 - 90,
                            "GAME OVER", 4, UI_COLOR);
            engine_DrawText(&engine, SCREEN_W/2 - 144, SCREEN_H/2 + 60,
                            "Press R to restart", 2, UI_COLOR);
        }
        else if (scene == WIN) {
            char txt[128];
            engine_DrawText(&engine, SCREEN_W/2 - 112, SCREEN_H/2 - 90,
                            "YOU WIN!", 4, UI_COLOR);
            engine_DrawText(&engine, SCREEN_W/2 - 168, SCREEN_H/2 + 60,
                            "Press R to play again", 2, UI_COLOR);
        }

        engine_EndFrame(&engine);
    }

    engine_StopLoop(&engine, &game_loop);

    texture_Destroy(&player_tex);
    texture_Destroy(&asteroid_tex);
    texture_Destroy(&bullet_tex);

    audio_DestroySound(&laser_sound);
    audio_DestroySound(&hit_sound);
    audio_DestroySound(&explosion_sound);
    audio_DestroySound(&start_sound);
    audio_DestroySound(&gameover_sound);
    audio_DestroySound(&win_sound);
    audio_DestroySound(&dead_sound);

    audio_DestroyLoop(&thrust_loop);
    audio_DestroyLoop(&game_loop);

    engine_Quit(&engine);
    return 0;
}
