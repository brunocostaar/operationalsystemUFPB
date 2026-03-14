#include "io.h"
#include "gdt.h"
#include "idt.h"
#include "paging.h"
#include "multiboot.h"
#include "pmm.h"
#include "kheap.h"

/* * Importando as marcações de memória do link.ld.
 * Declaramos como funções void para forçar o compilador a nos dar 
 * puramente o endereço de memória, impedindo que ele tente ler o conteúdo.
 */
extern void kernel_virtual_start(void);
extern void kernel_virtual_end(void);
extern void kernel_physical_start(void);
extern void kernel_physical_end(void);

typedef void (*call_module_t)(void);

/* Função auxiliar para imprimir números em hexadecimal na porta serial */
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

/* * Função principal do Kernel.
 * O argumento 'ebx' contém a localização do mapa de memória do GRUB.
 */
void kmain(unsigned int ebx)
{
    // Lendo as coordenadas exatas do nosso Kernel na memória usando '&'
    unsigned int kp_start = (unsigned int)&kernel_physical_start;
    unsigned int kp_end   = (unsigned int)&kernel_physical_end;
    multiboot_info_t *mbinfo = (multiboot_info_t*) ebx;
    /* ---------------- SERIAL E VÍDEO ---------------- */
    // Configura a porta serial COM1 para log de debug
    serial_configure_baud_rate(0x3F8, 3);
    serial_configure_buffers(SERIAL_COM1_BASE);

    serial_write("Kernel iniciado\n");
    
    // Imprimindo os limites físicos do Kernel no terminal do Linux
    serial_write("Memoria Fisica Start: ");
    serial_write_hex(kp_start);
    serial_write("\nMemoria Fisica End: ");
    serial_write_hex(kp_end);
    serial_write("\n");

    // Imprime na tela do QEMU
    fb_write("Bem-vindo ao OS!\n");

    /* ---------------- INFRAESTRUTURA BASE ---------------- */
    // Configura a GDT (Permissões de segmentos de memória)
    init_gdt();
    
    // Configura a IDT (Tabela de Tratamento de Interrupções / Teclado)
    idt_init();
    
    /* ---------------- PMM (Gerenciador Físico) ---------------- */
    serial_write("\nInicializando PMM com Mapa do Hardware...\n");
    
    // Passamos o mapa do GRUB e a área restrita do Kernel para blindar a memória
    pmm_init(mbinfo, kp_start, kp_end); 
    
    serial_write("PMM inicializado com sucesso!\n");
    
    /* ---------------- KHEAP (O Fatiador de RAM) ---------------- */
    serial_write("\n--- TESTE DO KMALLOC  ---\n");
    
    // Teste 1: Alocando um texto pequeno (16 bytes)
    char *texto = (char*) kmalloc(16);
    texto[0] = 'U';
    texto[1] = 'F';
    texto[2] = 'P';
    texto[3] = 'B';
    texto[4] = '\0'; // Terminador de string do C

    serial_write("1. Texto alocado no endereco: ");
    serial_write_hex((unsigned int)texto);
    serial_write("\nConteudo lido da RAM: ");
    serial_write(texto);
    serial_write("\n");

    // Teste 2: Alocando um número (4 bytes)
    unsigned int *numero = (unsigned int*) kmalloc(4);
    *numero = 0xDEADBEEF; // Um valor clássico de debug

    serial_write("2. Numero alocado no endereco: ");
    serial_write_hex((unsigned int)numero);
    
    // Teste 3: Alocando outro bloco para ver o salto
    unsigned int *outro_numero = (unsigned int*) kmalloc(4);
    serial_write("\n3. Outro numero no endereco: ");
    serial_write_hex((unsigned int)outro_numero);
    
    serial_write("\n-----------------------------------------------\n\n");
    serial_write("\n--- TESTE DE ALOCACAO DE RAM ---\n");
    unsigned int pagina1 = pmm_alloc_page();
    serial_write("Pagina 1: ");
    serial_write_hex(pagina1);
    
    unsigned int pagina2 = pmm_alloc_page();
    serial_write("\nPagina 2: ");
    serial_write_hex(pagina2);
    serial_write("\n--------------------------------\n\n");

    /* ---------------- PAGING (Memória Virtual) ---------------- */
    paging_init(ebx);
    serial_write("Paging ativo\n");

    /* ---------------- MULTIBOOT (Módulos) ---------------- */
    if (mbinfo->mods_count > 0)
    {
        module_t *mod = (module_t*) mbinfo->mods_addr;
        call_module_t start = (call_module_t)mod->mod_start;
        start();
    }
    else
    {
        serial_write("Nenhum modulo encontrado\n");
    }

    // O Kernel dorme e aguarda interrupções
    while (1)
        asm volatile("hlt");
}

