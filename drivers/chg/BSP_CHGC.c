/*************************************************************************
*   版权所有(C) 1987-2010, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_CHGC.c
*
*   作    者 :  Z00185733
*
*   描    述 :  充电模块在A核的核间通信模块。
*
*************************************************************************/
/**********************问题单修改记录************************************************
日    期              修改人                     问题单号                   修改内容
********************************************************************************************/
#include <generated/FeatureConfig.h>

#include <mach/common/bsp_version.h>
#include "BSP.h"
#include <linux/BSP_CHGC_DRV.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/netlink.h>
#include <linux/irq.h>
#include <mach/common/bsp_memory.h>
#include <linux/gpio.h>

#include <linux/mutex.h>
#include "../../arch/arm/mach-balong/include/mach/pwrctrl/BSP_PWC_SLEEPMGR.h"

#ifdef FEATURE_VOLT_PERCENT
#include "BSP_CHGC.h"
#endif
   
#if (FEATURE_OTG == FEATURE_ON) 
#include <mach/gpio.h>
#include <mach/hardware.h>
EXTCHAGRE_STATE_ENUM_LIST g_ACorExtChargingStateFlag = CHG_EXTCHAGRE_STATE_MAX;
#endif
#if(FEATURE_CHARGE == FEATURE_ON )
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 内部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部变量引用                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 全局变量申明                                                 *
 *--------------------------------------------------------------*/
#define TBATT_SUPL_MAGIC 	0x5543200

/*DOK GPIO定义 */
#if defined(CHIP_BB_6920CS)
#define INT_GPIO_DOK  				114
#define BALONG_GPIO_DOK_DETECT_PIN  (14)
#define BALONG_GPIO_DOK_DETECT		BALONG_GPIO_2(BALONG_GPIO_DOK_DETECT_PIN)
#else
#define INT_GPIO_DOK  				112
#define BALONG_GPIO_DOK_DETECT_PIN  (17)
#define BALONG_GPIO_DOK_DETECT		BALONG_GPIO_0(BALONG_GPIO_DOK_DETECT_PIN)
#endif
#define INTEGRATOR_BASE				IO_ADDRESS(0x90000000)
#define GPIO_DOK_REG				0x820	/*DOK/FLT使能上拉*/

#define DOK_GPIO_ONLINE				0x0
#define DOK_GPIO_OFFLINE			0x1

BSP_S32 CHG_MSG1 = 0;
BSP_S32 CHG_MSG2 = 0;
BSP_S32 CHG_MSG3 = 0;
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
BSP_S32 CHG_MSG4 = 0;
#endif
BSP_S32 CHG_DATA1[GET_CHG_DATA_MAX] = {0 };
BSP_S32 CHG_DATA2[GET_CHG_DATA_MAX] = {0 } ;
BSP_S32 CHG_DATA3[GET_CHG_DATA_MAX] = {0 } ;
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
BSP_S32 CHG_DATA4[GET_CHG_DATA_MAX] = {0 } ;
#endif
BSP_U32 g_u32BattSpFlag = 0;
BSP_U32 g_u32chgcStatus = 0;

/*充电器在位检测工作队列*/
struct work_struct	dok_detect_work;

extern USB_CHARGE_TYPE_ENUM usb_charge_type(void);
extern unsigned int usb_enum_completed(void);
extern BSP_S32 charger_event_proc(BSP_VOID *pMsgBody,BSP_U32 u32Len);

extern uint32_t  usb_get_charger_status(BSP_VOID);

/*****************************************************************************
* 函 数 名  :usb_charge_type_get
*
* 功能描述  :IFC回调,M核获取充电器类型接口
* 输入参数  :无
* 输出参数  :无
* 返回值：  :成功/失败
* 其它说明  : 无
****************************************************************************/  
int usb_charge_type_get(void)
{
    USB_CHARGE_TYPE_ENUM UsbChgType = USB_CHARGE_TYPE_NON_CHECK;
	
    UsbChgType = 1;
    printk( "UsbChgType:%d\n",UsbChgType);
    (void)BSP_CHGC_UsbReply(USB_CHG_TYP, UsbChgType,(BSP_U32)BSP_OK);
	
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  :DRV_APP_EVENT
*
* 功能描述  :CHG事件上报接口
* 输入参数  :CHG_DEVICE_ID:设备ID，CHG_EVENT_ID:事件ID
* 输出参数  :无
* 返回值：  :无
* 其它说明  : 无
****************************************************************************/  
void DRV_APP_EVENT(BSP_U32 CHG_DEVICE_ID,BSP_U32 CHG_EVENT_ID)
{
    int ret, size;
    DEVICE_EVENT event;

    size =  sizeof(DEVICE_EVENT);

    event.device_id = (DEVICE_ID)CHG_DEVICE_ID; 
    event.event_code =  (int)CHG_EVENT_ID; 
    event.len = 0;

    ret = device_event_report(&event, size);
    printk( "event:%d,%d\n",event.device_id, event.event_code);
    if (-1 == ret) 
    {
    	printk(KERN_ERR "device_event_init: can't add event\n");
    }
}

/*****************************************************************************
* 函 数 名  :CHG_Get_UsbEnum
*
* 功能描述  : IFC回调，报给M核USB枚举状态
* 输入参数  :无
* 输出参数  :无
* 返回值：  :无
* 其它说明  : 无
*
****************************************************************************/  
VOID CHG_Get_UsbEnum(void)
{
    BSP_U32 UsbEnum;
	
    UsbEnum = 0;
    printk("CHG_Get_UsbEnum:%d \n ",UsbEnum);
    (void)BSP_CHGC_UsbReply(USB_ENUM_COM,(BSP_U32)UsbEnum,(BSP_U32)BSP_OK);
}

static BSP_S32 balong_charge_open(struct inode *inode, struct file *file)
{
	if (NULL != inode && NULL != file)
	{
		;
	}
	return 0;
}

static BSP_S32 balong_charge_release(struct inode *inode, struct file *file)
{
	if (NULL != inode && NULL != file)
	{
		;
	}
	return 0;
}
/* BEIGN ,Modified by l00212897,2012/7/3 */
BSP_S32 balong_charge_ioctl(struct file *file, CHG_POLL_STATE cmd, void * arg)
{
	if (NULL == file)
	{
		return BSP_ERROR;
	}

    switch (cmd)
    {
    case CHG_POLL_BATT_LEVEL:	
        BSP_CHG_Appgetbatterystate((BATT_STATE_T *)arg);
        printk( " CHG_POLL_STATE:charging_state:%d,battery_level:%d \n",((BATT_STATE_T *)arg)->charging_state,((BATT_STATE_T *)arg)->battery_level);
        break;
    case CHG_POLL_USB_STATUS:
        if(BSP_ERROR == BSP_CHG_Appgetusbstate((CHG_USB_STATE *)arg))
        {
            printk( " CHG_POLL_STATE:current usb type is unknown\n");
            return BSP_ERROR;
        }
        printk( " CHG_POLL_STATE:current usb state is: %d \n",*(CHG_USB_STATE *)arg);
        break;
    default:
        break;

    }
	return BSP_OK;
}
/* END ,Modified by l00212897,2012/7/3 */
EXPORT_SYMBOL(balong_charge_ioctl);

static const struct file_operations balong_charge_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= balong_charge_ioctl,  /*lint !e64 */
	.open		= balong_charge_open,
	.release		= balong_charge_release,
};

