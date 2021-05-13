#pragma once
#include <SDL2/SDL.h>

void audio_init_background_music(char *buf, unsigned size);
void audio_init_crash(char *buf, unsigned size);
void audio_init_beep(char *buf, unsigned size);

Uint8 audio_is_ready();

void audio_start_playback();

void audio_play_beep();
void audio_play_crash();
void audio_uninit();