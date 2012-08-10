
#include "ViriiDB.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>


ViriiDB::ViriiDB(char* filename)
	:
	fRows(0)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL)
		printf("%s: %s\n", __func__, strerror(errno));

	fseek(handle, 0, SEEK_END);
	long length = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	#define LINESZ 1024
	char buffer[LINESZ];
	char delim[] = "|";

	while (fgets(buffer, LINESZ, handle)) {
		char* result = strtok(buffer, delim);
		// TODO: result = Virii Name
		//printf("%s", result);
		result = strtok(NULL, delim);
		// TODO: result = Virii Signature
		//printf("%s", result);
		fRows++;
	}
	fclose(handle);
}


ViriiDB::~ViriiDB()
{
}


int
ViriiDB::Search(char* data)
{
	//printf("%s\n", data);
	return 60;
}
