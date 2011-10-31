/* fischdb.c */

#define _BSD_SOURCE
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <libgen.h>
#include "fischdb.h"
#include "fisch-args.h"
#include "fisch-help.h"

/*
 * Wer länger unterwegs ist, kann seine Zier- und Nutzfische bei einem
 * niederbayerischen Fischverwahrungsunternehmen abgeben.
 *
 * Die Verwaltung der Gastfische soll nun durch ein neues ITSystem unterstützt werden.
 *
 * - Die Kunden zahlen für jeden Fisch einen Grundbetrag von 17 Euro.
 * - Für jeden Anruf, bei dem sie sich später nach dem Wohl ihrer Lieblinge erkundigen,
 *   wird allen Fischen des Anrufers ein ganzzahliger Euro-Betrag hinzugefügt
 *   (Höhe hängt davon ab, wie sehr die Nerven des Personals bei dieser Dienstleistung
 *   beansprucht wurden).
 * - Die zugehörige Datei „fischfile.dat“ besteht aus Datensätzen folgenden Aufbaus:
 * 	 (siehe fischfile.h)
 *
 * Das zu erstellende Verwaltungsprogramm fisch wird mit einem Kommandoparameter und
 * (je nach Kommando) evtl. weiteren Parametern aufgerufen, wie folgende Beispiele zeigen:
 *
 * - ./fisch -l
 *   gibt eine Liste mit dem gesamten Dateiinhalt tabellarisch aus.
 *
 * - ./fisch -l meier
 *   gibt eine Liste mit den Fischen des Besitzers „meier“ aus (Namen + Preise)
 *
 * - ./fisch -n schmidt glupschi
 *   fügt einen neuen Datensatz für den Fisch „glupschi“ des Besitzers „schmidt“ mit
 *   dem Standard-Anfangspreis 17 EUR (= Vollpension mit Poolbenutzung) am Ende der
 *   Datei hinzu.
 *
 * - ./fisch -z huber 22
 *   erhöht den Preis bei allen Fischen des Besitzers „huber“ um den Betrag 22
 *   Hinweis: Verwenden Sie die Funktion strtoul() zum Umwandeln eine Ganzzahl in
 *   Stringdarstellung in einen int-Wert
 *
 * -./fisch -d meier
 *   gibt den vom Besitzer „meier“ zu zahlenden Gesamtbetrag (Summe über alle seine Fische)
 *   aus und löscht alle Sätze von „meier“.
 *   Hinweis: Kopieren Sie die Ursprungsdatei in eine Zwischendatei um und lassen Sie dabei die
 *   zu löschenden Sätze weg. Benennen Sie dann diese Zwischendatei um. Sie können dazu die
 *   Systemfunktion „rename()“ (man 2 rename) verwenden.
 */

int main(int argc, char* argv[])
{
	fischdbfile = dirname(realpath(argv[0], NULL));
	strcat(fischdbfile, "/fisch.db");
	fischdb_open();
	fisch_args(argc, argv);
	fischdb_close();
	return 0;
}
