#include "data_loader.h"
#include "globals.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Globals G;

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

void on_data_error(void *arg)
{
    char *name = (char *)arg;
    printf("Data error: %s\n", name);
}

void load_font(char *name)
{
    emscripten_async_wget_data(name, name, on_font_load, on_data_error);
}
