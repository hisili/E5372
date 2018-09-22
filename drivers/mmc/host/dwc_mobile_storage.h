#ifndef _DWC_MOBILE_STORAGE_H_
#define _DWC_MOBILE_STORAGE_H_
#include "BSP.h"
/*macro definition*/

/*adapte from vxWorks to Linux*/
#define BALONG_USE_LINUX				(1) //this need to be deleted when release

#define SD_MAX_OPFREQUENCY			24000
#define MMC_MAX_OPFREQUENCY		20000

#define CARD_OCR_RANGE				0x00300000	/*3.2-3.4V bits 20,21 set.todo:why*/
#define SD_VOLT_RANGE				0x80ff8000	/*2.7-3.6V bits 20,21 set . todo:why*/
#define MMC_33_34V					0x80ff8000	/*2.7-3.6V bits 20,21 set.*/
#define FIFO_DEPTH					8			/*todo: we can get it from IP*/

/*Note: Card no.starts from 0. So for 30 cards, card no.is from 0-29*/
#define MAX_CARDS					1			//SD_MMC mode
#define MAX_HSMMC_CARDS				2			//HSMMC specific constant
#define MAX_HSSD_CARDS				2			//HSSD sepcific constant

/*Divider value for MMC mode. Basic freq.is divided by
2*MMC_FREQDIVIDER. Currently for MMC type, freq.=20MHz and
for SD type, freq.=25 MHz is used.*/

/*CIU_CLK is in KHz EG 24000*/
/*mmc: 1 for 25, 2 for 50.
   SD: 0 for 25, 1 for 50.*/
#define CIU_CLK						48000
#define MMC_FREQDIVIDER	 (CIU_CLK/(SD_MAX_OPFREQUENCY))//MMC work at 12.5MHz
#define SD_FREQDIVIDER       (CIU_CLK/SD_MAX_OPFREQUENCY*2) //sd and sdio, work at 25MHz

#define MAX_THRESHOLD				0x0fff
#define MIN_THRESHOLD				100

//25000/(400*2)+1 = 32 25000/32*2=390
//22000/(400*2)+1 = 28 22000/28*2=392
//24000/(400*2)+1 = 31 24000/31*2=387
#define FOD_VALUE					400000 /*kHz*/
#define MMC_FOD_VALUE				125       /*kHz*/
#define SD_FOD_VALUE					300       /*kHz*/

#define FOD_DIVIDER_VALUE		((CIU_CLK/(FOD_VALUE*2))+1)

/*max clock divider*/
#define MAX_CLK_DIVIDER				0x0ff

/*ip register constants. This offset is actual register address, refer to ip manual*/
#if 0
#define SDIO_CTRL					0x00
#define SDIO_PWREN					0x04
#define SDIO_CLKDIV					0x08
#define SDIO_CLKSRC					0x0c
#define SDIO_CLKENA					0x10
#define SDIO_TMOUT					0x14
#define SDIO_CTYPE					0x18
#define SDIO_BLKSIZ					0x1c
#define SDIO_BYTCNT					0x20
#define SDIO_INTMSK					0x24
#define SDIO_CMDARG					0x28
#define SDIO_CMD						0x2c
#define SDIO_RESP0					0x30
#define SDIO_RESP1					0x34
#define SDIO_RESP2					0x38
#define SDIO_RESP3					0x3c
#define SDIO_MINTSTS					0x40
#define SDIO_RINTSTS					0x44
#define SDIO_STATUS					0x48
#define SDIO_FIFOTH					0x4c
#define SDIO_CDETECT					0x50
#define SDIO_WRTPRT					0x54
#define SDIO_GPIO					0x58
#define SDIO_TCBCNT					0x5c
#define SDIO_TBBCNT					0x60
#define SDIO_DEBNCE					0x64
#define SDIO_USRID					0x68
#define SDIO_VERID					0x6c
#define SDIO_HCON					0x70
#endif

#define SDIO_BMOD					0x80/*Bus Mode Register; controls the Host Interface Mode.*/
#define SDIO_PLDMND					0x84/*Poll Demand Register*/
#define SDIO_DBADDR					0x88/*Descriptor List Base Address Register*/
#define SDIO_IDSTS					0x8c/*Internal DMAC Status Register.*/
#define SDIO_IDINTEN					0x90/*Internal DMAC Interrrupt Enable Register.*/
#define SDIO_DSCADDR					0x94/*Current Host Descriptor Address Register*/
#define SDIO_BUFADDR					0x98/*Current Host Buffer Address Register.*/

/*Reserved = 0x070 to 0x0ff*/
#define SDIO_FIFO_START 0x100

/*Constants defined for bitwise access of registers.*/
#define START_CMD_BIT				0x80000000 //bit31
#define HLE_BIT						0x00001000 //bit12
#define DATATIMEOUT_BIT				0x00000200 //bit9
#define CARD_WIDTH_BITS				0x0000ffff //bits 0-15
#define ENABLE_OD_PULLUP_BIT			0xfeffffff //bit24
#define FIFO_SET_BIT					0x00000002 //bit1 for ORing
#define DMA_SET_BIT					0X00000004 //bit2 for ORing
#define CONTROLLER_SET_BIT			0x00000001 //bit0 for ORing
#define ALL_SET_BITS					0x00000006 //bit0 for ANDing
#define RESET_FIFO_DMA_CONTROLLER_DATA   0x07   //bits 0, 1, 2 all ones.

/*************************GLOBAL BEGIN*****************************/
/* 基本数据类型定义 */
#if 0
typedef signed long long BSP_S64;
typedef signed int       BSP_S32;
typedef signed short     BSP_S16;
typedef signed char      BSP_S8;
typedef char             BSP_CHAR;

