#include "bmp_surface_fetcher.h"
#include "emscripten.h"
#include <SDL2/SDL.h>

BmpSurfaceFetcher *bmpsf_init()
{
    BmpSurfaceFetcher *ctx = malloc(sizeof(BmpSurfaceFetcher));
    ctx->event_id = SDL_RegisterEvents(1);
    return ctx;
}

typedef struct
{
    const BmpSurfaceFetcher *ctx;
    const char *filename;
    SDL_Surface *surface;
} bmpsf_fetch_args;

static void push_surface_load_event(bmpsf_fetch_args *arguments)
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
    bmpsf_fetch_args *arguments = (bmpsf_fetch_args *)arg;
    const char *name = arguments->filename;
    const BmpSurfaceFetcher *ctx = arguments->ctx;

    SDL_RWops *file = SDL_RWFromConstMem(buffer, size);
    arguments->surface = SDL_LoadBMP_RW(file, 1);

    push_surface_load_event(arguments);
}

static void on_data_error(void *arg)
{
}

void bmpsf_fetch(const BmpSurfaceFetcher *context, const char *filename)
{
    bmpsf_fetch_args *f = malloc(sizeof(bmpsf_fetch_args));
    *f = (bmpsf_fetch_args){context, filename, NULL};
    emscripten_async_wget_data(filename, f, on_surface_load, on_data_error);
}

SDL_Surface *bmpsf_get_surface(const BmpSurfaceFetcher *context, const SDL_Event *e)
{
    if (e->type == context->event_id)
    {
        const bmpsf_fetch_args *args = (const bmpsf_fetch_args *)e->user.data1;
        return args->surface;
    }
    return NULL;
}

const char *bmpsf_get_filename(const BmpSurfaceFetcher *context, const SDL_Event *e)
{
    if (e->type == context->event_id)
    {
        const bmpsf_fetch_args *args = (const bmpsf_fetch_args *)e->user.data1;
        return args->filename;
    }
    return NULL;
}

void bmpsf_clear_event(const SDL_Event *e)
{
    free(e->user.data1);
}