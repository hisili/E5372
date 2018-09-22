/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SYSCLK.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_SYSCLK_H__
#define __BSP_SYSCLK_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/*****************************************************************************
* 函 数 名  : sysClkTicksGet
*
* 功能描述  : 获得系统时钟启动以来的tick数.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 系统时钟启动以来的tick数.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
extern BSP_U32 sysClkTicksGet (BSP_VOID);

/*****************************************************************************
* 函 数 名  : sysAuxClkTicksGet
*
* 功能描述  : 获得系统辅助时钟启动以来的tick数.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 系统辅助时钟启动以来的tick数.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_U32 sysAuxClkTicksGet (BSP_VOID);

/*****************************************************************************
* 函 数 名  : sysTimestampRateSet
*
* 功能描述  : This routine sets the interrupt rate of the timestamp clock.  It does 
*             not enable system clock interrupts unilaterally, but if the timestamp is currently enabled, the clock is disabled and then 
*             re-enabled with the new rate.  Normally it is called by usrRoot() 
*             in usrConfig.c.
*
* 输入参数  : int ticksPerSecond   number of clock interrupts per second 
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateSet(BSP_S32 ticksPerSecond);

/*****************************************************************************
* 函 数 名  : sysTimestampRateGet
*
* 功能描述  : This routine returns the interrupt rate of the timestamp clock.
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : The number of ticks per second of the system clock.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateGet (BSP_VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SYSCLK_H__ */

