/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include "main.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

extern "C" {
#include "crc.h"
}


#include "SignatureDB.h"


/* CRC block sizes (in bytes) */
#define BLOCK_SIZE	64


bool
encode_data(crc_t* result, char* filename, long length)
{
	FILE* handle = fopen(filename, "rb");
	if (handle == NULL) {
		printf("   ! %s: \033[31mError: %s\033[0m\n",
			filename, strerror(errno));
		return false;
	}

	char* buffer = (char*)calloc(BLOCK_SIZE + 1, sizeof(char));
	if (buffer == NULL) {
		printf("   ! %s: \033[31mError: %s\033[0m\n",
			filename, strerror(errno));
		fclose(handle);	// close file
		return false;
	}

	int pos;
	long block = 0;
	for (pos = 0; pos < length; pos += BLOCK_SIZE) {
		memset(buffer, 0, BLOCK_SIZE);
		fseek(handle, pos, SEEK_SET);
		fread(buffer, BLOCK_SIZE, 1, handle);
		crc_t crc = crc_init();
		crc = crc_update(crc, buffer, BLOCK_SIZE);
		crc = crc_finalize(crc);
		result[block] = crc;
		//printf("%04X", result[block]);
		block++;
	}
	fclose(handle);	// close file
	free(buffer); // free buffer
	return true;
}


bool
process_file(char* filename, SignatureDB* db)
{
	struct stat st;
	stat(filename, &st);

	crc_t* checksum = (crc_t*)calloc((st.st_size / BLOCK_SIZE) + 1,
		sizeof(crc_t));

	if (checksum == NULL) {
		printf("   ! %s: \033[31mError: %s\033[0m\n",
			filename, strerror(errno));
		return false;
	}

	if (!encode_data(checksum, filename, st.st_size)) {
		free(checksum);
		return false;
	}

	char matchName[SIGNATURE_MAX_NAME];
	int result = 0;
	bool trigger = db->Search(checksum, (st.st_size / BLOCK_SIZE),
		matchName, &result);

	int fgColor = 0;
	if (trigger) {
		if (result > THRESHOLD_CRITICAL)
			fgColor = 31;
		else
			fgColor = 35;

		printf("   * %s: %ld blocks checked, chance of infection: "
			"\033[%dm%d%%\033[0m (%s)\n", filename, (st.st_size / BLOCK_SIZE),
			fgColor, result, matchName);
	} else {
		fgColor = 32;
		printf("   * %s: %ld blocks checked, chance of infection: "
			"\033[%dm%d%%\033[0m\n", filename, (st.st_size / BLOCK_SIZE),
			fgColor, result);
	}

	free(checksum);

	return true;
}


int
main(int argc, char* argv[])
{
	printf("BitVirii Scanner\n");
	if (argc < 2) {
		printf("  Usage: %s file file ...\n", argv[0]);
		return 1;
	}
	printf(" + Loading rootkit signature database...");

	SignatureDB* rootkitDB = new SignatureDB((char*)"db/rootkits.db");
	printf("[OK, %ld records loaded]\n", rootkitDB->GetRecordCount());

	printf(" + Scanning %d files...\n", argc - 1);
	while (argc > 1) {
		process_file(argv[argc - 1], rootkitDB);
		argc--;
	}
	return 0;
}
