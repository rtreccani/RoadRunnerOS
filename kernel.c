#include "basic_framebuffer.h"
#include "descriptor_tables.h"
#include "interrupts.h"
#include "soft_timer.h"
#include "keyboard.h"
#include "memory.h"
#include "pages.h"
#include "stdmem.h"

#define MAGIC_BREAK asm volatile ("xchgw %bx, %bx");

extern void* _kernel_start;
extern void* _kernel_end;
extern void* _kernel_size;

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	terminal_info_writestring("Booted RoadrunnerOS 1.0 \"meep meep\"\n", 0, 0);
	gdt_initialise();
	idt_initialise();
	irq_initialise();
	initialise_timers();
	initialise_keyboard();
	kenable_interrupts();
	uint32_t aligned_start = align_to((uint32_t)&_kernel_end, 4096);
	kmemory_space_assign((void*)aligned_start, 50000);
	create_page_directory();
	create_page_table(0);
	uint32_t* page = add_page_table_entry(0, 0);
	for(int i = 0; i < 1024; i++)
	{
		page[i] = i;
	}
	/* enable MMU here */
	enable_paging();
	for(;;);
}