#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "third_party/termbox/src/termbox.h"

int tb_print_int(int x, int y, int i) {
	char buf[13] = {0};
	if(snprintf(buf, sizeof(buf), "%d", i) < 0) {
		return -1;
	}

	assert(x + strlen(buf) < tb_width() && "int would go out of terminal");
	assert(y < tb_height() && "int would go out of terminal");

	for(size_t j = 0; j < strlen(buf); j++) {
		tb_change_cell(x + j, y, buf[j], TB_DEFAULT, TB_DEFAULT);
	}

	return 0;
}

int main(int argc, char** argv) {
	(void)argc; (void)argv;

	int ret = tb_init();
	if (ret) {
		printf("tb_init() failed: %d\n", ret);
		return -1;
	}
	tb_clear();

	tb_print_int(0, 0, tb_width());
	tb_print_int(0, 1, tb_height());
	tb_present();
	sleep(2);

	tb_shutdown();
	return 0;
}
