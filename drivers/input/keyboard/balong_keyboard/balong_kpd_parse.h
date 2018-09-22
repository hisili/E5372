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

#define KPD_SHORT_OFF_INTERVAL_TIME  (300)  /*��ʱ�������Ѿ����й�һ�μٹػ��ˣ��򱾴�ִ��һ����ػ�����λ:��*/

typedef struct
{
	KPD_MUX_ENUM  enmsg;
	unsigned int  virtualkeyvalue;    /*�����ϱ���ֵ*/
	
	/*�����¼��ϱ�ʱ��,kpd_parse_mux_init������*/
	unsigned int  PwrWpsTime;	/*�������¼�*/
	
	/*̧���¼��ϱ�ʱ�̷�Χ��kpd_parse_mux_init������*/
	unsigned int  PwrWpsTimeEnd;	/*�������¼�����ʱ��*/
}WPS_PWR_ITEM;
        

/*BEGIN: xuchao x00202188, Added for short boot, 2012-6-22*/
/*���ٿ��ػ������Ƿ�ʹ��NV��*/
typedef struct
{
    unsigned int ulEnable;   /*���ٿ��ػ��Ƿ�ʹ�ܣ�0����ʹ�ܣ�1��ʹ��*/
}NV_SHORT_POWER_ON_OFF_ENABLE_TYPE;

/*���ٿ��ػ�����������ϢNV��*/
typedef struct
{
    unsigned int ulPowerOffMaxTimes;   /*֧�ֵ����ٹػ�����*/
	unsigned int ulMaxTime;            /*�ٹػ���ʷ�ۼ�ʱ��>=��ʱ���Զ���ػ�����λСʱ*/
	unsigned int ulVoltLevel1;         /*���ű�����һ����ѹ����*/
	unsigned int ulVoltLevel2;         /*���ű����ڶ�����ѹ����*/
	unsigned int ulRTCLevel1;          /*С�ڵ�һ����ѹ��Ӧ��RTC����ʱ��*/
	unsigned int ulRTCLevel2;          /*��һ���͵ڶ���֮���ѹ��Ӧ��RTC����ʱ��*/
	unsigned int ulRTCLevel3;          /*���ڵ��ڵڶ�����ѹ��Ӧ��RTC����ʱ��*/
}NV_SHORT_POWER_ON_OFF_TYPE;

/*���ٿ��ػ���ǰ��Ϣ*/
typedef struct
{
    unsigned int ulShortOffTimes;   /*��ǰ�ٹػ�����*/
    unsigned int ulTotalTime;       /*�ٹػ���ʷ�ۼ�ʱ�䵥λСʱ*/
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

