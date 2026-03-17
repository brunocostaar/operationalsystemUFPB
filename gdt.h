#ifndef GDT_H
#define GDT_H

/* A estrutura que guarda o endereço e o tamanho da nossa tabela */
struct gdt {
    unsigned short size;
    unsigned int address;
} __attribute__((packed));

struct tss_entry_struct {
    unsigned int prev_tss;
    unsigned int esp0;
    unsigned int ss0;
    unsigned int esp1;
    unsigned int ss1;
    unsigned int esp2;
    unsigned int ss2;
    unsigned int cr3;
    unsigned int eip;
    unsigned int eflags;
    unsigned int eax;
    unsigned int ecx;
    unsigned int edx;
    unsigned int ebx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
    unsigned int fs;
    unsigned int gs;
    unsigned int ldt;
    unsigned short trap;
    unsigned short iomap_base;
} __attribute__((packed));

void load_gdt(struct gdt *gdt_ptr);
void init_gdt();
void tss_flush();
void set_kernel_stack(unsigned int stack);

#endif