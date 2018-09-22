/*
 *  linux/drivers/mmc/sdio_idmac.c - Balong internal dmac driver
 *
 *  Copyright (C) 
 * filename : sdio_idmac.c
 * author: 
 */
 //#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
//#include <linux/config.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <mach/platform.h>
#include <linux/scatterlist.h>
 #include "sdio_idmac.h"

#undef HISDIO_TRACE_LEVEL
#define HISDIO_TRACE_LEVEL  (5)

#define HIIDMAC_SG_OVER_LEN_NDEBUG	(0)

/*
   0 - all message
   1 - dump all register read/write
   2 - flow trace
   3 - timeouut err and protocol err
   */
#undef hisdio_trace
#undef hisdio_error
#define BALONG_IDMAC_DEBUG_TAG	"hisdio_idmac"
#define hisdio_trace(level, format, args...) do { \
	if((level) >= HISDIO_TRACE_LEVEL) { \
		printk(format":"BALONG_IDMAC_DEBUG_TAG":%s:%d: \n", ## args, __FUNCTION__,  __LINE__); \
	} \
}while(0)


#define hisdio_error(format, args...) do{ \
	printk(KERN_ERR""format":"BALONG_IDMAC_DEBUG_TAG" error:%s:%d: \n", ##args, __FUNCTION__, __LINE__); \
}while(0)

#undef _ALIGN_UP
#undef _ALIGN_DOWN
#undef _ALIGN
#undef PAGE_ALIGN
/* align addr on a size boundary - adjust address up/down if needed */
#define _ALIGN_UP(addr,size)	(((addr)+((size)-1))&(~((size)-1)))
#define _ALIGN_DOWN(addr,size)	((addr)&(~((size)-1)))

/* align addr on a size boundary - adjust address up if needed */
#define _ALIGN(addr,size)     _ALIGN_UP(addr,size)

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr,size)	_ALIGN(addr, size)

/*register base address*/
void __iomem * balong_internal_base = NULL;

/*global variables*/
static IDMAC_HANDLE_S *pstIdmacHandle = BSP_NULL;

 /*辅助buf,用于处理字节不对齐的情况*/
//BSP_U8 assist_buf[SDIO_BUS_WITDTH] = {0};
#define    PARTITION_TYPE_HEAD_SP   (DisIntComplete | DescFirst)
#define    PARTITION_TYPE_MID_SP    (DisIntComplete | DescOwnByDma)
#define    PARTITION_TYPE_TAIL_SP   (DisIntComplete | DescLast | DescOwnByDma)
#define    PARTITION_TYPE_TOTAL_SP     (DisIntComplete | DescFirst | DescLast | DescOwnByDma)
static BSP_U32 idmac_tx_qptr_flag[IDMAC_PARTITION_TYPE_MAX][IDMAC_PARTITION_TYPE_MAX]=
 {
    {
 	    PARTITION_TYPE_HEAD_SP, //0,0		
 	    PARTITION_TYPE_MID_SP, 	//0,1
 	    PARTITION_TYPE_MID_SP,	//0,2
 	    PARTITION_TYPE_MID_SP, //0,3
 	    PARTITION_TYPE_HEAD_SP //0,4 used when sg node not over 4k
 	},
    {
 	    PARTITION_TYPE_MID_SP, 	//1,0
 	    PARTITION_TYPE_MID_SP, 	//1,1
 	    PARTITION_TYPE_MID_SP,	//1,2
 	    PARTITION_TYPE_MID_SP, //1,3
 	    PARTITION_TYPE_MID_SP //1,4
 	},
 	{
 	    PARTITION_TYPE_MID_SP, //2,0
 	    PARTITION_TYPE_MID_SP, //2,1
 	    PARTITION_TYPE_TAIL_SP,	//2,2
 	    PARTITION_TYPE_TAIL_SP, //2,3
 	    PARTITION_TYPE_TAIL_SP//2,4
 	},
 	{
 	    PARTITION_TYPE_HEAD_SP, //3,0
 	    PARTITION_TYPE_MID_SP, //3,1
 	    PARTITION_TYPE_TAIL_SP,	//3,2
 	    PARTITION_TYPE_TOTAL_SP, //3,3
 	    PARTITION_TYPE_TOTAL_SP//3,4
 	},
 	{
 	    0, //4,0
 	    0, //4,1
 	    0,	//4,2
 	    0, //4,3
 	    0//4,4
 	}	
 };


 BSP_BOOL idmac_tx_buffer_flush_cache_flag=
 {
 #ifdef IDMAC_TX_BUF_FLUSH_CACHE_ENABLE
 	BSP_TRUE
 #else
 	BSP_FALSE
 #endif
 };

