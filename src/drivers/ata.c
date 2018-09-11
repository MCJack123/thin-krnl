#include <drivers/ata.h>

void ata_soft_reset(struct DEVICE *dev){
	outb(dev->dev_ctl, 0x04);
	outb(dev->dev_ctl, 0x00);
}

/* on Primary bus: ctrl->base =0x1F0, ctrl->dev_ctl =0x3F6. REG_CYL_LO=4, REG_CYL_HI=5, REG_DEVSEL=6 */
int ata_detect_dev_type (bool slavebit, struct DEVICE *ctrl)
{
	ata_soft_reset(ctrl);		/* waits until master drive is ready again */
	outb(ctrl->base + REG_DEVSEL, 0xA0 | slavebit<<4);
	inb(ctrl->dev_ctl);			/* wait 400ns for drive select to work */
	inb(ctrl->dev_ctl);
	inb(ctrl->dev_ctl);
	inb(ctrl->dev_ctl);
	unsigned cl=inb(ctrl->base + REG_CYL_LO);	/* get the "signature bytes" */
	unsigned ch=inb(ctrl->base + REG_CYL_HI);
 
	/* differentiate ATA, ATAPI, SATA and SATAPI */
	if (cl==0x14 && ch==0xEB) return ATADEV_PATAPI;
	if (cl==0x69 && ch==0x96) return ATADEV_SATAPI;
	if (cl==0 && ch == 0) return ATADEV_PATA;
	if (cl==0x3c && ch==0xc3) return ATADEV_SATA;
	return ATADEV_UNKNOWN;
}