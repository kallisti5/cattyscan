
#include "SignatureDB.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


SignatureDB::SignatureDB(char* filename)
	:
	fRows(0)
{
	FILE* handle = fopen(filename, "r");
	if (handle == NULL)
		printf("%s: %s\n", __func__, strerror(errno));

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
	fSignature = (sigDB*)malloc(fRows * sizeof(sigDB));

	fseek(handle, 0, SEEK_SET);

	long index = 0;
	while (fgets(buffer, LINESZ, handle)) {
		// Store Signature Name
		char* result = strtok(buffer, delim);
		strncpy(fSignature[index].name, result, SIGNATURE_MAX_NAME);
		// Store Signature
		result = strtok(NULL, delim);
		int hexPos = 0;
		int dataPos = 0;
		while (hexPos < strlen(result) && dataPos < SIGNATURE_MAX) {
			char hexString[] = { result[hexPos],
				result[hexPos + 1],
				result[hexPos + 2],
				result[hexPos + 3] };
			//printf("Broken apart: %s\n", hexString);
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


SignatureDB::~SignatureDB()
{
	free(fSignature);
}


bool
SignatureDB::Search(crc_t* data, long blocks, char* matchName, int* hitrate)
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
