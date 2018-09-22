/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcPeriMgr.c
* Description:
*                peri managerment
*
* Author:        ������
* Date:          2011-09-28
* Version:       1.0
*
*
*
* History:
* Author:		������
* Date:			2011-09-28
* Description:	Initial version
*
*******************************************************************************/
/*V3R2 CS f00164371*/
/*#include "generated/FeatureConfigDRV.h"*/
#include "generated/FeatureConfig.h"

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/system.h>
#include <linux/string.h>
#if(FEATURE_HSIC_SLAVE == FEATURE_ON)
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <mach/pwrctrl/BSP_PWC_COMMON.h>  
#include <mach/pwrctrl/BSP_PWC_PERIMGR.h>  
#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h> 
#include <mach/DrvInterface.h>             
#endif

#include "DrvInterface.h"

//#include <mach/balong_v100r001.h>
//#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
//#include <mach/pwrctrl/BSP_PWC_COMMON.h>
//#include <mach/pwrctrl/BSP_PWC_PERIMGR.h>
//#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h>

void pwrctrl_acpu_peri_func(void)
{

}


/*****************************************************************************
 Prototype      : BSP_PWRCTRL_UsbLowPowerEnter
 Description    : Usb lowPower enter
 Input          : wu
 Output         : wu
 Return Value   : wu 
 Calls          :
 Called By
*****************************************************************************/
void BSP_PWRCTRL_UsbLowPowerEnter(void)
{
    /*Stub @2012-3-23 for V7R1*/
}

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#else
unsigned long  BSP_SDMMC_ATProcess(unsigned int ulOp,
                unsigned long ulAddr,  unsigned long ulData,unsigned char *pucBuffer,unsigned long *pulErr)
{
   return 0;
}
EXPORT_SYMBOL(BSP_SDMMC_ATProcess);
#endif

EXPORT_SYMBOL(BSP_PWRCTRL_UsbLowPowerEnter);

#if(FEATURE_HSIC_SLAVE == FEATURE_ON)

s8 g_hsic_pwc_print_flag = FALSE;

