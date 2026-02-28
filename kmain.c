#include "io.h"
#include "gdt.h"
#include "idt.h" // <-- 1. Não esqueça de incluir o header da IDT!
#include "multiboot.h"

typedef void (*call_module_t)(void);

void kmain(unsigned int ebx) {
	
	multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;
    unsigned int address_of_module = mbinfo->mods_addr;
	
    /* Inicializa o hardware necessário (Serial, etc) */
    serial_configure_baud_rate(0x3F8, 3);
    serial_configure_buffers(SERIAL_COM1_BASE);
    
    // TESTE DE LOG
    serial_write("Log: Kernel iniciado com sucesso!\n");
    
    // TESTES DE TELA, ESCREVER E MOVER CURSOR
    fb_write("Bem-vindo ao OS!");    // Vai aparecer na tela azul
    
    // 1. Inicializa os Segmentos de Memória
    init_gdt();
    
    // 2. Inicializa o Tratamento de Interrupções e Remapeia o PIC
    idt_init(); // <-- 2. Adicione a chamada da IDT aqui
    
    // 3. Dá o sinal verde para a CPU ouvir o teclado e os erros
    asm volatile("sti"); // <-- 3. Adicione o comando STI aqui
	
	//Executando program pelo endereco guardado em ebx
	call_module_t start_program = (call_module_t) address_of_module;
    start_program();
	
    while(1) {
        // O SO fica rodando aqui para sempre
        // Se você apertar uma tecla, a CPU pausa o while, vai para a IDT, 
        // executa o código do teclado e volta pro while!
    }
}