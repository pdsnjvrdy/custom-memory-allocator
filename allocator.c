#include "allocator.h"
#include <unistd.h>   
#include <stddef.h>   

// This will point to the first block in our heap
block_meta *head = NULL;

// Simple function to round up size to the nearest multiple of 8
size_t align_size(size_t size) {
    return (size + 7) & ~7;
}

// When user gives us a pointer from my_malloc, we need to go back
// a few bytes to get the block_meta header
block_meta *get_header(void *ptr) {
    return (block_meta *)((char *)ptr - sizeof(block_meta));
}

void *my_malloc(size_t size) {
    (void)size;  // just to stop the compiler from giving warnings
    return NULL;
}

void my_free(void *ptr) {
    (void)ptr;   // just to stop the compiler from giving warnings
}