static struct miscdevice charge_miscdev = {
	.name	= "balong_charge_dev",
	.fops		= &balong_charge_fops
};

#if (FEATURE_OTG == FEATURE_ON) 
void  get_extchg_AF10_status(EXTCHAGRE_STATE_T * arg)
{
    printk("get_extchg_AF10_status  :entry ok !!  \n "); // UT测试用打印
    unsigned int lint_ret;
    AF10_STATE_ENUM_LIST lint_AF10Sta = AF10_STATE_OUT;
    lint_ret = *(volatile BSP_U32 *)MEMORY_AXI_AF10_ADDR;
    if(AF10_STATE_UNPLUG == lint_ret)
    {
        lint_AF10Sta = AF10_STATE_OUT;
    }
    else if(AF10_STATE_PLUGIN == lint_ret)
    {
        lint_AF10Sta = AF10_STATE_IN;
    }
    else if(AF10_DEVICE_ATTACH == lint_ret)
    {
        lint_AF10Sta = AF10_DEVICE_IN;
    }
    else if(AF10_DEVICE_REMOVE == lint_ret)
    {
        lint_AF10Sta = AF10_DEVICE_OUT;
    }
    else
    {
    }

    printk("get_extchg_AF10_status  :AF10 state= %d  \n ",lint_AF10Sta); // UT测试用打印
    printk("get_extchg_AF10_status  :extchg_state= %d  \n ",g_ACorExtChargingStateFlag); // UT测试用打印
    arg->extAF10_state = lint_AF10Sta;
    arg->extcharging_state = g_ACorExtChargingStateFlag;
    
    return ;
}

static BSP_S32 balong_extcharge_open(struct inode *inode, struct file *file)
{
    if (NULL != inode && NULL != file)
    {
        ;
    }
    return 0;
}

static BSP_S32 balong_extcharge_release(struct inode *inode, struct file *file)
{
    if (NULL != inode && NULL != file)
    {
        ;
    }
    return 0;
}

long balong_extcharge_ioctl(struct file *file, unsigned int cmd, unsigned long  arg)
{
    printk("balong_extcharge_ioctl  :entry ok !!  \n ");
    if (NULL == file)
    {
        ;
    }
    
    get_extchg_AF10_status((EXTCHAGRE_STATE_T *)arg);

    return 0;
}

static const struct file_operations balong_extcharge_fops = {
    .owner    = THIS_MODULE,
    .unlocked_ioctl    =  balong_extcharge_ioctl,
    .open    = balong_extcharge_open,
    .release    = balong_extcharge_release,
};

