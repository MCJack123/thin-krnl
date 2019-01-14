CFLAGS=-nostdlib -nodefaultlibs -m32 -g -nostdinc -I./src
LIBALLOC_CFLAGS=-O2 -fno-builtin -fPIC -Wall $(CFLAGS)

IDIR=src
ODIR=obj

_DEPS = stdlib.h liballoc.h stdio.h interrupt.h interpret.h hexedit.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = kasm.o kernel.o interrupt.o kernel_alloc.o liballoc.o stdio.o stdlib.o interpret.o hexedit.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_DRIVER_OBJ = ata.o
DRIVER_OBJ = $(patsubst %,$(ODIR)/driver_%,$(_DRIVER_OBJ))

_FS_OBJ = fs.o fat.o
FS_OBJ = $(patsubst %,$(ODIR)/fs_%,$(_DRIVER_OBJ))

kernel: $(OBJ) $(DRIVER_OBJ) $(FS_OBJ)
	$(info $< $@)
	ld -m elf_i386 -T link.ld -o kernel $(OBJ) $(DRIVER_OBJ) $(FS_OBJ)

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

obj/fs_%.o: src/fs/%.cpp src/fs/%.hpp src/fs/fs_base.hpp src/fs/fs.h $(DEPS)
	$(info $< $@)
	g++ $(CFLAGS) -c $< -o $@