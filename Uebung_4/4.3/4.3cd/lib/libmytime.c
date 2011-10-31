#include <sys/time.h>
#include <stdio.h>
#include "../include/mytime.h"

/*struct tstamp {
	struct timeval start, stop;
};*/

void start(struct tstamp *t) {
	gettimeofday(&t->start, NULL);
}

void stop(struct tstamp *t) {
	gettimeofday(&t->stop, NULL);
}

unsigned long extime(struct tstamp *t) {
	long msec;
	msec = (t->stop.tv_sec - t->start.tv_sec) * 1000;
	msec += (t->stop.tv_usec - t->start.tv_usec);
	return msec;
}
