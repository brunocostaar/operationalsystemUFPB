/* interrupts.c */
#include "io.h" // Assumindo que você tem funções de escrever na tela/serial aqui

// Este é o espelho exato do que o Assembly e a CPU empurraram na pilha!
struct cpu_state {
    // Empurrados pelo nosso 'pushad' no Assembly
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int esp_dummy; // esp original empurrado pelo pushad (ignoramos)
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;

    // Empurrados pelas nossas macros no Assembly
    unsigned int interrupt_number;
    unsigned int error_code;

    // Empurrados AUTOMATICAMENTE pela CPU antes de tudo
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
    unsigned int esp;
    unsigned int ss;
} __attribute__((packed));

// Mapa de tradução: Scan Code (PS/2 Set 1) -> ASCII
const char kbd_US[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   
  '*',  0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
  '-',  0,   0,   0, '+',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   
};

// A função mágica que o Assembly chama!
void interrupt_handler(struct cpu_state cpu) {
    // Se for a interrupção 0 (Divisão por zero)
    if (cpu.interrupt_number == 0) {
        // Aqui você usaria sua função de imprimir na tela/serial
        // Exemplo: printf("FALHA: Divisao por Zero!\n");
        
        // Como você já tem o framebuffer, vamos pintar o canto da tela de vermelho para testar:
        volatile unsigned short *video_memory = (unsigned short *)0xB8000;
        video_memory[0] = 0x4F44; // 'D' (de Divisão) com fundo vermelho (4) e letra branca (F)
    }

    if (cpu.interrupt_number == 32) {
        outb(0x20, 0x20);
    }

    // Interrupção 33 (Teclado)
    if (cpu.interrupt_number == 33) {
        
        unsigned char scan_code = inb(0x60);
        
        // Verifica se é um evento de APERTAR a tecla (Make Code)
        // Se o bit mais significativo estiver ligado (scan_code & 0x80), significa que a tecla foi SOLTA.
        if (!(scan_code & 0x80)) {
            
            // Traduz o código do botão para a letra real
            char c = kbd_US[scan_code];
            
            // Se for um caractere válido, imprimimos na tela!
            if (c != 0) {
                // Ponteiro direto para a memória de vídeo (como fizemos no capítulo 4)
                volatile unsigned short *video_memory = (unsigned short *)0xB8000;
                
                // Variável estática para lembrar onde o cursor parou na última vez
                static int cursor_x = 0;
                
                // Pinta a letra de branco (F) no fundo preto (0)
                // Usamos um deslocamento para não sobrescrever a sua mensagem de "Bem-vindo"
                video_memory[160 + cursor_x] = (unsigned short)c | (0x0F << 8);
                
                cursor_x++; // Move o cursor para o lado
            }
        }
        
        // Avisa o PIC que terminamos
        outb(0x20, 0x20);
    }
    
    // Se for outra interrupção não tratada, você pode imprimir o número dela.
}