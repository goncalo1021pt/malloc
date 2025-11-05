/*
 * Test: FT_MALLOC_PERTURB
 * 
 * Purpose: Demonstrates how perturb helps detect uninitialized memory bugs
 * 
 * Run with:
 *   LD_PRELOAD=../libft_malloc_x86_64.so FT_MALLOC_PERTURB=170 ./test_perturb
 * 
 * Expected behavior:
 *   - Allocated memory will be filled with 0xAA (170 in decimal)
 *   - Freed memory will be filled with 0x55 (~170)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_bytes(const char *label, unsigned char *ptr, size_t len) {
	printf("%s: ", label);
	for (size_t i = 0; i < len; i++) {
		printf("%02x ", ptr[i]);
	}
	printf("\n");
}

int main(void) {
	printf("=== FT_MALLOC_PERTURB Test ===\n\n");
	
	// Test 1: Uninitialized memory detection
	printf("Test 1: Uninitialized Memory\n");
	printf("-----------------------------\n");
	unsigned char *buf1 = malloc(16);
	printf("After malloc(16):\n");
	print_bytes("  Content", buf1, 16);
	printf("  Expected: AA AA AA AA... (if PERTURB=170)\n\n");
	
	// Write some data
	strcpy((char *)buf1, "Hello");
	printf("After strcpy(\"Hello\"):\n");
	print_bytes("  Content", buf1, 16);
	printf("  Notice: Uninitialized bytes still show 0xAA\n\n");
	
	// Test 2: Use-after-free detection
	printf("Test 2: Use-After-Free Detection\n");
	printf("---------------------------------\n");
	unsigned char *buf2 = malloc(16);
	strcpy((char *)buf2, "Test Data");
	printf("Before free:\n");
	print_bytes("  Content", buf2, 16);
	
	free(buf2);
	printf("After free:\n");
	print_bytes("  Content", buf2, 16);
	printf("  Expected: 55 55 55 55... (if PERTURB=170)\n");
	printf("  This helps detect use-after-free bugs!\n\n");
	
	// Test 3: Multiple allocations
	printf("Test 3: Multiple Allocations\n");
	printf("-----------------------------\n");
	unsigned char *ptr1 = malloc(8);
	unsigned char *ptr2 = malloc(8);
	unsigned char *ptr3 = malloc(8);
	
	printf("Three allocations of 8 bytes each:\n");
	print_bytes("  ptr1", ptr1, 8);
	print_bytes("  ptr2", ptr2, 8);
	print_bytes("  ptr3", ptr3, 8);
	printf("  All should be filled with 0xAA\n\n");
	
	// Test 4: Pattern verification
	printf("Test 4: Pattern Verification\n");
	printf("-----------------------------\n");
	unsigned char *test = malloc(32);
	int all_aa = 1;
	for (int i = 0; i < 32; i++) {
		if (test[i] != 0xAA) {
			all_aa = 0;
			break;
		}
	}
	
	if (all_aa) {
		printf("✓ SUCCESS: All bytes are 0xAA (perturb is working!)\n");
	} else {
		printf("  Note: Run with FT_MALLOC_PERTURB=170 to enable\n");
	}
	
	free(test);
	
	// Cleanup
	free(buf1);
	free(ptr1);
	free(ptr2);
	free(ptr3);
	
	printf("\n=== Test Complete ===\n");
	return 0;
}
