/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_OM_API.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_OM_API_H__
#define __BSP_OM_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



/*************************OM BEGIN************************/

typedef enum{
     MEMCHKTYPE16BIT,
     MEMCHKTYPE32BIT
}ENMEMCHKTYPE;

/*****************************************************************************
* 函 数 名  : BSP_OM_GetFlashSpec
*
* 功能描述  : 获取FLASH规格
*
* 输入参数  : 无
*
* 输出参数  : BSP_U32 *pu32FlashTotalSize  : FLASH总大小
*             BSP_U32 *pu32FlashBlkSize     : FLASH块大小
*             BSP_U32 *pu32FlashPageSize    : FLASH页大小
*
* 返 回 值  : BSP_ERROR:获取失败
*             BSP_OK:获取成功
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_OM_GetFlashSpec(BSP_U32 *pu32FlashTotalSize, BSP_U32 *pu32FlashBlkSize, BSP_U32 *pu32FlashPageSize);
/*****************************************************************************
* 函 数 名  : BSP_OM_GetVerTime
*
* 功能描述  : 获得ver时间
*
* 输入参数  : 无
*
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_OM_GetVerTime(BSP_S8* strVerTime);

/******************************************************************************
* Function     :   BSP_OM_MemRead
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemRead(BSP_U32 u32MemAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);
/******************************************************************************
* Function     :   BSP_OM_MemWrite
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemWrite(BSP_U32 u32MemAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);
/******************************************************************************
* Function     :   BSP_OM_MemNWrite
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemNWrite(BSP_U32 u32MemAddr, BSP_U32 u32Value, BSP_U32 u32Count);
/******************************************************************************
* Function     :   BSP_OM_MemPrint
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemPrint(BSP_U32 u32MemAddr,ENADDRTYPE enAddrType);
/******************************************************************************
* Function     :   BSP_OM_MemChk
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemChk(BSP_U32 u32StartAddr, BSP_U32 u32EndAddr, ENMEMCHKTYPE enChkType);
/******************************************************************************
* Function     :   BSP_OM_GetVersion
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_VOID BSP_OM_GetVersion(void);

/******************************************************************************
* Function     :   BSP_OM_GetFPGAVer
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_GetFPGAVer(void);

/******************************************************************************
* Function     :   BSP_OM_ShellLock
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_ShellLock(BSP_BOOL bRequest);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardType
*
* 功能描述  : 获取单板类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_GetHostCore
*
* 功能描述  : 查询当前CPU主从核类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : CPU主从核类型
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_CORE_TYPE_E BSP_GetHostCore(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetChipType
*
* 功能描述  : 获取芯片类型
*
* 输入参数  : BSP_VOID  
*
* 输出参数  : 无
*
* 返 回 值  : 芯片类型
*             PV500_CHIP:PV500芯片
*             V7R1_CHIP: V7R1芯片
*               
* 其它说明  : 无
*
*****************************************************************************/
BSP_CHIP_TYPE_E BSP_OM_GetChipType(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetTcmAdr
*
* 功能描述  : 获取DSP邮箱基地址
*
* 输入参数  : BSP_VOID  
* 输出参数  : 无
*
* 返 回 值  : DSP邮箱基地址
*
* 其它说明  : 返回值0为非法值
*
*****************************************************************************/
BSP_U32 BSP_OM_GetTcmAdr(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_MspProcReg
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
extern void BSP_MspProcReg(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);


#ifdef __VXWORKS__
/*****************************************************************************
Function   : DR_NV_Read
Description: Read NV from the file without Auth.
Input      : VOS_UINT16 usID -> NV ID
             VOS_VOID *pItem -> Save NV data buffer
             VOS_UINT32 ulLength -> buffer length
Return     : Ok or Err.
Other      :
*****************************************************************************/
unsigned int DR_NV_Read(unsigned short usID,void *pItem,unsigned int ulLength);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_OM_API_H__ */


