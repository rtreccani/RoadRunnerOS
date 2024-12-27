#pragma once

#include <stdint.h>
#include <pages.h>

typedef struct
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t esp; /* pointer to tip of stack */
    uint32_t ebp; /* pointer to base of stack */
    uint32_t eip; /* pointer to current instruction */
    uint32_t eflags;
    page_directory_entry_t* page_dir; /* pointer to the page directory for this task */
} task_state_t;

#define OFFSETOF(type, member) ((int)__builtin_offsetof(type, member))
/* oh lawd forgive me for this one please */
#define EAX_OFFSET OFFSETOF(task_state_t, eax)
#define EBX_OFFSET OFFSETOF(task_state_t, ebx)
#define ECX_OFFSET OFFSETOF(task_state_t, ecx)
#define EDX_OFFSET OFFSETOF(task_state_t, edx)
#define ESI_OFFSET OFFSETOF(task_state_t, esi)
#define EDI_OFFSET OFFSETOF(task_state_t, edi)
#define ESP_OFFSET OFFSETOF(task_state_t, esp)
#define EBP_OFFSET OFFSETOF(task_state_t, ebp)
#define EIP_OFFSET OFFSETOF(task_state_t, eip)
#define PAGE_DIR_OFFSET OFFSETOF(task_state_t, page_dir)

#define TASK_STATE_SIZE sizeof(task_state_t)

typedef void (*entry_point_t)(void);


/**
 * @brief tbd
 * 
 */
extern void _switch_to_task(task_state_t* old, task_state_t* new);

/**
 * @brief save the current single-tasking state
 */
void initialise_multitasking();

/**
 * @brief create a new task (and switch to it )
 * @param new_task_entry function pointer for the main of the new task.
 * @return a pointer to a struct containing the task state of the new task
 */
task_state_t* create_new_task(entry_point_t new_task_entry);
