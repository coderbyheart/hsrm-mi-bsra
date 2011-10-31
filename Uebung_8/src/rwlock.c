#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/dijkstra.h"

#define SEM_DB 0

#define SEM_NBR 0

#define SEM_MUTEX 0

void sem_init(){
	int sems[] = {SEM_MUTEX, SEM_DB, SEM_NBR};
	int length = sizeof(sems)/sizeof(int);
	if(PVinit(length, sems) != 0){
		printf("Fehler bei der Initialisierung der Semaphoren!\n");
		exit(1);
	}
}

void leser_lock(){
	while(Stand(SEM_MUTEX)) {
		sleep(1);
	}
	P(SEM_MUTEX);
	P(SEM_NBR);
	if(Stand(SEM_NBR) == 1) {
		P(SEM_DB);
	}
	V(SEM_MUTEX);
}

void leser_unlock(){
	P(SEM_MUTEX);
	if(Stand(SEM_NBR) > 1) {
		V(SEM_NBR);
	} else {
		fprintf(stderr, "Invalid semaphore count!\n");
	}
	if(!Stand(SEM_NBR)) {
		V(SEM_DB);
	}
	V(SEM_MUTEX);
}

void writer_lock(){
	while(Stand(SEM_DB)){
		sleep(1);
	}
	P(SEM_DB);
}

void writer_unlock(){
	V(SEM_DB);
}
