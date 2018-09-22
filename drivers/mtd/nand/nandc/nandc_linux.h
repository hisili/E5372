/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_linux.h
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
1.date:2011-07-27
 question number:
 modify reasion:        create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_LINUX_H_
#define _NANDC_LINUX_H_
    
#ifdef __cplusplus
extern "C"
{
#endif


#define MTD_MAX_OOBFREE_ENTRIES	        8
/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		            0
#define NAND_CMD_READ1		            1
#define NAND_CMD_RNDOUT		            5
#define NAND_CMD_PAGEPROG               0x10
#define NAND_CMD_READOOB                0x50
#define NAND_CMD_ERASE1		            0x60
#define NAND_CMD_STATUS		            0x70
#define NAND_CMD_STATUS_MULTI	        0x71
#define NAND_CMD_SEQIN		            0x80
#define NAND_CMD_RNDIN		            0x85
#define NAND_CMD_READID		            0x90
#define NAND_CMD_ERASE2		            0xd0
#define NAND_CMD_RESET		            0xff

#define NAND_CMD_LOCK		            0x2a
#define NAND_CMD_UNLOCK1	            0x23
#define NAND_CMD_UNLOCK2	            0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	            0x30
#define NAND_CMD_RNDOUTSTART	        0xE0
#define NAND_CMD_CACHEDPROG	            0x15

/* Extended commands for AG-AND device */
/*
 * Note: the command for NAND_CMD_DEPLETE1 is really 0x00 but
 *       there is no way to distinguish that from NAND_CMD_READ0
 *       until the remaining sequence of commands has been completed
 *       so add a high order bit and mask it off in the command.
 */
#define NAND_CMD_DEPLETE1	            0x100
#define NAND_CMD_DEPLETE2	            0x38
#define NAND_CMD_STATUS_MULTI	        0x71
#define NAND_CMD_STATUS_ERROR	        0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	        0x73
#define NAND_CMD_STATUS_ERROR1	        0x74
#define NAND_CMD_STATUS_ERROR2	        0x75
#define NAND_CMD_STATUS_ERROR3	        0x76
#define NAND_CMD_STATUS_RESET	        0x7f
#define NAND_CMD_STATUS_CLEAR	        0xff

#define NAND_CMD_NONE		            -1

/* Status bits */
#define NAND_STATUS_FAIL	            0x01
#define NAND_STATUS_FAIL_N1	            0x02
#define NAND_STATUS_TRUE_READY	        0x20
#define NAND_STATUS_READY	            0x40
#define NAND_STATUS_WP		            0x80


/* Option constants for bizarre disfunctionality and real
*  features
*/
/* Chip can not auto increment pages */
#define NAND_NO_AUTOINCR	            0x00000001
/* Buswitdh is 16 bit */
#define NAND_BUSWIDTH_16	            0x00000002
/* Device supports partial programming without padding */
#define NAND_NO_PADDING		            0x00000004
/* Chip has cache program function */
#define NAND_CACHEPRG		            0x00000008
/* Chip has copy back function */
#define NAND_COPYBACK		            0x00000010
/* AND Chip which has 4 banks and a confusing page / block
 * assignment. See Renesas datasheet for further information */
#define NAND_IS_AND		                0x00000020
/* Chip has a array of 4 pages which can be read without
 * additional ready /busy waits */
#define NAND_4PAGE_ARRAY	            0x00000040
/* Chip requires that BBT is periodically rewritten to prevent
 * bits from adjacent blocks from 'leaking' in altering data.
 * This happens with the Renesas AG-AND chips, possibly others.  */
#define BBT_AUTO_REFRESH	            0x00000080
/* Chip does not require ready check on read. True
 * for all large page devices, as they do not support
 * autoincrement.*/
#define NAND_NO_READRDY		            0x00000100
/* Chip does not allow subpage writes */
#define NAND_NO_SUBPAGE_WRITE	        0x00000200
/* Chip stores bad block marker on the last page of the eraseblock */
#define NAND_BB_LAST_PAGE	            0x00000400

/* Device is one of 'new' xD cards that expose fake nand command set */
#define NAND_BROKEN_XD		            0x00000400

/* Device behaves just like nand, but is readonly */
#define NAND_ROM		                0x00000800

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS \
	(NAND_NO_PADDING | NAND_CACHEPRG | NAND_COPYBACK)

/* Macros to identify the above */
#define NAND_CANAUTOINCR(chip)          (!(chip->options & NAND_NO_AUTOINCR))
#define NAND_MUST_PAD(chip)             (!(chip->options & NAND_NO_PADDING))
#define NAND_HAS_CACHEPROG(chip)        ((chip->options & NAND_CACHEPRG))
#define NAND_HAS_COPYBACK(chip)         ((chip->options & NAND_COPYBACK))
/* Large page NAND with SOFT_ECC should support subpage reads */
#define NAND_SUBPAGE_READ(chip)         ((chip->ecc.mode == NAND_ECC_SOFT) \
					                        && (chip->page_shift > 9))

