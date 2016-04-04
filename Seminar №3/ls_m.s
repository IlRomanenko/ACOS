.file "mls.c"
.text

.macro printf_mes mes str
	pushl \str
	pushl \mes
	call printf
	addl $8, %esp
.endm


/*
My stack : int len(4 bytes), char *res (4 bytes)

My params: char *str1(4 bytes), char *str2(4 bytes)
*/
.globl concat
concat:
	pushl %ebp
	movl %esp, %ebp
	subl $8, %esp #allocate stack

	movl $0, -8(%ebp)
	
	pushl 8(%ebp)
	call strlen
	addl $4, %esp

	addl %eax, -8(%ebp)

	pushl 12(%ebp)
	call strlen
	addl $4, %esp

	addl %eax, -8(%ebp)

	addl $1, -8(%ebp) #len = strlen(str1) + strlen(str2) + 1

	pushl -8(%ebp)
	call malloc 
	add $4, %esp

	movl %eax, -4(%ebp)

	pushl 8(%ebp)
	pushl -4(%ebp)
	call strcpy
	add $8, %esp

	pushl 12(%ebp)
	pushl -4(%ebp)
	call strcat
	add $8, %esp

	addl $8, %esp #free stack
	popl %ebp
	ret


/*
My stack:
char *s(4 bytes), char *ts(4 bytes), int i(4 bytes), *dirent (4 bytes), *DIR (4 bytes)

*DIR = -4(%ebp)
*dirent = -8(%ebp)
i = -12(%ebp)
*ts = -16(%ebp)
*s = -20(%ebp)

Total : 20 bytes



My params:
char* directory, int tabs
*directory 	= 8(%ebp)
tabs 		= 12(%ebp)
*/

.globl tree
tree:
	pushl %ebp
	movl %esp, %ebp

	subl $20, %esp #allocate 20 bytes for local variables



	/*
				TEST!
	
	printf_mes $message, 8(%ebp)
	printf_mes $int_mes, 12(%ebp)
	
				END_TEST!
	*/


	pushl 8(%ebp) #add first param to stack
	call opendir #result in %eax
	addl $4, %esp #delete 4(%esp) from stack

	movl %eax, -4(%ebp) #save %eax (*DIR) into stack(first 4 bytes from right)

	cmpl $0, -4(%ebp)
	je .tree_exit #check, that *DIR != NULL 

	/*					TEST 
	pushl $after_je_exit
	call puts
	addl $4, %esp
						END_TEST*/

.readdir_cycle:
									#while((ent = readdir(dir)) != NULL)
	pushl -4(%ebp) #readdir(dir)
	call readdir
	addl $4, %esp #delete param for readdir from stack

	movl %eax, -8(%ebp) #save result of readdir into stack(second 4 bytes from right)

	cmpl $0, -8(%ebp) #check, that ent != NULL

	je .end_cycle

	movl -8(%ebp), %ebx # ent (*dirent)
	addl $11, %ebx 		#ent->d_name

	

#	pushl %ebx
#	call strlen                                   INGORE .* DIRECTORIES
#	addl $4, %esp

	
	movzbl (%ebx), %eax
	cmpl $46, %eax

	je .readdir_cycle 	# if (ent->d_name[0] == '.')
						#	continue;


	

	movl $0, -12(%ebp) # int i = 0
.loop_print_tabs:
	
	movl -12(%ebp), %ecx

	cmpl %ecx, 12(%ebp)
	je .end_loop_print_tabs

	pushl $9
	call putchar
	addl $4, %esp

	addl $1, -12(%ebp)	#i++
	jmp .loop_print_tabs
.end_loop_print_tabs:

	
	printf_mes $message, %ebx #printf("%s", ent->d_name);
																
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

	#printf_mes $message, -20(%ebp)


	pushl -16(%ebp)
	call free
	pushl -20(%ebp)
	call free
	addl $8, %esp


	jmp .readdir_cycle

.end_cycle:
	pushl -4(%ebp) #push *DIR into stack
	call closedir	#closedir
	addl $4, %esp #remove *DIR from stack

.tree_exit:
	addl $20, %esp


	/*			TEST!
	pushl $exit_message
	call puts
	addl $4, %esp
				END_OF_TEST!*/

	popl %ebp
	ret




.globl main
main:
	
	/*это забавно. в стеке все идет так
	(esp)		-> argc
	(esp + 4)	-> argv
	затем уже argv подразделяется примерно так
	(argv)		-> имя программы
	(argv + 4)	-> первый аргумент(строка)
	(argv + 8)	-> второй аргумент(строка), если есть и т.д.*/

	movl 8(%esp), %eax
	pushl $0
	pushl 4(%eax)
	call tree
	addl $8, %esp 

	ret


.data
int_mes:
	.string "%d\n\0"
message:
	.string "%s\n\0"
exit_message:
	.string "\nEnd of tree\n\0"
backslash:
	.string "/\0"
after_je_exit:
	.string "\nafter_je_exit\n\0"
before_je_end_cycle:
	.string "\nbefore_je_end_cycle\n\0"
printf_directory:
	.string "/%s\n\0"
