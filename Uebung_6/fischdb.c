/* fischdb.c */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <sys/param.h>
#include "fischdb.h"
#include "fisch-args.h"

static int debug = 0;
static Fischsatz currentFisch;

/* Öffnet das DB-File */
void fischdb_open() {
	if ((fischdbfp = open(fischdbfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
		perror("fisch_open_db");
		exit(1);
	}
	if (debug) printf("DB:   %s\n", fischdbfile);
	fischdb_check_header();
}

/* Schließt das DB-File */
void fischdb_close() {
	fischdb_write_header();
	close(fischdbfp);
}


/* Checkt den Header der Datenbank-Datei und fügt diesen ggfs. ein */
void fischdb_check_header()
{
	int readBytes;
	lseek(fischdbfp, 0, SEEK_SET);
	readBytes = read(fischdbfp, &fischdbheader, sizeof (fischdb_header));
	if (readBytes < 0) {
		perror("fischdb_check_header");
		exit(1);
	}
	/* Header schreiben */
	if (readBytes == 0) {
		fischdbheader.num_entries = 0;
		fischdb_write_header();
	}
	if (debug) printf("Datenbank enthält %d Einträge.\n", fischdbheader.num_entries);
}

/* Schreibt den Header in die Datenbank */
void fischdb_write_header()
{
	lseek(fischdbfp, 0, SEEK_SET);
	if(write(fischdbfp, &fischdbheader, sizeof (fischdb_header)) < 0) {
		perror("fischdb_add");
		exit(1);
	}
}

/* Tabellen-Trenner ausgeben */
void fischdb_table_sep()
{
	printf("+---+--------------------+--------------------+------+\n");
}
/* Tabellen-Header ausgeben */
void fischdb_table_header()
{
	fischdb_table_sep();
	printf("|  #|            Besitzer|               Fisch| Preis|\n");
	fischdb_table_sep();
}

/* Tabellen-Zeile ausgeben */
void fischdb_table_row(int num, char *person, char *fisch, int preis)
{
	printf("|%3d|%20s|%20s|%6d|\n", num, person, fisch, preis);
}

/*  Listet alle Fische in der Datenbank. */
void fischdb_list_all()
{
	int num = 0;
	if (debug) printf("Liste aller Fische: %d Einträge.\n", fischdbheader.num_entries);
	if (fischdbheader.num_entries <= 0) return;
	fischdb_seek_afterheader();

	while((read(fischdbfp, &currentFisch, sizeof(Fischsatz))) > 0) {
		num++;
		if (num == 1) fischdb_table_header();
		fischdb_table_row(num, currentFisch.besitzer, currentFisch.fischname, currentFisch.preis);
	}
	if (num > 0) fischdb_table_sep();
}

/* Gibt eine Liste mit den Fischen des Besitzers person aus. */
void fischdb_list_person(char *person)
{
	int num = 0;
	if (debug) printf("Liste der Fische von %s\n", person);
	if (fischdbheader.num_entries <= 0) return;
	fischdb_seek_afterheader();
	while((read(fischdbfp, &currentFisch, sizeof(Fischsatz))) > 0) {
		if (strcmp(currentFisch.besitzer, person) == 0) {
			num++;
			if (num == 1) fischdb_table_header();
			fischdb_table_row(num, currentFisch.besitzer, currentFisch.fischname, currentFisch.preis);
		}
	}
	if (num > 0) {
		fischdb_table_sep();
	} else {
		printf("%s hat keine Fische!\n", person);
	}

}

/* Trägt den Fisch fisch für den Besitzer person ein. */
void fischdb_add(char *person, char *fisch)
{
	if (debug) printf("Füge Fisch %s von %s hinzu.\n", fisch, person);
	Fischsatz newFish;
	strncpy(newFish.besitzer, person, FISCH_MAX_NAME_LEN);
	strncpy(newFish.fischname, fisch, FISCH_MAX_NAME_LEN);
	newFish.preis = FISCH_DEFAULT_PRICE;
	lseek(fischdbfp, 0, SEEK_END);
	if (write(fischdbfp, &newFish, sizeof (Fischsatz)) < 0) {
		perror("fischdb_add");
		exit(1);
	}
	fischdbheader.num_entries++;
}

/* Erhöht den Preis bei allen Fischen des Besitzers person um den Betrag inc */
int fischdb_set_price(char *person, int inc)
{
	int updated = 0;
	int entry = 0;
	if (debug) printf("Erhöhe die Preise aller Fischen von %s um %d.\n", person, inc);
	fischdb_seek_afterheader();
	while((read(fischdbfp, &currentFisch, sizeof(Fischsatz))) > 0) {
		entry++;
		if (strcmp(currentFisch.besitzer, person) == 0) {
			currentFisch.preis += inc;
			fischdb_seek_entry(entry);
			if (write(fischdbfp, &currentFisch, sizeof (Fischsatz)) < 0) {
				perror("fischdb_set_price");
				exit(1);
			}
			updated++;
		}
	}
	if (debug) printf("%d Datensätze aktualisiert.\n", updated);
	return updated;
}

/* Setzt den Filepointer nach den Header-Datensatz */
void fischdb_seek_afterheader()
{
	lseek(fischdbfp, sizeof(fischdb_header), SEEK_SET);
}

/* Setzt den Filepointer an den Anfang eines Datensatzes */
void fischdb_seek_entry(int entry)
{
	lseek(fischdbfp, sizeof(fischdb_header) + ((entry - 1) * sizeof(Fischsatz)), SEEK_SET);
}

/* Gibt den vom Besitzer person zu zahlenden Gesamtbetrag aus und löscht alle Fische von person. Gibt den Betrag zurück. */
int fischdb_checkout(char *person)
{
	int checkoutSum = 0;
	int entry = 0;
	int numFishes = 0;
	fischdb_seek_afterheader();
	while((read(fischdbfp, &currentFisch, sizeof(Fischsatz))) > 0) {
		entry++;
		if (strcmp(currentFisch.besitzer, person) == 0) {
			numFishes++;
			checkoutSum += currentFisch.preis;
		}
	}
	if (numFishes > 0) {
		fischdb_remove(person);
		printf("Gesamtsumme für %d Fische von %s: %d\n", numFishes, person, checkoutSum);
	} else {
		printf("%s hat keine Fische.\n", person);
	}
	return checkoutSum;
}

/* Entfernt die Einträge von person durch anlegen einer Kopie der Datenbank */
void fischdb_remove(char *person)
{
	int entries = 0;

	/* Temp. Datei erzeugen */
	int fischdbtmpfp;
	char fischdbtmpfile[PATH_MAX];
	strcpy(fischdbtmpfile, fischdbfile);
	dirname(fischdbtmpfile);
	strcat(fischdbtmpfile, "/fisch.db.tmp");
	if ((fischdbtmpfp = open(fischdbtmpfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
		perror("fischdb_remove: open temp");
		exit(1);
	}

	/* Aktuelle DB durchgehen und Einträge anderer Personen in neue Datei kopieren */
	fischdb_seek_afterheader();
	lseek(fischdbtmpfp, sizeof(fischdb_header), SEEK_SET);
	while((read(fischdbfp, &currentFisch, sizeof(Fischsatz))) > 0) {
		if (strcmp(currentFisch.besitzer, person) != 0) {
			entries++;
			if (write(fischdbtmpfp, &currentFisch, sizeof (Fischsatz)) < 0) {
				perror("fischdb_remove: write entry");
				exit(1);
			}
		}
	}

	/* Neuen Hader schreiben */
	lseek(fischdbtmpfp, 0, SEEK_SET);
	fischdb_header fischdbtmpheader;
	fischdbtmpheader.num_entries = entries;
	if(write(fischdbtmpfp, &fischdbtmpheader, sizeof (fischdb_header)) < 0) {
		perror("fischdb_remove: write header");
		exit(1);
	}

	/* Dateien umbenennen */
	close(fischdbtmpfp);
	close(fischdbfp);
	if (debug) printf("%s -> %s\n", fischdbtmpfile, fischdbfile);
	if (rename(fischdbtmpfile, fischdbfile) < 0) {
		perror("fischdb_remove: replace db");
		exit(1);
	}
	fischdb_open();
}
