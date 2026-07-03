#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct block_meta {
    size_t size;      // total size of the block (includes header + data + footer)
    int is_free;      // 1 if free, 0 if allocated
} block_meta;

void *my_malloc(size_t size);
void my_free(void *ptr);

#endif