static struct miscdevice extcharge_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "balong_AF10_dev",
    .fops  = &balong_extcharge_fops
};
#endif
/*****************************************************************************
* 函 数 名  :chg_dok_detect_handler
*
* 功能描述  : DOK GPIO中断处理任务
* 输入参数  :无
* 输出参数  :无
* 返回值：  :成功/失败
* 其它说明  : 无
*
****************************************************************************/  
BSP_S32 chg_dok_detect_handler(void)   
{
	BSP_S32 gpio_value;
	BSP_U32 msg;
	BSP_U32 DOK_ON_OFF_ADDR = MEMORY_AXI_USB_INOUT_ADDR;

	gpio_value = gpio_get_value(BALONG_GPIO_DOK_DETECT);

	if (DOK_GPIO_OFFLINE == gpio_value)   /*拔出中断*/
	{
		gpio_value = gpio_get_value(BALONG_GPIO_DOK_DETECT);
		if (DOK_GPIO_OFFLINE != gpio_value) 
		{
			gpio_int_unmask_set(BALONG_GPIO_DOK_DETECT);
			printk("chg_dok_detect_handler: invalid interrupt!\n");
			return BSP_ERROR;
		}

		*(BSP_U32*)(DOK_ON_OFF_ADDR) = PMU2USB_C_FLAG|CHARGER_OFFLINE;
		printk("+++USB REMOVE:DOK_VALUE:%d+++\n",gpio_value);
		msg = CHARGER_OFFLINE;

		(void)gpio_int_trigger_set(BALONG_GPIO_DOK_DETECT, IRQ_TYPE_LEVEL_LOW); 
	}
	else  /*插入中断*/
	{
		gpio_value = gpio_get_value(BALONG_GPIO_DOK_DETECT);
		if(DOK_GPIO_ONLINE != gpio_value) 
		{
			(void)gpio_int_unmask_set(BALONG_GPIO_DOK_DETECT);
			printk("chg_dok_detect_handler: invalid interrupt!\n");
			return BSP_ERROR;
		} 

		*(BSP_U32*)(DOK_ON_OFF_ADDR) = PMU2USB_C_FLAG|CHARGER_ONLINE;
		printk("---USB INSERT:DOK_VALUE:%d---\n",gpio_value);
		msg = CHARGER_ONLINE;		
		(void)gpio_int_trigger_set(BALONG_GPIO_DOK_DETECT,IRQ_TYPE_LEVEL_HIGH);
	}	 
	
	/*充电器类型识别*/
	
	(void)gpio_int_unmask_set(BALONG_GPIO_DOK_DETECT);
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  :chg_dok_detect_irq
*
* 功能描述  : DOK GPIO中断处理函数
* 输入参数  :无
* 输出参数  :无
* 返回值：  :
* 其它说明  : 无
*
****************************************************************************/  
static irqreturn_t chg_dok_detect_irq(int irq, void *dev_id)	 
{	
	BSP_U32 gpio_value = 0; 

	/*判断是否为DOK中断*/
	(void)gpio_int_state_get(BALONG_GPIO_DOK_DETECT, (unsigned*)&gpio_value);
	if (!gpio_value)
	{	
		printk(KERN_DEBUG "gpio_2_14 value :%d,return IRQ_NONE\n",gpio_value);
		return IRQ_NONE;
	}
	
	/*锁中断*/
	(void)gpio_int_mask_set(BALONG_GPIO_DOK_DETECT);
	
	(void)gpio_int_state_clear(BALONG_GPIO_DOK_DETECT);

#if defined (FEATURE_LCD_ON)
	BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
	/*清状态*/
	schedule_work(&dok_detect_work);

	return IRQ_HANDLED;
}

/*****************************************************************************
* 函 数 名  :chg_dok_detect_init
*
* 功能描述  : DOK GPIO中断挂接
* 输入参数  :无
* 输出参数  :无
* 返回值：  :成功/失败
* 其它说明  : 无
*
****************************************************************************/ 
BSP_S32 chg_dok_detect_init(void)  
{
	BSP_S32 error = 0;  
	BSP_S32 DOK_Stat = DOK_GPIO_OFFLINE;
	BSP_U32 DOK_ON_OFF_ADDR = MEMORY_AXI_USB_INOUT_ADDR;

	if(gpio_request(BALONG_GPIO_DOK_DETECT, "DOK_DETECT"))
	{
		printk(KERN_ERR "dok_detect_gpio requests failed\n"); 	 
		return BSP_ERROR;
	}

	INIT_WORK(&dok_detect_work, (void*)chg_dok_detect_handler);

	(void)gpio_int_mask_set(BALONG_GPIO_DOK_DETECT);
	
	(void)gpio_direction_input(BALONG_GPIO_DOK_DETECT);	
	
	/*判断 USB/充电器是否在位*/
	DOK_Stat = gpio_get_value(BALONG_GPIO_DOK_DETECT);
	if (DOK_GPIO_ONLINE == DOK_Stat)
	{	  
		/*充电器在位，DOK为0，设置上升沿触发*/		
		printk("  DOK_ONLINE   \n");
		*(BSP_U32*)(DOK_ON_OFF_ADDR) = PMU2USB_C_FLAG|CHARGER_ONLINE;		
		(void)gpio_int_trigger_set(BALONG_GPIO_DOK_DETECT,IRQ_TYPE_LEVEL_HIGH);
	}
	else
	{ 
		/*充电器不在位，DOK为1，设置下降沿触发*/	
		printk("  DOK_OFFLINE   \n");
		*(BSP_U32*)(DOK_ON_OFF_ADDR) = PMU2USB_C_FLAG|CHARGER_OFFLINE;
		(void)gpio_int_trigger_set(BALONG_GPIO_DOK_DETECT,IRQ_TYPE_LEVEL_LOW);
	}

	(void)gpio_set_function(BALONG_GPIO_DOK_DETECT,GPIO_INTERRUPT);

	error = request_irq(INT_GPIO_DOK, (irq_handler_t) chg_dok_detect_irq,IRQF_SHARED ,"DOK_DETECT","DOK_DETECT");
	if (error) {
		printk(KERN_ERR "dok_detect_irq install failed\n"); 	 
		return BSP_ERROR;			 
	}

	/*清除中断/使能中断*/
	(void)gpio_int_state_clear(BALONG_GPIO_DOK_DETECT);
	(void)gpio_int_unmask_set(BALONG_GPIO_DOK_DETECT);

	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  :chg_get_dok_status
*
* 功能描述  : 获取DOK GPIO值
* 输入参数  :无
* 输出参数  :无
* 返回值：  :成功/失败
* 其它说明  : 无
****************************************************************************/ 
BSP_S32 chg_get_dok_status(void)  
{
	BSP_S32 DOK_Stat = 1;
	
	DOK_Stat = gpio_get_value(BALONG_GPIO_DOK_DETECT);
	printk("DOK_Stat %d\n",DOK_Stat);
	
	return DOK_Stat;
}

 /*****************************************************************************
 * 函 数 名  : BSP_CHGC_Init
 *
 * 功能描述  : CHGC初始化函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 其它说明  : 无
 *
 *****************************************************************************/
BSP_S32 BSP_CHG_Init(void) 
{
    BSP_S32 s32Ret1 = 0;
    BSP_S32 s32Ret2 = 0;
    BSP_S32 s32Ret3 = 0;
    BSP_S32 s32Ret4 = 0;
    BSP_S32 s32Ret5 = 0;

    #if (FEATURE_OTG == FEATURE_ON) 
    BSP_S32 u32Ret6 = 0;
    BSP_S32 s32Ret7 = 0;
    #endif

    
    s32Ret1 = (BSP_S32)BSP_IFCP_RegFunc(IFCP_FUNC_CHGC_ReportStaToAPP, (BSP_IFCP_REG_FUNC)BSP_CHGC_ReportStaToAPP); 
    s32Ret2 = (BSP_S32)BSP_IFCP_RegFunc(IFCP_FUNC_CHGC_CallUsb, (BSP_IFCP_REG_FUNC)BSP_CHGC_CallUsb);               
    s32Ret3 = (BSP_S32)BSP_IFCP_RegFunc(IFCP_FUNC_CHGC_GetInterface, (BSP_IFCP_REG_FUNC)BSP_CHGC_GetInterface);     
    s32Ret3 = (BSP_S32)BSP_IFCP_RegFunc(IFCP_FUNC_CHGC_SendOledIoctrl, (BSP_IFCP_REG_FUNC)BSP_CHGC_ReportToOled);   	
    s32Ret4 = misc_register(&charge_miscdev);
	
    s32Ret5 = (BSP_S32)BSP_IFCP_RegFunc(IFCP_FUNC_CHGC_TBAT_SUPL, (BSP_IFCP_REG_FUNC)BSP_CHGC_BattSply);            
        
    #if (FEATURE_OTG == FEATURE_ON) 
    u32Ret6 = BSP_IFCP_RegFunc(IFCP_FUNC_CHGC_SendUsbExtChgIoctrl, (BSP_IFCP_REG_FUNC)BSP_CHGC_ReportToUsb);
    s32Ret7 = misc_register(&extcharge_miscdev);
    #endif
    (void)BSP_CHG_ChargeEvent(0) ;   
    
    #if (FEATURE_OTG == FEATURE_ON) 
    if((BSP_OK != s32Ret1) || (BSP_OK != s32Ret2) || (BSP_OK != s32Ret3) \
        || (BSP_OK != s32Ret4 ) || (BSP_OK != u32Ret6) || (BSP_OK != s32Ret7) )
    #else 
    if((BSP_OK != s32Ret1)||(BSP_OK != s32Ret2)||(BSP_OK != s32Ret3)||BSP_OK != s32Ret4)
    #endif
    {
        if(BSP_OK != s32Ret1)
        {
            printk(KERN_ERR "CHGC init, BSP_CHG_SEND_APP_STA failed.\n");
        }
        
        if(BSP_OK != s32Ret2)
        {
            printk(KERN_ERR "CHGC init, Chg_Call_Usb failed.\n");
        }
        
        if(BSP_OK != s32Ret3)
        {
            printk(KERN_ERR "CHGC init, CHG_NAS_Interface failed.\n");
        }
        if(BSP_OK != s32Ret4)
        {
            printk(KERN_ERR "CHGC init, misc_register failed.\n");
        }
        
        return BSP_ERROR;        
    }
	
    printk(KERN_DEBUG "CHGC init:ok.\n");
    return BSP_OK;
    
}/*lint !e550*/
#if (FEATURE_OTG == FEATURE_ON) 

extern void dwc_otg_discharge_ctrl(CTRL_DISCHARGE_ENUM_LIST discharge_status);


/*****************************************************************************
 函 数 名  : BSP_GetExtChgSta
 功能描述  : 软件先检查USB ID检测GPIO_7_7的电平,如果是高电平,则判定对外充电功能测试失败;
 如果是低电平,则软件打开对外供电,由于VBUS对MAX8903供电,VL输出电压基本与VBUS一致,
 软件通过HADC9来检测VL输出电压,如果检测到的电压值在正常范围内(正常电压范围由硬件提供),
 则判定对外充电功能成功，否则判定失败。
 输入参数  :无
 输出参数  :无
 返回值： 1-模拟对外充电成功; 非1-模拟对外充电失败
*****************************************************************************/
BSP_S32 BSP_GetExtChgSta(void)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;
    
    s32ret_send = BSP_CHGC_AskInterface(GET_EXT_CHG_STA,0);
    
    if(BSP_OK == s32ret_send)
    {
        s32ret_rece = BSP_CHGC_Wait(GET_EXT_CHG_STA);
        if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA3[GET_EXT_CHG_STA])
        {
            return CHG_DATA1[GET_EXT_CHG_STA];
        }
        else
        {
            return BSP_ERROR;            
        }
    }
    
    else
    {
        return BSP_ERROR;
    }
}
/*****************************************************************************
* 函 数 名  :BSP_CHGC_ExtChgOnOff
* 功能描述:对外充电的控制开关函数
* 输入参数:enExtChargSwitch 对外充电的开关标志
* 输出参数:无
* 返回值：无
* 其它说明  : 无
****************************************************************************/
void BSP_CHGC_ExtChgOnOff(CTRL_DISCHARGE_ENUM_LIST enExtChargSwitch)
{
    CTRL_DISCHARGE_ENUM_LIST uExtChargSwitch;
    uExtChargSwitch = enExtChargSwitch;

    printk("BSP_CHGC_ExtChgOnOff  :uExtChargSwitch = %d  \n ",uExtChargSwitch); // UT测试用打印
    
    if( CTRL_DISCHARGE_ON ==  uExtChargSwitch)
    {
        dwc_otg_discharge_ctrl(CTRL_DISCHARGE_ON);
        printk("BSP_CHGC_ExtChgOnOff  :turn on  ext-charge \n "); 
    }
    else
    {
        dwc_otg_discharge_ctrl(CTRL_DISCHARGE_OFF);
        printk("BSP_CHGC_ExtChgOnOff  :turn off  ext-charge \n "); 
    }
}
/*****************************************************************************
* 函 数 名  :BSP_CHGC_GetExtChgVoltTempSta
* 功能描述  : BSP_CHG_GetExtChgConditon在A核的映射函数，获取对外充电条件的检测结果。
* 输入参数  :battery_state 电量信息
* 输出参数  :battery_state 电量信息
* 返回值：   0 操作成功
           -1 操作失败
* 其它说明  : 无
****************************************************************************/
BSP_S32 BSP_CHGC_GetExtChgVoltTempSta(void)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;

    printk("BSP_GetExtChgVoltTempSta entry ok \n "); // UT测试用打印    
    
    s32ret_send = BSP_CHGC_AskInterface(GET_EXT_CHG_VOLT_TEMP_STA,0);

    printk("BSP_CHGC_GetExtChgVoltTempSta  :s32ret_send = %d  \n ",s32ret_send); // UT测试用打印
    
    if(BSP_OK == s32ret_send)
    {
        s32ret_rece = BSP_CHGC_Wait(GET_EXT_CHG_VOLT_TEMP_STA);

         printk("BSP_GetExtChgVoltTempSta  :s32ret_rece = %d  \n ",s32ret_rece); // UT测试用打印
        if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA3[GET_EXT_CHG_VOLT_TEMP_STA])
        {
            return CHG_DATA1[GET_EXT_CHG_VOLT_TEMP_STA];
        }    
        else
        {
            printk("BSP_GetExtChgVoltTempSta  :,recieve message fail  \n "); // UT测试用打印
            return BSP_ERROR;            
        }
    }  
    else
    {
        return BSP_ERROR;
    }

}
/*****************************************************************************
 函 数 名  : BSP_CHGC_ReportToUsb
 功能描述  : A核接收C核发送的温保护状态，并根据其状态控制 OTG的对外充电开关
 输入参数  :无
 输出参数  :无
 返回值:无
*****************************************************************************/
void BSP_CHGC_ReportToUsb(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 cmd;
    BSP_U32 arg;
    
    cmd = (*(volatile BSP_U32 *)((BSP_U32)pMsgBody));
    arg =  (*(volatile BSP_U32 *)((BSP_U32)pMsgBody + sizeof(BSP_U32)));

    printk("BSP_CHGC_ReportToUsb :ext-charger state = %d\n ",cmd); // UT测试用打印
      /*对外充电状态参数合法性检查*/
    if((CHG_EXTCHAGRE_STATE_STOP > cmd) || (CHG_EXTCHAGRE_STATE_MAX <= cmd))
    {
        return;
    }
    else
    {
        g_ACorExtChargingStateFlag = cmd;
    }
     
    if(CHG_EXTCHAGRE_STATE_START == cmd)
    {
        BSP_CHGC_ExtChgOnOff(CTRL_DISCHARGE_ON);
    }
    else 
    {
        BSP_CHGC_ExtChgOnOff(CTRL_DISCHARGE_OFF);
    }
    
}
/*****************************************************************************
 函 数 名  : BSP_CHGC_ExtCharger_event_api
 功能描述  : 提供给USB OTG驱动的接口函数，当有AF10插拔时，USB调用此接口函数
 输入参数  :无
 输出参数  :无
 返回值:无
*****************************************************************************/
void BSP_CHGC_ExtCharger_event_api()
{
    printk("chg_extcharger_event_api entry ok\n "); // UT测试用打印

    BSP_S32 uResult = 0;

    /*查询对外充电的条件检测*/
    uResult = BSP_CHGC_GetExtChgVoltTempSta();

     printk("chg_extcharger_event_api  : uResult = %d  \n ",uResult);// UT测试用打印

    /*对外充电状态返回参数合法性检查*/
    if((CHG_EXTCHAGRE_STATE_STOP > uResult) || (CHG_EXTCHAGRE_STATE_MAX <= uResult))
    {
        return;
    }
    else
    {
        g_ACorExtChargingStateFlag = uResult;
    }

    /*由于AF10插拔时USB自动打开和关闭对外充电，
    此处我们只对插入AF10后，检查 温度为温保护引起的停止对外充电做处理*/
    if(CHG_EXTCHAGRE_STATE_TEMP == uResult)
    {
        BSP_CHGC_ExtChgOnOff(CTRL_DISCHARGE_OFF);
    }
    
}