#define PRINT_PWC_DBG(fmt, args...)  \
        do { \
        if (TRUE == g_hsic_pwc_print_flag) \
            { \
                printk(fmt, ## args) ; \
            } \
        } while (0)

void hsic_pwc_print_flag_set(s8 val)
{
    g_hsic_pwc_print_flag = val;
}

extern int gpio_value_set(UINT32 ucId, UINT32 ucMask, UINT32 ucData);
extern unsigned int usb_enum_completed(void);
/*extern unsigned int hsic_enum_completed(void);*/
extern int gpio_direction_output(unsigned gpio, int value);
extern int gpio_chip_value_get(unsigned int chip_num, unsigned int pin,int* value);

extern uint32_t hsic_enum_completed(void);
extern void hsic_enum_status_set(uint32_t ulEnumStatus);
extern int g_usb_current_ftm_mode;
/*modified by 00212112, 120411*/

extern void Modem_enable_hsic(int	iEnable);
extern int hsic_reg_init_for_lpd(void);
extern void hsic_bus_enable_by_pwrctl(void);
/*ends*/

HSIC_PWC_STATE hsic_pwc_state;
//s8 g_hsic_modem_wakeup = FALSE;

struct work_struct slave_wakeup_falling_q,slave_wakeup_rising_q;
struct work_struct host_active_q;
static struct workqueue_struct *workqueue;

u8 host_active_edge_flag= IRQ_TYPE_EDGE_FALLING;

//u8 hsic_suspend_before_enum = FALSE;

void hsic_pwc_gpio_init(void);
void hsic_pwc_init(void);
s8 slave_wakeup_rising_int_install(void);
irqreturn_t slave_wakeup_rising_isr(int irq, void *dev_id);
s8 slave_wakeup_rising_int_handler(struct work_struct *work);
s8 slave_wakeup_falling_int_install(void);
irqreturn_t slave_wakeup_falling_isr(int irq, void *dev_id);
s8 slave_wakeup_falling_int_handler(struct work_struct *work);


/*****************************************************************************
name            : hsic_pwc_state_set
Description    :HSIC �͹���״̬�л�
Input          : state:�����õ���״̬
Return Value   : OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
s8 hsic_pwc_state_set (HSIC_STATE_TYPE new_state)
{
    if((HSIC_INIT_STATE >new_state)||(HSIC_MAX_STATE <= new_state))
    {
        printk("hsic_pwc_state_set error,HSIC state invalid.\n");
        hsic_pwc_state.current_state = hsic_pwc_state.old_state;
        return ERROR;
    }
    hsic_pwc_state.old_state = hsic_pwc_state.current_state;
    hsic_pwc_state.current_state = new_state;
    printk("hsic_pwc_state_set,HSIC state change:%d -> %d.\n",hsic_pwc_state.old_state,hsic_pwc_state.current_state);
    return OK;
}
//EXPORT_SYMBOL(hsic_pwc_state_set);/*lint !e578*//*lint !508*/


/*****************************************************************************
name            : hsic_pwc_state_get
Description    :HSIC
Input          : �޲�������ʹ��ȫ�ֱ���hsic_pwc_state
Return Value   : hsic current state
Calls          :
Called By      :
*****************************************************************************/
HSIC_STATE_TYPE hsic_pwc_state_get (void)
{
    return hsic_pwc_state.current_state;
}
//EXPORT_SYMBOL(hsic_pwc_state_get);/*lint !e578*/


/*****************************************************************************
name            : hsic_pwc_gpio_init
Description    : hsic �͹������GPIO�ڼ��ж�����
Input          : NU_NULL
Return Value   : N/A
Calls          :
Called By      :
*****************************************************************************/
void hsic_pwc_gpio_init(void)
{
	printk("hsic_pwc_gpio_init,start...\n");
    /*���ýӿ�GPIO_2_1,��ʼ��Ϊ����ߵ�ƽ����ӦHOST_WAKEUP�ź�*/
    #if 0
    (void)gpio_direction_output(HOST_WAKEUP,GPIO_HIGH);
    //(void)gpio_value_set((unsigned int)HOST_WAKEUP_GPIO, (unsigned int)(0x1 << HOST_WAKEUP_PIN), (unsigned int)(GPIO_HIGH << HOST_WAKEUP_PIN));

    /*���ýӿ�GPIO_2_4,��ʼ������ߵ�ƽ����ӦSUSPEND_REQUEST�ź�(��δʹ��)*/
    (void)gpio_direction_output(SUSPEND_REQUEST,GPIO_HIGH);
    //(void)gpio_value_set((unsigned int)SUSPEND_REQUEST_GPIO, (unsigned int)(0x1 << SUSPEND_REQUEST_PIN), (unsigned int)(GPIO_HIGH << SUSPEND_REQUEST_PIN));
    #endif
    
    /*ע�Ტʹ��slave_wakeup_rising�ж�,�����ش���,��ӦSLAVE_WAKEUP�ź�*/
    slave_wakeup_rising_int_install();
    /*ע�Ტʹ��slave_wakeup_falling�ж�,�½��ش���,��ӦSLAVE_WAKEUP�ź�*/
    slave_wakeup_falling_int_install();     
    printk("hsic_pwc_gpio_init,end.\n");   
}
//EXPORT_SYMBOL(hsic_pwc_gpio_init);/*lint !e578*/


/*****************************************************************************
name            : Hsic_pwc_init
Description    :HSIC�͹��ĳ�ʼ���ӿ�
Input          : NU_NULL
Output         : NU_NULL
Return Value   :  N/A
Calls          :
Called By      :
*****************************************************************************/
void hsic_pwc_init(void)
{
    hsic_pwc_gpio_init();
    hsic_pwc_state.current_state = HSIC_INIT_STATE;
    hsic_pwc_state.old_state = HSIC_INIT_STATE; 
	(void)BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_HSIC); 
	PWRCTRL_HSIC_CLEAR_SLEEP_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
    //g_hsic_modem_wakeup = FALSE;
}
//EXPORT_SYMBOL(hsic_pwc_init);/*lint !e578*/
#if defined (FEATURE_FLASH_LESS)
#else
module_init(hsic_pwc_init);/*lint !e64*//*lint !e19*//*lint !e528*/
#endif

/*****************************************************************************
name            : slave_wakeup_rising_int_install
Description    :slave wakeup�������ź��ж�ע��
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
s8 slave_wakeup_rising_int_install(void)
{
    /*ע�Ტʹ��slave_wakeup_rising,�������жϴ���,��ӦSLAVE_WAKEUP�ź�*/
    printk("slave_wakeup_rising_int_install,start...\n");
    
    gpio_int_mask_set(SLAVE_WAKEUP_RISING);
    
    if(gpio_request(SLAVE_WAKEUP_RISING, "slave_wakeup_rising_int"))
    {
    	printk("slave_wakeup_rising_int_install,the slave wakeup rising signal requesting is busy!\n");
    	return ERROR;
    }

    workqueue = create_workqueue("slave_wakeup_rising_queue");
    if (NULL == workqueue)
    {
    	printk("slave_wakeup_rising_int_install,slave_wakeup_rising_queue workqueue create error!\n");
    	return ERROR;
    }

    INIT_WORK(&slave_wakeup_rising_q, slave_wakeup_rising_int_handler);/*lint !e155*//*lint !e69*//*lint !e64*/
    if (request_irq(SLAVE_WAKE_R_INT,slave_wakeup_rising_isr,IRQF_SHARED,"slave_wakeup_rising_int_handler", &slave_wakeup_rising_q)) 
    {
        printk("slave_wakeup_rising_int_install,can't get slave wakeup rising irq.\n");
        return ERROR;
    }	
    
    gpio_direction_input(SLAVE_WAKEUP_RISING); 
    gpio_set_function(SLAVE_WAKEUP_RISING,GPIO_INTERRUPT);
    gpio_int_state_clear(SLAVE_WAKEUP_RISING);
    gpio_int_trigger_set(SLAVE_WAKEUP_RISING,IRQ_TYPE_EDGE_RISING);
    gpio_int_unmask_set(SLAVE_WAKEUP_RISING);
    
    printk("slave_wakeup_rising_int_install success.\n");

    return OK;

}


