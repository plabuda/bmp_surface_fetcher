#include "state_playing.h"
#include "audio_interface.h"
#include "data_loader.h"
#include "font_printer.h"
#include "globals.h"
#include "scoreboard.h"
#include "world.h"
#include <SDL2/SDL.h>

extern Globals G;
const char *_state_waiting_info_string = "Cross the first checkpoint to start the timer";
int _state_string_offset_x;
int _state_playing_char_width;

//
// Waiting
//

void state_playing_waiting_enter()
{
    _state_string_offset_x = font_get_text_width(_state_waiting_info_string);
    _state_playing_char_width = font_get_text_width("x");
    _state_string_offset_x = (800 - _state_string_offset_x) / 2;
}

void state_playing_waiting_frame()
{
    world_steer(G.w, G.keys[0] - G.keys[1], G.keys[3] - G.keys[2]);

    Uint8 play_beep = SDL_FALSE;
    Uint8 play_crash = SDL_FALSE;
    world_update(G.w, &play_beep, &play_crash);

    if (play_beep)
    {
        audio_play_beep();
    }

    if (play_crash)
    {
        audio_play_crash();
    }

    if (G.w->current_checkpoint > 0)
    {
        gsm_set_state(&G.gsm, Playing);
    }

    world_render(G.renderer, G.w);
    font_print(_state_waiting_info_string, //
               _state_string_offset_x,     //
               400);
}

void state_playing_waiting_exit()
{
}

//
// Playing
//

void state_playing_enter()
{
    printf("Playing state enter\n");
}

void state_playing_frame()
{
    world_steer(G.w, G.keys[0] - G.keys[1], G.keys[3] - G.keys[2]);

    Uint8 play_beep = SDL_FALSE;
    Uint8 play_crash = SDL_FALSE;
    world_update(G.w, &play_beep, &play_crash);

    if (play_beep)
    {
        audio_play_beep();
    }

    if (play_crash)
    {
        audio_play_crash();
    }
    world_render(G.renderer, G.w);

    const char *time_format = "XX:YY.zzz";

    const char *total = "Total:";
    const char *segment = "Segment: ";
    const char *best = "   Best: ";

    int time_width = font_get_text_width(time_format);
    int lead_width = font_get_text_width(segment);
    int best_width = font_get_text_width(best);

    int y = 32;

    int total_time = SDL_GetTicks() - G.w->checkpoint_times[0];
    int best_time = G.s->best_total;

    font_print(total, 32, y);
    font_print_time_stamp(total_time, 32 + lead_width, y);
    font_print(best, 32 + lead_width + time_width, y);
    font_print_time_stamp(best_time, 32 + lead_width + time_width + best_width, y);

    if (total_time <= best_time)
    {
        font_print(" \x13", 32 + lead_width + time_width + best_width + time_width, y);
    }

    if (G.w->current_checkpoint > G.w->checkpoint_count)
    {
        scoreboard_submit_checkpoints(G.s, G.w->checkpoint_times, G.w->checkpoint_count + 1);
        gsm_set_state(&G.gsm, Playing_finished);
    }
    else
    {
        // Don't render the checkpoint data on last frame to prevent reading garbage
        y += 32;

        int segment_time = SDL_GetTicks() - G.w->checkpoint_times[G.w->current_checkpoint - 1];
        int best_segment = G.s->best_times[G.w->current_checkpoint - 1];

        font_print(segment, 32, y);
        font_print_time_stamp(segment_time, 32 + lead_width, y);
        font_print(best, 32 + lead_width + time_width, y);
        font_print_time_stamp(best_segment, 32 + lead_width + time_width + best_width, y);

        if (segment_time <= best_segment)
        {
            font_print(" \x13", 32 + lead_width + time_width + best_width + time_width, y);
        }
    }
}

void state_playing_exit()
{
    printf("Playing state exit\n");
}

//
// Finished
//

void state_playing_finished_enter()
{
    printf("Entering finished state\n");
    world_steer(G.w, 0, 0);
}

void state_playing_finished_frame()
{
    Uint8 play_crash = SDL_FALSE;
    world_update(G.w, NULL, &play_crash);

    if (play_crash)
    {
        audio_play_crash();
    }

    world_render(G.renderer, G.w);
    scoreboard_print(G.s);
    const char *menu_return = "Return to menu";

    int width = font_get_text_width(menu_return);
    int x = (800 - width) / 2;
    int left_x = x - _state_playing_char_width;
    int right_x = x + width;

    if ((SDL_GetTicks() / 500) % 2 == 0)
    {
        left_x -= _state_playing_char_width;
        right_x += _state_playing_char_width;
    }

    int y = 600 - 28;

    font_print(menu_return, x, y);
    font_print("\x11", left_x, y);
    font_print("\x10", right_x, y);

    if (G.keys[globals_key_space] > G.previous_keys[globals_key_space])
    {
        world_free(&(G.w));
        load_data("test.lvl");
        gsm_set_state(&(G.gsm), Menu);
    }
}

void state_playing_finished_exit()
{
}