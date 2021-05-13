#include "bmp_surface_fetcher.h"
#include "emscripten.h"
#include <SDL2/SDL.h>

SurfaceFetcherContext *init_surface_fetcher()
{
    SurfaceFetcherContext *ctx = malloc(sizeof(SurfaceFetcherContext));
    ctx->event_id = SDL_RegisterEvents(1);
    return ctx;
}

typedef struct
{
    const SurfaceFetcherContext *ctx;
    const char *filename;
    SDL_Surface *surface;
} fetch_args;

static void push_surface_load_event(fetch_args *arguments)
{
    SDL_Event load_event;
    SDL_zero(load_event);
    load_event.type = arguments->ctx->event_id;
    load_event.user.code = 0;
    load_event.user.data1 = arguments;
    SDL_PushEvent(&load_event);
}

static void on_surface_load(void *arg, void *buffer, int size)
{
    fetch_args *arguments = (fetch_args *)arg;
    const char *name = arguments->filename;
    const SurfaceFetcherContext *ctx = arguments->ctx;

    SDL_RWops *file = SDL_RWFromConstMem(buffer, size);
    arguments->surface = SDL_LoadBMP_RW(file, 1);

    push_surface_load_event(arguments);
}

static void on_data_error(void *arg)
{
    const char *name = (const char *)arg;
    printf("bmp fetching error: %s\n", name);
}

void fetch_surface_bmp(const SurfaceFetcherContext *context, const char *filename)
{
    fetch_args *f = malloc(sizeof(fetch_args));
    *f = (fetch_args){context, filename, NULL};
    emscripten_async_wget_data(filename, f, on_surface_load, on_data_error);
}

SDL_Surface *get_surface_bmp(const SurfaceFetcherContext *context, const SDL_Event *e)
{
    if (e->type == context->event_id)
    {
        const fetch_args *args = (const fetch_args *)e->user.data1;
        return args->surface;
    }
    return NULL;
}

const char *get_surface_filename(const SurfaceFetcherContext *context, const SDL_Event *e)
{
    if (e->type == context->event_id)
    {
        const fetch_args *args = (const fetch_args *)e->user.data1;
        return args->filename;
    }
    return NULL;
}

void clear_surface_event(const SDL_Event *e)
{
    free(e->user.data1);
}