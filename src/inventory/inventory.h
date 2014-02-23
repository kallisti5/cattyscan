/*
 * Copyright, 2012-2014 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef __INVENTORY_H__
#define __INVENTORY_H__


#include <cstdlib>
#include <limits.h>
#include <stdint.h>


#define SHA_LENGTH_MAX			64 // SHA 512 would be the max

#define BASELINE_DB_BLOCK		128

#define PLATFORM_RPM	(0 << 1)
#define PLATFORM_DEB	(1 << 1)


struct baseline_db {
	char	path[PATH_MAX];
	char	sha[SHA_LENGTH_MAX];
};


class BaselineDatabase
{
public:
							BaselineDatabase();
							~BaselineDatabase();

			void			Add(const char* filename, char* hash);
			bool			Lookup(const char* filename, char* hash);
			void			Empty();

private:
	struct	baseline_db*	fBaselineDB;
			uint32_t		fBaselineDBSize;
			uint32_t		fBaselineDBCount;
};


class InventoryEngine
{

public:
							InventoryEngine();
							~InventoryEngine();

			uint32_t		Check();
			void			Baseline();
private:
			BaselineDatabase* fBaselineStore;
			void			ProcessDirectory(const char* name, int level);
			bool			GenerateSHA(const char* filename, char* result);
};


#endif /* __INVENTORY_H__ */
