#include "ft_malloc.h"
#include <stdlib.h>

void test_zone_freeing() {
	write(1, "\n=== Testing Zone Freeing ===\n", 31);
	void *ptrs[150];
	for (int i = 0; i < 150; i++) {
		ptrs[i] = malloc(10);
	}
	
	write(1, "\n=== Allocated 150 tiny blocks of 10 bytes each ===\n", 45);
	show_alloc_mem();
	for (int i = 0; i < 150; i++) {
		free(ptrs[i]);
	}
	for (int i = 0; i < 150; i++) {
		ptrs[i] = malloc(100);
	}

	write(1, "\n=== Allocated 150 tiny blocks of 100 bytes each ===\n", 45);
	show_alloc_mem();
	for (int i = 0; i < 150; i++) {
		free(ptrs[i]);
	}
	write(1, "\n=== After freeing all tiny blocks ===\n", 38);
	show_alloc_mem();
}

void cleanup_check(void) {
	write(1, "\n=== Memory state at program exit ===\n", 39);
	show_alloc_mem();
}

#define NP 10000
int main() {
	atexit(cleanup_check);

	void *ptrs[NP];
	for (int i = 0; i < NP; i++) {
		ptrs[i] = malloc(200);
	}

	write(1, "\n=== After initial allocations ===\n", 35);
	// show_alloc_mem();
	for (int i = 0; i < NP; i++) {
		free(ptrs[i]);
	}
	for (int i = 0; i < NP; i++) {
		ptrs[i] = malloc(200);
	}
	write(1, "\n=== After reallocations ===\n", 29);
	// show_alloc_mem();
	for (int i = 0; i < NP; i++) {
		free(ptrs[i]);
	}
	// free(ptr1);
	// free(ptr2);
	// free(ptr3);
	// test_zone_freeing();
	return 0;
}
