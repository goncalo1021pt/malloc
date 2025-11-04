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

int main() {
	atexit(cleanup_check);

	test_zone_freeing();
	return 0;
}
