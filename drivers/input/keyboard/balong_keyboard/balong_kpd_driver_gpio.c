/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2011, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: kpd_driver_gpio.c                                               */
/*                                                                           */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2010-09                                                             */
/*                                                                           */
/* Description: keyboard module GPIO key driver operation                    */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/
#include <linux/timer.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include "BSP.h"
#include <linux/workqueue.h>
#include "balong_kpd_parse.h"
#include "balong_kpd_driver_gpio.h"
#if ( FEATURE_MMI_TEST == FEATURE_ON )
#include "../../../../arch/arm/mach-balong/mmi.h"
#include <linux/jiffies.h>
#endif

int hikpdlevel = 0;

extern void balong_kpd_del_timer(HTIMER *ptimer);
/* gpio key deshaking statement machine  */
static void kpd_driver_gpio_stm(int index ,KPD_DRIVER_EVENT_ENUM envent);
KPD_DRIVER_EVENT_ENUM kpd_driver_gpio_status_get( KPD_DRIVER_MSG_ENUM enMsg );
static volatile KPD_DRIVER_EVENT_ENUM    s_enpdpowerstatus   = KPD_DRIVER_EVENT_RELEASE;  


/*****************************************************************************
   宏定义 
*****************************************************************************/
/*
    GPIO keyboard statemation (normal) define: 
    Idle --(press)--> deshaking --(timeout)--> pressed --(release)--> Idle.
     ^                   |
     |-----------<-------+

    If any error or invalid event, goto idle
*/
int is_kpd_power_key_support_gpio( void )
{
    /*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
	|| (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES)||defined(CHIP_BB_6920CS)))
    return 0;
#else
    return 1;
#endif
	 /*--by pandong cs*/ 
}

KPD_DRIVER_EVENT_ENUM kpd_driver_power_status_get( void )
{
    return  s_enpdpowerstatus;
}


/******************************************************************************
*  Function:  kpd_driver_status_get
*  Description: get the key status.
*  Input:
*         enMsg : the key that want read status.
*  Output: 
*         None
*  Return:
*         the key status( driver layer status ) , see KPD_DRIVER_EVENT_ENUM for more information
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
KPD_DRIVER_EVENT_ENUM kpd_driver_status_get( KPD_DRIVER_MSG_ENUM enMsg )
{
    /*if( !is_kpd_power_key_support_gpio() && (KPD_DRIVER_MSG_POWER == enMsg) )*/
    if( !is_kpd_power_key_support_gpio() && (KPD_DRIVER_MSG_POWER == enMsg) )
    {
        /* for T0 board, need read PMU status to get power key status   */
        return kpd_driver_power_status_get();
    }
    else
    {
        /* for T1 board, the power key connect to a GPIO and use common gpio driver to handle it. */
        return kpd_driver_gpio_status_get( enMsg );
    }
}

/******************************************************************************
*  Function:  kpd_driver_gpio_event_handler
	*  Description: Handle the driver event.
*  Input:
*         pstGpioInfo : gpio information
*         enMsg       : key message
*         enEvent     : key event.
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
void kpd_driver_gpio_event_handler(KPD_GPIO_INFO_STRU *pstgpioinfo, KPD_DRIVER_MSG_ENUM enmsg, KPD_DRIVER_EVENT_ENUM enevent )
{
    /*if( !is_drv_kpd_mmi_test_mode() )
      
      else{
              In MMI test mode. send the msg directly 
              kpd_server_new_msg( pstGpioInfo->enKey, enEvent );
         }
        printk("now in pparsecallback\n");

   */
	int i=0;
	hikpd_enter();
	/*if is a release than turn the trigger type to be failing*/
	if(enevent == KPD_DRIVER_EVENT_RELEASE)
	{
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
		gpio_int_trigger_set((unsigned int)(pstgpioinfo->gpio),IRQ_TYPE_EDGE_FALLING  );
		/*end*/
	}
	while( i < KPD_CALLBACK_MAX_NUM )
	{
	    if(NULL != pstgpioinfo->pparsecallback[i])
        {   
		    pstgpioinfo->pparsecallback[i]( enmsg, enevent );            
        }
		i++;
	}
	hikpd_leave();
     
}

