#include "interrupt.h"
#include "stdio.h"

#define create_isr_wrapper(irq)\
unsigned int irq_##irq##_handler()\
{\
   unsigned int   current_offset;\
   asm goto("jmp %l[isr_end]":::"memory":isr_end);\
\
   isr_start:\
   asm volatile("pushal\npushl %1\ncall %0\nadd $4, %%esp\npopal\niret"::"r"((unsigned int) 0x01234567), "r"((unsigned int) irq):"memory");\
   isr_end:\
\
   current_offset = 0;\
   while (*((unsigned int*) (&&isr_start + current_offset)) != 0x01234567)\
   {\
      current_offset++;\
   }\
   *((unsigned int*) (&&isr_start + current_offset)) = irq_handler;\
   return (unsigned int) &&isr_start;\
}

void * interrupt_table = (void*)0x200020;
void * myTss = (void*)0x300020;
struct GDT gdtTable[4];
struct IDTDescr idtTable[16];

extern void setIdt(void *, unsigned int, unsigned int);
extern void setGdt(void *, unsigned int, unsigned int);
extern void reloadSegments();

// section: interrupt handlers

void irq_handler(unsigned int irq) {
    print("Interrupt: ");
    /*print(htoa(frame->ip));
    print(htoa(frame->cs));
    print(htoa(frame->flags));
    print(htoa(frame->sp));
    print(htoa(frame->ss));*/
    print(htoa(irq));
    print("\n");
}

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */
/*
arguments:
	offset1 - vector offset for master PIC
		vectors on the master become offset1..offset1+7
	offset2 - same for slave PIC: offset2..offset2+7
*/

void setupGDT() {
    gdtTable[0] = (struct GDT){0, 0, 0};                     // Selector 0x00 cannot be used
    gdtTable[1] = (struct GDT){.base=0, .limit=0xffffffff, .type=0x9A};         // Selector 0x08 will be our code
    gdtTable[2] = (struct GDT){.base=0x400000, .limit=0xffffffff, .type=0x92};         // Selector 0x10 will be our data
    gdtTable[3] = (struct GDT){.base=&myTss, .limit=0x70, .type=0x89}; // You can use LTR(0x18)
}


create_isr_wrapper(0x00);
create_isr_wrapper(0x01);
create_isr_wrapper(0x02);
create_isr_wrapper(0x03);
create_isr_wrapper(0x04);
create_isr_wrapper(0x05);
create_isr_wrapper(0x06);
create_isr_wrapper(0x07);
create_isr_wrapper(0x08);
create_isr_wrapper(0x09);
create_isr_wrapper(0x0A);
create_isr_wrapper(0x0B);
create_isr_wrapper(0x0C);
create_isr_wrapper(0x0D);
create_isr_wrapper(0x0E);
create_isr_wrapper(0x0F);

void setupIDT() {
    //for (int i = 0; i < 256; i++)
    //    idtTable[i] = (struct IDTDescr){(unsigned short)((unsigned int)(&base_interrupt_handler) & 0xFFFF), 8, 0, 0x9E, (unsigned short)(((unsigned int)(&base_interrupt_handler) & 0xFFFF0000) >> 16)};
}

void encodeGdtEntry(unsigned char *target, struct GDT source)
{
    // Check the limit to make sure that it can be encoded
    if ((source.limit > 65536) && (source.limit & 0xFFF) != 0xFFF) {
        return;
    }
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}

void PIC_remap(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void PIC_sendEOI(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}

void IRQ_set_mask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    unsigned short port;
    unsigned char value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

void setup_p3() {
    for (int i = 0; i < 16; i++) IRQ_clear_mask(i);
    asm("mov edx,[0x100018]\nmov [ebp + 4],edx"); // needs fix
    //asm("sti");
}

void setup_p2() {
    reloadSegments();
    setupIDT();
    PIC_remap(0x20, 0x28);
    setIdt(&idtTable, 2048, (unsigned int)&setup_p3);
}

void setupInterrupts() {
    asm("mov edx,[ebp + 4]\nmov [0x100018],edx"); // needs fix
    unsigned char * gdt_table = (unsigned char*)0x100020;
    encodeGdtEntry(gdt_table, gdtTable[0]);
    encodeGdtEntry(&gdt_table[8], gdtTable[1]);
    encodeGdtEntry(&gdt_table[16], gdtTable[2]);
    encodeGdtEntry(&gdt_table[24], gdtTable[3]);
    setGdt((void*)gdt_table, 32, (unsigned int)&setup_p2);
}