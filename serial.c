#include "io.h"

/** serial_configure_baud_rate:
*  Define a velocidade dos dados enviados. A velocidade padrão de uma porta
*  serial é de 115200 bits/s. O argumento é um divisor desse número, logo
*  a velocidade resultante se torna (115200 / divisor) bits/s.
*
*  @param com      A porta COM para configurar
*  @param divisor  O divisor
*/

void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    
    // Parte BAIXA para base + 0
    outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
    
    // Parte ALTA para base + 1
    outb(com + 1, (divisor >> 8) & 0x00FF);
    
    serial_configure_line(com); //DESATIVAR O DLAB 
}

/** serial_configure_line:
 * Configura a linha da porta serial especificada. A porta é configurada para ter um
 * comprimento de dados de 8 bits, sem bits de paridade, um bit de parada e controle
 * de quebra (break control) desativado.
 *
 * @param com  A porta serial a ser configurada
 */
void serial_configure_line(unsigned short com)
{
    /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
     * Conteúdo:| d | b | prty  | s | dl  |
     * Valor:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
     */
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}



/** serial_is_transmit_fifo_empty:
*  Checa se a fila de transmissão FIFO está vazia dada a porta enviada COM
*
*  @param  com A porta COM
*  @return 0 Caso a fila de transmissao nao esteja vazia
*          1 Caso a fila de transmissao esteja vazia
*/
int serial_is_transmit_fifo_empty(unsigned int com)
{
/* 0x20 = 0010 0000 */
return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_configure_buffers(unsigned short com) {
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7); 
}

void serial_write_byte(unsigned short com, char c) //Envia o byte
{
    while (serial_is_transmit_fifo_empty(com) == 0);
    outb(SERIAL_DATA_PORT(com), c);
}

/** serial_write:
 * Escreve uma sequência de caracteres na porta serial COM1.
 * * @param buf O buffer de texto
 * @param len O tamanho do texto
 * @return O número de bytes escritos
 */
int serial_write(char *buf)
{
    int i = 0;
    
    // Percorre a string caractere por caractere
    while (buf[i] != '\0') {
        /* Espera o hardware e envia o byte atual */
        serial_write_byte(SERIAL_COM1_BASE, buf[i]);
        i++;
    }
    
    return i; // Retorna quantos bytes foram enviados
}
