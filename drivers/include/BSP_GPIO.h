/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_GPIO.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_GPIO_H__
#define __BSP_GPIO_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* 中断触发类型枚举*/
typedef enum tagGPIO_INT_TRIG_E
{
    GPIO_HIGH_LEVEL          	=  1,/* 高电平触发*/
    GPIO_LOW_LEVEL           	=  2,/* 低电平触发*/
    GPIO_BOTH_EDGE           	=  3,/* 双沿触发*/
    GPIO_RISING_EDGE        	=  4,/* 上升沿触发*/
    GPIO_DECENDING_EDGE    	    =  5 /* 下降沿触发*/
} GPIO_INT_TRIG_E;

/* PV500上有2个GPIO */
typedef enum tagGPIO_GPIO_NUM_E
{
    GPIO_DEF_GPIO0          	=  0,
    GPIO_DEF_GPIO1           	=  1,
#if ((defined (CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)) \
    && (defined (BOARD_SFT)||defined(BOARD_ASIC)||defined(BOARD_ASIC_BIGPACK)))
	GPIO_DEF_GPIO2				,
	GPIO_DEF_GPIO3				,
	GPIO_DEF_GPIO4				,
	GPIO_DEF_GPIO5				,
    #elif defined (VERSION_V3R2)
	GPIO_DEF_GPIO2,
    GPIO_DEF_GPIO3,
    GPIO_DEF_GPIO4,
    GPIO_DEF_GPIO5,
    GPIO_DEF_GPIO6,
    GPIO_DEF_GPIO7,
    GPIO_DEF_GPIO8,
    GPIO_DEF_GPIO9,
    GPIO_DEF_GPIO10,
    GPIO_DEF_GPIO11,
	#endif
    GPIO_DEF_MAX  
} GPIO_GPIO_NUM_E;

/* 查询模式 */
typedef enum tagGPIO_GPIO_GETMODE_NUM_E
{
    LINE_STATE          	=  0,
    FORCE_LOAD_CTRL       =  1
} GPIO_GETMODE_NUM_E;


#ifdef BSP_CPE
typedef enum tagCPE_SIGNAL_INTENSITY_E
{ 
	SIGNALNULL,
    SIGNALLOW,
    SIGNALMIDDLE,
    SIGNALHIGH,
    SIGNALMAX
}CPE_SIGNAL_INTENSITY_E;

typedef enum tagCPE_SIGNAL_STATUS_E
{
     SIGNAL_STATUS_ABNORMAL,    //信号异常
     SIGNAL_STATUS_NORMAL,      //信号正常
     SIGNAL_STATUS_MAX
}CPE_SIGNAL_STATUS_E;

typedef enum tagGPIO_CTRL_E
{
    GPIO_ON,
    GPIO_OFF,
    GPIO_MAX
}GPIO_CTRL_E;

#endif

/*****************************************************************************
* 函 数 名  : BSP_GPIO_Init
*
* 功能描述  : GPIO初始化接口，挂接中断服务程序
*
* 输入参数  : BSP_VOID  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_VOID  BSP_GPIO_Init (BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_DrcSet
*
* 功能描述  : 设置GPIO引脚的方向
*
* 输入参数  : BSP_U32 u32Mask        指定待设置的GPIO位
*             BSP_U32 u32Directions  待设置的方向值,0：输入；1：输出
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32  BSP_GPIO_DrcSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_U32 u32Directions);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_DrcGet
*
* 功能描述  : 查询GPIO 引脚的方向
*
* 输入参数  : 无
* 输出参数  : BSP_U32 * pu32Directions  保存返回的方向值
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_DrcGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 * pu32Directions);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_ValSet
*
* 功能描述  : 设置GPIO 引脚的电平值
*
* 输入参数  : BSP_U32 u32Mask  指定待设置的GPIO位
*             BSP_U32 u32Data  待设置的电平值
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_ValSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_U32 u32Data);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_ValGet
*
* 功能描述  : 查询GPIO引脚的电平值
*
* 输入参数  : 无
* 输出参数  : BSP_U32 * pu32Data  保存返回的电平值
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_ValGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 * pu32Data);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntMaskSet
*
* 功能描述  : 屏蔽中断位
*
* 输入参数  : BSP_U32 u32Mask  指定屏蔽中断的GPIO位
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntMaskSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);  

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntUnMaskSet
*
* 功能描述  : 使能中断位
*
* 输入参数  : BSP_U32 u32Mask  指定使能中断的GPIO位
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntUnMaskSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntStateGet
*
* 功能描述  : 查询中断状态位
*
* 输入参数  : BSP_U32 *pu32Stat  
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntStateGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 *pu32Stat);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_RawIntStateGet
*
* 功能描述  : 查询原始中断状态位
*
* 输入参数  : 无
* 输出参数  : BSP_U32 *pu32RawStat  保存返回的原始中断状态
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_RawIntStateGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 *pu32RawStat);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntStateClear
*
* 功能描述  : 清除中断状态位，置1表示对应中断位清除中断
*
* 输入参数  : BSP_U32 u32Mask  指定待清除的GPIO位
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntStateClear(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntTriggerSet
*
* 功能描述  : 设置中断触发方式，上升沿、下降沿、双边沿、高电平、低电平
*
* 输入参数  : BSP_U32 u32Mask             指定待设置的GPIO位
*             GPIO_INT_TRIG enTrigMode  中断触发模式
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntTriggerSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, GPIO_INT_TRIG_E enTrigMode);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntInstall
*
* 功能描述  : 其他驱动模块注册GPIO中断处理程序的接口
*
* 输入参数  : BSP_U32 u32Mask  指定GPIO位ID
*             BSP_VOID* routine  待注册的回调函数指针
*             BSP_S32 para       回调函数参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntInstall(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_VOID* routine,BSP_S32 para);

/*****************************************************************************
* 函 数 名  : BSP_GPIO_IntUnInstall
*
* 功能描述  : 其他驱动模块卸载GPIO中断处理程序的接口
*
* 输入参数  : BSP_U32 u32Mask  指定GPIO位ID
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntUnInstall (GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_GPIO_H__ */


