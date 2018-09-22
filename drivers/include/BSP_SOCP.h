/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_GPIO.h
*
*   作    者 :  
*
*   描    述 :  IPF模块用户接口文件
*
*   修改记录 :  2012年04月16日  v1.00  创建
*************************************************************************/

#ifndef __BSP_SOCP_H__
#define __BSP_SOCP_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */


/*************************SOCP BEGIN************************/
/**************************************************************************
  宏定义 
**************************************************************************/

BSP_S32 BSP_SOCP_DrxBakReg() ;
BSP_VOID BSP_SOCP_DrxRestoreReg() ;

BSP_S32 BSP_BBPDMA_DrxBakReg();
BSP_S32 BSP_BBPDMA_DrxRestoreReg();
BSP_U32 BSP_SOCP_CanSleep(BSP_VOID);
BSP_VOID BSP_SOCP_DrxRestoreRegAppOnly(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_SOCP_CoderSetHifiSrcChan
* 功能描述  : 分配固定通道
* 输入参数  : 目的通道ID
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
extern BSP_S32 BSP_SOCP_CoderSetHifiSrcChan(BSP_U32 u32DestChanID);
#define  DRV_SOCP_CoderSetHifiSrcChan(u32DestChanID)  BSP_SOCP_CoderSetHifiSrcChan(u32DestChanID)

/*****************************************************************************
* 函 数 名  : BSP_SOCP_StartHifiChan
* 功能描述  : 启动固定通道
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
extern BSP_VOID BSP_SOCP_StartHifiChan(BSP_VOID);
#define DRV_SOCP_StartHifiChan BSP_SOCP_StartHifiChan

/*************************SOCP END************************/




#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif
