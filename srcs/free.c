#include "ft_malloc.h"

int free_zone(t_zone **current, void *ptr)
{
	t_zone *zone;
	t_block *block;

	while (*current) {
		zone = *current;
		if (ptr >= zone->start && 
			ptr < (void *)((char *)zone->start + zone->zone_size)) {
			block = zone->blocks;
			while (block) {
				if ((void *)block->data == ptr) {
					block->free = 1;
					pthread_mutex_unlock(&g_malloc_metadata.mutex);
					return 1;
				}
				block = block->next;
			}
		}
		current = &(*current)->next;
	}
	return 0;
}

void free(void *ptr)
{
	t_zone **current;
	t_zone *zone;
	
	if (ptr == NULL)
		return;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	
	current = &g_malloc_metadata.large;
	while (*current) {
		zone = *current;
		if (ptr >= zone->start && 
			ptr < (void *)((char *)zone->start + zone->zone_size)) {
			*current = zone->next;
			munmap(zone, zone->zone_size);
			pthread_mutex_unlock(&g_malloc_metadata.mutex);
			return;
		}
		current = &(*current)->next;
	}
	
	current = &g_malloc_metadata.tiny;
	if (free_zone(current, ptr))
		return;

	current = &g_malloc_metadata.small;
	if (free_zone(current, ptr))
		return;
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
