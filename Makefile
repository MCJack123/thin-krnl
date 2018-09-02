CFLAGS=-nostdlib -nodefaultlibs -m32 -g
LIBALLOC_CFLAGS=-O2 -fno-builtin -fPIC -Wall $(CFLAGS)
LIBALLOC_HEADERPATH=-I./

kernel: obj/kasm.o obj/kc.o obj/stdlib.o obj/liballoc.o obj/kalloc.o obj/stdio.o
	ld -m elf_i386 -T link.ld -o kernel obj/kasm.o obj/kc.o obj/stdlib.o obj/liballoc.o obj/kalloc.o obj/stdio.o

obj/kasm.o: src/kernel.asm
	nasm -f elf32 src/kernel.asm -o obj/kasm.o

obj/kc.o: src/kernel.c src/stdlib.h src/stdio.h src/liballoc.h
	gcc $(CFLAGS) -c src/kernel.c -o obj/kc.o

obj/stdlib.o: src/stdlib.c src/stdlib.h src/liballoc.h
	gcc $(CFLAGS) -c src/stdlib.c -o obj/stdlib.o

obj/stdio.o: src/stdio.c src/stdio.h src/stdlib.h src/liballoc.h
	gcc $(CFLAGS) -c src/stdio.c -o obj/stdio.o

obj/liballoc.o: src/liballoc.c src/liballoc.h
	gcc $(LIBALLOC_HEADERPATH) $(LIBALLOC_CFLAGS) -static -c src/liballoc.c -o obj/liballoc.o

obj/kalloc.o: src/kernel_alloc.c src/liballoc.h
	gcc $(CFLAGS) -c src/kernel_alloc.c -o obj/kalloc.o