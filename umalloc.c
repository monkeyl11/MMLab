#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <assert.h>
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "ANDREW LU ayl538" ANSI_RESET;

/*
 * The following helpers can be used to interact with the memory_block_t
 * struct, they can be adjusted as necessary.
 */

// A sample pointer to the start of the free list.
memory_block_t *free_head;
memory_block_t *prev_block;

/*
 * is_allocated - returns true if a block is marked as allocated.
 */
bool is_allocated(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & 0x1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc |= 0x1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(memory_block_t *block) {
    assert(block != NULL);
    block->block_size_alloc &= ~0x1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(memory_block_t *block) {
    assert(block != NULL);
    return block->block_size_alloc & ~(ALIGNMENT-1);
}

/*
 * get_next - gets the next block.
 */
memory_block_t *get_next(memory_block_t *block) {
    assert(block != NULL);
    return block->next;
}

/*
 * put_block - puts a block struct into memory at the specified address.
 * Initializes the size and allocated fields, along with NUlling out the next 
 * field.
 */
void put_block(memory_block_t *block, size_t size, bool alloc) {
    assert(block != NULL);
    assert(size % ALIGNMENT == 0);
    assert(alloc >> 1 == 0);
    block->block_size_alloc = size | alloc;
    block->next = NULL;
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(memory_block_t *block) {
    assert(block != NULL);
    return (void*)(block + 1);
}

/*
 * get_block - given a payload, returns the block.
 */
memory_block_t *get_block(void *payload) {
    assert(payload != NULL);
    return ((memory_block_t *)payload) - 1;
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required. 
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
memory_block_t *find(size_t size) {
    memory_block_t* currBlock = free_head;
    while(currBlock) {
        if (get_size(currBlock) >= size) {
            return currBlock;
        }
        currBlock = get_next(currBlock);
        prev_block = currBlock;
    }
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
memory_block_t *extend(size_t size) {
    memory_block_t* newHead = csbrk(ALIGN(size) + ALIGNMENT);
    put_block(newHead, ALIGN(size) - ALIGNMENT, false);
    newHead->next = free_head;
    free_head = newHead;
    return free_head;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
memory_block_t *split(memory_block_t *block, size_t size) {
    size = ALIGN(size); //add padding
    size_t sizeDiff = get_size(block) - size - (sizeof(memory_block_t));
    block->block_size_alloc = sizeDiff;
    memory_block_t* newBlock = (memory_block_t*)((char*)block + sizeDiff + sizeof(memory_block_t));
    put_block(newBlock, size, true);
    return newBlock;

}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
memory_block_t *coalesce(memory_block_t *block) {
    //* STUDENT TODO
    return NULL;
}



/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    size_t initialSize = 4 * PAGESIZE;
    free_head = csbrk(initialSize); //bruh idk how much i need
    put_block(free_head, initialSize - ALIGNMENT, false);
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size) {
    prev_block = NULL;
    memory_block_t* memBlock = find(size);
    if (!memBlock) {
        memBlock = extend(size);
    }
    if (get_size(memBlock) > size + ((ALIGNMENT * 10))) {
        memBlock = split(memBlock, size);
        allocate(memBlock);
        return get_payload(memBlock);
    }
    else {
        if (prev_block) {
            prev_block->next = memBlock->next;
        } 
        else {
            free_head = memBlock->next;
        }
        allocate(memBlock);
        return get_payload(memBlock);
    }
    return NULL;
}

/*
 * ufree -  frees the memory space pointed to by ptr, which must have been called
 * by a previous call to malloc.
 */
void ufree(void *ptr) {
    memory_block_t* p = get_block(ptr);
    if (is_allocated(p)) {
        deallocate(p);
        p->next = free_head;
        free_head = p;
    }
}