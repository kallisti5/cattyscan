#ifndef _SIGNATUREDB_H
#define _SIGNATUREDB_H


#include "crc.h"


#define SIGNATURE_MAX_NAME	32
#define SIGNATURE_MAX		4096

/* Suspect thresholds */
#define THRESHOLD_CRITICAL 90
#define THRESHOLD_POSSIBLE 50


typedef struct {
	char	name[SIGNATURE_MAX_NAME + 1];
	long	crcBlocks;
	crc_t	signature[SIGNATURE_MAX + 1];
} sigDB;


class SignatureDB
{
public:
				SignatureDB(char* filename);
				~SignatureDB();

		long	GetRecordCount() { return fRows; }
		bool	Search(crc_t* data, long blocks, char* matchName, int* hitrate);
private:
		sigDB*	fSignature;
		long	fRows;
};


#endif /* _SIGNATUREDB_H */
