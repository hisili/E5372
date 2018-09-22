	/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2011, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: kpd_driver.c                                                    */
/*                                                                           */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2010-09                                                             */
/*                                                                           */
/* Description: keyboard module driver operation  (driver layer )            */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Modification: Create this file                                         */
/* 2. Date:                                                                  */
/*                  将组合键修改为NV可配置，并且组合键时不上报单个事件 */
/*****************************************************************************/

#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include "balong_kpd_driver_gpio.h"
#include "balong_kpd_parse.h"
#include "../../../../arch/arm/mach-balong/mmi.h"
#include <mach/powerExchange.h>
#include <mach/common/bsp_memory.h>
#include <mach/platform.h>
#if defined(FEATURE_SHORT_ON_OFF) 
#include "../../../../arch/arm/mach-balong/include/mach/pwrctrl/BSP_PWC_SLEEPMGR.h"
#endif

#include <linux/netlink.h>

void kpd_server_new_msg( unsigned int	ulvirtualkey, unsigned int	ulparam );

extern KPD_DRIVER_EVENT_ENUM kpd_driver_status_get( KPD_DRIVER_MSG_ENUM enmsg );
extern void balong_kpd_del_timer(HTIMER *ptimer);
extern void tftClearWholeScreen( void );
extern void tftStringDisplay(unsigned char ucX, unsigned char ucY, unsigned char *pucStr);
extern int hi_TFUP_setTFUpdateSource(int flag);

extern int hi_keyboard_respond(void);
extern int sd_get_status(void);

/*****************************************************************************
   宏定义 
*****************************************************************************/
/*
    kpd state define
*/

#define KPD_STATE_PRESSED   (1)     
#define KPD_TIMER1_MUX           (500)  /* 500ms */ 
struct spinlock   mux_lock; /*lint !e86*/

/*
   mux key parse stm information define( state & timer )
*/
static HTIMER            s_pmuxtimer_0_softtimer;   /*组合键定时器*/
static HTIMER            s_pmuxtimer_1_softtimer;   /*500ms定时器*/

static unsigned int   s_ulwpsintcnt=0; /* record the wps interrupt occur. release / press */
unsigned int g_ulforceupdatetime = 0;  /*WPS+POWER组合键,SD卡强制升级时间*/
unsigned int g_PowerWpsCurrentTime =0;/*记录组合键被按下的当前时间*/
bool g_isPressed = FALSE;              /*记录组合键是否被按下*/  
WPS_PWR_ITEM wpsPwrItem[KPD_WPS_PWR_EVENT_MAX];

/*
  extern global variable
*/
/*extern unsigned int g_ulPwrIntCnt ;*/

/*lint -e752
extern unsigned char g_mmisdflag;
lint +e752*/
//extern BOOL sdmmcdrvInstalled;

/******************************************************************************
*  Function:    kpd_parse_mux_timer_isr
*  Description: 1秒定时器处理函数
*  Input:       emsg : int
*  Output:      None
*  Return:      None
*  Note  :      
*  History:     1. Created by l00131505.(2010-09-13)
*               2. Modified by x00202188.(2012-04-14)
********************************************************************************/
void kpd_parse_mux_timer_isr( int emsg )
{
    emsg = emsg;
	
    kpd_parse_mux( (KPD_DRIVER_MSG_ENUM)emsg, KPD_DRIVER_EVENT_TIMEOUT );
}

/******************************************************************************
*  Function:  kpd_parse_mux_check
*  Description: WPS key kpd_parse_mux_check stm.
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
 KPD_MUX_ENUM kpd_parse_mux_check( void)
{
    /*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined (BOARD_ASIC) && defined(CHIP_BB_6920ES)) /*y00186965*/ /*sunjian:与包先春确认else*/

#if (FEATURE_KEYBOARD == FEATURE_ON)
	if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) &&  (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER)))
	{
		return KPD_MUX_WPS_PWR;
	}
	else
	{
        	return KPD_MUX_NULL;		
	}
#else
	if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)))
    {
        return KPD_MUX_WPS_RST;
    } 
	else
	{
        return KPD_MUX_NULL;
    }
#endif	

