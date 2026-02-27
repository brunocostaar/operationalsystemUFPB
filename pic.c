#include "io.h"

/* Portas I/O do PIC Mestre e Escravo */
#define PIC1_PORT_A 0x20
#define PIC1_PORT_B 0x21
#define PIC2_PORT_A 0xA0
#define PIC2_PORT_B 0xA1

/* Comandos mágicos do PIC */
#define PIC_ICW1_ICW4       0x01    /* Indica que o ICW4 será necessário */
#define PIC_ICW1_INIT       0x10    /* Comando de inicialização! */
#define PIC_ICW4_8086       0x01    /* Define o modo de operação para 8086 */

/*
 * offset1 = Novo número inicial para o PIC Mestre (vamos usar 32 / 0x20)
 * offset2 = Novo número inicial para o PIC Escravo (vamos usar 40 / 0x28)
 */
void pic_remap(int offset1, int offset2) {
    unsigned char a1, a2;

    // 1. Salva as "máscaras" atuais (quais interrupções estão ativadas/desativadas)
    a1 = inb(PIC1_PORT_B);
    a2 = inb(PIC2_PORT_B);

    // 2. ICW1: Avisa os dois PICs que vamos começar a configurá-los (Reset)
    outb(PIC1_PORT_A, PIC_ICW1_INIT | PIC_ICW1_ICW4);
    outb(PIC2_PORT_A, PIC_ICW1_INIT | PIC_ICW1_ICW4);

    // 3. ICW2: Define os novos limites (Onde eles começam na IDT)
    outb(PIC1_PORT_B, offset1);
    outb(PIC2_PORT_B, offset2);

    // 4. ICW3: Configura a cascata (Avisa o Mestre que o Escravo está no pino 2)
    outb(PIC1_PORT_B, 4);
    outb(PIC2_PORT_B, 2);

    // 5. ICW4: Define o modo do ambiente (Modo 8086 antigo)
    outb(PIC1_PORT_B, PIC_ICW4_8086);
    outb(PIC2_PORT_B, PIC_ICW4_8086);

    // 6. Restaura as máscaras salvas
    outb(PIC1_PORT_B, a1);
    outb(PIC2_PORT_B, a2);
}