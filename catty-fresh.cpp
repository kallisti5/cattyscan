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

#include <curl/curl.h>
#include <curl/easy.h>

#include "common.h"


#ifdef DEBUG
#define TRACE(x...) printf(" D " x)
#else
#define TRACE(x...)
#endif
#define ERROR(x...) printf(" \033[31m! Error:\033[0m " x)
#define WARNING(x...) printf(" \033[33m! Warning:\033[0m " x)
#define CLEAN(x...) printf(" \033[37m+ Scanned:\033[0m " x)


size_t
writeCallback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	static int first_time = 1;

	char* home = getenv("HOME");
	char configPath[PATH_MAX];
	char databaseFile[PATH_MAX];

	if (home != NULL) {
		snprintf(configPath, PATH_MAX, "%s/.catty", home);
		snprintf(databaseFile, PATH_MAX, "%s/vicious.db", configPath);
	}

	struct stat st;
	if (stat(configPath, &st) != 0) {
		if (!S_ISDIR(st.st_mode)) {
			if (mkdir(configPath, 0755) != 0) {
				ERROR("Couldn't create %s directory!\n", configPath);
				return 0;
			}
		}
	}

	static FILE *outfile;
	size_t written;
	if (first_time) {
		first_time = 0;
		outfile = fopen(databaseFile, "w");
		if (outfile == NULL) {
			return -1;
		}
	}
	written = fwrite(ptr, size, nmemb, outfile);

	return written;
}


static bool
obtainDB(char* url)
{
	CURL *curl = curl_easy_init();

	if (curl == NULL) {
		ERROR("%s: LibCurl init failed!\n", __func__);
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);

	// TODO: We should have our own CA to prevent man-in-the-middle
	// curl_easy_setopt(curl, CURLOPT_CAPATH, 0L);

	CURLcode res = curl_easy_perform(curl);

	// Check for failures
	if (res != CURLE_OK) {
		ERROR("%s: LibCurl failure: %s\n", __func__, curl_easy_strerror(res));
		return false;
	}

	curl_easy_cleanup(curl);

	printf("Virus database updated successfully!\n");
	return true;
}


int
main(int argc, char* argv[])
{
	printf("CattyFresh, CattyScan v%d.%d\n", VERSION_MAJOR, VERSION_MINOR);
	printf("CattyFresh vicious software database updater\n");


	char cattyURL[] = "http://cdn.cattyscan.net";
	obtainDB(cattyURL);

	return 0;
}