#else
	/*在配置文件统一屏蔽lint -e960 */
    if( (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER))
		&& (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)))
    {
       return KPD_MUX_PWR_WPS_RST;
    }
    else if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) &&  (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER)))
    {    /*目前对其他组合键不作处理*/
        return KPD_MUX_WPS_PWR;
    } 
    else if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)))
    {    /*目前对其他组合键不作处理*/
        return KPD_MUX_WPS_RST;
    } 
    else if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER)))
    {    /*目前对其他组合键不作处理*/
        return KPD_MUX_PWR_RST;
    } 
    else
    {
        return KPD_MUX_NULL;
    }
	/*在配置文件统一屏蔽llint +e960 */
#endif

}


/******************************************************************************
*  Function:  kpd_parse_wps_timer_500ms_isr
*  Description: kpd_parse_wps_timer_500ms_isr
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
void kpd_parse_wps_timer_500ms_isr( KPD_MUX_ENUM emsg )
{
    int i =0;
    balong_kpd_del_timer(&s_pmuxtimer_1_softtimer);

    if(s_ulwpsintcnt >= 2)
    {
		/*和其他产品保持一致，SD卡升级不在屏幕显示提示信息*/

#if ((FEATURE_POWER_ON_OFF == FEATURE_ON ) && (FEATURE_MMI_TEST == FEATURE_ON))
		#if (FEATURE_KEYBOARD == FEATURE_ON)
		
		/*同时满足:非MMI测试模式、正常开机模式且SD开在位时方可进行SD卡升级*/
		if((mmi_test_mode == false)&&(POWER_ON_STATUS_FINISH_NORMAL == power_on_status_get())&&(0 == sd_mmc_get_status()))
       	{
		    printk(KERN_ERR "################### UPDATE #############################\r\n");
			hi_TFUP_setTFUpdateSource(true);
			hi_keyboard_respond();
		}
		#endif
#endif
    }
    else
    {
		
		/*组合键没有达到升级事件上报要求时，上报抬起事件中
		 *结束时间恰好等于升级事件上报时间的事件
		*/
		for(i=0; i<KPD_WPS_PWR_EVENT_MAX; i++)
        {
            if( wpsPwrItem[i].enmsg != emsg )
	        {
	            break;
	        }
			
            if( g_ulforceupdatetime == wpsPwrItem[i].PwrWpsTimeEnd )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### SD卡升级未触发 #### keyvalue=%d，enmsg=%d #######\r\n",
				wpsPwrItem[i].virtualkeyvalue, wpsPwrItem[i].enmsg);
            }
        }
    }
}


/******************************************************************************
*  Function:  kpd_wps_pwr_event_report
*  Description: 根据配置信息上报组合键事件
*  Called by:   kpd_wps_pwr_parse_handle
*               kpd_wps_pwr_release_handle
*               kpd_wps_pwr_timeout_handle
*  Input: item  : emsg   :KPD_MUX_ENUM
*                 enevent:KPD_DRIVER_EVENT_ENUM
*  Output:None
*  Return:None
*  Note  :      
*  History:     1. Created by x00202188.(2012-04-14)
********************************************************************************/
void kpd_wps_pwr_event_report( KPD_MUX_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent)
{
    int i =0;
    for(i=0; i<KPD_WPS_PWR_EVENT_MAX; i++)
    {
        if( wpsPwrItem[i].enmsg != emsg )
	    {
	        continue;
	    }
		
		/*按下时被上报*/
		if( enevent == KPD_DRIVER_EVENT_PRESS && KPD_PARSE_ITEM_TICK_MAX == wpsPwrItem[i].PwrWpsTime )
		{
		    if( wpsPwrItem[i].PwrWpsTimeEnd == g_PowerWpsCurrentTime )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### 组合键按下 #### g_PowerWpsCurrentTime=%d ,virtualkeyvalue=%d #######\r\n",
				g_PowerWpsCurrentTime,wpsPwrItem[i].virtualkeyvalue);
            }
		}
		/*time out时被上报*/
		else if( enevent == KPD_DRIVER_EVENT_TIMEOUT && KPD_PARSE_ITEM_TICK_MAX == wpsPwrItem[i].PwrWpsTime )
		{
		    if( wpsPwrItem[i].PwrWpsTimeEnd == g_PowerWpsCurrentTime )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### 组合键time out #### g_PowerWpsCurrentTime=%d ,virtualkeyvalue=%d  #######\r\n",
				g_PowerWpsCurrentTime,wpsPwrItem[i].virtualkeyvalue);
            }
		}
		/*抬起时被上报*/
		else if( enevent == KPD_DRIVER_EVENT_RELEASE && KPD_PARSE_ITEM_TICK_MAX != wpsPwrItem[i].PwrWpsTime )
		{
		    if( (wpsPwrItem[i].PwrWpsTime <= g_PowerWpsCurrentTime) && (g_PowerWpsCurrentTime < wpsPwrItem[i].PwrWpsTimeEnd) )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### 组合键抬起 #### g_PowerWpsCurrentTime=%d ,virtualkeyvalue=%d  #######\r\n",
				g_PowerWpsCurrentTime,wpsPwrItem[i].virtualkeyvalue);
            }
		}
    }
}

