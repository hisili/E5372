/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2011, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: Kpd_parse_common.c                                              */
/*                                                                           */
/* Author:                                                                   */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2010-09                                                             */
/*                                                                           */
/* Description: keyboard module parse operation                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/*****************************************************************************/
/**********************问题单修改记录************************************************
日    期              修改人                     问题单号                   修改内容
********************************************************************************************/

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/spinlock.h>
#include "BSP.h"
#include <linux/netlink.h>
#include "balong_kpd_parse.h"
#include "balong_kpd_driver_gpio.h"
#include <linux/kthread.h>
#include <linux/delay.h>
#include "../drivers/rtc/balong_rtc.h"
#include "../../../../arch/arm/mach-balong/include/mach/pwrctrl/BSP_PWC_SLEEPMGR.h"

#include <linux/BSP_CHGC_DRV.h>
#define KPD_MIN_VOLT_OFF (3400)  /*关机充电时按键开机门限*/

#if ( FEATURE_MMI_TEST == FEATURE_ON )
#include "../../../../arch/arm/mach-balong/mmi.h"
#include <linux/jiffies.h>

unsigned long rstCurrentTime =0;//记录reset键被按下的当前时间
unsigned long rstLastTime = 0;//记录reset键被最后一次按下的时间

#endif

#if (FEATURE_POWER_ON_OFF == FEATURE_ON)
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
#include <linux/workqueue.h>
#include <mach/powerExchange.h>

/*Indicate firmware is FACTORY version or NOT.*/
uint8_t is_current_firmware_factory = 0;
#endif /*((defined(CHIP_...*/
#endif /*(FEATURE_POWER_ON_OFF == FEATURE_ON)*/

extern DEVICE_EVENT *kpd_event;
struct mutex disable_lock;

extern void balong_kpd_del_timer(HTIMER *ptimer);
void kpd_parse_timer_isr( unsigned int  iParam );
void kpd_server_new_msg( unsigned int  ulvirtualkey, unsigned int  ulparam );
extern KPD_MUX_ENUM kpd_parse_mux_check( void);

int wps_power_status = 0; //组合键状态统计
unsigned long wpsCurrentTime = 0; //记录wps键被按下的当前时间
unsigned long wpsLastTime    = 0; //记录wps键被最后一次按下的时间

/*------------------------------------------------------------------------------------*/
/*struct  define*/
/*------------------------------------------------------------------------------------*/

typedef struct /* total 64 bytes */
{
    unsigned int  ulversion;          /* Should be 0x0001_0000. major + minor */
    
    unsigned int  ulkeyupdatetime;    /* 0 means not support. non-zero is the update check time */
   unsigned int  ultickunit;         /* the tick unit. ms */
    
    /*unsigned int  ulReserved[3];*/      /* reserved for feature */
    
    /*  key parse table define.
        There are two type item( Bit23-8 is different )
            long press scene : send msg after reach T0, T1, T2 ....
            release    scene : send msg while release the key within Tx / Ty
        
        for long press scene :
            Bit 31 - 24 : the driver layer key value. this item will be handle when the key be pressed.
                          0xFF means invalid item.                          
            Bit 23 - 16 : virtual key that need send to Application            
            Bit 15 -  8 : always 0x80
            Bit  7 -  0 : tick(Tn). when the press tick is equal with Tn, send the virtual key to app.
        
        for release scene :            
            Bit 31 - 24 : the driver layer key value. (same with long press scene)
                          0xFF means invalid item.                          
            Bit 23 - 16 : virtual key that need send to Application.
            Bit 15 -  8 : tick(Tx). above 0xF0 be reserved.
            Bit  7 -  0 : tick(Ty). if Tx <= releaes tick < Ty, send virtual key to app.
                          Ty = 0xFF means infinite.
    */
    unsigned int  ulitemtable[ KPD_EVENT_MAX ]; //11

	/* 组合键事件定义，说明参照单个按键说明 ，
       组合键编号参照KPD_MUX_ENUM
    */
	unsigned int  ulitemtablePwrWPS[ KPD_WPS_PWR_EVENT_MAX ]; /* 组合键用 */
	/* sizeof(versio_1_1) = 64bytes*/
}KPD_NV_VERSION_1_1_T; /* version 1.1 structure */


/*
    Key Item expand to unsigned int type.
*/
typedef struct
{
    unsigned int  ultickstartorflag; 
    unsigned int  ultickend;
    unsigned int  ulvirtualkeyvalue;
}KPD_PARSE_ITEM_T;


typedef struct
{
    KPD_DRIVER_MSG_ENUM  enmsg;
    HTIMER             htimer_softtimer;    
   // struct mutex  parse_lock;
	struct spinlock  parse_lock;            /*lint !e43*/
    unsigned int              ulpressed;
    unsigned int              ultickunit;
    unsigned int               ulcurrenttick;
    
    unsigned int              ulitemnum;
    /*unsigned int               ulCurrentItemPos;*/
    KPD_PARSE_ITEM_T     stitemlist[ KPD_PARSE_ITEM_MAX ];
}KPD_PARSE_T; 


/*------------------------------------------------------------------------------------*/
/*variable define and declare*/
/*------------------------------------------------------------------------------------*/

KPD_NV_VERSION_1_1_T  s_stkpdnvconfig = {0};

KPD_PARSE_T  s_stkpdparselist[ KPD_GPIO_MAX ];

/*BEGIN: xuchao x00202188, Added for short boot, 2012-6-22*/
#if defined(FEATURE_SHORT_ON_OFF) 
extern CURRENT_SHORT_POWER_ON_OFF_STATUS s_currentShortOnOffStatus;//快速开关机当前运行统计

