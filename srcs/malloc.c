#include "ft_malloc.h"

t_malloc_metadata g_malloc_metadata = {NULL, NULL, NULL, PTHREAD_MUTEX_INITIALIZER};

static void *allocate_large(size_t size) {
	size_t total_size;
	t_block *block;
	t_zone *zone;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);
	total_size = size + sizeof(t_block);
	total_size = ROUND_UP_PAGE(total_size + sizeof(t_zone));
	zone = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	zone->start = zone;
	zone->zone_size = total_size;
	zone->blocks_allocated = 1;

	block = (t_block *)((char *)zone + sizeof(t_zone));
	block->size = size;
	block->free = 0;
	block->next = NULL;

	zone->blocks = block;
	zone->next = g_malloc_metadata.large;
	g_malloc_metadata.large = zone;
	pthread_mutex_unlock(&g_malloc_metadata.mutex);

	return (void *)(block->data);
}

t_zone *create_zone(size_t zone_size) {
	t_zone *zone;

	zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED) {
		return NULL;
	}

	zone->start = zone;
	zone->zone_size = zone_size;
	zone->blocks_allocated = 0;
	zone->blocks_free = 0;
	zone->next = NULL;
	zone->blocks = NULL;
	zone->free_list = NULL;
	zone->zone_end = (char *)zone + sizeof(t_zone);
	return zone;
}

static t_block *carve_block(t_zone *zone, size_t size) {
	void *zone_limit = (char *)zone + zone->zone_size;
	size_t space_needed = sizeof(t_block) + size;

	if ((char *)zone->zone_end + space_needed > (char *)zone_limit)
		return NULL;

	t_block *new_block = (t_block *)zone->zone_end;
	new_block->size = size;
	new_block->free = 0;
	new_block->next = NULL;
	new_block->prev = NULL;
	new_block->free_next = NULL;

	if (zone->blocks == NULL) {
		zone->blocks = new_block;
	} else {
		t_block *last = zone->blocks;
		while (last->next)
			last = last->next;
		last->next = new_block;
		new_block->prev = last;
	}

	zone->blocks_allocated++;
	zone->zone_end = (char *)new_block + sizeof(t_block) + size;

	return new_block;
}

void add_to_free_list(t_zone *zone, t_block *block) {
	block->free = 1;
	block->free_next = zone->free_list;
	zone->free_list = block;
	zone->blocks_free++;
}

void remove_from_free_list(t_zone *zone, t_block *block) {
	t_block *current = zone->free_list;
	t_block *prev = NULL;
	
	while (current) {
		if (current == block) {
			if (prev)
				prev->free_next = current->free_next;
			else
				zone->free_list = current->free_next;
			zone->blocks_free--;
			return;
		}
		prev = current;
		current = current->free_next;
	}
}

t_block *find_free_block_in_zones(t_zone *zone_list, size_t size) {
	t_zone *zone = zone_list;
	t_block *block;

	while (zone) {
		block = zone->free_list;
		while (block) {
			if (block->size >= size) {
				remove_from_free_list(zone, block);
				block->free = 0;
				zone->blocks_allocated++;
				return block;
			}
			block = block->free_next;
		}
		zone = zone->next;
	}

	zone = zone_list;
	while (zone) {
		block = carve_block(zone, size);
		if (block)
			return block;
		zone = zone->next;
	}
	return NULL;
}

static void *allocate_tiny(size_t size) {
	t_block *block;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);
	block = find_free_block_in_zones(g_malloc_metadata.tiny, size);
	if (block) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return (void *)(block->data);
	}

	t_zone *new_zone = create_zone(TINY_ZONE_SIZE);
	if (!new_zone) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	new_zone->next = g_malloc_metadata.tiny;
	g_malloc_metadata.tiny = new_zone;
	block = carve_block(new_zone, size);
	if (!block) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
	return (void *)(block->data);
}

static void *allocate_small(size_t size) {
	t_block *block;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);
	block = find_free_block_in_zones(g_malloc_metadata.small, size);
	if (block) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return (void *)(block->data);
	}

	t_zone *new_zone = create_zone(SMALL_ZONE_SIZE);
	if (!new_zone) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	new_zone->next = g_malloc_metadata.small;
	g_malloc_metadata.small = new_zone;
	block = carve_block(new_zone, size);
	if (!block) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}

	pthread_mutex_unlock(&g_malloc_metadata.mutex);
	return (void *)(block->data);
}

void *malloc(size_t size) {
	if (size == 0)
		return NULL;
	if (size <= TINY_MAX_SIZE) 
		return allocate_tiny(size);
	else if (size <= SMALL_MAX_SIZE)
		return allocate_small(size);
	else 
		return allocate_large(size);
}
