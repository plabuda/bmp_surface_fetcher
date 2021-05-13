#pragma once
#include <SDL2/SDL.h>

typedef struct {
  Uint32 event_id;
} SurfaceFetcherContext;

SurfaceFetcherContext *init_surface_fetcher();
void fetch_surface_bmp(const SurfaceFetcherContext *context,
                       const char *filename);
SDL_Surface *get_surface_bmp(const SurfaceFetcherContext *context,
                             const SDL_Event *e);
const char *get_surface_filename(const SurfaceFetcherContext *context,
                                 const SDL_Event *e);

void clear_surface_event(const SDL_Event *e);