/*获取数据分段类型*/
__inline__ IDMAC_PARTITION_TYPE_E IDMAC_get_node_partition(BSP_U32 i, BSP_U32 cnt)
{
	if(1 == cnt) return IDMAC_PARTITION_TYPE_TOTAL;
	else if(0 == i) return IDMAC_PARTITION_TYPE_HEAD;
	else if(i == cnt -1) return IDMAC_PARTITION_TYPE_TAIL;
	else return IDMAC_PARTITION_TYPE_MID;
}

__inline__ BSP_U32 IDMAC_set_qptr(IDMAC_HANDLE_S *imachandle, IDMAC_PARTITION_TYPE_E ePartType,
                IDMAC_PARTITION_TYPE_E ePartType_sp,BSP_U32 buf1, BSP_U32 len1, DmaDesc **desc_over)
{
	BSP_U32 next = imachandle->Next;
	DmaDesc *desc = imachandle->Desc + next;
    hisdio_trace(1, "dmadesc addr: 0x%p ", desc);
	desc->length = (len1<<DescSize1Shift) & DescSize1Mask;
	desc->status = (next + 1 != imachandle->DescCount)?idmac_tx_qptr_flag[ePartType][ePartType_sp]:(idmac_tx_qptr_flag[ePartType][ePartType_sp]|
		DescEndOfRing);
	desc->buffer1 = buf1;

#ifdef IDMAC_DESC_2_POWER
	imachandle->Next = (next + 1) & (imachandle->DescCount - 1);
#else
	imachandle->Next = (next + 1 != imachandle->DescCount)?next + 1:0;
#endif
	*desc_over = desc;
	return next;
}


static BSP_S32 IDMAC_alloc_desc_queue(IDMAC_HANDLE_S *idmachandle, BSP_U32 no_of_desc)
{
	BSP_U32 cnt = 0;
	BSP_U32 first_desc = 0;
	BSP_U32 dma_addr = 0;
	idmachandle->DescCount = 0;
	hisdio_trace(2, "Total size of memory required for Tx Descriptors in Ring Mode = 0x%08x\n", (sizeof(DmaDesc)*no_of_desc));

	/*分配描述符空间*/
	//first_desc = (DmaDesc*)dma_alloc_coherent(NULL, sizeof(DmaDesc)*no_of_desc, &dma_addr, GFP_DMA|__GFP_WAIT);
	idmachandle->RawAddr= dma_alloc_coherent(NULL, sizeof(DmaDesc)*no_of_desc + BSP_CACHE_LINE_SIZE,
							&dma_addr, GFP_DMA|__GFP_WAIT);
	if(!idmachandle->RawAddr)
	{
		hisdio_error("dma alloc coherent error for descriptors memory allocation!");
		return BSP_ERROR;
	}
	first_desc = PAGE_ALIGN((BSP_U32)idmachandle->RawAddr, BSP_CACHE_LINE_SIZE);//align to 32 bits
	idmachandle->DescDma_Base = dma_addr;
	hisdio_trace(1, "frist desc address: 0x%x, raw addr: 0x%p", first_desc, idmachandle->RawAddr);
    /*modified for lint e665 */
	memset((BSP_VOID*)idmachandle->RawAddr, 0x0, (sizeof(DmaDesc)*no_of_desc));

	idmachandle->DescCount	= no_of_desc - 1;	//alloc one more descrpitor space
	idmachandle->Desc		= (DmaDesc*)first_desc;		//virtual address
	idmachandle->DescDma		= dma_addr + (first_desc -  (BSP_U32)idmachandle->RawAddr);		//actual phy address

	/*debug info*/
	hisdio_trace(1, "idmachandle==>[DescCount : %d], [Desc : 0x%p], [DescDma : 0x%8x]", idmachandle->DescCount,
				idmachandle->Desc, idmachandle->DescDma);
	
	for(cnt = 0; cnt < idmachandle ->DescCount; cnt++)
	{
		IDMAC_desc_init_ring(idmachandle->Desc + cnt, cnt == (idmachandle->DescCount - 1));
		hisdio_trace(0, "descrpitor [%02d : %08x]", cnt, (BSP_U32)(idmachandle->Desc + cnt));
	}

	idmachandle->Next = 0;
	idmachandle->NextDesc = idmachandle->Desc;
	
	return BSP_OK;
}

