/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_USB.h
*
*   作    者 :  
*
*   描    述 :  USB模块用户接口文件
*
*   修改记录 :  2012年04月16日  v1.00  创建
*************************************************************************/

#ifndef    __BSP_USB_H__
#define    __BSP_USB_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */


/* MODEM 管脚信号值定义 */
#define SIGNALNOCH 0
#define SIGNALCH 1
#define RECV_ENABLE 1
#define RECV_DISABLE 0
#define SEND_ENABLE 1
#define SEND_DISABLE 0
#define HIGHLEVEL 1
#define LOWLEVEL 0

BSP_VOID USB_UnReset(BSP_VOID);
BSP_BOOL USB_ETH_LinkStatGet(BSP_VOID);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif

