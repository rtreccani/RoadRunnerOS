#include "pages.h"
#include "memory.h"
#include "basic_framebuffer.h"
#include "memory_internal.h"

page_directory_entry_t* page_directory;

void set_active_page_directory(page_directory_entry_t* active_directory)
{
    page_directory = active_directory;
}

int get_page_directory_offset(void* address)
{
    return (((uint32_t)address) >> 22) & PAGE_DIR_SPAN;
}

int get_page_table_offset(void* address)
{
    return (((uint32_t)address) >> 12) & PAGE_TABLE_SPAN;
}

void* get_pointer_from_page_address(uint32_t page_address)
{
    return (void*)(page_address << 12);
}

uint32_t get_page_address_from_pointer(void* p_page)
{
    return ((uint32_t)(p_page)) >> 12;
}

page_directory_entry_t* create_page_directory()
{
    page_directory_entry_t* empty_directory = kmemory_assign_page_aligned_chunk(sizeof(page_directory_entry_t) * 1024);
    for(int i = 0; i < 1024; i++)
    {
        empty_directory[i].present = false;
    }
    return empty_directory;
}

page_table_entry_t* create_new_page_table(page_directory_entry_t* directory, int directory_idx)
{
    if(directory[directory_idx].present == true)
    {
        return (page_table_entry_t*) get_pointer_from_page_address(directory[directory_idx].page_table_addr);
    } else {
        page_table_entry_t* empty_table = kmemory_assign_page_aligned_chunk(sizeof(page_table_entry_t) * 1024);
        directory[directory_idx].present = true;
        directory[directory_idx].page_table_addr = get_page_address_from_pointer(empty_table);
        for(int i = 0; i < 1024; i++)
        {
            empty_table[i].present = false;
        }
        return empty_table;
    }
}

void map_virtual_page_to_physical_page(page_directory_entry_t* directory, void* virtual_page, void* physical_page)
{
    int directory_idx = get_page_directory_offset(virtual_page);
    int table_idx = get_page_table_offset(virtual_page);

    if(directory[directory_idx].present == false)
    {
        create_new_page_table(directory, directory_idx);
    }

    page_table_entry_t* needed_page_table = (page_table_entry_t*)get_pointer_from_page_address(directory[directory_idx].page_table_addr);
    needed_page_table[table_idx].present = true;
    needed_page_table[table_idx].page_frame_addr = get_page_address_from_pointer(physical_page);
}

void identity_map(page_directory_entry_t* directory, void* start_address, void* end_address)
{
    void* start_page_aligned = round_down_to_page_address(start_address);
    void* end_page_aligned = round_down_to_page_address(end_address);
    for(void* p_page = start_page_aligned; p_page <= end_page_aligned; p_page+=4096)
    {
        map_virtual_page_to_physical_page(directory, p_page, p_page);
    }
}


void enable_paging()
{
    _enable_paging();
}