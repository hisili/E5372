/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_USRCLK.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP 用户时钟定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_USRCLK_H__
#define __BSP_USRCLK_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */

/* 定时器使能位*/
#define CLK_DEF_TC_ENABLE	1     /* 1：Timer使能*/
#define	CLK_DEF_TC_DISABLE	0     /* 0：Timer禁止*/

#define BSP_ERR_CLK_NO_FREE_CLK         BSP_DEF_ERR(BSP_MODU_TIMER,(BSP_ERR_SPECIAL + 1)) 
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Alloc
*
* 功能描述  : 申请用户时钟ID
*
* 输入参数  : BSP_S32 * ps32UsrClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_U32 BSP_USRCLK_Alloc(BSP_S32 * ps32UsrClkId);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Free
*
* 功能描述  : 释放用户时钟
*
* 输入参数  : BSP_S32 s32UsrClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_U32 BSP_USRCLK_Free(BSP_S32 s32UsrClkId);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Connect
*
* 功能描述  : This routine specifies the interrupt service routine to be called 
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* 输入参数  : FUNCPTR routine   routine to be called at each clock interrupt
              BSP_S32 arg	        argument with which to call routine
              BSP_S32 s32UsrClkid      which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Disable
*
* 功能描述  : This routine disables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Enable
*
* 功能描述  : This routine enables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_Enable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateGet
*
* 功能描述  : This routine returns the interrupt rate of the system aux clock.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : BSP_S32 * pu32ClkRate  clk rate
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32 BSP_USRCLK_RateGet (BSP_S32 s32UsrClkId, BSP_S32 * pu32ClkRate);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateSet
*
* 功能描述  : This routine sets the interrupt rate of the usr clock.
*
* 输入参数  : BSP_S32 ticksPerSecond   number of clock interrupts per second 
              BSP_S32 s32UsrClkid         which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStart
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
*             u32Cycles   Timer寄存器的计数值
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStart(BSP_S32 s32UsrClkid,BSP_U32 u32Cycles);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStop
*
* 功能描述  : 关闭单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : 无
* 返 回 值  : 无.
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStop(BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerValue
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid   Timer Id
* 输出参数  : Value      当前Timer value寄存器的计数值
* 返 回 值  : OK&错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_U32  BSP_USRCLK_TimerValue(BSP_S32 s32UsrClkId, BSP_U32 *pu32Value);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecStart
*
* 功能描述  : 以毫秒为单位开启单次Timer定时器.
*
* 输入参数  : s32UsrClkid        Timer Id
*             u32MilliSecond   设置定时器超时的毫秒值
* 输出参数  : 无
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_TimerMilliSecStart(BSP_S32 s32UsrClkid ,BSP_U32 u32MilliSecond);


/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecValue
*
* 功能描述  : 获得当前Timer的毫秒值.
*
* 输入参数  : s32UsrClkid        Timer Id
* 输出参数  : BSP_U32 * pu32Value 当前Timer value寄存器的毫秒值
* 返 回 值  : OK&其他错误码
*
* 修改记录  : 2009年2月24日   liumengcun  creat
*****************************************************************************/
extern BSP_U32 BSP_USRCLK_TimerMilliSecValue(BSP_S32 s32UsrClkId ,BSP_U32 * pu32Value);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_USRCLK_H__ */


