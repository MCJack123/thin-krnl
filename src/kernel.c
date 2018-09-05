/*
 * kernel.c
 * Main code
 */

#include "stdio.h"
#include "interrupt.h"

void kmain(void) {
    unsigned int* lower;
    const char * text;
    setupInterrupts();
    //lasm("sti");
    clear();
	print("Calculating memory...\n");
    lower = get_mem_size();
    print("Memory size: ");
    print(itoa(lower[0]));
    print(" bytes\n");
    print("\n> ");
    text = scanl();
    print("\0330eText: ");
    print(text);
    print("\03307");
    //beep();
    print("\n\n\03304Stopping...");
    return;
}
