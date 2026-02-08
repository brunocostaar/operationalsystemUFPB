global loader                       ; Símbolo de entrada para elf
    extern kmain                    ; Declara que kmain está em outro lugar

    MAGIC_NUMBER equ 0x1BADB002     ; define a constante do número mágico
    FLAGS        equ 0x0            ; flags do multiboot
    CHECKSUM     equ -MAGIC_NUMBER  ; calcula o checksum
    
    KERNEL_STACK_SIZE equ 4096      ; Define a constante de 4KB para o tamanho do stack

    section .text                   ; início do trecho de código
    align 4                         ; código deve ter 4 bytes alinhados
        dd MAGIC_NUMBER             ; escrevo o número mágico para código de máquina,
        dd FLAGS                    ; as flags,
        dd CHECKSUM                 ; e o checksum

    loader:                         ; a etiqueta para o loader começar a rodar o script
        
        mov esp, kernel_stack + KERNEL_STACK_SIZE ; configurando o registrador esp (stack pointer) para o fim do stack, pois ele decresce.

        mov edi, 0xB8000            ; Endereço para começar a pintura
        mov ecx, 80 * 25            ; O tamanho da tela (2000 caracteres)
        mov ax, 0x1F20              ; Para a tela ficar azul
        rep stosw                   ; Comando de repetição: Até terminar de 'pintar'

        call kmain                  ; Chama a função principal em C
        
        mov eax, 0xCAFEBABE         ; coloco o número 0xCAFEBABE no registrador eax
    .loop:
        jmp .loop                   ; loop infinito

section .bss ; reservando memória na seção bss
align 4                             ; Alinhamento para performance
kernel_stack:                       ;
    resb KERNEL_STACK_SIZE          ; Reserva os bytes do stack