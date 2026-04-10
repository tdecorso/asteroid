#include "miniaudio.h"
#include <stdbool.h>

#define SOUND_POOL 4

typedef struct {
    ma_sound sounds[SOUND_POOL];
    int index;
} sound_t;

typedef ma_sound loop_t;

bool audio_Init(ma_engine* e);
bool audio_LoadSound(ma_engine* e, const char* path, sound_t* s);
bool audio_LoadLoop(ma_engine* e, const char* path, loop_t* l);
void audio_PlaySound(sound_t* s, float volume);
void audio_StartLoop(loop_t* l, float volume);
void audio_StopLoop(loop_t* l);
void audio_DestroySound(sound_t* s);
void audio_DestroyLoop(loop_t* l);
