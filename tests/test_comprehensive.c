/*
 * Test: All Features Combined
 * 
 * Purpose: Stress test with all features enabled
 * 
 * Run with:
 *   LD_PRELOAD=../libft_malloc_x86_64.so \
 *   FT_MALLOC_DEBUG=1 \
 *   FT_MALLOC_STATS=1 \
 *   FT_MALLOC_CHECK=1 \
 *   FT_MALLOC_PERTURB=170 \
 *   ./test_comprehensive
 * 
 * Expected behavior:
 *   - All debug features working together
 *   - No crashes or conflicts
 *   - Proper leak detection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show_alloc_mem_ex(void);

void test_tiny_allocations(void) {
	printf("\n=== TINY Allocations Test ===\n");
	void *ptrs[20];
	for (int i = 0; i < 20; i++) {
		ptrs[i] = malloc(i + 1);  // 1 to 20 bytes
	}
	
	// Free every other one
	for (int i = 0; i < 20; i += 2) {
		free(ptrs[i]);
	}
	
	// Reallocate to test reuse
	for (int i = 0; i < 20; i += 2) {
		ptrs[i] = malloc(i + 1);
	}
	
	// Cleanup
	for (int i = 0; i < 20; i++) {
		free(ptrs[i]);
	}
	printf("TINY test complete\n");
}

void test_small_allocations(void) {
	printf("\n=== SMALL Allocations Test ===\n");
	void *ptrs[10];
	for (int i = 0; i < 10; i++) {
		ptrs[i] = malloc(200 + i * 50);  // 200-650 bytes
	}
	
	// Free some
	free(ptrs[2]);
	free(ptrs[5]);
	free(ptrs[7]);
	
	// Reallocate
	ptrs[2] = malloc(250);
	ptrs[5] = malloc(450);
	
	// Cleanup
	for (int i = 0; i < 10; i++) {
		if (ptrs[i])
			free(ptrs[i]);
	}
	printf("SMALL test complete\n");
}

void test_large_allocations(void) {
	printf("\n=== LARGE Allocations Test ===\n");
	void *large1 = malloc(5000);
	void *large2 = malloc(10000);
	void *large3 = malloc(50000);
	
	memset(large1, 'A', 5000);
	memset(large2, 'B', 10000);
	memset(large3, 'C', 50000);
	
	free(large2);
	large2 = malloc(8000);
	
	free(large1);
	free(large2);
	free(large3);
	printf("LARGE test complete\n");
}

void test_realloc_patterns(void) {
	printf("\n=== Realloc Patterns Test ===\n");
	
	// Growing realloc
	char *str = malloc(10);
	strcpy(str, "Small");
	str = realloc(str, 100);
	strcat(str, " -> Medium");
	str = realloc(str, 1000);
	strcat(str, " -> Large");
	
	// Shrinking realloc
	str = realloc(str, 50);
	
	// Free
	free(str);
	printf("Realloc test complete\n");
}

void test_fragmentation(void) {
	printf("\n=== Fragmentation Test ===\n");
	void *ptrs[100];
	
	// Allocate many blocks
	for (int i = 0; i < 100; i++) {
		ptrs[i] = malloc((i % 10 + 1) * 16);
	}
	
	// Free odd indices
	for (int i = 1; i < 100; i += 2) {
		free(ptrs[i]);
	}
	
	// Reallocate with different sizes
	for (int i = 1; i < 100; i += 2) {
		ptrs[i] = malloc((i % 5 + 1) * 32);
	}
	
	// Show memory state
	printf("\nMemory state with fragmentation:\n");
	show_alloc_mem_ex();
	
	// Cleanup
	for (int i = 0; i < 100; i++) {
		free(ptrs[i]);
	}
	printf("Fragmentation test complete\n");
}

int main(void) {
	printf("========================================\n");
	printf("  COMPREHENSIVE MALLOC TEST SUITE\n");
	printf("========================================\n");
	
	test_tiny_allocations();
	test_small_allocations();
	test_large_allocations();
	test_realloc_patterns();
	test_fragmentation();
	
	printf("\n========================================\n");
	printf("  ALL TESTS COMPLETED\n");
	printf("========================================\n");
	printf("\nCheck statistics below for memory leaks!\n\n");
	
	return 0;
}
