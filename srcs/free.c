#include "ft_malloc.h"

void free(void *ptr)
{
	t_zone *zone;
	
	if (ptr == NULL)
		return;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	zone = g_malloc_metadata.large;
	if (zone) {
		t_zone **current = &g_malloc_metadata.large;
		while (*current) {
			if (*current == zone) {
				*current = zone->next;
				munmap(zone->start, zone->zone_size);
				pthread_mutex_unlock(&g_malloc_metadata.mutex);
				return;
			}
			current = &(*current)->next;
		}
	}
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