#endif

/*****************************************************************************
* 函 数 名  : BSP_CHGC_Wait
*
* 功能描述  : 等待核间通信完成
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_CHGC_Wait(BSP_U32 MsgType)
{
    BSP_U32 u32module = 1 << (BSP_U32)MsgType;
    BSP_U32 u32flag = 1;
    BSP_U32 i;

    /* 等待，直到发出的请求的函数的到了响应 */
    for(i=0; i<BSP_CHGC_TIMEOUT; i++)
    {
        if(g_u32chgcStatus & u32module)
        {
            printk(KERN_DEBUG "BSP_CHGC_Wait:BSP_OK  \n ");         
            break;
        }
        
        if(u32flag)
        {
            printk(KERN_DEBUG "###### module %d waiting", MsgType);
            u32flag = 0;
        }

        if(0 == (i%20))
        {
            printk(".");
        }

        msleep(20);
    }

    printk("\n");

    if(BSP_CHGC_TIMEOUT == i)
    {
        g_u32chgcStatus=(g_u32chgcStatus&(~u32module));
        return BSP_ERROR;
    }   
    else
    {
        g_u32chgcStatus=(g_u32chgcStatus&(~u32module));
        return BSP_OK;
    }
    
}

/*****************************************************************************
* 函 数 名  :USB_CHGC_REPLY
*
* 功能描述  : A核中的USB模块回复C核CHG_USB_SEND得到的状态和charge event
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHGC_UsbReply(BSP_U32 Msg_Type,BSP_U32 usb_msg,BSP_U32 Error_msg)
{
    BSP_U32 s32ret; 
    BSP_U32 msg[5];

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_CHGC_GetUsbReply;
    msg[2] = Msg_Type;
    msg[3] = usb_msg;
    msg[4] = (BSP_U32)Error_msg;
    s32ret = BSP_IFC_Send((BSP_VOID*)msg, sizeof(msg));

    
    if(BSP_OK != s32ret)
    {
        printk(KERN_ERR "BSP_CHGC_UsbReply type:%d ,  send ifc failed. ret=0x%x\n",msg[2] ,s32ret);
        printk(KERN_ERR "UsbReply:BSP_ERROR  \n ");        
        return BSP_ERROR;
    }
    else
    {
        printk(KERN_DEBUG "BSP_CHGC_UsbReply type:%d ,usb_msg:%d ,  ERROR_MSG=0x%x\n",msg[2] ,msg[3] ,msg[4]);  
        return BSP_OK;
    }
}
/*****************************************************************************
* 函 数 名  : BSP_CHGC_ReportStaToAPP
*
* 功能描述  :被C核调用的，把充电状态发送到APP的函数
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : 1&0
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHGC_ReportStaToAPP(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 dev_id;
    BSP_U32 evt_id;
#ifdef FEATURE_VOLT_PERCENT
    BSP_S32 ret    = BSP_OK;
    BSP_U32 size   = 0;
    battery_state_std  * bstat = BSP_NULL;
    DEVICE_EVENT       * event   = BSP_NULL;
#endif
    
    dev_id = (*(volatile BSP_U32 *)((BSP_U32)pMsgBody));
    evt_id =  (*(volatile BSP_U32 *)((BSP_U32)pMsgBody + sizeof(BSP_U32)));

	/*如果是电池低电事件或者是过温事件,投反对票*/
	if(((DEVICE_ID_BATTERY == dev_id) && (BAT_LOW_POWER == evt_id))
		||(DEVICE_ID_TEMP == dev_id))
	{
#if defined (FEATURE_LCD_ON)
		BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
	}
