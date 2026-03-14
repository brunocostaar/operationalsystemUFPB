#ifndef KHEAP_H
#define KHEAP_H

// Nosso malloc do Kernel
void* kmalloc(unsigned int size);

// Nosso free do Kernel
void kfree(void *ptr);

#endif