static NV_SHORT_POWER_ON_OFF_ENABLE_TYPE s_shortOnOffEnable = {0}; //快速开关机是否使能
NV_SHORT_POWER_ON_OFF_TYPE s_shortOnOffConfig ={0};                //快速开关机配置信息
static struct work_struct nv_read_work;
static bool isSendMsg = true;                                      //是否要向app发送消息
static unsigned long s_lastShortOffTime = 0;                       /*上一次假关机的时间(真开机到上次假关机的秒数)*/
#endif
/*END: xuchao x00202188, Added for short boot, 2012-6-22*/

#if (FEATURE_KEYBOARD == FEATURE_ON)
/*wps键双击定时器*/
static HTIMER            s_wps_dblclick_timer_softtimer;
static unsigned int   s_ulwpsclickcnt;               /* record the wps click occur.*/


//extern void kpd_parse_mux_init( unsigned int ulforceupdatetime );


/******************************************************************************
*  Function:  kpd_parse_wps_dblclick_isr
*  Description: kpd_parse_wps_dblclick_isr
*  Input:
*         eEvent : event
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
void kpd_parse_wps_dblclick_isr(unsigned int iparam )
{
	unsigned int g_ultime =0;  //记录两次wps键按下的时间差

    balong_kpd_del_timer(&s_wps_dblclick_timer_softtimer);

	g_ultime = (long)wpsCurrentTime - (long)wpsLastTime ;/*lint !e732 */
	g_ultime = g_ultime*1000 /HZ;
	printk("g_ultime = %d\n",g_ultime);
    if( s_ulwpsclickcnt >= 2 && g_ultime > KPD_DOUBLE_CLICK_DESHAKING_TIME )
    {
        /*显示SSID and password*/
	 kpd_server_new_msg( KEY_SSID, 0 );
		
        printk("+++++++++kpd_parse_wps_dblclick_isr+++++++++\n");
 
    }
    else /*wps键1s内按下次数小于2，不作处理*/
    {
         hikpd_trace(5,"the WPS key click times less 2 \r\n");
    }

   s_ulwpsclickcnt = 0;
    return;
}


/******************************************************************************
*  Function:  kpd_parse_pwr_dblclick_init
*  Description: power key double click parse init
*  Input:
*         null
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_wps_dblclick_init( void )
{
    s_ulwpsclickcnt = 0;
}
#endif

/**********************************************************************
FUNCTION:    KPD_PARSE_POWER_KEY_HANDLE
DESCRIPTION: The handle of power key: it will send/simulate event to 
             modules which related with power-on-off feature.
INPUT:       None.
OUTPUT:      None.
RETURN:      None.
NOTE:        1. Normal mode: sent power-key event to app to deal.
             2. Pwroff charge mode: Call IFC to notify C-CPU to process.
                If low batt detected, send low-batt event to app to show
                "charging" and still stay at power-off charging.
***********************************************************************/
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
#if (FEATURE_POWER_ON_OFF == FEATURE_ON)
void kpd_parse_power_key_handle(struct work_struct* work)
{
    int size = sizeof(DEVICE_EVENT);
    BATT_STATE_T batt_state;
    DEVICE_EVENT dev_evt;
    POWER_ON_STATUS_ENUM power_on_status = power_on_status_get();
    BSP_U32 batt_current_volt = BSP_CHG_VoltGet();

    BSP_CHG_GetBatteryState(&batt_state);

    if (POWER_ON_STATUS_FINISH_NORMAL == power_on_status)
    {
       /*This handle is not schedule for normal mode power-key event.*/
       pr_warning("%s should NOT be scheduled at normal start mode!!!\n", __FUNCTION__);
    }
    else if (POWER_ON_STATUS_FINISH_CHARGE == power_on_status)
    {
        /*if (BATT_LOW_POWER == batt_state.battery_level)*/
        if (batt_current_volt < KPD_MIN_VOLT_OFF)
        {
            dev_evt.device_id  = DEVICE_ID_BATTERY;
            dev_evt.event_code = BAT_LOW_POWER;
            dev_evt.len        = 0;
            device_event_report(&dev_evt, size);
            pr_info("Power key pressed while battery is low!!\n");
        }
        else
        {
            drvShutdown( DRV_SHUTDOWN_POWER_KEY );
            pr_info("Power key pressed and battery is OK, reset to normal mode!!\n");
        }
    }
    else
    {
        pr_alert("Inavlid power on status: 0x%x in %s\n", power_on_status_get(), __FUNCTION__);
    }

    return;
}
#endif /*(FEATURE_POWER_ON_OFF == FEATURE_ON)*/
#endif /*(defined(CHIP_BB_6920...*/

/******************************************************************************
*  Function:  kpd_wps_pwr_status_handle
*  Description: 统计当前组合键中按键被按下的情况
*  Input:
*         KPD_DRIVER_MSG_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         目前仅支持WPS和POWER组合键，场景如下:
*         有一个按键被按下为1，下一个被按下则为2，其中一个抬起为3，另一个抬起为4
*         有一个按键被按下为1，该按键被抬起则恢复为0
*         有一个按键被按下为1，下一个被按下则为2，其中一个抬起为3，该键又被按下则恢复为2
*  History:     
*
********************************************************************************/
void kpd_wps_pwr_status_handle(KPD_DRIVER_MSG_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent)
{	
    if( (KPD_DRIVER_EVENT_PRESS == enevent) &&
		(KPD_DRIVER_MSG_WPS == emsg ||KPD_DRIVER_MSG_POWER == emsg) )
	{
		if(4 == wps_power_status )
		{
		    wps_power_status =0;
		}
		
		if(3 == wps_power_status )
		{
		    wps_power_status =1;
		}
		
		wps_power_status++;
    }

	if( (KPD_DRIVER_EVENT_RELEASE == enevent) &&
		(KPD_DRIVER_MSG_WPS == emsg ||KPD_DRIVER_MSG_POWER == emsg) )
	{
		if( 1 == wps_power_status)
		{
			wps_power_status--;
		}
		else if(2 == wps_power_status)
		{
		    wps_power_status++;
		}
		else if(3 == wps_power_status)
		{
		    wps_power_status++;
		}
       
    }
}


