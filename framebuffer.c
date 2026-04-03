#include "io.h"

struct CoresCaractere *tela = (struct CoresCaractere *) 0xB8000;
unsigned int cursor_pos = 0;

void fb_write_cell(unsigned int posicao, char c, unsigned char cor_letra, unsigned char cor_fundo)
{
    tela[posicao].caractere = c; 
    /* Formata o byte de cores: fundo no high nibble, letra no low nibble */
    tela[posicao].cores = ((cor_fundo & 0x0F) << 4) | (cor_letra & 0x0F); 
}

/** fb_move_cursor:
* Move o cursor do framebuffer para determinada posiçao
*
* @param pos A nova posição do cursor
*/
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

// Função de rolagem de tela
void scroll_screen() {
    if (cursor_pos >= 2000) {
        // Move as linhas 1 a 24 para cima
        for (int i = 0; i < 1920; i++) {
            tela[i] = tela[i + 80];
        }

        // Limpa a última linha
        for (int i = 1920; i < 2000; i++) {
            fb_write_cell(i, ' ', FB_WHITE, FB_BLUE);
        }

        // Retorna o cursor para o começo da última linha
        cursor_pos = 1920;
    }
}

int fb_write(char *buf)
{
    int i = 0;
    while (buf[i] != '\0') {
        
        // Trata o caractere de nova linha '\n'
        if (buf[i] == '\n') {
            // Move o cursor para o início da próxima linha
            cursor_pos = ((cursor_pos / 80) + 1) * 80;
        } 
        else {
            // Escreve o caractere normal
            fb_write_cell(cursor_pos, buf[i], FB_WHITE, FB_BLUE);
            cursor_pos++;
        }

        // Checa e aplica o scroll imediatamente se a tela encher
        scroll_screen();

        i++;
    }

    // Atualiza o cursor físico na tela
    fb_move_cursor(cursor_pos);
    
    return i;
}

void fb_clear() {
    for (int i = 0; i < 2000; i++) {
        // Escreve espaços em branco na tela toda (cor azul do seu print)
        fb_write_cell(i, ' ', FB_WHITE, FB_BLUE);
    }
    cursor_pos = 0;
    fb_move_cursor(0);
}
