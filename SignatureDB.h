#ifndef _SIGNATUREDB_H
#define _SIGNATUREDB_H


typedef struct signature {
	char    name[32];
	char    signature[1024];
};


class SignatureDB
{
public:
				SignatureDB(char* filename);
				~SignatureDB();

		long	GetRecordCount() { return fRows; }
		int		Search(char* data);
private:
		long	fRows;
};


#endif /* _SIGNATUREDB_H */