/******************************************************************************
*  Function:  kpd_parse_list_init
*  Description: keypad parse array initialise
*  Input:
*         null
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_list_init( void )
{
    memset( (void *)(s_stkpdparselist), 0, sizeof(s_stkpdparselist) );/*lint !e522*/
}

/******************************************************************************
*  Function:  kpd_parse_item_to_list
*  Description: get each key event time
*  Input:
*         ulItem:event list in NV
*         pstparse:inner key parse struct
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_item_to_list( unsigned int ulitem, KPD_PARSE_T * pstparse )
{
    KPD_PARSE_ITEM_T      * pstitem = &(pstparse->stitemlist[ pstparse->ulitemnum ]);

    pstitem->ulvirtualkeyvalue = KPD_PARSE_ITEM_GET_VIRTUAL_VALUE( ulitem );
    pstitem->ultickstartorflag = KPD_PARSE_ITEM_GET_TIME_START( ulitem );
    pstitem->ultickend         = KPD_PARSE_ITEM_GET_TIME_END( ulitem );
    if( KPD_PARSE_ITEM_TICK_INFINITE == pstitem->ultickend )
    {
        pstitem->ultickend = 0xFFFFFFFF; /* expand to 32bit, easy to compare with the current tick */
    }

    pstparse->ulitemnum++;
}

/******************************************************************************
*  Function:  kpd_parse_nv_version_1_1
*  Description:  convert NV struct to inner-struct
*  Input:
*         pstkpdnv:struct read from NV
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_nv_version_1_1( KPD_NV_VERSION_1_1_T *pstkpdnv )
{
    KPD_PARSE_T           * pstparse;
    int                     i;
    unsigned int                  ulitem;
    KPD_DRIVER_MSG_ENUM     enmsg; 
    unsigned int                 ucdrvvalue;
	/*传入组合键SD卡升级上报时间和NV配置信息*/
    kpd_parse_mux_init(pstkpdnv->ulkeyupdatetime,pstkpdnv->ulitemtablePwrWPS);
    for( i = 0; i < (KPD_EVENT_MAX); i++ )
    {
        ulitem      = pstkpdnv->ulitemtable[ i ];
        ucdrvvalue  = (unsigned int )KPD_PARSE_ITEM_GET_DRV_VALUE( ulitem ); /* the key value is 1,2,3. convert is safely */
        enmsg       = (KPD_DRIVER_MSG_ENUM)ucdrvvalue;
        if( (KPD_DRIVER_MSG_INVALID < enmsg) && (enmsg < KPD_DRIVER_MSG_BUTT) )
        {
            /* -1 : 0 is KPD_DRIVER_MSG_INVALID, the valid value is 1,2,3 */
            pstparse = &s_stkpdparselist[ ucdrvvalue - 1 ];
            pstparse->enmsg         = enmsg;
            pstparse->ultickunit    = pstkpdnv->ultickunit;
            kpd_parse_item_to_list( ulitem, pstparse );

            spin_lock_init(&(pstparse->parse_lock));

        }
        else
        {
            /* invalid item, maybe reach tail, exit nv parse */
            if( 0x0 != ulitem )
            {
                break;
            }
            else
            {
                /* continue;*/
            }
        }
    }
}

/******************************************************************************
*  Function:  kpd_nv_read_work
*  Description: NV读取函数，读取快速开关机用到的两个NV的值
*  Called by  : kpd_nv_config_init
*  Input      : void *arg
*  Output     : None
*  Return     : None
*  Note       : 读取NV失败时默认快速开关机功能不使能
*  History    : 	1. Created by x00202188.(2012-06-25)
********************************************************************************/
void kpd_nv_read_work(struct work_struct *w)
{
#if defined(FEATURE_SHORT_ON_OFF)
	unsigned int ret = 0;
	int i = 0;
	s_shortOnOffEnable.ulEnable = SHORT_ON_OFF_DISABLE;
	memset((void*)(&s_shortOnOffConfig),0,sizeof(s_shortOnOffConfig));/*lint !e522*/

	/*读取快速开关机是否使能NV*/
	ret = NVM_Read( NV_SHORT_ON_OFF_ENABLE,  &s_shortOnOffEnable, sizeof(s_shortOnOffEnable) );
	if( NV_OK != ret  )
	{
		s_shortOnOffEnable.ulEnable = SHORT_ON_OFF_DISABLE;
		printk(KERN_ERR "\r\n[SHORT ON OFF] Keyboard read NV failed ID = 0x%x.,ret =%d，count=%d;\r\n", NV_SHORT_ON_OFF_ENABLE,ret,i);
		return;
	}
			
	/*读取快速开关机过放保护信息配置NV*/
	ret = NVM_Read( NV_SHORT_ON_OFF_CONFIG,  &s_shortOnOffConfig, sizeof(s_shortOnOffConfig) );
	if( NV_OK !=  ret )
	{
		s_shortOnOffEnable.ulEnable = SHORT_ON_OFF_DISABLE;
		printk(KERN_ERR "\r\n[SHORT ON OFF] Keyboard read NV failed ID = 0x%x.,ret =%d，count=%d;\r\n", NV_SHORT_ON_OFF_CONFIG,ret,i);
		return;
	}

	printk(KERN_ERR "\r\n[SHORT ON OFF] Keyboard read NV OK ID = %d;%d;%d;%d;%d;%d;%d;%d;\r\n", 
				s_shortOnOffEnable.ulEnable,s_shortOnOffConfig.ulPowerOffMaxTimes,s_shortOnOffConfig.ulMaxTime,
				s_shortOnOffConfig.ulVoltLevel1,s_shortOnOffConfig.ulVoltLevel2,
				s_shortOnOffConfig.ulRTCLevel1,s_shortOnOffConfig.ulRTCLevel2,s_shortOnOffConfig.ulRTCLevel3);

#endif

}

