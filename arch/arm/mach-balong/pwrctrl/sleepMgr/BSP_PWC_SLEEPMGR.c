/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepMgr.c
* Description:
*                sleep managerment
*
* Author:        刘永富
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		刘永富
* Date:			2011-09-20
* Description:	Initial version
*
*******************************************************************************/
/**********************问题单修改记录************************************************
日    期              修改人                     问题单号                   修改内容
********************************************************************************************/
/*V3R2 CS f00164371*/
#include "generated/FeatureConfigDRV.h"

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/platform_device.h>

#include <mach/balong_v100r001.h>

#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#include <mach/pwrctrl/BSP_PWC_COMMON.h>
#include <mach/pwrctrl/BSP_PWC_SLEEP.h>
#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h>
#include "BSP.h"
#include <mach/common/bsp_memory.h>
#include <linux/netlink.h>
#include <linux/BSP_CHGC_DRV.h>
#include "../drivers/rtc/balong_rtc.h"
#include "../drivers/input/keyboard/balong_keyboard/balong_kpd_parse.h"
#include "../drivers/led_drv/balong_led_drv.h"

extern int PWRCTRL_IntLock(void);
extern void PWRCTRL_IntUnlock(int lvl);

#if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
extern int32_t hw_dwc_otg_is_device_mode(void);
extern void dwc_otg_discharge_open();
extern void dwc_otg_discharge_close();
#endif

//extern BSP_U32 PS_BSP_AppGetSleepInfo();
BSP_PWC_ENTRY g_stPwcMspVote = {0};

BSP_PWC_ENTRY g_stPwcSocpVote = {0};

BSP_PWC_ENTRY g_stPwcSocpRestore = {0};

PWC_DS_SOCP_CB_STRU g_stFuncAcpuSocpCb = {NULL};

unsigned int g_ulAcpuBootJiffies = 0;

unsigned int g_ulAcupIdleTime = 0;
unsigned int g_ulAcpuInitFlag = FALSE;
/*启动过程禁止睡眠*/
unsigned int g_ulAcpuSleepVoteMap = (unsigned int)(1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK));

const unsigned int g_aTimerBaseAddr[PWRCTRL_TIMER_MAX]={ 
	PWRCTRL_SOC_ADDR_TIMER0_0, PWRCTRL_SOC_ADDR_TIMER0_1, PWRCTRL_SOC_ADDR_TIMER0_2, PWRCTRL_SOC_ADDR_TIMER0_3, 
	PWRCTRL_SOC_ADDR_TIMER0_4, PWRCTRL_SOC_ADDR_TIMER0_5, PWRCTRL_SOC_ADDR_TIMER0_6, PWRCTRL_SOC_ADDR_TIMER0_7, 
	PWRCTRL_SOC_ADDR_TIMER1_0, PWRCTRL_SOC_ADDR_TIMER1_1, PWRCTRL_SOC_ADDR_TIMER1_2, PWRCTRL_SOC_ADDR_TIMER1_3, 
	PWRCTRL_SOC_ADDR_TIMER1_4, PWRCTRL_SOC_ADDR_TIMER1_5, PWRCTRL_SOC_ADDR_TIMER1_6, PWRCTRL_SOC_ADDR_TIMER1_7, 
	PWRCTRL_SOC_ADDR_TIMER2_0, PWRCTRL_SOC_ADDR_TIMER2_1, PWRCTRL_SOC_ADDR_TIMER2_2, PWRCTRL_SOC_ADDR_TIMER2_3, 
	PWRCTRL_SOC_ADDR_TIMER2_4, PWRCTRL_SOC_ADDR_TIMER2_5, PWRCTRL_SOC_ADDR_TIMER2_6, PWRCTRL_SOC_ADDR_TIMER2_7 
                                      };
const unsigned int g_aTimerInputClk[PWRCTRL_TIMER_MAX]={ 

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))  \
    ||(defined(BOARD_SFT) && defined(VERSION_V7R1))
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, 
	PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_SLEEP,  PWRCTRL_CLK_TCXO, PWRCTRL_CLK_SLEEP,  PWRCTRL_CLK_TCXO  
#elif (defined (BOARD_SFT) && defined (VERSION_V3R2))
	PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, 
	PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO  
#elif (defined (BOARD_FPGA) && defined (VERSION_V3R2)) ||\
    (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
	PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, 
	PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO  
#elif ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
      && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, 
	PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_SLEEP, PWRCTRL_CLK_SLEEP,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  PWRCTRL_CLK_TCXO,  
	PWRCTRL_CLK_SLEEP,  PWRCTRL_CLK_TCXO, PWRCTRL_CLK_SLEEP,  PWRCTRL_CLK_TCXO  
#endif
   };

/*快速开关机用全局变量*/
#if defined(FEATURE_SHORT_ON_OFF)
/*快速开关机配置信息,由按键模块初始化时从NV获取默认值*/
extern NV_SHORT_POWER_ON_OFF_TYPE s_shortOnOffConfig;  

/*快速开关机当前运行情况统计，含假关机次数，系统历史运行时间，当前是假关机还是假开机状态*/
CURRENT_SHORT_POWER_ON_OFF_STATUS s_currentShortOnOffStatus = {0};
SHORT_POWER_OFF_MODE_PARA g_short_power_off_status = SHORT_POWER_OFF_MODE_CLR;

#endif

/*****************************************************************************
Prototype      : BSP_PWRCTRL_SleepInSocCB
Description    :
Input          : NU_NULL
Output         : NU_NULL
Return Value   : PWC_SUCCESS /
Calls          :
Called By      :
*****************************************************************************/
int BSP_PWRCTRL_SleepInSocCB(PWC_DS_SOCP_CB_STRU stFunc)
{
    if ((NULL == stFunc.pFuncDsIn) || (NULL == stFunc.pFuncDsIn))
    {
        printk("BSP_PWRCTRL_SleepInSocCB regist fail. \n");
        return ERROR;
    }

    g_stFuncAcpuSocpCb = stFunc;

    printk("BSP_PWRCTRL_SleepInSocCB regist Success. \n");

    return OK;
}

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
unsigned int g_ulDeepSleepVoteMask = PWRCTRL_DEEPSLEEP_VOTE_MASK;
unsigned int g_ulLightSleepVoteMask = PWRCTRL_LIGHTSLEEP_VOTE_MASK;
unsigned int g_ulLightSleepTimeThreshold = PWRCTRL_LIGHTSLEEP_TIME_THRESHOLD;
unsigned int g_ulDeepSleepTimeThreshold = PWRCTRL_DEEPSLEEP_TIME_THRESHOLD;
unsigned int g_ulAlwaysOnTimerMask = PWRCTRL_ALWAYSON_TIMER;

unsigned int g_ulSleepMgrStatisticsFlag = 0;
PWC_SLEEPMGR_STATISTICS_STRU g_stSleepMgrStatisticsInfo;

#define PWRCTRL_DeepSleepInTick(ulIndex) (g_stSleepMgrStatisticsInfo.astDeepSleepRecord[ulIndex].stSleepInTime.ulTickValue)
#define PWRCTRL_DeepSleepInTimer(ulIndex) (g_stSleepMgrStatisticsInfo.astDeepSleepRecord[ulIndex].stSleepInTime.ulTimerValue)
#define PWRCTRL_DeepSleepOutTick(ulIndex) (g_stSleepMgrStatisticsInfo.astDeepSleepRecord[ulIndex].stSleepOutTime.ulTickValue)
#define PWRCTRL_DeepSleepOutTimer(ulIndex) (g_stSleepMgrStatisticsInfo.astDeepSleepRecord[ulIndex].stSleepOutTime.ulTimerValue)

