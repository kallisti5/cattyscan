/*
 * Copyright, 2012-2014 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */


#include "inventory.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "sha2.h"


BaselineDatabase::BaselineDatabase()
	:
	fBaselineDBSize(BASELINE_DB_BLOCK),
	fBaselineDBCount(0)
{
	fBaselineDB = (baseline_db*)malloc(sizeof(baseline_db) * fBaselineDBSize);
}

BaselineDatabase::~BaselineDatabase()
{
	printf("Closing baseline database...\n");
	printf("Database size: %d\n", fBaselineDBSize);
	printf("Database item count: %d\n", fBaselineDBCount);

	if (fBaselineDB != NULL) {
		free(fBaselineDB);
		fBaselineDBSize = 0;
		fBaselineDBCount = 0;
	}
}

void
BaselineDatabase::Add(const char* filename, char* hash)
{
	if (fBaselineDBCount >= fBaselineDBSize) {
		printf("Baseline database full! Extending...\n");
		fBaselineDBSize += BASELINE_DB_BLOCK;
		baseline_db* newDB = (baseline_db*)realloc(fBaselineDB,
			sizeof(baseline_db) * fBaselineDBSize);
		if (newDB)
			fBaselineDB = newDB;
		else {
			printf("Error allocating room for Baseline Database!\n");
			return;
		}
	}

	strcpy(fBaselineDB[fBaselineDBCount].path, filename);
	strcpy(fBaselineDB[fBaselineDBCount].sha, hash);

	printf("%s  %s\n", fBaselineDB[fBaselineDBCount].sha,
		fBaselineDB[fBaselineDBCount].path);

	fBaselineDBCount++;
}


bool
BaselineDatabase::Lookup(const char* filename, char* hash)
{

}


void
BaselineDatabase::Empty()
{

}


InventoryEngine::InventoryEngine()
{
	fBaselineStore = new BaselineDatabase();
}


InventoryEngine::~InventoryEngine()
{
	if (fBaselineStore != NULL)
		delete fBaselineStore;
}


bool
InventoryEngine::GenerateSHA(const char* filename, char* result)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL) {
		printf("%s: %s\n", filename, strerror(errno));
		return false;
	}

	fseek(handle, 0, SEEK_END);
	long length = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	unsigned char* buffer = (unsigned char*)malloc(length + 1);

	if (buffer == NULL) {
		fclose(handle);
		return false;
	}

	fread(buffer, length, 1, handle);
	fclose(handle);

	SHA256_CTX context;
	SHA256_Init(&context);
	SHA256_Update(&context, (unsigned char*)buffer, length);
	SHA256_End(&context, result);

	free(buffer);
	return true;
}


uint32_t
InventoryEngine::Check()
{

}

void
InventoryEngine::ProcessDirectory(const char* name, int level)
{
	DIR* dir = opendir(name);
	if (dir == NULL)
		return;
	struct dirent* entry = readdir(dir);

	do {
		char cwpath[PATH_MAX];
		int len = snprintf(cwpath, sizeof(cwpath) - 1,
			"%s/%s", name, entry->d_name);
		cwpath[len] = 0;

		if (entry->d_type == DT_DIR) {
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
				continue;
			ProcessDirectory(cwpath, level + 1);
		} else {
			char hash[SHA_LENGTH_MAX];
			if (!GenerateSHA(cwpath, hash)) {
				continue;
			} else {
				fBaselineStore->Add(cwpath, hash);
			}
		}
	} while (entry = readdir(dir));
}

void
InventoryEngine::Baseline()
{
	ProcessDirectory("/etc", 0);
}
