#include <sys/time.h>
#include <stdio.h>

struct tstamp {
	struct timeval start, stop;
};

void start(struct tstamp *t) {
	gettimeofday(&t->start, NULL);
}

void stop(struct tstamp *t) {
	gettimeofday(&t->stop, NULL);
}

unsigned long extime(struct tstamp *t) {
	long msec;
	msec = (t->stop.tv_sec - t->start.tv_sec) * 1000;
	msec += (t->stop.tv_usec - t->start.tv_usec);/* / 1000;*/
	return msec;
}

int main() {
	int i = 0;
	struct tstamp t;
	
	start(&t);
	for (i = 0; i < 100; i++) {
		printf("%d\n", i+1);
	}
	stop(&t);
	printf("Gesamt: %lums\n", extime(&t));
	
	return(0);
}