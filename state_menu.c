#include "state_menu.h"
#include "audio_interface.h"
#include "font_printer.h"
#include "globals.h"
#include "world.h"
#include "world_demo.h"

extern Globals G;

int _state_menu_selected = 0;
int _state_menu_char_width;

const char *_state_menu_options[] = {"Start", "High Scores", "Credits"};

void state_menu_enter()
{
    _state_menu_char_width = font_get_text_width("a");
}

void state_menu_frame()
{
    world_demo_frame();

    if (G.keys[globals_key_space] > G.previous_keys[globals_key_space])
    {

        State states[] = {Playing_waiting, Scores, Credits};

        if (_state_menu_selected != 0 || G.w != NULL)
        {
            gsm_set_state(&(G.gsm), states[_state_menu_selected]);
        }
    }
    else
    {
        if (G.keys[globals_key_down] > G.previous_keys[globals_key_down])
        {
            _state_menu_selected++;
            audio_play_beep();
        }

        if (G.keys[globals_key_up] > G.previous_keys[globals_key_up])
        {
            _state_menu_selected--;
            audio_play_beep();
        }

        _state_menu_selected = (_state_menu_selected + 3) % 3;
    }

    const char *title = "Bike Island Zero";
    const char *arrows = "\x18: Accelerate | \x19: Brake / Reverse | \x1B\x1A: Turn";
    const char *accept = " Spacebar / Enter: Select menu option";

    int text_x = (800 - font_get_text_width(title)) / 2;
    font_print(title, text_x, 200);
    text_x = (800 - font_get_text_width(arrows)) / 2;
    font_print(arrows, text_x, 200 + 5 * 32);
    text_x = (800 - font_get_text_width(accept)) / 2;
    font_print(accept, text_x, 200 + 6 * 32);

    for (int i = 0; i < _state_menu_selected; i++)
    {
        int x = font_get_text_width(_state_menu_options[i]);
        x = (800 - x) / 2;
        font_print(_state_menu_options[i], x, 600 - (3 * 20 + 8) + 20 * i);
    }

    int width = font_get_text_width(_state_menu_options[_state_menu_selected]);
    int x = (800 - width) / 2;
    int left_x = x - _state_menu_char_width;
    int right_x = x + width;

    if ((SDL_GetTicks() / 500) % 2 == 0)
    {
        left_x -= _state_menu_char_width;
        right_x += _state_menu_char_width;
    }

    int y = 600 - (3 * 20 + 8) + 20 * _state_menu_selected;

    font_print(_state_menu_options[_state_menu_selected], x, y);
    font_print("\x11", left_x, y);
    font_print("\x10", right_x, y);

    for (int i = _state_menu_selected + 1; i < 3; i++)
    {
        int x = font_get_text_width(_state_menu_options[i]);
        x = (800 - x) / 2;
        font_print(_state_menu_options[i], x, 600 - (3 * 20 + 8) + 20 * i);
    }
}

void state_menu_exit()
{
    audio_play_crash();
}