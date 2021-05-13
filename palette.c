#include "palette.h"
#include "globals.h"
#include <SDL2/SDL.h>

extern Globals G;

// https://lospec.com/palette-list/a-s-i-a-n-1-5
const Uint8 _palette_values[][3] = {
    [Sand] = "\xdc\xd0\xb7",        [Sand_Dark] = "\xc9\xb8\x92", [C] = "\xb8\xa1\x6f",
    [D] = "\xe8\x8f\x54",           [E] = "\xc8\x69\x56",         [F] = "\xb0\x48\x33",
    [Water_Light] = "\x2f\x95\xa1", [Water] = "\x00\x6b\x86",     [Water_Dark] = "\x0f\x30\x54",
    [Metal_Light] = "\x3a\x55\x59", [Metal] = "\x32\x48\x4d",     [Metal_Dark] = "\x22\x35\x39",
    [M] = "\x8c\x42\x1d",           [Rust] = "\x72\x26\x23",      [Rust_Dark] = "\x52\x0b\x0b"};

void palette_set_color(PaletteColor c)
{
    const Uint8 *color = _palette_values[c];
    SDL_SetRenderDrawColor(G.renderer, color[0], color[1], color[2], SDL_ALPHA_OPAQUE);
}

Uint8 _palette_get_component(PaletteColor c, int index)
{
    const Uint8 *color = _palette_values[c];
    return color[index];
}

Uint8 palette_get_r(PaletteColor c)
{
    return _palette_get_component(c, 0);
}

Uint8 palette_get_g(PaletteColor c)
{
    return _palette_get_component(c, 1);
}

Uint8 palette_get_b(PaletteColor c)
{
    return _palette_get_component(c, 2);
}