/******************************************************************************
*  Function:  kpd_driver_gpio_status_get_raw
*  Description: read the GPIO register and get the key status.
*  Input:
*         pstGpioInfo : gpio information
*  Output: 
*         None
*  Return:
*         Key status
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
static KPD_DRIVER_EVENT_ENUM kpd_driver_gpio_status_get_raw(KPD_GPIO_INFO_STRU * pstgpioinfo )
{
    KPD_DRIVER_EVENT_ENUM kpd_event = KPD_DRIVER_EVENT_BUTT;
    int   uclevel = 0;
    int gpio = (pstgpioinfo->gpio);
    
    hikpd_enter();
    
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	uclevel = gpio_get_value((unsigned int)gpio);
	/*end*/
    
	if( pstgpioinfo->uckeypresslevelishigh)
	{
        /* high level for press. and low level for release */
        if(uclevel)
        { 
            hikpd_trace(1,"press event uclevel 1 pstgpioinfo->uckeypresslevelishigh 1");
            kpd_event = KPD_DRIVER_EVENT_PRESS;
        }
        else
        {
			hikpd_trace(1,"release event uclevel 0 pstgpioinfo->uckeypresslevelishigh 1");
            kpd_event = KPD_DRIVER_EVENT_RELEASE;
        }
	}
	else
    {
        /* low level for press. and high level for release */
        if( uclevel)
        {
			hikpd_trace(1,"release event uclevel 1 pstgpioinfo->uckeypresslevelishigh 0");
            kpd_event = KPD_DRIVER_EVENT_RELEASE;
        }
        else
        {
            hikpd_trace(1,"press event uclevel 0 pstgpioinfo->uckeypresslevelishigh 0");
            kpd_event = KPD_DRIVER_EVENT_PRESS;
        }
    }
    
    hikpd_leave();
    return kpd_event;
}


/******************************************************************************
*  Function:  kpd_driver_gpio_int_enable
*  Description: enable the gpio interrupt
*  Input:
*         pstGpioInfo : gpio information
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
void kpd_driver_gpio_int_enable(  KPD_GPIO_INFO_STRU *pstgpioinfo )
{
	int gpio = pstgpioinfo->gpio;
	hikpd_enter();
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    gpio_int_unmask_set( (unsigned int)gpio);
	/*end*/
    hikpd_trace(1,"unmask gpio %d", gpio);
    hikpd_leave();
}

/******************************************************************************
*  Function:  kpd_driver_gpio_int_disable
*  Description: disable the gpio interrupt
*  Input:
*         pstGpioInfo : gpio information
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
void kpd_driver_gpio_int_disable(  KPD_GPIO_INFO_STRU *pstgpioinfo )
{
    int gpio = pstgpioinfo->gpio;
    hikpd_enter();
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    gpio_int_mask_set( (unsigned int)gpio );
	/*end*/
    hikpd_leave();
}

/******************************************************************************
*  F
unction:  kpd_driver_gpio_isr
*  Description: gpio interrupt handler
*  Input:
*         the index of gpio informaion array.
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
        s_astkpdlist[index].enevent = enevent;
       if((&(s_astkpdlist[index].work)) && detect_queue)
        {
            //schedule_work(&(s_astkpdlist[index].work));
            hikpd_trace(1, "starting queue work....");
            queue_work(detect_queue, &(s_astkpdlist[index].work));
        }
       else
       {
            hikpd_trace(1,"Null Pointer work!");
       }
       //gpio_int_unmask_set(gpio);
        
    }
    hikpd_trace(1,"gpio_int_unmask_set");
********************************************************************************/
irqreturn_t kpd_driver_gpio_isr(int irq , void *inds)
 {
    int index = 0;
    KPD_GPIO_STM_STRU  * pstkey= (KPD_GPIO_STM_STRU *)inds;
    unsigned  ucdata = 0;    
    KPD_GPIO_INFO_STRU  * pstgpioinfo = pstkey->pstgpioinfo;
	int gpio = pstgpioinfo->gpio;
	index = pstgpioinfo->index;
	hikpd_trace(1,"\n#####gpio:%d", gpio);
	printk(" code in gpio_jsr  gpio is %d \n",gpio);
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    gpio_int_state_get((unsigned int)gpio, &ucdata);
	/*end*/
    if (!ucdata){
	   hikpd_trace(1,"IRQ_NONE");
	   return IRQ_NONE;
	}
	 /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
     gpio_int_state_clear((unsigned int)gpio);  
	 /*end*/
	if( index < KPD_GPIO_MAX )
    {
        KPD_DRIVER_EVENT_ENUM   enevent = KPD_DRIVER_EVENT_INVALID;

        enevent = kpd_driver_gpio_status_get_raw( pstgpioinfo);
        kpd_driver_gpio_stm(index,enevent);
          hikpd_trace(1,"return IRQ_HANDLED ");
        return IRQ_HANDLED;
     
    }
    else
    {
        /*lint -e527*/
        KPD_ASSERT( 0 );
		/*lint +e527*/
	    return IRQ_NONE;
    }
 }


