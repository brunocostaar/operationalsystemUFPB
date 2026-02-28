#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H
/* --- DEFINIÇÕES DE CORES DO VETOR --- */
#define FB_BLACK     0
#define FB_BLUE      1
#define FB_GREEN     2
#define FB_WHITE     15

/* --- ESTRUTURA DO FRAMEBUFFER --- */
struct CoresCaractere {
    unsigned char caractere; 
    unsigned char cores;     // 4 bits fundo, 4 bits letra
} __attribute__((packed));

/* PORTAS DE E/S */

/* TODAS as portas de E/S são calculadas em relação a porta de dados. Isto ocorre pois
* todas as portas seriais (COM1, COM2, COM3, COM4) apresentam suas portas na mesma
* ordem, mas iniciam com valores distintos.
*/

#define SERIAL_COM1_BASE                0x3F8      /* COM1  endereço porta base */

//Agrupamos agora as proximas portas sequencialmente

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

/* Comandos da porta de E/S */

/* SERIAL_LINE_ENABLE_DLAB:
* Isso altera o mapeamento da porta de dados (base) para receber o divisor de 
* baud rate em vez de caracteres de texto.
*/
#define SERIAL_LINE_ENABLE_DLAB         0x80

/* PORTAS DE E/S */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* COMANDOS PARA PORTA DE E/S */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

/** outb:
*  Envia o respectivo dado para a respectiva porta, definida em io.s
*
*  @param port A porta de E/S 
*  @param data O dado enviado para a porta de E/S
*/
void outb(unsigned short port, unsigned char data);


void serial_configure_baud_rate(unsigned short com, unsigned short divisor);
void serial_configure_line(unsigned short com);
int serial_is_transmit_fifo_empty(unsigned int com);
void serial_configure_buffers(unsigned short com);
void serial_write_byte(unsigned short com, char c);
int serial_write(char *buf); 
void serial_write_hex(unsigned int value);
/** inb:
*  Lê um byte de uma porta de E/S
*
*  @param  port Endereço da porta de E/S
*  @return      o byte lido
*/
unsigned char inb(unsigned short port);

/* -- PROTÓTIPOS DO FRAMEBUFFER -- */
void fb_write_cell(unsigned int posicao, char c, unsigned char cor_letra, unsigned char cor_fundo);
void fb_move_cursor(unsigned short pos);
int fb_write(char *buf);

#endif /* INCLUDE_IO_H */