static BSP_VOID IDMAC_free_desc_queue(IDMAC_HANDLE_S * idmachandle)
{
	hisdio_trace(1, "enter & leave");
	/*debug info*/
	hisdio_trace(1, "idmachandle==>[DescCount : %d], [Desc : 0x%p], [DescDma : 0x%8x]", idmachandle->DescCount,
				idmachandle->Desc, idmachandle->DescDma);
	dma_free_coherent(NULL,sizeof(DmaDesc)*(idmachandle->DescCount + 1),idmachandle->RawAddr,idmachandle->DescDma_Base);
}

static BSP_S32 IDMAC_set_node_ptr(
	BSP_U32 u32buf,
	BSP_U32 u32len,
	BSP_U32 u32NodeCnt,
	IDMAC_PARTITION_TYPE_E sg_list_ePartitionType,
	DmaDesc **descTag
)
{
	BSP_U32 cnt = 0;
	DmaDesc *desc = BSP_NULL;
	//DmaDesc *descTag = BSP_NULL;
	BSP_U32 buf = u32buf;
	BSP_U32 len = 0;
	BSP_U32 len_remain = u32len;
	BSP_U32 buf_skip_noalign_addr = IDMAC_skip_addr_noalign(u32buf);
	BSP_BOOL addr_align_flag = IDMAC_check_addr_align(u32buf);
	IDMAC_PARTITION_TYPE_E	ePartitionType = IDMAC_PARTITION_TYPE_TOTAL;

	if(!pstIdmacHandle)
	{
		hisdio_error("please initialize the dmac first, perform IDMAC_Init");
		return BSP_ERROR;
	}

	for(cnt=0; cnt < u32NodeCnt; cnt++)
	{
		/*如果地址不对齐，第一描述符使用辅助buf*/
		if((0 == cnt) && !addr_align_flag)
		{
			hisdio_trace(3, "check adress alignment please.");
			//memcpy((BSP_VOID *)pstIdmacHandle->assisit_buf, (BSP_VOID*)u32buf, IDMAC_get_addr_noalign_bytes(u32buf));
			return BSP_ERROR;
		}

		/*检查数据分段类型*/
		ePartitionType = IDMAC_get_node_partition(cnt, u32NodeCnt);
		hisdio_trace(1, "separate:ePartitionType :%d,cnt:%d",ePartitionType,cnt);
		switch(ePartitionType)
		{
			case IDMAC_PARTITION_TYPE_TOTAL:
				hisdio_error("error type,impossible!");
				return BSP_ERROR;
			/*BEGIN:y00206456 2012-04-25 Modified for pclint e527*/
            /*END:y00206456 2012-04-25 Modified for pclint e527*/

			case IDMAC_PARTITION_TYPE_HEAD:
				//buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				//len = addr_align_flag ? IDMAC_BUF_MAXSIZE : IDMAC_get_addr_noalign_bytes(u32buf);
				switch(sg_list_ePartitionType)
				{
					case IDMAC_PARTITION_TYPE_TOTAL:
						len = IDMAC_BUF_MAXSIZE;
						pstIdmacHandle->Next = 0;
						buf = u32buf;
						IDMAC_init_desc_base(pstIdmacHandle);
						break;
					case IDMAC_PARTITION_TYPE_HEAD:
						len = IDMAC_BUF_MAXSIZE;
						pstIdmacHandle->Next = 0;
						buf = u32buf;
						IDMAC_init_desc_base(pstIdmacHandle);
						break;
					case IDMAC_PARTITION_TYPE_MID:
						buf = u32buf;
						len = IDMAC_BUF_MAXSIZE;
						break;
					case IDMAC_PARTITION_TYPE_TAIL:
						buf = u32buf;
						len = IDMAC_BUF_MAXSIZE;
						break;
					default:
						break;
				}
				break;

			case IDMAC_PARTITION_TYPE_MID:
				buf = addr_align_flag ? (buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = IDMAC_BUF_MAXSIZE;
				break;

			case IDMAC_PARTITION_TYPE_TAIL:
				buf = addr_align_flag?(buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = len_remain;
				break;
			default:
				break;	
		}
		/*填充描述符*/
		(BSP_VOID)IDMAC_set_qptr(pstIdmacHandle, sg_list_ePartitionType,ePartitionType, buf, len ,&desc);

		/*if(((IDMAC_PARTITION_TYPE_HEAD == ePartitionType)&& (IDMAC_PARTITION_TYPE_HEAD == sg_list_ePartitionType))
			|| (IDMAC_PARTITION_TYPE_TOTAL == ePartitionType))*/
		if(((IDMAC_PARTITION_TYPE_HEAD == sg_list_ePartitionType)|| (IDMAC_PARTITION_TYPE_TOTAL == sg_list_ePartitionType))
		    && ((IDMAC_PARTITION_TYPE_HEAD == ePartitionType) || (IDMAC_PARTITION_TYPE_TOTAL== ePartitionType)))	
		{
			*descTag = desc;
		}

		/*if(((IDMAC_PARTITION_TYPE_TAIL == ePartitionType) && (IDMAC_PARTITION_TYPE_TAIL == sg_list_ePartitionType))
		    || ((IDMAC_PARTITION_TYPE_TAIL == ePartitionType) && (IDMAC_PARTITION_TYPE_TOTAL== sg_list_ePartitionType)))*/
		if(((IDMAC_PARTITION_TYPE_TAIL == sg_list_ePartitionType)||(IDMAC_PARTITION_TYPE_TOTAL== sg_list_ePartitionType)) 
		    && (IDMAC_PARTITION_TYPE_TAIL == ePartitionType))    
		{
		    hisdio_trace(1, "separate set own bit to 1. descTag:%p",*descTag);
		    if(*descTag)
			    IDMAC_set_DmaOwn(*descTag);
		}

		len_remain -= len;
	}
	return BSP_OK;
}

BSP_VOID IDMAC_NoCopySend(
	BSP_U32 u32buf,
	BSP_U32 u32len,
	BSP_U32 u32NodeCnt
)
{
	BSP_U32 cnt = 0;
	DmaDesc *desc = BSP_NULL;
	DmaDesc *descTag = BSP_NULL;
	BSP_U32 buf = u32buf;
	BSP_U32 len = 0;
	BSP_U32 len_remain = u32len;
	BSP_U32 buf_skip_noalign_addr = IDMAC_skip_addr_noalign(u32buf);
	BSP_BOOL addr_align_flag = IDMAC_check_addr_align(u32buf);
	IDMAC_PARTITION_TYPE_E	ePartitionType = IDMAC_PARTITION_TYPE_TOTAL;

	if(!pstIdmacHandle)
	{
		hisdio_error("please initialize the dmac first, perform IDMAC_Init");
		return;
	}

	for(cnt=0; cnt < u32NodeCnt; cnt++)
	{
		/*如果地址不对齐，第一描述符使用辅助buf*/
		if((0 == cnt) && !addr_align_flag)
		{
			hisdio_trace(3, "check adress alignment please.");
			memcpy((BSP_VOID *)pstIdmacHandle->assisit_buf, (BSP_VOID*)u32buf, IDMAC_get_addr_noalign_bytes(u32buf));
		}

		/*检查数据分段类型*/
		ePartitionType = IDMAC_get_node_partition(cnt, u32NodeCnt);
		switch(ePartitionType)
		{
			case IDMAC_PARTITION_TYPE_TOTAL:
				buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				len = u32len;
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);
				break;

			case IDMAC_PARTITION_TYPE_HEAD:
				buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				len = addr_align_flag ? IDMAC_BUF_MAXSIZE : IDMAC_get_addr_noalign_bytes(u32buf);
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);
				break;

			case IDMAC_PARTITION_TYPE_MID:
				buf = addr_align_flag ? (buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = IDMAC_BUF_MAXSIZE;
				break;

			case IDMAC_PARTITION_TYPE_TAIL:
				buf = addr_align_flag?(buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = len_remain;
				break;
			default:
				break;	
		}
		/*填充描述符*/
		(BSP_VOID)IDMAC_set_qptr(pstIdmacHandle, ePartitionType,IDMAC_PARTITION_TYPE_NORMAL,buf, len ,&desc);

		if((IDMAC_PARTITION_TYPE_HEAD == ePartitionType)
			|| (IDMAC_PARTITION_TYPE_TOTAL == ePartitionType))
		{
		    descTag = desc;
		}

		if(IDMAC_PARTITION_TYPE_TAIL == ePartitionType)
		{
		    if(descTag)
			    IDMAC_set_DmaOwn(descTag);
		}

		len_remain -= len;
	}

	IDMAC_resume_dma();
}

BSP_VOID IDMAC_NoCopyReceive(	BSP_U32 u32buf, 
									BSP_U32 u32len, 
									BSP_U32 u32NodeCnt)
{
	BSP_U32 cnt = 0;
	DmaDesc *desc = BSP_NULL;
	DmaDesc *descTag = BSP_NULL;
	BSP_U32 buf = u32buf;
	BSP_U32 len = 0;
	BSP_U32 len_remain = u32len;
	BSP_U32 buf_skip_noalign_addr = IDMAC_skip_addr_noalign(u32buf);
	BSP_BOOL addr_align_flag = IDMAC_check_addr_align(u32buf);
	IDMAC_PARTITION_TYPE_E ePartitionType = IDMAC_PARTITION_TYPE_TOTAL;

	if(!pstIdmacHandle)
	{
		hisdio_error("please initialize the dmac first, perform IDMAC_Init");
		return;
	}

	for(cnt=0; cnt < u32NodeCnt; cnt++)
	{
		/*如果地址不对齐，第一描述符使用辅助buf*/
		if((0 == cnt) && !addr_align_flag)
		{
			hisdio_trace(3, "check adress alignment please.");
			memcpy((BSP_VOID *)pstIdmacHandle->assisit_buf, (BSP_VOID*)u32buf, IDMAC_get_addr_noalign_bytes(u32buf));
		}

		/*检查数据分段类型*/
		ePartitionType = IDMAC_get_node_partition(cnt, u32NodeCnt);
		switch(ePartitionType)
		{
			case IDMAC_PARTITION_TYPE_TOTAL:
				buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				len = u32len;
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);
				break;

			case IDMAC_PARTITION_TYPE_HEAD:
				buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				len = addr_align_flag ? IDMAC_BUF_MAXSIZE : IDMAC_get_addr_noalign_bytes(u32buf);
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);
				break;

			case IDMAC_PARTITION_TYPE_MID:
				buf = addr_align_flag ? (buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = IDMAC_BUF_MAXSIZE;
				break;

			case IDMAC_PARTITION_TYPE_TAIL:
				buf = addr_align_flag?(buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = len_remain;
				break;
			default:
				break;	
		}
		/*填充描述符*/
		(BSP_VOID)IDMAC_set_qptr(pstIdmacHandle, ePartitionType,IDMAC_PARTITION_TYPE_NORMAL, buf, len ,&desc);

		if((IDMAC_PARTITION_TYPE_HEAD == ePartitionType)
			|| (IDMAC_PARTITION_TYPE_TOTAL == ePartitionType))
		{
			descTag = desc;
		}

		if(IDMAC_PARTITION_TYPE_TAIL == ePartitionType)
		{
		    hisdio_trace(2, "set own bit to 1.");
		    if(descTag)
			    IDMAC_set_DmaOwn(descTag);
		}

		len_remain -= len;
	}

	IDMAC_resume_dma();
}

BSP_S32	IDMAC_SingleChannelSendLLI(struct scatterlist *sgHeader, unsigned int dma_len)
{
	unsigned int cnt=0;
#if !(HIIDMAC_SG_OVER_LEN_NDEBUG)
	BSP_U32 u32NodeCnt  = 0;
#endif
	BSP_U32 u32NodeCnt_new = 0;
	DmaDesc *desc = BSP_NULL;
	DmaDesc *descTag = BSP_NULL;
	struct scatterlist  *sgNode = sgHeader;
	IDMAC_PARTITION_TYPE_E ePartitionType = IDMAC_PARTITION_TYPE_TOTAL;
	dma_addr_t buf = 0;
	BSP_U32 buf_len= 0;
	BSP_U32 len = 0;
	BSP_BOOL addr_align_flag = BSP_TRUE;
	if(!pstIdmacHandle)
	{
		hisdio_error("please initialize the dmac first, perform IDMAC_Init");
		return BSP_ERROR;
	}	
	for_each_sg(sgHeader, sgNode, dma_len, cnt) 
	{
		ePartitionType = IDMAC_get_node_partition(cnt, dma_len);
		hisdio_trace(1, "ePartitionType :%d,cnt:%d",ePartitionType,cnt);
		buf = sg_dma_address(sgNode);//phy address
		//buf_len = sg_dma_len(sgNode);
		buf_len = sgNode->length;
		
 		addr_align_flag = IDMAC_check_addr_align(buf);
		if(!addr_align_flag)
		{
			hisdio_error("buf node : %d not alignment,buf address : 0x%8x", cnt, buf);
			return BSP_ERROR;
		}
		else if(buf_len > (IDMAC_BUF_MAXSIZE))
		{
#if HIIDMAC_SG_OVER_LEN_NDEBUG		
			hisdio_error("buf node : buf len : %d", buf_len);
			return BSP_ERROR;
#else
            hisdio_trace(2, "buf_len over 4k: %d", buf_len);
			/*deal with in case of  sg->length > descriptor_max_length*/
			u32NodeCnt = IDMAC_get_node_count(buf, buf_len);
			if(u32NodeCnt < 1)
			{
				hisdio_error("node count calculate error : [count : %d]", u32NodeCnt);
				return BSP_ERROR;
			}
			if(((cnt -1) + u32NodeCnt) > IDMAC_DESC_SIZE)
			{
				hisdio_error("too many LLI descriptor!");
				return BSP_ERROR;
			}
			if(BSP_OK != IDMAC_set_node_ptr(buf, buf_len, u32NodeCnt, ePartitionType,&descTag))
			{
				hisdio_error("lli separate node failed!");
				return BSP_ERROR;
			}
			u32NodeCnt_new = u32NodeCnt_new + u32NodeCnt;
			hisdio_trace(1, "cnt %d: [u32NodeCnt_new : %d]", cnt ,u32NodeCnt_new);
			continue;//loop next cnt
#endif
		}
		switch(ePartitionType)
		{
			case IDMAC_PARTITION_TYPE_TOTAL:
				//buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				len = buf_len;
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);
				break;

			case IDMAC_PARTITION_TYPE_HEAD:
				//buf = addr_align_flag?buf_skip_noalign_addr:(BSP_U32)pstIdmacHandle->assisit_buf;
				//len = addr_align_flag ? IDMAC_BUF_MAXSIZE : IDMAC_get_addr_noalign_bytes(u32buf);
				len = buf_len;
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);				
				break;

			case IDMAC_PARTITION_TYPE_MID:
				//buf = addr_align_flag ? (buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = buf_len;
				break;

			case IDMAC_PARTITION_TYPE_TAIL:
				//buf = addr_align_flag?(buf_skip_noalign_addr+cnt*IDMAC_BUF_MAXSIZE):(buf_skip_noalign_addr+(cnt-1)*IDMAC_BUF_MAXSIZE);
				len = buf_len;
				break;
			default:
				break;	
		}
		/*填充描述符*/
		IDMAC_set_qptr(pstIdmacHandle, ePartitionType,IDMAC_PARTITION_TYPE_NORMAL, buf, len ,&desc);
		u32NodeCnt_new ++;
		hisdio_trace(0, "cnt %d: [u32NodeCnt_new : %d]", cnt ,u32NodeCnt_new);
		if((IDMAC_PARTITION_TYPE_HEAD == ePartitionType)
			|| (IDMAC_PARTITION_TYPE_TOTAL == ePartitionType))
		{
			descTag = desc;
		}

		if(IDMAC_PARTITION_TYPE_TAIL == ePartitionType)
		{
		    hisdio_trace(2, "set own bit to 1. descTag:%p",descTag);
		    if(descTag)
			    IDMAC_set_DmaOwn(descTag);
		}
	}
	if(3 >= HISDIO_TRACE_LEVEL)
	{
	    for(cnt=0;cnt < u32NodeCnt_new;cnt++)
	    {
	        struct tagDmaDesc* tmp = pstIdmacHandle->Desc + cnt;
	     hisdio_trace(1, "dmadesc: addr:0x%p,status:0x%8x, len:0x%8x,buf1:0x%8x,buf2:0x%8x ",tmp,tmp->status,
                      tmp->length,tmp->buffer1, tmp->buffer2);
	    }
    }	    
	hisdio_trace(1, "resume dma, start dma transfer!");	
	IDMAC_resume_dma();
	return BSP_OK;
}