/******************************************************************************
*  Function:  kpd_driver_gpio_deshaking_timer_isr
*  Description: gpio deshaking timeout handler
*  Input:
*         the index of gpio informaion array.
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
void kpd_driver_gpio_deshaking_timer_isr( unsigned long index )
{
	hikpd_enter();
	hikpd_trace(1,"+++++++++++++now you are in kpd_driver_gpio_deshaking_timer_isr\n");
    if( index < KPD_GPIO_MAX )
    {
        /*KPD_RECORD( "GPIO deshaking timer isr.", index, 0 );*/
        kpd_driver_gpio_stm( (int)index, KPD_DRIVER_EVENT_TIMEOUT );/*clean lint e713*/
    }
	hikpd_leave();
 }


void  kpd_general_parse(KPD_DRIVER_MSG_ENUM enmsg, KPD_DRIVER_EVENT_ENUM enevent )
{
	kpd_parse_common_handler(enmsg,enevent);
	kpd_parse_mux(enmsg,enevent);
}

BSP_U32 ifc_pmu_setarg(BSP_VOID * pMsgBody, BSP_U32  u32Len)
{
    BSP_U32  ulpmucmd = 0;
    ulpmucmd = BSP_REG((BSP_U32)pMsgBody, 0);
#ifndef FEATURE_SD_ENABLE
    /*没有SD卡的产品，采用双击power键进入MMI*/
    static unsigned long power_time = 0;
#endif

    if(1 == ulpmucmd)
    {
        printk("+++++++++++++++++ifc_pmu_setarg pmu is pressed \n");
#ifndef FEATURE_SD_ENABLE
    /*没有SD卡的产品，采用双击power键进入MMI*/
    if ( false == mmi_test_mode )
    {
        if ( ( jiffies - power_time ) < msecs_to_jiffies( MMI_POWER_TIMER ) )
        {
            printk("##############################################BEFORE MMI TEST KEY START!!!!");
            complete( &mmi_test_event.mmi_test_start );
            printk("##############################################AFTER MMI TEST KEY START!!!!");
            power_time = 0;
        }
        else
        { 
            power_time = jiffies;
        }
    }
#endif
		BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_KEY);
		s_enpdpowerstatus= KPD_DRIVER_EVENT_PRESS;
		kpd_general_parse(KPD_DRIVER_MSG_POWER, KPD_DRIVER_EVENT_PRESS);
	}
	else if(0 == ulpmucmd)
	{
		printk("+++++++++++++++ifc_pmu_setarg pmu is released \n");
		BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_KEY);
		s_enpdpowerstatus= KPD_DRIVER_EVENT_RELEASE;
		kpd_general_parse(KPD_DRIVER_MSG_POWER, KPD_DRIVER_EVENT_RELEASE);
	}
	else
	{
		printk("recevied the wrong value ");
		return ulpmucmd;
	}
	return 0;
}

 BSP_U32 ifc_pmu_init(BSP_VOID)
{
    BSP_U32 ret = 0;
	ret = BSP_IFCP_RegFunc(IFCP_FUNC_PMU_ID ,(BSP_IFCP_REG_FUNC)ifc_pmu_setarg);
	if(0 != ret) 
	{
        printk("Failed to recive message  ifc_pmu_setarg\n");
		return ret;
    }

    return 0;
}



