#include "pmm.h"
#include "multiboot.h"

#define PAGE_SIZE 4096
// Expandido para suportar até 4 GB de RAM no mundo real (1.048.576 blocos)
#define MAX_PAGES 1048576   

unsigned char memory_bitmap[MAX_PAGES / 8];

static inline void set_bit(int bit)
{
    memory_bitmap[bit/8] |= (1 << (bit%8));
}

static inline void clear_bit(int bit)
{
    memory_bitmap[bit/8] &= ~(1 << (bit%8));
}

static inline int test_bit(int bit)
{
    return memory_bitmap[bit/8] & (1 << (bit%8));
}

void pmm_init(multiboot_info_t *mbinfo, unsigned int kernel_start, unsigned int kernel_end)
{
    // 1.Começamos marcando a RAM inteira como OCUPADA (0xFF).
    for(int i = 0; i < MAX_PAGES / 8; i++) {
        memory_bitmap[i] = 0xFF;
    }

    // 2. Lendo o mapa de memória da Placa-Mãe via GRUB
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mbinfo->mmap_addr;
    unsigned int mmap_end = mbinfo->mmap_addr + mbinfo->mmap_length;

    while ((unsigned int) mmap < mmap_end) {
        // Tipo 1 significa "RAM Livre e Utilizável"
        if (mmap->type == 1) {
            unsigned int addr = mmap->base_addr_low;
            unsigned int length = mmap->length_low;
            
            // Destranca os bits específicos dessa região
            for (unsigned int i = 0; i < length; i += PAGE_SIZE) {
                clear_bit((addr + i) / PAGE_SIZE);
            }
        }
        // Avança o ponteiro para a próxima entrada do mapa (o tamanho é dinâmico)
        mmap = (multiboot_memory_map_t *) ((unsigned int) mmap + mmap->size + sizeof(mmap->size));
    }

    // 3. O GRUB diz que a marca de 1MB está livre, mas nosso Kernel está lá.
    // Trancamos novamente a área exata onde o código do Kernel está hospedado.
    for (unsigned int addr = kernel_start; addr <= kernel_end; addr += PAGE_SIZE) {
        set_bit(addr / PAGE_SIZE);
    }
    
    // 4. Tranca o endereço físico 0x0 por precaução (usado pelo hardware VGA da BIOS)
    set_bit(0);
}

unsigned int pmm_alloc_page()
{
    for(int i = 0; i < MAX_PAGES; i++)
    {
        if(!test_bit(i))
        {
            set_bit(i);
            return i * PAGE_SIZE;
        }
    }
    return 0; // Se retornar 0, significa que a RAM lotou (Kernel Panic)
}