/* Mask to zero out the chip options, which come from the id table */
#define NAND_CHIPOPTIONS_MSK	        (0x0000ffff & ~NAND_NO_AUTOINCR)

/* Non chip related options */
/* Use a flash based bad block table. This option is passed to the
 * default bad block table function. */
#define NAND_USE_FLASH_BBT	            0x00010000
/* This option skips the bbt scan during initialization. */
#define NAND_SKIP_BBTSCAN	            0x00020000
/* This option is defined if the board driver allocates its own buffers
   (e.g. because it needs them DMA-coherent */
#define NAND_OWN_BUFFERS	            0x00040000
/* Chip may not exist, so silence any errors in scan */
#define NAND_SCAN_SILENT_NODEV	        0x00080000

/* Options set by nand scan */
/* Nand scan has allocated controller struct */
#define NAND_CONTROLLER_ALLOC	        0x80000000

/* Cell info constants */
#define NAND_CI_CHIPNR_MSK	            0x03
#define NAND_CI_CELLTYPE_MSK	        0x0C


struct nand_oobfree
{
    u32 offset;
    u32 length;
};

struct mtd_partition {
	char *name;			/* identifier string */
    u32 reserved;		/* reserved for 64bit aligne */
	FSZ size;			/* partition size */
	FSZ offset;		/* offset within the master MTD space */
	u32 mask_flags;		/* master MTD flags to mask out for this partition */
	struct nand_ecclayout *ecclayout;	/* out of band layout for this partition (NAND only)*/
};

struct nand_ecclayout
{
    u32 eccbytes;
    u32 eccpos[64];
    u32 oobavail;
    struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES];
};

#define NAND_MAX_OOBSIZE	436
#define NAND_MAX_PAGESIZE	8192

struct nand_buffers {
	u8 ecccalc[NAND_MAX_OOBSIZE];
	u8 ecccode[NAND_MAX_OOBSIZE];
	u8 databuf[NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE];
};

/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec
#define NAND_MFR_FUJITSU	0x04
#define NAND_MFR_NATIONAL	0x8f
#define NAND_MFR_RENESAS	0x07
#define NAND_MFR_STMICRO	0x20
#define NAND_MFR_HYNIX		0xad
#define NAND_MFR_MICRON		0x2c
#define NAND_MFR_AMD		0x01

/**
 * struct nand_flash_dev - NAND Flash Device ID Structure
 * @name:	Identify the device type
 * @id:		device ID code
 * @pagesize:	Pagesize in bytes. Either 256 or 512 or 0
 *		If the pagesize is 0, then the real pagesize
 *		and the eraseize are determined from the
 *		extended id bytes in the chip
 * @erasesize:	Size of an erase block in the flash device.
 * @chipsize:	Total chipsize in Mega Bytes
 * @options:	Bitfield to store chip relevant options
 */
struct nand_flash_dev {
	char *name;
	int id;
	unsigned long pagesize;
	unsigned long chipsize;
	unsigned long erasesize;
	unsigned long options;
};

/**
 * struct nand_manufacturers - NAND Flash Manufacturer ID Structure
 * @name:	Manufacturer name
 * @id:		manufacturer ID code of device.
*/
struct nand_manufacturers {
	int id;
	char * name;
};

extern struct nand_flash_dev nand_flash_ids[];
extern struct nand_manufacturers nand_manuf_ids[];

/*
 * helper functions
 */
/*#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))*/

#define min_t(type, x, y)  (((type)(x) < (type)(y)) ? (x) : (y))

#define max_t(type, x, y)  (((type)(x) > (type)(y)) ? (x) : (y))

static __inline int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_LINUX_H_*/

