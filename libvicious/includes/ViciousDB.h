/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef _VICIOUSDB_H
#define _VICIOUSDB_H


#include "crc.h"


/* CRC block sizes (in bytes) */
#define BLOCK_SIZE  512

#define SIGNATURE_MAX_NAME	32
#define SIGNATURE_MAX		65535

/* Suspect thresholds */
#define THRESHOLD_CRITICAL 90
#define THRESHOLD_POSSIBLE 50


typedef struct {
	char	name[SIGNATURE_MAX_NAME + 1];
	long	crcBlocks;
	crc_t	signature[SIGNATURE_MAX + 1];
} sigDB;


class ViciousDB
{
public:
				ViciousDB(char* filename);
				~ViciousDB();

		int		ScanFile(char* filename);
		bool	EncodeFile(crc_t* result, char* filename, long length);
		long	GetRecordCount() { return fRows; }

private:
		bool	Search(crc_t* data, long blocks, char* matchName, int* hitrate);
		sigDB*	fSignature;
		long	fRows;
};


#endif /* _VICIOUSDB_H */
