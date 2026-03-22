#include "shell.h"
#include "ramfs.h"
#include "io.h"

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

extern int fb_write(char *buf);
extern unsigned char inb(unsigned short port);
extern void fb_write_cell(unsigned int posicao, char c, unsigned char cor_letra, unsigned char cor_fundo);
extern void fb_move_cursor(unsigned short pos);
extern unsigned int cursor_pos;
extern void fb_clear(); // Declarando a função de limpar a tela

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
    char *arg = 0; // Ponteiro para guardar o nome da pasta

    // Procura o primeiro espaço em branco para dividir o comando do argumento
    for (int i = 0; buf[i] != '\0'; i++) {
        if (buf[i] == ' ') {
            buf[i] = '\0'; // Transforma o espaço no fim da primeira palavra
            arg = &buf[i + 1]; // O argumento começa logo depois do espaço
            break;
        }
    }

    // Agora testamos qual foi o comando digitado
    if (str_compare(buf, "ls") == 0) {
        fs_ls();
    }
    else if (str_compare(buf, "help") == 0) {
        fb_write("Comandos disponiveis: ls, mkdir, rmdir, clear, help\n");
    }
    else if (str_compare(buf, "mkdir") == 0) {
        // Verifica se o usuário digitou o nome da pasta depois do mkdir
        if (arg != 0 && arg[0] != '\0') {
            fs_mkdir(arg); // Usa o seu ramfs para criar a pasta!
        } else {
            fb_write("Erro: Faltou o nome da pasta. Ex: mkdir Mel\n");
        }
    }
    else if (str_compare(buf, "rmdir") == 0) {
        // Verifica se o usuário digitou o nome da pasta depois do rmdir
        if (arg != 0 && arg[0] != '\0') {
            fs_rmdir(arg); // Usa o seu ramfs para deletar a pasta!
        } else {
            fb_write("Erro: Faltou o nome da pasta. Ex: rmdir Mel\n");
        }
    }
    else if (str_compare(buf, "clear") == 0) {
        fb_clear(); // Limpa a tela!
    }
    else if (buf[0] != '\0') {
        fb_write("Comando desconhecido.\n");
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
    fb_write("vic@OS:/$ ");

    while (1) {
        char c = get_char();
        if (c == 0) continue;

        if (c == '\n') {
            command_buffer[buffer_idx] = '\0';
            fb_write("\n");
            process_command(command_buffer);
            buffer_idx = 0;
            fb_write("vic@OS:/$ ");
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