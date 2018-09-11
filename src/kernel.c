/*
 * kernel.c
 * Main code
 */

#include <stdio.h>
#include <interrupt.h>
#include <drivers/ata.h>

void kmain(void) {
    unsigned int* lower;
    const char * text;
    void * buf;
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
    print("Fetching sector...");
    disk.slavebit = 0;
    disk.start_lba = 2048;
    disk.dcr = 0;
    disk.task_file = 0;
    disk.partition_size = 64*1048576;
    buf = malloc(512);
    ata_read_sectors(buf, 1, 9082, &disk);
    print(" Done.\n");
    print((const char *)buf);
    print("\n> ");
    //rsleep(200000000);
    text = scanl();
    //while (true) ;
    print("\0330eText: ");
    print(text);
    print("\03307");
    //beep();
    print("\n\n\03304CPU halted.");
    return;
}
