.PHONY: all run clean

OBJECTS = loader.o pmm.o kheap.o kmain.o io.o serial.o framebuffer.o gdt.o gdt_asm.o syscall.o\
          idt.o idt_asm.o pic.o interrupts.o interrupt_handlers.o paging.o paging_asm.o ramfs.o shell.o

CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
         -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-exceptions \
         -fno-pic -fno-pie -no-pie
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf32

build_fs: build_fs.c
	gcc build_fs.c -o build_fs

fs.img: build_fs
	./build_fs

all: os.iso

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

# =========================
# PROGRAM (CAPÍTULO 8)
# =========================
program: program.s
	mkdir -p iso/modules
	nasm -f bin $< -o iso/modules/program

filesystem: fs.img
	mkdir -p iso/modules
	cp fs.img iso/modules/fs.img
# =========================
# ISO
# =========================
os.iso: kernel.elf program filesystem
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
                -b boot/grub/stage2_eltorito    \
                -no-emul-boot                   \
                -boot-load-size 4               \
                -A os                           \
                -input-charset utf8             \
                -quiet                          \
                -boot-info-table                \
                -o os.iso                       \
                iso

run: program os.iso
	qemu-system-i386 -cdrom os.iso -serial stdio -d int -no-reboot > qemu.log 2>&1

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o kernel.elf os.iso build_fs fs.img
	rm -rf iso/modules
