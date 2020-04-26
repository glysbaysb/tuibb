#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <errno.h>
#include "third_party/termbox/src/termbox.h"
#include "tuibb.h"


int print_timeofday_element(struct TUIBB_CONTEXT* ctx) {
	time_t currentTimeInt = time(NULL);
	assert(currentTimeInt != -1);
	struct tm currentTimeStruct;
	localtime_r(&currentTimeInt, &currentTimeStruct);

	char buf[3 + 3 + 3 + 1] = {0};
	strftime(buf, sizeof(buf), "%H:%M:%S\ntest", &currentTimeStruct);

	int lbl = tuibb_label(ctx, 0, tb_height() - 1, tb_width(), buf);
	tuibb_color_element(ctx, lbl, TB_DEFAULT, TB_GREEN);
	return lbl;
}

void update_timeofday_element(struct TUIBB_CONTEXT* ctx, int elementID) {
	time_t currentTimeInt = time(NULL);
	assert(currentTimeInt != -1);
	struct tm currentTimeStruct;
	localtime_r(&currentTimeInt, &currentTimeStruct);

	char buf[3 + 3 + 3 + 1] = {0};
	strftime(buf, sizeof(buf), "%H:%M:%S", &currentTimeStruct);

	tuibb_update_content(ctx, elementID, buf);
}



int main(int argc, char** argv) {
	(void)argc; (void)argv;

	int ret = tb_init();
	if (ret) {
		printf("tb_init() failed: %d\n", ret);
		return -1;
	}
	tb_clear();

	struct TUIBB_CONTEXT* ctx = tuibb_init();
	assert(ctx);


	/* have some basic layout */
	int textbox = tuibb_textbox(ctx, 0, 0, 20, 20, "abc\r\ndefghijklmnopqrstuvwxyz");
	assert(textbox > 0);
	tuibb_edit(ctx, textbox);

	int otherTextbox = tuibb_textbox(ctx, 20, 0, 20, 20, "a\r\nb\r\nc\r\nd\r\ne");
	tuibb_color_element(ctx, otherTextbox, TB_BOLD | TB_RED, TB_DEFAULT);
	assert(otherTextbox > 0);

	int todLbl = print_timeofday_element(ctx);
	tb_present();

	/* evt loop */
	int termboxFDs[2] = {0, 0};
	ret = tb_poll_fds(termboxFDs, 2);
	assert(ret == 2);
	assert(termboxFDs[0] != 0);
	assert(termboxFDs[1] != 0);
	struct pollfd pollfds[2] = {
		{termboxFDs[0], POLLIN, 0},
		{termboxFDs[0], POLLIN, 0},
	};
	while(poll(pollfds, 2, 2000) >= 0) {
		if((pollfds[0].revents & POLLIN) == POLLIN) {
			struct tb_event evt;
			tb_peek_event(&evt, 0);
			if(evt.type == TB_EVENT_KEY && evt.ch == 'q') {
				break;
			}
		}

		update_timeofday_element(ctx, todLbl);
		tb_present();
	}

	tb_shutdown();

	return 0;
}
