#ifndef TUIBB
#define TUIBB

typedef struct TUIBB_CONTEXT TUIBB_CONTEXT;
struct TUIBB_CONTEXT* tuibb_init();

/**
 * Generates a textbox
 *
 * Tries to not print the text outside of the textbox -- if it doesn't
 * fit the remaining text is _silently_ ignored
 */
int tuibb_textbox(struct TUIBB_CONTEXT* ctx, int x, int y, int xW, int yW, const char* str);

/**
 * Generates a label
 */
int tuibb_label(struct TUIBB_CONTEXT* ctx, int x, int y, int xW, const char* str);

/**
 * Place the cursor into this element
 */
void tuibb_edit(struct TUIBB_CONTEXT* ctx, int id);

/**
 * Color element
 */
void tuibb_color_element(struct TUIBB_CONTEXT* ctx, int id, uint16_t fg, int bg);

#endif
