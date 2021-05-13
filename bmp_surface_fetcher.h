#pragma once
#include <SDL2/SDL.h>

typedef struct {
  Uint32 event_id;
} BmpSurfaceFetcher;

BmpSurfaceFetcher *bmpsf_init();

void bmpsf_fetch(const BmpSurfaceFetcher *context, const char *filename);

SDL_Surface *bmpsf_get_surface(const BmpSurfaceFetcher *context,
                               const SDL_Event *e);
const char *bmpsf_get_filename(const BmpSurfaceFetcher *context,
                               const SDL_Event *e);

void bmpsf_clear_event(const SDL_Event *e);