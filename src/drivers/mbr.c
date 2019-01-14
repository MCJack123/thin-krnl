#include <drivers/mbr.h>

chs_t mbr_convert_chs(unsigned int c) {
    chs_t retval;
    retval.head = (c & 0xFF0000) >> 16;
    retval.sector = (c & 0x003F00) >> 8;
    retval.cylinder = ((c & 0x00C000) >> 6) | (c & 0x0000FF);
    return retval;
}

partition_entry_t * mbr_read_entries(bool isSlave) {
    int i;
    char j;
    partition_entry_t retval[4];
    char * sector = malloc(512);
    ata_soft_reset();
    if (!ata_read_sectors(isSlave, 1, 0, sector)) return NULL;
    for (i = 0x1BE, j = 0; i < 0x1FE && j < 4; i+=16, j++) {
        retval[j].status = sector[i];
        retval[j].first_sector = mbr_convert_chs((sector[i+1] << 16) | (sector[i+2] << 8) | sector[i+3]);
        retval[j].type = sector[i+4];
        retval[j].last_sector = mbr_convert_chs((sector[i+5] << 16) | (sector[i+6] << 8) | sector[i+7]);
        retval[j].lba = ((unsigned int *)(&sector[i+8]))[0];
        retval[j].lba_size = ((unsigned int *)(&sector[i+8]))[1];
    }
    return retval;
}