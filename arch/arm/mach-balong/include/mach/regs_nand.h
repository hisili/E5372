/* arch/arm/mach-s3c2410/include/mach/regs-nand.h
 *
 * Copyright (c) 2004-2005 Simtec Electronics <linux@simtec.co.uk>
 *	http://www.simtec.co.uk/products/SWLINUX/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * S3C2410 NAND register definitions
*/

#include <linux/semaphore.h>

#ifndef __ASM_ARM_REGS_NAND
#define __ASM_ARM_REGS_NAND

#define BALONGV7R1_NFREG(x) (x)

#define BALONGV7R1_NFCONF  BALONGV7R1_NFREG(0x00)
#define BALONGV7R1_NFCMD   BALONGV7R1_NFREG(0x04)
#define BALONGV7R1_NFADDR  BALONGV7R1_NFREG(0x08)
#define BALONGV7R1_NFDATA  BALONGV7R1_NFREG(0x0C)
#define BALONGV7R1_NFSTAT  BALONGV7R1_NFREG(0x10)
#define BALONGV7R1_NFECC   BALONGV7R1_NFREG(0x14)

#define S3C2440_NFCONT   BALONGV7R1_NFREG(0x04)
#define S3C2440_NFCMD    BALONGV7R1_NFREG(0x08)
#define S3C2440_NFADDR   BALONGV7R1_NFREG(0x0C)
#define S3C2440_NFDATA   BALONGV7R1_NFREG(0x10)
#define S3C2440_NFECCD0  BALONGV7R1_NFREG(0x14)
#define S3C2440_NFECCD1  BALONGV7R1_NFREG(0x18)
#define S3C2440_NFECCD   BALONGV7R1_NFREG(0x1C)
#define S3C2440_NFSTAT   BALONGV7R1_NFREG(0x20)
#define S3C2440_NFESTAT0 BALONGV7R1_NFREG(0x24)
#define S3C2440_NFESTAT1 BALONGV7R1_NFREG(0x28)
#define S3C2440_NFMECC0  BALONGV7R1_NFREG(0x2C)
#define S3C2440_NFMECC1  BALONGV7R1_NFREG(0x30)
#define S3C2440_NFSECC   BALONGV7R1_NFREG(0x34)
#define S3C2440_NFSBLK   BALONGV7R1_NFREG(0x38)
#define S3C2440_NFEBLK   BALONGV7R1_NFREG(0x3C)

#define S3C2412_NFSBLK		BALONGV7R1_NFREG(0x20)
#define S3C2412_NFEBLK		BALONGV7R1_NFREG(0x24)
#define S3C2412_NFSTAT		BALONGV7R1_NFREG(0x28)
#define S3C2412_NFMECC_ERR0	BALONGV7R1_NFREG(0x2C)
#define S3C2412_NFMECC_ERR1	BALONGV7R1_NFREG(0x30)
#define S3C2412_NFMECC0		BALONGV7R1_NFREG(0x34)
#define S3C2412_NFMECC1		BALONGV7R1_NFREG(0x38)
#define S3C2412_NFSECC		BALONGV7R1_NFREG(0x3C)

#define BALONGV7R1_NFCONF_EN          (1<<15)
#define BALONGV7R1_NFCONF_512BYTE     (1<<14)
#define BALONGV7R1_NFCONF_4STEP       (1<<13)
#define BALONGV7R1_NFCONF_INITECC     (1<<12)
#define BALONGV7R1_NFCONF_nFCE        (1<<11)
#define BALONGV7R1_NFCONF_TACLS(x)    ((x)<<8)
#define BALONGV7R1_NFCONF_TWRPH0(x)   ((x)<<4)
#define BALONGV7R1_NFCONF_TWRPH1(x)   ((x)<<0)

#define BALONGV7R1_NFSTAT_BUSY        (1<<0)

