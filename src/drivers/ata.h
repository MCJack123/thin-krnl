#include <stdlib.h>

#define REG_CYL_LO 4
#define REG_CYL_HI 5
#define REG_DEVSEL 6

enum atadev_type {
    ATADEV_PATAPI,
    ATADEV_SATAPI,
    ATADEV_PATA,
    ATADEV_SATA,
    ATADEV_UNKNOWN
};

struct DEVICE {
    unsigned short base;
    unsigned short dev_ctl;
};

struct driverdata {
    unsigned int partition_size;
    unsigned char dcr;
    unsigned int start_lba;
    unsigned short task_file;
    unsigned char slavebit;
};

extern void ata_soft_reset(struct DEVICE *dev);
extern int ata_detect_dev_type(bool slavebit, struct DEVICE *ctrl);
extern void ata_read_sectors(void * dest, unsigned short sectors, unsigned int lba, struct devicedata *device);