/*****************************************************************************
name            : slave_wakeup_rising_isr
Description    :slave_wake�ź��������жϴ����ϰ벿
Input          : int irq, void *dev_id
Return Value   :  IRQ_NONE / IRQ_HANDLED
Calls          :
Called By      :
*****************************************************************************/
irqreturn_t slave_wakeup_rising_isr(int irq, void *dev_id)
{
    unsigned int ucData = 0; 
    /*�ж��Ƿ�Ϊslave wakeup ������GPIO0_7���ж�*/
    gpio_int_chip_state_get(SLAVE_WAKEUP_RISING_GPIO, SLAVE_WAKEUP_RISING_PIN, (unsigned*)&ucData);
    if (!ucData)
    {
    	/*printk(KERN_DEBUG"not slave wakeup rising isr! \n");*/
    	return IRQ_NONE;
    }
    /*���жϣ����ж�*/
    gpio_int_chip_mask_set(SLAVE_WAKEUP_RISING_GPIO, SLAVE_WAKEUP_RISING_PIN);
    gpio_int_chip_state_clear(SLAVE_WAKEUP_RISING_GPIO, SLAVE_WAKEUP_RISING_PIN);

    queue_work(workqueue,&slave_wakeup_rising_q);

    /*printk(KERN_DEBUG"slave wakeup rising isr ok! \n");*/
    return IRQ_HANDLED;
}

/*****************************************************************************
name            : slave_wakeup_rising_int_handler
Description    :slave_wake�ź��������жϴ����°벿
Input          : struct work_struct *work
Return Value   :  N/A
Calls          :
Called By      :
*****************************************************************************/
s8 slave_wakeup_rising_int_handler(struct work_struct *work)
{
    HSIC_STATE_TYPE current_state;
    int gpio_value = 0;

    current_state = hsic_pwc_state_get();
    PRINT_PWC_DBG("slave_wakeup_rising_int_handler start..hsic_current_state = %d\n", current_state);	    
    

    if((HSIC_INIT_STATE > current_state)&&(HSIC_MAX_STATE <= current_state))
    {
        printk("slave_wakeup_rising_int_handler error,HSIC state invalid.\n");	
        gpio_int_unmask_set(SLAVE_WAKEUP_RISING);
        return ERROR;
    }
	
    /*printk("slave_wakeup_rising_int_handler queue!\n");*/
    
    if(HSIC_L2_STATE == current_state)
    {
        /*ͶƱ��ֹ˯��*/
        (void)BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_HSIC); 
		PWRCTRL_HSIC_CLEAR_SLEEP_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
        hsic_pwc_state_set(HSIC_L20_STATE);
    }
    else if(HSIC_L3_STATE == current_state)
    {
            /*ͶƱ��ֹ˯��*/

        /*do nothing,���淢��GPIO�ź�,���Ѽ�L3-L0״̬�л���HOST ACTIVE���*/
		//Modem_enable_hsic(1);
    }
    else/* if(current_state >= HSIC_L02_STATE)*/
    {
        printk("slave_wakeup_rising_int_handler error,HSIC state doesn't switch,current state is:%d.\n",current_state);	
        gpio_int_unmask_set(SLAVE_WAKEUP_RISING);
        return ERROR;
    }
    
    /*����HOST_WAKEUP�ź�,GPIO_2_1�½��ص�ap*/
    //(void)gpio_value_set((unsigned int)HOST_WAKEUP_GPIO, (unsigned int)(0x1 << HOST_WAKEUP_PIN), (unsigned int)(GPIO_LOW << HOST_WAKEUP_PIN));
    if(OK ==  gpio_chip_value_get(HOST_WAKEUP_GPIO, HOST_WAKEUP_PIN,&gpio_value))
    {
        if(GPIO_LOW == gpio_value)
        {
            printk("slave_wakeup_rising_int_handler error,HOST_WAKEUP is low.\n");
        }
        else
        {
            gpio_direction_output(HOST_WAKEUP,GPIO_LOW);
            printk("slave_wakeup_rising_int_handler ok,sending HOST_WAKEUP FALLING.\n");
        }
    }
    else
    {
        printk("slave_wakeup_rising_int_handler error,get HOST_WAKEUP value failed.\n");
    }

    printk("slave_wakeup_rising_int_handler success.\n");	
    
    gpio_int_unmask_set(SLAVE_WAKEUP_RISING);
    return OK;
}


