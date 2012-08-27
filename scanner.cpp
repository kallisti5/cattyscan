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

#include "CattyDB.h"


#ifdef DEBUG
#define TRACE(x...) printf(" D " x)
#else
#define TRACE(x...)
#endif
#define ERROR(x...) printf(" \033[31m! Error:\033[0m " x)
#define WARNING(x...) printf(" \033[33m! Warning:\033[0m " x)
#define CLEAN(x...) printf(" \033[37m+ Scanned:\033[0m " x)


void
display_match(record* db, char* filename)
{
	printf("   \033[33mA potentially vicious file was detected within the "
		"search range!\033[0m\n");
	printf("   ================================================================\n");
	printf("     * File:\n");
	printf("       %s\n\n", filename);
	printf("     * Scan type: ");
	switch (db->type) {
		case RECORD_SIGNATURE:
			printf("SHA256 signature scan\n");
			break;
		case RECORD_TEXT:
			printf("Malicious string search\n");
			break;
		default:
			printf("Unknown\n");
			break;
	}
	printf("     * Name: %s\n", db->name);
	printf("     * Description: %s\n", db->description);
	printf("   ================================================================\n");
}
 

int
main(int argc, char* argv[])
{
	printf("CattyScan\n");
	printf("Vicious software scanner for UNIX\n");
	if (argc < 2) {
		printf("  Usage: %s file file ...\n", argv[0]);
		return 1;
	}

	printf(" + Loading vicious software database...\n");

	char* home = getenv("HOME");
	char databaseFile[PATH_MAX];
	if (home != NULL)
		snprintf(databaseFile, PATH_MAX, "%s/.catty/vicious.db", home);

	CattyDB* database = new CattyDB(databaseFile);
	if (database->GetRecordCount() == 0) {
		delete database;
		return 0;
	} else
		printf(" + [%ld records loaded]\n", database->GetRecordCount());

	long fileCount = argc - 1;
	long currentFile = argc - 1;
	long currentFileIndex = 0;

	int found = 0;
	printf(" + Scanning %d files...\n", fileCount);
	while (currentFile > 1) {

		struct stat sb;
		stat(argv[currentFile], &sb);
		if ((sb.st_mode & S_IFMT) != S_IFREG) {
			currentFile--;
			continue;
		}

		index_t result = database->ScanFile(argv[currentFile]);
		if (result >= 0) {
			record dbRecord;
			database->GetRecord(result, &dbRecord);
			display_match(&dbRecord, argv[currentFile]);
			found++;
		}
		currentFileIndex = fileCount - currentFile;

		if (!(currentFileIndex % 100) && currentFileIndex != 0)
			printf(" - %ld files scanned...\n", currentFileIndex);
		currentFile--;
	}
	delete database;

	printf(" * Scanning complete. \n");
	printf(" * %ld of %ld files scanned. %d matches detected.\n",
		currentFileIndex, fileCount, found);
	return found;
}
