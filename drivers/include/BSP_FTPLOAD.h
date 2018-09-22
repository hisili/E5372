/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_FTPLOAD.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_FTPLOAD_H__
#define __BSP_FTPLOAD_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/**************************************************************************
  宏定义 
**************************************************************************/

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_FTP_NOT_INIT            BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_MODULE_NOT_INITED)    /*没有初始化*/
#define BSP_ERR_FTP_NULL_PTR            BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_NULL_PTR)             /*参数错误*/
#define BSP_ERR_FTP_INVALID_PARAM       BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_INVALID_PARA)         /*内存空间分配失败*/
#define BSP_ERR_FTP_NO_MEM              BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_BUF_ALLOC_FAILED)     /*循环次数达到最大*/

#define BSP_ERR_FTP_NET                 BSP_DEF_ERR(BSP_MODU_FTP, 0x10)                         /*网络错误*/
#define BSP_ERR_FTP_XFER                BSP_DEF_ERR(BSP_MODU_FTP, 0x11)                         /*FTP建议传输失败*/
#define BSP_ERR_FTP_DATA_TRANSFER       BSP_DEF_ERR(BSP_MODU_FTP, 0x12)                         /*数据传输失败*/
#define BSP_ERR_FTP_CMD                 BSP_DEF_ERR(BSP_MODU_FTP, 0x13)                         /*命令发送失败*/
#define BSP_ERR_FTP_READ                BSP_DEF_ERR(BSP_MODU_FTP, 0x14)                         /*读失败*/
#define BSP_ERR_FTP_WRITE               BSP_DEF_ERR(BSP_MODU_FTP, 0x15)                         /*写失败*/

/**************************************************************************
  枚举定义
**************************************************************************/

/*FTP transport type*/
typedef enum tagFTP_LOAD_TYPE_E
{
    FTP_LOADTYPE_VXWORKS_IMAGE = 0,
	FTP_LOADTYPE_TENCILICA_TEXT,
	FTP_LOADTYPE_TENCILICA_BOOTCODE,
    FTP_LOADTYPE_BOOTSECT,
    FTP_LOADTYPE_COMMONFILE,
    FTP_LOADTYPE_IMPORT,
    FTP_LOADTYPE_IPCFG,
    FTP_LOADTYPE_WITHSPARE,
    FTP_LOADTYPE_SYSTEMIMG,
    FTP_LOADTYPE_BUTTOM
} FTP_LOAD_TYPE_E;


/*****************************************************************************
* 函 数 名  : LOADF
*
* 功能描述  : 
*
* 输入参数  : BSP_U8 * pu8FileName, BSP_U32 u32DestAddr, FTP_LOAD_TYPE_E enLoadType
* 
* 输出参数  : 无
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 LOADF(BSP_U8 * pu8FileName, BSP_U32 u32DestAddr, FTP_LOAD_TYPE_E enLoadType);

/*****************************************************************************
* 函 数 名  : LOADB
*
* 功能描述  : 
*
* 输入参数  : 无
* 
* 输出参数  : 无
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 LOADB(void);

/*****************************************************************************
* 函 数 名  : BSP_LoadDsp
*
* 功能描述  : 
*
* 输入参数  : 无
* 
* 输出参数  : 无
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_LoadDsp(void);

/*****************************************************************************
* 函 数 名  : BSP_RunDsp
*
* 功能描述  : 
*
* 输入参数  : 无
* 
* 输出参数  : 无
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_VOID BSP_RunDsp(void);

/*****************************************************************************
* 函 数 名  : BSP_BootDsp
*
* 功能描述  : 
*
* 输入参数  : 无
* 
* 输出参数  : 无
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_BootDsp(void);
BSP_VOID BSP_ReRunDsp();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_FTPLOAD_H__ */


