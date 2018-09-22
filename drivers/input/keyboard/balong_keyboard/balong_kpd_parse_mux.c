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
/*                  ����ϼ��޸�ΪNV�����ã�������ϼ�ʱ���ϱ������¼� */
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
   �궨�� 
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
static HTIMER            s_pmuxtimer_0_softtimer;   /*��ϼ���ʱ��*/
static HTIMER            s_pmuxtimer_1_softtimer;   /*500ms��ʱ��*/

static unsigned int   s_ulwpsintcnt=0; /* record the wps interrupt occur. release / press */
unsigned int g_ulforceupdatetime = 0;  /*WPS+POWER��ϼ�,SD��ǿ������ʱ��*/
unsigned int g_PowerWpsCurrentTime =0;/*��¼��ϼ������µĵ�ǰʱ��*/
bool g_isPressed = FALSE;              /*��¼��ϼ��Ƿ񱻰���*/  
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
*  Description: 1�붨ʱ��������
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
    || (defined (BOARD_ASIC) && defined(CHIP_BB_6920ES)) /*y00186965*/ /*sunjian:����ȴ�ȷ��else*/

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
	/*�������ļ�ͳһ����lint -e960 */
    if( (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER))
		&& (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)))
    {
       return KPD_MUX_PWR_WPS_RST;
    }
    else if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) &&  (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER)))
    {    /*Ŀǰ��������ϼ���������*/
        return KPD_MUX_WPS_PWR;
    } 
    else if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_WPS)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)))
    {    /*Ŀǰ��������ϼ���������*/
        return KPD_MUX_WPS_RST;
    } 
    else if((KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_RESET)) && (KPD_DRIVER_EVENT_PRESS == kpd_driver_status_get(KPD_DRIVER_MSG_POWER)))
    {    /*Ŀǰ��������ϼ���������*/
        return KPD_MUX_PWR_RST;
    } 
    else
    {
        return KPD_MUX_NULL;
    }
	/*�������ļ�ͳһ����llint +e960 */
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
		/*��������Ʒ����һ�£�SD������������Ļ��ʾ��ʾ��Ϣ*/

#if ((FEATURE_POWER_ON_OFF == FEATURE_ON ) && (FEATURE_MMI_TEST == FEATURE_ON))
		#if (FEATURE_KEYBOARD == FEATURE_ON)
		
		/*ͬʱ����:��MMI����ģʽ����������ģʽ��SD����λʱ���ɽ���SD������*/
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
		
		/*��ϼ�û�дﵽ�����¼��ϱ�Ҫ��ʱ���ϱ�̧���¼���
		 *����ʱ��ǡ�õ��������¼��ϱ�ʱ����¼�
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
	            printk(KERN_ERR "######### SD������δ���� #### keyvalue=%d��enmsg=%d #######\r\n",
				wpsPwrItem[i].virtualkeyvalue, wpsPwrItem[i].enmsg);
            }
        }
    }
}


/******************************************************************************
*  Function:  kpd_wps_pwr_event_report
*  Description: ����������Ϣ�ϱ���ϼ��¼�
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
		
		/*����ʱ���ϱ�*/
		if( enevent == KPD_DRIVER_EVENT_PRESS && KPD_PARSE_ITEM_TICK_MAX == wpsPwrItem[i].PwrWpsTime )
		{
		    if( wpsPwrItem[i].PwrWpsTimeEnd == g_PowerWpsCurrentTime )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### ��ϼ����� #### g_PowerWpsCurrentTime=%d ,virtualkeyvalue=%d #######\r\n",
				g_PowerWpsCurrentTime,wpsPwrItem[i].virtualkeyvalue);
            }
		}
		/*time outʱ���ϱ�*/
		else if( enevent == KPD_DRIVER_EVENT_TIMEOUT && KPD_PARSE_ITEM_TICK_MAX == wpsPwrItem[i].PwrWpsTime )
		{
		    if( wpsPwrItem[i].PwrWpsTimeEnd == g_PowerWpsCurrentTime )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### ��ϼ�time out #### g_PowerWpsCurrentTime=%d ,virtualkeyvalue=%d  #######\r\n",
				g_PowerWpsCurrentTime,wpsPwrItem[i].virtualkeyvalue);
            }
		}
		/*̧��ʱ���ϱ�*/
		else if( enevent == KPD_DRIVER_EVENT_RELEASE && KPD_PARSE_ITEM_TICK_MAX != wpsPwrItem[i].PwrWpsTime )
		{
		    if( (wpsPwrItem[i].PwrWpsTime <= g_PowerWpsCurrentTime) && (g_PowerWpsCurrentTime < wpsPwrItem[i].PwrWpsTimeEnd) )
            { 
                kpd_server_new_msg( wpsPwrItem[i].virtualkeyvalue, 0 );
	            printk(KERN_ERR "######### ��ϼ�̧�� #### g_PowerWpsCurrentTime=%d ,virtualkeyvalue=%d  #######\r\n",
				g_PowerWpsCurrentTime,wpsPwrItem[i].virtualkeyvalue);
            }
		}
    }
}

