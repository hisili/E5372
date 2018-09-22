/*
 * antenna_swtich.c - hisilicon a core antenna plug in/out detect driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
#if ((FEATURE_EPAD == FEATURE_ON) || (FEATURE_STICK == FEATURE_ON) || (FEATURE_E5 == FEATURE_ON) || (FEATURE_PHONE == FEATURE_ON)|| (FEATURE_HILINK == FEATURE_ON))
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/random.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/kthread.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/switch.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <linux/irq.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <mach/irqs.h>
#include <asm/sizes.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <linux/gpio.h>
#define UP  	(1)
#define DOWN 	(2)
typedef struct
{
    BSP_U32 u32ModuleId;
    BSP_U32 u32FuncId;
    BSP_U32 status;
}ANTENNA_IFC_MSG_STRU;
/*v7r1 es & cs antena plug in/out support, zkf52403*/
#if defined(CHIP_BB_6920ES)
#define ANTENNA_GPIO_INT        BALONG_GPIO_0(12)
#define ANTENNA_INT             (112)
#elif defined(CHIP_BB_6920CS)
#define ANTENNA_GPIO_INT        BALONG_GPIO_2(5)
#define ANTENNA_INT             (114)
#else
#error "unknown chip type!!"
#endif
#undef ANTEN_NAME
#define ANTEN_NAME "anten_"
static struct workqueue_struct  *antenna_workque = NULL;
static struct work_struct        antenna_work;
 int balong_anten_send_arg(BSP_U32 status)
{
    BSP_S32 ret;
    ANTENNA_IFC_MSG_STRU stAntIFCMsg = {0};
    
    /*消息体变量赋值*/
    stAntIFCMsg.u32ModuleId = IFC_MODULE_BSP;
    stAntIFCMsg.u32FuncId = IFCP_FUNC_ANTENNA_DETECT;
    stAntIFCMsg.status = status;

    /*通过IFC将消息发出*/
    ret = BSP_IFC_Send(&stAntIFCMsg, sizeof(ANTENNA_IFC_MSG_STRU));
    if(BSP_OK != ret)
    {
        printk(KERN_ERR"Ant_DRV: ACPU IFC Send Fail! ret=0x%x\n", ret, 0, 0, 0, 0, 0);
        return BSP_ERROR;
    }

    return BSP_OK;
}
static void anten_work(struct work_struct *work)
{
    /*debug info*/
#if 0	
    unsigned ucData = 0;
    int ret = 0;
    printk(KERN_ERR"value: %u\n", gpio_get_value(ANTENNA_GPIO_INT));
    gpio_raw_int_state_get(ANTENNA_GPIO_INT, &ucData);
    printk(KERN_ERR"raw int: %u\n", ucData);
    gpio_int_state_get(ANTENNA_GPIO_INT, &ucData);
#endif	
    balong_anten_send_arg(gpio_get_value(ANTENNA_GPIO_INT));
    //printk(KERN_ERR"mask int: %u\n", ucData);
}
static irqreturn_t anten_irq_irq(int irq, void *dev_id)   
{
	unsigned ucData = 0; 
	gpio_int_state_get(ANTENNA_GPIO_INT, &ucData);
	//printk(KERN_DEBUG"ucData: 0x%x\n", ucData);
	if (!ucData)
	{
		return IRQ_NONE;
	}
	//printk(KERN_DEBUG"antenna catch interrupt\n");
	gpio_int_mask_set(ANTENNA_GPIO_INT);
	gpio_int_state_clear(ANTENNA_GPIO_INT);
	ucData = gpio_get_value(ANTENNA_GPIO_INT);
    if (ucData)
    {     
		gpio_int_trigger_set(ANTENNA_GPIO_INT,IRQ_TYPE_LEVEL_LOW);
    }
    else
    {       
        gpio_int_trigger_set(ANTENNA_GPIO_INT,IRQ_TYPE_LEVEL_HIGH);
    }
	if(dev_id && antenna_workque)
	{
	    //printk(KERN_DEBUG"queue antena work!\n");
	    queue_work(antenna_workque, (struct work_struct *)dev_id);
    }	
    gpio_int_unmask_set(ANTENNA_GPIO_INT);
	return IRQ_HANDLED;
}
static void __init anten_irq_init(void)
{
    unsigned long ucData;
    printk("%s: begin\n",__FUNCTION__);
	if(gpio_request(ANTENNA_GPIO_INT, ANTEN_NAME"gpio"))
	{
		printk(KERN_ERR"request anten gpio is busy!\n");
		goto out;
	}
	gpio_int_mask_set(ANTENNA_GPIO_INT);   
	ucData = gpio_get_value(ANTENNA_GPIO_INT);
    if (ucData)
    {     
		gpio_int_trigger_set(ANTENNA_GPIO_INT,IRQ_TYPE_LEVEL_LOW);
    }
    else
    {       
        gpio_int_trigger_set(ANTENNA_GPIO_INT,IRQ_TYPE_LEVEL_HIGH);
    }
    
	antenna_workque = create_singlethread_workqueue(ANTEN_NAME"queue");
	if(antenna_workque == NULL)
	{
	    printk(KERN_ERR"antenna_workque create failed!\n");
	    goto out_queue;
	}
	INIT_WORK(&antenna_work, anten_work);	
	
    /*conncet antenna interrupt*/         
	if (request_irq(ANTENNA_INT, anten_irq_irq,IRQF_SHARED,ANTEN_NAME"irq", &antenna_work)) 
	{
	    printk(KERN_ERR"request anten gpio irq is busy!\n");
	    goto out_gpio;
	}
	gpio_direction_input(ANTENNA_GPIO_INT);     
    gpio_int_state_clear(ANTENNA_GPIO_INT);
    gpio_set_function(ANTENNA_GPIO_INT,GPIO_INTERRUPT); 	
    gpio_int_unmask_set(ANTENNA_GPIO_INT); 	    
    return;
out_gpio: 
    gpio_free(ANTENNA_GPIO_INT);
out_queue:
    if(NULL != antenna_workque)
    {
        destroy_workqueue(antenna_workque);
    }    
out:
    return;
    
}
static void __init anten_irq_exit(void)
{
    printk("%s: end\n",__FUNCTION__);
    if(NULL != antenna_workque)
    {
        destroy_workqueue(antenna_workque);
    }     
	free_irq(ANTENNA_INT, &antenna_work);
    gpio_free(ANTENNA_GPIO_INT);	
}
module_init(anten_irq_init);
module_exit(anten_irq_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("antenna driver for the Hisilicon antenna plug in/out");
MODULE_LICENSE("GPL");
#endif
