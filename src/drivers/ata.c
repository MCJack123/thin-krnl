#include <drivers/ata.h>
#include <stdio.h>

unsigned short base = 0x1F0;

unsigned short inb16(unsigned short port)
{
    unsigned short ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

void outb16(unsigned short port, unsigned short val)
{
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

void ata_set_base(unsigned short b) {
	base = b & ~0x000F;
}

void ata_soft_reset(void) {
	if (inb(base + 7) == 0xFF) return;
	outb((base | 0x200) + 6, inb((base | 0x200) + 6) | 4);
	outb((base | 0x200) + 6, inb((base | 0x200) + 6) & 0b11111011);
	rsleep(2000);
}

bool ata_check_drive(bool isSlave, unsigned short * values) {
	// Check if any drives are present
	unsigned char t;
	print("Checking drive\n");
	if (inb(base + 7) == 0xFF) return false;
	ata_soft_reset();
	rsleep(200000);
	outb(base + 6, 0xA0 | (isSlave << 4));
	outb(base + 2, 0);
	outb(base + 3, 0);
	outb(base + 4, 0);
	outb(base + 5, 0);
	outb(base + 7, 0xEC);
	if (inb(base + 7) == 0) return false;
	while (inb(base + 7) & 0x80);
	if (inb(base + 4) != 0 || inb(base + 5) != 0) return false;
	do t = inb(base + 7); while (!(t & 0x08) && !(t & 0x01));
	if (t & 1) return false;
	print("Reading info\n");
	if (values != NULL) {
		for (t = 0; t < 255; t++) values[t] = inb16(base);
		values[t] = inb16(base);
	} else {
		for (t = 0; t < 255; t++) inb16(base);
		inb16(base);
	}
	return true;
}

bool ata_read_sectors(bool isSlave, unsigned char sector_count, unsigned int LBA, unsigned short * buf) {
	unsigned int lbasize;
	short i;
	unsigned char t;
	if (!ata_check_drive(isSlave, buf)) return false;
	print("Initializing\n");
	lbasize = (buf[60] << 16) | buf[61];
	print(itoa(lbasize));
	//if (lbasize == 0 || LBA + sector_count > lbasize) return false;
	outb(base + 6, 0xE0 | (isSlave << 4) | ((LBA >> 24) & 0x0F));
	outb(base + 3, (unsigned char) LBA);
	outb(base + 4, (unsigned char)(LBA >> 8));
	outb(base + 5, (unsigned char)(LBA >> 16));
	outb(base + 7, 0x20);
	print("Reading sectors\n");
	for (i = 0; i < sector_count; i++) {
		do t = inb(base + 7);
		while ((!(t & 0x80) & (t & 8)) && (t & 1) && (t & 0x20));
		if ((t & 1) || (t & 0x20)) {
			ata_soft_reset();
			return false;
		}
		for (t = 0; t < 255; t++) buf[i * 256 + t] = inb16(base);
		buf[i * 256 + t] = inb16(base);
		print("Read sector ");
		print(itoa(i));
		print("\n");
		rsleep(400);
	}
	return true;
}

bool ata_write_sectors(bool isSlave, unsigned char sector_count, unsigned int LBA, unsigned short * buf) {
	unsigned int lbasize;
	short i;
	unsigned char t;
	unsigned short info[256];
	if (!ata_check_drive(isSlave, info)) return false;
	lbasize = (buf[60] << 16) | buf[61];
	if (lbasize == 0 || LBA + sector_count > lbasize) return false;
	outb(base + 6, 0xE0 | (isSlave << 4) | ((LBA >> 24) & 0x0F));
	outb(base + 3, (unsigned char) LBA);
	outb(base + 4, (unsigned char)(LBA >> 8));
	outb(base + 5, (unsigned char)(LBA >> 16));
	outb(base + 7, 0x30);
	for (i = 0; i < sector_count; i++) {
		do t = inb(base + 7);
		while ((!(t & 0x80) & (t & 8)) && (t & 1) && (t & 0x20));
		if ((t & 1) || (t & 0x20)) {
			ata_soft_reset();
			return false;
		}
		for (t = 0; t < 255; t++) {outb16(base, buf[i * 256 + t]); rsleep(10);}
		outb16(base, buf[i * 256 + t]);
		rsleep(400);
	}
	return true;
}