#ifndef _VIRIIDB_H
#define _VIRIIDB_H


class ViriiDB
{
public:
				ViriiDB(char* filename);
				~ViriiDB();

		long	GetRecordCount() { return fRows; }
		int		Search(char* data);
private:
		long	fRows;
};


#endif
