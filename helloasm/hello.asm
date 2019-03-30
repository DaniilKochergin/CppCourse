section         .text



section      .data

msg          dw         "",0x0a

global _start

;ax - частное, dx - остаток
division:
  xor rdx, rdx  
  div rcx
  add rax, 48
  mov [msg+ebp], rax
  add ebp, 1
  mov rax,rdx
  push rax
  xor rdx, rdx
  mov rsi, 10
  mov rax, rcx
  div rsi
  mov rcx, rax
  pop rax
  cmp rax, 0
  jne division
  ret


length:
 xor rdx, rdx
 add rbx, 1
 mov rsi, 10
 div rsi
 cmp rax, 0
 jne length
 ret

power:
 cmp rbx, 0
 je end
 xor rdx, rdx
 mul rsi
 sub rbx, 1
 jmp power
 end:
 ret


_start:
mov rcx, 1
mov ebp, 0
mov rax, 1
xor rbx, rbx
cmp rax, 0
jge not_negate
  mov rdx, 45
  mov [msg], rdx
  add ebp, 1
  neg rax
not_negate:
push rax
call length
sub bx, 1
mov ax, 1
mov rsi, 10
push bx
call power
mov rcx, rax
pop bx
pop rax
call division

mov             rax, 1
mov             rdi, 1
mov             rsi, msg
push      rdi
add        rbx, 2
mov        [msg+ebp], byte 0x0a
pop        rdi
mov             edx, ebx
syscall

mov             rax, 60
xor             rdi, rdi
syscall
