#include "fb.h"
#include "io.h"

#define FB_WIDTH 80
#define FB_HEIGHT 25
#define FB_ADDRESS 0xB8000

static unsigned short cursor_pos = 0;

static void fb_scroll(void)
{
    char *fb = (char *) FB_ADDRESS;

    for (int i = 0; i < (FB_HEIGHT - 1) * FB_WIDTH * 2; i++)
        fb[i] = fb[i + FB_WIDTH * 2];

    for (int i = (FB_HEIGHT - 1) * FB_WIDTH * 2;
         i < FB_HEIGHT * FB_WIDTH * 2; i++)
        fb[i] = 0;

    cursor_pos -= FB_WIDTH;
}

void fb_move_cursor(unsigned short pos)
{
    outb(0x3D4, 14);
    outb(0x3D5, (pos >> 8) & 0xFF);
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}

void fb_write_char(char c)
{
    char *fb = (char *) FB_ADDRESS;

    if (c == '\n') {
        cursor_pos += FB_WIDTH;
        cursor_pos -= cursor_pos % FB_WIDTH;
    } else {
        fb[cursor_pos * 2]     = c;
        fb[cursor_pos * 2 + 1] = 0x1F;
        cursor_pos++;
    }

    if (cursor_pos >= FB_WIDTH * FB_HEIGHT)
        fb_scroll();

    fb_move_cursor(cursor_pos);
}

void fb_write(const char *buf, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
        fb_write_char(buf[i]);
}
