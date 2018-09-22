/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_USB.h
*
*   ��    �� :  
*
*   ��    �� :  USBģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��04��16��  v1.00  ����
*************************************************************************/

#ifndef    __BSP_USB_H__
#define    __BSP_USB_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */


/* MODEM �ܽ��ź�ֵ���� */
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

