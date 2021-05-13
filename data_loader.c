#include "data_loader.h"
#include <SDL2/SDL.h>
#include <emscripten.h>

extern Uint32 FONT_LOAD;

void push_font_load_event(SDL_Surface *s)
{
    SDL_Event font_event;
    SDL_zero(font_event);
    font_event.type = FONT_LOAD;
    font_event.user.data1 = s;
    SDL_PushEvent(&font_event);
}

void on_font_load(void *arg, void *buffer, int size)
{
    SDL_RWops *file = SDL_RWFromConstMem(buffer, size);
    SDL_Surface *surface = SDL_LoadBMP_RW(file, 1);
    push_font_load_event(surface);
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
