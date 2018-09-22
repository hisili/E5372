/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_USB_NCM.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_USB_NCM_H__
#define __BSP_USB_NCM_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* ������*/
/*ר�ô�����*/
#define BSP_ERR_NCM_NOTXBUFF         BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_NCM_NOIDLEDEV        BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_NCM_AT_SEND_TIMEOUT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 3))/*0x10121003*/
#define BSP_ERR_NCM_WRONG_STATE      BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 4))/*0x10121004*/
#define BSP_ERR_NCM_AT_SEND_FAIL     BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 5))/*0x10121005*/
#define BSP_ERR_NCM_SEND_EXCEED_MAXPKT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 6))/*0x10121006*/
#define BSP_ERR_NCM_IPV6_DNS_NFT_FAIL  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 7))/*0x10121007*/


/*****************************************************************************
* �� �� ��  : BSP_NCM_Open
*
* ��������  : ������ȡ����ʹ��NCM�豸ID,����ͨ�����ͣ�PSʹ��;����ͨ�����ͣ�
*              MSP����AT����ʹ��
*
* �������  : NCM_DEV_TYPE_E enDevType  �豸����
* �������  : pu32NcmDevId              NCM �豸ID
*
* �� �� ֵ  : BSP_ERR_NET_NOIDLEDEV
*             OK
*
* �޸ļ�¼  : 2010��3��17��   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Open(NCM_DEV_TYPE_E enDevType, BSP_U32 * pu32NcmDevId);

/*****************************************************************************
* �� �� ��  : BSP_NCM_Write
*
* ��������  : ��������
*
* �������  : BSP_U32 u32NcmDevId  NCM�豸ID
                             void *     net_priv tcp/ip�����Ĳ���������tcp/ip�Խ�ʱ�ò�������ΪNULL
* �������  : BSP_VOID *pPktEncap       ����װ�׵�ַ
*
* �� �� ֵ  : BSP_OK
*             BSP_ERR_NET_INVALID_PARA
*             BSP_ERR_NET_BUF_ALLOC_FAILED
*
* �޸ļ�¼  : 2010��3��17��   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Write(BSP_U32 u32NcmDevId, BSP_VOID *pPktEncap, void *net_priv);

/*****************************************************************************
* �� �� ��  : BSP_NCM_Ioctl
*
* ��������  : ����NCM�豸����
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : 
*
* �޸ļ�¼  : 2010��3��17��   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Ioctl(BSP_U32 u32NcmDevId, NCM_IOCTL_CMD_TYPE_E enNcmCmd, BSP_VOID *param);

/*****************************************************************************
* �� �� ��  : BSP_NCM_Close
*
* ��������  : �ر�NCM�豸
*
* �������  : NCM_DEV_TYPE_E enDevType   �豸ID����
*             BSP_U32 u32NcmDevId        NCM�豸ID
* �������  : ��
*
* �� �� ֵ  : OK
*
* �޸ļ�¼  : 2010��3��17��   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Close(NCM_DEV_TYPE_E enDevType, BSP_U32 u32NcmDevId);

/*****************************************************************************
* �� �� ��  : NCM_debugPktGet
*
* ��������  : �ṩ��PS��ȡ�շ���ͳ��
*
* �������  : 
* �������  : NCM_PKT_INFO_S *penPktNum :ͳ�ƽṹ��
*
* �� �� ֵ  : OK/ERROR
*
* �޸ļ�¼  : 2011��8��5��   wangjing   creat
*****************************************************************************/
BSP_S32 NCM_debugPktGet(BSP_U32 u32DevId,NCM_PKT_INFO_S *penPktNum);

/*****************************************************************************
* �� �� ��  : NCM_debugPktClear
*
* ��������  : �ṩ��PS���ڽ�ͳ��ֵ��0
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : OK/ERROR
*
* �޸ļ�¼  : 2011��8��5��   wangjing   creat
*****************************************************************************/
BSP_S32 NCM_debugPktClear(BSP_U32 u32DevId);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_USB_NCM_H__ */


