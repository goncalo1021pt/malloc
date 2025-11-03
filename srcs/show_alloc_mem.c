#include "ft_malloc.h"


static void display_zones(const char *label, t_zone *zone, int fd)
{
	t_block *block;

	while (zone) {
		ft_fprintf(fd, "%s : %p\n", label, zone->start);
		block = zone->blocks;
		while (block) {
			if (!block->free) {
				ft_fprintf(fd, "%p - %p : %d bytes\n", 
					(void *)block->data, 
					(void *)((char *)block->data + block->size), 
					block->size);
			}
			block = block->next;
		}
		zone = zone->next;
	}
}


void show_alloc_mem(void)
{
    pthread_mutex_lock(&g_malloc_metadata.mutex);

    display_zones("TINY", g_malloc_metadata.tiny, 1);
    display_zones("SMALL", g_malloc_metadata.small, 1);
    display_zones("LARGE", g_malloc_metadata.large, 1);
    
    pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
