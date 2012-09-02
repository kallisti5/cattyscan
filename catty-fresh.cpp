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


static bool
obtainDB()
{
	CURL *curl = curl_easy_init();

	if (curl == NULL) {
		ERROR("%s: LibCurl init failed!\n", __func__);
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, "https://cdn.cattyscan.net/");

	// TODO: We should have our own CA to prevent man-in-the-middle
	// curl_easy_setopt(curl, CURLOPT_CAPATH, 0L);

	CURLcode res = curl_easy_perform(curl);

	// Check for failures
	if (res != CURLE_OK) {
		ERROR("%s: LibCurl failure: %s\n", curl_easy_strerror(res));
		return false;
	}

	curl_easy_cleanup(curl);
	return true;
}


int
main(int argc, char* argv[])
{
	printf("CattyFresh, CattyScan v%d.%d\n", VERSION_MAJOR, VERSION_MINOR);
	printf("CattyFresh vicious software database updater\n");

	char* home = getenv("HOME");
	char databaseFile[PATH_MAX];
	if (home != NULL)
		snprintf(databaseFile, PATH_MAX, "%s/.catty/vicious.db", home);

	obtainDB();
	return 0;
}
