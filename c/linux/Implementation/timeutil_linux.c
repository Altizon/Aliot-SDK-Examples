#include "timeutil.h"
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL );

    double time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    return time_in_mill;
}

void sleep_seconds(unsigned int seconds) {
    sleep(seconds);
}

