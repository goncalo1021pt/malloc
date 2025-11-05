/*
 * Test: FT_MALLOC_DEBUG and FT_MALLOC_STATS
 * 
 * Purpose: Demonstrates debug logging and statistics tracking
 * 
 * Run with:
 *   LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_DEBUG=1 ./test_debug
 *   LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_STATS=1 ./test_debug
 *   LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_DEBUG=1 FT_MALLOC_STATS=1 ./test_debug
 * 
 * Expected behavior:
 *   - DEBUG: Shows every malloc/free/realloc operation
 *   - STATS: Shows summary statistics on program exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	printf("=== Debug and Statistics Test ===\n\n");
	
	// Test various allocation patterns
	printf("Test 1: Basic Allocations\n");
	printf("-------------------------\n");
	void *p1 = malloc(32);
	void *p2 = malloc(64);
	void *p3 = malloc(128);
	printf("Allocated 3 blocks (32, 64, 128 bytes)\n\n");
	
	printf("Test 2: Free and Reuse\n");
	printf("----------------------\n");
	free(p2);
	printf("Freed middle block\n");
	void *p4 = malloc(64);
	printf("Allocated same size - should reuse freed block\n\n");
	
	printf("Test 3: Realloc\n");
	printf("---------------\n");
	void *p5 = malloc(16);
	strcpy(p5, "Small");
	printf("Original allocation: 16 bytes\n");
	
	p5 = realloc(p5, 256);
	printf("Reallocated to 256 bytes\n\n");
	
	printf("Test 4: Multiple Allocations\n");
	printf("----------------------------\n");
	void *ptrs[10];
	for (int i = 0; i < 10; i++) {
		ptrs[i] = malloc(i * 10 + 8);
	}
	printf("Allocated 10 blocks of varying sizes\n\n");
	
	printf("Test 5: Free Half\n");
	printf("-----------------\n");
	for (int i = 0; i < 10; i += 2) {
		free(ptrs[i]);
	}
	printf("Freed every other block (5 blocks)\n\n");
	
	printf("Test 6: Memory Leak Test\n");
	printf("-------------------------\n");
	void *leak1 = malloc(100);
	void *leak2 = malloc(200);
	printf("Allocated 2 blocks that won't be freed (intentional leak)\n\n");
	
	// Cleanup most allocations
	printf("Cleaning up...\n");
	free(p1);
	free(p3);
	free(p4);
	free(p5);
	for (int i = 1; i < 10; i += 2) {
		free(ptrs[i]);
	}
	
	printf("\n=== Test Complete ===\n");
	printf("If FT_MALLOC_STATS=1, statistics will appear below:\n");
	printf("(Should show 2 memory leaks: leak1 and leak2)\n\n");
	
	// Note: leak1 and leak2 are intentionally not freed
	// to demonstrate leak detection
	(void)leak1;
	(void)leak2;
	
	return 0;
}