#define PWRCTRL_LightSleepInTick(ulIndex) (g_stSleepMgrStatisticsInfo.astLightSleepRecord[ulIndex].stSleepInTime.ulTickValue)
#define PWRCTRL_LightSleepInTimer(ulIndex) (g_stSleepMgrStatisticsInfo.astLightSleepRecord[ulIndex].stSleepInTime.ulTimerValue)
#define PWRCTRL_LightSleepOutTick(ulIndex) (g_stSleepMgrStatisticsInfo.astLightSleepRecord[ulIndex].stSleepOutTime.ulTickValue)
#define PWRCTRL_LightSleepOutTimer(ulIndex) (g_stSleepMgrStatisticsInfo.astLightSleepRecord[ulIndex].stSleepOutTime.ulTimerValue)

#define PWRCTRL_ArmSleepInTick(ulIndex) (g_stSleepMgrStatisticsInfo.astArmSleepRecord[ulIndex].stSleepInTime.ulTickValue)
#define PWRCTRL_ArmSleepInTimer(ulIndex) (g_stSleepMgrStatisticsInfo.astArmSleepRecord[ulIndex].stSleepInTime.ulTimerValue)
#define PWRCTRL_ArmSleepOutTick(ulIndex) (g_stSleepMgrStatisticsInfo.astArmSleepRecord[ulIndex].stSleepOutTime.ulTickValue)
#define PWRCTRL_ArmSleepOutTimer(ulIndex) (g_stSleepMgrStatisticsInfo.astArmSleepRecord[ulIndex].stSleepOutTime.ulTimerValue)

#define logMsg printk
#endif

/********************************************************
*函数名		:
*函数功能	:
*输入参数	:
*输出参数	:
*返回值		:
*
*修改历史	:
*	日期	:
*	作者	:
*	修改内容:
********************************************************/
void PWRCTRL_TimerDisable(unsigned int* pState)
{
	unsigned int i=0;
    unsigned int ulTimerCtrl=0;

    for(i=0;i<PWRCTRL_TIMER_MAX;i++)
    {
        PWRCTRL_REG_READ32(PWRCTRL_TIMER_CTRL_ADDR(g_aTimerBaseAddr[i]), ulTimerCtrl);
        if((0 != (ulTimerCtrl & PWRCTRL_TIMER_EN_MSK)) \
            && (0 == ((1<<i) & PWRCTRL_ALWAYSON_TIMER)) \
            && (0 != ((1<<i) & PWRCTRL_ACPU_TIMER)) )
        {
            /*record timer status which is on before deepsleep*/
            pState[i] = PWRCTRL_TRUE;
            /*stop timer*/
            ulTimerCtrl &= (~(PWRCTRL_TIMER_EN_MSK));
            PWRCTRL_REG_WRITE32(PWRCTRL_TIMER_CTRL_ADDR(g_aTimerBaseAddr[i]), ulTimerCtrl);
            
        }
    }
}

/********************************************************
*函数名		:
*函数功能	:
*输入参数	:
*输出参数	:
*返回值		:
*
*修改历史	:
*	日期	:
*	作者	:
*	修改内容:
********************************************************/
void PWRCTRL_TimerEnable(unsigned int* pState)
{
	unsigned int i=0;
    unsigned int ulTimerCtrl=0;

    for(i=0;i<PWRCTRL_TIMER_MAX;i++)
    {
        if(PWRCTRL_TRUE == (pState[i]) \
            && (0 == ((1<<i) & PWRCTRL_ALWAYSON_TIMER)) \
            && (0 != ((1<<i) & PWRCTRL_ACPU_TIMER)) )
        {
            /*record timer status which is on before deepsleep*/
            pState[i] = PWRCTRL_FALSE;
            /*stop timer*/
            PWRCTRL_REG_READ32(PWRCTRL_TIMER_CTRL_ADDR(g_aTimerBaseAddr[i]), ulTimerCtrl);
            ulTimerCtrl |= (PWRCTRL_TIMER_EN_MSK);
            PWRCTRL_REG_WRITE32(PWRCTRL_TIMER_CTRL_ADDR(g_aTimerBaseAddr[i]), ulTimerCtrl);
            
        }
    }

}


/********************************************************
*函数名		:
*函数功能	:
*输入参数	:
*输出参数	:
*返回值		:
*
*修改历史	:
*	日期	:
*	作者	:
*	修改内容:
********************************************************/
unsigned int BSP_PWRCTRL_SleepVoteLock(PWC_CLIENT_ID_E  enClientId)
{
    int locklvl;
	if((enClientId < PWRCTRL_SLEEP_SCI) ||(enClientId >= PWRCTRL_CLIENT_BUTT))
    {
        printk("PWRCTRL: id is not exist! \n");
        return PWRCTRL_PARA_INVALID;
    }

    locklvl = PWRCTRL_IntLock();
    PWRCTRL_ACPU_SLEEP_LOCK(enClientId); 
    PWRCTRL_IntUnlock(locklvl);
    
    return PWRCTRL_SUCCESS;

}

/********************************************************
*函数名		:
*函数功能	:
*输入参数	:
*输出参数	:
*返回值		:
*
*修改历史	:
*	日期	:
*	作者	:
*	修改内容:
********************************************************/
unsigned int BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E  enClientId)
{
    int locklvl;

	if((enClientId < PWRCTRL_SLEEP_SCI) ||(enClientId >= PWRCTRL_CLIENT_BUTT))
    {
        printk("PWRCTRL: id is not exist! \n");
        return PWRCTRL_PARA_INVALID;
    }

    locklvl = PWRCTRL_IntLock();
    PWRCTRL_ACPU_SLEEP_UNLOCK(enClientId); 
    PWRCTRL_IntUnlock(locklvl);
    
    return PWRCTRL_SUCCESS;
}