#define S3C2440_NFCONF_BUSWIDTH_8	(0<<0)
#define S3C2440_NFCONF_BUSWIDTH_16	(1<<0)
#define S3C2440_NFCONF_ADVFLASH		(1<<3)
#define S3C2440_NFCONF_TACLS(x)		((x)<<12)
#define S3C2440_NFCONF_TWRPH0(x)	((x)<<8)
#define S3C2440_NFCONF_TWRPH1(x)	((x)<<4)

#define S3C2440_NFCONT_LOCKTIGHT	(1<<13)
#define S3C2440_NFCONT_SOFTLOCK		(1<<12)
#define S3C2440_NFCONT_ILLEGALACC_EN	(1<<10)
#define S3C2440_NFCONT_RNBINT_EN	(1<<9)
#define S3C2440_NFCONT_RN_FALLING	(1<<8)
#define S3C2440_NFCONT_SPARE_ECCLOCK	(1<<6)
#define S3C2440_NFCONT_MAIN_ECCLOCK	(1<<5)
#define S3C2440_NFCONT_INITECC		(1<<4)
#define S3C2440_NFCONT_nFCE		(1<<1)
#define S3C2440_NFCONT_ENABLE		(1<<0)

#define S3C2440_NFSTAT_READY		(1<<0)
#define S3C2440_NFSTAT_nCE		(1<<1)
#define S3C2440_NFSTAT_RnB_CHANGE	(1<<2)
#define S3C2440_NFSTAT_ILLEGAL_ACCESS	(1<<3)

#define S3C2412_NFCONF_NANDBOOT		(1<<31)
#define S3C2412_NFCONF_ECCCLKCON	(1<<30)
#define S3C2412_NFCONF_ECC_MLC		(1<<24)
#define S3C2412_NFCONF_TACLS_MASK	(7<<12)	/* 1 extra bit of Tacls */

#define S3C2412_NFCONT_ECC4_DIRWR	(1<<18)
#define S3C2412_NFCONT_LOCKTIGHT	(1<<17)
#define S3C2412_NFCONT_SOFTLOCK		(1<<16)
#define S3C2412_NFCONT_ECC4_ENCINT	(1<<13)
#define S3C2412_NFCONT_ECC4_DECINT	(1<<12)
#define S3C2412_NFCONT_MAIN_ECC_LOCK	(1<<7)
#define S3C2412_NFCONT_INIT_MAIN_ECC	(1<<5)
#define S3C2412_NFCONT_nFCE1		(1<<2)
#define S3C2412_NFCONT_nFCE0		(1<<1)

#define S3C2412_NFSTAT_ECC_ENCDONE	(1<<7)
#define S3C2412_NFSTAT_ECC_DECDONE	(1<<6)
#define S3C2412_NFSTAT_ILLEGAL_ACCESS	(1<<5)
#define S3C2412_NFSTAT_RnB_CHANGE	(1<<4)
#define S3C2412_NFSTAT_nFCE1		(1<<3)
#define S3C2412_NFSTAT_nFCE0		(1<<2)
#define S3C2412_NFSTAT_Res1		(1<<1)
#define S3C2412_NFSTAT_READY		(1<<0)

#define S3C2412_NFECCERR_SERRDATA(x)	(((x) >> 21) & 0xf)
#define S3C2412_NFECCERR_SERRBIT(x)	(((x) >> 18) & 0x7)
#define S3C2412_NFECCERR_MERRDATA(x)	(((x) >> 7) & 0x3ff)
#define S3C2412_NFECCERR_MERRBIT(x)	(((x) >> 4) & 0x7)
#define S3C2412_NFECCERR_SPARE_ERR(x)	(((x) >> 2) & 0x3)
#define S3C2412_NFECCERR_MAIN_ERR(x)	(((x) >> 2) & 0x3)
#define S3C2412_NFECCERR_NONE		(0)
#define S3C2412_NFECCERR_1BIT		(1)
#define S3C2412_NFECCERR_MULTIBIT	(2)
#define S3C2412_NFECCERR_ECCAREA	(3)

