#include "io.h"

struct CoresCaractere *tela = (struct CoresCaractere *) 0xB8000;
static unsigned int cursor_pos = 0;

void fb_write_cell(unsigned int posicao, char c, unsigned char cor_letra, unsigned char cor_fundo)
{
    tela[posicao].caractere = c; 
    /* Formata o byte de cores: fundo no high nibble, letra no low nibble */
    tela[posicao].cores = ((cor_fundo & 0x0F) << 4) | (cor_letra & 0x0F); 
}

/** fb_move_cursor:
*  Move o cursor do framebuffer para determinada posiçao
*
*  @param pos A nova posição do cursor
*/
void fb_move_cursor(unsigned short pos)
{
outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
outb(FB_DATA_PORT,    pos & 0x00FF);
}

int fb_write(char *buf)
{
    int i = 0;
    while (buf[i] != '\0') {
        
        // Trata o caractere de nova linha '\n'
        if (buf[i] == '\n') {
            // Move o cursor para o início da próxima linha
            // Lógica: (Linha Atual + 1) * 80 colunas
            cursor_pos = ((cursor_pos / 80) + 1) * 80;
        } 
        else {
            //Escreve o caractere normal
            fb_write_cell(cursor_pos, buf[i], FB_WHITE, FB_BLUE);
            cursor_pos++;
        }

        // Se chegou na coluna 80, o cursor já estará na 
        // próxima linha automaticamente pelo incremento, mas aqui garantimos
        // que ele não passe do limite total da tela (2000 células).
        if (cursor_pos >= 2000) {
            cursor_pos = 0; // Por enquanto, apenas reseta (ou você pode implementar scroll)
        }

        i++;
    }

    // Atualiza o cursor físico na tela
    fb_move_cursor(cursor_pos);
    
    return i;
}