#ifdef FEATURE_VOLT_PERCENT
    if (BAT_CHARGING_PERCENT == (BATTERY_EVENT)evt_id)
    {
        size  = sizeof(DEVICE_EVENT) + sizeof(battery_state_std);
        event = (DEVICE_EVENT *)kmalloc(size, GFP_KERNEL);

        if ( NULL == event)
        {
            CHG_ERROR("kmalloc failed \n");
            return BSP_ERROR;
        }

        event->len        = sizeof(battery_state_std); 
        event->device_id  = (DEVICE_ID)dev_id;
        event->event_code = (BSP_S32)evt_id;
        
        bstat                  = event->data;
        bstat->charge_state    = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 2 * sizeof(BSP_U32)));
        bstat->battery_percent = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 3 * sizeof(BSP_U32)));

        ret = device_event_report(event, size);
        if ( ret < 0 )
        {
            CHG_ERROR("device_event_report failed err:%d\n",ret);
            kfree(event);
            return ret;
        }

        /*report success */
        CHG_INFO("devId:%d,eventId:%d evenLen:%d dataLen:%d chargstat:%d  bat_percent:%d\n ",
                  event->device_id, event->event_code, size, event->len,
                  bstat->charge_state, bstat->battery_percent);  

        kfree(event);
    }
    else
    {
        /*APP状态上报接口打桩函数*/
        if( SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode() && BAT_LOW_POWER == evt_id)
        {
            dev_id = DEVICE_ID_KEY;
		    evt_id = GPIO_KEY_POWER_OFF;
        }
        DRV_APP_EVENT(dev_id, evt_id);

        //CHG_INFO("ReportStaToAPP:BSP_OK,DEVICE_ID=%d,EVENT_ID=%d  \n ",u32_DEVICE_ID,u32_EVENT_ID); // UT测试用打印 
    }
    
    return BSP_OK;
