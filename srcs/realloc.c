#include "ft_malloc.h"

void *realloc(void *ptr, size_t size)
{
	// Implementation of realloc
	printf("realloc of %p called with size: %zu\n", ptr, size);
	return NULL; // Placeholder
}