/*****************************************************************************
 函 数 名  : DRV_SMS_AWAKE_OLED_ANTISLEEP
 功能描述  : A核收到短信唤醒OLED时反对系统休眠的API接口，供短信模块调用
 输入参数  : None
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
BSP_U32 DRV_SMS_AWAKE_OLED_ANTISLEEP(void)
{
    return BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
}  

/********************************************************
*函数名		:
*函数功能	:
*输入参数	:
*输出参数	:
*返回值		:
*
*修改历史	:
*	日期	:
*	作者	:
*	修改内容:
********************************************************/
int PWRCTRL_TimerStatus(unsigned int timerAddr)
{
	unsigned int ulTimerVal = 0;
	unsigned int ulTimerCtrl = 0;

	PWRCTRL_REG_READ32(PWRCTRL_TIMER_CURVAL_ADDR(timerAddr), ulTimerVal);
	PWRCTRL_REG_READ32(PWRCTRL_TIMER_CTRL_ADDR(timerAddr), ulTimerCtrl);

	if((0 != (ulTimerCtrl & PWRCTRL_TIMER_INT_MSK)) \
		|| (0 == (ulTimerCtrl & PWRCTRL_TIMER_EN_MSK)) || (0 == ulTimerVal))
	{
		return PWRCTRL_FALSE;
	}

	return PWRCTRL_TRUE;
}
/********************************************************
*函数名		:
*函数功能	:
*输入参数	:
*输出参数	:
*返回值		:
*
*修改历史	:
*	日期	:
*	作者	:
*	修改内容:
********************************************************/
void PWRCTRL_TimerCheck(unsigned int* pTimerId, unsigned int* pTimerNxtVal)
{
	int i;

	unsigned int state = 0;
	unsigned int tmpTimerVal = 0;
	unsigned int TimerId = PWRCTRL_TIMER_MAX;
#if ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
              && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
	unsigned int TimerNxtVal = 0xffffffff;
#else
    unsigned int TimerNxtVal = 0;
#endif
	unsigned int ulclk, ulcount;
	
	for(i=0;i<PWRCTRL_TIMER_MAX;i++)
	{
		tmpTimerVal = 0;
		state = PWRCTRL_TimerStatus(g_aTimerBaseAddr[i]);

#if ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
          && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
        if ((0 == ((1 << i) & g_ulAlwaysOnTimerMask)) 
            || (state == PWRCTRL_FALSE))
        {
            continue;
        }
#else
		if((0 == (((1<<i) & PWRCTRL_ALWAYSON_TIMER) & PWRCTRL_ACPU_TIMER)) \
			|| (state == PWRCTRL_FALSE))
		{
			continue;
		}
#endif

		/**/
		ulclk = g_aTimerInputClk[i];
		PWRCTRL_REG_READ32(PWRCTRL_TIMER_CURVAL_ADDR(g_aTimerBaseAddr[i]), ulcount); 

		/*calculate time left, ms*/
		tmpTimerVal = ulcount / (ulclk/PWRCTRL_TIMER_MS_COV);

#if ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
                      && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
        if (TimerNxtVal > tmpTimerVal)
        {
            TimerNxtVal = tmpTimerVal;
            TimerId = i;
        }
#else
        /*just for debug*/
        if(0 == (tmpTimerVal % 1000))
        {
            //printk("PWRCTRL: ms %d, cnt %d, clk %d \n",tmpTimerVal, ulcount, ulclk);
        }


		if(TimerNxtVal < tmpTimerVal)
		{
			TimerNxtVal = tmpTimerVal;
			TimerId = i;
		}
#endif        
	}

	*pTimerId = TimerId;
	*pTimerNxtVal = TimerNxtVal;

}

void PWRCTRL_SleepMgrInitial(void)
{
    g_ulAcpuBootJiffies = jiffies/HZ;  /*单位为 s*/
    
#if ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
  && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
    g_ulDeepSleepVoteMask = PWRCTRL_DEEPSLEEP_VOTE_MASK;
    g_ulLightSleepVoteMask = PWRCTRL_LIGHTSLEEP_VOTE_MASK;
    g_ulLightSleepTimeThreshold = PWRCTRL_LIGHTSLEEP_TIME_THRESHOLD;
    g_ulDeepSleepTimeThreshold = PWRCTRL_DEEPSLEEP_TIME_THRESHOLD;
    g_ulAlwaysOnTimerMask = PWRCTRL_ALWAYSON_TIMER;

    g_ulSleepMgrStatisticsFlag = g_stPwrctrlSwitch.mntn;
    memset(&g_stSleepMgrStatisticsInfo, 0x0, sizeof(PWC_SLEEPMGR_STATISTICS_STRU));

    if (1 == g_ulSleepMgrStatisticsFlag)
    {
        g_stSleepMgrStatisticsInfo.stRunBeginTime.ulTickValue = jiffies;
        g_stSleepMgrStatisticsInfo.stRunBeginTime.ulTimerValue = PWRCTRL_GetSleepSlice();
    }

#else
    BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_BOOT);
#endif

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
    /*V7 MPW芯片只进wfi状态*/
    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_DMA);
#endif
	g_ulAcpuInitFlag = TRUE;
}

unsigned int PWRCTRL_GetTimerCount(unsigned int ulTimerAddr )
{
  unsigned int ulTimerValue;
  PWRCTRL_REG_READ32(PWRCTRL_TIMER_CURVAL_ADDR(ulTimerAddr), ulTimerValue);
  return ulTimerValue;

}

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))


/*****************************************************************************
 Prototype      : BSP_PWC_MspVoteRegister
 Description    : MSP注册投票接口
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By      :
*****************************************************************************/
void BSP_PWC_MspVoteRegister(FUNCPTR routine)
{
	g_stPwcMspVote.routine = routine;
}


/*****************************************************************************
 Prototype      : BSP_PWC_SocpVoteRegister
 Description    : Get sleep timer count.
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By      :
*****************************************************************************/
void BSP_PWC_SocpVoteRegister(FUNCPTR routine)
{
	g_stPwcSocpVote.routine = routine;
}

/*****************************************************************************
 Prototype      : BSP_PWC_SocpRestoreRegister
 Description    : //.
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By      :
*****************************************************************************/
void BSP_PWC_SocpRestoreRegister(FUNCPTR routine)
{
	g_stPwcSocpRestore.routine = routine;
}

    
/*****************************************************************************
 Prototype      : PWRCTRL_GetSleepSlice
 Description    : Get sleep timer count.
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By      :
*****************************************************************************/
unsigned int PWRCTRL_GetSleepSlice(void)
{
    return PWRCTRL_GetTimerCount(g_aTimerBaseAddr[PWRCTRL_SLICE_TIMER]);
}

/*****************************************************************************
Prototype      : PWRCTRL_CheckSleepVoteState
Description    :
Input          : NULL
Output         : NULL
Return Value   : NULL
Calls          :
Called By      :
*****************************************************************************/
void PWRCTRL_CheckSleepVoteState(void)
{

    /*MSP投票情况判断*/
	if (NULL != g_stPwcMspVote.routine)
	{
		if (TRUE != g_stPwcMspVote.routine())
		{
			BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_OAM);
	    }
	    else
	    {
	        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_OAM);
	    }	
	}
	
    /*SOCP投票情况判断*/
	if (NULL != g_stPwcSocpVote.routine)
	{
		if (BSP_OK != g_stPwcSocpVote.routine())
		{
			BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_SOCP);
	    }
	    else
	    {
	        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SOCP);
	    }	
	}

	/*ICC投票情况判断*/
    if (BSP_OK != BSP_ICC_CanSleep(0))
    {
        BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_ICC);
    }
    else
    {
        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_ICC);
    }
	if(BSP_OK != BSP_IFC_CanSleep())
	{
		BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_IFC);
    }
    else
    {
        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_IFC);
    }	
	
#if 0
	if (TRUE != PS_BSP_AppGetSleepInfo())
    {
        BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_PS);
    }
    else
    {
        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_PS);
    }
#endif
}

extern unsigned int g_ulAcpuStandbyFlag;

