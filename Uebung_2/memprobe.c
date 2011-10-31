#include <stdio.h>
#include <unistd.h>
#include "include/memprobe.h"

int main(void) {

	/* Größe einer Speicherseite */
	int pagesize = getpagesize();

	/* Pointer auf eine Speicheradresse initalisieren, angefangen bei 0 */
	void *c = 0;

	/* Die größe des Adressraumes ermitteln, bei 64 Bit dann nur 4GB abgrasen */
	int addrsize = sizeof(void *) * 8;
	if (addrsize > 32) {
		printf("Warning! Testing only up to 4 GB.");
	}
	
	/* Die Anzahl der zu testenden Speicherseiten */
	int seitenzahl = (1ULL<<32) / pagesize;

	printf("Pagesize is:     %d\n", getpagesize());
	printf("Number of pages: %d\n", seitenzahl);

	/* Hilfsvariablen für die Schleife */
	void *lastc = 0;
	int i = 0;
	int s;
	int lasts = -1;
	char stati[3][10] = { "No access", "Read", "Write" };
	do {
		/* Pointer auf aktuelle Page-Adresse setzen */
		c = i * pagesize;
		s = memprobe(c);
		if (s != lasts) {
			if (lasts == -1) {
				/* erstes ergebnis -> ignorieren */
			} else {
				printf("%10s: 0x%08x - 0x%08x\n", stati[lasts], lastc, c);
				lastc = c;
			}
			lasts = s;
		}
		i++;
	} while(i <= seitenzahl);
			printf("%10s: 0x%08x - 0x%08x\n", stati[s], lastc, c-1);
	return 0;
}
