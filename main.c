#include "ft_malloc.h"
#include <stdlib.h>

void test_zone_freeing() {
	write(1, "\n=== Testing Zone Freeing ===\n", 31);
	
	void *tiny_ptrs[150];
	
	for (int i = 0; i < 150; i++) {
		tiny_ptrs[i] = malloc(50);
	}
	
	write(1, "After 150 allocations (2 zones):\n", 34);
	show_alloc_mem();
	
	for (int i = 100; i < 150; i++) {
		free(tiny_ptrs[i]);
	}
	
	write(1, "\nAfter freeing zone 2 (should be munmapped):\n", 46);
	show_alloc_mem();
	
	for (int i = 0; i < 100; i++) {
		free(tiny_ptrs[i]);
	}
	
	write(1, "\nAfter freeing all (zone 1 should remain):\n", 44);
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
