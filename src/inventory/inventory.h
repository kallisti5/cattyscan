/*
 * Copyright, 2012-2014 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef __INVENTORY_H__
#define __INVENTORY_H__


#include <cstdlib>
#include <stdint.h>

#include "inventory_paths.h"


#define SHA_LENGTH_MAX			64 // SHA 512 would be the max

#define PLATFORM_RPM	(0 << 1)
#define PLATFORM_DEB	(1 << 1)


class InventoryEngine
{

public:
							InventoryEngine();
							~InventoryEngine();

			uint32_t		Check();
			void			Index(uint32_t item_mask);
private:
			uint32_t		platform;
			bool			GenerateSHA(const char* filename, char* result);
};


#endif /* __INVENTORY_H__ */