#pragma once
#include <SDL2/SDL.h>

typedef enum {
  Sand,
  Sand_Dark,
  C,
  D,
  E,
  F,
  Water_Light,
  Water,
  Water_Dark,
  Metal_Light,
  Metal,
  Metal_Dark,
  M,
  Rust,
  Rust_Dark
} PaletteColor;

void palette_set_color(PaletteColor c);
Uint8 palette_get_r(PaletteColor c);
Uint8 palette_get_g(PaletteColor c);
Uint8 palette_get_b(PaletteColor c);