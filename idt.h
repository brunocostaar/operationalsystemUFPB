#ifndef IDT_H
#define IDT_H

// Estrutura de uma entrada da IDT (8 bytes)
struct idt_entry {
    unsigned short base_lo;             // Os 16 bits mais baixos do endereço da função
    unsigned short sel;                 // Seletor do Segmento de Código (o mesmo da GDT)
    unsigned char always0;              // Este byte deve ser sempre 0
    unsigned char flags;                // Flags de permissão (ex: quem pode chamar isso?)
    unsigned short base_hi;             // Os 16 bits mais altos do endereço da função
} __attribute__((packed));

// Estrutura do ponteiro que passaremos para a CPU (6 bytes)
struct idt_ptr {
    unsigned short limit;               // Tamanho total da IDT - 1
    unsigned int base;                  // Endereço de memória onde a IDT começa
} __attribute__((packed));

// Declaração da função que vai inicializar tudo depois
void idt_init();

#endif