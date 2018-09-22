/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IPCONFIG_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_IPCONFIG_API_H__
#define __BSP_IPCONFIG_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


typedef enum tagBSP_NET_IPVER_E
{
  IPV4 = 0,
  IPV6
}BSP_NET_IPVER_E;

typedef struct tagBSP_NET_NETMASK_S
{
  BSP_U8 au8Mask[4];   //��������
}BSP_NET_NETMASK_S;

typedef struct tagBSP_NET_MAC_S
{
  BSP_U8 au8MAC[6];   // MAC��ַ
}BSP_NET_MAC_S;

typedef union tagBSP_NET_IP_U
{
  BSP_U8 au8IPV4[4];    // ��������ĵ�ַΪIPV4�ĵ�ַ
  BSP_U8 au8IPV6[16];   //��������ĵ�ַΪIPV6�ĵ�ַ
}BSP_NET_IP_U;


/*****************************************************************************
* �� �� ��  : BSP_SetFtpHostAddr
*
* ��������  : ����FTP HOST IP ��ַ
*
* �������  : pAddr: IP��ַ
*
* �������  : ��
*
* �� �� ֵ  : �ɹ�/ʧ��
*
* �޸ļ�¼  : 2010��1��20��   w65225
*****************************************************************************/
STATUS  BSP_SetFtpHostAddr(BSP_U8* pAddr);

/*****************************************************************************
* �� �� ��  : BSP_SetFtpHostAddrEx
*
* ��������  : ����FTP HOST IP ��ַ, �����浽Flash��
*
* �������  : pAddr: IP��ַ
*
* �������  : ��
*
* �� �� ֵ  : �ɹ�/ʧ��
*
* �޸ļ�¼  : 2010��1��20��   w65225
*****************************************************************************/
STATUS  BSP_SetFtpHostAddrEx(BSP_U8* pAddr);

/*****************************************************************************
* �� �� ��  : BSP_SetPsMacAddr
*
* ��������  : �޸�PS mac��ַ
*
* �������  :
* �������  : ��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2009��9��2��   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetPsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5);

/*****************************************************************************
* �� �� ��  : BSP_SetOsMacAddr
*
* ��������  : �޸�vxWorks mac��ַ
*
* �������  :
* �������  : ��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2009��9��2��   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetOsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5);

/*****************************************************************************
* �� �� ��  : BSP_CfgIpAddr
*
* ��������  : ���õ���IP��ַ
*
* �������  : pIpAddr: ����IP��ַ
*
* �������  : ��
*
* �� �� ֵ  : �ɹ�/ʧ��
*
* �޸ļ�¼  : 2010��1��20��   w65225
*****************************************************************************/
STATUS BSP_CfgIpAddr(BSP_U8* pIpAddr);

/*****************************************************************************
* �� �� ��  : BSP_CfgNetMaskAddr
*
* ��������  : ���õ���NetMask
*
* �������  : pNetMsskAddr: ����NetMask
*
* �������  : ��
*
* �� �� ֵ  : �ɹ�/ʧ��
*
* �޸ļ�¼  : 2010��1��20��   w65225
*****************************************************************************/
STATUS BSP_CfgNetMaskAddr(BSP_U8* pNetMsskAddr);

/*****************************************************************************
* �� �� ��  : BSP_ChangeLocalIpAddr
*
* ��������  : �޸�ȫ�ֱ�����IP ��ֵַ
*
* �������  : pChngIpAddr: IP��ַ
* �������  : ��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2009��9��2��   liumengcun  creat
*****************************************************************************/
STATUS BSP_ChangeLocalIpAddr(BSP_U8 *pChngIpAddr);

/*****************************************************************************
* �� �� ��  : BSP_ChangeLocalNetMask
*
* ��������  : �޸�ȫ�ֱ�����Netmaskֵ
*
* �������  : pChngNetMask: NetMask
* �������  : ��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2009��9��2��   liumengcun  creat
*****************************************************************************/
STATUS BSP_ChangeLocalNetMask(BSP_U8 *pChngNetMask);

/*****************************************************************************
* �� �� ��  : BSP_NET_OMIpMacSet
*
* ��������  : ����vxWorks�� IP/NetMask/Mac��ַ,�����浽Flash�ļ�ϵͳ��
*
* �������  : u32IPVer: IP�汾: V4/V6
*             pOMIP:    IP��ַ
*             pstNetMask:NetMask
*             pOMMac:   Mac��ַ
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_NET_OMIpMacSet(BSP_NET_IPVER_E u32IPVer, BSP_NET_IP_U* pOMIP, BSP_NET_NETMASK_S* stNetMask, BSP_NET_MAC_S* pOMMac);

/*****************************************************************************
* �� �� ��  : BSP_NET_OMIpMacGet
*
* ��������  : ��ȡvxWorks�� IP/NetMask/Mac��ַ
*
* �������  : u32IPVer: IP�汾: V4/V6
*             pOMIP:    IP��ַ
*             pstNetMask:NetMask
*             pOMMac:   Mac��ַ
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_NET_OMIpMacGet(BSP_NET_IPVER_E u32IPVer, BSP_NET_IP_U* pOMIP, BSP_NET_NETMASK_S* pstNetMask, BSP_NET_MAC_S* pOMMac);

/*****************************************************************************
* �� �� ��  : BSP_NET_PsMacSet
*
* ��������  : ����Э��ջMac��ַ,�����浽Flash�ļ�ϵͳ��
*
* �������  : u32IPVer: IP�汾: V4/V6
*             pOMIP:    IP��ַ
*             pstNetMask:NetMask
*             pOMMac:   Mac��ַ
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_NET_PsMacSet(BSP_NET_MAC_S* pLteMac);

/*****************************************************************************
* �� �� ��  : BSP_NET_PsMacGet
*
* ��������  : ��ȡЭ��ջMac��ַ
*
* �������  : pLteMac:   ��ȡ��Mac��ַ
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_NET_PsMacGet(BSP_NET_MAC_S* pLteMac);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_IPCONFIG_API_H__ */


