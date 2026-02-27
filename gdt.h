#ifndef GDT_H
#define GDT_H

/* A estrutura que guarda o endereço e o tamanho da nossa tabela */
struct gdt {
    unsigned short size;
    unsigned int address;
} __attribute__((packed));

/* Avisando ao C que essas funções existem (uma em Assembly, outra em C) */
void load_gdt(struct gdt *gdt_ptr);
void init_gdt();

#endif