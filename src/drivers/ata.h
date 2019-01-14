#ifndef THINKRNL_DRIVER_ATA_H
#define THINKRNL_DRIVER_ATA_H
#include <stdlib.h>

// Sets the base address of the IO operations (default is 0x1F0).
extern void ata_set_base(unsigned short);

// Soft-resets the bus.
extern void ata_soft_reset(void);

// Checks if a drive is present, and if so stores info from the IDENTIFY command in a buffer.
extern bool ata_check_drive(bool, unsigned short *);

// Reads a number of sectors into a buffer.
extern bool ata_read_sectors(bool, unsigned char, unsigned int, unsigned short *);

// Writes a number of sectors from a buffer.
extern bool ata_write_sectors(bool, unsigned char, unsigned int, unsigned short *);

#endif