//////////////////////////////////////////////////////////
#ifndef BSP_FALSE
#define BSP_FALSE                0
#endif

#ifndef BSP_TRUE
#define BSP_TRUE                 1
#endif

#ifndef BSP_NULL
#define BSP_NULL                 (void*)0
#endif

#ifndef BSP_OK
 #define BSP_OK (0)
#endif

#ifndef BSP_ERROR
 #define BSP_ERROR (-1)
#endif

/*NAND FLASH特性数据宏定义*/
#define NANDF_BUFFER_DATA_SIZE         0x800
#define NANDF_BUFFER_SPARE_SIZE        0x40
#define NANDF_BADFLAG_OFFSET           0x200

	
#define NANDF_WHOLE_BUFFER_SIZE     (NANDF_BUFFER_DATA_SIZE + NANDF_BUFFER_SPARE_SIZE)
	
/*NANDC寄存器*/
#define NANDC_CONFIG     0x0
#define NANDC_PWIDTH     0x4
#define NANDC_OPIDLE     0x8
#define NANDC_CMD        0xc
#define NANDC_ADDRL      0x10
#define NANDC_ADDRH      0x14
#define NANDC_DATANUM    0x18
#define NANDC_OP         0x1c
#define NANDC_STATUS     0x20
#define NANDC_INTEN      0x24
#define NANDC_INTSTATUS  0x28
#define NANDC_INTCLR     0x2c
#define NANDF_LOCKCFG    0x30
#define NANDC_LOCK_SA0   0x34
#define NANDC_LOCK_SA1   0x38
#define NANDC_LOCK_SA2   0x3c
#define NANDC_LOCK_SA3   0x40
#define NANDC_LOCK_EA0   0x44
#define NANDC_LOCK_EA1   0x48
#define NANDC_LOCK_EA2   0x4C
#define NANDC_LOCK_EA3   0x50
	
#define NANDF_LOCKADDRBEGIN(i)    (0x34 + (i)*4)
#define NANDF_LOCKADDREND(i)      (0x44 + (i)*4)

/*FLASH命令字宏定义*/
#define NANDF_CMD_READPAGE_FIRST           0x00
#define NANDF_CMD_READPAGE_SECOND          0x30
#define NANDF_CMD_STATUSREAD               0x70
#define NANDF_CMD_ERASE_FIRST              0x60
#define NANDF_CMD_ERASE_SECOND             0xD0
#define NANDF_CMD_PAGEPROG_FIRST           0x80
#define NANDF_CMD_PAGEPROG_SECOND          0x10
#define NANDF_CMD_READ_ID                  0x90


/*工作模式(NORMAN模式和BOOT模式)*/
#define NANDF_MODE_NORMAL      0x1
#define NANDF_MODE_BOOT        0x0
#define NANDF_MODE_INT         0x1
#define NANDF_MODE_POLL        0x0

/*器件配置寄存器*/
#define NANDF_ECC_ENABLE           0x20
#define NANDF_EXT_ECC_ENABLE       0x200
#define NANDF_ECC_NUM_4BIT         0x800
	
#define NANDF_ECC_TYPE_MASK        0xE20
#define NANDF_ECC_TYPE_SHIFT        10	
#define NANDF_LOCK_MASK            0x4
#define NANDF_LOCKALL_MASK         0x2
#define NANDF_LOCKED_NOT           0x0	
#define NANDF_LOCKED               0x1
	
/*操作寄存器*/
#define NANDF_READ_STAT_EN         0x1
#define NANDF_READ_STAT_DIS        0x0
#define NANDF_READ_DATA_EN         0x2
#define NANDF_READ_DATA_DIS        0x0
#define NANDF_WAIT_RB_HIGH_EN      0x4
#define NANDF_WAIT_RB_HIGH_DIS     0x0
#define NANDF_COM2_SEND_EN         0x8
#define NANDF_COM2_SEND_DIS        0x0
#define NANDF_WRITE_DATA_EN        0x10
#define NANDF_WRITE_DATA_DIS       0x0
#define NANDF_WRITE_ADDR_EN        0x20
#define NANDF_WRITE_ADDR_DIS       0x0
#define NANDF_COM1_SEND_EN         0x40
#define NANDF_COM1_SEND_DIS        0x0