/******************************************************************************
*  Function:  kpd_driver_gpio_one_node_init
*  Description: init one gpio node.
*  Input:
*         the index of gpio informaion array.
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
 int  kpd_driver_gpio_one_node_init( int index )
{
    int error=0;
	int level=0;
	int ret =0;
    KPD_GPIO_INFO_STRU  * pstgpioinfo = s_astkpdlist[ index ].pstgpioinfo;
	int gpio=pstgpioinfo->gpio;
	s_astkpdlist[ index ].enstate = KPD_GPIO_STATE_IDLE;
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	error = gpio_request((unsigned int)gpio, pstgpioinfo->desc);
	/*end*/
	if(error){
		goto fail1;
	}
    
/*ONLY Take affect on V7: CLEAR and MASK gpio interrupt before register.*/
#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	gpio_int_mask_set((unsigned int)gpio);
	gpio_int_state_clear((unsigned int)gpio);
	/*end*/
#endif /*(defined(CHIP_BB_...*/

	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	gpio_direction_input((unsigned int)gpio); 
	gpio_set_function((unsigned int)gpio,GPIO_INTERRUPT);
	level = gpio_get_value((unsigned int)gpio);
	/*end*/
	if(level)
	{
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
		ret = gpio_int_trigger_set((unsigned int)gpio, IRQ_TYPE_EDGE_FALLING);
		/*end*/		
	}
	else
	{
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
			ret = gpio_int_trigger_set((unsigned int)gpio,IRQ_TYPE_EDGE_RISING);
			/*end*/	
	}
	error = request_irq(INT_GPIO_10, (irq_handler_t) kpd_driver_gpio_isr,IRQF_SHARED ,pstgpioinfo->desc,&(s_astkpdlist[index]));
	if (error) {
			goto fail2;
	}
	printk("+++++++++++++++++++++request irq success irq is %d gpio num is %d \n",INT_GPIO_10,gpio);
	    hikpd_trace(1," request_irq successful ");
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
        gpio_int_state_clear((unsigned int)gpio);
	    gpio_int_unmask_set((unsigned int)gpio);
		/*end*/
        return 0;

fail2:
    hikpd_error(" request irq failed ");
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	gpio_free((unsigned int)gpio);
	/*end*/
fail1:
    hikpd_error("request gpio  failed \n gpio = %d \n",gpio);
	return error;
}/*lint !e550*/

/******************************************************************************
*  Function:  kpd_driver_gpio_status_set
*  Description: 假开机或假关机时屏蔽或打开WPS键和Reset键的gpio中断
*  Called by:   
*  Input: item  :
*  Output:None
*  Return:None
*  Note  :      
*  History:     1. Created by x00202188.(2012-07-03)
********************************************************************************/
void kpd_driver_gpio_status_set(KPD_DRIVER_MSG_ENUM key,bool isMask)
{
	KPD_GPIO_INFO_STRU  * pstgpioinfo = NULL;
	int gpio = 0;
	int i = 0;

	/*屏蔽或打开WPS键和Reset键的gpio中断*/
	for(i=0;i<KPD_GPIO_MAX;i++)
	{
	 	if(key == pstgpioinfoglb[i].enkey)
		{
			pstgpioinfo = s_astkpdlist[ i ].pstgpioinfo;
			gpio=pstgpioinfo->gpio;

			if( true == isMask )
			{
				gpio_int_mask_set((unsigned int)gpio);
				gpio_int_state_clear((unsigned int)gpio);
			}
			else
			{
				gpio_int_state_clear((unsigned int)gpio);
	    		gpio_int_unmask_set((unsigned int)gpio);
			}
	 	}
	}

	
}

/******************************************************************************
*  Function:  kpd_driver_gpio_goto_idle
*  Description: reset the GPIO stm to idle state
*  Input:
*         pstKey ; the gpio key pointer.
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
static void kpd_driver_gpio_goto_idle( KPD_GPIO_STM_STRU * pstkey )
{
   /* KPD_RECORD( "GPIO stm goto idle.", 0, 0 );*/
    hikpd_enter();
    pstkey->enstate = KPD_GPIO_STATE_IDLE;
	/* enable the interrupt */
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    gpio_int_unmask_set((unsigned int)(pstkey->pstgpioinfo->gpio));
	/*end*/
    balong_kpd_del_timer(&(pstkey->ptimer_softtimer));
    hikpd_leave();
}