#else
    /*APP状态上报接口*/
    if( SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode() && BAT_LOW_POWER == evt_id)
    {
        dev_id = DEVICE_ID_KEY;
		evt_id = GPIO_KEY_POWER_OFF;
	
		DRV_APP_EVENT(dev_id, evt_id);
		printk(KERN_ERR "ReportStaToAPP:BSP_OK,DEVICE_ID=%d,EVENT_ID=%d  \n ",dev_id,evt_id);
	}
	else
    {
        DRV_APP_EVENT(dev_id, evt_id);
     
        printk(KERN_ERR "ReportStaToAPP:BSP_OK,DEVICE_ID=%d,EVENT_ID=%d   \n ",dev_id,evt_id);
    }

    return OK;
#endif
}

/*****************************************************************************
* 函 数 名  : BSP_CHGC_CallUsb
*
* 功能描述  :被C核的充电模块调用的，查询USB的函数
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : 1&0
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHGC_CallUsb(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 u32MsgType = 0;

    u32MsgType = (*(volatile BSP_U32 *)((BSP_U32)pMsgBody));

    if(USB_ENUM_COM==u32MsgType)
    {
        (void)CHG_Get_UsbEnum();
        printk(KERN_DEBUG "CallUsb:Type=USB_ENUM_COM  \n ");         
        return BSP_OK;
    }
    else if(USB_CHG_TYP==u32MsgType)
    {
        (void)usb_charge_type_get();
        printk(KERN_DEBUG "CallUsb:Type=USB_CHG_TYP  \n ");      
        return BSP_OK;
    }
    else
    {
        (void)BSP_CHGC_UsbReply(u32MsgType,BSP_OK,(BSP_U32)BSP_ERROR);
        printk(KERN_DEBUG "CallUsb:Type=ERROR  \n ");      
        return BSP_ERROR;
    }
}

/*****************************************************************************
* 函 数 名  :BSP_CHGC_AskInterface
*
* 功能描述  : A核向C核发送状态请求
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHGC_AskInterface(BSP_U32 Msg_Type,BSP_U32 u32_msg)
{
    BSP_U32 u32send_ret; 
    BSP_U32 msg[4];

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_CHGC_InterfacePro;
    msg[2] = Msg_Type;
    msg[3] = u32_msg;
    
    u32send_ret = BSP_IFC_Send((BSP_VOID*)msg, sizeof(msg));
     
    if(BSP_OK != u32send_ret)
    {
        printk(KERN_ERR "BSP_CHGC_AskInterface type:%d , u32_msg:%d send ifc failed. ret=0x%x\n",msg[2] ,msg[3],u32send_ret);
        printk(KERN_ERR "AskInterface:BSP_ERROR  \n ");    
        return BSP_ERROR;
    }
    
    printk(KERN_DEBUG "BSP_CHGC_AskInterface type:%d , u32_msg:%d \n",msg[2] ,msg[3]);
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  :BSP_CHGC_GetInterface
*
* 功能描述  : 在调用C核函数后，获取C核的返回值
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
****************************************************************************/
BSP_U32 BSP_CHGC_GetInterface(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_S32 u32MsgType = 0;	
	static DEFINE_SPINLOCK(msg_recv_lock);
	
    spin_lock(&msg_recv_lock);
    u32MsgType = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody));

    if( 0 < u32MsgType && u32MsgType < GET_CHG_DATA_MAX)
    {
        CHG_DATA1[u32MsgType] = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + sizeof(BSP_U32)));
        CHG_DATA2[u32MsgType] = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 2*sizeof(BSP_U32)));
        CHG_DATA3[u32MsgType] = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 3*sizeof(BSP_U32)));
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
        /*第4个成员*/
        CHG_DATA4[u32MsgType] = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 4*sizeof(BSP_U32)));
#endif
        printk(KERN_DEBUG "GetInterface:BSP_OK,GetInterface:%d, CHG_DATA1[MsgType] =%d, CHG_DATA2[MsgType] =%d,CHG_DATA3[MsgType] =%d \n ", 
            u32MsgType,CHG_DATA1[u32MsgType],CHG_DATA2[u32MsgType],CHG_DATA3[u32MsgType]);/*lint !e409*/ /*lint !e515*/ 
    }
    else
    {
	    CHG_MSG1 = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + sizeof(BSP_U32)));
    	CHG_MSG2 = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 2*sizeof(BSP_U32)));
    	CHG_MSG3 = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 3*sizeof(BSP_U32)));
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
        /*第4个成员*/
        CHG_MSG4 = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 4 * sizeof(BSP_U32)));
#endif

#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
        printk(KERN_DEBUG "GetInterface:BSP_OK,GetInterface:%d,CHG_MSG1=%d,CHG_MSG2=%d,CHG_MSG3=%d,CHG_MSG4=%d,\
            g_u32chgcStatus=%d  \n ",u32MsgType,CHG_MSG1,CHG_MSG2,CHG_MSG3,CHG_MSG4,g_u32chgcStatus); // UT测试用打印 	   
#else
        printk(KERN_DEBUG "GetInterface:BSP_OK,GetInterface:%d,CHG_MSG1=%d,CHG_MSG2=%d,CHG_MSG3=%d,g_u32chgcStatus=%d  \n ",u32MsgType,CHG_MSG1,CHG_MSG2,CHG_MSG3,g_u32chgcStatus); // UT测试用打印 	   
#endif
    }
	
    g_u32chgcStatus|=(1<<u32MsgType);	
	
	spin_unlock(&msg_recv_lock);
    return BSP_OK;
}


/*****************************************************************************
* 函 数 名  :BSP_CHG_ChargeEvent
*
* 功能描述  : 查询目前是否正在充电中
* 输入参数  :无
* 输出参数  :无
* 返回值：   
*
* 其它说明  : 无
*
****************************************************************************/
void BSP_CHG_ChargeEvent(int charger_status)
{
    (void)BSP_CHGC_AskInterface(ChargeEvent,(BSP_U32)charger_status); 
    
}

