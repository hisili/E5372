/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_USB_NCM.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_USB_NCM_H__
#define __BSP_USB_NCM_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* 错误码*/
/*专用错误码*/
#define BSP_ERR_NCM_NOTXBUFF         BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_NCM_NOIDLEDEV        BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_NCM_AT_SEND_TIMEOUT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 3))/*0x10121003*/
#define BSP_ERR_NCM_WRONG_STATE      BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 4))/*0x10121004*/
#define BSP_ERR_NCM_AT_SEND_FAIL     BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 5))/*0x10121005*/
#define BSP_ERR_NCM_SEND_EXCEED_MAXPKT  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 6))/*0x10121006*/
#define BSP_ERR_NCM_IPV6_DNS_NFT_FAIL  BSP_DEF_ERR(BSP_MODU_USB_NCM,(BSP_ERR_SPECIAL + 7))/*0x10121007*/


/*****************************************************************************
* 函 数 名  : BSP_NCM_Open
*
* 功能描述  : 用来获取可以使用NCM设备ID,数据通道类型，PS使用;控制通道类型，
*              MSP传输AT命令使用
*
* 输入参数  : NCM_DEV_TYPE_E enDevType  设备类型
* 输出参数  : pu32NcmDevId              NCM 设备ID
*
* 返 回 值  : BSP_ERR_NET_NOIDLEDEV
*             OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Open(NCM_DEV_TYPE_E enDevType, BSP_U32 * pu32NcmDevId);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Write
*
* 功能描述  : 发送数据
*
* 输入参数  : BSP_U32 u32NcmDevId  NCM设备ID
                             void *     net_priv tcp/ip传来的参数，不和tcp/ip对接时该参数设置为NULL
* 输出参数  : BSP_VOID *pPktEncap       包封装首地址
*
* 返 回 值  : BSP_OK
*             BSP_ERR_NET_INVALID_PARA
*             BSP_ERR_NET_BUF_ALLOC_FAILED
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Write(BSP_U32 u32NcmDevId, BSP_VOID *pPktEncap, void *net_priv);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Ioctl
*
* 功能描述  : 配置NCM设备属性
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Ioctl(BSP_U32 u32NcmDevId, NCM_IOCTL_CMD_TYPE_E enNcmCmd, BSP_VOID *param);

/*****************************************************************************
* 函 数 名  : BSP_NCM_Close
*
* 功能描述  : 关闭NCM设备
*
* 输入参数  : NCM_DEV_TYPE_E enDevType   设备ID类型
*             BSP_U32 u32NcmDevId        NCM设备ID
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : 2010年3月17日   liumengcun  creat
*****************************************************************************/
BSP_U32 BSP_NCM_Close(NCM_DEV_TYPE_E enDevType, BSP_U32 u32NcmDevId);

/*****************************************************************************
* 函 数 名  : NCM_debugPktGet
*
* 功能描述  : 提供给PS获取收发包统计
*
* 输入参数  : 
* 输出参数  : NCM_PKT_INFO_S *penPktNum :统计结构体
*
* 返 回 值  : OK/ERROR
*
* 修改记录  : 2011年8月5日   wangjing   creat
*****************************************************************************/
BSP_S32 NCM_debugPktGet(BSP_U32 u32DevId,NCM_PKT_INFO_S *penPktNum);

/*****************************************************************************
* 函 数 名  : NCM_debugPktClear
*
* 功能描述  : 提供给PS用于将统计值清0
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : OK/ERROR
*
* 修改记录  : 2011年8月5日   wangjing   creat
*****************************************************************************/
BSP_S32 NCM_debugPktClear(BSP_U32 u32DevId);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_USB_NCM_H__ */


