#include "state_loading.h"
#include "audio_interface.h"
#include "data_loader.h"
#include "font_printer.h"
#include "globals.h"
#include "scoreboard.h"
#include "world_demo.h"

extern Globals G;
int _state_loading_timeout;

int _state_loading_x;

const char *_state_loading_strings[] = {"Loading -",  //
                                        "Loading \\", //
                                        "Loading |",  //
                                        "Loading /"}; //

void state_loading_enter()
{
    printf("Entering Loading State\n");

    world_demo_init();

    load_font("CodePage437Font.bmp");
    _state_loading_timeout = 0;
    _state_loading_x = 0;
    load_data("test.lvl");
    load_audio("midnight_drive.wav");
    load_audio("beep.wav");
    load_audio("crash.wav");
}

void state_loading_frame()
{
    _state_loading_timeout++;

    if (G.FontTexture != NULL)
    {
        if (_state_loading_x == 0)
        {
            _state_loading_x = (800 - font_get_text_width(_state_loading_strings[0])) / 2;
        }

        int num = (_state_loading_timeout / 6) % 4;
        font_print(_state_loading_strings[num], _state_loading_x, 200);

        if (G.w != NULL)
        {
            if (audio_is_ready() && _state_loading_timeout > 100)
            {
                G.s = scoreboard_init(G.w->checkpoint_count);
                gsm_set_state(&G.gsm, Simulation);
            }
        }
    }
}

void state_loading_exit()
{
    printf("Exiting Loading State\n");
}