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

t_zone *create_zone(size_t zone_size, size_t block_size) {
	t_zone *zone;

	zone = mmap(NULL, zone_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (zone == MAP_FAILED) {
		return NULL;
	}

	zone->start = zone;
	zone->zone_size = zone_size;
	zone->block_count = (zone_size - sizeof(t_zone)) / block_size;
	zone->blocks_allocated = 0;
	zone->next = NULL;
	zone->blocks = NULL;
	return zone;
}

static void *get_zone_end(t_zone *zone) {
	t_block *block = zone->blocks;
	void *end = (char *)zone + sizeof(t_zone);

	while (block) {
		void *block_end = (char *)block + sizeof(t_block) + block->size;
		if (block_end > end) 
			end = block_end;
		block = block->next;
	}
	return end;
}

static t_block *carve_block(t_zone *zone, size_t size) {
	void *zone_end = (char *)zone + zone->zone_size;
	void *current_end = get_zone_end(zone);
	size_t space_needed = sizeof(t_block) + size;

	if ((char *)current_end + space_needed > (char *)zone_end)
		return NULL;

	t_block *new_block = (t_block *)current_end;
	new_block->size = size;
	new_block->free = 0;
	new_block->next = NULL;

	if (zone->blocks == NULL)
		zone->blocks = new_block;
	else {
		t_block *last = zone->blocks;
		while (last->next)
			last = last->next;
		last->next = new_block;
	}

	zone->block_count++;
	zone->blocks_allocated++;

	return new_block;
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
	bool was_free;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);

	block = find_free_block_in_zones(g_malloc_metadata.tiny, size);
	if (block) {
		was_free = block->free;
		block->free = 0;
		if (was_free) {
			t_zone *zone = g_malloc_metadata.tiny;
			while (zone) {
				if ((void *)block >= zone->start && 
					(void *)block < (void *)((char *)zone->start + zone->zone_size)) {
					zone->blocks_allocated++;
					break;
				}
				zone = zone->next;
			}
		}
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
	bool was_free;

	pthread_mutex_lock(&g_malloc_metadata.mutex);
	size = ALIGN(size);

	block = find_free_block_in_zones(g_malloc_metadata.small, size);
	if (block) {
		was_free = block->free;
		block->free = 0;
		if (was_free) {
			t_zone *zone = g_malloc_metadata.small;
			while (zone) {
				if ((void *)block >= zone->start && 
					(void *)block < (void *)((char *)zone->start + zone->zone_size)) {
					zone->blocks_allocated++;
					break;
				}
				zone = zone->next;
			}
		}
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
