#ifndef FT_MALLOC_H
#define FT_MALLOC_H

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define TINY_MAX_SIZE 128
#define TINY_BLOCK_SIZE (sizeof(t_block) + TINY_MAX_SIZE)
#define TINY_ZONE_SIZE ROUND_UP_PAGE(sizeof(t_zone) + (TINY_BLOCK_SIZE * 100))

#define SMALL_MAX_SIZE 1024
#define SMALL_BLOCK_SIZE (sizeof(t_block) + SMALL_MAX_SIZE)
#define SMALL_ZONE_SIZE ROUND_UP_PAGE(sizeof(t_zone) + (SMALL_BLOCK_SIZE * 100))

#define ROUND_UP_PAGE(size) ((((size) + getpagesize() - 1) / getpagesize()) * getpagesize())

#define ALIGN(size) (((size) + 15) & ~15)

typedef struct s_block {
	size_t size;
	bool free;
	struct s_block *next;
	char data[1];
} t_block;

typedef struct s_zone {
	void *start;
	size_t zone_size;
	int block_count;
	int blocks_allocated;
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

// calloc.c
void *calloc(size_t nmemb, size_t size);

void show_alloc_mem(void);

// aux.c
void write_str(int fd, const char *str);
void write_hex(int fd, void *ptr);
void write_number(int fd, size_t num);	
size_t ft_strlen_local(const char *s);
void *ft_memcpy(void *dest, const void *src, size_t n);

#endif