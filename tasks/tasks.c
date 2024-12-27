#include "tasks.h"
#include "pages.h"
#include "stdint.h"
#include "memory.h"
#include "basic_framebuffer.h"

task_state_t old_task;

/* use this to crash back into the kernel if the other task exits.
   in the future this can be expanded to clean up the old task and figure out what
   task to run next, but for now it's a crashpad. */
void crashpad_function()
{
    kprintf("survived the warp back from the other task!\n");
    for(;;);
}

void initialise_multitasking()
{
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;" : "=m"(old_task.page_dir) ::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(old_task.eflags)::"%eax");
    /* maybe save some more stuff here to restore later on */
}

task_state_t* create_new_task(entry_point_t new_main)
{
    task_state_t* new_task_state = (task_state_t*)kmemory_assign_chunk(sizeof(task_state_t));

    new_task_state->page_dir = create_page_directory();
    identity_map(new_task_state->page_dir, 0, (void*)(1024 * 1024)); /* just for now */

    /* this gives over 5 pages to the stack for this task. can be raised.
       at some point i would need to catch page faults caused by the stack growing
       past the amount i gave it. is that a stack overflow??? */
    for(int i = 0; i < 5; i++)
    {
        void* new_page = kmemory_assign_page_aligned_chunk(PAGE_SIZE);
        map_virtual_page_to_physical_page(new_task_state->page_dir,
                                          (void*)(UINT32_MAX - (i * PAGE_SIZE) - PAGE_SIZE/2),
                                          new_page);
    }
    new_task_state->esp = UINT32_MAX;
    new_task_state->ebp = UINT32_MAX; /* does it need anything on the stack before it starts?? idk */

    new_task_state->eax = 0;
    new_task_state->ebx = 0;
    new_task_state->ecx = 0;
    new_task_state->edx = 0;
    new_task_state->esi = 0;
    new_task_state->edi = 0;
    new_task_state->eflags = 0;
    new_task_state->eip = (uint32_t)new_main;

    return new_task_state;
}