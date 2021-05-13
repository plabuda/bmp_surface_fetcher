#pragma once

void font_init();
void font_print(const char *text, int x, int y);
void font_print_time_stamp(int miliseconds, int x, int y);
int font_get_text_width(const char *text);