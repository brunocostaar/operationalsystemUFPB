#ifndef PAGING_H
#define PAGING_H

void paging_init(unsigned int ebx);
void map_page(unsigned int virt, unsigned int phys);

/* funções ASM do paging */
void load_page_directory(unsigned int addr);
void enable_paging(void);

extern unsigned int page_directory[];

#endif
