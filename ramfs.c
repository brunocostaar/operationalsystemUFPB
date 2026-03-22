#include "ramfs.h"
#include "kheap.h"

// Avisando ao compilador que essa função existe no framebuffer.c
extern void fb_write(char *buf);

FileNode* fs_root = 0;
FileNode* current_directory = 0;

void str_copy(char* dest, char* src) {
    int i = 0;
    while (src[i] != '\0' && i < MAX_FILENAME - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void init_fs() {
    fs_root = (FileNode*) kmalloc(sizeof(FileNode));
    str_copy(fs_root->name, "/");
    fs_root->is_directory = 1;
    fs_root->parent = fs_root;
    fs_root->child_count = 0;
    current_directory = fs_root;

    fb_write("Sistema de arquivos RAM inicializado na raiz /\n");
}

void fs_ls() {
    fb_write("Conteudo de ");
    fb_write(current_directory->name);
    fb_write(":\n");

    for (int i = 0; i < current_directory->child_count; i++) {
        fb_write("- ");
        fb_write(current_directory->children[i]->name);
        if (current_directory->children[i]->is_directory) {
            fb_write(" (DIR)");
        }
        fb_write("\n");
    }
}

void fs_mkdir(char* dirname) {
    if (current_directory->child_count >= MAX_CHILDREN) {
        fb_write("Erro: Diretorio cheio!\n");
        return;
    }

    FileNode* new_dir = (FileNode*) kmalloc(sizeof(FileNode));
    str_copy(new_dir->name, dirname);
    new_dir->is_directory = 1;
    new_dir->parent = current_directory;
    new_dir->child_count = 0;

    current_directory->children[current_directory->child_count] = new_dir;
    current_directory->child_count++;

    fb_write("Diretorio criado com sucesso.\n");
}

void fs_rmdir(char* dirname) {
    int found = -1;
    for (int i = 0; i < current_directory->child_count; i++) {
        int match = 1;
        for(int j = 0; dirname[j] != '\0'; j++) {
            if (current_directory->children[i]->name[j] != dirname[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            found = i;
            break;
        }
    }

    if (found != -1) {
        for (int i = found; i < current_directory->child_count - 1; i++) {
            current_directory->children[i] = current_directory->children[i+1];
        }
        current_directory->child_count--;
        fb_write("Diretorio removido.\n");
    } else {
        fb_write("Erro: Diretorio nao encontrado.\n");
    }
}