#include "state_credits.h"
#include "audio_interface.h"
#include "font_printer.h"
#include "globals.h"
#include "world_demo.h"
#include <SDL2/SDL.h>

extern Globals G;

int _state_credits_char_width;

const char *_state_credits_text[] = {"Bike Island Zero",
                                     "A game by @nora_sellisa",
                                     "",
                                     "",
                                     "",
                                     "Special thanks to my friend, Magda, for her moral and technical support.",
                                     "",
                                     "There was no game in 2020, sorry :("};

void state_credits_enter()
{
    _state_credits_char_width = font_get_text_width("x");
}

void state_credits_frame()
{
    world_demo_frame();

    for (int i = 0; i < sizeof(_state_credits_text) / sizeof(_state_credits_text[0]); i++)
    {

        const char *text = _state_credits_text[i];
        int y = 200 + i * 32;

        int width = font_get_text_width(text);
        int x = (800 - width) / 2;

        font_print(text, x, y);
    }

    const char *menu_return = "Return to menu";

    int width = font_get_text_width(menu_return);
    int x = (800 - width) / 2;
    int left_x = x - _state_credits_char_width;
    int right_x = x + width;

    if ((SDL_GetTicks() / 500) % 2 == 0)
    {
        left_x -= _state_credits_char_width;
        right_x += _state_credits_char_width;
    }

    int y = 600 - 28;

    font_print(menu_return, x, y);
    font_print("\x11", left_x, y);
    font_print("\x10", right_x, y);

    if (G.keys[globals_key_space] > G.previous_keys[globals_key_space])
    {
        gsm_set_state(&(G.gsm), Menu);
    }
}
void state_credits_exit()
{
    audio_play_crash();
}