#include "pmm.h"

#define PAGE_SIZE 4096
#define MAX_PAGES 32768   // suporta até 128MB RAM

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

void pmm_init()
{
    /* marca tudo como usado */
    for(int i=0;i<MAX_PAGES/8;i++)
        memory_bitmap[i]=0xFF;

    /* libera memória após 2MB (kernel + GRUB seguro) */
    for(unsigned int addr=0x200000;
        addr < 128*1024*1024;
        addr += PAGE_SIZE)
    {
        clear_bit(addr/PAGE_SIZE);
    }
}

unsigned int pmm_alloc_page()
{
    for(int i=0;i<MAX_PAGES;i++)
    {
        if(!test_bit(i))
        {
            set_bit(i);
            return i*PAGE_SIZE;
        }
    }
    return 0;
}
