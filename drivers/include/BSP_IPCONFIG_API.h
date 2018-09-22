/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IPCONFIG_API.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
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
  BSP_U8 au8Mask[4];   //子网掩码
}BSP_NET_NETMASK_S;

typedef struct tagBSP_NET_MAC_S
{
  BSP_U8 au8MAC[6];   // MAC地址
}BSP_NET_MAC_S;

typedef union tagBSP_NET_IP_U
{
  BSP_U8 au8IPV4[4];    // 标明传入的地址为IPV4的地址
  BSP_U8 au8IPV6[16];   //标明传入的地址为IPV6的地址
}BSP_NET_IP_U;


/*****************************************************************************
* 函 数 名  : BSP_SetFtpHostAddr
*
* 功能描述  : 设置FTP HOST IP 地址
*
* 输入参数  : pAddr: IP地址
*
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 修改记录  : 2010年1月20日   w65225
*****************************************************************************/
STATUS  BSP_SetFtpHostAddr(BSP_U8* pAddr);

/*****************************************************************************
* 函 数 名  : BSP_SetFtpHostAddrEx
*
* 功能描述  : 设置FTP HOST IP 地址, 并保存到Flash中
*
* 输入参数  : pAddr: IP地址
*
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 修改记录  : 2010年1月20日   w65225
*****************************************************************************/
STATUS  BSP_SetFtpHostAddrEx(BSP_U8* pAddr);

/*****************************************************************************
* 函 数 名  : BSP_SetPsMacAddr
*
* 功能描述  : 修改PS mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetPsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5);

/*****************************************************************************
* 函 数 名  : BSP_SetOsMacAddr
*
* 功能描述  : 修改vxWorks mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetOsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5);

/*****************************************************************************
* 函 数 名  : BSP_CfgIpAddr
*
* 功能描述  : 设置单板IP地址
*
* 输入参数  : pIpAddr: 单板IP地址
*
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 修改记录  : 2010年1月20日   w65225
*****************************************************************************/
STATUS BSP_CfgIpAddr(BSP_U8* pIpAddr);

/*****************************************************************************
* 函 数 名  : BSP_CfgNetMaskAddr
*
* 功能描述  : 设置单板NetMask
*
* 输入参数  : pNetMsskAddr: 单板NetMask
*
* 输出参数  : 无
*
* 返 回 值  : 成功/失败
*
* 修改记录  : 2010年1月20日   w65225
*****************************************************************************/
STATUS BSP_CfgNetMaskAddr(BSP_U8* pNetMsskAddr);

/*****************************************************************************
* 函 数 名  : BSP_ChangeLocalIpAddr
*
* 功能描述  : 修改全局变量的IP 地址值
*
* 输入参数  : pChngIpAddr: IP地址
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
STATUS BSP_ChangeLocalIpAddr(BSP_U8 *pChngIpAddr);

/*****************************************************************************
* 函 数 名  : BSP_ChangeLocalNetMask
*
* 功能描述  : 修改全局变量的Netmask值
*
* 输入参数  : pChngNetMask: NetMask
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
STATUS BSP_ChangeLocalNetMask(BSP_U8 *pChngNetMask);

/*****************************************************************************
* 函 数 名  : BSP_NET_OMIpMacSet
*
* 功能描述  : 设置vxWorks的 IP/NetMask/Mac地址,并保存到Flash文件系统中
*
* 输入参数  : u32IPVer: IP版本: V4/V6
*             pOMIP:    IP地址
*             pstNetMask:NetMask
*             pOMMac:   Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_OMIpMacSet(BSP_NET_IPVER_E u32IPVer, BSP_NET_IP_U* pOMIP, BSP_NET_NETMASK_S* stNetMask, BSP_NET_MAC_S* pOMMac);

/*****************************************************************************
* 函 数 名  : BSP_NET_OMIpMacGet
*
* 功能描述  : 获取vxWorks的 IP/NetMask/Mac地址
*
* 输入参数  : u32IPVer: IP版本: V4/V6
*             pOMIP:    IP地址
*             pstNetMask:NetMask
*             pOMMac:   Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_OMIpMacGet(BSP_NET_IPVER_E u32IPVer, BSP_NET_IP_U* pOMIP, BSP_NET_NETMASK_S* pstNetMask, BSP_NET_MAC_S* pOMMac);

/*****************************************************************************
* 函 数 名  : BSP_NET_PsMacSet
*
* 功能描述  : 设置协议栈Mac地址,并保存到Flash文件系统中
*
* 输入参数  : u32IPVer: IP版本: V4/V6
*             pOMIP:    IP地址
*             pstNetMask:NetMask
*             pOMMac:   Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_PsMacSet(BSP_NET_MAC_S* pLteMac);

/*****************************************************************************
* 函 数 名  : BSP_NET_PsMacGet
*
* 功能描述  : 获取协议栈Mac地址
*
* 输入参数  : pLteMac:   获取的Mac地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_NET_PsMacGet(BSP_NET_MAC_S* pLteMac);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_IPCONFIG_API_H__ */


