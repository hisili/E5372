/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  balong_rtc.c
*
*
*   ��    �� :  RTC A��ģ��
* 
*************************************************************************/
/**********************���ⵥ�޸ļ�¼******************************************
��	  ��			  �޸���		 ���ⵥ��			�޸�����
******************************************************************************/

#include <mach/common/bsp_version.h>
#include "BSP.h"
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/amba/bus.h>
#include <linux/rtc.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/bcd.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/amba/bus.h>
#include "balong_rtc.h"
#include <linux/semaphore.h>
#include <mach/balong_v100r001.h>
#include <linux/netlink.h>
#include <linux/kthread.h>



/*--------------------------------------------------------------*
 * ȫ�ֱ�������                                                 *
 *--------------------------------------------------------------*/

static void* balong_rtc_base = (void *)RTC_VA_BASE_ADDR;
/*the timer thread struct*/
struct task_struct  *alarm_task;
struct semaphore s_AlarmSem;
/*alarm�ص�����*/
ALARM_FUNC AlarmCallBackFunc = NULL;
/*alarmʹ��״̬*/
int alarm_is_used = 0;

/*��1970�굽�û��޸ĵ�ʱ�������*/
unsigned long g_ulLastSec    = 0;

/*��¼�û��޸�ʱ��ʱ��ϵͳ���������е���ʱ��ֵ����λ:��*/
unsigned long g_ulLastCount  = 0;

/*****************************************************************************
* �� �� ��  : balong_rtc_readtime
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
static int balong_rtc_readtime(struct device *dev, struct rtc_time *tm)
{

    rtc_time_to_tm(readl(balong_rtc_base + RTC_DR), tm);

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : balong_rtc_settime
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
static int balong_rtc_settime(struct device *dev, struct rtc_time *tm)
{
    int ret = 0;
    unsigned long time = 0;
	int tmp  = 0;
	unsigned long alarm = 0;
	int i = 0;

	ret  = rtc_valid_tm(tm);
	if (0 != ret)
	{
		printk(KERN_ERR"RTC: rtc_valid_tm error!\n");
        return BSP_ERROR;
	}
	
    /*���û��޸�ʱ��֮ǰϵͳ���е�������¼����*/
    g_ulLastCount = get_current_working_time();

	ret = rtc_tm_to_time(tm, &time);
	if (0 != ret)
    {   
		printk(KERN_ERR"RTC: balong rtc_tm_to_time Fail!\n");
        return BSP_ERROR;		
    }
	/* �����ֵ  */
	tmp = time - readl(balong_rtc_base + RTC_DR);

	/* ������ʱ�� */
    writel(time, balong_rtc_base + RTC_LR);

	/* ����alarmʱ�� */
	alarm = readl(balong_rtc_base + RTC_MR)+ tmp;
	
	writel(alarm, balong_rtc_base + RTC_MR);   
   
    /*����1970�굽�û��޸�ʱ��֮�������*/
    g_ulLastSec = time;
	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : along_alarm_irq_enable
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
static int balong_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	unsigned long imsc = 0;

	/* Clear any pending alarm interrupts. */
	readl(balong_rtc_base + RTC_ICR);

	imsc = readl(balong_rtc_base + RTC_CR);

    /* if interupt enabled,unmask */
    /* if interupt unenabled,mask */
	if (1 == enabled)
    {
        writel(imsc | RTC_BIT_AI, balong_rtc_base + RTC_CR);
    }
	else
    {   
        writel(imsc & (~RTC_BIT_AI), balong_rtc_base + RTC_CR);
    }

	return BSP_OK;
}/*lint !e550*/


