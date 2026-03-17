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

/* Criando um array com 6 entradas e o ponteiro da GDT */
struct gdt_entry gdt_entries[6];
struct gdt gdt_ptr;
struct tss_entry_struct tss_entry;

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

void set_kernel_stack(unsigned int stack) {
    tss_entry.esp0 = stack;
}

void write_tss(int num, unsigned short ss0, unsigned int esp0) {
    // Calculando a base e o limite do TSS
    unsigned int base = (unsigned int) &tss_entry;
    unsigned int limit = sizeof(tss_entry);

    // Adiciona o descritor TSS na GDT
    gdt_set_gate(num, base, limit, 0xE9, 0x00);

    // Zera o TSS (poderiamos usar memset)
    unsigned char *ptr = (unsigned char *)&tss_entry;
    for(unsigned int i = 0; i < sizeof(tss_entry); i++) {
        ptr[i] = 0;
    }

    tss_entry.ss0  = ss0;
    tss_entry.esp0 = esp0;

    // Configurações de segmentos indicadas pelos manuais (como as de User Mode)
    tss_entry.cs   = 0x0b;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x13;
    tss_entry.iomap_base = sizeof(tss_entry);
}

/* Função principal que inicializa tudo */
void init_gdt() {
    gdt_ptr.size = (sizeof(struct gdt_entry) * 6) - 1;
    gdt_ptr.address = (unsigned int)&gdt_entries;

    /* 1. Descritor Nulo (Index 0) */
    gdt_set_gate(0, 0, 0, 0, 0);

    /* 2. Segmento de Código do Kernel (Index 1) - Endereço base 0, Limite 4GB */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* 3. Segmento de Dados do Kernel (Index 2) - Endereço base 0, Limite 4GB */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* 4. Segmento de Código de Usuário (Index 3) - Endereço base 0, Limite 4GB, DPL 3 */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    /* 5. Segmento de Dados de Usuário (Index 4) - Endereço base 0, Limite 4GB, DPL 3 */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    /* 6. TSS (Index 5) */
    write_tss(5, 0x10, 0x0);

    /* Chama a função em Assembly que você criou no gdt.s! */
    load_gdt(&gdt_ptr);
    tss_flush();
}
// Created by victoria on 25/02/2026.
//