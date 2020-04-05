# API
```c
	tuibb_init();

	int id1 = tuibb_add_select(...);
	int id2 = tuibb_add_text(...);
	tuibb_show();

	int tfd = timerfd_create(..);

	while(epoll(..)) {
		if(tuibb_is_event(fd) && tuibb_get_changed_item(fd) == id1) {
		  puts(tuibb_get_option(id1)); 
		} else if(fd == tfd) {
		  tuibb_set_text(id2, timestamp());
		}
	}
```
