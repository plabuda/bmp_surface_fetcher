#include "data_loader.h"
#include "globals.h"

extern Globals G;

void state_loading_enter()
{
    load_font("CodePage437Font.bmp");
}

void state_loading_frame()
{
    if (G.FontTexture != NULL)
    {
        gsm_set_state(&G.gsm, Simulation);
    }
}

void state_loading_exit()
{
    printf("Exiting Loading State\n");
}