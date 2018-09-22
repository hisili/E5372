#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/bsp_pmu_drv.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <linux/irq.h>
#include <linux/sysfs.h>
#include <linux/gpio.h>

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/scatterlist.h>
#include <linux/random.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/switch.h>
#include <linux/fs.h>
#include <linux/slab.h> /*y00186965*/

#include <asm/dma.h>
#include <asm/io.h>
#include <mach/irqs.h>
#include <asm/sizes.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <mach/DrvInterface.h>
//#include <generated/FeatureConfig.h>
#include "product_config.h"
#include "Bsp_drv_sci.h"


#ifdef FEATURE_SIM_NOTIFY


//#define BALONG_GPIO_DETECT_CHIP  2
#define BALONG_GPIO_SIM_DETECT_PIN	 9
#define BALONG_GPIO_SIM_DETECT		BALONG_GPIO_2(BALONG_GPIO_SIM_DETECT_PIN)


#define DRIVER_NAME_SIM	"sim_io"
#define DETECT_NAME_SIM "sim_detect"

#define SCI_DETECT_DEBOUNCE_TIMES 5

/*
 * Description:
 * Synchronous semphor for isr and task
 */
//SEM_ID g_SCIDetectSem = 0;

/*
 * Description:
 * Synchronous task ID
 */
//static struct task_struct *sci_task;



struct work_struct	sim_detect_work;
//struct workqueue_struct *sim_detect_queue;

#if 0

/*lint -e525*/
//extern OMSCIFUNCPTR *g_omSciFuncPtr;





void sci_detect_task(void)
{
    UINT32 ulVal;
    BSP_U32 msg[SIM_DETECT_INFO_NO];
    BSP_S32 iRet;

	for(;;)
	{
		down(&g_SCIDetectSem);

		BSP_GPIO_ValGet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, &ulVal);
    	ulVal = ulVal & (1 << SIM_GPIO_INT_PIN);
    
    
    	if ((1 << SIM_GPIO_INT_PIN) != ulVal)   /*�γ��ж�*/
        {
            /*��������*/
            //sciDrvSleep(1000);
			msleep(1000);
            BSP_GPIO_ValGet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, &ulVal);
            ulVal = ulVal & (1 << SIM_GPIO_INT_PIN); 
    		
            if ((1 << SIM_GPIO_INT_PIN) == ulVal) /*���жϣ�������*/
            {
                BSP_GPIO_IntUnMaskSet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, (0x01 << SIM_GPIO_INT_PIN));
            }
    
            /*�����´��жϵĴ����������ߵ�ƽ���������ж�*/
            BSP_GPIO_IntTriggerSet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, 0x1 << SIM_GPIO_INT_PIN, GPIO_HIGH_LEVEL); 
        }
        else  /*�����ж�*/
        {
			msleep(1000);
            BSP_GPIO_ValGet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, &ulVal);
            ulVal = ulVal & (1 << SIM_GPIO_INT_PIN);
    		
            if((1 << SIM_GPIO_INT_PIN) != ulVal) /*���жϣ�������*/
            {
    	        BSP_GPIO_IntUnMaskSet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, (0x01 << SIM_GPIO_INT_PIN));
				
				continue;
            }
    
            /*�����´��жϵĴ����������͵�ƽ�����γ��ж�*/
    	    BSP_GPIO_IntTriggerSet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, 0x1 << SIM_GPIO_INT_PIN, GPIO_LOW_LEVEL); 
           
        }     
    
        /*���ж�*/
        BSP_GPIO_IntUnMaskSet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, (0x01 << SIM_GPIO_INT_PIN));  
		// send IFC message
		msg[0] = IFC_MODULE_BSP;
    	msg[1] = IFCP_FUNC_SIM_DETECT;
		msg[2] = msg[2]? SIM_CARD_IN : SIM_CARD_OUT;
		iRet =  BSP_IFC_Send(msg, sizeof(msg)); /* �ֽ��� */
	    if(BSP_OK != iRet)
	    {
	        printk("IFC DRCtrl Send error, iRet:%#x\n",iRet,0,0,0,0,0);
			goto exit;
	    }
	}
}

void sci_detect_isr(void)
{
	BSP_GPIO_IntMaskSet((GPIO_GPIO_NUM_E)GPIO_DEF_GPIO0, (0x01 << SIM_GPIO_INT_PIN));
	//SCI_RECORD_EVENT(SCI_EVENT_GPIO_ISR, (UINT32)__LINE__, 0);
	
	up(&g_SCIDetectSem);

}
#endif

