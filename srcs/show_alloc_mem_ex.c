#include "ft_malloc.h"

static void display_block(t_block *block, int fd)
{
	if (block->free) {
		write_color(fd, COLOR_RED);
		write_str(fd, "  [FREE] ");
	} else {
		write_color(fd, COLOR_GREEN);
		write_str(fd, "  [ALLOCATED] ");
	}
	
	write_hex(fd, (void *)block->data);
	write_str(fd, " - ");
	write_hex(fd, (void *)((char *)block->data + block->size));
	write_str(fd, " : ");
	write_number(fd, block->size);
	write_str(fd, " bytes");
	write_color(fd, COLOR_RESET);
	write_str(fd, "\n");
	
	print_hexdump(fd, (void *)block->data, block->size, MAX_HEXDUMP_BYTES);
}

static void display_zones_ex(const char *label, t_zone *zone, int fd, t_stats *stats)
{
	t_block *block;

	while (zone) {
		stats->total_zones++;
		
		write_color(fd, COLOR_CYAN);
		write_color(fd, COLOR_BOLD);
		write_str(fd, label);
		write_str(fd, " : ");
		write_hex(fd, zone->start);
		write_color(fd, COLOR_RESET);
		write_str(fd, "\n");
		
		block = zone->blocks;
		while (block) {
			display_block(block, fd);
			
			if (block->free) {
				stats->free_blocks++;
				stats->free_bytes += block->size;
			} else {
				stats->allocated_blocks++;
				stats->allocated_bytes += block->size;
			}
			
			block = block->next;
		}
		
		write_str(fd, "\n");
		zone = zone->next;
	}
}

static void display_statistics(t_stats *stats, int fd)
{
	write_color(fd, COLOR_YELLOW);
	write_color(fd, COLOR_BOLD);
	write_str(fd, "=== STATISTICS ===\n");
	write_color(fd, COLOR_RESET);
	
	write_str(fd, "Total zones: ");
	write_number(fd, stats->total_zones);
	write_str(fd, " (");
	write_number(fd, stats->tiny_zones);
	write_str(fd, " TINY, ");
	write_number(fd, stats->small_zones);
	write_str(fd, " SMALL, ");
	write_number(fd, stats->large_zones);
	write_str(fd, " LARGE)\n");
	
	write_color(fd, COLOR_GREEN);
	write_str(fd, "Allocated: ");
	write_number(fd, stats->allocated_bytes);
	write_str(fd, " bytes in ");
	write_number(fd, stats->allocated_blocks);
	write_str(fd, " blocks\n");
	write_color(fd, COLOR_RESET);
	
	write_color(fd, COLOR_RED);
	write_str(fd, "Free: ");
	write_number(fd, stats->free_bytes);
	write_str(fd, " bytes in ");
	write_number(fd, stats->free_blocks);
	write_str(fd, " blocks\n");
	write_color(fd, COLOR_RESET);
	
	size_t total_bytes = stats->allocated_bytes + stats->free_bytes;
	if (total_bytes > 0) {
		size_t fragmentation = (stats->free_bytes * 100) / total_bytes;
		write_str(fd, "Fragmentation: ");
		write_number(fd, fragmentation);
		write_str(fd, "%\n");
	}
}

void show_alloc_mem_ex(void)
{
	t_stats stats = {0, 0, 0, 0, 0, 0, 0, 0};
	
	pthread_mutex_lock(&g_malloc_metadata.mutex);
	
	write_color(1, COLOR_MAGENTA);
	write_color(1, COLOR_BOLD);
	write_str(1, "\n=== EXTENDED MEMORY ALLOCATION MAP ===\n\n");
	write_color(1, COLOR_RESET);
	
	t_zone *zone = g_malloc_metadata.tiny;
	while (zone) {
		stats.tiny_zones++;
		zone = zone->next;
	}
	
	zone = g_malloc_metadata.small;
	while (zone) {
		stats.small_zones++;
		zone = zone->next;
	}
	
	zone = g_malloc_metadata.large;
	while (zone) {
		stats.large_zones++;
		zone = zone->next;
	}
	
	display_zones_ex("TINY", g_malloc_metadata.tiny, 1, &stats);
	display_zones_ex("SMALL", g_malloc_metadata.small, 1, &stats);
	display_zones_ex("LARGE", g_malloc_metadata.large, 1, &stats);
	
	display_statistics(&stats, 1);
	
	write_str(1, "\n");
	
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
