#include <drivers/ata.h>

unsigned short base = 0x1F0;

void ata_set_base(unsigned short b) {
	base = b;
}

void ata_soft_reset(void) {
	if (inb(base + 7) == 0xFF) return;
	outb((base | 0x200) + 6, inb((base | 0x200) + 6) | 4);
	outb((base | 0x200) + 6, inb((base | 0x200) + 6) & 0b11111011);
}

bool ata_check_drive(bool isSlave, unsigned short * values) {
	// Check if any drives are present
	char t;
	if (inb(base + 7) == 0xFF) return false;
	outb(base + 2, 0);
	outb(base + 3, 0);
	outb(base + 4, 0);
	outb(base + 5, 0);
	outb(base + 6, 0xA0 | (isSlave << 4));
	outb(base + 7, 0xEC);
	if (inb(base + 7) == 0) return false;
	while (inb(base + 7) & 0x80);
	if (inb(base + 4) != 0 || inb(base + 5) != 0) return false;
	do t = inb(base + 7); while (!(t & 0x08) && !(t & 0x01));

}