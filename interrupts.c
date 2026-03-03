/* interrupts.c */
#include "io.h" 

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
// Mapa de tradução para quando o SHIFT está pressionado (PS/2 Set 1)
const char kbd_US_shifted[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',   
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',   
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',   
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0,   
  '*',  0,  ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
  '-',  0,   0,   0, '+',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0   
};

// Função para mover o cursor físico da placa de vídeo VGA
void update_hardware_cursor(int x, int y) {
    // Calcula a posição linear na tela (80 colunas)
    unsigned short pos = y * 80 + x;
    
    // Envia os 8 bits mais baixos da posição
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    
    // Envia os 8 bits mais altos da posição
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

// Avisamos o compilador que o Assembly tem essas funções
extern unsigned short read_cs();
extern unsigned short read_ds();

void executar_comando_gdt(int y) {
    unsigned short cs_val = read_cs();
    unsigned short ds_val = read_ds();
    volatile unsigned short *video = (unsigned short*)0xB8000;
    char hex[] = "0123456789ABCDEF";
    
    int offset = y * 80; // Calcula o início da linha atual
    
    // Escreve "CS:" e o valor (ex: 08)
    video[offset + 0] = (unsigned short)'C' | (0x1F << 8);
    video[offset + 1] = (unsigned short)'S' | (0x1F << 8);
    video[offset + 2] = (unsigned short)':' | (0x1F << 8);
    video[offset + 3] = (unsigned short)hex[(cs_val >> 4) & 0xF] | (0x1F << 8);
    video[offset + 4] = (unsigned short)hex[cs_val & 0xF] | (0x1F << 8);
    
    // Escreve "DS:" e o valor (ex: 10)
    video[offset + 6] = (unsigned short)'D' | (0x1F << 8);
    video[offset + 7] = (unsigned short)'S' | (0x1F << 8);
    video[offset + 8] = (unsigned short)':' | (0x1F << 8);
    video[offset + 9] = (unsigned short)hex[(ds_val >> 4) & 0xF] | (0x1F << 8);
    video[offset + 10] = (unsigned short)hex[ds_val & 0xF] | (0x1F << 8);
}

// Envia uma string inteira para a porta COM1
void serial_log(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        outb(0x3F8, str[i]);
    }
}

