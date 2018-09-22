#ifndef _SDIO_IDMAC_H_
#define _SDIO_IDMAC_H_
#include "dwc_mobile_storage.h"
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>

//#define  dmac_writew(addr,value)		((*(volatile unsigned int *)(addr)) = (value))
//#define  dmac_readw(addr,value)		(value =(*(volatile unsigned int *)(addr)))

/*Descriptor count*/
#define IDMAC_DESC_SIZE				(1*1024)

/*描述符个数是否为2的N次幂*/
#define IDMAC_DESC_2_POWER

/*总线宽度*/
#define SDIO_BUS_WITDTH				4

/*每个描述符对应的最大buf大小*/
#define IDMAC_BUF_MAXSIZE			(4*1024)

/*描述符存储类型*/
//#define IDMAC_DESC_MEM_TYPE_DMA
#define IDMAC_DESC_MEM_TYPE_CACHE
//#define IDMAC_DESC_MEM_TYPE_AXIMEM

#ifdef IDMAC_DESC_MEM_TYPE_CACHE
/*linux diable cache when using dma buf, need to check*/
/*描述符是否flush cache*/
#define IDMAC_DESC_FLUSH_CACHE_ENABLE

/*描述符是否valid cache*/
#define IDMAC_DESC_VALID_CACHE_ENABLE
#endif

/*发送buf是否需要刷cache*/
#define IDMAC_TX_BUF_FLUSH_CACHE_ENABLE

#define BSP_CACHE_LINE_SIZE			32
#define BSP_CACHE_ALIGN_SIZE			4


extern void __iomem *balong_internal_base;

enum DmaBusModeReg
{
	/*Bit description												Bits 	R/W*/
	DmaBurstLength256		= 0x00000700,	/*Dma burst length = 256	10:8	R*/
	DmaBurstLength128		= 0x00000600,	/*Dma burst length = 128	10:8	R*/
	DmaBurstLength64			= 0x00000500,	/*Dma burst length = 64		10:8	R*/
	DmaBurstLength32			= 0x00000400,	/*Dma burst length = 32		10:8	R*/
	DmaBurstLength16			= 0x00000300,	/*Dma burst length = 16		10:8	R*/
	DmaBurstLength8			= 0x00000200,	/*Dma burst length = 8		10:8	R*/
	DmaBurstLength4			= 0x00000100,	/*Dma burst length = 4		10:8	R*/
	DmaBurstLength1			= 0x00000000,	/*Dma burst length = 1		10:8	R*/

	DmaEnable				= 0x00000080,	/*IDMAC Enable			7	RW*/
	DmaDisable				= 0x00000000,	/*IDMAC Disable			7	RW*/

	DmaDescriptorSkip16		= 0x00000040,  /*(DSL)Descriptor skip length(no.of dwords)  6:2	  RW*/
	DmaDescriptorSkip8		= 0x00000020,  /*between two unchained descriptors	*/
	DmaDescriptorSkip4		= 0x00000010,  /*					*/
	DmaDescriptorSkip2		= 0x00000008,  /*					*/
	DmaDescriptorSkip1		= 0x00000004,  /*					*/
	DmaDescriptorSkip0		= 0x00000000,  /*					*/

	DmaFixedBurstEnable		= 0x00000002,  /*(FB)Fixed Burst SINGLE, INCR4, INCR8 or INCR16	1 RW*/
	DmaFixedBurstDisable		= 0x00000000,  /*SINGLE,INCR				1		RW*/

	DmaResetOn				= 0x00000001,  /*(SWR)Software Reset DMA engine	0	RW*/
	DmaResetOff				= 0x00000000   /*								0	RW*/
};

enum DmaIntStatusReg
{
	DmaStatusAbnormal		= 0x00000200,/*(AIS)Abnormal interrupt summary	9	RW*/
	DmaStatusNormal			= 0x00000100,/*(NIS)Normal interrupt summary		8	RW*/

	DmaStatusCardErrSum		= 0x00000020,/*Card Error Summary		5	RW*/
	DmaStatusNoDesc			= 0x00000010,/*Descriptor Unavailable		4	RW*/
	DmaStatusBusError			= 0x00000004,/*Fatal bus error(Abnormal)   2	RW*/
	DmaStatusRxCompleted		= 0x00000002,/*Receive completed (Abnormal)	1	RW*/
	DmaStatusTxCompleted		= 0x00000001/*Transmit completed(Normal)		0	RW*/	
};

enum DmaIntEnableReg
{
	DmaIntAbnormal		= 0x00000200, /*(AIS)Abnormal interrupt summary	9	RW*/
	DmaIntNormal			= 0x00000100, /*(NIS)Normal interrupt summary		8	RW*/

