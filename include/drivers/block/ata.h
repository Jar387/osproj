#ifndef DRIVERS_BLOCK_ATA_H
#define DRIVERS_BLOCK_ATA_H

#define PRIM_CHNN_BASE 0x1f0
#define SCND_CHNN_BASE 0x170

#define PRIM_CHNN_IRQ 14
#define SCND_CHNN_IRQ 15

#define PRIM_CHNN_VEC 0x21
#define SCND_CHNN_VEC 0x22

#define PRIM_CHNN_CTRL 0x3f6
#define SCND_CHNN_CTRL 0x376

#define ATA_DATA 0
#define ATA_ERROR 1
#define ATA_FEATURE 1
#define ATA_SECT 2
#define ATA_LBALOW 3
#define ATA_LBAMID 4
#define ATA_LBAHI 5
#define ATA_DRIVE 6
#define ATA_STATUS 7
#define ATA_CMD 7

#define PRIM_DATA PRIM_CHNN_BASE
#define PRIM_ERROR PRIM_CHNN_BASE+1
#define PRIM_FEATURE PRIM_CHNN_BASE+1
#define PRIM_SECT PRIM_CHNN_BASE+2
#define PRIM_LBALOW PRIM_CHNN_BASE+3
#define PRIM_LBAMID PRIM_CHNN_BASE+4
#define PRIM_LBAHI PRIM_CHNN_BASE+5
#define PRIM_DRIVE PRIM_CHNN_BASE+6
#define PRIM_STATUS PRIM_CHNN_BASE+7
#define PRIM_CMD PRIM_CHNN_BASE+7

#define SCND_DATA SCND_CHNN_BASE
#define SCND_ERROR SCND_CHNN_BASE+1
#define SCND_FEATURE SCND_CHNN_BASE+1
#define SCND_SECT SCND_CHNN_BASE+2
#define SCND_LBALOW SCND_CHNN_BASE+3
#define SCND_LBAMID SCND_CHNN_BASE+4
#define SCND_LBAHI SCND_CHNN_BASE+5
#define SCND_DRIVE SCND_CHNN_BASE+6
#define SCND_STATUS SCND_CHNN_BASE+7
#define SCND_CMD SCND_CHNN_BASE+7

#define ATA_MASTER_DRV 0xa0
#define ATA_SLAVE_DRV 0xb0

#define ATA_STATUS_BSY 0x80
#define ATA_STATUS_REDY 0x40
#define ATA_STATUS_FAULT 0x20
#define ATA_STATUS_SEEK 0x10
#define ATA_STATUS_COMPLETE 0x8
#define ATA_STATUS_CORR 0x4
#define ATA_STATUS_IDX 0x2
#define ATA_STATUS_ERR 0x1

#define ATA_CMD_IDENTIFY 0xec

#define ATA_VALID 1
#define ATA_INVALID 0

typedef struct {
	unsigned int valid;
	unsigned short base_addr;
	unsigned char device_select;
	unsigned int sector_count;
} ata_device_t;

void ata_init();

extern void ata_prim_int();
extern void ata_scnd_int();

#endif
