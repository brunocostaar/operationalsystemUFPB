#include "paging.h"
#include "multiboot.h"

#define PAGE_SIZE 4096
#define MAX_TABLES 32   /* até 128MB */


unsigned int page_directory[1024]
__attribute__((aligned(4096)));

unsigned int page_tables[MAX_TABLES][1024]
__attribute__((aligned(4096)));

extern void load_page_directory(unsigned int);
extern void enable_paging(void);

void paging_init(unsigned int ebx)
{
    asm volatile("cli");

    multiboot_info_t *mbinfo =
        (multiboot_info_t*) ebx;

    /* -----------------------------
       Descobre maior endereço usado
    ----------------------------- */
    unsigned int highest_addr = 0x400000; // mínimo 4MB

    if (mbinfo->mods_count > 0)
    {
        module_t *mod =
            (module_t*) mbinfo->mods_addr;

        if (mod->mod_end > highest_addr)
            highest_addr = mod->mod_end;
    }

    /* quantas page tables precisamos */
    unsigned int tables_needed =
        (highest_addr / (4 * 1024 * 1024)) + 1;

    if (tables_needed > MAX_TABLES)
        tables_needed = MAX_TABLES;

    /* -----------------------------
       Limpa directory
    ----------------------------- */
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0;

    /* -----------------------------
       Identity mapping necessário
    ----------------------------- */
    for (unsigned int t = 0; t < tables_needed; t++)
    {
        for (int i = 0; i < 1024; i++)
        {
            unsigned int addr =
                (t * 1024 + i) * PAGE_SIZE;

            page_tables[t][i] = addr | 3; // present + rw
        }

        page_directory[t] =
            ((unsigned int)page_tables[t]) | 3;
    }

    /* -----------------------------
       Ativa paging
    ----------------------------- */
unsigned int pd_phys;
asm volatile("mov %1, %0"
             : "=r"(pd_phys)
             : "r"(page_directory));

load_page_directory(pd_phys);
    enable_paging();

    asm volatile("sti");

}
void map_page(unsigned int virt, unsigned int phys)
{
    unsigned int dir = virt >> 22;
    unsigned int tbl = (virt >> 12) & 0x03FF;

    unsigned int *table =
        (unsigned int*)(page_directory[dir] & 0xFFFFF000);

    table[tbl] = phys | 3;
}
