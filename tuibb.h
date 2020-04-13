#ifndef TUIBB
#define TUIBB
int tb_vertical_line(int x, int yStart, int yLength, uint16_t fg, int bg);
int tb_horizontal_line(int xStart, int y, int xLength, uint16_t fg, int bg);
int tuibb_print(int x, int y, const char* text);
int tb_print_int(int x, int y, int i);
#endif
