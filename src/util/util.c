// Minote - util.c

#include "util.h"

#include <stdlib.h>

#include "log.h"

void *allocate(size_t size)
{
	void *result = calloc(1, size);
	if (result == NULL) {
		logCrit("Failed to allocate %d bytes", size);
		exit(EXIT_FAILURE);
	}
	return result;
}

void *reallocate(void *memory, size_t size)
{
	void *result = realloc(memory, size);
	if (result == NULL) {
		logCrit("Failed to reallocate %d bytes", size);
		exit(EXIT_FAILURE);
	}
	return result;
}

void _assertFailed(const char *cond)
{
	logCrit("Assert failed: %U", cond);
	exit(EXIT_FAILURE);
}

void noop(void)
{ }