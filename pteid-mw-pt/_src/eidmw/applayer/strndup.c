
#include <memory.h>
#include <stdlib.h>
#include <string.h>

//Win32 replacement for missing POSIX functions

size_t strnlen(const char*  str, size_t  maxlen)
{
	char*  p = memchr(str, 0, maxlen);

	if (p == NULL)
		return maxlen;
	else
		return (p - str);
}


char * strndup(const char *str, size_t maxlen)
{
	char *copy;
	size_t len;
	len = strnlen(str, maxlen);
	copy = malloc(len + 1);
	if (copy != NULL) {
		(void)memcpy(copy, str, len);
		copy[len] = '\0';
	}
	return copy;
}