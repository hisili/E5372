/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepMgr.h
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

#ifndef PWRCTRL_ACPU_SLEEP_MGR
#define PWRCTRL_ACPU_SLEEP_MGR
/*V3R2 CS f00164371*/
#include "generated/FeatureConfigDRV.h"

//#if (FEATURE_DEEPSLEEP == FEATURE_ON) || (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#if  (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))  //clean warning


/*********************************************************/
typedef enum
{

    SHORT_POWER_OFF_MODE_CLR = 0,
    SHORT_POWER_OFF_MODE_SET = 1,
    SHORT_POWER_OFF_MODE_MAX = 2,       /*非法值*/
}SHORT_POWER_OFF_MODE_PARA;

typedef enum
{
    PWRCTL_WAKE_LOCK      = 3, /*不进入睡眠*/
    PWRCTL_WAKE_UNLOCK    = 4, /*可进入睡眠*/
    PWRCTL_SHORT_OFF_MODE = 5, /*进入假关机模式*/
    PWRCTL_SHORT_ON_MODE  = 6, /*退出假关机模式*/
    PWRCTL_MODE_MAX,            /*非法值*/
}PWRCTL_IOCTL_CMD;

typedef enum
{

    PWRCTRL_SUCCESS = 0,
    PWRCTRL_FAIL,

    PWRCTRL_PARA_INVALID = 0x100,
}EN_PWRCTRL_ACPU_INFO;

typedef struct PWRCTRL_TIMER_ST_P
{
	unsigned int loadCount;
	unsigned int curValue;
	unsigned int ctrl;
	unsigned int intClr;
	unsigned int intStatus;
}PWRCTRL_TIMER_ST;

typedef struct
{
    FUNCPTR routine;
} BSP_PWC_ENTRY;

#define PWRCTRL_TIMER_LOADCNT_ADDR(addr)   \
	((unsigned int)&(((PWRCTRL_TIMER_ST *)(addr))->loadCount))
#define PWRCTRL_TIMER_CURVAL_ADDR(addr)   \
		((unsigned int)&(((PWRCTRL_TIMER_ST *)(addr))->curValue))
#define PWRCTRL_TIMER_CTRL_ADDR(addr)   \
		((unsigned int)&(((PWRCTRL_TIMER_ST *)(addr))->ctrl))
#define PWRCTRL_TIMER_INTSTAT_ADDR(addr)   \
            ((unsigned int)&(((PWRCTRL_TIMER_ST *)(addr))->intStatus))

typedef struct
{
    unsigned int ulTimerIntStat;          /* Interrupt Status Register */
    unsigned int ulTimerIntClr;           /* Interrupt Clear Register */
    unsigned int ulTimerRawIntStat;       /* Raw Interrupt Status Register */
    unsigned int ulTimerVersion;          /* Timer Version Register */
} PWRCTRL_GLOBAL_TIMER_REG_STRU;

 #if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))  \
    ||(defined(BOARD_SFT) && defined(VERSION_V7R1))
 #define PWRCTRL_SLICE_TIMER 2
 #elif (defined (BOARD_SFT) && defined (VERSION_V3R2))
 #define PWRCTRL_SLICE_TIMER 0
#elif (defined (BOARD_FPGA) && defined (VERSION_V3R2))||\
    (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
 #define PWRCTRL_SLICE_TIMER 0
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK) || defined(BOARD_SFT))) 
#define PWRCTRL_SLICE_TIMER 11
#endif

/*********************************************************/

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
      
#define PWRCTRL_SLEEP_RECORD_NUM 100
    
typedef struct
{
    unsigned int ulTickValue;
    unsigned int ulTimerValue;
} PWC_TIME_STRU;

typedef struct
{
    PWC_TIME_STRU stSleepInTime;
    PWC_TIME_STRU stSleepOutTime;
} PWC_SLEEP_TIME_STRU;

