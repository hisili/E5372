/*
 * Driver for keys on GPIO lines capable of generating interrupts.
 *
 * Copyright 2005 Phil Blundell
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
 /*
   *this  is a transfering code 
   *
   */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/gpio_keys.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/mutex.h>
#include <linux/netlink.h>
#include "BSP.h"
#include "balong_kpd_driver_gpio.h"
#include "balong_kpd_parse.h"

#include <generated/FeatureConfig.h>

KPD_GPIO_STM_STRU  s_astkpdlist[ KPD_GPIO_MAX ];
KPD_GPIO_INFO_STRU  pstgpioinfoglb[ KPD_GPIO_MAX +1 ];

#define IOS_CTRL98_OFFSET       0x0988          /*设置复用片选和复位信号*/
#if defined(CHIP_BB_6920CS)
#define IOS_CTRL_WPS_KEY_OFFSET	0x880
#define IOS_CTRL_RST_KEY_OFFSET	0x888
#endif

#define CRG_REG_WRITE(reg, data)                BSP_REG_WRITE(V7R1_SC_VA_BASE, reg, data)
#define CRG_REG_READ(reg, result)               BSP_REG_READ(V7R1_SC_VA_BASE, reg, result)
#define CRG_REG_SETBITS(reg,pos,bits,val)       BSP_REG_SETBITS(V7R1_SC_VA_BASE, reg, pos, bits, val)
#define CRG_REG_GETBITS(reg,pos,bits)           BSP_REG_GETBITS(V7R1_SC_VA_BASE, reg, pos, bits)

DEVICE_EVENT *kpd_event;
static struct gpio_keys_button gpio_keys_buttons[] = {
#if ((defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES)||defined(CHIP_BB_6920CS)))) /*y00186965*/ /*sunjian:与芯片确认CS*/
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		{
			.code			= (int)KPD_DRIVER_MSG_WPS,
			/*.type			= EV_KEY,*/
			.gpio			= (BALONG_GPIO_5(0)),
			.active_low		= 1,
			.wakeup			= 0,
	 		.debounce_interval	= 50,
	 		.desc			= "Button 2",
		},

		{
			.code			= (int)KPD_DRIVER_MSG_RESET,
			/*.type			= EV_KEY,*/
			.gpio			= (BALONG_GPIO_5(1)),
			.active_low		= 1,
			.wakeup			= 0,
	 		.debounce_interval	= 50,
	 		.desc			= "Button 3",
		},
	#elif defined(CHIP_BB_6920CS)
		{
			.code			= (int)KPD_DRIVER_MSG_WPS,
			/*.type			= EV_KEY,*/
			.gpio			= (BALONG_GPIO_2(1)),
			.active_low		= 1,
			.wakeup			= 0,
	 		.debounce_interval	= 50,
	 		.desc			= "Button 2",
		},

		{
			.code			= (int)KPD_DRIVER_MSG_RESET,
			/*.type			= EV_KEY,*/
			.gpio			= (BALONG_GPIO_2(4)),
			.active_low		= 1,
			.wakeup			= 0,
	 		.debounce_interval	= 50,
	 		.desc			= "Button 3",
		},
	#else
		{
			.code			= (int)KPD_DRIVER_MSG_WPS,
			/*.type			= EV_KEY,*/
			.gpio			= (BALONG_GPIO_0(9)),
			.active_low		= 1,
			.wakeup			= 0,
	 		.debounce_interval	= 50,
	 		.desc			= "Button 2",
		},

		{
			.code			= (int)KPD_DRIVER_MSG_RESET,
		/*.type			= EV_KEY,*/
			.gpio			= (BALONG_GPIO_0(10)),
			.active_low		= 1,
			.wakeup			= 0,
	 		.debounce_interval	= 50,
	 		.desc			= "Button 3",
		},
	#endif
	/*--by pandong cs*/	
#else
	{
		.code			= (int)KPD_DRIVER_MSG_POWER,
		/*.type			= EV_KEY,*/
		.gpio			= (BALONG_GPIO_10(0)),
		.active_low		= 1,
		.wakeup			= 0,
		.debounce_interval	= 50,
		.desc			= "Button 1",
	},
	
