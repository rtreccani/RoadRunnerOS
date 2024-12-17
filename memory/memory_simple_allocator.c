#include <stdint.h>
#include "basic_framebuffer.h"
#include "memory.h"

void* mem_space_begin = 0;
void* mem_space_end = 0;
uint32_t mem_space_size = 0;

#define MIN_FREE_SIZE_TO_SPLIT_ON 16
#define PREFIX_SIZE (sizeof(block_prefix_t))

inline void* round_down_to_page_address(void* fine_address)
{
    return (void*)((uint32_t)fine_address & (~4095));
}

inline void* round_up_to_page_address(void* fine_address)
{   /* if we add 4095 and round down, we're guaranteed to be rounding up. */
    return (void*)(((uint32_t)fine_address + 4095) & (~4095));
}

void kmemory_space_assign(void* start, uint32_t len)
{
    if(mem_space_begin != 0 || mem_space_size != 0 || mem_space_end != 0)
    {
        kprintf("%#tried to setup a kernel heap when one already exists!\n", COL_BG_ERROR, COL_FG_ERROR);
    }
    mem_space_begin = start;
    mem_space_size = len;
    mem_space_end = mem_space_begin + mem_space_size;
    block_prefix_t* first_block = (block_prefix_t*)mem_space_begin;
    first_block->is_block_free = FREE;
    first_block->block_length = mem_space_size - PREFIX_SIZE;
    kprintf("%#allocated a heap of size %d from %d to %d\n", COL_BG_INFO, COL_FG_INFO, (int)mem_space_size, (int)mem_space_begin, (int)mem_space_end);
}

static block_prefix_t* get_next_block_prefix(block_prefix_t* current_block)
{
    return (block_prefix_t*)((void*)current_block + PREFIX_SIZE + current_block->block_length);
}

void* kmemory_assign_page_aligned_chunk(uint32_t size_request)
{
    block_prefix_t* block_under_test = (block_prefix_t*)mem_space_begin;
    while((round_up_to_page_address((block_under_test + PREFIX_SIZE)) + size_request) < mem_space_end)
    {
        if(block_under_test->is_block_free == USED)
        {
            /* not free so we'll bump to the next block. */
            block_under_test = get_next_block_prefix(block_under_test);
        }
        else if(block_under_test->is_block_free == FREE)
        {
            void* nearest_4k_page_from_here = round_up_to_page_address((void*)block_under_test + PREFIX_SIZE);

            uint32_t offset_to_4k_page = nearest_4k_page_from_here -  ((void*)block_under_test + PREFIX_SIZE);

            /* we need to be able to shift to the 4k boundary, slap in an 8 byte prefix, and also fit in the request. */
            if(block_under_test->block_length < offset_to_4k_page + PREFIX_SIZE + size_request)
            { /* if we can't fit the block aligned to the 4k page, and the prefix, then skip. */
                block_under_test = get_next_block_prefix(block_under_test);
            } else {
                block_under_test->is_block_free = USED;
                if(offset_to_4k_page == 0)
                {
                    /* if the page is naturally 4k-aligned we can just use it as-is.*/
                    return (void*)block_under_test + PREFIX_SIZE;
                }
                /* create a new block aligned at the 4k boundary (with the prefix before the boundary) */
                block_prefix_t* new_aligned_block = round_up_to_page_address((void*)block_under_test + PREFIX_SIZE) - PREFIX_SIZE;
                new_aligned_block->is_block_free = USED;
                new_aligned_block->block_length = size_request;

                /* shrink the existing block til it touches the new aligned block. */
                uint32_t original_block_length = block_under_test->block_length;
                block_under_test->block_length = (uint32_t)new_aligned_block - ((uint32_t)block_under_test + PREFIX_SIZE);

                block_prefix_t* post_aligned_block = (block_prefix_t*)((void*)new_aligned_block + PREFIX_SIZE + size_request);
                post_aligned_block->is_block_free = FREE;
                post_aligned_block->block_length = original_block_length - PREFIX_SIZE - new_aligned_block->block_length - block_under_test->block_length;
                return (void*)((void*)new_aligned_block + PREFIX_SIZE);
            }
        }
    }
    return 0;
}

void* kmemory_assign_chunk(uint32_t size_request)
{
    block_prefix_t* block_under_test = (block_prefix_t*)mem_space_begin;
    while(((void*)block_under_test + PREFIX_SIZE + size_request) < mem_space_end)
    {
        if(block_under_test->is_block_free == USED)
        {
            /* not free so we'll bump to the next block. */
            block_under_test = get_next_block_prefix(block_under_test);
        }
        else if(block_under_test->is_block_free == FREE)
        {
            if(block_under_test->block_length < (size_request))
            {
                /* not big enough so we'll bump to the next block */
                block_under_test = get_next_block_prefix(block_under_test);
            } else
            {
                block_under_test->is_block_free = USED;
                block_prefix_t* new_block = ((block_prefix_t*)((void*)block_under_test + PREFIX_SIZE + size_request));
                /*new_block is the beginning of the new block which we're creating. */
                new_block->is_block_free = FREE;
                new_block->block_length = block_under_test->block_length - PREFIX_SIZE - size_request;
                block_under_test->block_length = size_request;

                return (void*)((uint32_t)block_under_test + PREFIX_SIZE);
            }
        }
        else
        {
            kprintf("%# encountered invalid heap prefix at %d\n", COL_BG_ERROR, COL_FG_ERROR, (uint32_t)block_under_test);
            return 0;
        }
    }
    kprintf("malloc failed!!!\n");
    for(;;);
    return 0;
}


/**
 * @brief walk the heap and recombine any fragmented free space.
 */
static void recombine_fragments()
{
    block_prefix_t* current_block = (block_prefix_t*)mem_space_begin;
    block_prefix_t* next_block = get_next_block_prefix(current_block);
    while((void*)current_block <= (void*)mem_space_end && (void*)next_block <= (void*)mem_space_end)
    {
        if(current_block->is_block_free == FREE && next_block->is_block_free  == FREE)
        {
            current_block->block_length += PREFIX_SIZE + next_block->block_length;
        }
        current_block = next_block;
        next_block = get_next_block_prefix(current_block);
    }
}

/**
 * @brief free a given block of memory that was previously requested via malloc.
 * @param the location of memory to be freed.
 * @return the size of the freed block. should always be at least the size of the allocated block, or 0 if an error occured.
 */
uint32_t kmemory_free_chunk(void* loc)
{
    block_prefix_t* block_under_test = (block_prefix_t*)mem_space_begin;
    while((void*)block_under_test <= loc){
        if((uint32_t)block_under_test + PREFIX_SIZE == (uint32_t)loc)
        {
            block_under_test->is_block_free = FREE;
            /* do recombination and stuff here */
            recombine_fragments();
            return block_under_test->block_length;
        }
        block_under_test = get_next_block_prefix(block_under_test);
    }
    kprintf("%#failed to release at %d!!\n", COL_BG_ERROR, COL_FG_ERROR, (uint32_t)loc);
    return 0;
}

