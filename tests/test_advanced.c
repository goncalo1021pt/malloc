/*
 * Test: FT_MALLOC_CHECK and show_alloc_mem_ex
 * 
 * Purpose: Demonstrates double-free detection and extended memory view
 * 
 * Run with:
 *   LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_CHECK=1 ./test_advanced
 * 
 * Expected behavior:
 *   - CHECK: Detects and reports double-free attempts
 *   - show_alloc_mem_ex: Shows detailed memory map with hexdump
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration of bonus functions
void show_alloc_mem(void);
void show_alloc_mem_ex(void);

int main(void) {
	printf("=== Advanced Features Test ===\n\n");
	
	// Test 1: Normal memory operations
	printf("Test 1: Creating Test Data\n");
	printf("---------------------------\n");
	char *str1 = malloc(32);
	strcpy(str1, "Hello, World!");
	
	char *str2 = malloc(64);
	strcpy(str2, "This is a longer string for testing");
	
	int *numbers = malloc(sizeof(int) * 10);
	for (int i = 0; i < 10; i++) {
		numbers[i] = i * 100;
	}
	
	char *str3 = malloc(50);
	strcpy(str3, "Test data 123");
	
	printf("Created 4 allocations with various data\n\n");
	
	// Test 2: Show basic memory map
	printf("Test 2: Basic Memory Map\n");
	printf("------------------------\n");
	show_alloc_mem();
	printf("\n");
	
	// Test 3: Free some blocks to create fragmentation
	printf("Test 3: Creating Fragmentation\n");
	printf("-------------------------------\n");
	free(str2);
	printf("Freed str2 (middle block)\n\n");
	
	// Test 4: Show extended memory map with hexdump
	printf("Test 4: Extended Memory Map\n");
	printf("---------------------------\n");
	show_alloc_mem_ex();
	
	// Test 5: Double-free detection (if CHECK is enabled)
	printf("\nTest 5: Double-Free Detection\n");
	printf("------------------------------\n");
	printf("Attempting double-free on str3...\n");
	free(str3);
	printf("First free: OK\n");
	
	printf("Second free (should be detected if FT_MALLOC_CHECK=1):\n");
	free(str3);  // This should be detected!
	
	printf("\n");
	
	// Test 6: Realloc and show memory state
	printf("Test 6: Realloc Test\n");
	printf("--------------------\n");
	char *new_str = malloc(20);
	strcpy(new_str, "Before realloc");
	printf("Before: \"%s\" (20 bytes)\n", new_str);
	
	new_str = realloc(new_str, 100);
	printf("After realloc to 100 bytes\n\n");
	
	printf("Final Memory State:\n");
	printf("-------------------\n");
	show_alloc_mem_ex();
	
	// Cleanup
	printf("\nCleaning up remaining allocations...\n");
	free(str1);
	free(numbers);
	free(new_str);
	
	printf("\n=== Test Complete ===\n");
	return 0;
}