/*****************************************************************************
* �� �� ��  : balong_rtc_readalarm
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
static int balong_rtc_readalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	rtc_time_to_tm(readl(balong_rtc_base + RTC_MR), &alarm->time);

    alarm->pending = readl(balong_rtc_base + RTC_RIS) & RTC_BIT_AI;
	alarm->enabled = ((readl(balong_rtc_base + RTC_CR) & RTC_BIT_IM) >> 1) & RTC_BIT_AI;/*lint !e572*/

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : balong_rtc_setalarm
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
static int balong_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
    int ret = 0;
    unsigned long time = 0;
    
    /*�ر�ALARM,����Ҫ����ʱ��*/
    if(1 == alarm->enabled)
    {
	    ret = rtc_valid_tm(&alarm->time);
	    if (0 != ret) 
        {
   		    printk(KERN_ERR"RTC: balong rtc_valid_tm Fail!\n");
            return BSP_ERROR;	       
	    }
	
	    ret = rtc_tm_to_time(&alarm->time, &time);
	    if (0 != ret)
        {   
		    printk(KERN_ERR"RTC: balong rtc_tm_to_time Fail!\n");
            return BSP_ERROR;		
        }

	    writel(time, balong_rtc_base + RTC_MR);
    }
    
    balong_alarm_irq_enable(dev, alarm->enabled);
   
	return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : rtc_power_off_notify_main_controller
*
* ��������  : ֪ͨӦ����ػ�
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : ��
*
* ����˵��  : ��
*
*****************************************************************************/
void rtc_power_off_notify_main_controller(void)
{
    int ret, size;
    DEVICE_EVENT event;

    size = sizeof(DEVICE_EVENT);

    event.device_id = (DEVICE_ID)DEVICE_ID_KEY; 
    event.event_code = (int)KEY_POWER_OFF; 
    event.len = 0;

    ret = device_event_report(&event, size);
    printk(KERN_ERR " RTC: rtc alarm event:device id=%d,event id=%d\n",event.device_id, event.event_code);
    if (-1 == ret) 
    {
    	printk(KERN_ERR "device_event_init: can't add event\n");
    }

}

/*****************************************************************************
* �� �� ��  : balong_interrupt
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
static irqreturn_t balong_interrupt(int irq, void *data)
{
	unsigned long rtcmis = 0;

	rtcmis = readl(balong_rtc_base + RTC_MIS);
	if (rtcmis) 
    {
        
		readl(balong_rtc_base + RTC_ICR);

		if (rtcmis & RTC_BIT_AI)
        {      
            /*�ͷ��ź���*/
            up( &s_AlarmSem );

        }

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}
/*test callback*/
static void alarm_shutdown(void)
{
#if defined (FEATURE_LCD_ON)
    /*�ػ����̲�����˯��*/
    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
    /*��ӡ��ǰʱ��*/
    get_current_working_time();
    /*֪ͨӦ�ùػ�*/
    rtc_power_off_notify_main_controller();
}
/*****************************************************************************
* �� �� ��  : alarm_task_func
*
* ��������  : 
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
int alarm_task_func( void * data )
{
    while(1)/*lint !e716*/
    {
        /*��ȡ�ź���*/
        down(&s_AlarmSem);
        /*����alarm�ص�����*/
		if(AlarmCallBackFunc != NULL)
		{
            AlarmCallBackFunc();
		}
        
    }
    return 0;/*lint !e527*/
}

/*l00212897 added for alram */

