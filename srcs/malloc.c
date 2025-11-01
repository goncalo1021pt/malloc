#include "ft_malloc.h"

void *malloc(size_t size)
{
	// void *ptr;

	
	// Implementation of malloc
	(void)size; // Suppress unused parameter warning
	write(2, "hello from malloc.c\n", 20);
	// TODO: implement actual malloc
	return NULL; // Placeholder
}
