#ifndef TUIBB
#define TUIBB

typedef struct TUIBB_CONTEXT TUIBB_CONTEXT;
struct TUIBB_CONTEXT* tuibb_init();

int tb_vertical_line(int x, int yStart, int yLength, uint16_t fg, int bg);
int tb_horizontal_line(int xStart, int y, int xLength, uint16_t fg, int bg);
int tuibb_print(int x, int y, const char* text);
int tuibb_print_int(int x, int y, int i);
int tuibb_textbox(struct TUIBB_CONTEXT* ctx, int x, int y, int xW, int yW, const char* str);
#endif
