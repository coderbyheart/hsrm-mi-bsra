#include <stdio.h>
#include "include/punkt.h"
#include "include/strich.h"
#include "include/rest_op.h"

#define TRUE  1
#define FALSE 0

/* ------------------------------------ */
/* Hilfsoperationen                     */
/* ------------------------------------ */
void getarg (int * i)
{
	int val;
	printf("bitte einen int-Parameter eingeben! \n");
	scanf("%d", &val);
	*i = val;
}

void get2args (int * i, int * j)
{
	int val1, val2;
	printf("bitte zwei int-Parameter eingeben! \n");
	scanf("%d %d", &val1, &val2);
	*i = val1;
	*j = val2;
}

void puterg0 (char * s, int res)
{
	printf("%s () = %d \n\n", s, res);
}

void puterg1 (char * s, int p1, int res)
{
	printf("%s ( %d ) = %d \n\n", s, p1, res);
}

void puterg2 (char * s, int p1, int p2, int res)
{
	printf("%s ( %d, %d ) = %d \n\n", s, p1, p2, res);
}

/* ------------------------------------ */
/* Hauptprogramm                        */
/* ------------------------------------ */

int 
main( )
{
	int a, b, erg;
	int sel_op;
	int ende = FALSE;

	do
	{
		printf("Gewuenschte Operation?   \n");
		printf("   1: add    2: sub      \n");
		printf("   3: mult   4: div      \n");
		printf("   5: rest   6: rest_add \n");
		printf("   7: basis              \n");
		printf("  99: Programmende       \n\n");

		scanf("%d", &sel_op);	
		switch (sel_op)
		{
			case 1:
				get2args( &a, &b );
				erg = add ( a, b );
				puterg2("add", a, b, erg);
				break;
			case 2:
				get2args( &a, &b );
				erg = sub( a, b );
				puterg2("sub", a, b, erg);
				break;
			case 3:
				get2args( &a, &b );
				erg = mult( a, b );
				puterg2("mult", a, b, erg);
				break;
			case 4:
				get2args( &a, &b );
				erg = div( a, b );
				puterg2("div", a, b, erg);
				break;
			case 5:
				getarg( &a );
				erg = rest( a );
				puterg1("rest", a, erg);
				break;
			case 6:
				get2args( &a, &b );
				erg = rest_add( a, b );
				puterg2("rest_add", a, b, erg);
				break;
			case 7:
				erg = basis( );
				puterg0("basis", erg);
				break;
			case 99:
				ende = TRUE;
				break;
			default:
				break;
		}
	} while ( !ende );

	return 0;
}