/*****************************************************************************
name            : slave_wakeup_falling_int_install
Description    :slave wakeup�½����ź��ж�ע��
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
s8 slave_wakeup_falling_int_install(void)
{
    /*ע�Ტʹ��slave_wakeup_falling,�½����жϴ���,��ӦSLAVE_WAKEUP�ź�*/
    printk("slave_wakeup_falling_int_install,start...\n");

    gpio_int_mask_set(SLAVE_WAKEUP_FALLING);
    
    if(gpio_request(SLAVE_WAKEUP_FALLING, "slave_wakeup_falling_int"))
    {
    	printk("slave_wakeup_falling_int_install,the slave wakeup falling signal requesting is busy!\n");
    	return ERROR;
    }

    workqueue = create_workqueue("slave_wakeup_falling_queue");
    if (NULL == workqueue)
    {
    	printk("slave_wakeup_falling_int_install,slave_wakeup_falling_queue workqueue create error!\n");
    	return ERROR;
    }

    INIT_WORK(&slave_wakeup_falling_q, slave_wakeup_falling_int_handler);/*lint !e155*//*lint !e69*//*lint !e64*/
    if (request_irq(SLAVE_WAKE_F_INT,slave_wakeup_falling_isr,IRQF_SHARED,"slave_wakeup_falling_int_handler", &slave_wakeup_falling_q)) 
    {
        printk("slave_wakeup_falling_int_install,can't get slave wakeup falling irq.\n");
        return ERROR;
    }	
    
    gpio_direction_input(SLAVE_WAKEUP_FALLING); 
    gpio_set_function(SLAVE_WAKEUP_FALLING,GPIO_INTERRUPT);
    gpio_int_state_clear(SLAVE_WAKEUP_FALLING);
    gpio_int_trigger_set(SLAVE_WAKEUP_FALLING,IRQ_TYPE_EDGE_FALLING);
    gpio_int_unmask_set(SLAVE_WAKEUP_FALLING);
    
    printk("slave_wakeup_falling_int_install success.\n");

    return OK;

}


/*****************************************************************************
name            : slave_wakeup_falling_isr
Description    :slave_wake�ź��½����жϴ����ϰ벿
Input          : int irq, void *dev_id
Return Value   :  IRQ_NONE/IRQ_HANDLED
Calls          :
Called By      :
*****************************************************************************/
irqreturn_t slave_wakeup_falling_isr(int irq, void *dev_id)
{
    unsigned int ucData = 0; 
    /*�ж��Ƿ�Ϊslave wakeup �½��ص��ж�*/
    gpio_int_chip_state_get(SLAVE_WAKEUP_FALLING_GPIO, SLAVE_WAKEUP_FALLING_PIN, (unsigned*)&ucData);
    if (!ucData)
    {
    	/*printk(KERN_DEBUG"not slave wakeup falling isr! \n");*/
    	return IRQ_NONE;
    }
    /*���жϣ����ж�*/
    gpio_int_chip_mask_set(SLAVE_WAKEUP_FALLING_GPIO, SLAVE_WAKEUP_FALLING_PIN);
    gpio_int_chip_state_clear(SLAVE_WAKEUP_FALLING_GPIO, SLAVE_WAKEUP_FALLING_PIN);

    queue_work(workqueue,&slave_wakeup_falling_q);

    /*printk(KERN_DEBUG"slave wakeup falling isr ok! \n");*/
    return IRQ_HANDLED;
}

