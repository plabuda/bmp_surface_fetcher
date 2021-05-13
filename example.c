#include "bmp_surface_fetcher.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>

SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Texture *texture;

BmpSurfaceFetcher *fetcher;

void quit()
{
    emscripten_cancel_main_loop();
    printf("quitting \n");
}

void process_events()
{
    SDL_Event event;

    /* Grab all the events off the queue. */
    while (SDL_PollEvent(&event))
    {
        // Surface safely returns NULL when event is of a wrong type
        // so bmpsf_get_surface can be used as a test for event.type
        if (bmpsf_get_surface(fetcher, &event))
        {
            printf("BMP Surface fetched called\n");
            // bmpsf_get_filename returns the same const char pointer
            // that was passed into bmpsf_fetch, so if you want
            // to detect between multiple textures, pointer comparison
            // is sufficient
            printf("Filename is: %s\n", bmpsf_get_filename(fetcher, &event));

            surface = bmpsf_get_surface(fetcher, &event);

            // For this example, convert the surface to a texture
            texture = SDL_CreateTextureFromSurface(renderer, surface);

            // Important! To avoid casting away const qualifier of the filename
            // a small structure is allocated for each event
            // to avoid a memory leak, you need to call bmpsf_clear_event
            // the event is unusable after that operation
            bmpsf_clear_event(&event);
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

    // Simply copy the texture to the top left corner of the window
    if (texture != NULL)
    {
        SDL_RenderClear(renderer);

        SDL_Rect rect;
        SDL_zero(rect);
        SDL_QueryTexture(texture, NULL, NULL, &(rect.w), &(rect.h));
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_RenderPresent(renderer);
    }
}

int main()
{
    // Initialize a simple window and obtain the renderer
    SDL_Init(SDL_INIT_VIDEO);
    renderer = SDL_CreateRenderer(                                             //
        SDL_CreateWindow("BMP Fetcher Example", SDL_WINDOWPOS_UNDEFINED,       //
                         SDL_WINDOWPOS_UNDEFINED, 512, 512, SDL_WINDOW_SHOWN), //
        -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);             //

    // Initialize BMP Fetcher. This will register an ID for a custom SDL event
    fetcher = bmpsf_init();

    const char *filename = "texture.bmp";
    // Fetch a BMP file
    // Fetching happens on a separate thread, using emscripten_async_wget_data
    // When BMP is ready, an SDL_Event will be pushed onto queue, containing the surface
    bmpsf_fetch(fetcher, filename);

    emscripten_set_main_loop(frame, 60, 0);
}