BSP_S32	 IDMAC_SingleChannelReceiveLLI(struct scatterlist *sgHeader, unsigned int dma_len)
{
	unsigned int cnt=0;
	DmaDesc *desc = BSP_NULL;
	DmaDesc *descTag = BSP_NULL;
#if !(HIIDMAC_SG_OVER_LEN_NDEBUG)
	BSP_U32 u32NodeCnt  = 0;
#endif
	BSP_U32 u32NodeCnt_new = 0;	
	struct scatterlist  *sgNode = sgHeader;
	IDMAC_PARTITION_TYPE_E ePartitionType = IDMAC_PARTITION_TYPE_TOTAL;
	dma_addr_t buf = 0;
	BSP_U32 buf_len= 0;
	BSP_U32 len = 0;
	BSP_BOOL addr_align_flag = BSP_TRUE;
	if(!pstIdmacHandle)
	{
		hisdio_error("please initialize the dmac first, perform IDMAC_Init");
		return BSP_ERROR;
	}	
	for_each_sg(sgHeader, sgNode, dma_len, cnt) 
	{
		ePartitionType = IDMAC_get_node_partition(cnt, dma_len);
		buf = sg_dma_address(sgNode);//phy
		//buf_len = sg_dma_len(sgNode);
		buf_len = sgNode->length;
		hisdio_trace(1, "buf_len: %d, buf address: 0x%x ", buf_len, buf);
		addr_align_flag = IDMAC_check_addr_align(buf);
		if((!addr_align_flag))
		{
			hisdio_error("buf node : %d not alignment,buf address : 0x%x, buf len : %d", cnt, buf, buf_len);
			return BSP_ERROR;
		}	
		else if(buf_len > (IDMAC_BUF_MAXSIZE))
		{
#if HIIDMAC_SG_OVER_LEN_NDEBUG		
			hisdio_error("buf node : buf len : %d", buf_len);
			return BSP_ERROR;
#else
            hisdio_trace(2, "buf_len over 4k: %d", buf_len);
			/*deal with in case of  sg->length > descriptor_max_length*/
			u32NodeCnt = IDMAC_get_node_count(buf, buf_len);
			if(u32NodeCnt < 1)
			{
				hisdio_error("node count calculate error : [count : %d]", u32NodeCnt);
				return BSP_ERROR;
			}
			if(((cnt -1) + u32NodeCnt) > IDMAC_DESC_SIZE)
			{
				hisdio_error("too many LLI descriptor!");
				return BSP_ERROR;
			}
			if(BSP_OK != IDMAC_set_node_ptr(buf, buf_len, u32NodeCnt, ePartitionType,&descTag))
			{
				hisdio_error("lli separate node failed!");
				return BSP_ERROR;
			}
			u32NodeCnt_new = u32NodeCnt_new + u32NodeCnt;
			hisdio_trace(0, "cnt %d: [u32NodeCnt_new : %d]", cnt ,u32NodeCnt_new);
			continue;//loop next cnt
#endif
		}		
		switch(ePartitionType)
		{
			case IDMAC_PARTITION_TYPE_TOTAL:				
				len = buf_len;
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);
				hisdio_trace(1,"total enter,descdma: 0x%x",(BSP_U32)pstIdmacHandle->DescDma);
				break;

			case IDMAC_PARTITION_TYPE_HEAD:
				len = buf_len;
				pstIdmacHandle->Next = 0;
				IDMAC_init_desc_base(pstIdmacHandle);				
				break;

			case IDMAC_PARTITION_TYPE_MID:
				hisdio_trace(1, "next is %d", pstIdmacHandle->Next);
				len = buf_len;
				break;

			case IDMAC_PARTITION_TYPE_TAIL:	
				hisdio_trace(1, "next is %d", pstIdmacHandle->Next);
				len = buf_len;
				break;
			default:
				break;	
		}
		/*填充描述符*/
		IDMAC_set_qptr(pstIdmacHandle, ePartitionType,IDMAC_PARTITION_TYPE_NORMAL, buf, len ,&desc);
		if((IDMAC_PARTITION_TYPE_HEAD == ePartitionType)
			|| (IDMAC_PARTITION_TYPE_TOTAL == ePartitionType))
		{
			descTag = desc;
			hisdio_trace(1,"descTag = desc");
		}
		/*解决descTag 空指针问题*/
		if(BSP_NULL != descTag)
	    {
           if(IDMAC_PARTITION_TYPE_TAIL == ePartitionType)
		   {
			  hisdio_trace(1,"descTag set own");
			  IDMAC_set_DmaOwn(descTag);
		   }
		}
		
	}
