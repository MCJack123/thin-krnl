#ifndef THINKRNL_DRIVER_ATA_H
#define THINKRNL_DRIVER_ATA_H
#include <stdlib.h>

extern void ata_set_base(unsigned short);
extern void ata_soft_reset(void);
extern bool ata_check_drive(bool, unsigned short *);

#endif