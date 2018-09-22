/*************************************************************************
*   ��Ȩ����(C) 1987-2015, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_ICC_OM.h
*
*   ��    �� :  z67193
*
*   ��    �� :  ICCģ���ν�ɲ�ͷ�ļ�
*
*   �޸ļ�¼ :
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
  ͷ�ļ�����                            
**************************************************************************/
#include "product_config.h"
#include "BSP.h"
#include "bsp_icc_drv.h"
#include <mach/common/mem/bsp_mem.h>

/**************************************************************************
  �궨�� 
**************************************************************************/
#define ICC_OM_DEBUG

#define ICC_OM_CHAN_CTRL     100
#define ICC_OM_CHAN_ALL      101

/**************************************************************************
  ö�ٶ���
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
  STRUCT����
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
  ��������
**************************************************************************/
BSP_VOID ICC_OmInit(BSP_VOID);
BSP_VOID ICC_EventRecord(ICC_EVENT_E event, BSP_U32 ChanId, BSP_U32 param1, BSP_U32 param2);
BSP_VOID ICC_ExcSave(BSP_VOID);


/**************************************************************************
  ȫ�ֱ�������
**************************************************************************/


/**************************************************************************
  �����붨��
**************************************************************************/



#ifdef __cplusplus
}
#endif

#endif   




