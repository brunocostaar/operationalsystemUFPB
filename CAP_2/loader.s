global loader                   ; Símbolo de entrada para elf

    MAGIC_NUMBER equ 0x1BADB002     ; define a constante do número mágico
    FLAGS        equ 0x0            ; flags do multiboot
    CHECKSUM     equ -MAGIC_NUMBER  ; calcula o checksum
                                    ; (numéro mágico + checksum + flags deve ser = 0)

    section .text                   ; início do trecho de código
    align 4                         ; código deve ter 4 bytes alinhados
        dd MAGIC_NUMBER             ; escrevo o número mágico para código de máquina,
        dd FLAGS                    ; as flags,
        dd CHECKSUM                 ; e o checksum

    loader:                         ; a etiqueta para o loader começar a rodar o script
        mov edi, 0xB8000      	    ; Endereço para começar a pintura
        mov ecx, 80 * 25            ; O tamanho da tela (2000 caracteres)
        mov ax, 0x1F20              ; Para a tela ficar azul: 
                                    ;    0x20 = Caractere Espaço (limpa o texto)
                                    ;    0x1F = 1 (Fundo Azul) + F (Texto Branco)
        rep stosw                   ; Comando de repetição: Até terminar de 'pintar'
        
        mov eax, 0xCAFEBABE         ; coloco o número 0xCAFEBABE no registrador eax quando tudo finalizar
    .loop:
        jmp .loop                   ; loop infinito
