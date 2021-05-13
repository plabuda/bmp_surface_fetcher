#include "data_loader.h"
#include "globals.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>

Globals G;

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
        if (event.type == G.FONT_LOAD)
        {
            printf("Font load event\n");
            if (G.FontTexture != NULL)
            {
                SDL_DestroyTexture(G.FontTexture);
            }

            G.FontTexture = event.user.data2;
        }
        else if (event.type == SDL_QUIT)
        {
            quit();
        }
    }
}

void state_simulation_frame()
{
    SDL_Rect rect;
    SDL_zero(rect);

    SDL_QueryTexture(G.FontTexture, NULL, NULL, &(rect.w), &(rect.h));
    SDL_SetRenderTarget(G.renderer, NULL);
    SDL_RenderCopy(G.renderer, G.FontTexture, NULL, &rect);
}

void frame()
{
    process_events();
    SDL_RenderClear(G.renderer);
    if (G.FontTexture != NULL)
    {
        state_simulation_frame();
    }
    SDL_RenderPresent(G.renderer);
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_zero(G);
    SDL_Window *w =
        SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    G.renderer = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    G.FONT_LOAD = SDL_RegisterEvents(1);
    load_font("CodePage437Font.bmp");
    emscripten_set_main_loop(frame, 60, 0);
}