/******************************************************************************
*  Function:  kpd_driver_gpio_stm_deshaking_handler
*  Description: GPIO key stm.
*  Input:
*         index : the index of key array;
*         eEvent : key event
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
static void kpd_driver_gpio_stm_deshaking_handler( int index, KPD_DRIVER_EVENT_ENUM enevent )
{
    KPD_GPIO_STM_STRU        * pstkey      = &s_astkpdlist[ index ];  /* iIndex wouldn't overflow */
    KPD_GPIO_INFO_STRU * pstgpioinfo = pstkey->pstgpioinfo;
	    int gpio=pstgpioinfo->gpio;
    if( KPD_DRIVER_EVENT_TIMEOUT == enevent )
    {
        KPD_DRIVER_EVENT_ENUM  enkeystatus = KPD_DRIVER_EVENT_INVALID;

        /* deshaking finished, stop the timer and enable the interrupt again */

       balong_kpd_del_timer(&(pstkey->ptimer_softtimer));

       kpd_driver_gpio_int_enable( pstgpioinfo );
       

        enkeystatus = kpd_driver_gpio_status_get_raw( pstgpioinfo );
        printk("deshaking fun kpd_driver_gpio_status_get_raw == %d\n",enkeystatus);
        
        if( KPD_DRIVER_EVENT_PRESS == enkeystatus ) 
        {
            /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
             gpio_int_trigger_set((unsigned int)gpio,IRQ_TYPE_EDGE_RISING);   
			/*end*/
            /* Yeah, it is a TRUE press. start press handle and send msg.*/
            pstkey->enstate = KPD_GPIO_STATE_PRESSED;
             

            /* the pCallback has been checked when kpd init */
            hikpd_trace(1,"kpd_driver_gpio_event_handler");
            kpd_driver_gpio_event_handler( pstgpioinfo, pstgpioinfo->enkey, KPD_DRIVER_EVENT_PRESS );
            printk("the ture press returned here \n");
            
        }
        else
        {
            /* It is a shaking, ignore it, goto idle */
            pstkey->enstate       = KPD_GPIO_STATE_IDLE;
        }
    }
    else
    {
        /*the gpio interrupt be disabled, shouldn't receiver other event */
        hikpd_trace(1,"///////////////////////////////////////the gpio interrupt bu diabled KPD_ASSERT(0) ");
        kpd_driver_gpio_goto_idle(pstkey );
        /*lint -e527*/
        KPD_ASSERT(0);
        /*lint +e527*/
        
    }    
}