/*****************************************************************************
* 函 数 名  :BSP_CHGC_ChargingStatus
*
* 功能描述  : chg_get_charging_status在A核的映射函数,查询目前是否正在充电中
* 输入参数  :无
* 输出参数  :无
* 返回值：   0 未充电
*                         1 充电中
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHG_ChargingStatus(void)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;
    int send_ui_charging_start;
    
    s32ret_send = BSP_CHGC_AskInterface(GET_CHG_STA,0); 
    
    if(BSP_OK == s32ret_send)
    {
        s32ret_rece = BSP_CHGC_Wait(GET_CHG_STA);
        #ifdef FEATURE_HUAWEI_MBB_CHG
        if(BSP_OK == s32ret_rece )
        #else
        if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA3[GET_CHG_STA])
        #endif
        {
            send_ui_charging_start = CHG_DATA1[GET_CHG_STA];
            printk(KERN_DEBUG "charging_start:%d  \n ",send_ui_charging_start); 
            return send_ui_charging_start;
        }
        else
        {
            printk(KERN_ERR "charging_start:ERROR2  \n "); 
            return BSP_ERROR;
        }
    }
    else
    {
        printk(KERN_ERR "charging_start:ERROR1  \n "); 
        return BSP_ERROR;
    }
}

/*****************************************************************************
* 函 数 名  :BSP_CHG_GetBatteryState
*
* 功能描述  : app_get_battery_state在A核的映射函数,获取底层电池状态信
* 输入参数  :battery_state 电量信息
* 输出参数  :battery_state 电量信息
* 返回值：   0 操作成功
                         -1操作失败
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHG_GetBatteryState(BATT_STATE_T *battery_state)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;
	static BATT_STATE_T g_battery_state = {0};

/*Make GET_BAT_STA atomic, reentry is NOT allowed for IFC call.*/
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
    static DEFINE_MUTEX(batt_state_get_lock);
    mutex_lock(&batt_state_get_lock);
#endif /*((defined(CHIP_BB_6920ES) |...*/
    s32ret_send = BSP_CHGC_AskInterface(GET_BAT_STA,0);

    if(BSP_OK == s32ret_send)
    {
        s32ret_rece = BSP_CHGC_Wait(GET_BAT_STA);
        #ifdef FEATURE_HUAWEI_MBB_CHG
        if(BSP_OK == s32ret_rece )
        #else
        if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA3[GET_BAT_STA])
        #endif
        {
            battery_state->charging_state = CHG_DATA1[GET_BAT_STA];   /*lint !e64 */
            battery_state->battery_level = CHG_DATA2[GET_BAT_STA];    /*lint !e64 */
            #ifdef FEATURE_HUAWEI_MBB_CHG
            battery_state->batt_temp_state = CHG_DATA3[GET_BAT_STA];
            #endif
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
            battery_state->batt_percent = CHG_DATA4[GET_BAT_STA];/*lint !e10*//*lint !e63*/
#endif

			g_battery_state.charging_state = CHG_DATA1[GET_BAT_STA];
			g_battery_state.battery_level = CHG_DATA2[GET_BAT_STA];
            #ifdef FEATURE_HUAWEI_MBB_CHG
            g_battery_state.batt_temp_state = CHG_DATA3[GET_BAT_STA];
            #endif
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
            g_battery_state.batt_percent = CHG_DATA4[GET_BAT_STA];/*lint !e63*//*lint !e10*/
#endif
            printk(KERN_DEBUG "battery_state:%d ,%d \n ",battery_state->charging_state,battery_state->battery_level); 
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
            mutex_unlock(&batt_state_get_lock);
#endif /*((defined(CHIP_BB_6920ES) |...*/
            return BSP_OK;
        }        
        else
        {
		    battery_state->charging_state = g_battery_state.charging_state;
            battery_state->battery_level = g_battery_state.battery_level;        	
            #ifdef FEATURE_HUAWEI_MBB_CHG
            battery_state->batt_temp_state = g_battery_state.batt_temp_state;
            #endif
            printk(KERN_ERR "battery_state:ERROR2  \n "); // UT测试用打印
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
            mutex_unlock(&batt_state_get_lock);
#endif /*((defined(CHIP_BB_6920ES) |...*/
            return BSP_OK;
        }
    }
    
    else
    {
        printk("battery_state:ERROR1  \n ");
#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON))
        mutex_unlock(&batt_state_get_lock);
#endif /*((defined(CHIP_BB_6920ES) |...*/
        return BSP_ERROR;
    }
}

/*****************************************************************************
* 函 数 名  :BSP_CHG_Appgetbatterystate
*
* 功能描述  : app获取电池状态和充电状态
* 输入参数  :battery_state 电量信息
* 输出参数  :battery_state 电量信息
* 返回值：   0 操作成功
                         -1操作失败
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHG_Appgetbatterystate(BATT_STATE_T *battery_state)
{
    BATT_STATE_T sBatInfo;
    memset(&sBatInfo, 0, sizeof(sBatInfo));  /*lint !e522*/
    BSP_CHG_GetBatteryState(&sBatInfo);
    copy_to_user(battery_state,&sBatInfo , sizeof(BATT_STATE_T));

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  :BSP_CHG_Appgetusbstate
*
* 功能描述  : app获取USB状态
* 输入参数  :USB状态
* 输出参数  :USB状态
* 返回值：   0 操作成功
             -1操作失败