/*****************************************************************************
name            : slave_wakeup_falling_int_handler
Description    :slave_wake�ź��½����жϴ����°벿
Input          : struct work_struct *work
Output         : NU_NULL
Return Value   :  N/A
Calls          :
Called By      :
*****************************************************************************/
s8 slave_wakeup_falling_int_handler(struct work_struct *work)
{
    HSIC_STATE_TYPE current_state;   
    int i = 0;
    
    current_state = hsic_pwc_state_get();
    PRINT_PWC_DBG("slave_wakeup_falling_int_handler start.. hsic_current_state = %d\n", current_state);

    if((HSIC_INIT_STATE> current_state)&&(HSIC_MAX_STATE <= current_state))
    {
        printk("slave_wakeup_falling_int_handler error,HSIC state invalid.\n");	
        gpio_int_unmask_set(SLAVE_WAKEUP_FALLING);
        return ERROR;
    }
    	
    /*printk("slave_wakeup_falling_int_handler queue!\n");*/

    /*��HSICö����ɺ���ΪL0�����ﲻ������*/
    #if 0
    if((HSIC_L20_STATE == current_state)||(HSIC_L30_STATE == current_state))
    {
        do
        {
            if (1 != hsic_enum_completed())
            {
                msleep(200);
            }
            else
            {
                hsic_pwc_state_set(HSIC_L0_STATE);
                break;
            }
            i++;
        }while(i < 15);

        if(i >= 15)
        {
            printk("slave_wakeup_falling_int_handler failed,hsic enum not complete.\n");
            return ERROR;
        }

    }
    else
    {
        printk("slave_wakeup_falling_int_handler error,HSIC state doesn't switch,current_state:%d.\n",current_state);
        gpio_int_unmask_set(SLAVE_WAKEUP_FALLING);
        return ERROR;
    }
    #endif
    
    gpio_int_unmask_set(SLAVE_WAKEUP_FALLING);
    
    printk("slave_wakeup_falling_int_handler success,but do nothing.\n");
    return OK;
}


/*****************************************************************************
name            : host_active_pwc_isr
Description    :host_active �ź��ж��ϰ벿�͹��Ĳ��ִ���
Input          : NU_NULL
Return Value   :  NULL
Calls          :
Called By      :
*****************************************************************************/
void host_active_pwc_isr(void)
{
    /*ע�Ტʹ��host_active,��ʼ��Ϊ�½����жϴ���*/
    /*����HSIC�������,������ʽ��Ӧ����������øı�*/
    #if 0
    if(IRQ_TYPE_EDGE_FALLING == host_active_edge_flag)
    {       
        /*��HOST_ACTIVE�жϴ�����ʽ����Ϊ�����ش���*/
        gpio_int_trigger_set(HOST_ACTIVE,IRQ_TYPE_EDGE_RISING);
    }
    else if(IRQ_TYPE_EDGE_RISING == host_active_edge_flag)
    {
        /*��HOST_ACTIVE�жϴ�����ʽ����Ϊ�½��ش���*/
        gpio_int_trigger_set(HOST_ACTIVE,IRQ_TYPE_EDGE_FALLING);
    }
    else
    {
        printk(KERN_DEBUG"host_active_pwc_isr,trigger type error!\n");
    }
    printk(KERN_DEBUG"host active pwc isr ok.\n");
    #endif
}


s8 hsic_cp_wakeup_ap_L3(void)
{
    int gpio_value = 0;

    /*���ж�Ϊ������CP�Ի���*/
    if(OK ==  gpio_chip_value_get(SLAVE_WAKEUP_RISING_GPIO, SLAVE_WAKEUP_RISING_PIN,&gpio_value))
    {
        if(GPIO_LOW == gpio_value)
        {
            if(OK ==  gpio_chip_value_get(HOST_WAKEUP_GPIO, HOST_WAKEUP_PIN,&gpio_value))
            {
                if(GPIO_LOW == gpio_value)
                {
                    PRINT_PWC_DBG("hsic_cp_wakeup_ap_L3 failed,HOST_WAKEUP is already LOW.\n");
                    return ERROR;
                }
                else
                {
                    (void)BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_HSIC); 
					PWRCTRL_HSIC_CLEAR_SLEEP_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
                    gpio_direction_output(HOST_WAKEUP, GPIO_LOW);
                    printk("hsic_cp_wakeup_ap_L3 ok,sending HOST_WAKEUP FALLING.\n");                  
                }
            }
            else
            {
                PRINT_PWC_DBG("hsic_cp_wakeup_ap_L3 failed,get HOST_WAKEUP value failed.\n");
                return ERROR;
            }
        }
        else
        {
            PRINT_PWC_DBG("hsic_cp_wakeup_ap_L3 rising error,the SLAVE_WAKEUP_RISING_GPIO is not LOW.\n");
            return ERROR;
        }
    }
    else
    {
        PRINT_PWC_DBG("hsic_cp_wakeup_ap_L3 rising error,get SLAVE_WAKEUP_RISING_GPIO value failed.\n");
        return ERROR;
    }
    
    return OK;
}