/******************************************************************************
*  Function:  kpd_nv_config_init
*  Description:  initialize the struct in NV,and convert it to inner array
*  Input:
*         null
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_nv_config_init(void)
{
#if (FEATURE_KEYBOARD == FEATURE_ON)

    if( NV_OK != DR_NV_Read( NV_KPD_CONFIG,  &s_stkpdnvconfig, sizeof(s_stkpdnvconfig) ) )
    {
        printk(KERN_ERR "\r\nError,Keyboard read NV failed ID = %d.\r\n", NV_KPD_CONFIG);
        return ;
    }
	
/*BEGIN: xuchao x00202188, Added for short boot, 2012-6-22*/
#if defined(FEATURE_SHORT_ON_OFF) 
	INIT_WORK(&nv_read_work,kpd_nv_read_work);
    schedule_work(&nv_read_work);
#endif
/*END  : xuchao x00202188, Added for short boot, 2012-6-22*/
	
    kpd_parse_list_init();
    kpd_parse_nv_version_1_1( (KPD_NV_VERSION_1_1_T *)&s_stkpdnvconfig );
    kpd_parse_wps_dblclick_init();
#else	
	
	s_stkpdnvconfig.ulversion = 0x00010000;
	s_stkpdnvconfig.ultickunit = 1000;
	s_stkpdnvconfig.ulkeyupdatetime = 2000;
    
	s_stkpdnvconfig.ulitemtable[0]=0x01008000;
	s_stkpdnvconfig.ulitemtable[1]=0x01048002;
    
	s_stkpdnvconfig.ulitemtable[2]=0x02008000;
	s_stkpdnvconfig.ulitemtable[3]=0x02028005;;
    
    s_stkpdnvconfig.ulitemtable[4]=0x03008000;
    s_stkpdnvconfig.ulitemtable[5]=0x03038005;
    
    s_stkpdnvconfig.ulitemtable[6]=0xffffffff;
    s_stkpdnvconfig.ulitemtable[7]=0xffffffff;
    s_stkpdnvconfig.ulitemtable[8]=0xffffffff;
    
    s_stkpdnvconfig.ulitemtable[9]=0xffffffff;
    s_stkpdnvconfig.ulitemtable[10]=0xffffffff;
    s_stkpdnvconfig.ulitemtable[11]=0xffffffff;
    kpd_parse_list_init();
    kpd_parse_nv_version_1_1( (KPD_NV_VERSION_1_1_T *)&s_stkpdnvconfig );

#endif
}


/******************************************************************************
*  Function:  kpd_parse_timer_isr
*  Description:  kpd_parse_timer_isr
*  Input:
*         iParam:inner keypad parse array addr
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/

void kpd_parse_timer_isr( unsigned int  iParam )
{
    KPD_PARSE_T *pstparse = (KPD_PARSE_T *)iParam;
    hikpd_enter();
    if( pstparse){
        kpd_parse_common_handler( pstparse->enmsg, KPD_DRIVER_EVENT_TIMEOUT);
    }
   hikpd_leave();
}

/*
KPD_DRIVER_EVENT_ENUM kpd_driver_status_get( KPD_DRIVER_MSG_ENUM enMsg )
{
    if( !is_kpd_power_key_support_gpio() && (KPD_DRIVER_MSG_POWER == enMsg) )
    {
        // for T0 board, need read PMU status to get power key status   
        return kpd_driver_power_status_get();
    }
    else
    {
       for T1 board, the power key connect to a GPIO and use common gpio driver to handle it.
        return kpd_driver_gpio_status_get( enMsg );
    }
}
*/

/******************************************************************************
*  Function:  kpd_short_on_config
*  Description: 假开机时处理一些恢复操作
*  Called by  : kpd_short_boot_check
*  Input      : None
*  Output     : None
*  Return     : None
*  Note       : 
*  History    : 	1. Created by x00202188.(2012-06-22)
********************************************************************************/
void kpd_short_on_config()
{
#if defined(FEATURE_SHORT_ON_OFF) 
	isSendMsg = true; // 恢复向app上报事件
	kpd_driver_gpio_status_set(KPD_DRIVER_MSG_WPS,false);    //打开WPS键中断
	kpd_driver_gpio_status_set(KPD_DRIVER_MSG_RESET,false); //打开Reset键中断
#if defined(FEATURE_SD_ENABLE)
	BSP_Short_On_MMC();                     //SD卡重新加载及SD卡中断打开
#endif
	schedule_work(&nv_read_work);       //假开机时更新按键NV
#endif
}

/******************************************************************************
*  Function:  kpd_short_off_config
*  Description: 假关机时处理一些善后操作
*  Called by  : kpd_short_boot_check
*  Input      : None
*  Output     : None
*  Return     : None
*  Note       : 
*  History    : 	1. Created by x00202188.(2012-06-22)
********************************************************************************/
void kpd_short_off_config()
{
#if defined(FEATURE_SHORT_ON_OFF) 
	isSendMsg = false; // 屏蔽向app上报的事件
	kpd_driver_gpio_status_set(KPD_DRIVER_MSG_WPS,true);   //屏蔽WPS键中断
	kpd_driver_gpio_status_set(KPD_DRIVER_MSG_RESET,true);//屏蔽Reset键中断
#if defined(FEATURE_SD_ENABLE)
	BSP_Short_Off_MMC();                    //SD卡卸载及SD卡中断屏蔽
#endif
#endif
}

