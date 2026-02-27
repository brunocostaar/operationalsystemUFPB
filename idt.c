#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load();
extern void interrupt_handler_0();
extern void interrupt_handler_1();
extern void interrupt_handler_32(); 
extern void interrupt_handler_33(); 
extern void pic_remap(int offset1, int offset2); // <-- ADICIONE ISSO AQUI

void idt_set_gate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_init() {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_set_gate(0, (unsigned int)interrupt_handler_0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned int)interrupt_handler_1, 0x08, 0x8E);
    idt_set_gate(32, (unsigned int)interrupt_handler_32, 0x08, 0x8E);
    idt_set_gate(33, (unsigned int)interrupt_handler_33, 0x08, 0x8E);

    pic_remap(0x20, 0x28);
    idt_load();
}