void arch_idle(void)
{
#if ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) && !defined(PRODUCT_CFG_BUILD_SEPARATE) \
          && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
              
	unsigned int timerId,timerNextSchVal;
    unsigned int OldDSSlice =0;
    unsigned int NewDSSlice =0;
    unsigned int OldWFISlice = 0;
     unsigned int NewWFISlice =0;
    unsigned int currValue;
    unsigned int ulRecordPosition = 0;

    /**/
	if(FALSE == g_ulAcpuInitFlag)
	{
		return;
	}
    currValue = jiffies/HZ;  /*单位为 s*/
    
    if((currValue < (g_ulAcpuBootJiffies + 20)) && (currValue >= g_ulAcpuBootJiffies))
    {
        if(0 == (jiffies % 500))
        {
            /*printk(KERN_INFO "\nPWRCTRL: +++++++++++++++++++++ %d, %d, %d \n", \
           		 g_ulAcpuBootJiffies,(volatile unsigned int)jiffies,currValue);*/
        }
        return;
    }
    BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_BOOT);
    /*部分查询方式投票组件睡眠投票情况判断*/
    PWRCTRL_CheckSleepVoteState();

    if (1 == g_ulSleepMgrStatisticsFlag)
    {
        g_stSleepMgrStatisticsInfo.ulSleepTotalCount++;
    }

	/*check timer, get closest time number*/
	PWRCTRL_TimerCheck(&timerId, &timerNextSchVal);

    if (1 == g_ulSleepMgrStatisticsFlag)
    {
        g_stSleepMgrStatisticsInfo.ulLastMinTimerId = timerId;
        g_stSleepMgrStatisticsInfo.ulLastMinTimeMs = timerNextSchVal;
        g_stSleepMgrStatisticsInfo.ulLastVoteMap = g_ulAcpuSleepVoteMap;
    }

    if (1 == g_ulAcpuStandbyFlag)
    {
#if defined (FEATURE_LCD_ON)
        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_LCD);
#endif
    }
    
	/*check votemap and enter special branch*/
	if((0 == (g_ulAcpuSleepVoteMap & PWRCTRL_DEEPSLEEP_VOTE_MASK))
		 && ((timerNextSchVal) > PWRCTRL_DEEPSLEEP_TIME_THRESHOLD)
#ifdef __PWRCTRL_VERSION_CONTROL__
		 && (PWRCTRL_SWITCH_ON == g_stPwrctrlSwitch.deepsleep)
#endif		 
		 )		
	{
            if (1 == g_ulSleepMgrStatisticsFlag)
            {
                g_stSleepMgrStatisticsInfo.ulDeepSleepCount++;
                ulRecordPosition = ((g_stSleepMgrStatisticsInfo.ulDeepSleepCount-1)%PWRCTRL_SLEEP_RECORD_NUM);
                PWRCTRL_DeepSleepInTick(ulRecordPosition) = jiffies;
                PWRCTRL_DeepSleepInTimer(ulRecordPosition) = PWRCTRL_GetSleepSlice();
            }
             OldDSSlice = PWRCTRL_GetSleepSlice();
		if(g_stPwrctrlSwitch.drxPeriPd && (0 == (g_ulAcpuSleepVoteMap & PWRCTRL_PERI_POWERDOWN_VOTE_MASK)))
		{
        	PWRCTRL_DeepSleep(1);
		}
		else
		{
			PWRCTRL_DeepSleep(0);
		}
             NewDSSlice = PWRCTRL_GetSleepSlice();
             g_ulAcupIdleTime  += (OldDSSlice - NewDSSlice);		
            if (1 == g_ulSleepMgrStatisticsFlag)
            {
                PWRCTRL_DeepSleepOutTick(ulRecordPosition) = jiffies;
                PWRCTRL_DeepSleepOutTimer(ulRecordPosition) = PWRCTRL_GetSleepSlice();
                g_stSleepMgrStatisticsInfo.stDeepSleepTime.ulTickValue += 
                    (PWRCTRL_DeepSleepOutTick(ulRecordPosition)-PWRCTRL_DeepSleepInTick(ulRecordPosition));
                g_stSleepMgrStatisticsInfo.stDeepSleepTime.ulTimerValue +=
                    (PWRCTRL_DeepSleepInTimer(ulRecordPosition)-PWRCTRL_DeepSleepOutTimer(ulRecordPosition));
            }

	}
	else		/*wfi*/
	{
        if (1 == g_ulSleepMgrStatisticsFlag)
        {
            g_stSleepMgrStatisticsInfo.ulArmSleepCount++;
            ulRecordPosition = ((g_stSleepMgrStatisticsInfo.ulArmSleepCount-1)%PWRCTRL_SLEEP_RECORD_NUM);
            PWRCTRL_ArmSleepInTick(ulRecordPosition) = jiffies;
            PWRCTRL_ArmSleepInTimer(ulRecordPosition) = PWRCTRL_GetSleepSlice();
        }

        //OldSlice = PWRCTRL_ArmSleepInTimer(ulRecordPosition);
        OldWFISlice = PWRCTRL_GetSleepSlice();

        PWRCTRL_ArmSleep();

        if (1 == g_ulSleepMgrStatisticsFlag)
        {
            PWRCTRL_ArmSleepOutTick(ulRecordPosition) = jiffies;
            PWRCTRL_ArmSleepOutTimer(ulRecordPosition) = PWRCTRL_GetSleepSlice();
            g_stSleepMgrStatisticsInfo.stArmSleepTime.ulTickValue +=
                (PWRCTRL_ArmSleepOutTick(ulRecordPosition)-PWRCTRL_ArmSleepInTick(ulRecordPosition));
            g_stSleepMgrStatisticsInfo.stArmSleepTime.ulTimerValue +=
                (PWRCTRL_ArmSleepInTimer(ulRecordPosition)-PWRCTRL_ArmSleepOutTimer(ulRecordPosition));
        }

        //NewSlice = PWRCTRL_ArmSleepOutTimer(ulRecordPosition);
        NewWFISlice = PWRCTRL_GetSleepSlice();
        g_ulAcupIdleTime  += (OldWFISlice - NewWFISlice);
	}

    if (1 == g_ulSleepMgrStatisticsFlag)
    {
        g_stSleepMgrStatisticsInfo.stSleepTotalTime.ulTickValue = 
            (g_stSleepMgrStatisticsInfo.stArmSleepTime.ulTickValue
            + g_stSleepMgrStatisticsInfo.stLightSleepTime.ulTickValue
            + g_stSleepMgrStatisticsInfo.stDeepSleepTime.ulTickValue);

        g_stSleepMgrStatisticsInfo.stSleepTotalTime.ulTimerValue = 
            (g_stSleepMgrStatisticsInfo.stArmSleepTime.ulTimerValue
            + g_stSleepMgrStatisticsInfo.stLightSleepTime.ulTimerValue
            + g_stSleepMgrStatisticsInfo.stDeepSleepTime.ulTimerValue);
    }
#endif  
    return;
}

#else

