ENTRY(loader)

SECTIONS {
    /* O kernel é carregado na marca exata de 1 MB físico e virtual */
    . = 0x00100000;

    /* Etiquetas de INÍCIO do kernel. Como não estamos no "Higher Half" (3GB),
       o endereço físico e virtual são idênticos aqui. */
    kernel_virtual_start = .;
    kernel_physical_start = .;

    /* Alinha em 4 KB e guarda o código executável */
    .text ALIGN(4K) : {
        *(.text) 
    }

    /* Alinha em 4 KB e guarda dados somente leitura (ex: textos entre aspas) */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    /* Alinha em 4 KB e guarda variáveis globais que já começam com algum valor */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* Alinha em 4 KB e guarda variáveis não inicializadas (a memória é zerada) */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss) 
    }

    /* Etiquetas de FIM do kernel. Tudo o que vier depois daqui é RAM livre */
    kernel_virtual_end = .;
    kernel_physical_end = .;
}
