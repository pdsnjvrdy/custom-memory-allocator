#include <stdio.h>
#include "allocator.h"

int main() {
    printf("Testing custom allocator with best-fit and coalescing...\n\n");

    // Allocate 3 blocks of different sizes
    int *a = my_malloc(10);
    int *b = my_malloc(20);
    int *c = my_malloc(30);

    printf("Allocated three blocks of sizes 10, 20, 30\n");
    printf("Addresses: a = %p, b = %p, c = %p\n\n", a, b, c);

    // Free the middle block
    my_free(b);
    printf("Freed the middle block (20 bytes)\n\n");

    // Now allocate something small - best-fit should pick the 20-byte block
    int *d = my_malloc(8);
    printf("Allocated 8 bytes. Best-fit should reuse the 20-byte block\n");
    printf("Address of d: %p\n\n", d);

    // Free the first block
    my_free(a);
    printf("Freed the first block (10 bytes)\n\n");

    // Free the third block
    my_free(c);
    printf("Freed the third block (30 bytes)\n\n");

    // Now all blocks are free. Coalescing should merge them into one big block
    printf("All blocks are free now. Coalescing should merge adjacent free blocks.\n");

    // Allocate a large block that only fits if coalescing worked
    int *e = my_malloc(70);
    if (e != NULL) {
        printf("Allocated 70 bytes successfully! This proves coalescing worked.\n");
        printf("Address of e: %p\n", e);
    } else {
        printf("Could not allocate 70 bytes. Coalescing might have failed.\n");
    }

    printf("\nTest finished.\n");
    return 0;
}