/******************************************************************************
*  Function:  kpd_driver_gpio_stm
*  Description: GPIO key stm.
*  Input:
*         iIndex : the index of key array;
*   *         eEvent : key event
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/
static void kpd_driver_gpio_stm(int index,KPD_DRIVER_EVENT_ENUM enevent)
{
   
    unsigned long flags = 0 ;
    KPD_GPIO_STM_STRU * pstkey = &s_astkpdlist[index];
    KPD_GPIO_INFO_STRU * pstgpioinfo = pstkey->pstgpioinfo;
    hikpd_enter();
    hikpd_trace(1, "++++++++++++++++++++++++++++++++kpd_driver_gpio_stm index = %d \nenstate = %d \nenenvent = %d ",index,pstkey->enstate,enevent);
    /* avoid the deshaking / gpio event conflict */
    spin_lock_irqsave(&(s_astkpdlist[index].disable_lock), flags);/*lint !e26 !e515*/
    
    switch( pstkey->enstate )
    {
    case KPD_GPIO_STATE_IDLE:
        if( KPD_DRIVER_EVENT_PRESS == enevent ) 
        {   
            unsigned int ret = 0;
            kpd_driver_gpio_int_disable( pstgpioinfo ); /* 避免中断太多 */
            /* start the deshaking timer */
            memset(&(s_astkpdlist[index].ptimer_softtimer),0,sizeof(HTIMER));
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 5) (int to unsigned int))*/
			/*lint -e64*/
            ret = SOFTTIMER_CREATE_TIMER(&(s_astkpdlist[index].ptimer_softtimer), KPD_GPIO_DESHAKING_TIME, \
                                        SOFTTIMER_NO_LOOP,kpd_driver_gpio_deshaking_timer_isr,(unsigned int)index);
			if(SOFTTIMER_OK != ret)
            {
                printk(KERN_ERR "create ptimer_softtimer failed\n");    
                kpd_driver_gpio_int_enable( pstgpioinfo );
                spin_unlock_irqrestore(&(s_astkpdlist[index].disable_lock),flags);
                return;
            }
			/*lint +e64*/
			/*end*/
            hikpd_trace(1,"pstkey->enstate = KPD_GPIO_STATE_DESHAKING ");
            pstkey->enstate = KPD_GPIO_STATE_DESHAKING;
        }
        else
        {
            /* Why, maybe the interrupt be lost, send a release key to parse */
            kpd_driver_gpio_event_handler( pstgpioinfo, pstgpioinfo->enkey, KPD_DRIVER_EVENT_RELEASE );
        }
        break;
    case KPD_GPIO_STATE_DESHAKING:              /* Because the gpio int be disabled, so, only timeout event */    
        hikpd_trace(1,"KPD_GPIO_STATE_DESHAKING");
        kpd_driver_gpio_stm_deshaking_handler( index, enevent );
        break;
    case KPD_GPIO_STATE_PRESSED:       /* After deshaking, make sure the key is be pressed */
        hikpd_trace(1,"KPD_GPIO_STATE_PRESSED");
        if( KPD_DRIVER_EVENT_RELEASE == enevent )
        {   
            /* the pCallback has been checked when kpd init */
            hikpd_trace(1,"KPD_DRIVER_EVENT_RELEASE");
            kpd_driver_gpio_event_handler( pstgpioinfo, pstgpioinfo->enkey, KPD_DRIVER_EVENT_RELEASE );
        }
        else
        {
            /* maybe the interrupt be lost, send a release msg first */
            hikpd_trace(1,"enevent:%d", enevent);
            kpd_driver_gpio_event_handler( pstgpioinfo, pstgpioinfo->enkey, KPD_DRIVER_EVENT_RELEASE );
        }

        kpd_driver_gpio_goto_idle( pstkey );
        break;
    default:
    /* Invalid state, wouldn't reach here */
        hikpd_trace(1," ///////////////////////////////////////////////KPD_ASSERTdefault");
        KPD_ASSERT( 0 );/*lint !e527*/
        kpd_driver_gpio_goto_idle( pstkey );/*lint !e527*/
        break;
    }
    spin_unlock_irqrestore(&(s_astkpdlist[index].disable_lock),flags);
    hikpd_trace(1,"+++++++++++++++++++++++++++++++++++++++++++++++stm leave");
    hikpd_leave();
}
/*****************************************************************************
*  Function:  kpd_driver_lookup
*  Description: get the index by the key value
*  Input:
*         eMsg : key value
*  Output: 
*         None
*  Return:
*         Index
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
*******************************************************************************/

static int kpd_driver_lookup( KPD_DRIVER_MSG_ENUM enmsg )
{
    int  i = 0;

    for( i = 0; i < KPD_GPIO_MAX - 1; i++ )
    {
        if( enmsg == s_astkpdlist[i].pstgpioinfo->enkey )
        {
            return i;
        }
    }
    KPD_ASSERT(0);/*lint !e527*/
    return -1;/*lint !e527*/
}

/*****************************************************************************
*  Function:  kpd_driver_gpio_status_get
*  Description: get gpio key status.
*  Input:
*         eMsg : key value
*  Output: 
*         None
*  Return:
*         status;
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
********************************************************************************/

KPD_DRIVER_EVENT_ENUM kpd_driver_gpio_status_get( KPD_DRIVER_MSG_ENUM enmsg )
{
    int iindex;

    iindex = kpd_driver_lookup( enmsg );
   if( iindex >= 0 )
    {
        return kpd_driver_gpio_status_get_raw( s_astkpdlist[iindex].pstgpioinfo );
    }
    else
    {
        return KPD_DRIVER_EVENT_INVALID;
    }
}
/*****************************************************************************
*  Function:  kpd_driver_gpio_init
*  Description: init the driver(gpio) module.
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         
*  History:     
*               1. Created by l00131505.(2010-09-13)
*******************************************************************************/


