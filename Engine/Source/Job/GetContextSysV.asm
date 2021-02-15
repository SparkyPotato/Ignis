mov r8, [rsp]
mov [rdi + 8 * 0], r8 ; Store RIP
lea r8, [rsp + 8 * 1]
mov [rdi + 8 * 1], r8 ; Store RSP

mov [rdi + 8 * 2], rbx
mov [rdi + 8 * 3], rbp
mov [rdi + 8 * 4], r12
mov [rdi + 8 * 5], r13
mov [rdi + 8 * 6], r14
mov [rdi + 8 * 7], r15

xor eax, eax
ret
