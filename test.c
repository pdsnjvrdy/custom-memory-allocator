#include <stdio.h>
#include "allocator.h"

int main() {
    printf("Starting custom allocator test...\n");

    int *a = my_malloc(sizeof(int));
    if (a != NULL) {
        *a = 42;
        printf("Value at a: %d\n", *a);
    }

    char *b = my_malloc(100);
    if (b != NULL) {
        printf("Allocated 100 bytes at address: %p\n", b);
    }

    // Free a
    my_free(a);
    printf("Freed 'a' successfully.\n");

    // Allocate again - this should reuse the freed block
    int *c = my_malloc(sizeof(int));
    if (c != NULL) {
        *c = 99;
        printf("Value at c: %d\n", *c);
    }

    printf("Test finished.\n");
    return 0;
}