/*****************************************************************************
name            : host_active_pwc_int_handler
Description    :host_active �ź��ж��°벿�͹��Ĳ��ִ���
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
s8 host_active_pwc_int_handler(void)
{
    HSIC_STATE_TYPE current_state;

    int gpio_value = 0;
    //int i = 0;
	int error = 0;
    /*
    os_bus_resource_h usb_periph_h = (os_bus_resource_h)&hsic_bus_rsrc_mem;
    printk("AP_detect_handler queue!\n");
    */
        
    current_state = hsic_pwc_state_get();

    PRINT_PWC_DBG("host_active_pwc_int_handler start...hsic_current_state = %d\n", current_state);
    
    /*��ΪHOST_ACTIVE�ж��½���*/
    if(IRQ_TYPE_EDGE_FALLING == host_active_edge_flag)
    {
        //host_active_edge_flag = IRQ_TYPE_EDGE_RISING;
        if((HSIC_L2_STATE == current_state)||(HSIC_L20_STATE == current_state))
        {
            /*��Ϊ��ʱ�ѽ���L3̬*/
            hsic_pwc_state_set(HSIC_L3_STATE);
            /*��λHSIC*/
            Modem_enable_hsic(0);
            /*ö�ٳɹ���־����*/
            //hsic_enum_status_set(0);

            /*��HOST_ACTIVE�жϴ�����ʽ����Ϊ�����ش���*/
            host_active_edge_flag = IRQ_TYPE_EDGE_RISING;
            gpio_int_trigger_set(HOST_ACTIVE,IRQ_TYPE_EDGE_RISING);
            
            /*ͶƱ����˯��, �µ��ɹ��Ĺ���ģ�����*/
            (void)BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_HSIC); 
			PWRCTRL_HSIC_SET_L3_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
        }
        else
        {
            printk("host_active_pwc_int_handler falling error,HSIC state doesn't switch,current_state:%d.\n",current_state);
            return ERROR;
        }
    } 
    else/*��ΪHOST_ACTIVE�ж�������*/
    {

        if((HSIC_L3_STATE == current_state) || (HSIC_L30_STATE == current_state))
        {
            /*ͶƱ��ֹ˯��*/
            (void)BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_HSIC);
			PWRCTRL_HSIC_CLEAR_SLEEP_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
            hsic_pwc_state_set(HSIC_L30_STATE);
                        
            /*HSIC��λ�⸴λ*/
            hsic_enum_status_set(0);
            //printk("host_active hsic_reg_init_for_lpd() start!\n");
            error = hsic_reg_init_for_lpd(); 
            if (error != 0)
            {
              printk("^_^ slave_wakeup hsic_reg_init_for_lpd():error = %d.\n",error);
            }
            hsic_bus_enable_by_pwrctl();            
            /*HSIC��������*/
            //printk("HSIC reset start!\n");            
            Modem_enable_hsic(1); 

            msleep(20);
            	
            if(OK ==  gpio_chip_value_get(HOST_WAKEUP_GPIO, HOST_WAKEUP_PIN,&gpio_value))
            {
                if(GPIO_HIGH == gpio_value)
                {
                    printk("host_active_pwc_int_handler rising error,HOST_WAKEUP is high.\n");
                    return ERROR;
                }
                else
                {
                    #if 0
		    do
		    {
                        if (1 != hsic_enum_completed())
                        {
                            msleep(200);
                        }
                        else
                        {
                            gpio_direction_output(HOST_WAKEUP, GPIO_HIGH);
                            break;
                        }
		        i++;
		    }while(i < 10);
		    
                    if(i >= 10)
                    {
                        printk("host_active_pwc_int_handler rising failed,hsic enum not complete.\n");
                        return ERROR;
                    }
                    #endif
                    /* ö�ٳɹ�����AP�����Ի���*/
                    gpio_direction_output(HOST_WAKEUP, GPIO_HIGH);
                    printk("host_active_pwc_int_handler rising ok,sending HOST_WAKEUP RISING.\n");
                    
                }
            }
            else
            {
                printk("host_active_pwc_int_handler rising error,get HOST_WAKEUP value failed.\n");
                return ERROR;
            }                     
        }
        else
        {
            printk("host_active_pwc_int_handler rising error,HSIC state doesn't switch,current_state:%d.\n",current_state);
            return ERROR;
        }
    }
    
    printk("host_active_pwc_int_handler success.\n");

    return OK;
    /*gpio_int_unmask_set(HOST_ACTIVE);*/
}

/*****************************************************************************
name            : hsic_suspend_pwc_int_handler
Description    :hsic suspend �ź��жϴ���
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
s8 hsic_suspend_pwc_int_handler(void)
{
    HSIC_STATE_TYPE current_state;
    current_state = hsic_pwc_state_get();
    
    PRINT_PWC_DBG("hsic_suspend_pwc_int_handler,start.. hsic_current_state = %d\n", current_state);

    
    if((HSIC_INIT_STATE > current_state)&&(HSIC_MAX_STATE <= current_state))
    {
        PRINT_PWC_DBG("hsic_suspend_pwc_int_handler error,HSIC state invalid.\n");	
        return ERROR;
    }

    
    if(HSIC_L0_STATE == current_state)
    {
        hsic_pwc_state_set(HSIC_L2_STATE);
        /*ͶƱ����˯��, �µ��ɹ��Ĺ���ģ�����*/
        if(g_usb_current_ftm_mode != TRUE)
        {
            (void)BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_HSIC); 
			PWRCTRL_HSIC_SET_L2_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
        }
    }
    else
    {
        PRINT_PWC_DBG("hsic_suspend_pwc_int_handler error,current_state:%d.\n",current_state);
        return ERROR;
    }
    printk("hsic_suspend_pwc_int_handler success.\n");
    return OK;
    
}

