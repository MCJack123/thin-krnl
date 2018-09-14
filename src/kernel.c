/*
 * kernel.c
 * Main code
 */

#include <stdio.h>
#include <interrupt.h>
#include <drivers/ata.h>
#include <interpret.h>

void kmain(void) {
    unsigned int* lower;
    char * text;
    const char * done = " Done.\n";
    void * buf = (void*)0x500000;
    unsigned int i;
    struct driverdata disk;
    //lasm("sti");
    clear();
	print("Calculating memory...\n");
    lower = get_mem_size();
    print("Memory size: ");
    print(itoa(lower[0]));
    print(" bytes\n");
    //io_wait();
    //rsleep(200000000);
    //setupInterrupts();
    /*disk.slavebit = 0;
    disk.start_lba = 2048;
    disk.dcr = 0;
    disk.task_file = 0x1F0;
    disk.partition_size = 64*1048576;
    //buf = malloc(512);
    for (i = 0; i < 1024; i++) ((char*)buf)[i] = 0;
    print("Fetching sector...");
    ata_lba_read(buf, 1, 9000);
    print(done);*/
    print("\nThinKrnl console\nType 'help' for help.\n");
    while (true) {
        print("> ");
        text = scanl();
        if (run_command(text)) break;
    }
    /*print("\0330eText: ");
    print(text);
    print("\03307");*/
    //beep();
    print("\n\n\03304System halted.");
    return;
}
