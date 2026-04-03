#ifndef RAMFS_H
#define RAMFS_H

#define MAX_FILENAME 32
#define MAX_CHILDREN 10

// Estrutura do nosso "Nó" (Pasta ou Arquivo)
typedef struct FileNode {
    char name[MAX_FILENAME];
    int is_directory; // 1 para pasta, 0 para arquivo
    struct FileNode* parent; // Ponteiro para a pasta "mãe"
    struct FileNode* children[MAX_CHILDREN]; // O que tem dentro dessa pasta
    int child_count; // Quantidade atual de itens
    char* data; 
    int size;
} FileNode;

// Variáveis globais para controlar onde estamos na navegação
extern FileNode* fs_root;
extern FileNode* current_directory;

// Funções que vamos implementar no .c
void init_fs();
void fs_mkdir(char* dirname);
void fs_ls();
void fs_rmdir(char* dirname);

#endif
