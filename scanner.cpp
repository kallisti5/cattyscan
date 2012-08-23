/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "ViciousDB.h"


int
main(int argc, char* argv[])
{
	printf("Vicious Scanner\n");
	if (argc < 2) {
		printf("  Usage: %s file file ...\n", argv[0]);
		return 1;
	}

	printf(" + Loading rootkit signature database...\n");
	ViciousDB* rootkitDB = new ViciousDB((char*)"db/rootkits.db");
	if (rootkitDB->GetRecordCount() == 0)
		return 0;
	else
		printf(" + [%ld records loaded]\n", rootkitDB->GetRecordCount());

	printf(" + Scanning %d files...\n", argc - 1);
	while (argc > 1) {
		rootkitDB->ScanFile(argv[argc - 1]);
		argc--;
	}
	return 0;
}
