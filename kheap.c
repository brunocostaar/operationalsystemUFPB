#include "kheap.h"
#include "pmm.h"

// Header invisivel de 12 bytes (tamanho, status, proximo)
typedef struct block_meta {
    unsigned int size;       
    int is_free;             
    struct block_meta *next; 
} block_meta_t;

#define META_SIZE sizeof(block_meta_t)

static block_meta_t *heap_head = (void*)0;

// O AÇOUGUEIRO: Corta um bloco grande em dois pedaços
static void split_block(block_meta_t *block, unsigned int requested_size) {
    // Só fatiamos se o espaço que sobrar der para guardar um novo header + alguns bytes
    unsigned int remaining_space = block->size - requested_size;
    
    if (remaining_space >= META_SIZE + 4) {
        // A matemática dos ponteiros: pula o header atual e o tamanho dos dados solicitados
        // para gravar o novo header no exato byte onde o espaço livre começa
        block_meta_t *new_free_block = (block_meta_t*)((char*)block + META_SIZE + requested_size);
        
        new_free_block->size = remaining_space - META_SIZE;
        new_free_block->is_free = 1;
        new_free_block->next = block->next;
        
        // Ajusta o bloco atual para o tamanho exato que o usuario pediu
        block->size = requested_size;
        block->next = new_free_block;
    }
}

static block_meta_t* request_new_page(void) {
    unsigned int raw_page = pmm_alloc_page();
    if (raw_page == 0) return (void*)0; 

    block_meta_t *block = (block_meta_t*) raw_page;
    block->size = 4096 - META_SIZE; 
    block->is_free = 1;
    block->next = (void*)0;

    return block;
}

void* kmalloc(unsigned int size) {
    if (size == 0) return (void*)0;

    // Alinhamento em 4 bytes para a CPU ler mais rapido
    unsigned int aligned_size = (size + 3) & ~3;
    
    block_meta_t *current = heap_head;
    block_meta_t *last = (void*)0;

    // Busca um bloco livre
    while (current != (void*)0) {
        if (current->is_free && current->size >= aligned_size) {
            // Achou um espaco. Fatie-o se for grande demais.
            split_block(current, aligned_size);
            current->is_free = 0;
            return (void*)(current + 1); 
        }
        last = current;
        current = current->next;
    }

    // Nao achou espaco na lista, pede pagina nova
    block_meta_t *new_block = request_new_page();
    if (!new_block) return (void*)0;

    if (last) {
        last->next = new_block;
    } else {
        heap_head = new_block; 
    }

    // Fatie a pagina nova logo de inicio
    split_block(new_block, aligned_size);
    new_block->is_free = 0;

    return (void*)(new_block + 1);
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_meta_t *block = (block_meta_t*)ptr - 1;
    block->is_free = 1;

}
