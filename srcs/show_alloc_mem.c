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

static size_t calculate_total(t_zone *zone) {
    size_t total = 0;
    t_block *block;
    
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (!block->free)
                total += block->size;
            block = block->next;
        }
        zone = zone->next;
    }
    return total;
}

void show_alloc_mem(void)
{
	size_t total;

    pthread_mutex_lock(&g_malloc_metadata.mutex);

    display_zones("TINY", g_malloc_metadata.tiny, 1);
    display_zones("SMALL", g_malloc_metadata.small, 1);
    display_zones("LARGE", g_malloc_metadata.large, 1);

	total = calculate_total(g_malloc_metadata.tiny) +
            calculate_total(g_malloc_metadata.small) +
            calculate_total(g_malloc_metadata.large);
    
    write_str(1, "Total : ");
    write_number(1, total);
    write_str(1, " bytes\n");
    
    pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
