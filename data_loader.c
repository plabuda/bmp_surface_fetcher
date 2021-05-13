#include "data_loader.h"
#include "globals.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Globals G;

void push_level_load_event(char *name, World *w)
{
    SDL_Event load_event;
    SDL_zero(load_event);
    load_event.type = G.LEVEL_LOAD;
    load_event.user.code = 0;
    load_event.user.data1 = name;
    load_event.user.data2 = w;
    SDL_PushEvent(&load_event);
}

void push_font_load_event(char *name, SDL_Texture *t)
{
    SDL_Event font_event;
    SDL_zero(font_event);
    font_event.type = G.FONT_LOAD;
    font_event.user.code = 0;
    font_event.user.data1 = name;
    font_event.user.data2 = t;
    SDL_PushEvent(&font_event);
}

void push_audio_load_event(char *name, void *buf, unsigned size)
{
    SDL_Event audio_event;
    SDL_zero(audio_event);
    audio_event.type = G.AUDIO_LOAD;
    audio_event.user.code = size;
    audio_event.user.data1 = name;
    audio_event.user.data2 = buf;
    SDL_PushEvent(&audio_event);
}

void on_level_load(void *arg, void *buffer, int size)
{
    printf("level received\n");
    char *name = (char *)arg;
    int floatsize = (size % sizeof(float) == 0) ? (size / sizeof(float)) : (size / sizeof(float) + 1);
    floatsize *= sizeof(float);
    World *w = world_from_floats(buffer);
    push_level_load_event(name, w);
}

void on_font_load(void *arg, void *buffer, int size)
{
    SDL_RWops *file = SDL_RWFromConstMem(buffer, size);
    SDL_Surface *surface = SDL_LoadBMP_RW(file, 1);
    SDL_SetColorKey(surface, 1, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));
    SDL_Texture *t = SDL_CreateTextureFromSurface(G.renderer, surface);
    SDL_FreeSurface(surface);

    char *name = (char *)arg;
    push_font_load_event(name, t);
}

void on_audio_load(unsigned handle, void *arg, void *buf, unsigned size)
{
    push_audio_load_event(arg, buf, size);
}

void on_data_error(void *arg)
{
    char *name = (char *)arg;
    printf("Data error: %s\n", name);
}

void load_data(char *name)
{
    emscripten_async_wget_data(name, name, on_level_load, on_data_error);
}

void load_font(char *name)
{
    emscripten_async_wget_data(name, name, on_font_load, on_data_error);
}

void load_audio(char *name)
{
    emscripten_async_wget2_data(name, "GET", NULL, name, 0, on_audio_load, NULL, NULL);
}