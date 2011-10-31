/**
 * @file	fisch.c
 * @brief	Fischverwaltungsprogramm
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include "../include/fischdef.h"
#include "../include/rwlock.h"


/**
 * @brief Debug support macros
 *
 * Output messages if @c verbose is higher than a given level.
 *
 * NOTE: Usage @c debug((<printf-params>)); i.e. use @e two brackets
 *       to enclose the printf parameter list!
 */
#define debug1(x) do{if(verbose >= 1){ printf x ;fflush(stdout);}}while(0)
#define debug2(x) do{if(verbose >= 2){ printf x ;fflush(stdout);}}while(0)
#define debug3(x) do{if(verbose >= 3){ printf x ;fflush(stdout);}}while(0)
#define debug4(x) do{if(verbose >= 4){ printf x ;fflush(stdout);}}while(0)

#ifdef DEBUG
static void debug_wait(void){
  printf("Press Enter! ");
  fflush(stdout);
  getchar();
}
#else

static void debug_wait(void){};

#endif


/**
 * @brief Kurze Hilfe ausgeben
 */
static void rtfm(char* argv[])
{
	printf("Verwendung: %s {<Option>} <param1> {<param2>}\n", argv[0]);
	printf("Funktion: Fischverwaltung\n");
	printf("Optionen:\n");
	printf("     -l {<besitzer>}       - Gib den Fischbestand aus\n");
	printf("     -n <besitzer> <fisch> - neuen Datensatz hinzufuegen\n");
	printf("     -z <besitzer> <Euro>  - Zuschlag für Besitzer\n");
	printf("     -d <besitzer>         - die Rechnung bitte!\n");
}

/**
 * @brief Namen auf zulaessige Laenge pruefen und ggf. kuerzen
 */
static void evtl_namen_kuerzen(char *name)
{
	if(strlen(name) >= MAX_NAME)
	{
		printf("Warnung: Name \"%s\" zu lang, ", name);
		name[MAX_NAME-1] = '\0';
		printf("gekuerzt zu: \"%s\"\n", name);
	}
}

/**
 * @brief Eine Liste des Fischbestandes ausgeben.
 */
static void liste(char *besitzer)
{
	int fd, ret;
	int i = 0;
	int gesamtpreis = 0;
	Fischsatz fisch;

	if(besitzer)
		evtl_namen_kuerzen(besitzer);
	leser_lock();
	debug_wait();
	fd = open(DATEI, O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		leser_unlock();
		exit(1);
	}
	while((ret = read(fd, &fisch, sizeof(fisch))) == sizeof(fisch))
	{
		if(besitzer)
		{
			if(!strcmp(fisch.besitzer, besitzer))
			{
				if(++i == 1)
				{
					printf("Fische von Besitzer %s:\n", besitzer);
					printf("Name  Preis\n");
				}
				printf("%s: %d Eur\n", fisch.fischname, fisch.preis);
				gesamtpreis += fisch.preis;
			}
		}
		else
		{
			if(++i == 1)
			{
				printf("Liste aller Fische:\n");
				printf("Besitzer Name Preis\n");
			}
			printf("%s   %s: %d Eur\n", fisch.besitzer, fisch.fischname, fisch.preis);
		}
	}
	if(ret < 0)
	{
		perror("read");
		leser_unlock();
		exit(1);
	}
	else if(ret > 0)
	{
		fprintf(stderr, "Warnung: Datei \"%s\" korrumpiert\n", DATEI);
	}
	if(i == 0)
	{
		if(besitzer)
			printf("%s hat keine Fische bei uns\n", besitzer);
		else
			printf("Keine Fische vorhanden\n");
	}
	else
	{
		printf("Insgesamt: %d Fische", i);
		if(gesamtpreis)
			printf(", Summe: %d Euro\n", gesamtpreis);
		else
			printf("\n");
	}
	close(fd);
	leser_unlock();
}

/**
 * @brief Einen neuen Fisch in die Datenbank eintragen
 */
static void neu(char *besitzer, char *fischname)
{
	int fd;
	int ret;
	Fischsatz fisch;

	evtl_namen_kuerzen(besitzer);
	evtl_namen_kuerzen(fischname);
	
	writer_lock();
	debug_wait();
	fd = open(DATEI, O_RDWR|O_CREAT, 0600);
	if(fd < 0)
	{
		perror("open");
		writer_unlock();
		exit(1);
	}
	while((ret = read(fd, &fisch, sizeof(fisch))) == sizeof(fisch))
	{
		if(!strcmp(fisch.besitzer, besitzer) && 
		   !strcmp(fisch.fischname, fischname))
		{
			printf("Kann Besitzer \"%s\" / Fisch \"%s\" nicht eintragen: Eintrag existiert bereits!\n", besitzer, fischname);
			writer_unlock();
			exit(1);
		}
	}
	if(ret < 0)
	{
		perror("read");
		writer_unlock();
		exit(1);
	}
	else if(ret > 0)
	{
		fprintf(stderr, "Warnung: Datei \"%s\" korrumpiert\n", DATEI);
		lseek(fd, -ret, SEEK_END);
	}
	strcpy(fisch.besitzer, besitzer);
	strcpy(fisch.fischname, fischname);
	fisch.preis = GRUNDPREIS;
	if((ret = write(fd, &fisch, sizeof(fisch))) != sizeof(fisch))
	{
		if(ret < 0)
			perror("write");
		else
			fprintf(stderr, "?!? Konnte nur %d von %d bytes schreiben\n", ret, sizeof(fisch));
	}
	close(fd);
	writer_unlock();
}

