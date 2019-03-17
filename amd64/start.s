#include "syscalls.h"

.intel_syntax noprefix

.text
    .globl _start, syscall
    .globl syscall1, syscall2, syscall3, syscall4, syscall5, syscall6

_start:
    xor rbp,rbp
    pop rdi
    mov rsi,rsp
    and rsp,-16
    call main
	jmp _exit	
	ret

syscall:
    mov rax,rdi
    syscall
    ret

syscall1:
    mov rax,rdi
    mov rdi,rsi
    syscall
    ret

syscall2:
    mov rax,rdi
    mov rdi,rsi
    mov rsi,rdx
    syscall
    ret

syscall3:
    mov rax,rdi
    mov rdi,rsi
    mov rsi,rdx
    mov rdx,rcx
    syscall
    ret

syscall4:
    mov rax,rdi
    mov rdi,rsi
    mov rsi,rdx
    mov rdx,rcx
    mov r10,r8
    syscall
    ret

syscall5:
    mov rax,rdi
    mov rdi,rsi
    mov rsi,rdx
    mov rdx,rcx
    mov r10,r8
    mov r8,r9
    syscall
    ret

syscall6:
    mov rax,rdi
    mov rdi,rsi
    mov rsi,rdx
    mov rdx,rcx
    mov r10,r8
    mov r8,r9
    mov r9,[rsp+8]
    syscall
	ret

_exit:
    mov rdi,rax
    mov rax,SYS_EXIT
    syscall
    ret
    