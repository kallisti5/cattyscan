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
#include <unistd.h>

#include "util.h"


#define DEBUG 1

#ifdef DEBUG
#define TRACE(x...) printf(" D " x)
#else
#define TRACE(x...)
#endif
#define ERROR(x...) printf(" \033[31m! Error:\033[0m " x)
#define WARNING(x...) printf(" \033[33m! Warning:\033[0m " x)
#define CLEAN(x...) printf(" \033[37m+ Scanned:\033[0m " x)


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

	char buffer[RECORD_MAX_TOTAL];

	// Find number of entries
	while (fgets(buffer, RECORD_MAX_TOTAL, handle))
		fRows++;

	// Allocate signature database
	fRecord = (record*)calloc(fRows + 1, sizeof(record));

	fseek(handle, 0, SEEK_SET);

	index_t index = 0;
	char delim[] = "|";
	while (fgets(buffer, RECORD_MAX_TOTAL, handle)) {
		// Store record type
		char* result = strtok(buffer, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		fRecord[index].type = atoi(result);

		// Store record Name
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fRecord[index].name, result, RECORD_MAX_NAME);

		// Store record description
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fRecord[index].description, result, RECORD_MAX_DESCRIPTION);		

		// Store data
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fRecord[index].value, result, RECORD_MAX_DESCRIPTION);		

		index++;
	}
	// index should eq fRows here

	fclose(handle);
}


ViciousDB::~ViciousDB()
{
	free(fRecord);
}


index_t
ViciousDB::CheckSignature(char* hash)
{
	index_t index = 0;
	while (index < fRows) {
		if (fRecord[index].type != RECORD_SIGNATURE) {
			// Skip non-signature
			index++;
			continue;
		}
		if (memcmp(hash, fRecord[index].value, SHA_LENGTH) == 0) {
			ERROR("MATCH!\n");
			return index;
		} else {
			ERROR("NO MATCH!\n");
		}
		index++;
	}
	return -1;
}


index_t
ViciousDB::SearchString(char* string)
{


	return -1;
}

bool
ViciousDB::GenerateSHA(FILE* handle, char* result)
{
	fseek(handle, 0, SEEK_END);
	long length = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	unsigned char* buffer = (unsigned char*)malloc(length + 1);
	if (buffer == NULL) {
		ERROR("%s: %s\n", __func__, strerror(errno));
		fseek(handle, 0, SEEK_SET);
		return false;
	}

	fread(buffer, length, 1, handle);

	SHA256_CTX context;
	SHA256_Init(&context);
	SHA256_Update(&context, (unsigned char*)buffer, length);
	SHA256_End(&context, result);

	printf("SHA: %s\n", result);

	free(buffer); // free buffer
	return true;
}


index_t
ViciousDB::ScanFile(char* filename)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL) {
		ERROR("%s: %s\n", filename, strerror(errno));
		return -1;
	}

	char hash[SHA_LENGTH];
	if (!GenerateSHA(handle, hash)) {
		fclose(handle);
		return -1;
	}

	long index = -1;
	index = CheckSignature(hash);

	if (index >= 0) {
		WARNING("%s: match! (%s)\n", filename, fRecord[index].name);
	}

	fclose(handle);
	return index;
}