	DmaIntCardErrSum		= 0x00000020, /*Card Error Summary				5	RW*/
	DmaIntNoDesc		= 0x00000010, /*Descriptor Unavailable				4	RW*/
	DmaIntBusError		= 0x00000004, /*Fatal bus error (Abnormal)			2	RW*/
	DmaIntRxCompleted	= 0x00000002, /*Receive completed (Abnormal)		1	RW*/
	DmaIntTxCompleted	= 0x00000001	/*Transmit completed(Normal)		0	RW*/
};

/*****************************************
*DMA Engine descriptors
*****************************************/
enum DmaDescriptorStatus
{
	DescOwnByDma		= 0x80000000,   /*(OWN)Descriptor is owned by DMA engine	31	RW*/
	DescCardErrorSum		= 0x40000000,   /*(CES)status of the transaction to or form the card  30 RW*/

	DescEndOfRing		= 0x00000020,   /*(ER) the first buffer of the data 	5	RW*/
	DescChain			= 0x00000010,   /*(CH) the second address in the descriptor is the 
	Next Descriptor address rather than the second buffer address. 		4		RW*/

	DescFirst				= 0x00000008, /*(FS)indicates that this descriptor contains the first 
	buffer of the data			3		RW*/
	DescLast				= 0x00000004,  /*(LD)indicates that the buffers pointed to by this descriptor
	are the last buffers of the data		2	RW*/
	DisIntComplete		= 0x00000002,  /*(DIC)prevent the setting of the TI/RI bit of the IDMAC
	Status Registers (IDSTS) for the data that ends in the buffer pointed to bye this descriptor 	1	RW*/

	DescSize2Mask		= 0x3FFE000,   /*(TBS2) Buffer 2 size			25:13*/
	DescSize2Shift			= 13,
	DescSize1Mask		= 0x00001FFF, /*TBS1  Buffer 1 size 			12:0*/
	DescSize1Shift			= 0
};

/*内置DMA描述符结构体*/
typedef struct tagDmaDesc
{
	BSP_U32 status;		/*Status*/
	BSP_U32 length;		/*Buffer1 and Buffer2 length*/
	BSP_U32 buffer1;		/*Network Buffer 1 pointer (Dma-able)*/
	BSP_U32 buffer2;		/*Network Buffer 2 pointer or next descriptor pointer (Dma-able)in chain structure*/
}DmaDesc;

/*synopGMAC device data*/
typedef struct tagIDMAC_HANDLE_S
{
	BSP_VOID*  RawAddr;/*the virtual address before align*/ 
	BSP_U32	DescDma; /*Dma-able address of descriptor either in ring or chain mode, this is used by the IDMAC*/
	DmaDesc *Desc;  /*start address of descriptors ring of chain, this is used by the driver*/

	BSP_U32 DescCount; /*number of descriptors in the descriptor queue/pool*/

	BSP_U32 Next;    /*index of the descriptor next available with driver, given to DMA*/

	DmaDesc *NextDesc;  /*Descriptor address corresponding to the index Next*/

	BSP_U8	*assisit_buf; /*辅助buf,用于字节不对齐时的处理*/
	BSP_BOOL bDescSetupFlag;  /*描述符是否建立完成标识*/
	BSP_BOOL bTmpDescFlag;   /*临时描述符标识*/
	BSP_BOOL bLackDescFlag;  /*描述符不足标识*/

	/*cache line*/
	BSP_U32 DescDma_Base;
}IDMAC_HANDLE_S;

/*IDMAC包分段枚举*/
typedef enum tagIDMAC_PARTITION_TYPE_E
{
	IDMAC_PARTITION_TYPE_HEAD	= 0,	/*首段包*/
	IDMAC_PARTITION_TYPE_MID,		/*中段包*/
	IDMAC_PARTITION_TYPE_TAIL,		/*尾段包*/
	IDMAC_PARTITION_TYPE_TOTAL,	/*整段包*/
	IDMAC_PARTITION_TYPE_NORMAL, /*单个节点不超过4K*/
	IDMAC_PARTITION_TYPE_MAX		/*边界值*/
}IDMAC_PARTITION_TYPE_E;

/*检查buf地址是否对齐*/
//FIXME? check this in linux side
#define IDMAC_check_addr_align(buf) \
		( 0==(buf)%BSP_CACHE_ALIGN_SIZE)

/*获取对齐的buf地址*/
#define IDMAC_get_addr_align(buf) \
		((buf)-(buf)%BSP_CACHE_ALIGN_SIZE)

/*跳过不对齐部分后的buf地址*/
#define IDMAC_skip_addr_noalign(buf)\
		(IDMAC_check_addr_align(buf)?(buf):(IDMAC_get_addr_align(buf)+BSP_CACHE_ALIGN_SIZE))

/*获取不对齐字节个数*/
#define IDMAC_get_addr_noalign_bytes(buf)\
		(IDMAC_skip_addr_noalign(buf)-(buf))