#define NANDF_ADDR_1CYCLE          0x1
#define NANDF_ADDR_2CYCLE          0x2
#define NANDF_ADDR_3CYCLE          0x3
#define NANDF_ADDR_4CYCLE          0x4
#define NANDF_ADDR_5CYCLE          0x5


/*偏移位*/
#define NANDF_ADDR_CYCLE_SHIFT_BITS     0x9
#define NANDF_STATUS_SHIFT_BITS         0x10
#define NANDF_COMMAND2_SHIFT_BITS       0x8
#define NANDF_HIGH_ADDR_SHIFT_BITS      28
#define NANDF_LOW_ADDR_SHIFT_BITS       4

/*读写数据数目配置寄存器*/
#define NANDF_PAGE_MODE                 0x840

/*中断状态寄存器*/
#define NANDF_INTSTAT_READ_ERROR        0x40

/*中断清除寄存器*/
#define NANDF_INTCLEAN_READ_ERROR       0x40

#define NANDC_LOOP_MAX            100000
#define NANDC_MARKBAD_LOOP_MAX    3
#define NANDF_ID_LENGTH			  5
#define NANDF_STATUS_READ         1
#define NANDF_STATUS_READ_NOT     0
	
#define NANDF_READ_STATUS         1
#define NANDF_READ_STATUS_NOT     0	
#define NANDF_TRACE_LEVEL         300
#define NANDF_SUPPORT_TYPE        5
#define NANDF_PAGESIZE_4K         0x1000
#define NANDF_OP_SINGLE           0
#define NANDF_OP_TWICE            1
#define NANDF_BADBLOCK_FLAG_OFFSET    0x200

/* NAND Flash Maker ID */
#define NAND_MAKER_TOSHIBA	0x98
#define NAND_MAKER_SAMSUNG	0xec
#define NAND_MAKER_FUJITSU	0x04
#define NAND_MAKER_NATIONAL	0x8f
#define NAND_MAKER_RENESAS	0x07
#define NAND_MAKER_STMICRO	0x20
#define NAND_MAKER_HYNIX	0xad
#define NAND_MAKER_MICRON	0x2c
#define NAND_MAKER_AMD		0x01

/* NAND Flash Device ID  maker--totalsize(bit)--pagesize--sparesize--width*/
#define HYNIX_8G_2K_64_8       0xDC
#define SAMSUNG_1G_2K_64_8     0xA1
#define TOSHIBA_2G_1K_32_16    0xBA
#define HYNIX_1G_2K_64_8       0xA1
#define HYNIX_2G_1K_32_16      0xBA
#define TOSHIBA_2G_2K_64_8     0xAA
/* Flash Lock area*/
#define NAND_BOOT_LOCK_START   0x0 
#define NAND_BOOT_LOCK_END     0x1
#define NAND_IMAGE_LOCK_START  0x2
#define NAND_IMAGE_LOCK_END    0x119
#define NAND_FILESYS_LOCK_START 0x11a
#define NAND_FILESYS_LOCK_END   0x79b
#define NAND_NV_LOCK_START     0x79c
#define NAND_NV_LOCK_END       0x7ff
/**************************************************************************
  全局变量声明
**************************************************************************/
typedef unsigned int word_t;
typedef unsigned int bool_t;
typedef void void_t;

typedef short s16_t;
typedef unsigned short u16_t;
typedef int s32_t;
typedef unsigned int u32_t;
typedef char s8_t;
typedef unsigned char u8_t;
/**************************************************************************
  枚举定义
**************************************************************************/
/* NandF Pagesize */
typedef enum tagNANDF_PAGESIZE_E
{
	PAGESIZE_DMA = 0,
	PAGESIZE_SRCPRF = 5
} NANDF_PAGESIZE_E;

