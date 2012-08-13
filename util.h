/*
 * Copyright, 2012 Alexander von Gluck - UNIXZen Software
 *
 * Released under the terms of the MIT license.
 */
#ifndef _UTIL_H
#define _UTIL_H


static unsigned int
htoi(const char *ptr)
{
	unsigned int value = 0;
	char ch = *ptr;

	while (ch == ' ' || ch == '\t')
		ch = *(++ptr);

	for (;;) {

		if (ch >= '0' && ch <= '9')
			value = (value << 4) + (ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			value = (value << 4) + (ch - 'A' + 10);
		else if (ch >= 'a' && ch <= 'f')
			value = (value << 4) + (ch - 'a' + 10);
		else
			return value;
		ch = *(++ptr);
	}
}


#endif /* _UTIL_H */
