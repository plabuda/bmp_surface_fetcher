#include "bmp_surface_fetcher.h"
#include "data_loader.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>

Uint32 FONT_LOAD;

SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Texture *FontTexture;

const char *filename = "texture.bmp";

BmpSurfaceFetcher *ctx;

void quit()
{
    emscripten_cancel_main_loop();
    printf("quitting \n");
}

void process_events()
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event))
    {
        if (bmpsf_get_surface(ctx, &event))
        {
            printf("Surface event called\n");
            printf("Filename is: %s\n", bmpsf_get_filename(ctx, &event));
            surface = bmpsf_get_surface(ctx, &event);
            FontTexture = SDL_CreateTextureFromSurface(renderer, surface);
            bmpsf_clear_event(&event);
        }
        else if (event.type == FONT_LOAD)
        {
            surface = (SDL_Surface *)event.user.data1;
            FontTexture = SDL_CreateTextureFromSurface(renderer, surface);
        }
        else if (event.type == SDL_QUIT)
        {
            quit();
        }
    }
}

void frame()
{
    process_events();
    if (FontTexture != NULL)
    {
        SDL_RenderClear(renderer);
        SDL_Rect rect;
        SDL_zero(rect);

        SDL_QueryTexture(FontTexture, NULL, NULL, &(rect.w), &(rect.h));
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, FontTexture, NULL, &rect);
        SDL_RenderPresent(renderer);
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *w =
        SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    FONT_LOAD = SDL_RegisterEvents(1);

    ctx = bmpsf_init();

    bmpsf_fetch(ctx, filename);

    //  load_font("CodePage437Font.bmp");
    emscripten_set_main_loop(frame, 60, 0);
}
