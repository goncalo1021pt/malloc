#include "ft_malloc.h"
#include <stdio.h>
#include <string.h>

int main(void) {
	printf("=== Double Free Detection Test ===\n\n");
	
	// Allocate a block
	char *data = malloc(100);
	strcpy(data, "This is some test data.");
	printf("Allocated data at %p: %s\n", data, data);
	
	// Free the block
	printf("Freeing data at %p (first time)\n", data);
	free(data);
	
	// Attempt to double-free
	printf("Freeing data at %p (second time - should detect double-free)\n", data);
	free(data);  // This should be detected!

	
	printf("\n=== Test Complete ===\n");
	return 0;
}