/******************************************************************************
*  Function:  kpd_wps_pwr_item_convert
*  Description: ת��NV���ã�����ʼ���ڲ�����
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
*  Description: �����¼�������������ϼ��еڶ�������������ʱ���ñ�����
*  Called by:   kpd_parse_mux
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :      ������ϼ��ձ�����ʱִ��һ��
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
*  Description: ̧���¼�������������ϼ�������һ��������̧��ʱ���ñ�����
*  Called by:   kpd_parse_mux
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :      ̧��ʱҲ���ϱ��¼� 
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
*  Description: ��ʱ�¼�������������ϼ������º�1���ӱ�ִ��һ�μ���Ƿ�ﵽ
*               �¼��ϱ�Ҫ�󣬸����¼��ϱ�
*  Called by:   kpd_parse_mux
*  Input: eMsg      : key value
*         enumEvent : key event
*  Output:None
*  Return:None
*  Note  :      ���º�һ�붨ʱ��һ��ִ��һ��   
*  History:     1. Created by x00202188.(2012-04-14)
********************************************************************************/
void kpd_wps_pwr_timeout_handle( KPD_MUX_ENUM emsg, KPD_DRIVER_EVENT_ENUM  enevent )
{	
	unsigned int ret = 0;
    
	
	g_PowerWpsCurrentTime ++;

	/*��ϼ�������ʱ�䳬�����ֵ*/
	if( KPD_PARSE_ITEM_TICK_MAX <= g_PowerWpsCurrentTime )
	{
		return;
	}

    /*SD�������¼�����*/
    if(g_PowerWpsCurrentTime == g_ulforceupdatetime)
	{ 
		s_ulwpsintcnt = 0;
		printk(KERN_ERR "################### Press WPS #############################\r\n");

		/*��������Ʒ����һ�£�SD������������Ļ��ʾ��ʾ��Ϣ*/

        memset(&s_pmuxtimer_1_softtimer,0,sizeof(HTIMER));
		/*lint -e64*/
        ret = SOFTTIMER_CREATE_TIMER(&s_pmuxtimer_1_softtimer, KPD_TIMER1_MUX, \
                                    SOFTTIMER_NO_LOOP,kpd_parse_wps_timer_500ms_isr,emsg);  
        if(SOFTTIMER_OK != ret)
        {
            printk(KERN_ERR "create s_pmuxtimer_1_softtimer failed\n");    
        }        
	} 

	/*�������¼�����*/
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
*  Description: ��ϼ��жϴ�����,Ŀǰ��֧��WPS+POWER
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
	/*�ٹػ�֮�󣬲�������ϼ�*/
	if(SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode())
	{
	    return;
	}
#endif

	if( KPD_DRIVER_MSG_WPS == emsg )
	{/*500ms��ʱ���м��WPS�����´�����*/
        s_ulwpsintcnt++;
    }
	imuxkeystate = kpd_parse_mux_check();
	spin_lock_irqsave(&mux_lock, flags);/*lint !e26 !e515*/

    switch(imuxkeystate)
	{
	    case KPD_MUX_WPS_PWR:
			/*case:��ϼ��ǰ���״̬*/
			
			if( KPD_DRIVER_EVENT_TIMEOUT == enevent )
			{/*1�붨ʱ�������ĳ�ʱ�¼�*/
			    kpd_wps_pwr_timeout_handle(imuxkeystate, enevent);
			}
			else
			{/*����������ʱ���ж��¼�*/
			    kpd_wps_pwr_parse_handle(imuxkeystate, enevent);			
			}
			break;
		default:
			/*case:��ϼ����а���δ����*/
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
*  Description: ��ϼ���ʼ������
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