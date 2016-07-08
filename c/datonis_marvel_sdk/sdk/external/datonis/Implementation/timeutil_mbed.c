#include "timeutil.h"
#include <wmtime.h>
#include <wm_os.h>
#include <wmstdio.h>

unsigned long long get_time_ms() {
	unsigned long long time = wmtime_time_get_posix();
	time = time * 1000;
	time = time + os_ticks_to_msec(os_ticks_get());
	//wmprintf("Current time: %lld\n\r", time);
	return time;
}

void sleep_seconds(unsigned int seconds) {
    os_thread_sleep(os_msec_to_ticks(seconds));
}

