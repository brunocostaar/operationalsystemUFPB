#include "io.h"

// Copia da struct usada no interrupts.c
struct cpu_state {
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp_dummy;
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;

    unsigned int interrupt_number;
    unsigned int error_code;

    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
    unsigned int esp;
    unsigned int ss;
} __attribute__((packed));

unsigned int syscall_handler(struct cpu_state *cpu)
{
    switch (cpu->eax)
    {
        case 0:
            fb_write((char*) cpu->ebx);
            break;

        default:
            fb_write("Syscall desconhecida\n");
            break;
    }

    return 0;
}
