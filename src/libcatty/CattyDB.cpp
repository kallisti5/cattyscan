/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include "CattyDB.h"

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


CattyDB::CattyDB(char* filename)
	:
	fRows(0)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL) {
		ERROR("%s: %s: %s\n", __func__, filename, strerror(errno));
		ERROR("%s: You may want to try running catty-fresh first to "
			"obtain a new vicious software database\n", __func__);
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

		// Store record UUID
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fRecord[index].uuid, result, RECORD_MAX_UUID);

		// Store record description
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fRecord[index].description, result, RECORD_MAX_DESCRIPTION);

		// Store record Threat
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		fRecord[index].threat = atoi(result);

		// Store data
		result = strtok(NULL, delim);
		if (result == NULL) {
			fRows--;
			continue;
		}
		strncpy(fRecord[index].value, result, RECORD_MAX_VALUE);

		index++;
	}
	// index should eq fRows here

	fclose(handle);
}


CattyDB::~CattyDB()
{
	free(fRecord);
}


bool
CattyDB::GetRecord(index_t index, record* recordEntry)
{
	if (index < 0 || index > fRows)
		return false;

	memcpy(recordEntry, &fRecord[index], sizeof(record));

	return true;
}


index_t
CattyDB::CheckSignature(char* hash)
{
	index_t index = 0;
	while (index < fRows) {
		if (fRecord[index].type != RECORD_SIGNATURE) {
			// Skip non-signature records
			index++;
			continue;
		}
		if (memcmp(hash, fRecord[index].value, SHA_LENGTH_MAX) == 0) {
			return index;
		}
		index++;
	}
	return -1;
}


index_t
CattyDB::CheckString(FILE* handle)
{
	fseek(handle, 0, SEEK_END);
	long fileLength = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	char* buffer = (char*)malloc(fileLength + 1);
	if (buffer == NULL)
		return false;

	fread(buffer, fileLength, 1, handle);

	index_t index = 0;
	while (index < fRows) {
		if (fRecord[index].type != RECORD_TEXT) {
			// Skip non-text records
			index++;
			continue;
		}
		long hexLen = strlen(fRecord[index].value) - 1;

		long i = 0;
		long hexPos = 0;
		char data[hexLen + 1];
		while (i < hexLen) {
			char hexStr[] = { fRecord[index].value[hexPos], fRecord[index].value[hexPos + 1] };
			data[i] = htoi(hexStr);
			i++;
			hexPos += 2;
		}
		data[i] = '\0';

		if (memmem(buffer, fileLength, data) != NULL) {
			free(buffer);
			return index;
		}
		index++;
	}
	free(buffer);
	return -1;
}

bool
CattyDB::GenerateSHA(FILE* handle, char* result)
{
	fseek(handle, 0, SEEK_END);
	long length = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	unsigned char* buffer = (unsigned char*)malloc(length + 1);
	if (buffer == NULL)
		return false;

	fread(buffer, length, 1, handle);

	SHA256_CTX context;
	SHA256_Init(&context);
	SHA256_Update(&context, (unsigned char*)buffer, length);
	SHA256_End(&context, result);

	//printf("SHA: %s\n", result);

	free(buffer); // free buffer
	return true;
}


index_t
CattyDB::ScanFile(char* filename)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL) {
		ERROR("%s: %s\n", filename, strerror(errno));
		return -1;
	}

	// *** Generate SHA hash and compare
	char hash[SHA_LENGTH_MAX];
	if (!GenerateSHA(handle, hash)) {
		ERROR("%s: %s\n", filename, strerror(errno));
		fclose(handle);
		return -1;
	}

	long index = -1;

	index = CheckSignature(hash);
	if (index >= 0) {
		WARNING("%s: match! (%s)\n", filename, fRecord[index].description);
		fclose(handle);
		return index;
	}

	// *** Give file handle and do a binary grep through database
	index = CheckString(handle);
	if (index >= 0) {
		WARNING("%s: match! (%s)\n", filename, fRecord[index].description);
		fclose(handle);
		return index;
	}

	#if 0
	} else {
		CLEAN("%s: clean\n", filename);
	}
	#endif

	fclose(handle);
	return index;
}