/*****************************************************************************
* �� �� ��  : balong_rtc_alarm_time_to_sec
*
* ��������  : ��alarmʱ�任��Ϊ��
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
unsigned long balong_rtc_alarm_time_to_sec(unsigned int days,unsigned int hours,unsigned int mins,unsigned int secs)
{
    unsigned long time = 0;
    time = (unsigned long)(days*86400 + hours*3600 + mins*60 + secs);
    return time;
}

/*****************************************************************************
* �� �� ��  : balong_rtc_alarm_set
*
* ��������  : ����alarmʱ�䣬������ʱ
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
int balong_rtc_alarm_set(unsigned int days,unsigned int hours,unsigned int mins,unsigned int secs,ALARM_FUNC alarm_callback)
{
    unsigned long time = 0;
    unsigned long count = 0;

    if(1 == alarm_is_used)
    {
        printk(KERN_ERR "alarm: the alarm is used! \n"); 
        return BSP_ERROR;
    }

    if((24 <= hours) ||(60 <= mins)||(60 <= secs))
    {
        return BSP_ERROR;
    }
    /*��alarmʱ�任��Ϊ��*/
    time = balong_rtc_alarm_time_to_sec(days,hours,mins,secs);
    count = readl(balong_rtc_base + RTC_DR);
    /*д�붨ʱʱ�䣬��ֹ���*/
    if(count <= (0xffffffff - time))
    {
        count = count + time;
    }
    else
    {
       time = time -(0xffffffff - count);
       count = time;
    }
    
    writel(count, balong_rtc_base + RTC_MR);   
    /*ע��ص�����*/
    //AlarmCallBackFunc = alarm_shutdown;
    AlarmCallBackFunc = alarm_callback;
    
    /*enable interrupt*/
    balong_alarm_irq_enable(NULL, 1);
    alarm_is_used = 1;
    /*��ӡ��ǰʱ��*/
    get_current_working_time();
    printk(KERN_ERR "alarm: the set alarm time is day:%d hour:%d min:%d sec:%d \n",
        days,hours,mins,secs);

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : balong_rtc_alarm_clear
*
* ��������  : ����alarm���ж�
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : BSP_OK:    �����ɹ�
*             BSP_ERROR: ����ʧ��
*
* ����˵��  : ��
*
*****************************************************************************/
int balong_rtc_alarm_clear(void)
{
    /*disable alarm interupt*/
    balong_alarm_irq_enable(NULL, 0);
    alarm_is_used = 0;
    AlarmCallBackFunc = NULL;
    /*��ӡ��ǰʱ��*/
    get_current_working_time();
    printk(KERN_ERR "alarm: the alarm is stop! \n");
    return BSP_OK;
    
}
/*****************************************************************************
* �� �� ��  : get_current_working_time
*
* ��������  : ��ȡ��ǰϵͳ�ۼƿ���ʱ��
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : unsigned long:  ������Ŀǰ������������
*
* ����˵��  : 
*
*****************************************************************************/
unsigned long get_current_working_time(void)
{
    unsigned long count = 0;
    struct rtc_time time;
    
    /*��ȡ��ǰRTCʱ��*/
    count = readl(balong_rtc_base + RTC_DR);
    rtc_time_to_tm(count, &time);

    /*��ǰϵͳ���е�ʱ�� = �û����ʱ��֮�����е����� + �û����ʱ��֮ǰ���е�����*/
    count = count - g_ulLastSec + g_ulLastCount;

    printk(KERN_ERR"Current RTC Time: %d-%d-%d %d:%d:%d, count=%lu;\n",
        time.tm_year + RTC_BASE_YEAR,time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec,count);

    return count;
}

/*RTC�����������ݽṹ*/
static const struct rtc_class_ops balongv7r1_rtc_ops = {
	.read_time	= balong_rtc_readtime,
	.set_time	= balong_rtc_settime,
	.read_alarm	= balong_rtc_readalarm,
	.set_alarm	= balong_rtc_setalarm,
	.alarm_irq_enable = balong_alarm_irq_enable,
};

/*
��ʼ����������Ҫ��Ӳ����ʼ��������
����driverע�ắ����platform_driver_register������ע��RTCӲ�������������
����deviceע�ắ����platform_device_register������ע��RTCӲ���豸��
*/
static int __devinit  balong_rtc_probe(struct platform_device *pdev)
{

    struct rtc_device *rtc;
    
	rtc = rtc_device_register("balong_rtc", &pdev->dev, &balongv7r1_rtc_ops,
			THIS_MODULE);
    
	platform_set_drvdata(pdev, rtc);

    /*���ü�����ֵ*/
    //writel(0, balong_rtc_base + RTC_LR);

	/* ���ж� */
	readl(balong_rtc_base + RTC_ICR);

	/*disable the alarm interupt */
	writel(0, balong_rtc_base + RTC_CR);

    if (request_irq(RTC_INT_ID, balong_interrupt, IRQF_DISABLED, "balong_rtc", rtc))
	{
		printk(KERN_ERR"RTC: balong request_irq Fail!\n");
        rtc_device_unregister(rtc);
        return BSP_ERROR;
	}
    
    /*��ʼ������������*/
    sema_init(&s_AlarmSem,SEM_EMPTY);
    /*���������߳�*/
    alarm_task = kthread_create(alarm_task_func,(void *)NULL,"acpu_alarm");
    if ( NULL != alarm_task )
    {
        wake_up_process(alarm_task);
    }  

    return BSP_OK;    
}

