# BMP Surface Fetcher

This is a simple, two file library for Emscripten + SDL2, which allows you to fetch a BMP file directly to a SDL_Surface object.

## Usage

Look into the [example.c] file for an usage example.

[example.c]: https://github.com/plabuda/blob/master/example.c

To include the library in your project, add the .c and .h files.

You can also get those files with [clib] package manager

[clib]: https://github.com/clibs/clib

TL;DR:
1. Initialize a fetcher
2. call bmpsf_fetch with a desired filename
3. When polling SDL Events, look out for an event with your freshly-fetched SDL_Surface

## LICENSE

MIT or Public Domain, whichever suits you best. Credit is appreciated, but not needed. If you find the lib useful, tell me about it, I'd be happy to hear that. 

## Attribution

[texture.bmp] used as an example comes from https://cc0textures.com/view?id=Bricks058, licensed under CC0

[texture.bmp]: https://github.com/plabuda/bmp_surface_fetcher/blob/master/texture.bmp
