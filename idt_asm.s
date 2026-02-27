global idt_load
extern idtp         ; Avisa o Assembly que 'idtp' foi criada no arquivo C

section .text
align 4

idt_load:
    lidt [idtp]     ; Carrega o ponteiro de 6 bytes da IDT no processador
    ret             ; Retorna para o código C