/******************************************************************************
*  Function:  kpd_short_boot_check
*  Description: 判断当前是否支持假关机，若支持则需根据情况上报假开机还是假关机
*  Called by  : kpd_server_new_msg
*  Input      : None
*  Output     : unsigned int  *	: 按键上报值
*  Return     : int : true:假关机或者假开机 false:真关机
*  Note       : 根据NV、过放保护参数以及当前所处状态，确定在关机时上报真关机还是假关机
*  History    : 	1. Created by x00202188.(2012-06-22)
********************************************************************************/
bool kpd_short_boot_check(unsigned int  *ulvirtualkey)
{
	
#if defined(FEATURE_SHORT_ON_OFF) 

	if( SHORT_ON_OFF_DISABLE == s_shortOnOffEnable.ulEnable)
	{  
	    /*未使能快速开关机功能，直接返回*/	
		printk(KERN_ERR "[SHORT ON OFF] short on off feature is disabled!\r\n");	
	    return false;
	}

    /*通过读取标志位来判断当前是应该上报假开机还是假关机*/
	if( SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode())
	{
	    /*满足假开机条件，清空标志位后，上报假开机事件给APP*/
		if((0 == PWRCTRL_SetShortOffMode(false)) && (0 == balong_rtc_alarm_clear()))
	    {
	    	kpd_short_on_config();
	    	*ulvirtualkey = KEY_SHORT_POWER_ON;
			return true;
	    }
		else
		{
		    /*清空假关机标志位失败，上报真关机*/
			return false;
		}
	}
	else
	{
		/*更新快速开关机历史累计运行时间*/
        s_currentShortOnOffStatus.ulTotalTime = (unsigned int)(get_current_working_time() / 3600);

        /*一段时间内连续关机，第二次为真关机*/
        if( 0 == s_lastShortOffTime )
        {
            s_lastShortOffTime = get_current_working_time(); //记录第一次假关机时间
		}
        else
        {
            if( KPD_SHORT_OFF_INTERVAL_TIME > (get_current_working_time() - s_lastShortOffTime)  )
            {
                /*在一段时间内执行过一次假关机，此次执行真关机*/
                printk(KERN_ERR "[SHORT ON OFF] short on off interval is too short!\r\n");
                printk(KERN_ERR "[SHORT ON OFF] s_lastShortOffTime=%lu,currentShortOffTime=%lu\r\n",
                    s_lastShortOffTime, get_current_working_time());
                return false;
            }
            else
            {
                /*两次假关机之间的间隔满足要求，可以假关机*/
                s_lastShortOffTime = get_current_working_time(); /*记录这一次假关机时间*/
            }
        }
		
	    if((s_currentShortOnOffStatus.ulTotalTime >= s_shortOnOffConfig.ulMaxTime)
		    ||(s_currentShortOnOffStatus.ulShortOffTimes >= s_shortOnOffConfig.ulPowerOffMaxTimes))
	    {
	        /*如果快速开关机次数达到最大值，或者假关机历史累加时间达到最大值，直接返回*/	
			printk(KERN_ERR "[SHORT ON OFF] short on off times or history total time is overtaking the default values!\r\n");	       
			printk(KERN_ERR "[SHORT ON OFF] NV_ulMaxTime=%d,ulTotalTime=%d,NV_ulPowerOffMaxTimes=%d,ulShortOffTimes=%d\r\n",
				s_shortOnOffConfig.ulMaxTime,s_currentShortOnOffStatus.ulTotalTime,s_shortOnOffConfig.ulPowerOffMaxTimes,s_currentShortOnOffStatus.ulShortOffTimes);	
			return false;
	    }
		
		/*满足假关机条件，上报假关机事件给APP*/
		printk(KERN_ERR "[SHORT ON OFF] NV_ulMaxTime=%d,ulTotalTime=%d,NV_ulPowerOffMaxTimes=%d,ulShortOffTimes=%d\r\n",
			s_shortOnOffConfig.ulMaxTime,s_currentShortOnOffStatus.ulTotalTime,s_shortOnOffConfig.ulPowerOffMaxTimes,s_currentShortOnOffStatus.ulShortOffTimes);	

		kpd_short_off_config();
		*ulvirtualkey = KEY_SHORT_POWER_OFF;
		return true;
	}
	
#else

	return false;

#endif
}

void kpd_server_new_msg( unsigned int  ulvirtualkey, unsigned int  ulparam )
{
    int ret;
    int size;
#if (FEATURE_POWER_ON_OFF == FEATURE_ON)
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
    static struct work_struct kpd_power_key_handle_work;
    static uint8_t is_kpd_power_work_inited = 0;
    POWER_ON_STATUS_ENUM power_on_status = power_on_status_get();

    if (0 == is_kpd_power_work_inited && POWER_ON_STATUS_FINISH_CHARGE == power_on_status)
    {
        INIT_WORK(&kpd_power_key_handle_work, kpd_parse_power_key_handle);
        is_kpd_power_work_inited = 1;
    }
#endif /*(defined(CHIP_BB_6920...*/
#endif /*(FEATURE_POWER_ON_OFF == FEATURE_ON)*/

    size = sizeof(DEVICE_EVENT);
    printk(KERN_ERR " send vlvirtualkey %d\n",ulvirtualkey); 
    kpd_event->event_code = ulvirtualkey;   /*lint !e713*/
	
    if( KEY_POWER_OFF == ulvirtualkey)
    {
#if (FEATURE_POWER_ON_OFF == FEATURE_ON)
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
        /*TODO: Should we flush work somewhere.???*/
        /*BSP_CHG_GetBatteryState can't be called at TIMER/ISR context, so schedule a work.*/
        /*Only scheudule a work in power-off charge mode to handle power-key, in case of 
          realtime problem, since usb charger detecting also schdule a work with system workqueue.*/
        if (POWER_ON_STATUS_FINISH_CHARGE == power_on_status)
        {
            schedule_work(&kpd_power_key_handle_work);
        }
        else
        {
        	/*BEGIN: xuchao x00202188, Added for short boot, 2012-6-22*/
			if( true == kpd_short_boot_check(&ulvirtualkey))
			{
			    /*快速开关机分支,根据标志位决定上报假开机事件还是假关机事件*/
				kpd_event->event_code = ulvirtualkey;
				printk(KERN_ERR "[SHORT ON OFF] send vlvirtualkey %d canceled;send vlvirtualkey %d\n",KEY_POWER_OFF,ulvirtualkey); 
			}
			else
			{
			    /*真关机分支，关机过程中可能休眠，投反对票保证不休眠*/
			    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_APP);
			}
			/*END  : xuchao x00202188, Added for short boot, 2012-6-22*/
			
			ret = device_event_report(kpd_event, size);
            if (ret)
            {
            	BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_APP);
                pr_err("%s: Send power key event to app failed\n", __FUNCTION__);
            }

            /*Send the Shutdown request to C-CPU, in case of APP power-off failed.*/
            if (is_current_firmware_factory)
            {
                pr_info("%s: Factory release: Send power-off request to C-CPU\n", __FUNCTION__);
                drvShutdown( DRV_SHUTDOWN_POWER_KEY );
            }
        }
