extern interrupt_handler  ; A função em C que vamos criar no próximo passo!

section .text
align 4

; Macro para interrupções que NÃO geram código de erro
; Nós empurramos um 0 falso para a pilha ficar sempre do mesmo tamanho
%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword 0        ; Empurra um código de erro falso (dummy)
    push dword %1       ; Empurra o número da interrupção
    jmp common_interrupt_handler
%endmacro

; Macro para interrupções que JÁ geram código de erro (a CPU faz isso sozinha)
%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword %1       ; Empurra o número da interrupção
    jmp common_interrupt_handler
%endmacro

; Criando as funções para os primeiros erros da CPU (Exceções)
no_error_code_interrupt_handler 0  ; Divisão por zero
no_error_code_interrupt_handler 1  ; Debug
no_error_code_interrupt_handler 2  ; Non-maskable interrupt
no_error_code_interrupt_handler 3  ; Breakpoint
no_error_code_interrupt_handler 4  ; Overflow
no_error_code_interrupt_handler 5  ; Bound range exceeded
no_error_code_interrupt_handler 6  ; Invalid opcode
no_error_code_interrupt_handler 7  ; Device not available
error_code_interrupt_handler    8  ; Double fault (Essa a CPU passa erro!)

; Interrupção do Relógio (Timer - IRQ0)
no_error_code_interrupt_handler 32
; Interrupção do Teclado (Hardware não manda código de erro na pilha)
no_error_code_interrupt_handler 33

; ----------------------------------------------------
; O "Funil" - O escudo que protege o C
; ----------------------------------------------------
common_interrupt_handler:
    pushad              ; Salva os registradores (eax, ebx, ecx, edx, esi, edi, esp, ebp)

    ; Chama a nossa função em C (passando a stack atual como argumento)
    call interrupt_handler 

    popad               ; Restaura os registradores exatamente como estavam
    add esp, 8          ; Limpa os 8 bytes da pilha (código de erro + número da interrupção)
    iret                ; O retorno especial mágico!