s8 hsic_cp_wakup_ap_L2(void)
{
    int gpio_value = 0;
        
    /*���ж�Ϊ������CP�Ի���*/
    if(OK ==  gpio_chip_value_get(SLAVE_WAKEUP_RISING_GPIO, SLAVE_WAKEUP_RISING_PIN,&gpio_value))
    {
        if(GPIO_LOW == gpio_value)
        {
            /*send host_wakeup signal falling edge*/
            if(OK ==  gpio_chip_value_get(HOST_WAKEUP_GPIO, HOST_WAKEUP_PIN,&gpio_value))
            {
                if(GPIO_LOW == gpio_value)
                {
                    PRINT_PWC_DBG("hsic_cp_wakup_ap_L2 error,HOST_WAKEUP is already LOW.\n");
                    return ERROR;
                }
                else
                {
                    (void)BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_HSIC);
					PWRCTRL_HSIC_CLEAR_SLEEP_BIT(MEMORY_AXI_WIFI_HSIC_PERI_ADDR);
                    hsic_pwc_state_set(HSIC_L20_STATE);
                    gpio_direction_output(HOST_WAKEUP, GPIO_LOW);
                    printk("hsic_cp_wakup_ap_L2 ok,sending HOST_WAKEUP FALLING.\n");
                }
            }
            else
            {
                PRINT_PWC_DBG("hsic_cp_wakup_ap_L2 error,get HOST_WAKEUP value failed.\n");
                return ERROR;
            }
            
        }
        else
        {
            PRINT_PWC_DBG("hsic_cp_wakup_ap_L2 error,the SLAVE_WAKEUP_RISING_GPIO is not LOW.\n");
            return ERROR;
        }
    }
    else
    {
        PRINT_PWC_DBG("hsic_cp_wakup_ap_L2 error,get SLAVE_WAKEUP_RISING_GPIO value failed.\n");
        return ERROR;
    }
    return OK;

}

/*****************************************************************************
name            : hsic_resume_pwc_int_handler
Description    :hsic resume �ź��жϴ���
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
s8 hsic_resume_pwc_int_handler(void)
{
    HSIC_STATE_TYPE current_state;
    int gpio_value = 0;
    
    current_state = hsic_pwc_state_get();
    
    PRINT_PWC_DBG("hsic_resume_pwc_int_handler,start..hsic_current_state = %d\n", current_state);
    
     
    if(HSIC_L20_STATE == current_state)
    {
        if(OK ==  gpio_chip_value_get(HOST_WAKEUP_GPIO, HOST_WAKEUP_PIN,&gpio_value))
        {
            if(GPIO_HIGH == gpio_value)
            {
                PRINT_PWC_DBG("hsic_resume_pwc_int_handler error,HOST_WAKEUP is high.\n");
                return ERROR;
            }
            else
            {                
                /*��HSIC��������֤�����ﲻ�ж�*/
                if (1)//(1 == hsic_enum_completed())
                {
                    gpio_direction_output(HOST_WAKEUP, GPIO_HIGH);
                    printk("hsic_resume_pwc_int_handler ok,sending HOST_WAKEUP RISING.\n");
                }
                else
                {
                    PRINT_PWC_DBG("hsic_resume_pwc_int_handler error,hsic enum not complete at L20.\n");
                    return ERROR;
                }
                hsic_pwc_state_set(HSIC_L0_STATE);
            }
        }
        else
        {
            PRINT_PWC_DBG("hsic_resume_pwc_int_handler error,get HOST_WAKEUP value failed.\n");
            return ERROR;
        }
            
        /*	HSIC�ָ������յ�host wakeup �����ز�����HSIC״̬ΪL0*/
    }
    else
    {
        PRINT_PWC_DBG("hsic_resume_pwc_int_handler error,current_state:%d.\n",current_state);
        return ERROR;
    }
    
    printk("hsic_resume_pwc_int_handler success.\n");
    return OK;
}

static int s_cp_wakeup_ap_flag = 1;
void hsic_set_wakeup_ap_flag(int state)
{
    s_cp_wakeup_ap_flag = state;
	printk("hsic_set_wakeup_ap_flag flag = %d.\n", s_cp_wakeup_ap_flag);
}


