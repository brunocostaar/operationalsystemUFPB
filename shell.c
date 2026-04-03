#include "shell.h"
#include "ramfs.h"
#include "io.h"

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

extern void fs_read(char* filename);
extern void fs_write(char* filename, char* content);
extern void fs_mv(char* old_name, char* new_name);
extern int fb_write(char *buf);
extern unsigned char inb(unsigned short port);
extern void fb_write_cell(unsigned int posicao, char c, unsigned char cor_letra, unsigned char cor_fundo);
extern void fb_move_cursor(unsigned short pos);
extern unsigned int cursor_pos;
extern void fb_clear(); // Declarando a função de limpar a tela
extern void fs_pwd();
extern void fs_cd(char* dirname);
extern void fs_touch(char* filename);
extern void fs_rm(char* filename);

#define MAX_COMMAND_LEN 128
char command_buffer[MAX_COMMAND_LEN];
int buffer_idx = 0;

// O dicionário completo do teclado (agora com todas as letras!)
unsigned char keyboard_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
  '9', '0', '-', '=', '\b',
  '\t',
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
 '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',
  'm', ',', '.', '/',   0,
  '*',
    0,
  ' ',
    0,
};

int str_compare(char *s1, char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void process_command(char *buf) {
    char *arg1 = 0;
    char *arg2 = 0;

    // 1. Separa o comando da primeira palavra (arg1)
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == ' ') {
            buf[i] = '\0'; 
            arg1 = &buf[i + 1]; 
            break;
        }
    }

    // 2. Se existe arg1, procura um espaço dentro dele para separar a segunda palavra (arg2)
    if (arg1 != 0) {
        for (int i = 0; arg1[i] != '\0'; i++) {
            if (arg1[i] == ' ') {
                arg1[i] = '\0';
                arg2 = &arg1[i + 1];
                break;
            }
        }
    }

    // 3. Roteamento de Comandos
    if (str_compare(buf, "ls") == 0) {
        fs_ls();
    }
    else if (str_compare(buf, "help") == 0) {
        fb_write("--- MANUAL DO SISTEMA ---\n");
        fb_write("Navegacao: ls, pwd, cd [dir], clear\n");
        fb_write("Diretorios: mkdir [dir], rmdir [dir]\n");
        fb_write("Arquivos: touch [arq], rm [arq]\n");
        fb_write("Dados: write [arq] [texto], read [arq]\n");
        fb_write("Outros: mv [antigo] [novo], help\n");
        fb_write("-------------------------\n");
    }
    else if (str_compare(buf, "mkdir") == 0) {
        if (arg1 != 0 && arg1[0] != '\0') {
            fs_mkdir(arg1);
        } else {
            fb_write("Erro: Faltou o nome da pasta. Ex: mkdir Mel\n");
        }
    }
    else if (str_compare(buf, "rmdir") == 0) {
        if (arg1 != 0 && arg1[0] != '\0') {
            fs_rmdir(arg1);
        } else {
            fb_write("Erro: Faltou o nome da pasta. Ex: rmdir Mel\n");
        }
    }
    else if (str_compare(buf, "clear") == 0) {
        fb_clear();
    }
    else if (str_compare(buf, "pwd") == 0) {
        fs_pwd();
    }
    else if (str_compare(buf, "cd") == 0) {
        if (arg1 != 0 && arg1[0] != '\0') {
            fs_cd(arg1);
        } else {
            fb_write("Erro: Faltou o destino. Ex: cd home\n");
        }
    }
    else if (str_compare(buf, "touch") == 0) {
        if (arg1 != 0 && arg1[0] != '\0') {
            fs_touch(arg1);
        } else {
            fb_write("Erro: Faltou o nome do arquivo. Ex: touch notas.txt\n");
        }
    }
    else if (str_compare(buf, "rm") == 0) {
        if (arg1 != 0 && arg1[0] != '\0') {
            fs_rm(arg1);
        } else {
            fb_write("Erro: Faltou o nome do arquivo. Ex: rm notas.txt\n");
        }
    }
    else if (str_compare(buf, "read") == 0) {
        if (arg1 != 0 && arg1[0] != '\0') {
            fs_read(arg1);
        } else {
            fb_write("Erro: Faltou o nome do arquivo. Ex: read notas.txt\n");
        }
    }
    else if (str_compare(buf, "write") == 0) {
        if (arg1 != 0 && arg2 != 0 && arg2[0] != '\0') {
            fs_write(arg1, arg2);
        } else {
            fb_write("Erro: Faltou nome ou conteudo. Ex: write notas.txt ola_mundo\n");
        }
    }
    else if (str_compare(buf, "mv") == 0) {
        if (arg1 != 0 && arg2 != 0 && arg2[0] != '\0') {
            fs_mv(arg1, arg2);
        } else {
            fb_write("Erro: Faltou parametros. Ex: mv antigo.txt novo.txt\n");
        }
    }
    else if (buf[0] != '\0') {
        fb_write("Comando desconhecido. Digite 'help' para ver a lista.\n");
    }
}

char get_char() {
    unsigned char scancode;
    while ((inb(KBD_STATUS_PORT) & 1) == 0);
    scancode = inb(KBD_DATA_PORT);

    // O filtro mágico que impede a repetição infinita!
    if (scancode & 0x80) {
        return 0;
    }
    return keyboard_map[scancode];
}

void run_shell() {
    fb_write("\nTerminal pronto. Digite comandos:\n");
    fb_write("Group3@OS:/$ ");

    while (1) {
        char c = get_char();
        if (c == 0) continue;

        if (c == '\n') {
            command_buffer[buffer_idx] = '\0';
            fb_write("\n");
            process_command(command_buffer);
            buffer_idx = 0;
            fb_write("Group3@OS:/$ ");
        }
        else if (c == '\b') { // Lógica do Backspace
            if (buffer_idx > 0) {
                buffer_idx--;
                cursor_pos--;
                fb_write_cell(cursor_pos, ' ', 15, 1); // 15 = Branco, 1 = Azul
                fb_move_cursor(cursor_pos);
            }
        }
        else if (buffer_idx < MAX_COMMAND_LEN - 1) {
            command_buffer[buffer_idx++] = c;
            char str[2] = {c, '\0'};
            fb_write(str);
        }
    }
}