#else /*!6920 OR !E5*/
        ret = device_event_report(kpd_event, size);
        if (-1 == ret)
        {
            printk(KERN_ERR "device_event_init: can't add event\n");
        }

		drvShutdown( DRV_SHUTDOWN_POWER_KEY );
#endif /*(defined(CHIP_BB_6920...*/
#endif
	 
     }
    else
    {
    
#if defined(FEATURE_SHORT_ON_OFF) 
    	/*假关机之后不上报除开机事件之外的事件*/
    	if( false == isSendMsg )
    	{
    		PWRCTRL_GetShortOffMode();//仅打印log，调试定位用
    		return;
    	}
#endif 

#if defined (FEATURE_LCD_ON)
		BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
        ret = device_event_report(kpd_event, size);
        if (-1 == ret)
        {
#if defined (FEATURE_LCD_ON)
			BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_LCD);
#endif
			printk(KERN_ERR "device_event_init: can't add event\n");
        }

    }
    printk("+++++++++++++++++++++++++++++++++++send vlvirtualkey end \n");

}


/******************************************************************************
*  Function:  kpd_parse_press_state_handle
*  Description:  kpd_parse_press_state_handle
*  Input:
*         pstparse:inner keypad parse array addr
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_press_state_handle( KPD_PARSE_T *pstparse )
{
    KPD_PARSE_ITEM_T * pstitem = NULL;   
    unsigned int             i = 0;/*pstparse->ulCurrentItemPos;*/
    unsigned int  ret = 0;
    hikpd_enter();
    
    pstitem = &pstparse->stitemlist[i];
    //if(pstparse->htimer)
    while( i < pstparse->ulitemnum )
    {
/*BEGIN Modified by xuchao for mmi 2012-02-21*/
#if ( FEATURE_MMI_TEST == FEATURE_ON )
        if ( true == mmi_test_mode )
        {
            /*mmi测试模式下不上报按键*/
            printk( KERN_INFO " %s: break while!!!\r\n ", __func__ );
            break;
        }
#endif	
/*END   Modified by xuchao for mmi 2012-02-21*/

        if( KPD_PARSE_ITEM_FLAG_PRESS == pstitem->ultickstartorflag )
        {
            if( pstparse->ulcurrenttick == pstitem->ultickend )
            {           
                /* Match, send the msg to client */
			   //组合键被按下时，单个按键事件不上报
               if(KPD_DRIVER_MSG_POWER == pstparse->enmsg )
               {
                   if(KPD_DRIVER_EVENT_PRESS !=  kpd_driver_status_get(KPD_DRIVER_MSG_WPS))
                   {

                       /*BEGIN PN:N/A,Modified by c00191475, 2013/01/08*/
                       /*Power键和Reset键的互斥，由应用保证，底层不再处理*/                      
                       kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 ); 
                       /*END   PN:N/A,Modified by c00191475, 2013/01/08*/
                   }
#if defined(FEATURE_SHORT_ON_OFF) 
				   //假关机之后，即使组合键被按下也要上报开机事件
				   else if(SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode()
				   	         && KEY_POWER_OFF == pstitem->ulvirtualkeyvalue)
				   {
				   	   kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
				   }
#endif
               }
			   else if(KPD_DRIVER_MSG_WPS == pstparse->enmsg)
			   {
			       if(KPD_DRIVER_EVENT_PRESS !=  kpd_driver_status_get(KPD_DRIVER_MSG_POWER))
                   {
                       kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
                   }
			   }
#if (FEATURE_KEYBOARD == FEATURE_ON)
				else if((KPD_DRIVER_MSG_RESET== pstparse->enmsg )&&(KEY_FACTORY_RESTORE == pstitem->ulvirtualkeyvalue))
				{
                    /*BEGIN PN:N/A,Modified by c00191475, 2013/01/08*/
                    /*Power键和Reset键的互斥，由应用保证，底层不再处理*/  
					BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_APP);
					kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
			        /*END   PN:N/A,Modified by c00191475, 2013/01/08*/			
				}
#endif				
               else
               {
                    kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
               }
            }
        }
        pstitem++;
        i++;
    }

    /* Last step, restart the timer */
    //kpd_timer_create( &(pstparse->htimer), pstparse->ultickunit, kpd_parse_timer_isr, (unsigned long)pstparse );
    memset(&(pstparse->htimer_softtimer),0,sizeof(HTIMER));
	/*lint -e64*/
    ret = SOFTTIMER_CREATE_TIMER(&(pstparse->htimer_softtimer), pstparse->ultickunit, \
                            SOFTTIMER_NO_LOOP,kpd_parse_timer_isr,(unsigned long)pstparse);
    if(SOFTTIMER_OK != ret)
    {
        printk(KERN_ERR "create htimer_softtimer failed\n");    
    }
	/*lint +e64*/

    hikpd_leave();
    
}


