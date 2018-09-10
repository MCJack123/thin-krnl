CFLAGS=-nostdlib -nodefaultlibs -m32 -g -nostdinc -I./src
LIBALLOC_CFLAGS=-O2 -fno-builtin -fPIC -Wall $(CFLAGS)

IDIR=src
ODIR=obj

_DEPS = stdlib.h liballoc.h stdio.h interrupt.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = interrupt.o kernel_alloc.o kernel.o kasm.o liballoc.o stdio.o stdlib.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DRIVER_OBJ = ata.o asm_ata.o
DRIVER_OBJ = $(patsubst %,$(ODIR)/driver_%,$(_DRIVER_OBJ))

kernel: $(OBJ) $(DRIVER_OBJ)
	$(info $< $@)
	ld -m elf_i386 -T link.ld -o kernel $(OBJ) $(DRIVER_OBJ)

obj/liballoc.o: src/liballoc.c src/liballoc.h
	$(info $< $@)
	gcc $(LIBALLOC_CFLAGS) -static -c src/liballoc.c -o obj/liballoc.o

obj/kasm.o: src/kernel.asm
	$(info $< $@)
	nasm -f elf32 src/kernel.asm -o obj/kasm.o

obj/%.o: src/%.c $(DEPS)
	$(info $< $@)
	gcc $(CFLAGS) -c $< -o $@

obj/driver_%.o: src/drivers/%.c src/drivers/%.h $(DEPS)
	$(info $< $@)
	gcc $(CFLAGS) -c $< -o $@

obj/driver_asm_%.o: src/drivers/%.asm
	$(info $< $@)
	nasm -f elf32 $< -o $@