/*************************************************************************
*   版权所有(C) 1987-2015, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ICC_OM.h
*
*   作    者 :  z67193
*
*   描    述 :  ICC模块可谓可测头文件
*
*   修改记录 :
*   Date        Author      DTS number        Descriptor 
*   2012-5-12   z67193      /                 created
*************************************************************************/
#ifndef _BSP_ICC_OM_H                                                                  
#define _BSP_ICC_OM_H

#ifdef __cplusplus
extern "C"
{
#endif


/**************************************************************************
  头文件包含                            
**************************************************************************/
#include "product_config.h"
#include "BSP.h"
#include "bsp_icc_drv.h"
#include <mach/common/mem/bsp_mem.h>

/**************************************************************************
  宏定义 
**************************************************************************/
#define ICC_OM_DEBUG

#define ICC_OM_CHAN_CTRL     100
#define ICC_OM_CHAN_ALL      101

/**************************************************************************
  枚举定义
**************************************************************************/
typedef enum _ICC_OM_EVENT_E
{
    ICC_OM_EVENT_INIT  = 0,
    ICC_OM_EVENT_CONNECTING,
    ICC_OM_EVENT_CONNECTED,
    ICC_OM_EVENT_CONNECT_FAIL,
    ICC_OM_EVENT_OPEN,
    ICC_OM_EVENT_OPEN_OK,
    ICC_OM_EVENT_OPEN_FAIL,
    ICC_OM_EVENT_OPEN_ALLOC,
    ICC_OM_EVENT_OPEN_WAIT,
    ICC_OM_EVENT_WRITE_STREAM,
    ICC_OM_EVENT_WRITE_PACKET,
    ICC_OM_EVENT_WRITE_FAIL,
    ICC_OM_EVENT_READ_STREAM,
    ICC_OM_EVENT_READ_PACKET,
    ICC_OM_EVENT_READ_FAIL,
    ICC_OM_EVENT_MAX
}ICC_OM_EVENT_E;

/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct _ICC_EVENT_S
{
    BSP_U16             event;
    BSP_U16             chan;
    BSP_U32             param1;
    BSP_U32             param2;
    BSP_U32             TimeStamp;
}ICC_EVENT_S;

typedef struct _ICC_OM_CTRL_S
{
    BSP_U32             IccOmEnbale;
    BSP_U32             IccOmAddrBase;
    BSP_U32             IccOmAddrSize;
    BSP_U32             IccOmAddrPos;
}ICC_OM_CTRL_S;

/**************************************************************************
  函数声明
**************************************************************************/
BSP_VOID ICC_OmInit(BSP_VOID);
BSP_VOID ICC_EventRecord(ICC_EVENT_E event, BSP_U32 ChanId, BSP_U32 param1, BSP_U32 param2);
BSP_VOID ICC_ExcSave(BSP_VOID);


/**************************************************************************
  全局变量声明
**************************************************************************/


/**************************************************************************
  错误码定义
**************************************************************************/



#ifdef __cplusplus
}
#endif

#endif   




