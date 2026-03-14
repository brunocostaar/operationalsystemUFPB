global loader
extern kmain

; =========================
; Cabeçalho Multiboot (Exigência do GRUB)
; =========================
MAGIC_NUMBER equ 0x1BADB002
FLAGS        equ 0x0
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096 ; Tamanho da pilha (4 KB)

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    cli ; Desliga interrupções de hardware até o SO estar 100% pronto

    ; Configura o ponteiro da pilha (stack pointer) da CPU
    mov esp, kernel_stack + KERNEL_STACK_SIZE

    ; Pinta a tela de azul (limpa o framebuffer usando a porta 0xB8000)
    mov edi, 0xB8000
    mov ecx, 80 * 25
    mov ax, 0x1F20
    rep stosw

    ; Empurra APENAS o registrador ebx na pilha.
    ; O GRUB salva no ebx o ponteiro para o mapa de memória (multiboot_info).
    ; O seu código C vai puxar isso como o argumento 'unsigned int ebx'.
    push ebx      

    ; Salta para o código em C!
    call kmain

.loop:
    jmp .loop ; Se o C der crash ou retornar, a CPU entra em loop infinito aqui

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE ; Reserva fisicamente os 4 KB na memória para a pilha
