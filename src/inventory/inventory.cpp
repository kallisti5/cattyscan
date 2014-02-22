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


InventoryEngine::InventoryEngine()
{
	#warning TODO: Detect platform package manager
	platform = PLATFORM_DEB;
}


InventoryEngine::~InventoryEngine()
{

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
				printf("%s  %s\n", hash, cwpath);
			}
		}
	} while (entry = readdir(dir));
}

void
InventoryEngine::Baseline()
{
	ProcessDirectory("/etc", 0);
}