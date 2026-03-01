BITS 32
; Coloca um valor fácil de se distinguir em eax
mov eax, 0xDEADBEEF

; Entra em loop infinito ($ é o começo da mesma linha/instrução)
jmp $
