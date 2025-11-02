#include "ft_malloc.h"


static void display_zones(t_zone *zone, int fd)
{
	t_block *block;

	while (zone)
	{
		ft_fprintf(fd, "ZONE : %p - %d bytes\n", zone->start, zone->zone_size);
		block = zone->blocks;
		while (block)
		{
			ft_fprintf(fd, "%p - %p : %d bytes\n", (void *)block->data, (void *)block->data + block->size, block->size);
			block = block->next;
		}
		zone = zone->next;
	}
}


void show_alloc_mem(void)
{
	t_zone *zone;
	int fd = 1;
	pthread_mutex_lock(&g_malloc_metadata.mutex);

	zone = g_malloc_metadata.tiny;
	display_zones(zone, fd);

	zone = g_malloc_metadata.small;
	display_zones(zone, fd);

	zone = g_malloc_metadata.large;
	display_zones(zone, fd);
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