void arch_idle(void)
{
	unsigned int timerId,timerNextSchVal;
	unsigned int aNotAlwaysOnTimerState[PWRCTRL_TIMER_MAX]={PWRCTRL_FALSE};
    unsigned int OldSlice =0;
    unsigned int NewSlice =0;
    unsigned int currValue;
    /**/
    currValue = jiffies/HZ;  /*单位为 s*/
    
    if((currValue < (g_ulAcpuBootJiffies + 20)) && (currValue >= g_ulAcpuBootJiffies))
    {
        if(0 == (jiffies % 500))
        {
            /*printk(KERN_INFO "\nPWRCTRL: +++++++++++++++++++++ %d, %d, %d \n",\
            		g_ulAcpuBootJiffies,(volatile unsigned int)jiffies,currValue);*/
        }
        return;
    }

	/*feed watchdog*/


	/*check timer, get closest time number*/
	PWRCTRL_TimerCheck(&timerId, &timerNextSchVal);
    if(timerId == PWRCTRL_TIMER_MAX)
    {
        timerNextSchVal = PWRCTRL_DEEPSLEEP_TIME_THRESHOLD + PWRCTRL_LIGHTSLEEP_TIME_THRESHOLD;
    }

	/*check votemap and enter special branch*/
	if((0 == (g_ulAcpuSleepVoteMap & PWRCTRL_DEEPSLEEP_VOTE_MASK)) \
		 && ((timerNextSchVal) > PWRCTRL_DEEPSLEEP_TIME_THRESHOLD))		/*deepsleep */
	{
        //PWRCTRL_REG_WRITE32(IO_ADDRESS(0x90020000), 'D');
        PWRCTRL_DEBUG_TRACE('D');
        PWRCTRL_TimerDisable(aNotAlwaysOnTimerState);
       

        PWRCTRL_DeepSleep();


        PWRCTRL_TimerEnable(aNotAlwaysOnTimerState);
	}
	else if((0 == (g_ulAcpuSleepVoteMap & PWRCTRL_LIGHTSLEEP_VOTE_MASK)) \
		 && ((timerNextSchVal) > PWRCTRL_LIGHTSLEEP_TIME_THRESHOLD))	    /*light sleep */
	{
        //PWRCTRL_REG_WRITE32(IO_ADDRESS(0x90020000), 'L');
        PWRCTRL_DEBUG_TRACE('L');
        OldSlice = PWRCTRL_GetTimerCount(g_aTimerBaseAddr[PWRCTRL_SLICE_TIMER]);
		PWRCTRL_LightSleep();
        NewSlice = PWRCTRL_GetTimerCount(g_aTimerBaseAddr[PWRCTRL_SLICE_TIMER]);

        g_ulAcupIdleTime  += (OldSlice - NewSlice);

	}
	else		/*wfi*/
	{
        OldSlice = PWRCTRL_GetTimerCount(g_aTimerBaseAddr[PWRCTRL_SLICE_TIMER]);
        PWRCTRL_DEBUG_TRACE('A');
        PWRCTRL_ArmSleep();

        NewSlice = PWRCTRL_GetTimerCount(g_aTimerBaseAddr[PWRCTRL_SLICE_TIMER]);

        g_ulAcupIdleTime  += (OldSlice - NewSlice);
    
	}
    return;
}
#endif

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnGetTimerInfo
 Description    : get timer info
 Input          : ulTimerId: Timer ID
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnGetTimerInfo(UINT32 ulTimerId)
{
    UINT32 ulTimerLoad, ulTimerValue,ulTimerControl, ulTimerIntStat;    

    if (ulTimerId > 23)
    {
        logMsg("PWRCTRL_SleepMntnGetTimerInfo param ulTimerId %d error.\n",ulTimerId);
        return;
    }
    
    ulTimerLoad = PWRCTRL_ReadReg32(PWRCTRL_TIMER_LOADCNT_ADDR(g_aTimerBaseAddr[ulTimerId]));
    ulTimerValue = PWRCTRL_ReadReg32(PWRCTRL_TIMER_CURVAL_ADDR(g_aTimerBaseAddr[ulTimerId]));
    ulTimerControl = PWRCTRL_ReadReg32(PWRCTRL_TIMER_CTRL_ADDR(g_aTimerBaseAddr[ulTimerId]));
    ulTimerIntStat = PWRCTRL_ReadReg32(PWRCTRL_TIMER_INTSTAT_ADDR(g_aTimerBaseAddr[ulTimerId]));

    logMsg("Timer %02d: Load=0x%08x, Value=0x%08x, Control=0x%08x, IntStat=0x%08x\n",
            ulTimerId, ulTimerLoad, ulTimerValue, ulTimerControl, ulTimerIntStat);
}

/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnGetGlobalTimerInfo
 Description    : get global timer info
 Input          : ulGroupId: Timer Group ID
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnGetGlobalTimerInfo(UINT32 ulGroupId)
{
    UINT32 ulTimerAddr, ulTimerIntStat, ulTimerRawIntStat,ulTimerVersion;    

    if (ulGroupId > 2)
    {
        logMsg("PWRCTRL_SleepMntnGetGlobalTimerInfo param ulGroupId %d error.\n",ulGroupId);
        return;
    }

    if (0 == ulGroupId)
    {
        ulTimerAddr = g_aTimerBaseAddr[7] + 0x14;
    }
    else if (1 == ulGroupId)
    {
        ulTimerAddr = g_aTimerBaseAddr[15] + 0x14;
    }
    else
    {
        ulTimerAddr = g_aTimerBaseAddr[23] + 0x14;
    }

    ulTimerIntStat = PWRCTRL_ReadReg32((UINT32)&(((PWRCTRL_GLOBAL_TIMER_REG_STRU *)(ulTimerAddr))-> ulTimerIntStat));
    ulTimerRawIntStat = PWRCTRL_ReadReg32((UINT32)&(((PWRCTRL_GLOBAL_TIMER_REG_STRU *)(ulTimerAddr))-> ulTimerRawIntStat));
    ulTimerVersion = PWRCTRL_ReadReg32((UINT32)&(((PWRCTRL_GLOBAL_TIMER_REG_STRU *)(ulTimerAddr))-> ulTimerVersion));

    logMsg("Global Timer %d: IntStat=0x%x, RawIntStat=0x%x, Version=0x%x\n",
            ulGroupId, ulTimerIntStat, ulTimerRawIntStat, ulTimerVersion);
}

/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnGetAllTimerInfo
 Description    : get all timer info
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnGetAllTimerInfo(void)
{
    UINT32 i;

    logMsg("Show All Timer Info:\n");
    for(i = 0; i <= 23; i++)
    {
        PWRCTRL_SleepMntnGetTimerInfo(i);
    }
    PWRCTRL_SleepMntnGetGlobalTimerInfo(0);
    PWRCTRL_SleepMntnGetGlobalTimerInfo(1);
    PWRCTRL_SleepMntnGetGlobalTimerInfo(2);
}

/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnGetTickStandard
 Description    : get tick and standard timer value
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnGetTickAndTimerValue(void)
{
    UINT32 ulTickValue, ulTimerValue;

    ulTimerValue = PWRCTRL_GetSleepSlice();
    ulTickValue = jiffies;

    logMsg("PWRCTRL_SleepMntnGetTickAndTimerValue: Tick 0x%x, Timer 0x%x\n",ulTickValue,ulTimerValue);
}

