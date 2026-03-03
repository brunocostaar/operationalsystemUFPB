global inb

; inb - Retorna um byte dado a porta de E/S
; stack: [esp + 4] Endereço da porta de E/S
;        [esp    ] Endereço de retorno
inb:
mov dx, [esp + 4]       ; Movo o Endereço da porta de E/S para o registrador dx
in  al, dx              ; Leio um byte da porta de E/S e armazeno no registrador al
ret       		; Retorna o valor lido de al

global outb             ; 

; outb - envio um byte para a porta de E/S
; stack: [esp + 8] O byte de dados (8 bits)
;        [esp + 4] A porta de E/S
;        [esp    ] Endereço de retorno
outb:
mov al, [esp + 8]    ; MOvo os dados para o registrador al
mov dx, [esp + 4]    ; Movo o endereço da porta de E/S para dx
out dx, al           ; Envio o dado do registrador al para o registrador dx
ret                  ; retorno resultado

global read_cs
read_cs:
    mov eax, 0
    mov ax, cs      ; Puxa o Code Segment da CPU
    ret

global read_ds
read_ds:
    mov eax, 0
    mov ax, ds      ; Puxa o Data Segment da CPU
    ret