	{
		.code			= (int)KPD_DRIVER_MSG_WPS,
		/*.type			= EV_KEY,*/
		.gpio			= (BALONG_GPIO_10(1)),
		.active_low		= 1,
		.wakeup			= 0,
 		.debounce_interval	= 50,
 		.desc			= "Button 2",
	},

	{
		.code			= (int)KPD_DRIVER_MSG_RESET,
		/*.type			= EV_KEY,*/
		.gpio			= (BALONG_GPIO_10(2)),
		.active_low		= 1,
		.wakeup			= 0,
 		.debounce_interval	= 50,
 		.desc			= "Button 3",
	},
#endif	

};

static struct gpio_keys_platform_data gpio_keys_platform_data = {
	.buttons	= gpio_keys_buttons,
    /*解决typeof参数类型不对的问题-- linux内核*/
    /*lint -e516 -e84 -e746 -e806*/	
	.nbuttons	= ARRAY_SIZE(gpio_keys_buttons),/*lint !e30*/
	/*lint +e516 +e84 +e746 +e806*/
	.rep		= 0,
};

static struct platform_device balong_gpio_keys_device = {
	.name	= "gpio-keys",
	.id	= -1,
	.dev 	= {
		.platform_data	= &gpio_keys_platform_data,
	}
};

void balong_kpd_del_timer(HTIMER *ptimer)
{
    if(NULL == ptimer)
        return;
    
    if(SOFTTIMER_NULL_PTR != *ptimer)
    {
        SOFTTIMER_DEL_TIMER(ptimer);
        memset(ptimer,0,sizeof(HTIMER));
    }
    
    return;
}
    
/*called when platform device be moved from the kernel*/
static int gpio_keys_remove(struct platform_device *pdev)
{
    int i;
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	device_init_wakeup(&pdev->dev, (bool)0);
	for (i = 0; i < pdata->nbuttons; i++) 
        {

	        free_irq(INT_GPIO_10, &(s_astkpdlist[i]));
            balong_kpd_del_timer(&(s_astkpdlist[i].ptimer_softtimer));
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
            gpio_free((unsigned int)(s_astkpdlist[i].pstgpioinfo->gpio));
			/*end*/
	    }
    return 0;
}

