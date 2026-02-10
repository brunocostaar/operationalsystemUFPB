global outb

; outb(unsigned short port, unsigned char data)
; cdecl:
; [esp + 4] = port
; [esp + 8] = data
outb:
    mov al, [esp + 8]
    mov dx, [esp + 4]
    out dx, al
    ret