#if 0	
	if(3 >= HISDIO_TRACE_LEVEL)
	{
	    for(cnt=0;cnt < u32NodeCnt_new;cnt++)
	    {
	        struct tagDmaDesc* tmp = pstIdmacHandle->Desc + cnt;
	     hisdio_trace(1, "dmadesc: addr:0x%p,status:0x%8x, len:0x%8x,buf1:0x%8x,buf2:0x%8x ",tmp,tmp->status,
                      tmp->length,tmp->buffer1, tmp->buffer2);
	    }
    }
#endif    
	hisdio_trace(1,"end");
	IDMAC_resume_dma();
	return BSP_OK;
}

BSP_S32	IDMAC_SingleChannelStart(struct scatterlist *sgHeader, unsigned int dma_len,  enum dma_data_direction direction)
{
	BSP_S32 ret = BSP_OK;
	if(DMA_TO_DEVICE == direction)
	{
		/*memory to peripheral*/
		ret = IDMAC_SingleChannelSendLLI(sgHeader, dma_len);
	}
	else if(DMA_FROM_DEVICE == direction)
	{
		/*peripheral to memory*/
		ret = IDMAC_SingleChannelReceiveLLI(sgHeader, dma_len);
	}
	else
	{
		return BSP_ERROR;
	}
	return ret;
}

