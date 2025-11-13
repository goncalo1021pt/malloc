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

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BOLD    "\033[1m"

#define MAX_HEXDUMP_BYTES 64

typedef struct s_block {
	size_t size;
	bool free;
	struct s_block *next;
	struct s_block *prev;
	struct s_block *free_next;
	char data[1];
} t_block;

typedef struct s_zone {
	void *start;
	size_t zone_size;
	int blocks_allocated;
	int blocks_free;
	struct s_zone *next;
	t_block *blocks;
	t_block *free_list;
	void *zone_end;
} t_zone;

typedef struct s_malloc_metadata {
	t_zone *tiny;
	t_zone *small;
	t_zone *large;
	pthread_mutex_t mutex;
} t_malloc_metadata;

typedef struct s_stats {
	size_t total_zones;
	size_t tiny_zones;
	size_t small_zones;
	size_t large_zones;
	size_t allocated_bytes;
	size_t allocated_blocks;
	size_t free_bytes;
	size_t free_blocks;
} t_stats;

typedef struct s_debug_config {
	bool initialized;
	bool debug;           // FT_MALLOC_DEBUG - verbose logging
	bool stats;           // FT_MALLOC_STATS - show stats on exit
	bool check;           // FT_MALLOC_CHECK - double-free detection
	int perturb;          // FT_MALLOC_PERTURB - fill pattern (0-255, -1 = disabled)
	size_t total_mallocs;
	size_t total_frees;
	size_t total_reallocs;
} t_debug_config;

extern t_malloc_metadata g_malloc_metadata;
extern t_debug_config g_debug_config;

// malloc.c
void *malloc(size_t size);
void add_to_free_list(t_zone *zone, t_block *block);
void remove_from_free_list(t_zone *zone, t_block *block);

// free.c
void free(void *ptr);

// realloc.c
void *realloc(void *ptr, size_t size);

// calloc.c
void *calloc(size_t nmemb, size_t size);

void show_alloc_mem(void);
void show_alloc_mem_ex(void);

// debug.c
void init_debug_config(void);
void debug_log_malloc(void *ptr, size_t size);
void debug_log_free(void *ptr);
void debug_log_realloc(void *old_ptr, void *new_ptr, size_t size);
void debug_perturb_alloc(void *ptr, size_t size);
void debug_perturb_free(void *ptr, size_t size);
bool debug_check_double_free(t_block *block);
void debug_free_invalid_pointer(void *ptr);
void debug_show_stats(void);

// aux.c
void write_str(int fd, const char *str);
void write_hex(int fd, void *ptr);
void write_number(int fd, size_t num);	
size_t ft_strlen_local(const char *s);
void *ft_memcpy(void *dest, const void *src, size_t n);
void write_color(int fd, const char *color);
int is_printable(unsigned char c);
void write_byte_hex(int fd, unsigned char byte);
void print_ascii(int fd, unsigned char *data, size_t len);
void print_hexdump(int fd, void *ptr, size_t size, size_t max_bytes);

#endif