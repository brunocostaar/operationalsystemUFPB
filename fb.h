#ifndef FB_H
#define FB_H

void fb_move_cursor(unsigned short pos);
void fb_write_char(char c);
void fb_write(const char *buf, unsigned int len);

#endif
