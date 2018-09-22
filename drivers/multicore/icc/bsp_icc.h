/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ICC.h
*
*   作    者 :  z67193
*
*   描    述 :  ICC模块用户头文件
*
*   修改记录 :  2010年01月08日  v1.00  z67193  创建
*
*************************************************************************/

#ifndef _BSP_ICC_H                                                                  
#define _BSP_ICC_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
  头文件包含                            
**************************************************************************/
#include <mach/common/bsp_global.h>
#include <mach/balong_v100r001.h>
#include <mach/hardware.h>

/**************************************************************************
  宏定义 
**************************************************************************/
#define ICC_CHAN_NUM_MAX            (32)

#define ICC_CHAN_PRIORITY_HIGH      (0)
#define ICC_CHAN_PRIORITY_LOW       (255)
#define ICC_CHAN_PRIORITY_DEFAULT   ICC_CHAN_PRIORITY_LOW

#define ICC_CHAN_FIFO_MIN           (32)
#define ICC_CHAN_FIFO_MAX           (65536)
#define ICC_CHAN_FIFO_SIZE1         (1024)
#define ICC_CHAN_FIFO_SIZE2         (2048)
#define ICC_CHAN_FIFO_SIZE3         (4096)

/**************************************************************************
  结构定义
**************************************************************************/
typedef enum tagICC_CHAN_STATE_E
{
	ICC_CHAN_STATE_CLOSED   = 0,
	ICC_CHAN_STATE_OPENED,
	ICC_CHAN_STATE_CLOSEING,
	ICC_CHAN_STATE_BUTT
}ICC_CHAN_STATE_E;

typedef enum tagICC_CHAN_MODE_E{
	ICC_CHAN_MODE_STREAM    = 0,
	ICC_CHAN_MODE_PACKET,
	ICC_CHAN_MODE_BUTT
}ICC_CHAN_MODE_E;

typedef enum tagICC_IOCTL_CMD_E{
	ICC_IOCTL_SET_WRITE_CB  = 0,
	ICC_IOCTL_SET_READ_CB,
	ICC_IOCTL_SET_EVENT_CB,
	ICC_IOCTL_GET_STATE,
	ICC_IOCTL_SET_BUTT
}ICC_IOCTL_CMD_E;


typedef enum tagICC_EVENT_E{
	ICC_EVENT_CLOSE         = 0,
	ICC_EVENT_RESET,
	ICC_EVENT_BUTT
}ICC_EVENT_E;


typedef BSP_U32 (*icc_event_cb)(BSP_U32 u32ChanID, BSP_U32 u32Event, BSP_VOID* Param);
typedef BSP_U32 (*icc_write_cb)(BSP_U32 u32ChanID);
typedef BSP_U32 (*icc_read_cb)(BSP_U32 u32ChanID, BSP_S32 u32Size);

typedef struct tagICC_CHAN_ATTR_S{
	BSP_U32			u32FIFOInSize;
	BSP_U32			u32FIFOOutSize;
	BSP_U32			u32Priority;
	ICC_CHAN_MODE_E		enChanMode;
	BSP_U32			u32TimeOut;
	icc_event_cb		event_cb;
	icc_write_cb		write_cb;
	icc_read_cb		read_cb;
}ICC_CHAN_ATTR_S;

/**************************************************************************
  函数声明
**************************************************************************/
extern BSP_S32 BSP_ICC_Open(BSP_U32 u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);
BSP_S32 BSP_ICC_Close(BSP_U32 u32ChanId);
BSP_S32 BSP_ICC_Write(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Read(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Ioctl(BSP_U32 u32ChanId, BSP_U32 cmd, BSP_VOID *param);


/**************************************************************************
  错误码定义
**************************************************************************/


#define BSP_ERR_ICC_BASE			        (BSP_S32)((0x80000000) | (BSP_DEF_ERR(BSP_MODU_ICC, 0)))
#define BSP_ERR_ICC_NOT_INIT			(BSP_ERR_ICC_BASE + 0x1)
#define BSP_ERR_ICC_NOT_CONNECT		(BSP_ERR_ICC_BASE + 0x2)
#define BSP_ERR_ICC_NULL				(BSP_ERR_ICC_BASE + 0x3)
#define BSP_ERR_ICC_NO_MEM				(BSP_ERR_ICC_BASE + 0x4)
#define BSP_ERR_ICC_INVALID_CHAN		(BSP_ERR_ICC_BASE + 0x5)
#define BSP_ERR_ICC_INVALID_PARAM		(BSP_ERR_ICC_BASE + 0x6)
#define BSP_ERR_ICC_EXIST				(BSP_ERR_ICC_BASE + 0x7)
#define BSP_ERR_ICC_NOT_EXIST			(BSP_ERR_ICC_BASE + 0x8)
#define BSP_ERR_ICC_OPENED				(BSP_ERR_ICC_BASE + 0x9)
#define BSP_ERR_ICC_NOT_OPEN			(BSP_ERR_ICC_BASE + 0xa)
#define BSP_ERR_ICC_NO_SEM				(BSP_ERR_ICC_BASE + 0xb)
#define BSP_ERR_ICC_NEGOTIATE_FAIL		(BSP_ERR_ICC_BASE + 0xc)
#define BSP_ERR_ICC_BUFFER_FULL			(BSP_ERR_ICC_BASE + 0xd)
#define BSP_ERR_ICC_BUFFER_EMPTY		(BSP_ERR_ICC_BASE + 0xe)
#define BSP_ERR_ICC_INVALID_PACKET		(BSP_ERR_ICC_BASE + 0xf)
#define BSP_ERR_ICC_PACKET_SIZE			(BSP_ERR_ICC_BASE + 0x10)
#define BSP_ERR_ICC_USER_BUFFER_SIZE	(BSP_ERR_ICC_BASE + 0x11)


#define ARM_GIC_IPI_COUNT		16
#define GIC_SW_INTERRUPT			(IO_ADDRESS(PBXA9_GIC_DIST_CONTROL) + 0xf00)

#ifdef __cplusplus
}
#endif

#endif   


