#ifndef FT_MALLOC_H
#define FT_MALLOC_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <pthread.h>
#include <sys/time.h>

#include "../libft/libft.h"

typedef struct s_block
{
	size_t          size;
	struct s_block *next;
	int             free;
	void            *ptr;
	char            data[1];
} t_block;

// malloc.c
void *malloc(size_t size);

// free.c
void free(void *ptr);

// realloc.c
void *realloc(void *ptr, size_t size);

void show_alloc_mem(void);

#endif