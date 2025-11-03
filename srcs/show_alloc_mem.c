#include "ft_malloc.h"

static void display_zones(const char *label, t_zone *zone, int fd)
{
	t_block *block;

	while (zone) {
		write_str(fd, label);
		write_str(fd, " : ");
		write_hex(fd, zone->start);
		write_str(fd, "\n");
		
		block = zone->blocks;
		while (block) {
			if (!block->free) {
				write_hex(fd, (void *)block->data);
				write_str(fd, " - ");
				write_hex(fd, (void *)((char *)block->data + block->size));
				write_str(fd, " : ");
				write_number(fd, block->size);
				write_str(fd, " bytes\n");
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