typedef struct
{
    PWC_TIME_STRU stRunBeginTime;
    PWC_TIME_STRU stRunCurrentTime;

    PWC_TIME_STRU stSleepTotalTime;
    PWC_TIME_STRU stArmSleepTime;
    PWC_TIME_STRU stLightSleepTime;
    PWC_TIME_STRU stDeepSleepTime;

    UINT32 ulSleepTotalCount;
    UINT32 ulArmSleepCount;
    UINT32 ulLightSleepCount;
    UINT32 ulDeepSleepCount;

    PWC_SLEEP_TIME_STRU astArmSleepRecord[PWRCTRL_SLEEP_RECORD_NUM];
    PWC_SLEEP_TIME_STRU astLightSleepRecord[PWRCTRL_SLEEP_RECORD_NUM];
    PWC_SLEEP_TIME_STRU astDeepSleepRecord[PWRCTRL_SLEEP_RECORD_NUM];

    unsigned int ulLastMinTimerId;
    unsigned int ulLastMinTimeMs;
    unsigned int ulLastVoteMap;
    
    unsigned int ulLastOldTime;
    unsigned int ulLastOldTick;
    unsigned int ulLastNewTime;
    unsigned int ulLastNewTick;
    unsigned int ulLastOffsetIn;
    unsigned int ulLastOffsetOut;

} PWC_SLEEPMGR_STATISTICS_STRU;
#endif
	
#define PWRCTRL_CLIENT_ID_MASK         	(0x1f)
#define PWRCTRL_ACPU_PARA_ALIGN			(0x200)
	
	
#define PWRCTRL_TIMER_MAX				(24)

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))\
    ||(defined(BOARD_SFT) && defined(VERSION_V7R1))
