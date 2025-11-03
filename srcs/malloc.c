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
	zone->block_count = 1;

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

t_zone *create_zone(size_t zone_size, size_t block_size) {
	t_zone *zone;
	t_block *block;

	zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED)
		return NULL;

	zone->start = zone;
	zone->zone_size = zone_size;
	zone->block_count = (zone_size - sizeof(t_zone)) / block_size;
	zone->next = NULL;

	block = (t_block *)((char *)zone + sizeof(t_zone));
	zone->blocks = block;

	for (size_t ctd = 0; ctd < zone->block_count; ctd++) {
		block->size = block_size - sizeof(t_block);
		block->free = 1;
		if (ctd < zone->block_count - 1)
			block->next = (t_block *)((char *)block + block_size);
		else
			block->next = NULL;
		block = block->next;
	}
	return zone;
}

t_block *find_free_block_in_zones(t_zone *zone_list, size_t size) {
	t_zone *zone = zone_list;
	t_block *block;

	while (zone) {
		block = zone->blocks;
		while (block) {
			if (block->free && block->size >= size) {
				return block;
			}
			block = block->next;
		}
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
		block->free = 0;
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return (void *)(block->data);
	}
	t_zone *new_zone = create_zone(TINY_ZONE_SIZE, TINY_BLOCK_SIZE);
	if (!new_zone) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	new_zone->next = g_malloc_metadata.tiny;
	g_malloc_metadata.tiny = new_zone;
	block = new_zone->blocks;
	block->free = 0;
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
	return (void *)(block->data);
}

static void *allocate_small(size_t size) {
	t_block *block;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);

	block = find_free_block_in_zones(g_malloc_metadata.small, size);
	if (block) {
		block->free = 0;
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return (void *)(block->data);
	}
	t_zone *new_zone = create_zone(SMALL_ZONE_SIZE, SMALL_BLOCK_SIZE);
	if (!new_zone) {
		pthread_mutex_unlock(&g_malloc_metadata.mutex);
		return NULL;
	}
	new_zone->next = g_malloc_metadata.small;
	g_malloc_metadata.small = new_zone;
	block = new_zone->blocks;
	block->free = 0;
	pthread_mutex_unlock(&g_malloc_metadata.mutex);
	return (void *)(block->data);
}

void *malloc(size_t size) {
	void *exit;
	if (size == 0)
		return NULL;
	if (size <= TINY_MAX_SIZE) 
		exit = allocate_tiny(size);
	else if (size <= SMALL_MAX_SIZE)
		exit = allocate_small(size);
	else 
		exit = allocate_large(size);
	return exit;
}
