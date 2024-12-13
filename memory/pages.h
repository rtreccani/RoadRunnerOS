#pragma once
#include <stdint.h>
#include <stdbool.h>

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


void create_page_directory();
void create_page_table();
void* add_page_table_entry(int page_directory_idx, int page_table_idx);
void enable_paging();