section .text

global _start

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
  	cmp rcx, 0
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
 	je .end
 	xor rdx, rdx
 	mul rsi
 	sub rbx, 1
 	jmp power
 	.end:
 	ret


print_number: 
	mov rcx, 1
	mov ebp, 0
	xor rbx, rbx
	cmp rax, 0
	jge not_negate
	 	mov rdx, 45
	  	mov [msg], rdx
	  	add ebp, 1
	  	neg rax
	not_negate:
	push 			rax
	call 			length
	sub 			rbx, 1
	mov 			rax, 1
	mov 			rsi, 10
	push 			rbx
	call 			power
	mov 			rcx, rax
	pop 			rbx
	pop 			rax
	call 			division
	mov             rax, 1
	mov             rdi, 1
	mov             rsi, msg
	push      		rdi
	add        		rbx, 2
	mov       		[msg+ebp], byte 0x0a
	pop        		rdi
	mov             rdx, rbx
	syscall
	ret


_start:
  		pop   			rax
        cmp             rax, 2
        jne             end

        pop             rax
        pop             rdi
        mov             rax, 2
        xor             rsi, rsi
        xor             rdx, rdx
        syscall
        cmp             rax, 0
        jl              end
        mov             rdi, rax
        xor             rbx, rbx


		mov r9, 1
		mov rbx, 0
read:

		xor rax, rax
		mov rsi, buf
		mov rdx, 1
		syscall
		cmp rax, 0
		je end

; rd9 - have spaces before

; Check for spaces
		cmp 		[buf], word 10  
		je 			count
		cmp 		[buf], word 9
		je 			count
		cmp 		[buf], word 11
		je 			count
		cmp 		[buf], word 12
		je 			count
		cmp 		[buf], word 13
		je 			count
		cmp			[buf], word 32
		je 			count
;end checks
		jmp perevod
		count:
		cmp r9, 0
		jne read
		mov r9, 1
		add rbx, 1
		jmp read
perevod:
		mov 		r9, 0
		jmp 		read

end:
		cmp			r9, 1
		je 			.loop
		add 		rbx, 1		
.loop:	
		mov         rax, 3
    	syscall    	
		mov 		rax, rbx
		call 		print_number
		mov  		rax, 60
 		xor 		rdi, rdi
  		syscall  


section .bss
		buf: 		resb 1
		section      .data

msg          dw         "",0x0a