/* NandF OpType */
typedef enum tagNANDF_OPERATION_TYPE_E
{
	OP_READ = 0,
	OP_WRITE,
	OP_ERASE,
	OP_MARKBAD,
	OP_CHECKBAD,
	OP_YAFFSREAD = 5,
	OP_YAFFSWRITE,
	OP_YAFFSERASE,
	OP_READNOCHECK = 8,
	OP_MAX
} NANDF_OPERATION_TYPE_E;

/* NandF ecc_type */
typedef enum tagNANDF_ECC_TYPE_E
{
	ECCTYPE_NO = 0,
	ECCTYPE_1BIT,
	ECCTYPE_4BIT,
	ECCTYPE_8BIT
} NANDF_ECC_TYPE_E;

/* NandF lock_module */
typedef enum tagNANDF_LOCK_MODULE_E
{
	MODULE_BSP = 0,
	MODULE_PL,
	MODULE_PS,
	MODULE_3,
	MODULE_MAX
} NANDF_LOCK_MODULE_E;

/**************************************************************************
  STRUCT定义
**************************************************************************/
/*sturcture for NANDF */
typedef struct tagNANDF_DATA_S
{
	word_t u32PageSize; /* page size */
	word_t u32Width; /* data width */
	word_t u32CycleNum ; /* read/write address cycle number */
	word_t u32EraseCycleNum ; /* erase address cycle number */
	word_t u32BlockSize; /* block size */
	word_t u32BlockNum; /* block num */
	word_t u32SpareSize; /* spare Size */
} NANDF_DATA_S;

/*sturcture for NANDF */
typedef struct tagNANDF_DEFAULT_DATA_S
{
	u8_t u8Maker; /* maker */
	u8_t u8DeviceCode; /* Device Code */
	NANDF_DATA_S stNandCFG;  /* NandFlash Config Value */
} NANDF_DEFAULT_DATA_S;

/*sturcture of attr for NANDF */
typedef struct tagNANDF_ATTR_S
{
	bool_t bModuleInit; /* 初始化标志 */
	struct semaphore  u32FlashInitSem; /* 初始化同步信号量 */
	word_t u32TotalSize ; /* 总容量 */
	word_t u32WholePageSize ; /* 页总大小 */
	word_t u32PageBitWidth ; /* 页大小所占位宽 */
	word_t u32AddrChangeNum; /* 地址转换使用的算法索引（具体器件型号相关） */
	NANDF_DATA_S stNandCFG;  /* NandFlash Config Value */
} NANDF_ATTR_S;

/*sturcture of operation for NANDF */
typedef struct tagNANDF_OPERATION_S
{
	word_t u32PhyBaseAddr; /* 待操作的物理页基地址 */
	word_t u32OffsetInPage; /* 页内偏移 */
	word_t u32OPSize ; /* 读或者写大小 */
	u8_t * pu8DataBuffer ; /* 数据buffer指针 */
	u8_t * pu8SpareBuffer ; /* 冗余区buffer指针 */
	NANDF_OPERATION_TYPE_E enOPType; /* 操作类型 */
} NANDF_OPERATION_S;

typedef struct tagNANDF_APP_INFO_S
{
    word_t u32OPAreaStart;
    word_t u32OPAreaEnd;
    word_t u32PrtAreaStart;
    word_t u32PrtAreaEnd;
}NANDF_APP_INFO_S;

typedef struct tagNANDF_LOCKADDRESS_S
{
	word_t u32StartAddress; /* 锁定起始地址 */
	word_t u32EndAddress; /* 锁定结束地址 */
} NANDF_LOCKADDRESS_S;

/**************************************************************************
  UNION定义
**************************************************************************/


/**************************************************************************
  OTHERS定义
**************************************************************************/


/**************************************************************************
  函数声明
**************************************************************************/

#endif /* __ASM_ARM_REGS_NAND */

