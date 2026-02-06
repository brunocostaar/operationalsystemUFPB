ENTRY(loader)                /* O nome da label de entrada */

SECTIONS {
    . = 0x00100000;          /* O código deve ser iniciado em 1MB, antes disso é  */
			     /* Reservado para Bios, GRUB e etc */		
    .text ALIGN (0x1000) :   /* alinho em espaços de 4 KB */
    {
        *(.text)             /* Juntar todas as seções de texto de todos os arquivos */
    }

    .rodata ALIGN (0x1000) : /* Alinho em espaços de 4 KB */
    {
        *(.rodata*)          /* Seção para todas constantes de todos arquivos */
    }

    .data ALIGN (0x1000) :   /* Alinho em espaços de 4 KB */
    {
        *(.data)             /* Juntar todas as variáveis globais já inicializadas */
    }
			     /* Ambos comandos rodata e data coletam dados de todos arquivos do projeto */

    .bss ALIGN (0x1000) :    /* Alinho em espaços 4 KB */
    {
        *(COMMON)            /* Seção comum para todos os arquivos */
        *(.bss)              /* Seção bss para todos os arquivos */
    }			     /* Coleta variáveis não inicializadas */
}
