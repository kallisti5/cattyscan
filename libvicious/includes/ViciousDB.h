/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef _VICIOUSDB_H
#define _VICIOUSDB_H


#include <stdio.h>

#include "sha2.h"


/* Record types */
#define RECORD_UNKNOWN			0 << 0
#define RECORD_SIGNATURE		1 << 0
#define RECORD_TEXT				1 << 1

/* Record limits */
#define RECORD_MAX_NAME			32
#define RECORD_MAX_DESCRIPTION	1024
#define RECORD_MAX_VALUE		65535
#define RECORD_MAX_TEXT			1024
#define RECORD_MAX_TOTAL		RECORD_MAX_NAME \
								+ RECORD_MAX_DESCRIPTION \
								+ RECORD_MAX_VALUE + 4

/* Signature record */
#define SHA_LENGTH				32 // Signature Length

/* Suspect thresholds */
#define THRESHOLD_CRITICAL		90
#define THRESHOLD_POSSIBLE		50

typedef long index_t;

typedef struct {
	int			type;
	char		name[RECORD_MAX_NAME + 1];
	char		description[RECORD_MAX_DESCRIPTION + 1];
	char		value[RECORD_MAX_VALUE + 1];
} record;


class ViciousDB
{
public:
				ViciousDB(char* filename);
				~ViciousDB();

		index_t ScanFile(char* filename);
		bool	GenerateSHA(FILE* handle, char* result);
		index_t	GetRecordCount() { return fRows; }

private:
		index_t	CheckSignature(char* hash);
		index_t	SearchString(char* string);

		record*	fRecord;
		long	fRows;
};


#endif /* _VICIOUSDB_H */