/******************************************************************************
*  Function:  kpd_wps_pwr_item_convert
*  Description: 转换NV配置，并初始化内部变量
*  Called by:   kpd_parse_mux_init
*  Input: item  : unsigned int *
*  Output:None
*  Return:None
*  Note  :      
*  History:     1. Created by x00202188.(2012-04-14)
********************************************************************************/
void kpd_wps_pwr_item_convert(unsigned int * item)
{
	int i = 0;
	
	memset(wpsPwrItem, 0, sizeof(wpsPwrItem));
	
	for(i=0; i < KPD_WPS_PWR_EVENT_MAX; i++)
	{
	    wpsPwrItem[i].enmsg           = KPD_PARSE_ITEM_GET_DRV_VALUE(item[i]);/*lint !e64*/
		wpsPwrItem[i].virtualkeyvalue = KPD_PARSE_ITEM_GET_VIRTUAL_VALUE( item[i] );
		wpsPwrItem[i].PwrWpsTime      = KPD_PARSE_ITEM_GET_TIME_START( item[i] );
		wpsPwrItem[i].PwrWpsTimeEnd   = KPD_PARSE_ITEM_GET_TIME_END( item[i] );

		if( KPD_MUX_NULL >= wpsPwrItem[i].enmsg || KPD_MUX_BUTT <= wpsPwrItem[i].enmsg )
	    {
	        break;
	    }
	}
}


/******************************************************************************
*  Function:  kpd_wps_pwr_release_handle
*  Description: 按下事件处理函数，当组合键中第二个按键被按下时调用本函数
*  Called by:   kpd_parse_mux
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :      仅在组合键刚被按下时执行一次
*  History:     1. Created by x00202188.(2012-04-14)
********************************************************************************/
void kpd_wps_pwr_parse_handle( KPD_MUX_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent )
{
	
    unsigned int ret = 0;

	if ( TRUE == g_isPressed )
	{
	    return;
	}
	
	g_PowerWpsCurrentTime = 0;
	g_isPressed = TRUE;
	
	kpd_wps_pwr_event_report(emsg,enevent);
	
	memset(&s_pmuxtimer_0_softtimer,0,sizeof(HTIMER));
	/*lint -e64*/
    ret = SOFTTIMER_CREATE_TIMER(&s_pmuxtimer_0_softtimer, 1000, \
                                    SOFTTIMER_NO_LOOP,kpd_parse_mux_timer_isr,emsg);
	if(SOFTTIMER_OK != ret)
    {
        printk(KERN_ERR "create s_pmuxtimer_0_softtimer  failed\n");    
    } 
	/*lint +e64*/
}

/******************************************************************************
*  Function:  kpd_wps_pwr_release_handle
*  Description: 抬起事件处理函数，当组合键中其中一个按键被抬起时调用本函数
*  Called by:   kpd_parse_mux
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :      抬起时也可上报事件 
*  History:     1. Created by x00202188.(2012-04-14)
********************************************************************************/
void kpd_wps_pwr_release_handle( KPD_MUX_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent )
{
    
	
	if( FALSE == g_isPressed )
	{
	    return;
	}
    balong_kpd_del_timer(&s_pmuxtimer_0_softtimer);

    kpd_wps_pwr_event_report(emsg,enevent);

	g_isPressed = FALSE;
}

