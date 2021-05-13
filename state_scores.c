#include "state_scores.h"
#include "audio_interface.h"
#include "font_printer.h"
#include "globals.h"
#include "world_demo.h"

extern Globals G;
int _state_scores_char_width;

void state_scores_enter()
{
    _state_scores_char_width = font_get_text_width("x");
}
void state_scores_frame()
{
    world_demo_frame();
    scoreboard_print_best(G.s);

    const char *menu_return = "Return to menu";

    int width = font_get_text_width(menu_return);
    int x = (800 - width) / 2;
    int left_x = x - _state_scores_char_width;
    int right_x = x + width;

    if ((SDL_GetTicks() / 500) % 2 == 0)
    {
        left_x -= _state_scores_char_width;
        right_x += _state_scores_char_width;
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
void state_scores_exit()
{
    audio_play_crash();
}