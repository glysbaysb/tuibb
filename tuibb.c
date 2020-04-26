#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "third_party/termbox/src/termbox.h"
#include "third_party/liblist/inc/llist.h"

extern char* strdup(const char*);

typedef struct TUIBB_ELEMENT {
	size_t x,
		  y,
		  xW,
		  yW,
		  id;
	const char* content;
	bool editable;
} TUIBB_ELEMENT;

struct TUIBB_CONTEXT {
	llist elements;
};

static int tuibb_print_text(int x, int y, int xW, int yW, const char* str) {
	struct tb_cell* cells = tb_cell_buffer();
	size_t xPos = 0,
		   yPos = 0;

	for(size_t i = 0; i < strlen(str) && yPos <= yW; i++) {
		if(str[i] == '\n') {
			yPos++;
		} else if(str[i] == '\r') {
			xPos = 0;
		} else {
			struct tb_cell* cellText = &cells[(y + yPos) * tb_width() + (x + xPos)];
			cellText->ch = str[i];
			tb_put_cell(x + xPos, y + yPos, cellText);

			xPos++;
			if(xPos == xW) {
				yPos++;
				xPos = 0;
			}
		}
	}

	return 0;
}

int tuibb_label(struct TUIBB_CONTEXT* ctx, int x, int y, int xW, const char* str) {
	tuibb_print_text(x, y, xW, 1, str);

	TUIBB_ELEMENT* el = calloc(1, sizeof(TUIBB_ELEMENT));
	el->x = x;
	el->y = y;
	el->xW = xW;
	el->yW = 1;
	el->content = strdup(str);
	el->editable = false;

	el->id = llist_size(ctx->elements) + 1;

	llist_add_node(ctx->elements, el, ADD_NODE_REAR);
	return el->id;
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

	tuibb_print_text(x + 1, y + 1, xW - 2, yW - 2, str); // +1 to get into the box, -2 cuz of the borders

	/* add to ctx */
	TUIBB_ELEMENT* el = calloc(1, sizeof(TUIBB_ELEMENT));
	el->x = x;
	el->y = y;
	el->xW = xW;
	el->yW = yW;
	el->content = strdup(str);
	el->editable = true;

	el->id = llist_size(ctx->elements) + 1;

	llist_add_node(ctx->elements, el, ADD_NODE_REAR);
	return el->id;
}

static void _set_cursor(llist_node node, void *arg) {
	TUIBB_ELEMENT* el = (TUIBB_ELEMENT*)node;
	if(el->id == (int)arg) {
		const size_t BORDER_OFFSET = 1; // how much i need to add, to not put the cursor into the border
		tb_set_cursor(el->x + BORDER_OFFSET, el->y + BORDER_OFFSET);
	}
}

void tuibb_edit(struct TUIBB_CONTEXT* ctx, int id) {
	llist_for_each_arg(ctx->elements, _set_cursor, (void *)id);
}

struct COLOR_ELEMENT {
	int id;
	uint16_t fg,
			 bg;
};

static void _color_element(llist_node node, void *arg_) {
	struct COLOR_ELEMENT* arg = (struct COLOR_ELEMENT*)arg_;
	TUIBB_ELEMENT* el = (TUIBB_ELEMENT*)node;
	if(el->id != arg->id) {
		return;
	}

	struct tb_cell* cells = tb_cell_buffer();
	bool colorBottomBorder = el->yW > 1;
	for(size_t y = 0; y < el->yW + (int)colorBottomBorder; y++) {
		for(size_t x = 0; x < el->xW; x++) {
			struct tb_cell* cell = &cells[(el->x + x) + (el->y + y) * tb_width()];
			cell->bg = arg->bg;
			cell->fg = arg->fg;

			tb_put_cell(el->x + x, el->y + y, cell);
		}
	}
}

void tuibb_color_element(struct TUIBB_CONTEXT* ctx, int id, uint16_t fg, int bg) {
	struct COLOR_ELEMENT arg = {
		.id = id,
		.fg = fg,
		.bg = bg
	};
	llist_for_each_arg(ctx->elements, _color_element, (void *)&arg);
}

struct TUIBB_CONTEXT* tuibb_init() {
	struct TUIBB_CONTEXT* ctx = (struct TUIBB_CONTEXT*)calloc(1, sizeof(struct TUIBB_CONTEXT));
	assert(ctx);

	ctx->elements = llist_create(NULL, NULL, FLAG_MT_SUPPORT);
	assert(ctx->elements);

	return ctx;
}

struct UPDATE_CONTENT{
	int id;
	const char* buf;
};
static void _update_content(llist_node node, void* arg_) {
	struct UPDATE_CONTENT* arg = (struct UPDATE_CONTENT*)arg_;
	TUIBB_ELEMENT* el = (TUIBB_ELEMENT*)node;
	if(el->id == arg->id) {
		const size_t BORDER_OFFSET = el->yW > 1 ? 1 : 0; // how much i need to add, to not put the cursor into the border
		tuibb_print_text(el->x + BORDER_OFFSET, el->y + BORDER_OFFSET, el->xW, el->yW, arg->buf);
	}

	const char* newText = strdup(arg->buf);
	free(el->content);
	el->content = newText;
}

void tuibb_update_content(struct TUIBB_CONTEXT* ctx, int elementID, const char* buf) {
	struct UPDATE_CONTENT arg = {
		.id = elementID,
		.buf = buf
	};
	llist_for_each_arg(ctx->elements, _update_content, (void *)&arg);
}