/**
 * @brief Zuschlag fuer alle Fische eines Besitzers
 */
static void zuschlag(char *besitzer, int zuschlag)
{
	int fd, ret, gefunden = 0;
	Fischsatz fisch;

	evtl_namen_kuerzen(besitzer);
	writer_lock();
	debug_wait();
	fd = open(DATEI, O_RDWR);
	if(fd < 0)
	{
		perror("open");
		writer_unlock();
		exit(1);
	}
	while((ret = read(fd, &fisch, sizeof(fisch))) == sizeof(fisch))
	{
		if(!strcmp(fisch.besitzer, besitzer))
		{
			fisch.preis += zuschlag;
			if(lseek(fd, -sizeof(fisch), SEEK_CUR) ==(off_t)-1)
			{
				perror("lseek");
				writer_unlock();
				exit(1);
			}
			if(write(fd, &fisch, sizeof(fisch)) != sizeof(fisch))
			{
				perror("write");
				writer_unlock();
				exit(1);
			}
			gefunden = 1;
		}
	}
	if(ret < 0)
	{
		perror("read");
		writer_unlock();
		exit(1);
	}
	else if(ret > 0)
	{
		fprintf(stderr, "Warnung: Datei \"%s\" korrumpiert\n", DATEI);
	}
	if(!gefunden)
	{
		printf("Achtung: Kein Kunde namens \"%s\" in der Datei \"%s\"\n", besitzer, DATEI);
	}
	close(fd);
	writer_unlock();
}

/**
 * @brief Rechnung fuer Besitzer ausgeben, seine Eintraege loeschen
 */
static void rechnung(char *besitzer)
{
	int fd, fdz, ret, gefunden = 0;
	Fischsatz fisch;
	int gesamtpreis = 0;
	writer_lock();
	debug_wait();
	if(rename(DATEI, ZWISCHENDATEI) < 0)
	{
		perror("rename");
		writer_unlock();
		exit(1);
	}
	evtl_namen_kuerzen(besitzer);

	fdz = open(ZWISCHENDATEI, O_RDONLY);
	if(fdz < 0)
	{
		perror("open");
		writer_unlock();
		exit(1);
	}
	fd = open(DATEI, O_RDWR|O_CREAT, 0600);
	if(fd < 0)
	{
		perror("open");
		writer_unlock();
		exit(1);
	}
	while((ret = read(fdz, &fisch, sizeof(fisch))) == sizeof(fisch))
	{
		if(!strcmp(fisch.besitzer, besitzer))
		{
			gesamtpreis += fisch.preis;
			gefunden = 1;
		}
		else
		{
			if(write(fd, &fisch, sizeof(fisch)) != sizeof(fisch))
			{
				perror("write");
				writer_unlock();
				exit(1);
			}
		}
	}
	if(ret < 0)
	{
		perror("read");
		writer_unlock();
		exit(1);
	}
	else if(ret > 0)
	{
		fprintf(stderr, "Warnung: Datei \"%s\" korrumpiert\n", DATEI);
	}
	if(gefunden)
	{
		printf("Rechnungsbetrag fuer %s: %d Eur\n", besitzer, gesamtpreis);
	}
	else
	{
		printf("Achtung: Kein Kunde namens \"%s\" in der Datei \"%s\"\n", besitzer, DATEI);
	}
	close(fdz);
	close(fd);
	writer_unlock();
}

/**
 * @brief Hauptprogramm
 */
int main(int argc, char* argv[])
{
	char *end;
	int z;
	sem_init();
	if(argc > 1)
	{
		if(!strcmp("-l", argv[1]))
		{
			if(argc == 3)
				liste(argv[2]);
			else if(argc == 2)
				liste(NULL);
			else
				rtfm(argv);
		}
		else if(!strcmp("-n", argv[1]))
		{
			if(argc == 4)
				neu(argv[2], argv[3]);
			else
				rtfm(argv);
		}
		else if(!strcmp("-z", argv[1]))
		{
			if(argc == 4)
			{
				z = strtoul(argv[3], &end, 0);
				if(argv[3] == end)
				{
					fprintf(stderr, "%s: Keine Zahl: %s\n", argv[0], argv[3]);
					exit(1);
				}
				zuschlag(argv[2], z);
			}
			else
				rtfm(argv);
		}
		else if(!strcmp("-d", argv[1]))
		{
			if(argc == 3)
				rechnung(argv[2]);
			else
				rtfm(argv);
		}
		else
			rtfm(argv);
	}
	else
	{
		rtfm(argv);
	}
	exit(0);
}
