#pragma once
#include <stdint.h>

typedef enum
{
    FREE = 0xAAAAAAAA,
    USED = 0x55555555,
} block_status_t;

typedef struct
{
    block_status_t is_block_free;
    uint32_t block_length;
} block_prefix_t;

_Static_assert(sizeof(block_prefix_t) == 8, "block prefix heap size is wrong!");

/**
 * @brief align an address to the next highest multiple of an alignment value.
 * @param base the exact memory address
 * @param align the size to align it to.
 * @return the aligned address.
 */
uint32_t align_to(uint32_t base, uint32_t align);

/**
 * @brief round down an address to the nearest page address
 * @param fine_address the address to return.
 * @return fine_address, rounded down to the nearest multiple of 4096
 */
void* round_down_to_page_address(void* fine_address);

/**
 * @brief round up an address to the nearest page address
 * @param fine_address the address to return.
 * @return fine_address, rounded up to the nearest multiple of 4096
 */
void* round_up_to_page_address(void* fine_address);


/**
 * @brief tell the kernel to create a heap memory of a given size/location.
 * @param start a pointer to the beginning of the available memory for the heap.
 * @param len the size of the heap, in bytes.
 */
void kmemory_space_assign(void* start, uint32_t len);

/**
 * @brief assign a region of memory to the callee.
 * @param size the requested minimum length of memory to be allocated.
 * @return the pointer to the requested memory, else null to indicate the memory couldn't be allocated.
 */
void* kmemory_assign_chunk(uint32_t size);

/**
 * @brief assign a region of memory to the callee, with the start address guaranteed to be a multiple of 4096 bytes.
 * this is used for assigning page tables in the virtual memory manager, as the page directory and table use a 20 bit
 * pointer (with the bottom 12 bits assumed to be all 0's).
 * @param size the requested minimum length of memory to be allocated.
 * @return the pointer to the requested memory, else null to indicate the memory couldn't be allocated.
 */
void* kmemory_assign_page_aligned_chunk(uint32_t size);

/**
 * @brief free a given block of memory that was previously requested via malloc.
 * @param the location of memory to be freed.
 * @return the size of the freed block. should always be at least the size of the allocated block, or 0 if an error
 */
uint32_t kmemory_free_chunk(void* loc);