/*lint +e525*/
BSP_U32 u32ASimDetectCntEnter = 0;
BSP_U32 u32ASimDetectCntMid0 = 0;
BSP_U32 u32ASimDetectCntMid1 = 0;
BSP_U32 u32ASimDetectCntEnd = 0;
BSP_U32 u32ASimSleepErrCnt = 0;
BSP_U32 u32ASimSleepLcdErrCnt = 0;
//
BSP_BOOL bLcdVoteFlag = BSP_FALSE;

/*******************************************************************************
  Function:      sd_detect_handler(struct work_struct *data)
  Description:   sd detect handle work
  Input:         data:work_struct
                 devid:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void sim_detect_handler(struct work_struct *data)
{    
	BSP_U32 msg[SIM_DETECT_INFO_NO] = {0};
    BSP_S32 iRet;
    BSP_U32 ucData = 0;
	BSP_U32 u32SleepVote=0;
	BSP_U32 u32SleepVote1=0;
	BSP_U32 cir=0;
	   
   u32ASimDetectCntMid1 ++;
   
   /*��ѯSIM ��ʶ���ж�״̬*/
   ucData = gpio_get_value(BALONG_GPIO_SIM_DETECT);
   
   if (!ucData)   /*�γ��ж�*/
   {
		for(cir=0;cir<SCI_DETECT_DEBOUNCE_TIMES;cir++)
		{
			/*��������*/
			msleep(100);
			ucData = gpio_get_value(BALONG_GPIO_SIM_DETECT);
			if (ucData) /*���жϣ�������*/
			{
				gpio_int_unmask_set(BALONG_GPIO_SIM_DETECT);
				//spin_unlock(&host->lock); 
				printk("sim_detect_handler: GPIO dithering,not a ture draw int !\n");
				u32SleepVote = BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SCI);
				if(BSP_OK != u32SleepVote)
				{
					u32ASimSleepErrCnt ++;
				}
				return;
			}
		}
   
	   msg[2] = SIM_CARD_OUT;

		if(BSP_FALSE == bLcdVoteFlag)
		{

#if defined (FEATURE_LCD_ON)
			u32SleepVote1 = BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
			if(BSP_OK != u32SleepVote1)
			{
				u32ASimSleepLcdErrCnt ++;
				return ;
			}
			bLcdVoteFlag = BSP_TRUE;
		}
		   
	   /*�����´��жϵĴ����������ߵ�ƽ���������ж�*/
	   gpio_int_trigger_set(BALONG_GPIO_SIM_DETECT, IRQ_TYPE_LEVEL_HIGH); 
   
   }
   else  /*�����ж�*/
   {
		for(cir=0;cir<SCI_DETECT_DEBOUNCE_TIMES;cir++)
		{
			msleep(100);
			ucData = gpio_get_value(BALONG_GPIO_SIM_DETECT);
			if(!ucData) /*���жϣ�������*/
			{
				gpio_int_unmask_set(BALONG_GPIO_SIM_DETECT);
				//spin_unlock(&host->lock);
				printk(KERN_ERR"sim_detect_handler: GPIO dithering,not a ture insert int !\n");

				u32SleepVote = BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SCI);
				if(BSP_OK != u32SleepVote)
				{
					u32ASimSleepErrCnt ++;
				}
				return;
			}
		}

	   msg[2] = SIM_CARD_IN;
	   
	   /*�����´��жϵĴ����������͵�ƽ�����γ��ж�*/
	   gpio_int_trigger_set(BALONG_GPIO_SIM_DETECT,IRQ_TYPE_LEVEL_LOW);
   }	 
   
   msg[0] = IFC_MODULE_BSP;
   msg[1] = IFCP_FUNC_SIM_DETECT;
   iRet =  BSP_IFC_Send(msg, sizeof(msg)); /* �ֽ��� */
   if(BSP_OK != iRet)
   {
	   printk("IFC DRCtrl Send error, iRet:%#x\n",iRet,0,0,0,0,0);
	   gpio_int_unmask_set(BALONG_GPIO_SIM_DETECT);
		u32SleepVote = BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SCI);
		if(BSP_OK != u32SleepVote)
		{
			u32ASimSleepErrCnt ++;
		}
	   return ;
   }
   /*���ж�*/
   gpio_int_unmask_set(BALONG_GPIO_SIM_DETECT);
   
   u32ASimDetectCntEnd ++;

	u32SleepVote = BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SCI);
	if(BSP_OK != u32SleepVote)
	{
		u32ASimSleepErrCnt ++;
	}
	
	return ;
	
}