/******************************************************************************
*  Function:  kpd_wps_pwr_timeout_handle
*  Description: 超时事件处理函数，当组合键被按下后，1秒钟被执行一次检测是否达到
*               事件上报要求，负责事件上报
*  Called by:   kpd_parse_mux
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :      按下后被一秒定时器一秒执行一次   
*  History:     1. Created by x00202188.(2012-04-14)
********************************************************************************/
void kpd_wps_pwr_timeout_handle( KPD_MUX_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent )
{	
	unsigned int ret = 0;
    
	
	g_PowerWpsCurrentTime ++;

	/*组合键被按下时间超过最大值*/
	if( KPD_PARSE_ITEM_TICK_MAX <= g_PowerWpsCurrentTime )
	{
		return;
	}

    /*SD卡升级事件处理*/
    if(g_PowerWpsCurrentTime == g_ulforceupdatetime)
	{ 
		s_ulwpsintcnt = 0;
		printk(KERN_ERR "################### Press WPS #############################\r\n");

		/*和其他产品保持一致，SD卡升级不在屏幕显示提示信息*/

        memset(&s_pmuxtimer_1_softtimer,0,sizeof(HTIMER));
		/*lint -e64*/
        ret = SOFTTIMER_CREATE_TIMER(&s_pmuxtimer_1_softtimer, KPD_TIMER1_MUX, \
                                    SOFTTIMER_NO_LOOP,kpd_parse_wps_timer_500ms_isr,emsg);  
        if(SOFTTIMER_OK != ret)
        {
            printk(KERN_ERR "create s_pmuxtimer_1_softtimer failed\n");    
        }        
	} 

	/*非升级事件处理*/
	kpd_wps_pwr_event_report(emsg,enevent);
	
	memset(&s_pmuxtimer_0_softtimer,0,sizeof(HTIMER));
    ret = SOFTTIMER_CREATE_TIMER(&s_pmuxtimer_0_softtimer, 1000, \
                                    SOFTTIMER_NO_LOOP,kpd_parse_mux_timer_isr,emsg);	
    if(SOFTTIMER_OK != ret)
    {
        printk(KERN_ERR "create s_pmuxtimer_0_softtimer failed\n");    
    }            
	/*lint +e64*/
}

/******************************************************************************
*  Function:  kpd_parse_mux
*  Description: 组合键中断处理函数,目前仅支持WPS+POWER
*  Called by:   kpd_parse_mux_timer_isr,kpd_general_parse
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :         
*  History:     1. Created by l00131505.(2010-09-13)
*               2. Modified by x00202188.(2012-04-14)
********************************************************************************/
void kpd_parse_mux( KPD_DRIVER_MSG_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent )
{
	unsigned long flags =0 ;
	KPD_MUX_ENUM imuxkeystate = KPD_MUX_NULL;
    emsg = emsg; /* avoid compiler warning */

#if defined(FEATURE_SHORT_ON_OFF) 
	/*假关机之后，不处理组合键*/
	if(SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode())
	{
	    return;
	}
#endif

	if( KPD_DRIVER_MSG_WPS == emsg )
	{/*500ms定时器中检测WPS被按下次数用*/
        s_ulwpsintcnt++;
    }
	imuxkeystate = kpd_parse_mux_check();
	spin_lock_irqsave(&mux_lock, flags);/*lint !e26 !e515*/

    switch(imuxkeystate)
	{
	    case KPD_MUX_WPS_PWR:
			/*case:组合键是按下状态*/
			
			if( KPD_DRIVER_EVENT_TIMEOUT == enevent )
			{/*1秒定时器发来的超时事件*/
			    kpd_wps_pwr_timeout_handle(imuxkeystate, enevent);
			}
			else
			{/*按键被按下时的中断事件*/
			    kpd_wps_pwr_parse_handle(imuxkeystate, enevent);			
			}
			break;
		default:
			/*case:组合键中有按键未按下*/
			if ( KPD_DRIVER_MSG_POWER == emsg || KPD_DRIVER_MSG_WPS == emsg )
			{
			    kpd_wps_pwr_release_handle(KPD_MUX_WPS_PWR, enevent);
			}
			break;
	}
	spin_unlock_irqrestore(&mux_lock,flags);
}

/******************************************************************************
*  Function:  kpd_parse_mux_init
*  Description: 组合键初始化函数
*  Input:  ulforceUpdateTime: key force update time     
*  Output: None
*  Return: None
*  Note  :       
*  History:     
********************************************************************************/
void kpd_parse_mux_init( unsigned int ulforceupdatetime ,unsigned int *ulitem)
{

    spin_lock_init(&mux_lock);
    g_ulforceupdatetime = ulforceupdatetime/1000;
	kpd_wps_pwr_item_convert(ulitem);

}