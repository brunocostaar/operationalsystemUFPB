#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"

typedef void (*call_module_t)(void);

/* helper debug */
void serial_write_hex(unsigned int value)
{
    char hex[] = "0123456789ABCDEF";
    char out[2];
    out[1] = '\0';

    serial_write("0x");

    for (int i = 28; i >= 0; i -= 4)
    {
        out[0] = hex[(value >> i) & 0xF];
        serial_write(out);
    }
}

void kmain(unsigned int ebx)
{
    /* ---------------- SERIAL ---------------- */
    serial_configure_baud_rate(0x3F8, 3);
    serial_configure_buffers(SERIAL_COM1_BASE);

    serial_write("Kernel iniciado\n");

    fb_write("Bem-vindo ao OS!\n");

    /* ---------------- GDT / IDT ---------------- */
    init_gdt();
    idt_init();

    /* ---------------- PAGING ---------------- */
    paging_init(ebx);

    serial_write("Paging ativo\n");

    /* ---------------- MULTIBOOT ---------------- */
    multiboot_info_t *mbinfo =
        (multiboot_info_t*) ebx;

    if (mbinfo->mods_count > 0)
    {
        module_t *mod =
            (module_t*) mbinfo->mods_addr;
        call_module_t start =
            (call_module_t)mod->mod_start;

        start();
        
    }
    else
    {
        serial_write("Nenhum modulo encontrado\n");
    }

    while (1)
        asm volatile("hlt");
}