/******************************************************************************
*  Function:  kpd_parse_release_state_handle
*  Description:  kpd_parse_release_state_handle
*  Input:
*         pstparse:inner keypad parse array addr
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_release_state_handle( KPD_PARSE_T *pstparse )
{
    KPD_PARSE_ITEM_T * pstitem = NULL;
    unsigned int             i = 0;/*pstparse->ulCurrentItemPos;*/
    hikpd_enter();
    
    pstitem     = &pstparse->stitemlist[i];
    hikpd_trace(1,"=+++=============kpd_parse_release_state_handlepstparse->ulitemnum %dpstparse->enmsg=%d" ,pstparse->ulitemnum,pstparse->enmsg);
    while( i < pstparse->ulitemnum )
    {
        hikpd_trace(5,"=pstparse->ulitemnum%d ====pstitem->ultickstartorflag %x  pstitem->ultickend %x ",pstparse->ulitemnum,pstitem->ultickstartorflag,pstitem->ultickend);

/*BEGIN Modified by xuchao for mmi 2012-02-21*/
#if ( FEATURE_MMI_TEST == FEATURE_ON ) 
        if ( true == mmi_test_mode )
        {
            /*mmi测试模式下不上报按键*/
            printk( KERN_INFO " %s: break while!!!\r\n ", __func__ );
            break;
        }		
#endif
/*END   Modified by xuchao for mmi 2012-02-21*/

        if( pstitem->ultickstartorflag < KPD_PARSE_ITEM_TICK_MAX )
        {   
            /* this item is release state */
            hikpd_trace(1,"++++++++++++++++++++++++++++++++++++++++//////////release ");
            if( (pstitem->ultickstartorflag  <= pstparse->ulcurrenttick) && (pstparse->ulcurrenttick < pstitem->ultickend) )
            {
                hikpd_trace(1,"+++++++++in while send mesg");
				
			   //组合键被按下时，单个按键事件不上报
               if(KPD_DRIVER_MSG_POWER == pstparse->enmsg )
               {
                   if(0 == wps_power_status)
                   {
                       kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
                   }
               }
			   else if(KPD_DRIVER_MSG_WPS == pstparse->enmsg)
			   {
			       if(0 == wps_power_status)
                   {
                       kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
                   }
			   }	
               else
               {
                    kpd_server_new_msg( pstitem->ulvirtualkeyvalue, 0 );
               }
			   
            }
        }
        pstitem++;
        i++;
        hikpd_trace(1,"in while i= %d",i);
    }
    
    hikpd_leave();
}


/******************************************************************************
*  Function:  kpd_parse_lookup
*  Description:  kpd_parse_lookup
*  Input:
*         enmsg:drv key enum
*  Output: 
*         None
*  Return:
*         inner keypad parse struct address
*  Note  : 
*         
*  History:     
*
********************************************************************************/
KPD_PARSE_T *kpd_parse_lookup( KPD_DRIVER_MSG_ENUM enmsg )
{
    int  i;
    
    for( i = 0; i < KPD_GPIO_MAX; i++ )
    {
        if( enmsg == s_stkpdparselist[i].enmsg )
        {
            return (KPD_PARSE_T *)&s_stkpdparselist[i];
        }
    }
    
    return NULL;
}


/******************************************************************************
*  Function:  kpd_parse_handle_one_event
*  Description:  kpd_parse_handle_one_event
*  Input:
*         pstparse:inner keypad parse array addr
*         enevent:event enum
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_handle_one_event( KPD_PARSE_T *pstparse, KPD_DRIVER_EVENT_ENUM  enevent )
{

/*BEGIN Modified by xuchao for mmi 2012-02-21*/
#if ( FEATURE_MMI_TEST == FEATURE_ON ) 
    unsigned int kpd_index = 0;
	unsigned int g_ultime =0;//记录两次reset键按下的时间差