BSP_VOID  IDMAC_config_init()
{
	/*设置总线模式*/
	sdio_dma_bus_mode_init(DmaEnable | DmaFixedBurstEnable |DmaDescriptorSkip0);

	/*清除所有的中断状态*/
	sdio_dma_interrupt_clear(DmaStatusAbnormal | DmaStatusNormal | DmaStatusCardErrSum |DmaStatusNoDesc |
						DmaStatusBusError | DmaStatusRxCompleted | DmaStatusTxCompleted);

	/*设置中断使能位*/
	sdio_dma_interrupt_mask(DmaIntAbnormal | DmaIntNormal | DmaIntCardErrSum |DmaIntBusError | DmaIntRxCompleted | DmaIntTxCompleted);
}

 /**********************************************
 *function:	DMA_Init
 *description:	DMA inititation
 *input:		BSP_VOID
 *output:		NULL
 *return:		BSP_OK,BSP_ERROR
 *
 *************************************************/
 BSP_S32	IDMAC_Init(void __iomem * base)
 {
 	/*FIXME? add this function to sdio driver's probe, internal dmac*/
 	hisdio_trace(1, "enter");
	if(!base)
	{
		hisdio_error("register base virtual address NUll, failed!");
		return BSP_ERROR;
	}
	balong_internal_base = base;
	hisdio_trace(1, "internal base : 0x%p", balong_internal_base);
	/*配置内置DMA句柄*/
	pstIdmacHandle = (IDMAC_HANDLE_S*)kmalloc(sizeof(IDMAC_HANDLE_S), GFP_KERNEL);
	/*check ptr alignment, mod should be 0*/
#if 0
	if(!IDMAC_check_addr_align(pstIdmacHandle))
	{
		hisdio_error("pstIdmacHandle not alignment");
		kfree(pstIdmacHandle);
		hisdio_trace(1, "leave");
		return BSP_ERROR;
	}
#endif
	hisdio_trace(1, "pstIdmacHandle addr: 0x%p", pstIdmacHandle);
	memset((BSP_VOID*)pstIdmacHandle, 0x0, sizeof(IDMAC_HANDLE_S));

	/*分配辅助buf:cache line 对齐，linux 没有必要*/
	pstIdmacHandle->assisit_buf = kmalloc(BSP_CACHE_ALIGN_SIZE,GFP_KERNEL);
	if(!pstIdmacHandle->assisit_buf)
	{
		hisdio_error("pstIdmacHandle->assist_buf alloc failed!");
	}
	hisdio_trace(1, "assisit buf : 0x%p", pstIdmacHandle->assisit_buf);
	//IDMAC_config_init();
	/*创建描述符链*/
	if(BSP_OK != IDMAC_alloc_desc_queue(pstIdmacHandle, IDMAC_DESC_SIZE + 1))
	{
		kfree(pstIdmacHandle->assisit_buf);
		kfree(pstIdmacHandle);
		hisdio_trace(1, "leave");
		return BSP_ERROR;
	}

	/*将描述符链首地址通知DMA引擎*/
	IDMAC_init_desc_base(pstIdmacHandle);

	pstIdmacHandle->bDescSetupFlag = BSP_TRUE;
	
	hisdio_trace(1, "leave");
 	return BSP_OK;
 }
 
 BSP_VOID IDMAC_Exit(void __iomem * base)
 {
 	/*disable dma control etc.*/
	hisdio_trace(1, "enter");
	if(!pstIdmacHandle)
	{
		hisdio_error("pstIdmacHandle is NULL!");
		return;
	}
	IDMAC_free_desc_queue(pstIdmacHandle);
	kfree(pstIdmacHandle->assisit_buf);
	kfree(pstIdmacHandle);
	/*free memmory*/
	hisdio_trace(1, "leave");
	
 }