/*****************************************************************************
name            : hsic_modem_wakeup_ap
Description    :modem ����AP
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
void hsic_modem_wakeup_ap(int state)
{

    HSIC_STATE_TYPE current_state;
    int gpio_value = 0;
    
    current_state = hsic_pwc_state_get();
    if (0 == s_cp_wakeup_ap_flag)
    {
        return;
    }
    if (HSIC_L2_STATE == current_state)
    {
        printk("hsic_modem_wakeup_ap,start..hsic_current_state = %d set gpio with %d\n", current_state, state);        
        
        if(OK != hsic_cp_wakup_ap_L2())
	{
            printk("hsic_modem_wakeup_ap error,L2 to L0 failed.\n");
            return;
	}                
    }    
    else if (HSIC_L3_STATE == current_state) 
    {
        printk("hsic_modem_wakeup_ap,start..hsic_current_state = %d set gpio with %d\n", current_state, state);
     //   hsic_pwc_state_set(HSIC_L30_STATE);
	// Modem_enable_hsic(1);
	if(OK != hsic_cp_wakeup_ap_L3())
	{
            printk("hsic_modem_wakeup_ap error,L3 to L0 failed.\n");
            return;
	}
    }

}

/*****************************************************************************
name            : hsic_modem_need_wakeup_ap
Description    :modem�Ƿ���Ҫ ����AP
Input          : NU_NULL
Return Value   :  OK/ERROR
Calls          :
Called By      :
*****************************************************************************/
UINT32 acm_write_test = 0;
void acm_write_test_set(UINT32 val)
{
    acm_write_test = val;
}
int hsic_modem_need_wakeup_ap(void)
{
	int	iRelt = 0;
	HSIC_STATE_TYPE current_state;

        if(0 != acm_write_test)
        {
            printk("###acm write###\n");
        }

	current_state = hsic_pwc_state_get();
	if((HSIC_L2_STATE== current_state)||(HSIC_L3_STATE == current_state))
	{
            printk("hsic_modem_need_wakeup_ap,start..hsic_current_state = %d\n", current_state);
            iRelt = 1;
	}
	else 
	{
	    iRelt = 0;
	}
	return iRelt;    
}
int hsic_modem_need_wait_resume_ok(void)
{
	int	iRelt = 0;
	HSIC_STATE_TYPE current_state;

	current_state = hsic_pwc_state_get();
	if((HSIC_L20_STATE== current_state)||(HSIC_L30_STATE == current_state))
	{
        printk("hsic_modem_need_wait_resume_ok,start..hsic_current_state = %d\n", current_state);
        iRelt = 1;
	}
	else 
	{
	    iRelt = 0;
	}
	return iRelt;    
}

int hsic_modem_enum_ok(void)
{
    if(HSIC_L0_STATE == hsic_pwc_state_get())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
void modem_wakeup_test(s8 val)
{
    g_hsic_modem_wakeup = val;
}
*/
/*****************************************************************************
* �� �� ��  : Hsic_makesure_ready
*
* ��������  : ����ap,������hsic���ӡ�
*
* �������  : ��
* �������  : *pbEnable
*
* �� �� ֵ  : BSP_VOID
*
* �޸ļ�¼ : 
*
*****************************************************************************/
void Hsic_makesure_ready(void)
{
    BSP_U16	i = 0;
    if (hsic_modem_need_wakeup_ap() == 1
        || hsic_modem_need_wait_resume_ok() == 1)
    {
		hsic_modem_wakeup_ap(0);
		msleep(20);
	    do
	    {
		    if (1 != hsic_modem_enum_ok())
		    {
		        msleep(10);
		    }
	    	else
		    {
                break;
		    }
	        i++;
	    }while(i < 200);
    }
}

#endif

extern int wifi_power_off_full(void);
extern void tftPwrOff(void);
extern unsigned int BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E  enClientId);

unsigned int g_ulEquipStandbyCount = 0;
unsigned int g_ulAcpuStandbyFlag = 0;

/*****************************************************************************
�� �� ��  : EQUIP_StandbyState_Acpu
��������  : ���߲��Խ���Standby�͹���״̬
�������  : ��
�������  : ��
����ֵ    : 0   OK
			-1  ERROR    
�޸���ʷ      :
	1.��    ��   : 
	  ��    ��   : 
         �޸�����   : �����ɺ���
*****************************************************************************/
unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime)
{
#if ( FEATURE_E5 == FEATURE_ON )
	g_ulEquipStandbyCount++;

	wifi_power_off_full(); 
    BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_LIGHTSLEEP_WIFI);
    BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_WIFI);
    
	tftPwrOff();
    
    g_ulAcpuStandbyFlag = 1;
#endif
	return 0;
}
EXPORT_SYMBOL(BSP_PWRCTRL_StandbyStateAcpu);

#endif
