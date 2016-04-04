.text

.globl concat
concat:
	pushl %ebp
	movl %esp, %ebp
	subl $8, %esp 
	movl $0, -8(%ebp)
	pushl 8(%ebp)
	call strlen
	addl %eax, -8(%ebp)
	pushl 12(%ebp)
	call strlen
	addl %eax, -8(%ebp)
	addl $1, -8(%ebp)
	pushl -8(%ebp)
	call malloc 
	movl %eax, -4(%ebp)
	pushl 8(%ebp)
	pushl -4(%ebp)
	call strcpy
	pushl 12(%ebp)
	pushl -4(%ebp)
	call strcat
	addl $36, %esp 
	popl %ebp
	ret

.globl tree
tree:
	pushl %ebp
	movl %esp, %ebp
	subl $20, %esp 
	pushl 8(%ebp) 
	call opendir 
	addl $4, %esp 
	movl %eax, -4(%ebp)
	cmpl $0, -4(%ebp)
	je .tree_exit
.readdir_cycle:
	pushl -4(%ebp) 
	call readdir
	addl $4, %esp 
	movl %eax, -8(%ebp)
	cmpl $0, -8(%ebp) 
	je .end_cycle
	movl -8(%ebp), %ebx 
	addl $11, %ebx 		
	movzbl (%ebx), %eax
	cmpl $46, %eax
	je .readdir_cycle 
	movl $0, -12(%ebp)
.loop_print_tabs:
	movl -12(%ebp), %ecx
	cmpl %ecx, 12(%ebp)
	je .end_loop_print_tabs
	pushl $9
	call putchar
	addl $4, %esp
	addl $1, -12(%ebp)	
	jmp .loop_print_tabs
.end_loop_print_tabs:
	pushl %ebx
	pushl $message
	call printf
	addl $8, %esp
	pushl $backslash
	pushl 8(%ebp)
	call concat
	movl %eax, -16(%ebp)
	addl $8, %esp
	pushl %ebx
	pushl -16(%ebp)
	call concat
	movl %eax, -20(%ebp)
	addl $8, %esp
	movl 12(%ebp), %eax
	addl $1, %eax
	pushl %eax
	pushl -20(%ebp)
	call tree
	addl $8, %esp
	pushl -16(%ebp)
	call free
	pushl -20(%ebp)
	call free
	addl $8, %esp
	jmp .readdir_cycle
.end_cycle:
	pushl -4(%ebp)
	call closedir
	addl $4, %esp 
.tree_exit:
	addl $20, %esp
	popl %ebp
	ret

.globl main
main:
	movl 8(%esp), %eax
	pushl $0
	pushl 4(%eax)
	call tree
	addl $8, %esp 
	ret

.data
message:
	.string "%s\n\0"
backslash:
	.string "/\0"
