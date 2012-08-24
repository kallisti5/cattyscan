/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include "ViciousDB.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "util.h"

#define DEBUG 1

#ifdef DEBUG
#define TRACE(x...) printf(" D " x)
#else
#define TRACE(x...)
#endif
#define ERROR(x...) printf(" \033[31m! Error:\033[0m " x)


ViciousDB::ViciousDB(char* filename)
	:
	fRows(0)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL) {
		ERROR("%s: %s: %s\n", __func__, filename, strerror(errno));
		return;
	}

	fseek(handle, 0, SEEK_END);
	long length = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	#define LINESZ 262144
	char buffer[LINESZ];
	char delim[] = "|";

	// Find number of entries
	while (fgets(buffer, LINESZ, handle))
		fRows++;

	// Allocate signature database
	fSignature = (sigDB*)calloc(fRows + 1, sizeof(sigDB));

	fseek(handle, 0, SEEK_SET);

	long index = 0;
	while (fgets(buffer, LINESZ, handle)) {
		// Store Signature Name
		char* result = strtok(buffer, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fSignature[index].name, result, SIGNATURE_MAX_NAME);

		// Store Signature
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}

		int hexPos = 0;
		int dataPos = 0;
		while (hexPos < strlen(result) && dataPos < SIGNATURE_MAX) {
			char hexString[] = { result[hexPos],
				result[hexPos + 1],
				result[hexPos + 2],
				result[hexPos + 3] };
			long blockValue = htoi(hexString);
			fSignature[index].signature[dataPos] = blockValue;
			//printf("Stored: 0x%04X\n", fSignature[index].signature[dataPos]);
			hexPos += 4;
			dataPos++;
		}
		fSignature[index].crcBlocks = hexPos;
		index++;
	}
	// index should eq fRows here

	fclose(handle);
}


ViciousDB::~ViciousDB()
{
	free(fSignature);
}


bool
ViciousDB::Search(crc_t* data, long blocks, char* matchName, int* hitrate)
{
	long record = 0;
	while (record < fRows) {
		int block = 0;
		int matches = 0;

		while (block < blocks && block < fSignature[record].crcBlocks) {
			matches
				+= (data[block] == fSignature[record].signature[block]) ? 1 : 0;
			block++;
		}

		if (block == 0)
			*hitrate = 0;
		else
			*hitrate = (matches * 100) / block;

		if (*hitrate > 25) {
			TRACE("%d blocks of %d blocks hit (rate %d%%) : %s\n", matches,
				block, *hitrate, fSignature[record].name);
		}

		if (*hitrate > THRESHOLD_POSSIBLE) {
			strcpy(matchName, fSignature[record].name);
			return true;
		}
		record++;
	}

	return false;
}


bool
ViciousDB::EncodeFile(crc_t* result, char* filename, long length)
{
	FILE* handle = fopen(filename, "rb");
	if (handle == NULL) {
		ERROR("%s: %s\n", filename, strerror(errno));
		return false;
	}

	char* buffer = (char*)calloc(BLOCK_SIZE + 1, sizeof(char));
	if (buffer == NULL) {
		ERROR("%s: %s\n", filename, strerror(errno));
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


int
ViciousDB::ScanFile(char* filename)
{
	struct stat st;
	stat(filename, &st);

	crc_t* checksum = (crc_t*)calloc((st.st_size / BLOCK_SIZE) + 1,
		sizeof(crc_t));

	if (checksum == NULL) {
		ERROR("%s: %s\n", filename, strerror(errno));
		return -1;
	}

	if (!EncodeFile(checksum, filename, st.st_size)) {
		free(checksum);
		return -1;
	}

	char matchName[SIGNATURE_MAX_NAME];
	int result = 0;
	bool trigger = Search(checksum, (st.st_size / BLOCK_SIZE),
		matchName, &result);

	if (result > 25)
		ERROR("%s: found traces of %s\n", filename, matchName);

	free(checksum);

	return result;
}
