#include "ft_malloc.h"

static t_block *get_block_from_ptr(void *ptr) {
	t_zone *zone;
	t_block *block;

	zone = g_malloc_metadata.tiny;
	while (zone) {
		block = zone->blocks;
		while (block) {
			if ((void *)(block->data) == ptr) {
				return block;
			}
			block = block->next;
		}
		zone = zone->next;
	}

	zone = g_malloc_metadata.small;
	while (zone) {
		block = zone->blocks;
		while (block) {
			if ((void *)(block->data) == ptr) {
				return block;
			}
			block = block->next;
		}
		zone = zone->next;
	}

	zone = g_malloc_metadata.large;
	while (zone) {
		block = zone->blocks;
		while (block) {
			if ((void *)(block->data) == ptr) {
				return block;
			}
			block = block->next;
		}
		zone = zone->next;
	}

	return NULL;

}

void *realloc(void *ptr, size_t size)
{
	t_block *block;
	size_t old_size;
	void *new_ptr;

	init_debug_config();
	
	if (!ptr)
		return malloc(size);

	if (size == 0) {
		free(ptr);
		return NULL;
	}

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	block = get_block_from_ptr(ptr);
	if (!block) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}

	old_size = block->size;
	size_t aligned_size = ALIGN(size);

	if (aligned_size <= old_size) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return ptr;
	}

	pthread_mutex_unlock(&g_malloc_metadata.mutex);
	
	new_ptr = malloc(size);
	if (!new_ptr)
		return NULL;
	ft_memcpy(new_ptr, ptr, old_size);
	free(ptr);
	
	g_debug_config.total_reallocs++;
	debug_log_realloc(ptr, new_ptr, size);

	return new_ptr;
}