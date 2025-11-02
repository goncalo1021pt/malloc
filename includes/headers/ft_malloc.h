#ifndef FT_MALLOC_H
#define FT_MALLOC_H

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/time.h>

#include "../libft/libft.h"

#define TINY_MAX_SIZE 128
#define SMALL_MAX_SIZE 1024
#define ALIGN(size) (((size) + 15) & ~15)

typedef struct s_block {
	size_t size;
	int free;
	struct s_block *next;
	char data[1];
} t_block;

typedef struct s_zone {
	void *start;
	size_t zone_size;
	size_t block_count;
	struct s_zone *next;
	t_block *blocks;
} t_zone;

typedef struct s_malloc_metadata {
	t_zone *tiny;
	t_zone *small;
	t_zone *large;
	pthread_mutex_t mutex;
} t_malloc_metadata;

extern t_malloc_metadata g_malloc_metadata;

// malloc.c
void *malloc(size_t size);

// free.c
void free(void *ptr);

// realloc.c
void *realloc(void *ptr, size_t size);

void show_alloc_mem(void);

#endif