/*获取对齐字节个数*/
#define IDMAC_get_addr_align_bytes(buf,len)\
		(len-IDMAC_get_addr_noalign_bytes(buf))

/*获取节点个数*/
//FIXME? what did this work?
#define IDMAC_get_node_count(buf,len) \
		(((buf)+(len)-1)/IDMAC_BUF_MAXSIZE-(buf)/IDMAC_BUF_MAXSIZE+1)

/*注意:buff和len必须为BSP_U32类型，如果不是，调用时必须进行强制类型转换*/
#define IDMAC_get_desc_count(buf,len)\
		(IDMAC_check_addr_align((buf))) \
			?IDMAC_get_node_count((buf),(len))\
			:(IDMAC_get_node_count(IDMAC_skip_addr_noalign((buf)), \
			IDMAC_get_addr_align_bytes((buf), (len)))+1))

#define IDMAC_set_DmaOwn(Desc)\
		((Desc)->status |= (BSP_U32)DescOwnByDma)

//#define GPIO_REGBASE_ADR		GPIO0_REGBASE_ADR

/*寄存器基址配置*/
#define SD_REGBASE_ADDR		balong_internal_base//0x900AD000~0x900ADFFF
#define SDIO_REGBASE_ADR		(SD_REGBASE_ADDR)//refer to mmc control base address
//#define SDIO_GPIO_REGBASE_ADR	GPIO_REGBASE_ADR

#define IDMAC_resume_dma() \
		BSP_REG_WRITE(SDIO_REGBASE_ADR, SDIO_PLDMND, 1)

#define sdio_dma_bus_mode_init(init_value) \
		BSP_REG_WRITE(SDIO_REGBASE_ADR, SDIO_BMOD, (init_value))

#define sdio_dma_interrupt_mask(init_value) \
		BSP_REG_WRITE(SDIO_REGBASE_ADR, SDIO_IDINTEN, (init_value))

#define sdio_dma_interrupt_clear(init_value) \
		BSP_REG_WRITE(SDIO_REGBASE_ADR, SDIO_IDSTS, (init_value))

#define IDMAC_desc_init_ring(desc, last_ring_desc)\
		do{ \
			(desc)->status = (last_ring_desc)?DescEndOfRing:0;\
			(desc)->length = 0; \
		}while(0)

#define IDMAC_init_desc_base(idmachandle) \
		BSP_REG_WRITE(SDIO_REGBASE_ADR, SDIO_DBADDR, (BSP_U32)idmachandle->DescDma)

#define IDMAC_get_assist_data(pstIdmachHandle, u32buf, u32len) \
		memcpy((BSP_VOID*)u32buf, (BSP_VOID*)pstIdmachHandle->assist_buf, (int)u32len)

#define IDMAC_flush_desc(idmachandle, StartIdxTag, StartDescTag, num)//cacheFlush it's unnecessary when using dma buf in linux
#define IDMAC_invalidate_desc(idmachandle, StartIdxTag, StartDescTag, num)//up to

//extern BSP_U8 assist_buf[SDIO_BUS_WITDTH];
//extern BSP_U32 idmac_tx_qptr_flag[IDMAC_PARTITION_TYPE_MAX];
//extern BSP_BOOL idmac_tx_buffer_flush_cache_flag;

#if 0
BSP_VOID *IDMAC_alloc_memory(BSP_U32 align_size, BSP_U32 bytes);
BSP_VOID *IDMAC_free_memory(BSP_VOID *buffer);
BSP_VOID *IDMAC_alloc_consistent_dmaable_memory(BSP_U32 align_size, BSP_U32 size, BSP_U32*addr);
BSP_VOID IDMAC_free_consistent_dmaable_memory(BSP_VOID *addr);

BSP_S32 IDMAC_alloc_desc_queue(IDMAC_HANDLE_S* idmachandle, BSP_U32 no_of_desc);
BSP_VOID IDMAC_free_desc_queue(IDMAC_HANDLE_S *idmachandle);
#endif

BSP_S32	 IDMAC_Init(void __iomem * base);
BSP_VOID IDMAC_Exit(void __iomem * base);
BSP_VOID  IDMAC_config_init(void);
BSP_VOID IDMAC_NoCopySend(BSP_U32 u32buf, BSP_U32 u32len, BSP_U32 s32NodeCnt);
BSP_VOID IDMAC_NoCopyReceive(BSP_U32 u32buf, BSP_U32 u32len, BSP_U32 s32NodeCnt);
BSP_S32	IDMAC_SingleChannelSendLLI(struct scatterlist *sgHeader, unsigned int dma_len);
BSP_S32	IDMAC_SingleChannelReceiveLLI(struct scatterlist *sgHeader, unsigned int dma_len);
BSP_S32	IDMAC_SingleChannelStart(struct scatterlist *sgHeader, unsigned int dma_len,  enum dma_data_direction direction);
#endif/*_SDIO_IDMAC_H_*/
