/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ICC_DRV.h
*
*   作    者 :  z67193
*
*   描    述 :  ICC模块驱动头文件
*
*   修改记录 :  2010年01月08日  v1.00  z67193  创建
*
*************************************************************************/

#ifndef _BSP_ICC_DRV_H                                                                  
#define _BSP_ICC_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif


/**************************************************************************
  头文件包含                            
**************************************************************************/

#include <linux/spinlock.h>
#include <asm/cacheflush.h>
#include <mach/common/bsp_memory.h>
#include "BSP.h"

/* V7R1暂不添加ICC可维可测功能，后续在FeatureConfig.h中添加，并且在C核ICC模块中同步添加 */
#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#define FEATURE_ON                                  1
#define FEATURE_OFF                                 0
#define FEATURE_ICC_DEBUG                           FEATURE_OFF
#endif

/**************************************************************************
  宏定义 
**************************************************************************/
#define SM_BASE_ADDR			MEMORY_AXI_ICC_ADDR

#define ICC_CACHEABLE

#define ICC_PACKET_HEADER       0x71abcdef

#define  ICC_DEBUG_printk printk

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
#define ICC_PACKET_HEADER_LEN   12
#else
#define ICC_PACKET_HEADER_LEN   8
#endif

#ifdef BSP_CORE_MODEM
#define ICC_HOST            0
#define ICC_TAGET           1
#else
#define ICC_HOST            1
#define ICC_TAGET           0
#endif

// 添加列表节点方式
#define ICC_LIST_ADD_PRIORITY	0
#define ICC_LIST_ADD_HEAD		1
#define ICC_LIST_ADD_TAIL		2

// 通道信号定义
#define ICC_CHAN_SIGNAL_OPEN	0x1
#define ICC_CHAN_SIGNAL_CLOSE	0x2
#define ICC_CHAN_SIGNAL_RESET	0x4
#define ICC_CHAN_SIGNAL_DATA	0x8
#define ICC_CHAN_SIGNAL_EMPTY	0x10


/**************************************************************************
  枚举定义
**************************************************************************/
typedef enum tagICC_CMD_E
{
	ICC_CMD_CLOSE       = 0,
	ICC_CMD_RESET,
	ICC_CMD_BUTT
}ICC_CMD_E;


/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct tagICC_PACKET_S
{
	BSP_U8             *data;
	BSP_S32             size;
}ICC_PACKET_S;

typedef struct tagICC_DEBUG_S
{
	BSP_U32			u32SendTimes;
	BSP_U32			u32SendBytes;
	BSP_U32			u32RecvTimes;
	BSP_U32			u32RecvBytes;
	BSP_U32			u32RecvInt;
	BSP_U32			u32LastHaveBytes;
	BSP_U32			u32LastReadBytes;
	BSP_U32			u32NoticeEmpty;
	BSP_U32			u32RecvEmpty;
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
	BSP_U32			u32IdleSize;
#endif
}ICC_DEBUG_S;

typedef struct tagICC_SUB_CHAN_S
{
	BSP_U32			u32SubChanState;
	BSP_U32			u32Signal;
	BSP_U32			u32SpinLock;
	BSP_U32			u32FIFOsize;
	BSP_U8			*pStart;
	BSP_U8			*pEnd;
	BSP_U8			*pWrite;
	BSP_U8			*pRead;
}ICC_SUB_CHAN_S;

typedef struct tagICC_CHAN_S
{
	ICC_CHAN_STATE_E	enState;
	BSP_BOOL			bAlloc;
	BSP_U32				u32Priority;
	BSP_U32				u32SpinLock;
	ICC_CHAN_MODE_E		enMode;
	ICC_SUB_CHAN_S		*pSubChanInfo[2];
}ICC_CHAN_S;

typedef struct tagICC_CTRL_CHAN_S
{
	BSP_BOOL		bInit[2];
	BSP_BOOL		bConnect;
	BSP_U32			u32SpinLock;
	ICC_CHAN_S		astChanInfo[ICC_CHAN_NUM_MAX];
}ICC_CTRL_CHAN_S;

typedef struct tagICC_CHAN_DEV_S
{
	BSP_U32	            u32ChanID;
	BSP_U32	            u32Priority;
	ICC_CHAN_MODE_E		enMode;
	BSP_U32			    *pSpinLock;
	ICC_SUB_CHAN_S	    *ptxSubChanInfo;
	ICC_SUB_CHAN_S	    *prxSubChanInfo;
	BSP_U32	            bWriteNeed;
	BSP_U32	            bFirstRecv;
	icc_event_cb		event_cb;
	icc_read_cb	        read_cb;
	icc_write_cb        write_cb;
	struct semaphore    semSync;
	struct semaphore    semOpen;
	struct semaphore    semRead;
	ICC_PACKET_S        packet;
	ICC_DEBUG_S         sDebug;
	struct tagICC_CHAN_DEV_S *next;
}ICC_CHAN_DEV_S;


typedef struct tagICC_DEV_S
{
	BSP_BOOL            bInit;
}ICC_DEV_S;


/**************************************************************************
  函数声明
**************************************************************************/
BSP_S32 BSP_ICC_Init(void);
BSP_VOID ICC_ListGet(ICC_CHAN_DEV_S *pList, BSP_U32 u32ChanId, ICC_CHAN_DEV_S **pNode);


/**************************************************************************
  全局变量声明
**************************************************************************/
/*
#define ICC_FLUSH_CACHE(ptr, size) \
    cacheFlush(DATA_CACHE, (void  *)(((BSP_U32)(ptr)) & (~0x1F)), (((size) + (((BSP_U32)(ptr)) & 0x1f) + 31)>>5)<<5)

#define ICC_INVALID_CACHE(ptr, size) \
    cacheInvalidate(DATA_CACHE, (void *)((BSP_U32)(ptr) & (~0x1F)), ((size + ((BSP_U32)(ptr) & 0x1f) + 31)>>5)<<5);
*/

#define ICC_FLUSH_CACHE(ptr, size) \
	flush_kernel_vmap_range((void  *)(((BSP_U32)(ptr)) & (~0x1F)), (((size) + (((BSP_U32)(ptr)) & 0x1f) + 31)>>5)<<5)

#define ICC_INVALID_CACHE(ptr, size) \
	invalidate_kernel_vmap_range((void *)((BSP_U32)(ptr) & (~0x1F)), (((size) + ((BSP_U32)(ptr) & 0x1f) + 31)>>5)<<5);


/**************************************************************************
  错误码定义
**************************************************************************/



#ifdef __cplusplus
}
#endif

#endif   



