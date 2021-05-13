#include "audio_interface.h"
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

void process_keys(SDL_EventType type, int code)
{
    int value = type == SDL_KEYDOWN ? 1 : 0;

    int offset = code - SDL_SCANCODE_RIGHT;
    if (offset >= 0 && offset < 4)
    {
        G.keys[offset] = value;
    }
    else if (code == SDL_SCANCODE_SPACE || code == SDL_SCANCODE_RETURN)
    {
        G.keys[4] = value;
    }
}

void process_events()
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    memcpy(G.previous_keys, G.keys, globals_key_count);

    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            process_keys(event.type, event.key.keysym.scancode);
        }
        else if (event.type == G.LEVEL_LOAD)
        {
            printf("Level load event \n");
            if (G.w != NULL)
            {
                world_free(&(G.w));
            }
            G.w = (World *)(event.user.data2);
        }
        else if (event.type == G.FONT_LOAD)
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
        else if (event.type == G.AUDIO_LOAD)
        {
            printf("Audio load event: %s\n", (char *)event.user.data1);
            switch (((char *)event.user.data1)[0])
            {

            case 'm':
                audio_init_background_music(event.user.data2, event.user.code);
                break;

            case 'c':
                audio_init_crash(event.user.data2, event.user.code);
                break;

            case 'b':
                audio_init_beep(event.user.data2, event.user.code);
                break;
            }

            audio_start_playback();
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

    int w, h;
    int midx, midy;
    SDL_QueryTexture(G.t, NULL, NULL, &w, &h);

    midx = w / 2;
    midy = h / 2;

    SDL_Rect r;

    for (int i = 400; i > 380; i--)
    {
        r.w = (w * i) / 400;
        r.h = (h * i) / 400;
        r.x = midx - r.w / 2;
        r.y = midy - r.h / 2;

        SDL_RenderCopy(G.renderer, G.t, &r, NULL);
    }
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
    Uint32 format = SDL_GetWindowPixelFormat(w);
    printf("Format is: %u, RGBA is %u\n", format, SDL_PIXELFORMAT_RGB888);
    int width, height;
    SDL_GetRendererOutputSize(G.renderer, &width, &height);

    G.t = SDL_CreateTexture(G.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(G.t, SDL_BLENDMODE_BLEND);

    G.LEVEL_LOAD = SDL_RegisterEvents(3);
    G.FONT_LOAD = G.LEVEL_LOAD + 1;
    G.AUDIO_LOAD = G.FONT_LOAD + 1;

    renderable_init_window(800, 600);

    EM_ASM({
        FS.mkdir('/data');
        FS.mount(IDBFS, {}, '/data');
        FS.syncfs(true, function(err){
                            // handle callback
                        });
    });

    // Test the state machine on 2 example fields
    G.gsm = gsm_new();
    // Temporary change
    gsm_set_state(&(G.gsm), Loading);
    emscripten_set_main_loop(loop_fn, 60, 0);
}