#endif
/*END   Modified by xuchao for mmi 2012-02-21*/

    /* disable the timer before handle the event */
    balong_kpd_del_timer(&(pstparse->htimer_softtimer));

    hikpd_enter();
    switch( enevent )
    {
    case KPD_DRIVER_EVENT_PRESS:
        hikpd_trace(1, "KPD_DRIVER_EVENT_PRESS");
        pstparse->ulpressed         = 1;
        pstparse->ulcurrenttick    = 0;
        /*pstparse->ulCurrentItemPos = 0;*/
/*BEGIN Modified by xuchao for mmi 2012-02-21*/		
#if ( FEATURE_MMI_TEST == FEATURE_ON ) 		
        printk( KERN_INFO "%s: mmi_test_mode is %d!!!\r\n", __func__, mmi_test_mode );			 

		if ( true == mmi_test_mode )
        {  
            /*确定有按键按下*/
            switch ( pstparse->enmsg )
            {
                case KPD_DRIVER_MSG_POWER:
                    mmi_test_event.pwr_key_cnt++;
					break;		
                case KPD_DRIVER_MSG_WPS:
                    mmi_test_event.wps_key_cnt++;
					break;
                case KPD_DRIVER_MSG_RESET:
				{
					rstCurrentTime = jiffies;
					g_ultime = (long)rstCurrentTime - (long)rstLastTime ;/*lint !e732 */
			        g_ultime = g_ultime*1000 /HZ;
					
					rstLastTime = rstCurrentTime;
					mmi_test_event.rst_key_cnt++;
					break;
                }
		default:
			break;
            }
			
			if( (pstparse->enmsg == KPD_DRIVER_MSG_RESET) && (300 >= g_ultime) && (rstLastTime!=0) )
			{//两次reset键按下的间隔要大于300ms		
					printk( KERN_INFO "%s: press key too fast!!!!!!g_ultime=%d\r\n", __func__, g_ultime );			 
					break;
			}
            printk( KERN_INFO " key press !!! index: %d!!!\r\n ", pstparse->enmsg );			
            complete( &mmi_test_event.key_to_test );			
        }         
#endif  
/*END   Modified by xuchao for mmi 2012-02-21*/

        /* timer will be start after handle the event */
        kpd_parse_press_state_handle( pstparse );
		break;
    case KPD_DRIVER_EVENT_TIMEOUT:
        hikpd_trace(1, "KPD_DRIVER_EVENT_TIMEOUT");
        if( pstparse->ulcurrenttick < (KPD_PARSE_ITEM_TICK_MAX -1) )
        {
            pstparse->ulcurrenttick++; /* increate the tick count */
                
            /* timer will be start after handle the event */
            kpd_parse_press_state_handle( pstparse );
        }
        else
        {
            
            /* Why, maybe the nv item is wrong.   */
        }
		
/*BEGIN Modified by xuchao for mmi 2012-02-21*/
#if ( FEATURE_MMI_TEST == FEATURE_ON ) 		
        /*mmi测试过程中,如果按键按下超过5秒钟,则认为按键有问题，测试失败*/	
        if ( true == mmi_test_mode )
        {
            if ( ( pstparse->ulcurrenttick * pstparse->ultickunit ) > MMI_KEY_TIME_DEF )
            {
                mmi_test_event.key_timeout = true;
                printk( KERN_INFO " Key press overtime, key_timeout: %d!!!\r\n ", mmi_test_event.key_timeout );
                complete( &mmi_test_event.key_to_test );					
            }
        }
#endif		
/*END   Modified by xuchao for mmi 2012-02-21*/

		break;
    case KPD_DRIVER_EVENT_RELEASE:
        hikpd_trace(1, "KPD_DRIVER_EVENT_RELEASE");
        if( pstparse->ulpressed )
        {        
            pstparse->ulpressed      = 0;
            hikpd_trace(1, "kpd_parse_release_state_handle");
            kpd_parse_release_state_handle( pstparse );
        }
        pstparse->ulcurrenttick = 0;
		
/*BEGIN Modified by xuchao for mmi 2012-02-21*/
#if ( FEATURE_MMI_TEST == FEATURE_ON ) 
        /* 如果有按键抬起则清除该按键对应的超时标志 */
        if ( true == mmi_test_event.key_timeout )		
        {
            do 
            {
                if ( s_stkpdparselist[kpd_index].ulcurrenttick * s_stkpdparselist[kpd_index].ultickunit >= MMI_KEY_TIME_DEF )			
                {
                    break;
                }
            }while ( ++kpd_index < KPD_GPIO_MAX );
            if ( kpd_index >= KPD_GPIO_MAX )
            {
                mmi_test_event.key_timeout = false;
                printk( KERN_INFO " Key press release, key_timeout: %d!!!\r\n ", mmi_test_event.key_timeout );            
            }
        }
		break;
#endif		
/*END   Modified by xuchao for mmi 2012-02-21*/

    default:
        hikpd_trace(1, "defalut");
        /* invalid event, nothing need to do */
		break;
    }
    hikpd_leave();
}

/******************************************************************************
*  Function:  kpd_parse_common_handler
*  Description:  keypad parse entry
*  Input:
*         enmsg:keypad DRV value
*         enevent:event enum
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*
********************************************************************************/
void kpd_parse_common_handler( KPD_DRIVER_MSG_ENUM enmsg, KPD_DRIVER_EVENT_ENUM  enevent )
{
    unsigned long flags = 0;
    KPD_PARSE_T * pstparse = (KPD_PARSE_T *)kpd_parse_lookup( enmsg );
    hikpd_trace(1,"kpd_parse_common_handler enmsg =%d enevent = %d ",enmsg,enevent);
    
    if( NULL != pstparse )
    {  
       spin_lock_irqsave(&(pstparse->parse_lock),flags);/*lint !e26 !e515*/
       #if (FEATURE_KEYBOARD == FEATURE_ON)
#if defined(FEATURE_MENU_DISPLAY)
/* 在开启菜单模式的情况下，不需要上报WPS按键双击事件 */
#else
       //xcbao:check wps key double click within 1s
	   if((KPD_DRIVER_MSG_WPS == enmsg) &&(KPD_DRIVER_EVENT_PRESS == enevent) \
		  && (KPD_MUX_WPS_PWR != kpd_parse_mux_check()))
	   {

		  if(0 == s_ulwpsclickcnt)
		  {
             unsigned int ret = 0;
             memset(&s_wps_dblclick_timer_softtimer,0,sizeof(HTIMER));
             /*lint -e64 */
             ret = SOFTTIMER_CREATE_TIMER(&s_wps_dblclick_timer_softtimer, KPD_WPS_DBLCLICK_TIMER, \
                                    SOFTTIMER_NO_LOOP,kpd_parse_wps_dblclick_isr,0);
             if(SOFTTIMER_OK != ret)
            {
                printk(KERN_ERR "create s_wps_dblclick_timer_softtimer failed\n");    
            }
			/*lint +e64 */
			wpsLastTime = jiffies;
		  }
		  wpsCurrentTime = jiffies;
		  s_ulwpsclickcnt++;		
	   }
#endif
       #endif
       hikpd_trace(1,"pstparse = 0x%x",(unsigned int)pstparse);
	   
	   /*组合键按下状态统计*/
	   kpd_wps_pwr_status_handle(enmsg,  enevent);
	   kpd_parse_handle_one_event( pstparse,  enevent );

       hikpd_trace(1,"kpd_parse_handle_one_eventpstparse->enmsg =%d  ",pstparse->enmsg);

	   spin_unlock_irqrestore(&(pstparse->parse_lock),flags);
    }
    else
    {
        hikpd_trace(1,"Error keyboard read NV failed ID   ");
    }
    hikpd_leave();
    
}



