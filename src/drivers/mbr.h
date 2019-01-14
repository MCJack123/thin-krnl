#ifndef THINKRNL_DRIVER_MBR_H
#define THINKRNL_DRIVER_MBR_H
#include <stdlib.h>
#include <drivers/ata.h>

typedef struct chs {
    short cylinder;
    unsigned char head;
    char sector;
} chs_t;

typedef struct partition_entry {
    unsigned char status;
    chs_t first_sector;
    unsigned char type;
    chs_t last_sector;
    unsigned int lba;
    unsigned int lba_size;
} partition_entry_t;

// Converts a 3-byte CHS value into a chs_t struct.
extern chs_t mbr_convert_chs(unsigned int);

// Returns an array of four partitions from the MBR.
extern partition_entry_t * mbr_read_entries(bool);

#endif