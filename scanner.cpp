/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "ViciousDB.h"


#ifdef DEBUG
#define TRACE(x...) printf(" D " x)
#else
#define TRACE(x...)
#endif
#define ERROR(x...) printf(" \033[31m! Error:\033[0m " x)
#define WARNING(x...) printf(" \033[33m! Warning:\033[0m " x)
#define CLEAN(x...) printf(" \033[37m+ Scanned:\033[0m " x)

 
int
main(int argc, char* argv[])
{
	printf("Vicious Scanner\n");
	if (argc < 2) {
		printf("  Usage: %s file file ...\n", argv[0]);
		return 1;
	}

	printf(" + Loading vicious software database...\n");

	char* home = getenv("HOME");
	char databaseFile[PATH_MAX];
	if (home != NULL)
		snprintf(databaseFile, PATH_MAX, "%s/.vicious/vicious.db", home);

	ViciousDB* database = new ViciousDB(databaseFile);
	if (database->GetRecordCount() == 0) {
		delete database;
		return 0;
	} else
		printf(" + [%ld records loaded]\n", database->GetRecordCount());

	printf(" + Scanning %d files...\n", argc - 1);
	while (argc > 1) {
		database->ScanFile(argv[argc - 1]);
		argc--;
	}
	delete database;
	return 0;
}
