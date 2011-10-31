#include <sys/time.h>
#ifndef __LIBMYTIME_H__
#define __LIBMYTIME_H__
struct tstamp {
	struct timeval start, stop;
};

extern void start(struct tstamp *t);
extern void stop(struct tstamp *t);
extern unsigned long extime(struct tstamp *t);

#endif