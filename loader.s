global loader
extern kmain

; =========================
; Multiboot header (CAP 9)
; =========================
MAGIC_NUMBER equ 0x1BADB002
FLAGS        equ 0x0
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    cli
    mov esp, kernel_stack + KERNEL_STACK_SIZE

    mov edi, 0xB8000
    mov ecx, 80 * 25
    mov ax, 0x1F20
    rep stosw

    push ebx      ; 

call kmain

.loop:
    jmp .loop

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE
