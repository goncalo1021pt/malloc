#include "ft_malloc.h"

static t_block *defragment_block(t_zone *zone, t_block *block) {
	t_block *next = block->next;
	t_block *prev = block->prev;
	void *block_end = (char *)block + sizeof(t_block) + block->size;
	
	if (next && next->free && (void *)next == block_end) {
		remove_from_free_list(zone, next);
		
		block->size += sizeof(t_block) + next->size;
		block->next = next->next;
		if (next->next)
			next->next->prev = block;
	}
	
	if (prev && prev->free) {
		void *prev_end = (char *)prev + sizeof(t_block) + prev->size;
		if ((void *)block == prev_end) {
			remove_from_free_list(zone, prev);
			prev->size += sizeof(t_block) + block->size;
			prev->next = block->next;
			if (block->next)
				block->next->prev = prev;
			
			return prev;
		}
	}
	
	return block;
}

static int free_zone(t_zone **zone_list, void *ptr)
{
	t_zone **current = zone_list;
	t_zone *zone;
	t_block *block;

	while (*current) {
		zone = *current;
		if (ptr >= zone->start && 
			ptr < (void *)((char *)zone->start + zone->zone_size)) {
			block = zone->blocks;
			while (block) {
				if ((void *)block->data == ptr) {
					if (block->free) {
						debug_check_double_free(block);
						return 1;
					}
					
					zone->blocks_allocated--;
					debug_perturb_free((void *)block->data, block->size);
					
					block = defragment_block(zone, block);
					add_to_free_list(zone, block);
					
					if (zone->blocks_allocated == 0 && zone->next != NULL) {
						*current = zone->next;
						munmap(zone, zone->zone_size);
					}
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
	
	init_debug_config();
	
	if (ptr == NULL)
		return;
	
	debug_log_free(ptr);
	g_debug_config.total_frees++;
	
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
	if (free_zone(current, ptr)) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return;
	}
	current = &g_malloc_metadata.small;
	if (free_zone(current, ptr)) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return;
	}
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
}
