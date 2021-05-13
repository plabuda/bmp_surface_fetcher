#include "data_loader.h"
#include "font_printer.h"
#include "globals.h"
#include "palette.h"
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
            SDL_SetTextureColorMod(G.FontTexture,             //
                                   palette_get_r(Water_Dark), //
                                   palette_get_g(Water_Dark), //
                                   palette_get_b(Water_Dark));
            font_init();
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
    palette_set_color(Sand);
    SDL_RenderClear(G.renderer);
    if (G.FontTexture != NULL)
    {
        state_simulation_frame();
    }
    SDL_RenderPresent(G.renderer);
}

/**
 * The loop handler, will be called repeatedly
 */
void loop_fn()
{
    const int nominator = 60;
    const int denominator = 1000;
    static int accumulated_ticks = 0;
    static int old_ticks = 0;
    static int new_ticks = 0;

    new_ticks = SDL_GetTicks();

    accumulated_ticks += (new_ticks - old_ticks) * nominator;
    old_ticks = new_ticks;

    if (accumulated_ticks > denominator)
    {
        frame();
        accumulated_ticks -= denominator;
    }
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
