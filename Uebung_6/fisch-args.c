/* fisch-args.c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "fisch-help.h"
#include "fischdb.h"

/**
 * Parst die übergebenen Argumente
 *
 * getopt unterstützt leider die geforderte Argument-Struktur nicht,
 * z.B. funktioniert -l meier nicht, wenn l als "l::" deklariert ist.
 * Das geht nur wenn -lmeier übergeben wird.
 */
void fisch_args(int argc, char *argv[]) {

	char *param = argv[1];
	char c = param[1];

	if (argc < 1 || !c) {
		fisch_help();
		exit(1);
	}

	switch (c) {
	case 'l':
		if (argc < 3) {
			fischdb_list_all();
		} else if (argc > 3) {
			fisch_help();
			exit(1);
		} else {
			fischdb_list_person(argv[2]);
		}
		break;
	case 'n':
		if (argc < 4) {
			fisch_help();
			exit(1);
		} else if (argc > 4) {
			fisch_help();
			exit(1);
		} else {
			fischdb_add(argv[2], argv[3]);
		}
		break;
	case 'z':
		if (argc < 4) {
			fisch_help();
			exit(1);
		} else if (argc > 4) {
			fisch_help();
			exit(1);
		} else {
			int inc = strtoul(argv[3], NULL, 10);
			if (inc <= 0) {
				fisch_help();
				exit(1);
			}
			fischdb_set_price(argv[2], inc);
		}
		break;
	case 'd':
		if (argc < 3) {
			fisch_help();
			exit(1);
		} else if (argc > 3) {
			fisch_help();
			exit(1);
		} else {
			fischdb_checkout(argv[2]);
		}
		break;
	case 'h':
	case '?':
	default:
		fisch_help();
		exit(0);
	}
}
