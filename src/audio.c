#define MINIAUDIO_IMPLEMENTATION
#include "audio.h"

bool audio_Init(ma_engine* e) {
    return ma_engine_init(NULL, e) == MA_SUCCESS;
}

bool audio_LoadSound(ma_engine* e, const char* path, sound_t* s) {
    for (int i = 0; i < SOUND_POOL; ++i) {
        if (ma_sound_init_from_file(e,path, 0, NULL, NULL, &s->sounds[i]) != MA_SUCCESS) return false;
    }
    return true;
}

bool audio_LoadLoop(ma_engine* e, const char* path, loop_t* l) {
    if (ma_sound_init_from_file(e, path, 0, NULL, NULL, l) != MA_SUCCESS) return false;
    ma_sound_set_looping(l, true);
    return true;
}

void audio_DestroySound(sound_t* s) {
    for (int i = 0; i < SOUND_POOL; ++i) {
        ma_sound_uninit(&s->sounds[i]);
    }
}

void audio_PlaySound(sound_t* s, float volume) {
    ma_sound* snd = &s->sounds[s->index];

    ma_sound_set_volume(snd, volume);

    ma_sound_stop(snd);
    ma_sound_seek_to_pcm_frame(snd, 0);
    ma_sound_start(snd);

    s->index = (s->index + 1) % SOUND_POOL;
}

void audio_StartLoop(loop_t* l, float volume) {
    ma_sound_set_volume(l, volume);
    ma_sound_start(l);
}

void audio_StopLoop(loop_t* l) {
    ma_sound_stop(l);
}

void audio_DestroyLoop(loop_t* l) {
    ma_sound_uninit(l);
}
