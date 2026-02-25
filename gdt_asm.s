global load_gdt

; Função que o C vai chamar para carregar a GDT
; stack: [esp + 4] contém o endereço da struct gdt que criamos no Passo 2
load_gdt:
    mov eax, [esp + 4]  ; Pega o endereço da nossa struct
    lgdt [eax]          ; Carrega a GDT no processador!

    ; Atualiza os registradores de dados para o offset 0x10 (Kernel Data)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Atualiza o registrador de código (CS) fazendo um "far jump" para 0x08 (Kernel Code)
    jmp 0x08:flush_cs

flush_cs:
    ret                 ; Volta para o código em C
