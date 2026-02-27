#include "io.h"
#include "gdt.h"

void kmain() {
    /* Inicializa o hardware necessário (Serial, etc) */
    serial_configure_baud_rate(0x3F8, 3);
    serial_configure_buffers(SERIAL_COM1_BASE);
    
    //TESTE DE LOG
    serial_write("Log: Kernel iniciado com sucesso!\n");
    
    //TESTES DE TELA, ESCREVER E MOVER CURSOR
    fb_write("Bem-vindo ao OS!");    // Vai aparecer na tela azul
    init_gdt();
    while(1) {
        // O SO fica rodando aqui para sempre
    }

}
