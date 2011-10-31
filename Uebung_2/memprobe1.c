#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "include/memprobe.h"

int main(void);

int datatest=42; 

void memAreaCheck(void* c, void* lastc){

  int x=42;
  /*Testobjekte zum vergleichen deklarieren bzw. allozieren*/
	/*Heap*/
	int * heaptest = malloc (100);
	/*Daten - siehe oben, globale Variable datatest */
	/*Stack, lokale Variable in main*/
	int * stackzeiger = &x;
	/*Code Funktionzeiger*/
	int (*fp)(void) = main;

	/*Zeichenketten*/
	static char *string1 = "Hallo Welt\n";
	static char string2[]= "Hallo Welt\n";
	
	if (&datatest < (int*)c && &datatest > (int *)lastc){
	  printf("Data");
	}
	if (heaptest<(int *)c && heaptest>(int *)lastc){
	  printf("Heap");
	}
	if (stackzeiger<(int *)c && stackzeiger>(int *)lastc){			    
	  printf("Stack");
	}
	if(fp < c && fp > lastc){
	  printf("Code");
	}
	if (string1 < c && string1 > lastc){
	  printf(" String1 liegt hier");
	}
	if (string2 < c && string2 > lastc){
	  printf(" String2 liegt hier");
	}

	printf("\n");

	free(heaptest);
	
}

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
			  printf("%10s: 0x%08x - 0x%08x ", stati[lasts], lastc, c);
			  memAreaCheck(c,lastc);
			  lastc = c;
			}
			lasts = s;
		}
		i++;
	} while(i <= seitenzahl);
	
	printf("%10s: 0x%08x - 0x%08x", stati[s], lastc, c-1);
	memAreaCheck(c,lastc);
	

	return 0;
}
