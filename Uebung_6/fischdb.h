/* fischdb.h */
#ifndef FISCHDB_H
#define FISCHDB_H
#define FISCH_DEFAULT_PRICE 17
#define FISCH_MAX_NAME_LEN 20
/* Die Datenbank besteht aus Datensätzen folgenden Aufbaus */
typedef struct fis {
	char besitzer[20];
	char fischname[20];
	int preis;
} Fischsatz;
/* Header mit allgemeinen Informationen */
typedef struct _fischdb_header {
	int num_entries;
} fischdb_header;
/* Dateipfad der Datenbank */
char *fischdbfile;
/* Header der Datenbank */
fischdb_header fischdbheader;
/* File-Pointer des Db-Files */
int fischdbfp;
/* Öffnet das DB-File */
void fischdb_open();
/* Schließt das DB-File */
void fischdb_close();
/* Checkt den Header der Datenbank-Datei und fügt diesen ggfs. ein */
void fischdb_check_header();
/*  Listet alle Fische in der Datenbank. */
void fischdb_list_all();
/* Gibt eine Liste mit den Fischen des Besitzers person aus. */
void fischdb_list_person(char *person);
/* Trägt den Fisch fisch für den Besitzer person ein. */
void fischdb_add(char *person, char *fisch);
/* Schreibt den Header in die Datenbank */
void fischdb_write_header();
/* Setzt den Filepointer nach den Header-Datensatz */
void fischdb_seek_afterheader();
/* Setzt den Filepointer an den Anfang eines Datensatzes */
void fischdb_seek_entry(int entry);
/* Erhöht den Preis bei allen Fischen des Besitzers person um den Betrag inc
 * Gibt die Anzahl der veränderten Einträge zurück */
int fischdb_set_price(char *person, int inc);
/* Tabellen-Trenner ausgeben */
void fischdb_table_sep();
/* Tabellen-Header ausgeben */
void fischdb_table_header();
/* Tabellen-Zeile ausgeben */
void fischdb_table_row();
/* Gibt den vom Besitzer person zu zahlenden Gesamtbetrag aus und löscht alle Fische von person. Gibt den Betrag zurück. */
int fischdb_checkout(char *person);
/* Entfernt die Einträge von person durch anlegen einer Kopie der Datenbank */
void fischdb_remove(char *person);
#endif