static int  balong_rtc_remove(struct platform_device *pdev)
{
	struct rtc_device *rtc = platform_get_drvdata(pdev);

    if (alarm_task)
    {   
        /*ɾ���߳�*/
        kthread_stop(alarm_task);
        alarm_task = NULL;
    }

    free_irq(RTC_INT_ID, rtc);

	rtc_device_unregister(rtc);
    
	return BSP_OK;   
}

static struct platform_device balong_rtc_device = {
        .name           = "balong_rtc",
};

static struct platform_driver balong_rtc_driver = {
	.driver = {
		.name = "balong_rtc",
		.owner = THIS_MODULE,
	},
	.probe	= balong_rtc_probe,
	.remove = __devexit_p(balong_rtc_remove),
};

static int __init balongv7r1_rtc_init (void)
{   
    /*ע��ƽ̨�豸������*/
    platform_device_register(&balong_rtc_device);
    return platform_driver_register(&balong_rtc_driver);
}

static void __exit balongv7r1_rtc_exit(void)
{
	platform_device_unregister(&balong_rtc_device);
	platform_driver_unregister(&balong_rtc_driver);
}

module_init(balongv7r1_rtc_init);
module_exit(balongv7r1_rtc_exit);


/********************test begin**********************/

int balong_rtc_read_time(struct rtc_time *tm)
{
    /*��ȡ��ǰRTCʱ��*/
    rtc_time_to_tm(readl(balong_rtc_base + RTC_DR), tm);
    /*ϵͳ������������*/
    tm->tm_mon +=1;

	return BSP_OK;
}

int balong_rtc_alarm_test(int level)
{
    switch (level)
    {
        case 0:
            balong_rtc_alarm_set(0,0,0,5,alarm_shutdown);    
            break;
        case 1:
            balong_rtc_alarm_set(0,0,0,30,alarm_shutdown);
            break;
        case 2:
            balong_rtc_alarm_set(0,0,2,0,alarm_shutdown);
            break;
        case 3:
            balong_rtc_alarm_set(0,0,30,0,alarm_shutdown);
            break;
        case 4:
            balong_rtc_alarm_set(0,1,0,0,alarm_shutdown);
            break;
        case 5:
            balong_rtc_alarm_set(0,12,0,0,alarm_shutdown);
            break;
        default:
            break;
       
    }

    printk(KERN_ERR "alarm: set alarm time at level %d\n",level);
    
    return BSP_OK;
}


int balong_rtc_read_test(void)
{
    struct rtc_time time;
    balong_rtc_read_time(&time);

    printk(KERN_ERR"RTC: RTC Time: %d-%d-%d %d:%d:%d\n",
        time.tm_year,time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec);

    return BSP_OK;
}

int balong_rtc_set_test(int y,int mon,int d,int h,int minute,int s)
{
    struct rtc_time time;
    time.tm_year = y;
    time.tm_mon = mon;
    time.tm_mday = d;
    time.tm_hour = h;
    time.tm_min = minute;
    time.tm_sec = s;
    printk(KERN_ERR"RTC: Will Set Time: %d-%d-%d %d:%d:%d\n",y + RTC_BASE_YEAR,mon,d,h,minute,s);

    balong_rtc_settime(NULL,&time);
    
    /*����ʱ�����Ҫ1��2�����Ҳ�����Ч*/
    msleep(2000);

    printk(KERN_ERR"RTC: RTC Now count: %lu\n",get_current_working_time());

    printk(KERN_ERR"RTC: RTC Now g_ulLastSec: %lu, g_ulLastCount: %lu\n",g_ulLastSec,g_ulLastCount);

    return BSP_OK;
}
/********************test end**********************/


