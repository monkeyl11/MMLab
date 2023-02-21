
#include "umalloc.h"
#include "err_handler.h"
#include <stdio.h>


//Place any variables needed here from umalloc.c as an extern.
extern memory_block_t *free_head;

/*
 * check_heap -  used to check that the heap is still in a consistent state.
 *
 * STUDENT TODO:
 * Required to be completed for checkpoint 1:
 *
 *      - Ensure that the free block list is in the order you expect it to be in.
 *        As an example, if you maintain the free list in memory-address order,
 *        lowest address first, ensure that memory addresses strictly ascend as you
 *        traverse the free list.
 *
 *      - Check if any memory_blocks (free and allocated) overlap with each other. 
 *        Hint: Run through the heap sequentially and check that for some memory_block 
 *        n, memory_block n+1 has a sensible block_size and is within the valid heap addresses. 
 *        Consider tracking the lowest and highest heap addresses you have ever seen.
 *
 *      - Ensure that each memory_block is aligned. 
 * 
 * Should return 0 if the heap is still consistent, otherwise return a non-zero
 * return code. Asserts are also a useful tool here.
 */
int check_heap() {
    // Example heap check:
    // Check that all blocks in the free list are marked free.
    // If a block is marked allocated, return -1.
    memory_block_t *curr = free_head;
    memory_block_t *prev = NULL;
    //checking for alignment and also making sure allocated blocks arent here
    while (curr) {
        printf("Current free node size: %lu\n", curr->block_size_alloc);
        printf("Current free node address: %p\n", curr);
        printf("Current free node pointing to: %p\n\n", curr->next);
        if (is_allocated(curr)) { //checking to make sure block isn't allocated
            logging(LOG_ERROR, "allocated block in free list");
            return -1;
        }
        if (curr->block_size_alloc % ALIGNMENT != 0) { //checking to make sure block is aligned
            logging(LOG_ERROR, "improper alignment");
            return -1;
        }
        if (prev) { //checking to make sure there are no overlaps in allocation (free list is address-sorted)
            if (prev + prev->block_size_alloc/ALIGNMENT + 1 > curr) {
                logging(LOG_ERROR, "overlapping freed blocks");
            return -1;
            }
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}