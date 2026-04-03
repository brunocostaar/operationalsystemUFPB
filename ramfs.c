#include "ramfs.h"
#include "kheap.h"
#include "shell.h"

// Avisando ao compilador que essa função existe no framebuffer.c
extern void fb_write(char *buf);
extern int str_compare(char *s1, char *s2); // usar função de comparar string

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

// --- COMANDO PWD (Descobrir onde estamos) ---
void fs_pwd_helper(FileNode* node) {
    if (node == fs_root) {
        fb_write("/");
        return;
    }
    fs_pwd_helper(node->parent); // Sobe até a raiz recursivamente
    fb_write(node->name);
    fb_write("/");
}

void fs_pwd() {
    if (current_directory == fs_root) {
        fb_write("/\n");
    } else {
        fs_pwd_helper(current_directory);
        fb_write("\n");
    }
}

// --- COMANDO CD (Navegar entre pastas) ---
void fs_cd(char* dirname) {
    if (str_compare(dirname, "..") == 0) {
        current_directory = current_directory->parent;
        return;
    }
    if (str_compare(dirname, ".") == 0) return;
    if (str_compare(dirname, "/") == 0) {
        current_directory = fs_root;
        return;
    }

    for (int i = 0; i < current_directory->child_count; i++) {
        FileNode* child = current_directory->children[i];
        if (child->is_directory && str_compare(child->name, dirname) == 0) {
            current_directory = child;
            return;
        }
    }
    fb_write("Erro: Diretorio nao encontrado.\n");
}

// --- COMANDO TOUCH (Criar um arquivo vazio) ---
void fs_touch(char* filename) {
    if (current_directory->child_count >= MAX_CHILDREN) {
        fb_write("Erro: Diretorio cheio!\n");
        return;
    }

    FileNode* new_file = (FileNode*) kmalloc(sizeof(FileNode));
    str_copy(new_file->name, filename);
    new_file->is_directory = 0; // 0 significa que é um arquivo, não uma pasta
    new_file->parent = current_directory;
    new_file->child_count = 0;

    current_directory->children[current_directory->child_count] = new_file;
    current_directory->child_count++;

    fb_write("Arquivo criado.\n");
}

// --- COMANDO RM (Deletar um arquivo) ---
void fs_rm(char* filename) {
    int found = -1;
    for (int i = 0; i < current_directory->child_count; i++) {
        FileNode* child = current_directory->children[i];
        // Tem que ser arquivo (is_directory == 0) e ter o mesmo nome
        if (child->is_directory == 0 && str_compare(child->name, filename) == 0) {
            found = i;
            break;
        }
    }

    if (found != -1) {
        for (int i = found; i < current_directory->child_count - 1; i++) {
            current_directory->children[i] = current_directory->children[i+1];
        }
        current_directory->child_count--;
        fb_write("Arquivo removido.\n");
    } else {
        fb_write("Erro: Arquivo nao encontrado.\n");
    }
}

// --- COMANDO READ ---
void fs_read(char* filename) {
    for (int i = 0; i < current_directory->child_count; i++) {
        FileNode* child = current_directory->children[i];
        if (child->is_directory == 0 && str_compare(child->name, filename) == 0) {
            if (child->data != 0) {
                fb_write(child->data);
                fb_write("\n");
            } else {
                fb_write("(Arquivo vazio)\n");
            }
            return;
        }
    }
    fb_write("Erro: Arquivo nao encontrado.\n");
}

// --- COMANDO WRITE ---
void fs_write(char* filename, char* content) {
    for (int i = 0; i < current_directory->child_count; i++) {
        FileNode* child = current_directory->children[i];
        if (child->is_directory == 0 && str_compare(child->name, filename) == 0) {
            // Conta o tamanho da string
            int len = 0;
            while(content[len] != '\0') len++;
            
            // Aloca espaço exato na memória física para o texto usando o seu kmalloc
            child->data = (char*) kmalloc(len + 1);
            str_copy(child->data, content);
            child->size = len;
            
            fb_write("Dados gravados com sucesso.\n");
            return;
        }
    }
    fb_write("Erro: Arquivo nao encontrado.\n");
}

// --- COMANDO MV (Renomear ou Mover) ---
void fs_mv(char* src_name, char* dest_name) {
    int src_idx = -1;
    int dest_idx = -1;
    FileNode* src_node = 0;

    // 1. Procura o arquivo/diretório de origem
    for (int i = 0; i < current_directory->child_count; i++) {
        if (str_compare(current_directory->children[i]->name, src_name) == 0) {
            src_idx = i;
            src_node = current_directory->children[i];
            break;
        }
    }

    if (src_idx == -1) {
        fb_write("Erro: Origem nao encontrada.\n");
        return;
    }

    // 2. Verifica se o destino existe e se é um diretório
    for (int i = 0; i < current_directory->child_count; i++) {
        if (str_compare(current_directory->children[i]->name, dest_name) == 0) {
            if (current_directory->children[i]->is_directory) {
                dest_idx = i;
            }
            break;
        }
    }

    // 3. Executa a Ação (Mover vs Renomear)
    if (dest_idx != -1) {
        // O DESTINO É UMA PASTA: Vamos mover o ponteiro para lá
        FileNode* dest_dir = current_directory->children[dest_idx];
        
        if (dest_dir->child_count >= MAX_CHILDREN) {
            fb_write("Erro: Diretorio de destino cheio.\n");
            return;
        }

        // A. Adiciona o nó no array da nova pasta
        dest_dir->children[dest_dir->child_count] = src_node;
        dest_dir->child_count++;
        src_node->parent = dest_dir; // Atualiza quem é o novo pai

        // B. Remove o nó do array da pasta antiga (shift para a esquerda)
        for (int i = src_idx; i < current_directory->child_count - 1; i++) {
            current_directory->children[i] = current_directory->children[i+1];
        }
        current_directory->child_count--;

        fb_write("Movido para o diretorio com sucesso.\n");
    } else {
        // O DESTINO NÃO É PASTA: Apenas renomeia a string
        str_copy(src_node->name, dest_name);
        fb_write("Renomeado com sucesso.\n");
    }
}
