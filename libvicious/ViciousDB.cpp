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

#include "util.h"


#define ERROR(x...) printf(" ! " x)

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
	while (fgets(buffer, LINESZ, handle)) {
		if ((fRows % 25) == 0)
			printf(".");
		fRows++;
	}

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

		if ((index % 5) == 0)
			printf(".");

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

		if (matches <= block)
			*hitrate = (matches * 100 / block * 100) / 100;
		else
			*hitrate = 0;

		#if 0
		printf("%d blocks of %d blocks hit (rate %d%%)\n",
			matches, block, *hitrate);
		#endif

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


int
ViciousDB::ScanFile(char* filename)
{
	struct stat st;
	stat(filename, &st);

	crc_t* checksum = (crc_t*)calloc((st.st_size / BLOCK_SIZE) + 1,
		sizeof(crc_t));

	if (checksum == NULL) {
		printf("   ! %s: \033[31mError: %s\033[0m\n",
			filename, strerror(errno));
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

	#if 0
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
	#endif

	free(checksum);

	return result;
}
