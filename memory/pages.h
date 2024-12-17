#pragma once
#include <stdint.h>
#include <stdbool.h>

#define PAGE_DIR_SPAN    0b00000000000000000000001111111111
#define PAGE_TABLE_SPAN  0b00000000000000000000001111111111
#define PAGE_OFFSET_SPAN 0b00000000000000000000111111111111

typedef struct __attribute__((packed))
{
    bool present : 1;
    bool write_allowed : 1;
    bool user_mode : 1;
    bool page_level_writethrough : 1;
    bool page_level_cache_disable : 1;
    bool accessed : 1;
    bool dirty : 1;
    bool pat_supported: 1;
    bool global_translation : 1;
    uint8_t _unused_bits : 3;
    uint32_t page_frame_addr : 20;
} page_table_entry_t;

_Static_assert(sizeof(page_table_entry_t) == 4, "page table entry size is wrong :(");

typedef struct __attribute__((packed))
{
    bool present : 1;
    bool write_allowed : 1;
    bool user_mode : 1;
    bool page_level_writethrough : 1;
    bool page_level_cache_disable : 1;
    bool accessed : 1;
    bool _unused_bit : 1;
    bool _must_be_false : 1;
    uint8_t _unused_bits : 4;
    uint32_t page_table_addr : 20;
} page_directory_entry_t;

_Static_assert(sizeof(page_directory_entry_t) == 4, "page directory entry size is wrong :(");

/**
 * @brief create a new empty page directory.
 * this function creates a 4kB block of page_directory_entry_t elements at 1024 * 4B elements.
 * they are guaranteed to be all have present=false, but no other setting is done. 
 * @return a pointer to a block of 1024 page_directory_entry_t elements.
 */
page_directory_entry_t* create_page_directory();

/**
 * @brief set the global pointer to the active page directory to a given page directory pointer.
 * @param active_directory a pointer to a page directory.
 * note that this doesn't change the directory on a hardware level. (API tbd for this).
 */
void set_active_page_directory(page_directory_entry_t* active_directory);

/**
 * create a new page table and attach it to a directory at a given index.
 * @param directory the directory to attach the table to.
 * @param directory_idx the index of that directory at which the page table should be mapped.
 * @return pointer to the new table in memory.
 */
page_table_entry_t* create_new_page_table(page_directory_entry_t* directory, int directory_idx);

/**
 * @brief map a page in virtual address space to one in physical address space.
 * @param directory the directory under which the mapping should occur.
 * @param the virtual address to be mapped.
 * @param the physical address to be mapped onto.
 * note the addresses need not be page-aligned as the function will round down to the nearest page boundary for you.
 * they also don't need to share a common page offset, ie: map 4097-> 123 will work.
 */
void map_virtual_page_to_physical_page(page_directory_entry_t* directory, void* virtual_page, void* physical_page);

/**
 * @brief map all the addresses of the kernel to themselves to stop the processor shitting the bed (technical term) when paging is enabled.
 * @param directory the directory in which the identity mapping occurs.
 * @param start_address pointer to somewhere in the first page to be protected
 * @param end_address pointer to somewhere in the last page to be protected.
 * this is used to protect ie: the stack and interrupts etc.. from being mapped to nothing.
 * If you don't do this, you'll get a double-fault exception.
 */
void identity_map(page_directory_entry_t* directory, void* start_address, void* end_address);

/**
 * @brief enable paging and set the CR3 register to point to the value in @var page_directory
 */
void enable_paging();