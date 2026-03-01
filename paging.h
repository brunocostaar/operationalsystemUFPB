#ifndef PAGING_H
#define PAGING_H

void paging_init(unsigned int ebx);

extern unsigned int page_directory[];

void load_page_directory(unsigned int addr);
void map_page(unsigned int virt, unsigned int phys);
void enable_paging(void);

#endif

