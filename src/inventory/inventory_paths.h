/*
 * Copyright, 2012-2014 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef __INVENTORY_PATHS_H__
#define __INVENTORY_PATHS_H__


#define CORE_BINARIES	(0 << 1)
#define CORE_LIBRARIES	(1 << 1)
#define CORE_CONFIGS	(2 << 1)


const struct inventory_paths {
	uint32_t		type;
	bool			directory;
	const char*		path;
} kInventoryPaths[] = {
	{CORE_BINARIES,		true,	"/bin"},
	{CORE_BINARIES,		true,	"/usr/bin"},
	{CORE_LIBRARIES,	true,	"/lib"},
	{CORE_LIBRARIES,	true,	"/usr/lib"},
	{CORE_CONFIGS,		false,	"/etc/passwd"},
	{CORE_CONFIGS,		false,	"/etc/shadow"}
};

#endif /* __INVENTORY_PATHS_H__ */