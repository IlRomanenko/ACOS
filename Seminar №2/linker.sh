gcc $1.c -c -o $1.o 
 ld -o $1.out -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o -lc $1.o /usr/lib/x86_64-linux-gnu/crtn.o

