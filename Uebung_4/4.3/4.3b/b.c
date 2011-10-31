#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

static int verbose = 0;
static int dosleep = 0;
static int runs = 10000;

void doTimeDiff()
{
	int i;
	struct timeval t;
	struct timeval p;
	double max = 0;
	double min = 0;
	double d;
	for(i = 0; i < runs; i++) {
		gettimeofday(&t, NULL);
		if (i > 1) {
			// Differenz in Sekunden
			d = difftime(t.tv_sec, p.tv_sec) * 1000000;
			// Differenz in Microsekunden
			if (p.tv_usec > t.tv_usec) {
				d += (1000000 - p.tv_usec) + t.tv_usec;
			} else {
				d += t.tv_usec - p.tv_usec;
			}
			if (i == 0 || (d > 0 && min > d)) min = d;
			if (d > max) max = d;
			if(verbose) printf("Diff: %.0f ms\n", d);
			if(dosleep) sleep(1);	
		}
		p = t;
	}
	printf("Anzahl Durchläufe:  %d\n", i);
	printf("Minimale Differenz: %.0f ms\n", min);
	printf("Maximale Differenz: %.0f ms\n", max);
}

int main(int argc, char *argv[])
{
	doTimeDiff();
	return 0;
}