/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnGetVoteStatus
 Description    : get vote status
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnGetVoteStatus(void)
{
    UINT32 ulTempVal = g_ulAcpuSleepVoteMap;
	UINT32 u32Stat[32] = {0};
	UINT32 i = 0 , u32Date =0x1, u32BitValue;

	for (i = 0; i < (PWRCTRL_CLIENT_BUTT - 0x100) ; i++)
    {   
        if (0 != i)
        {
            u32Date <<= 1;
        }        
		
        u32BitValue = ulTempVal & u32Date;
       
        /* 如果有中断 ,则调用对应中断处理函数 */
        if (0 != u32BitValue)
        {  
            u32Stat[i] = 1;
        }
		else
		{
			u32Stat[i] = 0;
		}
    }

	logMsg("各组件投票状态(1表示反对睡眠 ，0表示赞成睡眠):\n");
	logMsg("LCD   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_LCD- 0x100]);
	logMsg("BOOT  VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_BOOT - 0x100]);
	logMsg("WIFI  VOTE STATUS: %d \n", u32Stat[PWRCTRL_LIGHTSLEEP_WIFI - 0x100]);
	logMsg("OAM   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_OAM - 0x100]);
	logMsg("SOCP  VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_SOCP - 0x100]);
	logMsg("SD    VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_SD - 0x100]);
	logMsg("ICC   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_ICC - 0x100]);
	logMsg("SCI   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_SCI - 0x100]);
	logMsg("USB   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_USB - 0x100]);
	logMsg("IFC   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_IFC - 0x100]);
	logMsg("KEY   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_KEY - 0x100]);
	logMsg("APP   VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_APP - 0x100]);
	logMsg("RNIC  VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_RNIC - 0x100]);
	#if(FEATURE_HSIC_SLAVE == FEATURE_ON)
	logMsg("HSIC  VOTE STATUS: %d \n", u32Stat[PWRCTRL_SLEEP_HSIC - 0x100]);
	#endif
	
}
/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnShowSleepMgrStat
 Description    : show SleepMgr statistics info
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnShowSleepMgrStat(UINT32 ulIndex)
{
    UINT32 i;
        
    logMsg("PWRCTRL_SleepMntnShowSleepMgrStat:\n");

    g_stSleepMgrStatisticsInfo.stRunCurrentTime.ulTickValue = jiffies;
    g_stSleepMgrStatisticsInfo.stRunCurrentTime.ulTimerValue = PWRCTRL_GetSleepSlice();

    logMsg("Begin Run Time:     tick 0x%08x, timer 0x%08x\n",g_stSleepMgrStatisticsInfo.stRunBeginTime.ulTickValue,g_stSleepMgrStatisticsInfo.stRunBeginTime.ulTimerValue);
    logMsg("Current Run Time:   tick 0x%08x, timer 0x%08x\n",g_stSleepMgrStatisticsInfo.stRunCurrentTime.ulTickValue,g_stSleepMgrStatisticsInfo.stRunCurrentTime.ulTimerValue);

    logMsg("\n");

    logMsg("Total Sleep Time:   tick 0x%08x, timer 0x%08x\n",g_stSleepMgrStatisticsInfo.stSleepTotalTime.ulTickValue,g_stSleepMgrStatisticsInfo.stSleepTotalTime.ulTimerValue);
    logMsg("Arm Sleep Time:     tick 0x%08x, timer 0x%08x\n",g_stSleepMgrStatisticsInfo.stArmSleepTime.ulTickValue,g_stSleepMgrStatisticsInfo.stArmSleepTime.ulTimerValue);
    logMsg("Light Sleep Time:   tick 0x%08x, timer 0x%08x\n",g_stSleepMgrStatisticsInfo.stLightSleepTime.ulTickValue,g_stSleepMgrStatisticsInfo.stLightSleepTime.ulTimerValue);
    logMsg("Deep Sleep Time:    tick 0x%08x, timer 0x%08x\n",g_stSleepMgrStatisticsInfo.stDeepSleepTime.ulTickValue,g_stSleepMgrStatisticsInfo.stDeepSleepTime.ulTimerValue);

    logMsg("\n");

    logMsg("Last Min Timer Id:  %d\n",g_stSleepMgrStatisticsInfo.ulLastMinTimerId);
    logMsg("Last Min Time Ms:   0x%x\n",g_stSleepMgrStatisticsInfo.ulLastMinTimeMs);
    logMsg("Last Min Vote Map:  0x%08x\n",g_stSleepMgrStatisticsInfo.ulLastVoteMap);
    
    logMsg("Last Old Time:      0x%x\n",g_stSleepMgrStatisticsInfo.ulLastOldTime);
    logMsg("Last Old Tick:      0x%x\n",g_stSleepMgrStatisticsInfo.ulLastOldTick);
    logMsg("Last Offset In:     0x%x\n",g_stSleepMgrStatisticsInfo.ulLastOffsetIn);
    logMsg("Last New Time:      0x%x\n",g_stSleepMgrStatisticsInfo.ulLastNewTime);
    logMsg("Last New Tick:      0x%x\n",g_stSleepMgrStatisticsInfo.ulLastNewTick);
    logMsg("Last Offset Out:    0x%x\n",g_stSleepMgrStatisticsInfo.ulLastOffsetOut);

    logMsg("\n");
    
    logMsg("Total Sleep num:    0x%x\n",g_stSleepMgrStatisticsInfo.ulSleepTotalCount);
    logMsg("Arm Sleep num:      0x%x\n",g_stSleepMgrStatisticsInfo.ulArmSleepCount);
    logMsg("Light Sleep num:    0x%x\n",g_stSleepMgrStatisticsInfo.ulLightSleepCount);
    logMsg("Deep Sleep num:     0x%x\n",g_stSleepMgrStatisticsInfo.ulDeepSleepCount);
    
    if (0 != ulIndex)
    {
        logMsg("\n");
        logMsg("==========================================================\n");
        logMsg("Arm Sleep Record: Current position %d\n",((g_stSleepMgrStatisticsInfo.ulArmSleepCount-1)%PWRCTRL_SLEEP_RECORD_NUM));
        logMsg("Num     In Tick    In Timer  |  Out Tick   Out Timer\n");
        for (i=0; i<(g_stSleepMgrStatisticsInfo.ulArmSleepCount < PWRCTRL_SLEEP_RECORD_NUM ? g_stSleepMgrStatisticsInfo.ulArmSleepCount : PWRCTRL_SLEEP_RECORD_NUM); i++)
        {
            logMsg("%03d  0x%08x  0x%08x  |  0x%08x  0x%08x\n",
                i,
                g_stSleepMgrStatisticsInfo.astArmSleepRecord[i].stSleepInTime.ulTickValue,
                g_stSleepMgrStatisticsInfo.astArmSleepRecord[i].stSleepInTime.ulTimerValue,
                g_stSleepMgrStatisticsInfo.astArmSleepRecord[i].stSleepOutTime.ulTickValue,
                g_stSleepMgrStatisticsInfo.astArmSleepRecord[i].stSleepOutTime.ulTimerValue,
                0);
        }
            
        logMsg("\n");
        logMsg("==========================================================\n");
        logMsg("Light Sleep Record: Current position %d\n",((g_stSleepMgrStatisticsInfo.ulLightSleepCount-1)%PWRCTRL_SLEEP_RECORD_NUM));
        logMsg("Num     In Tick    In Timer  |  Out Tick   Out Timer\n");
        for (i=0; i<(g_stSleepMgrStatisticsInfo.ulLightSleepCount < PWRCTRL_SLEEP_RECORD_NUM ? g_stSleepMgrStatisticsInfo.ulLightSleepCount : PWRCTRL_SLEEP_RECORD_NUM); i++)
        {
            logMsg("%03d  0x%08x  0x%08x  |  0x%08x  0x%08x\n",
                i,
                g_stSleepMgrStatisticsInfo.astLightSleepRecord[i].stSleepInTime.ulTickValue,
                g_stSleepMgrStatisticsInfo.astLightSleepRecord[i].stSleepInTime.ulTimerValue,
                g_stSleepMgrStatisticsInfo.astLightSleepRecord[i].stSleepOutTime.ulTickValue,
                g_stSleepMgrStatisticsInfo.astLightSleepRecord[i].stSleepOutTime.ulTimerValue,
                0);
        }

        logMsg("\n");
        logMsg("==========================================================\n");
        logMsg("Deep Sleep Record: Current position %d\n",((g_stSleepMgrStatisticsInfo.ulDeepSleepCount-1)%PWRCTRL_SLEEP_RECORD_NUM));
        logMsg("Num     In Tick    In Timer  |  Out Tick   Out Timer\n");
        for (i=0; i<(g_stSleepMgrStatisticsInfo.ulDeepSleepCount < PWRCTRL_SLEEP_RECORD_NUM ? g_stSleepMgrStatisticsInfo.ulDeepSleepCount : PWRCTRL_SLEEP_RECORD_NUM); i++)
        {
            logMsg("%03d  0x%08x  0x%08x  |  0x%08x  0x%08x\n",
                i,
                g_stSleepMgrStatisticsInfo.astDeepSleepRecord[i].stSleepInTime.ulTickValue,
                g_stSleepMgrStatisticsInfo.astDeepSleepRecord[i].stSleepInTime.ulTimerValue,
                g_stSleepMgrStatisticsInfo.astDeepSleepRecord[i].stSleepOutTime.ulTickValue,
                g_stSleepMgrStatisticsInfo.astDeepSleepRecord[i].stSleepOutTime.ulTimerValue);
        }
    }
}

/*****************************************************************************
 Prototype      : PWRCTRL_SleepMntnCmdHelp
 Description    : show SleepMntn command help
 Input          : NULL
 Output         : NULL
 Return Value   : NULL
 Calls          :
 Called By
*****************************************************************************/
void PWRCTRL_SleepMntnCmdHelp(void)
{
    logMsg("=============Show SleepMntn Command Help==============\n");
    logMsg("PWRCTRL_SleepMntnGetTimerInfo(TimerID) \n");
    logMsg("PWRCTRL_SleepMntnGetGlobalTimerInfo(GlobalTimerID) \n");
    logMsg("PWRCTRL_SleepMntnGetAllTimerInfo \n");
    logMsg("PWRCTRL_SleepMntnGetTickAndTimerValue \n");
    logMsg("PWRCTRL_SleepMntnGetVoteStatus \n");
    logMsg("PWRCTRL_SleepMntnShowSleepMgrStat(index) \n");
}
EXPORT_SYMBOL(BSP_PWC_MspVoteRegister);
EXPORT_SYMBOL(BSP_PWC_SocpVoteRegister);
EXPORT_SYMBOL(BSP_PWC_SocpRestoreRegister);
#endif
EXPORT_SYMBOL(BSP_PWRCTRL_SleepInSocCB);

/********************************************************
*函数名	:PWRCTRL_SetWakeLock
*函数功能	:提供给APP的接口，在快速开关机过程中防止单板休眠
*输入参数	:BSP_BOOL bLock
*输出参数	:
*返回值	:PWRCTRL_SUCCESS:执行成功
*          PWRCTRL_PARA_INVALID:执行失败
*修改历史	:
*	日期	:2012-6-25
*	作者	:包先春
*	修改内容:初版作成
********************************************************/
int PWRCTRL_SetWakeLock(BSP_BOOL bLock)
{
    int locklvl = 0;
    PWC_CLIENT_ID_E  enClientId = PWRCTRL_SLEEP_APP;
	
	if((enClientId < PWRCTRL_SLEEP_SCI) ||(enClientId >= PWRCTRL_CLIENT_BUTT))
    {
        printk("PWRCTRL: id is not exist! \n");
        return PWRCTRL_PARA_INVALID;
    }

    locklvl = PWRCTRL_IntLock();
	
    if(bLock)
    {
        PWRCTRL_ACPU_SLEEP_LOCK(enClientId); 
    }
    else
    {
        PWRCTRL_ACPU_SLEEP_UNLOCK(enClientId);
    }
	
    PWRCTRL_IntUnlock(locklvl);

    return PWRCTRL_SUCCESS;
}
/********************************************************
*函数名	:PWRCTRL_SendEventToApp
*函数功能	:上报真关机事件
*输入参数	:
*输出参数	:
*返回值	:   无
*
*修改历史	:
*	日期	:2012-7-04
*	作者	:l00212897
*	修改内容:初版作成
********************************************************/
void PWRCTRL_SendEventToApp(DEVICE_ID id, KEY_EVENT code)
{
    int ret = 0;
	int size = 0;
    DEVICE_EVENT event ;
	unsigned int hour = 0;

    size = sizeof(DEVICE_EVENT);

    event.device_id = id; 
    event.event_code = code; 
    event.len = 0;

    ret = device_event_report(&event, size);
	
	/*打印当前时间以及事件信息*/
    hour = get_current_working_time();
    printk(KERN_ERR " [SHORT ON OFF] PWRCTRL_SendEventToApp:device id=%d,event id=%d,hour=%u;\r\n",event.device_id, event.event_code,hour);
    if (-1 == ret) 
    {
    	printk(KERN_ERR " [SHORT ON OFF] PWRCTRL_SendEventToApp: can't add event\r\n");
    }

}
/********************************************************
*函数名	:PWRCTRL_AlarmShutdown
*函数功能	:过放保护时间到，上报真关机事件
*输入参数	:
*输出参数	:
*返回值	:   无
*
*修改历史	:
*	日期	:2012-7-04
*	作者	:l00212897
*	修改内容:初版作成
********************************************************/
static void PWRCTRL_AlarmShutdown(void)
{
#if defined (FEATURE_LCD_ON)
    /*关机流程不允许睡眠*/
    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
    /*通知应用关机*/
    PWRCTRL_SendEventToApp(DEVICE_ID_KEY,KEY_POWER_OFF);
}

/********************************************************
*函数名	:PWRCTRL_SetOverDischargeProtect
*函数功能	:已经确定执行假关机后，使用本函数调用rtc接口开启过放保护
*输入参数	:
*输出参数	:
*返回值	:0:执行成功
*          -1:执行失败
*修改历史	:
*	日期	:2012-6-25
*	作者	:徐超
*	修改内容:初版作成
********************************************************/
int PWRCTRL_SetOverDischargeProtect()
{
#if defined(FEATURE_SHORT_ON_OFF)

	unsigned int batt_current_volt;//当前电压值

	batt_current_volt = BSP_CHG_VoltGet();
	
    /*查询快速开关机过放保护NV设置值*/
	printk(KERN_ERR "\r\n[SHORT ON OFF] set_over_discharge_protect NV info = %d;%d;%d;%d;%d;%d;%d;volt=%u;\r\n", 
			s_shortOnOffConfig.ulPowerOffMaxTimes,s_shortOnOffConfig.ulMaxTime,
			s_shortOnOffConfig.ulVoltLevel1,s_shortOnOffConfig.ulVoltLevel2,
			s_shortOnOffConfig.ulRTCLevel1,s_shortOnOffConfig.ulRTCLevel2,s_shortOnOffConfig.ulRTCLevel3,
			batt_current_volt);

	/*根据当前电量调用rtc接口设置三档唤醒时间*/
	if(batt_current_volt <= s_shortOnOffConfig.ulVoltLevel1)
	{
		balong_rtc_alarm_set(s_shortOnOffConfig.ulRTCLevel1/24,s_shortOnOffConfig.ulRTCLevel1%24,0,0,PWRCTRL_AlarmShutdown);
	}
	else if(batt_current_volt > s_shortOnOffConfig.ulVoltLevel1 
		&& batt_current_volt <= s_shortOnOffConfig.ulVoltLevel2)
	{
		balong_rtc_alarm_set(s_shortOnOffConfig.ulRTCLevel2/24,s_shortOnOffConfig.ulRTCLevel2%24,0,0,PWRCTRL_AlarmShutdown);
	}
	else
	{
		balong_rtc_alarm_set(s_shortOnOffConfig.ulRTCLevel3/24,s_shortOnOffConfig.ulRTCLevel3%24,0,0,PWRCTRL_AlarmShutdown);
	}

	/*更新快速开关机次数*/
	s_currentShortOnOffStatus.ulShortOffTimes++;
#endif	
    return 0;
}

/********************************************************
*函数名	:PWRCTRL_SetShortOffMode
*函数功能	:提供给APP的接口，用于通知底软当前是否进入假关机状态
*输入参数	:BSP_BOOL bSleepMode
*输出参数	:
*返回值	:PWRCTRL_SUCCESS:执行成功
*修改历史	:
*	日期	:2012-6-25
*	作者	:包先春
*	修改内容:初版作成
********************************************************/
int PWRCTRL_SetShortOffMode(BSP_BOOL bSleepMode)
{
#if defined(FEATURE_SHORT_ON_OFF)

	if(bSleepMode)
	{
		*(unsigned int*)(IO_ADDRESS(CORESHARE_MEM_SHORT_ONOFF_ADDR)) = SHORT_POWER_OFF_MODE_SET; 
		PWRCTRL_SetOverDischargeProtect();
        BSP_CHG_PollTimerSet(CHG_SHORT_ONOFF_POLL_TIMER);
        #if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
        if (!hw_dwc_otg_is_device_mode())
        {
        	dwc_otg_discharge_close();
            BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_USB); //d00117529 20130217            
        }
        
        #endif
	}	
	else
	{
		*(unsigned int*)(IO_ADDRESS(CORESHARE_MEM_SHORT_ONOFF_ADDR)) = (unsigned int)SHORT_POWER_OFF_MODE_CLR;
        BSP_CHG_PollTimerSet(CHG_FAST_POLL_TIMER);
        #if ((FEATURE_OTG == FEATURE_ON) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
        if (!hw_dwc_otg_is_device_mode())
        {
        	dwc_otg_discharge_open();
             BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_USB);//d00117529 20130217
             PWRCTRL_SendEventToApp(DEVICE_ID_USB,USB_AF10_ATTACH);
        }
        #endif
	}	
	logMsg("[SHORT ON OFF]SetShortOffMode,set value:%d\n", *(unsigned int*)(IO_ADDRESS(CORESHARE_MEM_SHORT_ONOFF_ADDR)));
	
#endif
	return PWRCTRL_SUCCESS;
}


/********************************************************
*函数名	:PWRCTRL_GetShortOffMode
*函数功能	:提供给按键的接口，用于获取底软当前是否进入假关机状态
*输入参数	:
*输出参数	:
*返回值	:SHORT_POWER_OFF_MODE_PARA:当前是否是假关机状态
*修改历史	:
*	日期	:2012-6-25
*	作者	:包先春
*	修改内容:初版作成
********************************************************/
SHORT_POWER_OFF_MODE_PARA PWRCTRL_GetShortOffMode()
{
#if defined(FEATURE_SHORT_ON_OFF)
	int value = 0;
	value = *(int*)(IO_ADDRESS(CORESHARE_MEM_SHORT_ONOFF_ADDR));
	logMsg("[SHORT ON OFF]GetShortOffMode,get value:%d\n", value);
	return (SHORT_POWER_OFF_MODE_PARA)value;
#else
    return SHORT_POWER_OFF_MODE_MAX;
#endif
}

int PWRCTRL_Ioctl(struct inode *inode,struct file *file, unsigned int cmd,unsigned long data)
{
	int ret = 0;

	printk(KERN_ERR "\r\n [SHORT ON OFF] PWRCTRL_Ioctl: cmd=%u, data = %lu!\r\n",cmd,data);
	
	if (NULL == file)
	{
		printk(KERN_ERR "\r\n PWRCTRL_Ioctl: file is NULL!\r\n");
		return -1;
	}

	switch(cmd)
	{
		case PWRCTL_WAKE_LOCK:
			/*APP投票不进入睡眠*/
			ret = PWRCTRL_SetWakeLock(true);
			break;
		case PWRCTL_WAKE_UNLOCK:
			/*APP投票可进入睡眠*/
			ret = PWRCTRL_SetWakeLock(false);
			break;
#if defined(FEATURE_SHORT_ON_OFF)
        case PWRCTL_SHORT_OFF_MODE:
            /*进入假关机模式*/
            balong_breath_temp_enable(true); //为了假开机时亮一下LED，尽量和真开机一致
            ret = PWRCTRL_SetShortOffMode(true);
            break;
        case PWRCTL_SHORT_ON_MODE:
            /*假开机成功*/
            balong_breath_temp_enable(false);//临时开启完毕，恢复LED状态
            ret = PWRCTRL_SetShortOffMode(false);
            if ( PWRCTRL_SUCCESS == ret )
            {
                if( BSP_FALSE == balong_breath_get_status() )
                {
                    /*根据NV配置情况，熄灭LED*/
                     balong_breath_led_ioctl(NULL, NULL, BREATH_LED_ID_POWER, BREATH_LED_STATE_OFF);
                }
            }
            break;
#endif
		default:
			/*驱动不支持该命令*/
			return -ENOTTY;
	}
	
	return ret;
}

int PWRCTRL_Open(struct inode *inode, struct file *file)
{
	if (NULL == inode || NULL == file)
	{
		;/*for pclint*/
	}
	return 0;
}

int PWRCTRL_Release(struct inode *inode, struct file *file)
{
	if (NULL == inode || NULL == file)
	{
		;/*for pclint*/
	}
	return 0;
}

static const struct file_operations PWRCTRL_Fops = {
	.owner		= THIS_MODULE,
	.ioctl   	= PWRCTRL_Ioctl,
	.open		= PWRCTRL_Open,
	.release	= PWRCTRL_Release,
};

static struct miscdevice PWRCTRL_Miscdev = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "power_control",
	.fops	= &PWRCTRL_Fops
};

static int __init PWRCTRL_Init(void)
{
	int ret = 0;
	
	ret = misc_register(&PWRCTRL_Miscdev);
	if (0 > ret)
	{
		printk(KERN_ERR "\r\n misc_register [power control module] failed.\r\n");
		return ret;
	}
	
	return ret;
}

static void __exit PWRCTRL_Exit(void)
{
	int ret = 0;
	ret = misc_deregister(&PWRCTRL_Miscdev);
	if (0 > ret)
	{
		printk(KERN_ERR "\r\n misc_deregister [power control module] failed.\r\n");
	}
}

module_init(PWRCTRL_Init);
module_exit(PWRCTRL_Exit);

MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("Balong Power Control Driver");
MODULE_LICENSE("GPL");

#else
/*++pandong cs*/
void arch_idle(void)
{
}
/*--pandong cs*/

#endif
