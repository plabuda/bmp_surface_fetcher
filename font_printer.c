#include "font_printer.h"
#include "globals.h"
#include <SDL2/SDL.h>

extern Globals G;
SDL_Rect _font_srcRect, _font_dstRect;

void font_init()
{
    if (G.FontTexture != NULL)
    {
        SDL_zero(_font_srcRect);
        SDL_zero(_font_dstRect);

        SDL_QueryTexture(G.FontTexture, NULL, NULL, &(_font_srcRect.w), &(_font_srcRect.h));
        _font_dstRect.w = _font_srcRect.w /= 32;
        _font_dstRect.h = _font_srcRect.h /= 8;
    }
}

void _font_print_char(char c)
{
    if (G.FontTexture != NULL)
    {
        _font_srcRect.x = _font_srcRect.w * (c % 32);
        _font_srcRect.y = _font_srcRect.h * (c / 32);
        SDL_RenderCopy(G.renderer, G.FontTexture, &_font_srcRect, &_font_dstRect);
    }
}

void _font_print_time(int minutes, int seconds, int miliseconds, int x, int y)
{
    char time_string[20];
    sprintf(time_string, "%02d:%02d.%03d", minutes, seconds, miliseconds);
    font_print(time_string, x, y);
}

void font_print_time_stamp(int miliseconds, int x, int y)
{
    int ms = miliseconds % 1000;
    int s = miliseconds / 1000;
    int m = s / 60;

    s %= 60;

    if (m > 99)
    {
        m = 99;
    }

    _font_print_time(m, s, ms, x, y);
}

void font_print(const char *text, int x, int y)
{
    _font_dstRect.x = x;
    _font_dstRect.y = y;

    int offset = 0;

    while (text[offset] != '\0')
    {
        if (text[offset] == '\n')
        {
            _font_dstRect.y += _font_dstRect.h;
            _font_dstRect.x = x;
        }
        else
        {
            _font_print_char(text[offset]);
            _font_dstRect.x += _font_dstRect.w;
        }
        offset++;
    }
}

int font_get_text_width(const char *text)
{
    int offset = 0;

    int current_width = 0;
    int max_width = 0;

    while (text[offset] != '\0')
    {
        if (text[offset] == '\n')
        {
            max_width = current_width > max_width ? current_width : max_width;
            current_width = 0;
        }
        else
        {
            current_width += _font_dstRect.w;
        }
        offset++;
    }

    return current_width > max_width ? current_width : max_width;
}