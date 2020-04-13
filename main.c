#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "third_party/termbox/src/termbox.h"

int tuibb_print(int x, int y, const char* text) {
	assert(x + strlen(text) < tb_width() && "int would go out of terminal");
	assert(y < tb_height() && "int would go out of terminal");

	for(size_t i = 0; i < strlen(text); i++) {
		tb_change_cell(x + i, y, text[i], TB_DEFAULT, TB_DEFAULT);
	}

	return 0;
}


int tb_print_int(int x, int y, int i) {
	char buf[13] = {0};
	if(snprintf(buf, sizeof(buf), "%d", i) < 0) {
		return -1;
	}

	assert(x + strlen(buf) < tb_width() && "int would go out of terminal");
	assert(y < tb_height() && "int would go out of terminal");

	tuibb_print(x, y, buf);

	return 0;
}

void print_timeofday() {
	time_t currentTimeInt = time(NULL);
	assert(currentTimeInt != -1);
	struct tm currentTimeStruct;
	localtime_r(&currentTimeInt, &currentTimeStruct);

	char buf[3 + 3 + 3 + 1] = {0};
	strftime(buf, sizeof(buf), "%H:%M:%S", &currentTimeStruct);
	tuibb_print(0, 1, buf);
}

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	int ret = tb_init();
	if (ret) {
		printf("tb_init() failed: %d\n", ret);
		return -1;
	}
	tb_clear();

	int termboxFDs[2] = {0, 0};
	ret = tb_poll_fds(termboxFDs, 2);
	assert(ret == 2);
	assert(termboxFDs[0] != 0);
	assert(termboxFDs[1] != 0);

	print_timeofday();
	tb_present();
	sleep(2);

	tb_shutdown();
	return 0;
}