/*called when device first recognized by the driver*/
static int __devinit gpio_keys_probe(struct platform_device *pdev)
{
	struct gpio_keys_platform_data *pdata = pdev->dev.platform_data;
	int i=0;
	int error=0;
	int wakeup = 0;
    int size;

#if (FEATURE_KEYBOARD  == FEATURE_ON)
#if defined(CHIP_BB_6920ES)
	//gpio0_9~10复用设置
	CRG_REG_SETBITS(IOS_CTRL98_OFFSET, 21,1, 0);
	CRG_REG_SETBITS(IOS_CTRL98_OFFSET, 23,1, 0);

#endif
#endif

	memset(s_astkpdlist, 0, sizeof(s_astkpdlist));
	memset(pstgpioinfoglb, 0, sizeof(pstgpioinfoglb));

    #if 0
    /*++by pandong cs*/
    #if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
        ifc_pmu_init();
    #elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
        ifc_pmu_init();
    #endif    
    /*--by pandong cs*/
    #endif
	    
	/*initialize NV */
	kpd_nv_config_init();
	size = sizeof(DEVICE_EVENT);
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	kpd_event = (DEVICE_EVENT *)kmalloc((size_t)size, GFP_ATOMIC);
	/*end*/
	if (NULL == kpd_event)
    {
        goto fail1;
    }   
	kpd_event->device_id = DEVICE_ID_KEY;
	kpd_event->len = 0;
	/*initialize struct pstgpioinfoglb from pdata*/
	for(i=0;i<pdata->nbuttons;i++)
	{
		pstgpioinfoglb[i].enkey =(KPD_DRIVER_MSG_ENUM) pdata->buttons[i].code;
		pstgpioinfoglb[i].pparsecallback[0] = kpd_parse_common_handler;
		pstgpioinfoglb[i].pparsecallback[1] = kpd_parse_mux;
		pstgpioinfoglb[i].pparsecallback[2] = NULL;
		pstgpioinfoglb[i].desc = pdata->buttons[i].desc;
		/*pstgpioinfoglb[i].type = pdata->buttons[i].type;*/
		pstgpioinfoglb[i].gpio=pdata->buttons[i].gpio;
		pstgpioinfoglb[i].active_low = pdata->buttons[i].active_low;
		pstgpioinfoglb[i].wakeup = pdata->buttons[i].wakeup;    
		pstgpioinfoglb[i].uckeypresslevelishigh=pdata->buttons[i].wakeup;/*lint !e734 */
		pstgpioinfoglb[i].index=i;
		if(pstgpioinfoglb[i].wakeup == 1)
			wakeup=1;
				
	}
	device_init_wakeup(&pdev->dev, (bool)wakeup);
	pstgpioinfoglb[i].enkey = KPD_DRIVER_MSG_INVALID;
	pstgpioinfoglb[i].pparsecallback[0] = NULL;
	pstgpioinfoglb[i].pparsecallback[1] = NULL;
	pstgpioinfoglb[i].pparsecallback[2] = NULL;
	pstgpioinfoglb[i].desc=NULL;
	/*pstgpioinfoglb[i].type=0;*/
	pstgpioinfoglb[i].gpio=0;
	pstgpioinfoglb[i].active_low=0;
	pstgpioinfoglb[i].wakeup=0;
	pstgpioinfoglb[i].uckeypresslevelishigh=0;
	pstgpioinfoglb[i].index=i;		
	for(i=0;i<KPD_GPIO_MAX;i++)
	{
	    hikpd_trace(5,"pstgpioinfoglb->enkey => %d ",pstgpioinfoglb[i].enkey);
	    if(KPD_DRIVER_MSG_INVALID != pstgpioinfoglb[i].enkey)
		{
		    if(NULL != pstgpioinfoglb->pparsecallback)  /*lint !e663*/
			{
				s_astkpdlist[i].pstgpioinfo = &pstgpioinfoglb[i];
				spin_lock_init(&s_astkpdlist[i].disable_lock);
				/*function entry to init ever gpio*/
				error = kpd_driver_gpio_one_node_init(i);
			}
			else
			{
				KPD_ASSERT( 0);/*lint !e527*/
			}
		}
		else
		{
		   break;
		}
	}


    /*++by pandong cs*/
    #if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
        ifc_pmu_init();
    #elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
        ifc_pmu_init();
    #endif    
    /*--by pandong cs*/

	return 0;
 fail1:
        return error;
}
/*platform 驱动构架,设置了回调函数和属性*/
static struct platform_driver gpio_keys_device_driver = {
	.probe		= gpio_keys_probe, 
	.remove     = gpio_keys_remove,
	.driver		= {
		.name	= "gpio-keys",
		.owner	= THIS_MODULE,
		}
};
/*module init function called when module ismod*/
static int __init gpio_keys_init(void)
{
    
    int ret;
	ret = platform_device_register(&balong_gpio_keys_device);
	if(ret)
	{
	    goto failed1;
	}
	
    ret = platform_driver_register(&gpio_keys_device_driver);
    if(ret)
    {
	 platform_device_unregister(&balong_gpio_keys_device);
        goto failed2;        
    }
    return ret;
failed2:
        hikpd_trace(5,"failed to register platform driver");
        return ret;
failed1:
    hikpd_trace(5,"failed to register platform device");
    return ret;

    
        
}

static void __exit gpio_keys_exit(void)
{
	platform_driver_unregister(&gpio_keys_device_driver);
	platform_device_unregister(&balong_gpio_keys_device);
}
module_init(gpio_keys_init);
module_exit(gpio_keys_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lively");
MODULE_DESCRIPTION("Keyboard driver for CPU GPIOs");
MODULE_ALIAS("platform:balong_gpio-keys");