// A função mágica que o Assembly chama!
void interrupt_handler(struct cpu_state cpu) {
if (cpu.interrupt_number == 14)
    {
        unsigned int fault_addr;
        // Puxa o endereço problemático salvo pela CPU no registrador CR2
        asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

        volatile unsigned short *video = (unsigned short*)0xB8000;

        // 1. O "KERNEL PANIC": Pinta a tela INTEIRA de Vermelho (Fundo 4, Letra Branca F)
        for (int i = 0; i < 80 * 25; i++) {
            video[i] = (unsigned short)' ' | (0x4F << 8);
        }

        // 2. Escreve a mensagem de erro no topo
        char msg[] = "KERNEL PANIC - PAGE FAULT! ACESSO NEGADO AO ENDERECO: 0x";
        int pos = 0;
        while (msg[pos] != '\0') {
            video[pos] = (unsigned short)msg[pos] | (0x4F << 8);
            pos++;
        }

        // 3. Traduz e imprime o endereço (CR2) logo após a mensagem
        char hex[] = "0123456789ABCDEF";
        for (int i = 0; i < 8; i++) {
            unsigned int nibble = (fault_addr >> (28 - i * 4)) & 0xF;
            video[pos + i] = (unsigned short)hex[nibble] | (0x4F << 8);
        }
	update_hardware_cursor(pos + 8, 0); //Colocar o cursor no fim da mensagem de erro
	
	// --- GERANDO O LOG TÉCNICO NA SERIAL ---
        serial_log("\n========================================\n");
        serial_log("[CRITICAL] KERNEL PANIC!\n");
        serial_log("[CRITICAL] Motivo: Page Fault (Interrupcao 14)\n");
        serial_log("[CRITICAL] Endereco de memoria invalido: 0x");
        
        // Envia o endereço em hexadecimal letra por letra para a serial
        for (int i = 0; i < 8; i++) {
            unsigned int nibble = (fault_addr >> (28 - i * 4)) & 0xF;
            outb(0x3F8, hex[nibble]); 
        }
        
        serial_log("\n[CRITICAL] O sistema foi paralisado por seguranca.\n");
        serial_log("========================================\n\n");
        
        // 4. Congela a máquina para proteger o hardware
        while (1) asm volatile("hlt");
    }

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
    // Interrupção 33 (Teclado)
    // Interrupção 33 (Teclado)
    if (cpu.interrupt_number == 33) {
        
        unsigned char scan_code = inb(0x60);
        
        // Variáveis estáticas "lembram" os valores entre uma interrupção e outra
        static int shift_pressed = 0;
        static int cursor_x = 0;
        static int cursor_y = 2; // Começamos na linha 2 para não sobrescrever "Bem-vindo ao OS!"
        static char command_buffer[80]; // Memória da linha atual
        static int buffer_index = 0;    // Posição no buffer
        // 1. Verifica se SOLTOU o Shift (Break codes: Left Shift 0xAA, Right Shift 0xB6)
        if (scan_code == 0xAA || scan_code == 0xB6) {
            shift_pressed = 0;
        }
        // 2. Verifica se APERTOU o Shift (Make codes: Left Shift 0x2A, Right Shift 0x36)
        else if (scan_code == 0x2A || scan_code == 0x36) {
            shift_pressed = 1;
        }
        // 3. Processa outras teclas (Apenas Make Codes)
        else if (!(scan_code & 0x80)) {
            
            char c;
            // Escolhe o mapa correto dependendo do estado do Shift
            if (shift_pressed) {
                c = kbd_US_shifted[scan_code];
            } else {
                c = kbd_US[scan_code];
            }
            
            if (c != 0) {
                volatile unsigned short *video_memory = (unsigned short *)0xB8000;
                
                // --- LÓGICA DO ENTER (O EXECUTOR DE COMANDOS) ---
                if (c == '\n') {
                    command_buffer[buffer_index] = '\0'; // Fecha a string digitada
                    
                    cursor_x = 0; 
                    cursor_y++;   
                    
                    // Checa se o usuário digitou exatamente "gdt"
                    if (command_buffer[0] == 'g' && command_buffer[1] == 'd' && 
                        command_buffer[2] == 't' && command_buffer[3] == '\0') {
                        
                        executar_comando_gdt(cursor_y); 
                        cursor_y++; 
                    }
                    // NOVO COMANDO: Checa se o usuário digitou "page"
                    else if (command_buffer[0] == 'p' && command_buffer[1] == 'a' && 
                             command_buffer[2] == 'g' && command_buffer[3] == 'e' && 
                             command_buffer[4] == '\0') {
                        
                        // Criamos um ponteiro para um endereço bizarro e não mapeado
                        volatile unsigned int *ponteiro_proibido = (unsigned int *)0xDEADC0DE;
                        
                        // Tentamos ler o valor que está lá. Isso vai estourar a paginação!
                        unsigned int falha_proposital = *ponteiro_proibido; 
                        
                        // (O código nunca vai chegar nesta linha abaixo, a CPU vai travar antes)
                        (void)falha_proposital;
                    }
                    
                    buffer_index = 0; // Zera o buffer para a próxima linha
                }
                // --- LÓGICA DO BACKSPACE (APAGANDO DA TELA E DO BUFFER) ---
                // --- LÓGICA DO BACKSPACE (SEGURO E COM MULTI-LINHA) ---
                else if (c == '\b') {
                    // Caso 1: Estamos no meio da linha atual
                    if (cursor_x > 0) { 
                        cursor_x--;
                        if (buffer_index > 0) buffer_index--; // Apaga da memória também
                        video_memory[cursor_y * 80 + cursor_x] = (unsigned short)' ' | (0x1F << 8);
                    } 
                    // Caso 2: O cursor está no começo da linha (x=0), MAS ainda temos caracteres no buffer
                    else if (cursor_y > 0 && buffer_index > 0) {
                        cursor_y--;       // Sobe uma linha
                        cursor_x = 79;    // Vai para a última coluna da direita
                        buffer_index--;   // Remove do buffer
                        video_memory[cursor_y * 80 + cursor_x] = (unsigned short)' ' | (0x1F << 8);
                    }
                }
                // --- LÓGICA DAS LETRAS (ESCREVE E SALVA NO BUFFER) ---
                else {
                    video_memory[cursor_y * 80 + cursor_x] = (unsigned short)c | (0x1F << 8);
                    cursor_x++;
                    
                    // Guarda o caractere para o nosso comando
                    if (buffer_index < 79) {
                        command_buffer[buffer_index++] = c;
                    }
                    
                    if (cursor_x >= 80) {
                        cursor_x = 0;
                        cursor_y++;
                    }
                }
            }
        }
        //Atualiza a posição do cursor piscante na placa de vídeo
        update_hardware_cursor(cursor_x, cursor_y);
        // Avisa o PIC que terminamos
        outb(0x20, 0x20);
    }
    
    // Se for outra interrupção não tratada, você pode imprimir o número dela.
}