/*******************************************************************************
  Function:      sim_detect_irq_cd(int irq, void *dev_id)
  Description:   sd  detect interrpt handle
  Input:         irq: irq number
                 devid: device id
  Output:        irq handle result
  Return:        NA
  Others:        NA
*******************************************************************************/

static irqreturn_t sim_detect_irq_cb(int irq, void *dev_id)    
{  
    BSP_U32 ucDataVal = 0; 
	BSP_U32 msg[SIM_DETECT_INFO_NO] = {0};
    BSP_S32 iRet;
    BSP_U32 ucData = 0;
	BSP_U32 u32SleepVote=0;

	u32ASimDetectCntEnter ++;
	/*�ж��Ƿ�ΪGPIO2_9���ж�*/
    gpio_int_state_get(BALONG_GPIO_SIM_DETECT, (unsigned*)&ucDataVal);
    if (!ucDataVal)
    {
        return IRQ_NONE;
    }

	u32SleepVote = BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_SCI);
	if(BSP_OK != u32SleepVote)
	{
		u32ASimSleepErrCnt ++;
		return IRQ_NONE;
	}
	
	u32ASimDetectCntMid0 ++;
    /*���жϣ���״̬*/
    gpio_int_mask_set(BALONG_GPIO_SIM_DETECT);
    gpio_int_state_clear(BALONG_GPIO_SIM_DETECT);
	schedule_work(&sim_detect_work);
	
    return IRQ_HANDLED;
}
BSP_U32 SCI_Init()
{
	BSP_U32  u32SimDectSta = 0;
	GPIO_INT_TRIG_E eGpioIntSta = 0;
	BSP_U32 ret = 0;
	BSP_U32 ucData = 0;

	if(gpio_request(BALONG_GPIO_SIM_DETECT, "sim_detect"))
	{
		printk("======================SCI_Init:gpio_request fail=======================\n");
		return BSP_ERROR;
	}

	INIT_WORK(&sim_detect_work, sim_detect_handler);
	//INIT_WORK(&host->sd_detect_work, (void *)sd_detect_handler);
	
    /*�ж�SIM���Ƿ���λ,�Ӷ����ô�������*/    
	ucData = gpio_get_value(BALONG_GPIO_SIM_DETECT);/*����λ�ߵ�ƽ*/
    if (ucData)
    {
		gpio_int_trigger_set(BALONG_GPIO_SIM_DETECT,IRQ_TYPE_LEVEL_LOW);
    }
    else
    {       
        gpio_int_trigger_set(BALONG_GPIO_SIM_DETECT,IRQ_TYPE_LEVEL_HIGH);
    }

    /*�ҽ�SIM������ж�*/       
	gpio_direction_input(BALONG_GPIO_SIM_DETECT);     
	gpio_int_unmask_set(BALONG_GPIO_SIM_DETECT);
    gpio_int_state_clear(BALONG_GPIO_SIM_DETECT);
    gpio_set_function(BALONG_GPIO_SIM_DETECT,GPIO_INTERRUPT);

	if (request_irq(INT_GPIO_2, sim_detect_irq_cb,IRQF_SHARED, DETECT_NAME_SIM, "sim_detect"))
	{
		printk("==================SCI_Init:request_irq fail===========================\n");
		return BSP_ERROR;
    }
	printk("======================SCI_Init:OK!======================\n");
	return BSP_OK;
	
}

BSP_VOID SimDetectPrint()
{
	printk("u32ASimDetectCntEnter is 0x%x\n",u32ASimDetectCntEnter);
	printk("u32ASimDetectCntMid0 is 0x%x\n",u32ASimDetectCntMid0);
	printk("u32ASimDetectCntMid1 is 0x%x\n",u32ASimDetectCntMid1);
	printk("u32ASimDetectCntEnd is 0x%x\n",u32ASimDetectCntEnd);
	printk("u32ASimSleepErrCnt is 0x%x\n",u32ASimSleepErrCnt);
	printk("u32ASimSleepLcdErrCnt is 0x%x\n",u32ASimSleepLcdErrCnt);
}

#endif