#define PWRCTRL_ALWAYSON_TIMER (0xffffff)
#elif (defined (BOARD_SFT) && defined (VERSION_V3R2))
#define PWRCTRL_ALWAYSON_TIMER ((1 << 0)|(1 << 1) | (1 << 2)|(1 << 3)|(1 << 5))
#elif (defined (BOARD_FPGA) && defined (VERSION_V3R2)) ||\
    (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#define PWRCTRL_ALWAYSON_TIMER (0xffffff)
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
#define PWRCTRL_ALWAYSON_TIMER ((1 << 1)|(1 << 2) | (1 << 3)|(1 << 4)|(1 << 5)|(1 << 10)|(1 << 11)|(1 << 20)|(1 << 22)) 
#else
#define PWRCTRL_ALWAYSON_TIMER (0xffffff)

#endif

#define PWRCTRL_ACPU_TIMER              ((1<<0)|(1<<2)|(1<<7)|(1<<14)|(1<<16))
#define PWRCTRL_TIMER_EN_MSK            (1<<0)
#define PWRCTRL_TIMER_INT_MSK           (1<<2)
#define PWRCTRL_TIMER_MS_COV            (1000)

	
#define PWRCTRL_CLK_SLEEP				(32768)
#define PWRCTRL_CLK_TCXO				(19200000)
	
	
	
#define PWRCTRL_ACPU_SLEEP_LOCK(id)  \
		g_ulAcpuSleepVoteMap |= (unsigned int)(1 << (id & PWRCTRL_CLIENT_ID_MASK))
	
#define PWRCTRL_ACPU_SLEEP_UNLOCK(id)  \
		g_ulAcpuSleepVoteMap &= (~(unsigned int)(1 << (id & PWRCTRL_CLIENT_ID_MASK)))


//clean warning
#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#ifdef FEATURE_SD_ENABLE
#if (FEATURE_HSIC_SLAVE==FEATURE_ON)
#define PWRCTRL_DEEPSLEEP_VOTE_MASK \
    (  (1 << (PWRCTRL_SLEEP_OAM & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_LCD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SOCP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_HSIC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_ICC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_LIGHTSLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_PS & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SCI & PWRCTRL_CLIENT_ID_MASK)) \
	 | (1 << (PWRCTRL_SLEEP_RNIC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_IFC & PWRCTRL_CLIENT_ID_MASK)) \
	 | (1 << (PWRCTRL_SLEEP_APP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << ( (int)PWRCTRL_SLEEP_SPECIALTIMER & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_KEY & PWRCTRL_CLIENT_ID_MASK)))
 #else
 #define PWRCTRL_DEEPSLEEP_VOTE_MASK \
    (  (1 << (PWRCTRL_SLEEP_OAM & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_LCD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SOCP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_ICC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_LIGHTSLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_PS & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SCI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_RNIC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_IFC & PWRCTRL_CLIENT_ID_MASK)) \
	 | (1 << (PWRCTRL_SLEEP_APP & PWRCTRL_CLIENT_ID_MASK)) \
    | (1 << ( (int)PWRCTRL_SLEEP_SPECIALTIMER & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_KEY & PWRCTRL_CLIENT_ID_MASK)))
 #endif    
#define PWRCTRL_LIGHTSLEEP_VOTE_MASK \
    (  (1 << (PWRCTRL_SLEEP_OAM & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SOCP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_ICC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SCI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_PS & PWRCTRL_CLIENT_ID_MASK)))
#else
#if (FEATURE_HSIC_SLAVE==FEATURE_ON)
#define PWRCTRL_DEEPSLEEP_VOTE_MASK \
    (  (1 << (PWRCTRL_SLEEP_OAM & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_LCD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SOCP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_HSIC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_ICC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_LIGHTSLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_PS & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SCI & PWRCTRL_CLIENT_ID_MASK)) \
	 | (1 << (PWRCTRL_SLEEP_RNIC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_IFC & PWRCTRL_CLIENT_ID_MASK)) \
	 | (1 << (PWRCTRL_SLEEP_APP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << ( (int)PWRCTRL_SLEEP_SPECIALTIMER & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_KEY & PWRCTRL_CLIENT_ID_MASK)))
 #else
 #define PWRCTRL_DEEPSLEEP_VOTE_MASK \
    (  (1 << (PWRCTRL_SLEEP_OAM & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_LCD & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SOCP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_ICC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_LIGHTSLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_PS & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SCI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_RNIC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_IFC & PWRCTRL_CLIENT_ID_MASK)) \
	 | (1 << (PWRCTRL_SLEEP_APP & PWRCTRL_CLIENT_ID_MASK)) \
    | (1 << ( (int)PWRCTRL_SLEEP_SPECIALTIMER & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_KEY & PWRCTRL_CLIENT_ID_MASK)))
 #endif    
#define PWRCTRL_LIGHTSLEEP_VOTE_MASK \
    (  (1 << (PWRCTRL_SLEEP_OAM & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SOCP & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_ICC & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_SCI & PWRCTRL_CLIENT_ID_MASK)) \
     | (1 << (PWRCTRL_SLEEP_PS & PWRCTRL_CLIENT_ID_MASK)))
#endif

#define PWRCTRL_DEEPSLEEP_TIME_THRESHOLD				(20)
#define PWRCTRL_LIGHTSLEEP_TIME_THRESHOLD				(5) 
#define PWRCTRL_PERI_POWERDOWN_VOTE_MASK    (1 << (PWRCTRL_SLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK))

#else
#if (FEATURE_HSIC_SLAVE==FEATURE_ON)
#define PWRCTRL_DEEPSLEEP_VOTE_MASK				           \
	((1 << ( PWRCTRL_SLEEP_DMA & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_SLEEP_LCD & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_SLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( (int)PWRCTRL_SLEEP_HSIC & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( PWRCTRL_TEST_DEEPSLEEP & PWRCTRL_CLIENT_ID_MASK)))
#else
#define PWRCTRL_DEEPSLEEP_VOTE_MASK				           \
	((1 << ( PWRCTRL_SLEEP_DMA & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_SLEEP_LCD & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_SLEEP_WIFI & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK)) \
   | (1 << ( PWRCTRL_TEST_DEEPSLEEP & PWRCTRL_CLIENT_ID_MASK)))
#endif
#define PWRCTRL_LIGHTSLEEP_VOTE_MASK					\
    ((1 << ( PWRCTRL_SLEEP_DMA & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_SLEEP_USB & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_SLEEP_BOOT & PWRCTRL_CLIENT_ID_MASK))  \
   | (1 << ( PWRCTRL_TEST_LIGHTSLEEP & PWRCTRL_CLIENT_ID_MASK)))  
	

#define PWRCTRL_DEEPSLEEP_TIME_THRESHOLD				(105)
#define PWRCTRL_LIGHTSLEEP_TIME_THRESHOLD				(5)
#endif


extern void PWRCTRL_SleepMgrInitial(void);
extern unsigned int PWRCTRL_GetTimerCount(unsigned int ulTimerAddr );
extern unsigned int BSP_PWRCTRL_SleepVoteLock(unsigned int id);
extern unsigned int BSP_PWRCTRL_SleepVoteUnLock(unsigned int id);

#ifdef FEATURE_SHORT_ON_OFF
extern int PWRCTRL_SetShortOffMode(BSP_BOOL bSleepMode);
extern SHORT_POWER_OFF_MODE_PARA PWRCTRL_GetShortOffMode();
#endif

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
extern unsigned int PWRCTRL_GetSleepSlice(void);
#endif

#endif
#endif
