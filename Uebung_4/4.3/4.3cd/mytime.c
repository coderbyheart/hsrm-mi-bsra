#include "include/mytime.h"
#include <stdio.h>
int main(){
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