#include "paging.h"

#define PAGE_SIZE 4096

/* Page Directory (4KB alinhado) */
unsigned int page_directory[1024]
__attribute__((aligned(4096)));

/*
 * 4 Page Tables = 16MB identity mapped
 * 4 * 4MB = 16MB
 */
unsigned int page_tables[32][1024]
__attribute__((aligned(4096)));

/* funções em assembly */
extern void load_page_directory(unsigned int);
extern void enable_paging(void);

void paging_init(unsigned int ebx)
{
    (void)ebx; // ainda não usamos aqui

    /* Desabilita interrupções durante setup */
    asm volatile("cli");

    /* -----------------------------
       Limpa Page Directory
    ----------------------------- */
    for (int i = 0; i < 1024; i++)
        page_directory[i] = 0;

    /* -----------------------------
       Identity map dos primeiros 16MB
    ----------------------------- */
    for (int t = 0; t < 32; t++)
    {
        for (int i = 0; i < 1024; i++)
        {
            unsigned int addr =
                (t * 1024 + i) * PAGE_SIZE;

            /* Present + RW */
            page_tables[t][i] = addr | 3;
        }

        /* conecta tabela ao directory */
        page_directory[t] =
            ((unsigned int)page_tables[t]) | 3;
    }

    /* -----------------------------
       Ativa Paging
    ----------------------------- */
    load_page_directory((unsigned int)page_directory);
    enable_paging();

    asm volatile("sti");
}