typedef unsigned long long  BSP_U64;
typedef unsigned int        BSP_U32;
typedef unsigned short      BSP_U16;
typedef unsigned char       BSP_U8;

typedef int                 BSP_BOOL;
typedef void                BSP_VOID;
typedef int                 BSP_STATUS;

typedef signed long long*   BSP_PS64;
typedef signed int*         BSP_PS32;
typedef signed short*       BSP_PS16;
typedef signed char*        BSP_PS8;

typedef unsigned long long* BSP_PU64;
typedef unsigned int*       BSP_PU32;
typedef unsigned short*     BSP_PU16;
typedef unsigned char*      BSP_PU8;

typedef int*                BSP_PBOOL;
typedef void*               BSP_PVOID;
typedef int*                BSP_PSTATUS;
#endif
#ifndef BSP_CONST
#define BSP_CONST               const
#endif

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

/*
 * 寄存器操作宏定义
 */
#undef BSP_REG
#undef BSP_REG8
#undef BSP_REG16
#undef BSP_REG_CLRBITS
#undef BSP_REG_SETBITS
#undef BSP_REG_GETBITS
#undef BSP_REG_READ
#undef BSP_REG_WRITE
#undef BSP_REG_WRITEBITS
#undef BSP_REG8_READ
#undef BSP_REG16_READ
#undef BSP_REG_MODIFY
#define BSP_REG(base, reg) (*(volatile BSP_U32 *)((BSP_U32)base + (reg)))
#define BSP_REG8(base, reg) (*(volatile BSP_U8 *)((BSP_U32)base + (reg)))
#define BSP_REG16(base, reg) (*(volatile BSP_U16 *)((BSP_U32)base + (reg)))

/* 将var中[pos, pos + bits-1]比特清零,  pos从0开始编号
   e.g BDOM_CLR_BITS(var, 4, 2) 表示将Bit5~4清零 */
#define BSP_REG_CLRBITS(base, reg, pos, bits) (BSP_REG(base, reg) &= ~((((BSP_U32)1 << (bits)) - 1) << (pos)))
   
/* 将var中[pos, pos + bits-1]比特设置为val,  pos从0开始编号
   e.g BDOM_SET_BITS(var, 4, 2, 2) 表示将Bit5~4设置为b'10 */
#define BSP_REG_SETBITS(base, reg, pos, bits, val) (BSP_REG(base, reg) = (BSP_REG(base, reg) & (~((((BSP_U32)1 << (bits)) - 1) << (pos)))) \
                                                                         | ((BSP_U32)((val) & (((BSP_U32)1 << (bits)) - 1)) << (pos)))

/* 获取var中[pos, pos + bits-1]比特值,  pos从0开始编号 */
#define BSP_REG_GETBITS(base, reg, pos, bits) ((BSP_REG(base, reg) >> (pos)) & (((BSP_U32)1 << (bits)) - 1))

#define BSP_REG_READ(base, reg, result) \
    ((result) = BSP_REG(base, reg))

#define BSP_REG_WRITE(base, reg, data) \
    (BSP_REG(base, reg) = (data))

#define BSP_REG_WRITEBITS(base, reg, data) \
            (BSP_REG(base, reg) |= (data))
#define BSP_REG8_READ(base, reg, result) \
    ((result) = BSP_REG8(base, reg))

#define BSP_REG16_READ(base, reg, result) \
    ((result) = BSP_REG16(base, reg))

#define BSP_REG_MODIFY(base, reg, clrmsk, setmsk) \
    (BSP_REG(base, reg) = ((BSP_REG(base, reg) & ~clrmsk) | setmsk))
#if 0
typedef BSP_S32 (*FUNCPTR)(void);
typedef BSP_VOID (*VOIDFUNCPTR)(void);
typedef BSP_S32 STATUS;
typedef void VOID;
#endif
/*externel function definition*/
/*internel function definition*/
#if 0
typedef enum tagCLK_SOURCE_E
{
	CLKSRC_0,
	CLKSRC_1,
	CLKSRC_2,
	CLKSRC_3,
	CLKSRC_MAX //max clk source
}CLK_SOURCE_E;

/*IP register structure*/
typedef union tagINTR_STATUS_BITS_U
{
	BSP_U16 wReg;
	struct IntstsbitsTag
	{
		BSP_U32 Card_Detect		:1; //LSB
		BSP_U32 Rsp_Err			:1;//LSB
		BSP_U32 Cmd_Done		:1;//LSB
		BSP_U32 Data_trans_over	:1;//LSB
		BSP_U32 Tx_FIFO_datareq	:1;//LSB
		BSP_U32 Rx_FIFO_datareq	:1;//LSB
		BSP_U32 Rsp_crc_err		:1;//LSB
		BSP_U32 Data_crc_err		:1;//LSB
		BSP_U32 Rsp_tmout		:1;//LSB
		BSP_U32 Data_read_tmout	:1;//LSB
		BSP_U32 Data_starv_tmout	:1;//LSB
		BSP_U32 FIFO_run_err		:1;//LSB
		BSP_U32 HLE				:1;//LSB
		BSP_U32 SBE				:1;//LSB
		BSP_U32 ACD				:1;//LSB
		BSP_U32 EBE				:1;//LSB
	}Bits; /*struct*/	
}INTR_STATUS_BITS_U;
#endif
/*0x000-
*
*-0x58  FIXME?
*/


#endif /*_DWC_MOBILE_STORAGE_H_*/
