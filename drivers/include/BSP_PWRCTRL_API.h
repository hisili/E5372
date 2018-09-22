/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_PWRCTRL_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
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

/*PSע�ắ������*/
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
* �� �� ��  : set32KHwTimerMode
*
* ��������  : ��ѯ32KӲ��ʱ�Ӽ�����ֵ
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 set32KHwTimerMode(DRX_TIMER_MODE_E eMode);
#if 0
/*****************************************************************************
* �� �� ��  : get32KHwTimerMode
*
* ��������  : ��ѯ32KӲ��ʱ�Ӽ������Ĺ���ģʽ
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
DRX_TIMER_MODE_E get32KHwTimerMode();
#endif
/*****************************************************************************
* �� �� ��  : BSP_DRX_RegCb
*
* ��������  : PSע�ắ��
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_VOID BSP_DRX_RegCb (DRX_REG_CB_E enType, BSPRegDRXFunc pFunc);
/*****************************************************************************
* �� �� ��  : usrTimer4Start
*
* ��������  : ����32KӲ��ʱ�Ӷ�ʱ��
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 usrTimer4Start(DRX_TIMER_ID_E eTimerId, BSP_U32 u32Count);
/*****************************************************************************
* �� �� ��  : usrTimer4Stop
*
* ��������  : ֹͣ32KӲ��ʱ�Ӷ�ʱ��
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 usrTimer4Stop(DRX_TIMER_ID_E eTimerId);
/*****************************************************************************
* �� �� ��  : usrTimer4Value
*
* ��������  : ��ѯ32K BBP ʱ�Ӷ�ʱ��ʣ��ֵ
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

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


