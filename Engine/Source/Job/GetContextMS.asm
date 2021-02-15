; Intel NASM assembly for storing register states into a FiberContext.
; This is for the Microsoft x64 ABI.

; rcx contains the address of the FiberContext to fill in
mov r8, [rsp] ; Load the return address (stored in the stack)
mov [rcx + 8 * 0], r8 ; Store it into the context's rip
lea r8, [rsp + 8 * 1] ; Load the address after the return address
mov [rcx + 8 * 1], r8 ; Store it into the context's rsp

; General Purpose Registers
mov [rcx + 8 * 2], rbx
mov [rcx + 8 * 3], rbp
mov [rcx + 8 * 4], r12
mov [rcx + 8 * 5], r13
mov [rcx + 8 * 6], r14
mov [rcx + 8 * 7], r15
mov [rcx + 8 * 8], rdi
mov [rcx + 8 * 9], rsi

; Vector Registers (we don't use YMM or ZMM so we only store XMM)
movups [rcx + 8 * 10 + 16 * 0], xmm6
movups [rcx + 8 * 10 + 16 * 1], xmm7
movups [rcx + 8 * 10 + 16 * 2], xmm8
movups [rcx + 8 * 10 + 16 * 3], xmm9
movups [rcx + 8 * 10 + 16 * 4], xmm10
movups [rcx + 8 * 10 + 16 * 5], xmm11
movups [rcx + 8 * 10 + 16 * 6], xmm12
movups [rcx + 8 * 10 + 16 * 7], xmm13
movups [rcx + 8 * 10 + 16 * 8], xmm14
movups [rcx + 8 * 10 + 16 * 9], xmm15

xor eax, eax ; Set return value to 0.
ret
