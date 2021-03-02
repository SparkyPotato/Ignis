; GetContext
mov r8, [rsp] ; Store the return address into r8
mov [rcx + 8 * 0], r8 ; Move the return address into the Context's RIP
lea r8, [rsp + 8 * 1] 
mov [rcx + 8 * 1], r8 ; Store the value of RSP

mov [rcx + 8 * 2], rbx ; All other registers
mov [rcx + 8 * 3], rbp
mov [rcx + 8 * 4], r12
mov [rcx + 8 * 5], r13
mov [rcx + 8 * 6], r14
mov [rcx + 8 * 7], r15
mov [rcx + 8 * 8], rdi
mov [rcx + 8 * 9], rsi

movups [rcx + 8 * 10 + 16 * 0], xmm6 ; XMM registers
movups [rcx + 8 * 10 + 16 * 1], xmm7
movups [rcx + 8 * 10 + 16 * 2], xmm8
movups [rcx + 8 * 10 + 16 * 3], xmm9
movups [rcx + 8 * 10 + 16 * 4], xmm10
movups [rcx + 8 * 10 + 16 * 5], xmm11
movups [rcx + 8 * 10 + 16 * 6], xmm12
movups [rcx + 8 * 10 + 16 * 7], xmm13
movups [rcx + 8 * 10 + 16 * 8], xmm14
movups [rcx + 8 * 10 + 16 * 9], xmm15

; Restore all stored state from the second argument,
; which is stored in rdx.
mov r8, [rdx + 8 * 0] ; Load RIP into r8

mov rsp, [rdx + 8 * 1] ; Change RSP back

mov rbx, [rdx + 8 * 2] ; Other GPRs
mov rbp, [rdx + 8 * 3]
mov r12, [rdx + 8 * 4]
mov r13, [rdx + 8 * 5]
mov r14, [rdx + 8 * 6]
mov r15, [rdx + 8 * 7]
mov rdi, [rdx + 8 * 8]
mov rsi, [rdx + 8 * 9]

movups xmm6, [rdx + 8 * 10 + 16 * 0] ; XMM registers
movups xmm7, [rdx + 8 * 10 + 16 * 1]
movups xmm8, [rdx + 8 * 10 + 16 * 2]
movups xmm9, [rdx + 8 * 10 + 16 * 3]
movups xmm10, [rdx + 8 * 10 + 16 * 4]
movups xmm11, [rdx + 8 * 10 + 16 * 5]
movups xmm12, [rdx + 8 * 10 + 16 * 6]
movups xmm13, [rdx + 8 * 10 + 16 * 7]
movups xmm14, [rdx + 8 * 10 + 16 * 8]
movups xmm15, [rdx + 8 * 10 + 16 * 9]

push r8 ; Set the return address to the loaded address

xor eax, eax
ret
