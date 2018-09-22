/*************************************************************************
*   ��Ȩ����(C) 1987-2010, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_CHGC.c
*
*   ��    �� :  Z00185733
*
*   ��    �� :  ���ģ����A�˵ĺ˼�ͨ��ģ�顣
*
*************************************************************************/
/**********************���ⵥ�޸ļ�¼************************************************
��    ��              �޸���                     ���ⵥ��                   �޸�����
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
 * �궨��                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ⲿ����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ڲ�����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ⲿ��������                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * ȫ�ֱ�������                                                 *
 *--------------------------------------------------------------*/
#define TBATT_SUPL_MAGIC 	0x5543200

/*DOK GPIO���� */
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
#define GPIO_DOK_REG				0x820	/*DOK/FLTʹ������*/

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

/*�������λ��⹤������*/
struct work_struct	dok_detect_work;

extern USB_CHARGE_TYPE_ENUM usb_charge_type(void);
extern unsigned int usb_enum_completed(void);
extern BSP_S32 charger_event_proc(BSP_VOID *pMsgBody,BSP_U32 u32Len);

extern uint32_t  usb_get_charger_status(BSP_VOID);

/*****************************************************************************
* �� �� ��  :usb_charge_type_get
*
* ��������  :IFC�ص�,M�˻�ȡ��������ͽӿ�
* �������  :��
* �������  :��
* ����ֵ��  :�ɹ�/ʧ��
* ����˵��  : ��
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
* �� �� ��  :DRV_APP_EVENT
*
* ��������  :CHG�¼��ϱ��ӿ�
* �������  :CHG_DEVICE_ID:�豸ID��CHG_EVENT_ID:�¼�ID
* �������  :��
* ����ֵ��  :��
* ����˵��  : ��
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
* �� �� ��  :CHG_Get_UsbEnum
*
* ��������  : IFC�ص�������M��USBö��״̬
* �������  :��
* �������  :��
* ����ֵ��  :��
* ����˵��  : ��
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
    printk("get_extchg_AF10_status  :entry ok !!  \n "); // UT�����ô�ӡ
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

    printk("get_extchg_AF10_status  :AF10 state= %d  \n ",lint_AF10Sta); // UT�����ô�ӡ
    printk("get_extchg_AF10_status  :extchg_state= %d  \n ",g_ACorExtChargingStateFlag); // UT�����ô�ӡ
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
* �� �� ��  :chg_dok_detect_handler
*
* ��������  : DOK GPIO�жϴ�������
* �������  :��
* �������  :��
* ����ֵ��  :�ɹ�/ʧ��
* ����˵��  : ��
*
****************************************************************************/  
BSP_S32 chg_dok_detect_handler(void)   
{
	BSP_S32 gpio_value;
	BSP_U32 msg;
	BSP_U32 DOK_ON_OFF_ADDR = MEMORY_AXI_USB_INOUT_ADDR;

	gpio_value = gpio_get_value(BALONG_GPIO_DOK_DETECT);

	if (DOK_GPIO_OFFLINE == gpio_value)   /*�γ��ж�*/
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
	else  /*�����ж�*/
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
	
	/*���������ʶ��*/
	
	(void)gpio_int_unmask_set(BALONG_GPIO_DOK_DETECT);
	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  :chg_dok_detect_irq
*
* ��������  : DOK GPIO�жϴ�����
* �������  :��
* �������  :��
* ����ֵ��  :
* ����˵��  : ��
*
****************************************************************************/  
static irqreturn_t chg_dok_detect_irq(int irq, void *dev_id)	 
{	
	BSP_U32 gpio_value = 0; 

	/*�ж��Ƿ�ΪDOK�ж�*/
	(void)gpio_int_state_get(BALONG_GPIO_DOK_DETECT, (unsigned*)&gpio_value);
	if (!gpio_value)
	{	
		printk(KERN_DEBUG "gpio_2_14 value :%d,return IRQ_NONE\n",gpio_value);
		return IRQ_NONE;
	}
	
	/*���ж�*/
	(void)gpio_int_mask_set(BALONG_GPIO_DOK_DETECT);
	
	(void)gpio_int_state_clear(BALONG_GPIO_DOK_DETECT);

#if defined (FEATURE_LCD_ON)
	BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
	/*��״̬*/
	schedule_work(&dok_detect_work);

	return IRQ_HANDLED;
}

/*****************************************************************************
* �� �� ��  :chg_dok_detect_init
*
* ��������  : DOK GPIO�жϹҽ�
* �������  :��
* �������  :��
* ����ֵ��  :�ɹ�/ʧ��
* ����˵��  : ��
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
	
	/*�ж� USB/������Ƿ���λ*/
	DOK_Stat = gpio_get_value(BALONG_GPIO_DOK_DETECT);
	if (DOK_GPIO_ONLINE == DOK_Stat)
	{	  
		/*�������λ��DOKΪ0�����������ش���*/		
		printk("  DOK_ONLINE   \n");
		*(BSP_U32*)(DOK_ON_OFF_ADDR) = PMU2USB_C_FLAG|CHARGER_ONLINE;		
		(void)gpio_int_trigger_set(BALONG_GPIO_DOK_DETECT,IRQ_TYPE_LEVEL_HIGH);
	}
	else
	{ 
		/*���������λ��DOKΪ1�������½��ش���*/	
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

	/*����ж�/ʹ���ж�*/
	(void)gpio_int_state_clear(BALONG_GPIO_DOK_DETECT);
	(void)gpio_int_unmask_set(BALONG_GPIO_DOK_DETECT);

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  :chg_get_dok_status
*
* ��������  : ��ȡDOK GPIOֵ
* �������  :��
* �������  :��
* ����ֵ��  :�ɹ�/ʧ��
* ����˵��  : ��
****************************************************************************/ 
BSP_S32 chg_get_dok_status(void)  
{
	BSP_S32 DOK_Stat = 1;
	
	DOK_Stat = gpio_get_value(BALONG_GPIO_DOK_DETECT);
	printk("DOK_Stat %d\n",DOK_Stat);
	
	return DOK_Stat;
}

 /*****************************************************************************
 * �� �� ��  : BSP_CHGC_Init
 *
 * ��������  : CHGC��ʼ������
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * ����˵��  : ��
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
 �� �� ��  : BSP_GetExtChgSta
 ��������  : ����ȼ��USB ID���GPIO_7_7�ĵ�ƽ,����Ǹߵ�ƽ,���ж������繦�ܲ���ʧ��;
 ����ǵ͵�ƽ,������򿪶��⹩��,����VBUS��MAX8903����,VL�����ѹ������VBUSһ��,
 ���ͨ��HADC9�����VL�����ѹ,�����⵽�ĵ�ѹֵ��������Χ��(������ѹ��Χ��Ӳ���ṩ),
 ���ж������繦�ܳɹ��������ж�ʧ�ܡ�
 �������  :��
 �������  :��
 ����ֵ�� 1-ģ�������ɹ�; ��1-ģ�������ʧ��
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
* �� �� ��  :BSP_CHGC_ExtChgOnOff
* ��������:������Ŀ��ƿ��غ���
* �������:enExtChargSwitch ������Ŀ��ر�־
* �������:��
* ����ֵ����
* ����˵��  : ��
****************************************************************************/
void BSP_CHGC_ExtChgOnOff(CTRL_DISCHARGE_ENUM_LIST enExtChargSwitch)
{
    CTRL_DISCHARGE_ENUM_LIST uExtChargSwitch;
    uExtChargSwitch = enExtChargSwitch;

    printk("BSP_CHGC_ExtChgOnOff  :uExtChargSwitch = %d  \n ",uExtChargSwitch); // UT�����ô�ӡ
    
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
* �� �� ��  :BSP_CHGC_GetExtChgVoltTempSta
* ��������  : BSP_CHG_GetExtChgConditon��A�˵�ӳ�亯������ȡ�����������ļ������
* �������  :battery_state ������Ϣ
* �������  :battery_state ������Ϣ
* ����ֵ��   0 �����ɹ�
           -1 ����ʧ��
* ����˵��  : ��
****************************************************************************/
BSP_S32 BSP_CHGC_GetExtChgVoltTempSta(void)
{
    BSP_S32 s32ret_send;
    BSP_S32 s32ret_rece;

    printk("BSP_GetExtChgVoltTempSta entry ok \n "); // UT�����ô�ӡ    
    
    s32ret_send = BSP_CHGC_AskInterface(GET_EXT_CHG_VOLT_TEMP_STA,0);

    printk("BSP_CHGC_GetExtChgVoltTempSta  :s32ret_send = %d  \n ",s32ret_send); // UT�����ô�ӡ
    
    if(BSP_OK == s32ret_send)
    {
        s32ret_rece = BSP_CHGC_Wait(GET_EXT_CHG_VOLT_TEMP_STA);

         printk("BSP_GetExtChgVoltTempSta  :s32ret_rece = %d  \n ",s32ret_rece); // UT�����ô�ӡ
        if(BSP_OK == s32ret_rece && BSP_OK == CHG_DATA3[GET_EXT_CHG_VOLT_TEMP_STA])
        {
            return CHG_DATA1[GET_EXT_CHG_VOLT_TEMP_STA];
        }    
        else
        {
            printk("BSP_GetExtChgVoltTempSta  :,recieve message fail  \n "); // UT�����ô�ӡ
            return BSP_ERROR;            
        }
    }  
    else
    {
        return BSP_ERROR;
    }

}
/*****************************************************************************
 �� �� ��  : BSP_CHGC_ReportToUsb
 ��������  : A�˽���C�˷��͵��±���״̬����������״̬���� OTG�Ķ����翪��
 �������  :��
 �������  :��
 ����ֵ:��
*****************************************************************************/
void BSP_CHGC_ReportToUsb(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 cmd;
    BSP_U32 arg;
    
    cmd = (*(volatile BSP_U32 *)((BSP_U32)pMsgBody));
    arg =  (*(volatile BSP_U32 *)((BSP_U32)pMsgBody + sizeof(BSP_U32)));

    printk("BSP_CHGC_ReportToUsb :ext-charger state = %d\n ",cmd); // UT�����ô�ӡ
      /*������״̬�����Ϸ��Լ��*/
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
 �� �� ��  : BSP_CHGC_ExtCharger_event_api
 ��������  : �ṩ��USB OTG�����Ľӿں���������AF10���ʱ��USB���ô˽ӿں���
 �������  :��
 �������  :��
 ����ֵ:��
*****************************************************************************/
void BSP_CHGC_ExtCharger_event_api()
{
    printk("chg_extcharger_event_api entry ok\n "); // UT�����ô�ӡ

    BSP_S32 uResult = 0;

    /*��ѯ��������������*/
    uResult = BSP_CHGC_GetExtChgVoltTempSta();

     printk("chg_extcharger_event_api  : uResult = %d  \n ",uResult);// UT�����ô�ӡ

    /*������״̬���ز����Ϸ��Լ��*/
    if((CHG_EXTCHAGRE_STATE_STOP > uResult) || (CHG_EXTCHAGRE_STATE_MAX <= uResult))
    {
        return;
    }
    else
    {
        g_ACorExtChargingStateFlag = uResult;
    }

    /*����AF10���ʱUSB�Զ��򿪺͹رն����磬
    �˴�����ֻ�Բ���AF10�󣬼�� �¶�Ϊ�±��������ֹͣ������������*/
    if(CHG_EXTCHAGRE_STATE_TEMP == uResult)
    {
        BSP_CHGC_ExtChgOnOff(CTRL_DISCHARGE_OFF);
    }
    
}

#endif

/*****************************************************************************
* �� �� ��  : BSP_CHGC_Wait
*
* ��������  : �ȴ��˼�ͨ�����
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_CHGC_Wait(BSP_U32 MsgType)
{
    BSP_U32 u32module = 1 << (BSP_U32)MsgType;
    BSP_U32 u32flag = 1;
    BSP_U32 i;

    /* �ȴ���ֱ������������ĺ����ĵ�����Ӧ */
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
* �� �� ��  :USB_CHGC_REPLY
*
* ��������  : A���е�USBģ��ظ�C��CHG_USB_SEND�õ���״̬��charge event
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
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
* �� �� ��  : BSP_CHGC_ReportStaToAPP
*
* ��������  :��C�˵��õģ��ѳ��״̬���͵�APP�ĺ���
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : 1&0
*
* ����˵��  : ��
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

	/*����ǵ�ص͵��¼������ǹ����¼�,Ͷ����Ʊ*/
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
        /*APP״̬�ϱ��ӿڴ�׮����*/
        if( SHORT_POWER_OFF_MODE_SET == PWRCTRL_GetShortOffMode() && BAT_LOW_POWER == evt_id)
        {
            dev_id = DEVICE_ID_KEY;
		    evt_id = GPIO_KEY_POWER_OFF;
        }
        DRV_APP_EVENT(dev_id, evt_id);

        //CHG_INFO("ReportStaToAPP:BSP_OK,DEVICE_ID=%d,EVENT_ID=%d  \n ",u32_DEVICE_ID,u32_EVENT_ID); // UT�����ô�ӡ 
    }
    
    return BSP_OK;
#else
    /*APP״̬�ϱ��ӿ�*/
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
* �� �� ��  : BSP_CHGC_CallUsb
*
* ��������  :��C�˵ĳ��ģ����õģ���ѯUSB�ĺ���
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : 1&0
*
* ����˵��  : ��
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
* �� �� ��  :BSP_CHGC_AskInterface
*
* ��������  : A����C�˷���״̬����
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
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
* �� �� ��  :BSP_CHGC_GetInterface
*
* ��������  : �ڵ���C�˺����󣬻�ȡC�˵ķ���ֵ
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
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
        /*��4����Ա*/
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
        /*��4����Ա*/
        CHG_MSG4 = (BSP_S32)(*(volatile BSP_U32 *)((BSP_U32)pMsgBody + 4 * sizeof(BSP_U32)));
#endif

#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
        printk(KERN_DEBUG "GetInterface:BSP_OK,GetInterface:%d,CHG_MSG1=%d,CHG_MSG2=%d,CHG_MSG3=%d,CHG_MSG4=%d,\
            g_u32chgcStatus=%d  \n ",u32MsgType,CHG_MSG1,CHG_MSG2,CHG_MSG3,CHG_MSG4,g_u32chgcStatus); // UT�����ô�ӡ 	   
#else
        printk(KERN_DEBUG "GetInterface:BSP_OK,GetInterface:%d,CHG_MSG1=%d,CHG_MSG2=%d,CHG_MSG3=%d,g_u32chgcStatus=%d  \n ",u32MsgType,CHG_MSG1,CHG_MSG2,CHG_MSG3,g_u32chgcStatus); // UT�����ô�ӡ 	   
#endif
    }
	
    g_u32chgcStatus|=(1<<u32MsgType);	
	
	spin_unlock(&msg_recv_lock);
    return BSP_OK;
}


/*****************************************************************************
* �� �� ��  :BSP_CHG_ChargeEvent
*
* ��������  : ��ѯĿǰ�Ƿ����ڳ����
* �������  :��
* �������  :��
* ����ֵ��   
*
* ����˵��  : ��
*
****************************************************************************/
void BSP_CHG_ChargeEvent(int charger_status)
{
    (void)BSP_CHGC_AskInterface(ChargeEvent,(BSP_U32)charger_status); 
    
}

/*****************************************************************************
* �� �� ��  :BSP_CHGC_ChargingStatus
*
* ��������  : chg_get_charging_status��A�˵�ӳ�亯��,��ѯĿǰ�Ƿ����ڳ����
* �������  :��
* �������  :��
* ����ֵ��   0 δ���
*                         1 �����
*
* ����˵��  : ��
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
* �� �� ��  :BSP_CHG_GetBatteryState
*
* ��������  : app_get_battery_state��A�˵�ӳ�亯��,��ȡ�ײ���״̬��
* �������  :battery_state ������Ϣ
* �������  :battery_state ������Ϣ
* ����ֵ��   0 �����ɹ�
                         -1����ʧ��
*
* ����˵��  : ��
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
            printk(KERN_ERR "battery_state:ERROR2  \n "); // UT�����ô�ӡ
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
* �� �� ��  :BSP_CHG_Appgetbatterystate
*
* ��������  : app��ȡ���״̬�ͳ��״̬
* �������  :battery_state ������Ϣ
* �������  :battery_state ������Ϣ
* ����ֵ��   0 �����ɹ�
                         -1����ʧ��
*
* ����˵��  : ��
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
* �� �� ��  :BSP_CHG_Appgetusbstate
*
* ��������  : app��ȡUSB״̬
* �������  :USB״̬
* �������  :USB״̬
* ����ֵ��   0 �����ɹ�
             -1����ʧ��
*
* ����˵��  : ��
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
	/*5371/5376 ����8903���оƬ��DOK��ⲻ����ʹ��*/
    if(1)    /*USB��λ*/
    {
        UsbChgType = 1;
        if(USB_CHARGE_TYPE_HW == UsbChgType)    /*��׼�����*/
        {
            usb_state = CHG_USB_HW; 
        }
        else if(USB_CHARGE_TYPE_NON_HW == UsbChgType)    /*�Ǳ�����*/
        {
            usb_state = CHG_USB_NON_HW; 
        }
        else    /*USB����δʶ��*/
        {
            return BSP_ERROR;
        }
        
    }
    else    /*USB����λ*/
    {
        usb_state = CHG_USB_NON;
    }
    copy_to_user(usb_status,&usb_state , sizeof(CHG_USB_STATE));

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : BSP_CHG_StateSet
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,ʹ�ܻ��߽�ֹ���
 �������  :ulState      0:��ֹ���
 						  1:ʹ�ܳ��
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
void BSP_CHG_StateSet(unsigned long ulState)
{
    (void)BSP_CHGC_AskInterface((BSP_U32) SET_CHG_STA,(BSP_U32) ulState);
}
/*****************************************************************************
 �� �� ��  : BSP_CHG_PollTimerSet
 ��������  :��A�������õ����ҹػ�״̬������ʱ������ѯ����
 �������  :ulState      
                     0:�л�Ϊ�춨ʱ��
                     1:��������ʱ�����ڣ����ͼٹػ�״̬�Ĺ���
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
void BSP_CHG_PollTimerSet(BSP_U32 ulState)
{
    (void)BSP_CHGC_AskInterface((BSP_U32) SET_CHG_POLL_TIMER_STA, ulState);
}

/*****************************************************************************
 �� �� ��  : BSP_CHG_StateGet
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,��ѯ�Ƿ���Ҫ����
 �������  :
 �������  :
 ����ֵ��
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
 �� �� ��  : BSP_CHG_VoltGet
 ��������  : ��ѯC���ط��빲���ڴ����ʵʱ��ѹֵ
 �������  : NONE
 �������  : BSP_U32 ��ѹֵ
 ����ֵ��
*****************************************************************************/
BSP_U32 BSP_CHG_VoltGet(void)
{	
	st_batt_info_share_mem  batt_info_share_mem;//��ǰ��ѹֵ���ӹ����ڴ��л�ȡ

	/*���ó�ŵ�ӿڲ�ѯ��ǰ����*/
	memcpy((void *)(&batt_info_share_mem),(const void *)MEMORY_AXI_CHARGE_ADDR,sizeof(st_batt_info_share_mem));
	printk(KERN_ERR "\r\nBSP_CHG_VoltGet: voltage = %u;\r\n", batt_info_share_mem.batt_current_volt);

	return batt_info_share_mem.batt_current_volt;
}


/*****************************************************************************
 �� �� ��  : BSP_CHG_Sply
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,��ѯ�Ƿ���Ҫ����
 �������  :
 �������  :
 ����ֵ��
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
 �� �� ��  : BSP_CHG_GetCbcState
 ��������  : getCbcState��A�˵�ӳ�亯�������ص��״̬�͵���
 �������  :pusBcs 0:����ڹ��� 1:�������ӵ��δ����
                                       2:û����������
                          pucBcl  0:���������û���������ӣ�100:ʣ������ٷֱ�
 �������  : pusBcs 0:����ڹ��� 1:�������ӵ��δ����
                                       2:û����������
                          pucBcl  0:���������û���������ӣ�100:ʣ������ٷֱ�
 ����ֵ��   0 �����ɹ�
                         -1����ʧ��

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
 �� �� ��  : BSP_CHG_StateSet
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,ʹ�ܻ��߽�ֹ���
 �������  :ulState      0:��ֹ���
 						  1:ʹ�ܳ��
 �������  :��
 ����ֵ��    ��
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
 �� �� ��  : BSP_CHGC_BattSply
 ��������  :BSP_CHGC_BattSply��A�˵�ӳ�亯��,��ʼ����״̬
 �������  :
 �������  :��
 ����ֵ��    ��
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
 �� �� ��  : BSP_CHGC_SplyStGet
 ��������  :BSP_CHGC_SplyStGet��ȡ�����־
 �������  :
 �������  :��
 ����ֵ��    ��
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
 �� �� ��  : BSP_CHG_StateGet
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,��ѯ�Ƿ���Ҫ����
 �������  :
 �������  :
 ����ֵ��
*****************************************************************************/
BSP_S32 BSP_CHG_StateGet(void)
{
    return BSP_ERROR;
}

/*****************************************************************************
 �� �� ��  : BSP_CHG_Sply
 ��������  :chg_state_set(UINT32 ulstate)��A�˵�ӳ�亯��,��ѯ�Ƿ���Ҫ����
 �������  :
 �������  :
 ����ֵ��
*****************************************************************************/
BSP_S32 BSP_CHG_Sply( void)
{
    return BSP_ERROR;
}

#endif

EXPORT_SYMBOL(BSP_CHG_StateGet);
EXPORT_SYMBOL(BSP_CHG_Sply);

