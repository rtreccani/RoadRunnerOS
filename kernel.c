#include "basic_framebuffer.h"
#include "descriptor_tables.h"
#include "interrupts.h"
#include "soft_timer.h"
#include "keyboard.h"
#include "memory.h"
#include "pages.h"
#include "stdmem.h"
#include "tasks.h"

#define MAGIC_BREAK asm volatile ("xchgw %bx, %bx");

extern void* _kernel_start;
extern void* _kernel_end;

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

	uint32_t aligned_start = (uint32_t)round_up_to_page_address(&_kernel_end);
	kmemory_space_assign((void*)aligned_start, 0x8000000);

	page_directory_entry_t* system_directory = create_page_directory();

	identity_map(system_directory, 0, &_kernel_end);

	// /* enable MMU here */
	set_active_page_directory(system_directory);
	initialise_multitasking();

	void bar()
	{
		kprintf("WE SURVIVED YEAHH\n");
	}

	entry_point_t bar_entry = &bar;
	create_new_task(bar_entry);

	for(;;);
}