#include "ft_malloc.h"

void *calloc(size_t nmemb, size_t size)
{
	size_t total_size;
	void *ptr;
	size_t i;

	if (nmemb == 0 || size == 0)
		return NULL;

	if (nmemb > (size_t)-1 / size)
		return NULL;

	total_size = nmemb * size;
	ptr = malloc(total_size);
	if (!ptr)
		return NULL;
	i = 0;
	while (i < total_size) {
		((char *)ptr)[i] = 0;
		i++;
	}
	return ptr;
}
