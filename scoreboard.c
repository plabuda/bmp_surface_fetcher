#include "scoreboard.h"
#include "font_printer.h"
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdlib.h>
#include <string.h>

Scoreboard *scoreboard_init(int time_count)
{
    Scoreboard *result = malloc(sizeof(Scoreboard));
    result->time_count = time_count;
    result->times = malloc(time_count * sizeof(Uint32));
    result->best_times = malloc(time_count * sizeof(Uint32));
    result->total = 0;

    // load from file here
    const char *filename = "data/scoreboard";
    FILE *file;

    printf("Opening file for read\n");
    if ((file = fopen(filename, "rb")))
    {
        printf("File opened\n");
        fread(&(result->best_total), sizeof(Uint32), 1, file);
        fread(result->best_times, sizeof(Uint32), result->time_count, file);
        fclose(file);
        printf("File closed\n");
    }
    else
    {
        printf("Failed to open file\n");

        result->best_total = 1000 * 60 * 99 + 1000 * 59 + 999;
        for (int i = 0; i < result->time_count; i++)
        {
            result->best_times[i] = result->best_total;
        }
    }
    return result;
}

void _scoreboard_print_extended(Scoreboard *s, int show_current)
{

    const int checkpoint_text_width = sizeof("Checkpoint #20") - 1;
    const int time_width = sizeof("XX:YY.000") - 1;

    const int gap_first = 23;
    const int gap_second = 7;

    const int char_width = font_get_text_width("x");

    int beginning = (checkpoint_text_width + gap_first + time_width + gap_second + time_width);
    beginning = (800 - beginning * char_width) / 2;

    const int time_first = beginning + (checkpoint_text_width + gap_first) * char_width;
    const int time_second = beginning + (checkpoint_text_width + gap_first + time_width + gap_second) * char_width;
    const int time_final =
        beginning + (checkpoint_text_width + gap_first + time_width + gap_second + time_width) * char_width;

    char time_string[20];
    int y = 32;

    if (show_current)
    {
        font_print("Yours: ", time_first, y);
    }

    font_print("Best: ", time_second, y);
    y += 32;

    font_print("Total time", beginning, y);
    if (show_current)
    {
        font_print_time_stamp(s->total, time_first, y);
        if (s->best_total >= s->total)
        {
            if ((SDL_GetTicks() / 500) % 2 == 1)
            {
                font_print(" \x13", time_final, y);
            }
        }
    }
    font_print_time_stamp(s->best_total, time_second, y);
    y += 64;

    for (int i = 0; i < s->time_count; i++)
    {
        sprintf(time_string, "Checkpoint #%d", i + 1);
        font_print(time_string, beginning, y);

        font_print_time_stamp(s->best_times[i], time_second, y);

        if (show_current)
        {
            font_print_time_stamp(s->times[i], time_first, y);
            if (s->best_times[i] >= s->times[i])
            {
                if ((SDL_GetTicks() / 500) % 2 == 1)
                {
                    font_print(" \x13", time_final, y);
                }
            }
        }
        y += 32;
    }
}

void scoreboard_print_best(Scoreboard *s)
{
    _scoreboard_print_extended(s, 0);
}

void scoreboard_print(Scoreboard *s)
{
    _scoreboard_print_extended(s, 1);
}

void scoreboard_submit_checkpoints(Scoreboard *s, Uint32 *checkpoints, int checkpoint_count)
{
    s->total = 0;

    for (int i = 0; i < checkpoint_count - 1; i++)
    {
        s->times[i] = checkpoints[i + 1] - checkpoints[i];
        s->total += s->times[i];

        if (s->best_times[i] > s->times[i])
        {
            s->best_times[i] = s->times[i];
        }
    }

    if (s->best_total > s->total)
    {
        s->best_total = s->total;
    }

    // save to file here
    const char *filename = "data/scoreboard";

    FILE *file;
    if ((file = fopen(filename, "wb")))
    {
        printf("File opened for write\n");
        fwrite(&(s->best_total), sizeof(Uint32), 1, file);
        fwrite(s->best_times, sizeof(Uint32), s->time_count, file);
        fclose(file);
        printf("File closed\n");
    }
    else
    {
        printf("Cannot open file\n");
    }

    EM_ASM({
        FS.syncfs(false, function(err){
                             // handle callback
                         });
    });
}

void scoreboard_free(Scoreboard *s)
{
    free(s->times);
    free(s->best_times);
}