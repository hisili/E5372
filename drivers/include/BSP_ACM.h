/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ACM.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_ACM_H__
#define __BSP_ACM_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */




typedef BSP_VOID (*ACM_WRITE_DONE_CB_T)(char* pDoneBuff, int s32DoneSize);
typedef BSP_VOID (*ACM_READ_DONE_CB_T)(BSP_VOID);
typedef BSP_VOID (*ACM_EVENT_CB_T)(ACM_EVT_E enEvt);
typedef BSP_VOID (*ACM_FREE_CB_T)(char* pBuff);
typedef BSP_VOID (*ACM_MODEM_MSC_READ_CB_T)(MODEM_MSC_STRU* pModemMsc);
typedef BSP_VOID (*ACM_MODEM_REL_IND_CB_T)(BSP_BOOL bEnable);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_ACM_H__ */


