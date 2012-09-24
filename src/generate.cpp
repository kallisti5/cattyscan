/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

extern "C" {
#include "crc.h"
}


#include "CattyDB.h"


bool
process_file(char* filename)
{
	struct stat st;
	stat(filename, &st);

	crc_t* checksum = (crc_t*)calloc((st.st_size / SIGNATURE_BLOCK_SIZE) + 1,
		sizeof(crc_t));

	if (checksum == NULL) {
		printf("   ! %s: \033[31mError: %s\033[0m\n",
			filename, strerror(errno));
		return false;
	}

	char* home = getenv("HOME");
	char databaseFile[PATH_MAX];
	if (home != NULL)
		snprintf(databaseFile, PATH_MAX, "%s/.catty/catty.db", home);
	CattyDB* db = new CattyDB(databaseFile);

	if (!db->EncodeFile(checksum, filename, st.st_size)) {
		free(checksum);
		delete db;
		return false;
	}

	printf("%s|", filename);

	long position = 0;
	while (position < (st.st_size / SIGNATURE_BLOCK_SIZE)) {
		printf("|%04X", checksum[position]);
		position++;
	}
	printf("\n");

	free(checksum);
	delete db;
	return true;
}


int
main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Catty fingerprint\n");
		printf("  Usage: %s file file ...\n", argv[0]);
		return 1;
	}
	while (argc > 1) {
		process_file(argv[argc - 1]);
		argc--;
	}
	return 0;
}
