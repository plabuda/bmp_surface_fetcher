#include "data_loader.h"
#include "font_printer.h"
#include "game_state_machine.h"
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

void frame()
{
    palette_set_color(Sand);
    SDL_RenderClear(G.renderer);
    gsm_frame(&G.gsm);
    SDL_RenderPresent(G.renderer);
}

/**
 * The loop handler, will be called repeatedly
 */
void loop_fn()
{
    process_events();
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

    G.gsm = gsm_new();
    // Temporary change
    gsm_set_state(&(G.gsm), Loading);
    emscripten_set_main_loop(loop_fn, 60, 0);
}
