#include "tasks.h"
.intel_syntax noprefix
.global _switch_to_new_task
.extern old_task

_switch_to_new_task:
    mov [esp + EAX_OFFSET + 4], eax /* i think the first 4 bytes on the stack are the return address for this function. */
    mov eax, esp /* store the old EAX in the first struct passed in, then store the old ESP in the new EAX. */
    add eax, 4  /* i think this should mean eax now points to the beginning of the first struct (convenient innit) */
    mov [eax + EBX_OFFSET], ebx
    mov [eax + ECX_OFFSET], ecx /*snapshot all of the variable registers into the struct. */
    mov [eax + EDX_OFFSET], edx
    mov [eax + ESI_OFFSET], esi
    mov [eax + EDI_OFFSET], edi
    mov [eax + ESP_OFFSET], esp
    mov [eax + EBP_OFFSET], ebp
    mov ebx, cr3 /* take cr3 into ebx, and then move it to the old struct */
    mov [eax + PAGE_DIR_OFFSET], ebx
    /* now we need to move to the new task (eep) */

    mov eax, [esp + TASK_STATE_SIZE + 4]
    mov ebx, [eax + PAGE_DIR_OFFSET]
    mov cr3, ebx
    mov ebp, [eax + EBP_OFFSET]
    mov esp, [eax + ESP_OFFSET]
    mov edi, [eax + EDI_OFFSET]
    mov esi, [eax + ESI_OFFSET]
    mov edx, [eax + EDX_OFFSET]
    mov ecx, [eax + ECX_OFFSET]
    mov ebx, [eax + EBX_OFFSET]
    ret