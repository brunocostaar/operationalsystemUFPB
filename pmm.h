#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

// Agora recebemos o mapa do GRUB e os limites do Kernel
void pmm_init(multiboot_info_t *mbinfo, unsigned int kernel_start, unsigned int kernel_end);

unsigned int pmm_alloc_page(void);

#endif
