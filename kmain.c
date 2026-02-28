#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "pmm.h"
#include "multiboot.h"

typedef void (*call_module_t)(void);

void serial_write_hex(unsigned int value)
{
    char hex[] = "0123456789ABCDEF";
    char out[2];

    serial_write("0x");

    out[1] = '\0';

    for (int i = 28; i >= 0; i -= 4)
    {
        unsigned int digit = (value >> i) & 0xF;
        out[0] = hex[digit];
        serial_write(out);
    }
}

void kmain(unsigned int eax, unsigned int ebx)
{
serial_write("EAX = ");
serial_write_hex(eax);
serial_write("\n");
    /* Debug serial */
    serial_configure_baud_rate(0x3F8, 3);
    serial_configure_buffers(SERIAL_COM1_BASE);
    serial_write("Kernel iniciado\n");

    fb_write("Bem-vindo ao OS!\n");

    /* Inicialização básica */
    init_gdt();
    idt_init();

    /* -------- GERENCIADOR DE MEMÓRIA FÍSICA -------- */
    pmm_init();          // ← NOVO PASSO

    /* -------- PAGING -------- */
//    paging_init(ebx);

    serial_write("Paging ativo\n");

    /* -------- MAPEAR MODULO DO GRUB -------- */
    multiboot_info_t *mbinfo =
        (multiboot_info_t*)ebx;

    if (mbinfo->mods_count > 0)
    {
        module_t *mod =
            (module_t*)mbinfo->mods_addr;

        serial_write("Mapeando modulo...\n");


        serial_write("Executando modulo...\n");

        call_module_t start =
            (call_module_t)mod->mod_start;

        start();
    }
    else
    {
        serial_write("Nenhum modulo encontrado\n");
    }

    while(1)
        asm volatile("hlt");
}
