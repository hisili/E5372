/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2011, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: kpd_parse.h                                                     */
/*                                                                           */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2010-09                                                             */
/*                                                                           */
/* Description: keyboard module parse information define                     */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/
#ifndef KPD_PARSE_H
#define KPD_PARSE_H
#include "BSP.h"
#include "balong_kpd_driver_gpio.h"
#include "../../../nvim/balong_nvim.h"

 /* 50 msecond for gpio key deshaking timeout*/
#ifdef KERNEL_MANAGE_SUSPEND
#define KPD_GPIO_DESHAKING_TIME     (20)
#else
#define KPD_GPIO_DESHAKING_TIME     (50)
#endif
/*------------------------------------------------------------------------------------*/
/*macro definition*/
/*------------------------------------------------------------------------------------*/

/*  
    NV struct and operation macro define.

    the macro be used to expand the item field
*/
#define KPD_PARSE_ITEM_GET_DRV_VALUE( ulitem )      (unsigned int)( ((ulitem) >> 24) & 0xFF )
 /* if need convert virtual value, change it */
#define KPD_PARSE_ITEM_GET_VIRTUAL_VALUE( ulitem )  (unsigned int)( ((ulitem) >> 16) & 0xFF ) 
#define KPD_PARSE_ITEM_GET_TIME_START( ulitem )     (unsigned int)( ((ulitem) >>  8) & 0xFF )
#define KPD_PARSE_ITEM_GET_TIME_END( ulitem )       (unsigned int)( ((ulitem) >>  0) & 0xFF )

/* 0x80 --> 0xFE be reserved for flags, the time should less than 0x80 */
#define KPD_PARSE_ITEM_TICK_INFINITE                (unsigned)0xFF
#define KPD_PARSE_ITEM_TICK_MAX                     (unsigned)0x80
#define KPD_PARSE_ITEM_FLAG_BASE                    KPD_PARSE_ITEM_TICK_MAX
#define KPD_PARSE_ITEM_FLAG_PRESS                   KPD_PARSE_ITEM_FLAG_BASE

#define KPD_NV_UNIT                         (16)  /* total 16 * 32Bit = 64bytes */
#define KPD_EVENT_MAX                       (KPD_NV_UNIT-5)
#define KPD_PARSE_ITEM_MAX                  16
#define KPD_WPS_PWR_EVENT_MAX               2

#define NV_OK                                      0
#define NV_KPD_CONFIG                          56
/*BEGIN: xuchao x00202188, Added for short boot, 2012-6-22*/
#define SHORT_ON_OFF_ENABLE                1
#define SHORT_ON_OFF_DISABLE               0

#define NV_SHORT_ON_OFF_ENABLE             0xD111 
#define NV_SHORT_ON_OFF_CONFIG             0xD112
/*END  : xuchao x00202188, Added for short boot, 2012-6-22*/

#if (FEATURE_KEYBOARD == FEATURE_ON)
#define KPD_WPS_DBLCLICK_TIMER           (1000)  /* 1000ms for power double click check*/ 
#endif

#define KPD_DOUBLE_CLICK_DESHAKING_TIME  (100)

#define KPD_SHORT_OFF_INTERVAL_TIME  (300)  /*该时间内若已经进行过一次假关机了，则本次执行一次真关机，单位:秒*/

typedef struct
{
	KPD_MUX_ENUM  enmsg;
	unsigned int  virtualkeyvalue;    /*虚拟上报键值*/
	
	/*按下事件上报时刻,kpd_parse_mux_init中设置*/
	unsigned int  PwrWpsTime;	/*非升级事件*/
	
	/*抬起事件上报时刻范围，kpd_parse_mux_init中设置*/
	unsigned int  PwrWpsTimeEnd;	/*非升级事件结束时刻*/
}WPS_PWR_ITEM;
        

/*BEGIN: xuchao x00202188, Added for short boot, 2012-6-22*/
/*快速开关机功能是否使能NV项*/
typedef struct
{
    unsigned int ulEnable;   /*快速开关机是否使能：0：不使能；1：使能*/
}NV_SHORT_POWER_ON_OFF_ENABLE_TYPE;

/*快速开关机功能配置信息NV项*/
typedef struct
{
    unsigned int ulPowerOffMaxTimes;   /*支持的最大假关机次数*/
	unsigned int ulMaxTime;            /*假关机历史累加时间>=此时间自动真关机，单位小时*/
	unsigned int ulVoltLevel1;         /*过放保护第一档电压门限*/
	unsigned int ulVoltLevel2;         /*过放保护第二档电压门限*/
	unsigned int ulRTCLevel1;          /*小于第一档电压对应的RTC唤醒时间*/
	unsigned int ulRTCLevel2;          /*第一档和第二档之间电压对应的RTC唤醒时间*/
	unsigned int ulRTCLevel3;          /*大于等于第二档电压对应的RTC唤醒时间*/
}NV_SHORT_POWER_ON_OFF_TYPE;

/*快速开关机当前信息*/
typedef struct
{
    unsigned int ulShortOffTimes;   /*当前假关机次数*/
    unsigned int ulTotalTime;       /*假关机历史累加时间单位小时*/
}CURRENT_SHORT_POWER_ON_OFF_STATUS;

/* common keyboard parse routine */
void kpd_parse_common_handler( KPD_DRIVER_MSG_ENUM enmsg, KPD_DRIVER_EVENT_ENUM enevent);
void kpd_parse_mux_init( unsigned int ulforceupdatetime ,unsigned int *ulitem);
void kpd_parse_mux( KPD_DRIVER_MSG_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent );
void kpd_nv_config_init( void );

extern int balong_rtc_alarm_clear();
extern unsigned long get_current_working_time();
extern void BSP_Short_On_MMC(void);
extern void BSP_Short_Off_MMC(void);
extern BSP_U32 NVM_Read(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);
/* KPD_PARSE_H */
#endif 

