ENTRY(loader)                /* O nome da sua label no assembly */

SECTIONS {
    . = 0x00100000;          /* O código DEVE começar em 1MB */

    .text : {
        *(.text)             /* Todo o código vai aqui */
    }

    .rodata : {
        *(.rodata)           /* Strings e constantes (Read Only Data) */
    }

    .data : {
        *(.data)             /* Variáveis globais inicializadas */
    }

    .bss : {
        *(.bss)              /* Variáveis não inicializadas e a STACK */
    }
}