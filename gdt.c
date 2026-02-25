#include "gdt.h"

/* A estrutura que representa uma linha (entrada) da tabela GDT */
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

/* Criando um array com 3 entradas e o ponteiro da GDT */
struct gdt_entry gdt_entries[3];
struct gdt gdt_ptr;

/* Função auxiliar para preencher a matemática binária de cada linha */
void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

/* Função principal que inicializa tudo */
void init_gdt() {
    gdt_ptr.size = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_ptr.address = (unsigned int)&gdt_entries;

    /* 1. Descritor Nulo (Index 0) */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* 2. Segmento de Código do Kernel (Index 1) - Endereço base 0, Limite 4GB */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* 3. Segmento de Dados do Kernel (Index 2) - Endereço base 0, Limite 4GB */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Chama a função em Assembly que você criou no gdt.s! */
    load_gdt(&gdt_ptr);
}
// Created by victoria on 25/02/2026.
//