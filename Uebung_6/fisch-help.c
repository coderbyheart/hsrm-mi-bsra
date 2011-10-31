/* fisch-help.c */

#include "stdio.h"

void fisch_help()
{
	printf("\n");
	printf("Verwaltet die Datenbank eines Fischverwahrungsunternehmens.\n");
	printf("\n");
	printf("fisch akzeptiert folgende Parameter:\n");
	printf("\n");
	printf("  ./fisch -l\n");
	printf("    Listet alle Fische in der Datenbank.\n");
	printf("\n");
	printf("  ./fisch -l meier\n");
	printf("    Gibt eine Liste mit den Fischen des Besitzers „meier“ aus.\n");
	printf("\n");
	printf("  ./fisch -n schmidt glupschi\n");
	printf("    Trägt den Fisch „glupschi“ für den Besitzer „schmidt“ ein.\n");
	printf("\n");
	printf("  ./fisch -z huber 22\n");
	printf("    Erhöht den Preis bei allen Fischen des Besitzers „huber“ um den Betrag 22\n");
	printf("\n");
	printf("  ./fisch -d meier\n");
	printf("    Gibt den vom Besitzer „meier“ zu zahlenden Gesamtbetrag aus und löscht alle Fische von „meier“.\n");
	printf("\n");
}