*
* 其它说明  : 无
*
****************************************************************************/
BSP_S32 BSP_CHG_Appgetusbstate(CHG_USB_STATE *usb_status)
{
    CHG_USB_STATE usb_state;
    USB_CHARGE_TYPE_ENUM UsbChgType = USB_CHARGE_TYPE_NON_CHECK;

    if(NULL == usb_status)
    {
        printk(KERN_ERR"the arg is ilegal!\n");
        return BSP_ERROR;
    }
	/*5371/5376 采用8903充电芯片，DOK检测不能再使用*/
    if(1)    /*USB在位*/
    {
        UsbChgType = 1;
        if(USB_CHARGE_TYPE_HW == UsbChgType)    /*标准充电器*/
        {
            usb_state = CHG_USB_HW; 
        }
        else if(USB_CHARGE_TYPE_NON_HW == UsbChgType)    /*非标充电器*/
        {
            usb_state = CHG_USB_NON_HW; 
        }
        else    /*USB类型未识别*/
        {
            return BSP_ERROR;
        }
        
    }
    else    /*USB不在位*/
    {
        usb_state = CHG_USB_NON;
    }
    copy_to_user(usb_status,&usb_state , sizeof(CHG_USB_STATE));

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : BSP_CHG_StateSet
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						  1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
void BSP_CHG_StateSet(unsigned long ulState)
{
    (void)BSP_CHGC_AskInterface((BSP_U32) SET_CHG_STA,(BSP_U32) ulState);
}
/*****************************************************************************
 函 数 名  : BSP_CHG_PollTimerSet
 功能描述  :由A核来设置调整家关机状态下慢定时器的轮询周期
 输入参数  :ulState      
                     0:切换为快定时器
                     1:调整慢定时器周期，降低假关机状态的功耗
 输出参数  :无
 返回值：    无
*****************************************************************************/
void BSP_CHG_PollTimerSet(BSP_U32 ulState)
{
    (void)BSP_CHGC_AskInterface((BSP_U32) SET_CHG_POLL_TIMER_STA, ulState);
}

/*****************************************************************************
 函 数 名  : BSP_CHG_StateGet
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,查询是否需要补电
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
BSP_S32 BSP_CHG_StateGet(void)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;

    s32ret_send = BSP_CHGC_AskInterface(GET_CHG_STA_AT,0);

    if(BSP_OK == s32ret_send)
    {
		s32ret_rece = BSP_CHGC_Wait(GET_CHG_STA_AT);

		if(BSP_OK == s32ret_rece)
		{
			return CHG_DATA1[GET_CHG_STA_AT];
		}

		else
		{
			return BSP_ERROR;
		}
	}
	else
	{
		return BSP_ERROR;
	}
}

/*****************************************************************************
 函 数 名  : BSP_CHG_VoltGet
 功能描述  : 查询C核载放入共享内存里的实时电压值
 输入参数  : NONE
 输出参数  : BSP_U32 电压值
 返回值：
*****************************************************************************/
BSP_U32 BSP_CHG_VoltGet(void)
{	
	st_batt_info_share_mem  batt_info_share_mem;//当前电压值，从共享内存中获取

	/*调用充放电接口查询当前电量*/
	memcpy((void *)(&batt_info_share_mem),(const void *)MEMORY_AXI_CHARGE_ADDR,sizeof(st_batt_info_share_mem));
	printk(KERN_ERR "\r\nBSP_CHG_VoltGet: voltage = %u;\r\n", batt_info_share_mem.batt_current_volt);

	return batt_info_share_mem.batt_current_volt;
}


/*****************************************************************************
 函 数 名  : BSP_CHG_Sply
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,查询是否需要补电
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
BSP_S32 BSP_CHG_Sply( void)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;

#if defined (FEATURE_FLASH_LESS)
	return BSP_OK;
#endif

    s32ret_send = BSP_CHGC_AskInterface(SET_CHG_SPLY,0);

    if(BSP_OK == s32ret_send)
    {
		s32ret_rece = BSP_CHGC_Wait(SET_CHG_SPLY);
		if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA1[SET_CHG_SPLY])
		{
			return BSP_OK;
		}
		else
		{
			return BSP_ERROR;
		}
	}
	else
	{
		return BSP_ERROR;
	}
}

/*****************************************************************************
 函 数 名  : BSP_CHG_GetCbcState
 功能描述  : getCbcState在A核的映射函数，返回电池状态和电量
 输入参数  :pusBcs 0:电池在供电 1:与电池连接电池未供电
                                       2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接，100:剩余电量百分比
 输出参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电
                                       2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接，100:剩余电量百分比
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
BSP_S32 BSP_CHG_GetCbcState(unsigned char *pusBcs,unsigned char *pucBcl)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;

    s32ret_send = BSP_CHGC_AskInterface(GET_CBC_STA,0);

    if(BSP_OK == s32ret_send)
    {
        s32ret_rece = BSP_CHGC_Wait(GET_CBC_STA);
        if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA3[GET_CBC_STA])
        {
            *pusBcs = CHG_DATA1[GET_CBC_STA]; /*lint !e734 */
            *pucBcl = CHG_DATA2[GET_CBC_STA]; /*lint !e734 */
            printk(KERN_DEBUG "CbcState:%d ,%d \n ",CHG_DATA1[GET_CBC_STA],CHG_DATA2[GET_CBC_STA]); 
            return BSP_OK;
        }
        else
        {
            printk(KERN_ERR "GetCbcState:ERROR2  \n "); 
            return BSP_ERROR;            
        }
    }
    else
    {
        printk(KERN_ERR "GetCbcState:ERROR1  \n ");
        return BSP_ERROR;
    }
}


/*****************************************************************************
 函 数 名  : BSP_CHG_StateSet
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						  1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
BSP_S32 BSP_CHGC_ReportToOled(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 cmd;
    BSP_U32 arg;

    cmd = (*(volatile BSP_U32 *)((BSP_U32)pMsgBody));
    arg =  (*(volatile BSP_U32 *)((BSP_U32)pMsgBody + sizeof(BSP_U32)));

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
    balong_tft_ioctl(NULL,cmd, arg);
#else
    balong_oled_ioctl(NULL,cmd, arg);
#endif
    printk(KERN_DEBUG "BSP_CHGC_ReportToOled:BSP_OK,cmd=%d,arg=%d  \n ",cmd,arg);        

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : BSP_CHGC_BattSply
 功能描述  :BSP_CHGC_BattSply在A核的映射函数,开始补电状态
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
BSP_S32 BSP_CHGC_BattSply(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
	BSP_U32 u32BattFlag = 0;
    u32BattFlag = (*(volatile BSP_U32 *)((BSP_U32)pMsgBody));
	if(TBATT_SUPL_MAGIC == (u32BattFlag & 0x0FFFFFFFE))
	{
		g_u32BattSpFlag = (u32BattFlag & 0x1);		
		printk(KERN_DEBUG "BSP_CHGC_BattSply:BSP_OK,u32BattFlag =0x%x\n ",u32BattFlag); 		  
		return BSP_OK;
	}
	else
	{
		return BSP_ERROR;
	}
}

/*****************************************************************************
 函 数 名  : BSP_CHGC_SplyStGet
 功能描述  :BSP_CHGC_SplyStGet获取补电标志
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
BSP_U32 BSP_CHGC_SplyStGet(void)
{
	return  g_u32BattSpFlag;
}

EXPORT_SYMBOL(BSP_CHG_GetCbcState);
EXPORT_SYMBOL(BSP_CHG_GetBatteryState);
EXPORT_SYMBOL(BSP_CHG_ChargingStatus);
EXPORT_SYMBOL(BSP_CHG_StateSet);
EXPORT_SYMBOL(BSP_CHG_Init);
EXPORT_SYMBOL(BSP_CHGC_BattSply);
EXPORT_SYMBOL(BSP_CHGC_SplyStGet);

/* added a dummy init, so, the export is aviable */
static int __init BSP_CHG_dummy_Init( void )
{
	return 0;
}
#if defined (FEATURE_FLASH_LESS)
#else
module_init(BSP_CHG_dummy_Init);
#endif
#else

/*****************************************************************************
 函 数 名  : BSP_CHG_StateGet
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,查询是否需要补电
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
BSP_S32 BSP_CHG_StateGet(void)
{
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : BSP_CHG_Sply
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,查询是否需要补电
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
BSP_S32 BSP_CHG_Sply( void)
{
    return BSP_ERROR;
}

#endif

EXPORT_SYMBOL(BSP_CHG_StateGet);
EXPORT_SYMBOL(BSP_CHG_Sply);

