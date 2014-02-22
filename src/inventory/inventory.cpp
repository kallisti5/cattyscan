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
InventoryEngine::Index(uint32_t item_mask)
{
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir ("/bin")) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {

			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;

			char filename[PATH_MAX];
			sprintf(filename, "/bin/%s", ent->d_name);
			char hash[SHA_LENGTH_MAX];
			if (!GenerateSHA(filename, hash)) {
				printf("%s: %s\n", filename, strerror(errno));
			}
			printf("%s  %s\n", hash, filename);
  		}
  		closedir(dir);
	} else {
		/* could not open directory */
		perror ("");
	}
}