#include "ft_malloc.h"

t_malloc_metadata g_malloc_metadata = {NULL, NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

void *malloc(size_t size) {
	size_t total_size;
	t_block *block;
	t_zone *zone;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);
	total_size = size + sizeof(t_block);
	zone = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	zone->start = zone;
	zone->zone_size = total_size;
	zone->block_count = 1;

	block = (t_block *)(char *)zone + sizeof(t_zone);
	block->size = size;
	block->free = 0;
	block->next = NULL;

	zone->blocks = block;
	zone->next = g_malloc_metadata.large;
	g_malloc_metadata.large = zone;
	pthread_mutex_unlock(&g_malloc_metadata.mutex);

	return (void *)(block->data);
}
