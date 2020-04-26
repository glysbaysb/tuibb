#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "third_party/termbox/src/termbox.h"
#include "third_party/liblist/inc/llist.h"

typedef struct TUIBB_ELEMENT {
	size_t x,
		  y,
		  xW,
		  yW,
		  id;
	const char* content;
} TUIBB_ELEMENT;

struct TUIBB_CONTEXT {
	llist elements;
};

int tb_vertical_line(int x, int yStart, int yLength, uint16_t fg, int bg) {
	for(int i = 0; i < yLength; i++) {
		tb_change_cell(x, yStart + i, ' ', fg, bg);
	}
	return 0;
}

int tb_horizontal_line(int xStart, int y, int xLength, uint16_t fg, int bg) {
	for(int i = 0; i < xLength; i++) {
		tb_change_cell(xStart + i, y, ' ', fg, bg);
	}
	return 0;
}

int tuibb_print(int x, int y, const char* text) {
	assert(x + strlen(text) < tb_width() && "int would go out of terminal");
	assert(y < tb_height() && "int would go out of terminal");

	struct tb_cell* cells = tb_cell_buffer();
	for(size_t i = 0; i < strlen(text); i++) {
		struct tb_cell* cell = &cells[y * tb_width() + x + i];
		cell->ch = text[i];
		tb_put_cell(x + i, y, cell);
	}

	return 0;
}


int tuibb_print_int(int x, int y, int i) {
	char buf[13] = {0};
	if(snprintf(buf, sizeof(buf), "%d", i) < 0) {
		return -1;
	}

	assert(x + strlen(buf) < tb_width() && "int would go out of terminal");
	assert(y < tb_height() && "int would go out of terminal");

	tuibb_print(x, y, buf);

	return 0;
}

int tuibb_textbox(struct TUIBB_CONTEXT* ctx, int x, int y, int xW, int yW, const char* str) {
	struct tb_cell* cells = tb_cell_buffer();

	/* borders */
	for(size_t i = 0; i < xW; i++) {
		struct tb_cell* cellBorder = &cells[y * tb_width() + x + i];
		if(i == 0) {
			cellBorder->ch = 0x250C;
		} else if(i == xW -1) {
			cellBorder->ch = 0x2510;
		} else {
			cellBorder->ch = 0x2500;
		}
		tb_put_cell(x + i, y, cellBorder);


		struct tb_cell* cellBorderBottom = &cells[(y + yW) * tb_width() + x + i];
		if(i == 0) {
			cellBorderBottom->ch = 0x2514;
		} else if(i == xW -1) {
			cellBorderBottom->ch = 0x2518;
		} else {
			cellBorderBottom->ch = 0x2500;
		}
		tb_put_cell(x + i, (y + yW), cellBorderBottom);
	}
	for(size_t i = 1; i < yW; i++) {
		struct tb_cell* cellBorderLeft = &cells[(y + i) * tb_width() + x];
		cellBorderLeft->ch = 0x2502;
		tb_put_cell(x, y + i, cellBorderLeft);

		struct tb_cell* cellBorderRight = &cells[(y + i) * tb_width() + x + xW - 1];
		cellBorderRight->ch = 0x2502;
		tb_put_cell(x + xW - 1, y + i, cellBorderRight);
	}

	/* text */
	size_t xPos = 0,
		   yPos = 0;
	for(size_t i = 0; i < strlen(str); i++) {
		if(str[i] == '\n') {
			yPos++;
		} else if(str[i] == '\r') {
			xPos = 0;
		} else {
			struct tb_cell* cellText = &cells[(y + 1 + yPos) * tb_width() + (x + 1 + xPos)];
			cellText->ch = str[i];
			tb_put_cell(x + 1 + xPos, y + 1 + yPos, cellText);

			xPos++;
			if(xPos == xW - 2) {
				yPos++;
				xPos = 0;
			}
		}
	}

	TUIBB_ELEMENT* el = calloc(1, sizeof(TUIBB_ELEMENT));
	el->x = x;
	el->y = y;
	el->xW = xW;
	el->yW = yW;
	el->content = strdup(str);

	el->id = llist_size(ctx->elements) + 1;

	llist_add_node(ctx->elements, el, ADD_NODE_REAR);
	return el->id;
}


struct TUIBB_CONTEXT* tuibb_init() {
	struct TUIBB_CONTEXT* ctx = (struct TUIBB_CONTEXT*)calloc(1, sizeof(struct TUIBB_CONTEXT));
	assert(ctx);

	ctx->elements = llist_create(NULL, NULL, FLAG_MT_SUPPORT);
	assert(ctx->elements);

	return ctx;
}
