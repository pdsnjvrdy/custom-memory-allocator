#include "allocator.h"
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>

#define BLOCK_META_SIZE sizeof(block_meta)

block_meta *head = NULL;
void *heap_end = NULL;

// Round up to nearest multiple of 8
size_t align_size(size_t size) {
    return (size + 7) & ~7;
}

block_meta *get_header(void *ptr) {
    return (block_meta *)((char *)ptr - BLOCK_META_SIZE);
}

block_meta *get_footer(block_meta *block) {
    return (block_meta *)((char *)block + block->size - BLOCK_META_SIZE);
}

// This now does BEST-FIT instead of first-fit
block_meta *find_free_block(size_t size) {
    if (heap_end == NULL) {
        return NULL;
    }

    block_meta *curr = head;
    block_meta *best = NULL;
    size_t smallest_diff = ~0; // largest possible size_t value

    while (curr != NULL && (char *)curr < (char *)heap_end) {
        if (curr->is_free == 1 && curr->size >= size) {
            // Check if this is the smallest fit so far
            if (curr->size - size < smallest_diff) {
                smallest_diff = curr->size - size;
                best = curr;
                // If we find an exact match, we can stop searching
                if (smallest_diff == 0) {
                    break;
                }
            }
        }
        // Move to the next block
        curr = (block_meta *)((char *)curr + curr->size);
    }
    return best;
}

// Ask the OS for more memory
block_meta *request_memory(size_t size) {
    void *old_brk = sbrk(0);
    block_meta *new_block = sbrk(size);

    if ((void *)new_block == (void *)-1) {
        return NULL; // sbrk failed
    }

    heap_end = (char *)old_brk + size;

    new_block->size = size;
    new_block->is_free = 1;

    block_meta *footer = get_footer(new_block);
    footer->size = size;
    footer->is_free = 1;

    return new_block;
}

void *my_malloc(size_t size) {
    size = align_size(size);
    if (size == 0) {
        return NULL;
    }

    // Total block size = user data + header + footer
    size_t total_block_size = size + (2 * BLOCK_META_SIZE);
    total_block_size = align_size(total_block_size);

    // Try to find a free block
    block_meta *block = find_free_block(total_block_size);

    // If no free block found, request new memory from OS
    if (block == NULL) {
        block = request_memory(total_block_size);
        if (block == NULL) {
            return NULL;
        }
        // If this is the first block, set the head
        if (head == NULL) {
            head = block;
        }
    }

    // Mark the block as allocated
    block->is_free = 0;
    block_meta *footer = get_footer(block);
    footer->is_free = 0;

    // We need at least header+footer+8 bytes for the new block, so ...
    if (block->size >= total_block_size + BLOCK_META_SIZE + 8) {
        block_meta *new_free = (block_meta *)((char *)block + total_block_size);
        size_t remaining = block->size - total_block_size;

        new_free->size = remaining;
        new_free->is_free = 1;

        block_meta *new_free_footer = get_footer(new_free);
        new_free_footer->size = remaining;
        new_free_footer->is_free = 1;

        // Shrink the current block
        block->size = total_block_size;
        footer = get_footer(block);
        footer->size = total_block_size;
        footer->is_free = 0;
    }

    return (void *)((char *)block + BLOCK_META_SIZE);
}

// COALESCING
void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    block_meta *block = get_header(ptr);

    // Mark the block as free
    block->is_free = 1;
    block_meta *footer = get_footer(block);
    footer->is_free = 1;

    // Now we coalesce: merge with the next block if it is free
    block_meta *next_block = (block_meta *)((char *)block + block->size);
    if ((char *)next_block < (char *)heap_end && next_block->is_free == 1) {
        // Merge current block with next block
        size_t new_size = block->size + next_block->size;
        block->size = new_size;
        footer = get_footer(block);
        footer->size = new_size;
        footer->is_free = 1;
    }

    block_meta *curr = head;
    block_meta *prev_block = NULL;

    while (curr != NULL && (char *)curr < (char *)heap_end) {
        if ((char *)((char *)curr + curr->size) == (char *)block) {
            prev_block = curr;
            break;
        }
        curr = (block_meta *)((char *)curr + curr->size);
    }

    if (prev_block != NULL && prev_block->is_free == 1) {
        // Merge prev block with current block (which is now free)
        size_t new_size = prev_block->size + block->size;
        prev_block->size = new_size;
        footer = get_footer(prev_block);
        footer->size = new_size;
        footer->is_free = 1;
        // Block is now absorbed into prev_block
    }
}