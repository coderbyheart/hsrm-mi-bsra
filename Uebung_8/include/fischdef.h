/**
 * @file	fischdef.h
 * @brief	Definitionen fuer das Fisch-Programm
 *
 */

#ifndef __FISCHDEF_H__
#define __FISCHDEF_H__


#define MAX_NAME	20	/**< Maximal zul�ssige Namensl�nge */
#define GRUNDPREIS	17	/**< Grundpreis fuer Vollpension mit Poolbenutzung */
#define DATEI		"fischfile.dat"
#define ZWISCHENDATEI	"fischfile.bak"

/**
 * @brief Struktur eines Eintrages in der Datenbank
 */
 
typedef struct fis
{
	char besitzer[MAX_NAME];		/**< Name des Besitzers */
	char fischname[MAX_NAME];		/**< Name des Fisches   */
	int  preis;						/**< (bisher) zu zahlender Betrag */
} Fischsatz;


#endif /* __FISCHDEF_H__ */
