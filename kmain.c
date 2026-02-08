// Definindo a estrutura para um caractere na tela
struct CoresCaractere {
    unsigned char caractere; // bit 0 - 7
    unsigned char cores;     // bit 8 - 15 (4 bits fundo, 4 bits letra)
} __attribute__((packed));

void kmain() {
    // Aponta para o início da memória de vídeo (Modo Texto)
    struct CoresCaractere *tela = (struct CoresCaractere *) 0xB8000;

    // Escreve 'A' com fundo azul (1) e letra branca (F) -> 0x1F
    tela[0].caractere = 'A';
    tela[0].cores = 0x1F;
}