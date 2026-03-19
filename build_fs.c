#include <stdio.h>
#include <string.h>

#define MAX_FILES 16

struct fs_file {
    char name[32];
    unsigned int size;
    unsigned int offset;
};

struct fs_header {
    unsigned int nfiles;
    struct fs_file files[MAX_FILES];
};

int main() {
    FILE *out = fopen("fs.img", "wb");

    struct fs_header header;
    header.nfiles = 1;

    // arquivo 1
    strcpy(header.files[0].name, "hello.txt");

    char data[] = "Hello from FS!\n";
    header.files[0].size = sizeof(data);

    // offset começa depois do header
    header.files[0].offset = sizeof(struct fs_header);

    // escreve header
    fwrite(&header, sizeof(header), 1, out);

    // escreve dados
    fwrite(data, sizeof(data), 1, out);

    fclose(out);
    return 0;
}
