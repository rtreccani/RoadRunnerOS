#include "pages.h"
#include "memory.h"
#include "basic_framebuffer.h"
#include "memory_internal.h"

page_directory_entry_t* page_directory;

void create_page_directory()
{
    page_directory = kmemory_assign_chunk(sizeof(page_directory_entry_t) * 1024);
    for(int i = 0; i < 1024; i++)
    {
        page_directory[i].present = false;
    }
}

void catch_uninitalised_page_directory()
{
    if(!page_directory)
    {
        kprintf("page directory not initialised\n");
        for(;;);
    }
}

void create_page_table(int page_directory_idx)
{
    page_table_entry_t* page_table = kmemory_assign_chunk(sizeof(page_table_entry_t) * 1024);
    for(int i = 0; i < 1024; i++)
    {
        page_table[i].present = false;
    }
    catch_uninitalised_page_directory();
    page_directory[page_directory_idx].page_table_addr = ((uint32_t)page_table) >> 12;
}

void* add_page_table_entry(int page_directory_idx, int page_table_idx)
{
    (void)page_directory_idx;
    (void)page_table_idx;
    uint32_t page_addr = ((uint32_t)kmemory_assign_chunk(4096)) >> 12;
    catch_uninitalised_page_directory();
    uint32_t page_table_addr = ((uint32_t)page_directory[page_directory_idx].page_table_addr) << 12;
    page_table_entry_t* page_table = (page_table_entry_t*)page_table_addr;
    page_table->page_frame_addr = page_addr;
    page_table->present = true;
    return (void*)(page_addr << 12);
}

void identity_map_kernel(void* kernel_start, void* kernel_end)
{
    kernel_start = kernel_start & 0xFFFFF000;
    kernel_end = (kernel_end & 0xFFFFF000) + 0x00001000;
    kernel_page = kernel_start;
    while(kernel_page < kernel_end)
    {
        uint32_t page_directory_idx = (((uint32_t)kernel_page) & 0xFFC00000) >> 22;
        uint32_t page_table_idx = (((uint32_t)kernel_page) &     0x003FF000) >> 12;
        catch_uninitalised_page_directory();
        if(!page_directory[page_directory_idx])
        {
            add_page_table_entry(page_directory_idx, 
        }
    }
}

void enable_paging()
{
    _enable_paging();
}