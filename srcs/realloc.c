#include "ft_malloc.h"

t_block *get_block_from_ptr(void *ptr, size_t size) {
	t_zone *zone;
	t_block *block;

	(void)size;
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
	if (!ptr)
		return malloc(size);
	if (size == 0) {
		free(ptr);
		return NULL;
	}
	t_block *block = get_block_from_ptr(ptr, size);
	if (!block)
		return NULL;

	if (size > block->size) {
		void *new_ptr = malloc(size);
		if (!new_ptr)
			return NULL;
		ft_memcpy(new_ptr, ptr, block->size);
		free(ptr);
		return new_ptr;
	}
	return ptr;
}