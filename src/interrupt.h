#include <stdlib.h>
#define ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	    0x10		/* Initialization - required! */
 
#define ICW4_8086	    0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	    0x10		/* Special fully nested (not) */

#define PIC1_COMMAND    0x0020
#define PIC1_DATA       0x0021
#define PIC2_COMMAND    0x00A0
#define PIC2_DATA       0x00A1

#define PIC_EOI		    0x20		/* End-of-interrupt command code */

struct GDT {
    int base;
    int limit;
    unsigned char type;
};

struct IDTDescr {
   unsigned short offset_1; // offset bits 0..15
   unsigned short selector; // a code segment selector in GDT or LDT
   unsigned char zero;      // unused, set to 0
   unsigned char type_attr; // type and attributes, see below
   unsigned short offset_2; // offset bits 16..31
};

struct interrupt_frame
{
    unsigned short ip;
    unsigned short cs;
    unsigned short flags;
    unsigned short sp;
    unsigned short ss;
};

extern void * interrupt_table;

extern void setupInterrupts();