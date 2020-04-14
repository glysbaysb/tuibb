#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "third_party/termbox/src/termbox.h"
#include "tuibb.h"


void print_timeofday() {
	time_t currentTimeInt = time(NULL);
	assert(currentTimeInt != -1);
	struct tm currentTimeStruct;
	localtime_r(&currentTimeInt, &currentTimeStruct);

	char buf[3 + 3 + 3 + 1] = {0};
	strftime(buf, sizeof(buf), "%H:%M:%S", &currentTimeStruct);
	tuibb_print(0, tb_height() - 1, buf);
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

	int termboxFDs[2] = {0, 0};
	ret = tb_poll_fds(termboxFDs, 2);
	assert(ret == 2);
	assert(termboxFDs[0] != 0);
	assert(termboxFDs[1] != 0);

	tb_horizontal_line(0, tb_height() - 1, tb_width(), TB_DEFAULT, TB_GREEN);
	print_timeofday();
	tb_present();
	sleep(2);

	tb_shutdown();
	return 0;
}
