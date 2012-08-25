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

	printf(" + Loading rootkit signature database...\n");

	char* home = getenv("HOME");
	char databaseFile[PATH_MAX];
	if (home != NULL)
		snprintf(databaseFile, PATH_MAX, "%s/.vicious/rootkit.db", home);

	ViciousDB* rootkitDB = new ViciousDB(databaseFile);
	if (rootkitDB->GetRecordCount() == 0)
		return 0;
	else
		printf(" + [%ld records loaded]\n", rootkitDB->GetRecordCount());

	printf(" + Scanning %d files...\n", argc - 1);
	while (argc > 1) {
		int result = rootkitDB->ScanFile(argv[argc - 1]);

		if (result > 75)
			ERROR("%s (%d%% infection chance)\n", argv[argc - 1], result);
		else if (result > 45)
			WARNING("%s (%d%% infection chance)\n", argv[argc - 1], result);
//		else if (result >= 0)
//			CLEAN("%s (%d%% infection chance)\n", argv[argc - 1], result);
		// < 0 is error (which is displayed)

		argc--;
	}
	return 0;
}
