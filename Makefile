CFLAGS=-nostdlib -nodefaultlibs -m32 -g -nostdinc -I./src
LIBALLOC_CFLAGS=-O2 -fno-builtin -fPIC -Wall $(CFLAGS)

IDIR=src
ODIR=obj

_DEPS = stdlib liballoc stdio interrupt
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS).h)

_OBJ = interrupt kernel_alloc kernel kasm liballoc stdio stdlib
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ).o)

_DRIVER_OBJ = ata asm_ata
DRIVER_OBJ = $(patsubst %,$(ODIR)/driver_%,$(_OBJ).o)

kernel: $(OBJ) $(DRIVER_OBJ)
	ld -m elf_i386 -T link.ld -o kernel obj/kasm.o obj/kc.o obj/stdlib.o obj/liballoc.o obj/kalloc.o obj/stdio.o obj/interrupt.o

obj/liballoc.o: src/liballoc.c src/liballoc.h
	gcc $(LIBALLOC_CFLAGS) -static -c src/liballoc.c -o obj/liballoc.o

obj/kasm.o: src/kernel.asm
	nasm -f elf32 src/kernel.asm -o obj/kasm.o

obj/%.o: src/%.c $(DEPS)
	gcc $(CFLAGS) -c $< -o $@

obj/driver_%.o: src/drivers/%.c src/drivers/%.h $(DEPS)
	gcc $(CFLAGS) -c $< -o $@

obj/driver_asm_%.o: src/drivers/%.asm
	nasm -f elf32 $< -o $@