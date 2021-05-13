
#include "globals.h"
#include <SDL2/SDL.h>

extern Globals G;

void state_simulation_enter()
{
}
void state_simulation_frame()
{
    SDL_Rect rect;
    SDL_zero(rect);

    SDL_QueryTexture(G.FontTexture, NULL, NULL, &(rect.w), &(rect.h));
    SDL_SetRenderTarget(G.renderer, NULL);
    SDL_RenderCopy(G.renderer, G.FontTexture, NULL, &rect);
}
void state_simulation_exit()
{
}