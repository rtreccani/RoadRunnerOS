.intel_syntax noprefix
.global _enable_paging
.extern page_directory

_enable_paging:
    mov eax, page_directory
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    ret