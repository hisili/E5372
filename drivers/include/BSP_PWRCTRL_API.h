/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_PWRCTRL_API.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_PWRCTRL_API_H__
#define __BSP_PWRCTRL_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */





BSP_VOID BSP_PWRCTRL_SoftClkEn(PWRCTRL_MODULE_E enModu);
BSP_VOID BSP_PWRCTRL_SoftClkDis(PWRCTRL_MODULE_E enModu);
BSP_U32 BSP_PWRCTRL_SoftClkStatus(PWRCTRL_MODULE_E enModu);

/*PS注册函数类型*/
typedef BSP_U8   (*BSPRegDRXFunc)(BSP_VOID);

typedef enum tagDRX_SLEEP_MODE_E
{
    DRX_WEEK_SLEEP_MODE_E = 0x0,
    DRX_DEEP_SLEEP_MODE_E = 0x1,
    DRX_WEEK2_SLEEP_MODE_E = 0x2,
	DRX_NO_SLEEP_MODE_E = 0x3
}DRX_SLEEP_MODE_E;


typedef enum tagDRX_TIMER_ID_E
{
    DRX_LTESYS_TIMER = 0x0,
    DRX_LTET3412_TIMER,    
    DRX_LTEVPLMN_TIMER,    
    DRX_LTETSRHC_TIMER,    
    DRX_LTETA_TIMER,    
    DRX_DEEP_SLEEP_TIMER,    
    DRX_LTE_PS32K_TIMER,
    DRX_LTE_PS32K_BASETIMER,
    DRX_LTE_NO_TIMER
}DRX_TIMER_ID_E;

typedef enum tagDRX_TIMER_MODE_E
{
    DRX_TIMER_FREE_MODE = 0x0,
    DRX_TIMER_USER_MODE
}DRX_TIMER_MODE_E;

typedef enum tagDRX_REG_CB_E
{
    DRX_PS_GET_SLEEP_INFO = 0x0,
    DRX_RTC_TIME_START = 0x1,
	DRX_RTC_TIME_END = 0x2
}DRX_REG_CB_E;

/*****************************************************************************
* 函 数 名  : set32KHwTimerMode
*
* 功能描述  : 查询32K硬件时钟计数器值
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 set32KHwTimerMode(DRX_TIMER_MODE_E eMode);
#if 0
/*****************************************************************************
* 函 数 名  : get32KHwTimerMode
*
* 功能描述  : 查询32K硬件时钟计数器的工作模式
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
DRX_TIMER_MODE_E get32KHwTimerMode();
#endif
/*****************************************************************************
* 函 数 名  : BSP_DRX_RegCb
*
* 功能描述  : PS注册函数
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_VOID BSP_DRX_RegCb (DRX_REG_CB_E enType, BSPRegDRXFunc pFunc);
/*****************************************************************************
* 函 数 名  : usrTimer4Start
*
* 功能描述  : 启动32K硬件时钟定时器
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 usrTimer4Start(DRX_TIMER_ID_E eTimerId, BSP_U32 u32Count);
/*****************************************************************************
* 函 数 名  : usrTimer4Stop
*
* 功能描述  : 停止32K硬件时钟定时器
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 usrTimer4Stop(DRX_TIMER_ID_E eTimerId);
/*****************************************************************************
* 函 数 名  : usrTimer4Value
*
* 功能描述  : 查询32K BBP 时钟定时器剩余值
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 usrTimer4Value(DRX_TIMER_ID_E eTimerId);

typedef enum tagPWC_SLEEP_TYPE_E
{
	PWC_ARM_SLEEP = 0,
	PWC_LIGHT_SLEEP,
	PWC_DEEP_SLEEP,
	PWC_NOWAKE_SRC,
	PWC_TYPE_BUTTON
}PWC_SLEEP_TYPE_E;

BSP_VOID BSP_PWRCTRL_ClkGateEn();
BSP_VOID BSP_PWRCTRL_ClkGateDis();
BSP_VOID BSP_PWRCTRL_SetWakeSrc(PWC_SLEEP_TYPE_E enSleepType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_PWRCTRL_API_H__ */


