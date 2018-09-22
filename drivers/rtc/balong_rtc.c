/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_rtc.c
*
*
*   描    述 :  RTC A核模块
* 
*************************************************************************/
/**********************问题单修改记录******************************************
日	  期			  修改人		 问题单号			修改内容
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
 * 全局变量申明                                                 *
 *--------------------------------------------------------------*/

static void* balong_rtc_base = (void *)RTC_VA_BASE_ADDR;
/*the timer thread struct*/
struct task_struct  *alarm_task;
struct semaphore s_AlarmSem;
/*alarm回调函数*/
ALARM_FUNC AlarmCallBackFunc = NULL;
/*alarm使用状态*/
int alarm_is_used = 0;

/*从1970年到用户修改的时间的秒数*/
unsigned long g_ulLastSec    = 0;

/*记录用户修改时间时，系统开机后运行的总时间值，单位:秒*/
unsigned long g_ulLastCount  = 0;

/*****************************************************************************
* 函 数 名  : balong_rtc_readtime
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
static int balong_rtc_readtime(struct device *dev, struct rtc_time *tm)
{

    rtc_time_to_tm(readl(balong_rtc_base + RTC_DR), tm);

	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : balong_rtc_settime
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
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
	
    /*将用户修改时间之前系统运行的秒数记录下来*/
    g_ulLastCount = get_current_working_time();

	ret = rtc_tm_to_time(tm, &time);
	if (0 != ret)
    {   
		printk(KERN_ERR"RTC: balong rtc_tm_to_time Fail!\n");
        return BSP_ERROR;		
    }
	/* 计算差值  */
	tmp = time - readl(balong_rtc_base + RTC_DR);

	/* 设置新时间 */
    writel(time, balong_rtc_base + RTC_LR);

	/* 更新alarm时间 */
	alarm = readl(balong_rtc_base + RTC_MR)+ tmp;
	
	writel(alarm, balong_rtc_base + RTC_MR);   
   
    /*更新1970年到用户修改时间之间的秒数*/
    g_ulLastSec = time;
	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : along_alarm_irq_enable
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
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
* 函 数 名  : balong_rtc_readalarm
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
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
* 函 数 名  : balong_rtc_setalarm
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
static int balong_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alarm)
{
    int ret = 0;
    unsigned long time = 0;
    
    /*关闭ALARM,不需要设置时间*/
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
* 函 数 名  : rtc_power_off_notify_main_controller
*
* 功能描述  : 通知应用真关机
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
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
* 函 数 名  : balong_interrupt
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
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
            /*释放信号量*/
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
    /*关机流程不允许睡眠*/
    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif
    /*打印当前时间*/
    get_current_working_time();
    /*通知应用关机*/
    rtc_power_off_notify_main_controller();
}
/*****************************************************************************
* 函 数 名  : alarm_task_func
*
* 功能描述  : 
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int alarm_task_func( void * data )
{
    while(1)/*lint !e716*/
    {
        /*获取信号量*/
        down(&s_AlarmSem);
        /*调用alarm回调函数*/
		if(AlarmCallBackFunc != NULL)
		{
            AlarmCallBackFunc();
		}
        
    }
    return 0;/*lint !e527*/
}

/*l00212897 added for alram */

/*****************************************************************************
* 函 数 名  : balong_rtc_alarm_time_to_sec
*
* 功能描述  : 将alarm时间换算为秒
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
unsigned long balong_rtc_alarm_time_to_sec(unsigned int days,unsigned int hours,unsigned int mins,unsigned int secs)
{
    unsigned long time = 0;
    time = (unsigned long)(days*86400 + hours*3600 + mins*60 + secs);
    return time;
}

/*****************************************************************************
* 函 数 名  : balong_rtc_alarm_set
*
* 功能描述  : 设置alarm时间，开启定时
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
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
    /*将alarm时间换算为秒*/
    time = balong_rtc_alarm_time_to_sec(days,hours,mins,secs);
    count = readl(balong_rtc_base + RTC_DR);
    /*写入定时时间，防止溢出*/
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
    /*注册回调函数*/
    //AlarmCallBackFunc = alarm_shutdown;
    AlarmCallBackFunc = alarm_callback;
    
    /*enable interrupt*/
    balong_alarm_irq_enable(NULL, 1);
    alarm_is_used = 1;
    /*打印当前时间*/
    get_current_working_time();
    printk(KERN_ERR "alarm: the set alarm time is day:%d hour:%d min:%d sec:%d \n",
        days,hours,mins,secs);

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : balong_rtc_alarm_clear
*
* 功能描述  : 屏蔽alarm中中断
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int balong_rtc_alarm_clear(void)
{
    /*disable alarm interupt*/
    balong_alarm_irq_enable(NULL, 0);
    alarm_is_used = 0;
    AlarmCallBackFunc = NULL;
    /*打印当前时间*/
    get_current_working_time();
    printk(KERN_ERR "alarm: the alarm is stop! \n");
    return BSP_OK;
    
}
/*****************************************************************************
* 函 数 名  : get_current_working_time
*
* 功能描述  : 读取当前系统累计开机时间
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : unsigned long:  开机后到目前所经历的秒数
*
* 其它说明  : 
*
*****************************************************************************/
unsigned long get_current_working_time(void)
{
    unsigned long count = 0;
    struct rtc_time time;
    
    /*读取当前RTC时间*/
    count = readl(balong_rtc_base + RTC_DR);
    rtc_time_to_tm(count, &time);

    /*当前系统运行的时间 = 用户变更时间之后运行的秒数 + 用户变更时间之前运行的秒数*/
    count = count - g_ulLastSec + g_ulLastCount;

    printk(KERN_ERR"Current RTC Time: %d-%d-%d %d:%d:%d, count=%lu;\n",
        time.tm_year + RTC_BASE_YEAR,time.tm_mon,time.tm_mday,time.tm_hour,time.tm_min,time.tm_sec,count);

    return count;
}

/*RTC操作函数数据结构*/
static const struct rtc_class_ops balongv7r1_rtc_ops = {
	.read_time	= balong_rtc_readtime,
	.set_time	= balong_rtc_settime,
	.read_alarm	= balong_rtc_readalarm,
	.set_alarm	= balong_rtc_setalarm,
	.alarm_irq_enable = balong_alarm_irq_enable,
};

/*
初始化函数做必要的硬件初始化工作。
调用driver注册函数（platform_driver_register函数）注册RTC硬件相关驱动程序。
调用device注册函数（platform_device_register函数）注册RTC硬件设备。
*/
static int __devinit  balong_rtc_probe(struct platform_device *pdev)
{

    struct rtc_device *rtc;
    
	rtc = rtc_device_register("balong_rtc", &pdev->dev, &balongv7r1_rtc_ops,
			THIS_MODULE);
    
	platform_set_drvdata(pdev, rtc);

    /*设置计数初值*/
    //writel(0, balong_rtc_base + RTC_LR);

	/* 清中断 */
	readl(balong_rtc_base + RTC_ICR);

	/*disable the alarm interupt */
	writel(0, balong_rtc_base + RTC_CR);

    if (request_irq(RTC_INT_ID, balong_interrupt, IRQF_DISABLED, "balong_rtc", rtc))
	{
		printk(KERN_ERR"RTC: balong request_irq Fail!\n");
        rtc_device_unregister(rtc);
        return BSP_ERROR;
	}
    
    /*初始化信量不可用*/
    sema_init(&s_AlarmSem,SEM_EMPTY);
    /*创建任务线程*/
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
        /*删除线程*/
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
    /*注册平台设备与驱动*/
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
    /*读取当前RTC时间*/
    rtc_time_to_tm(readl(balong_rtc_base + RTC_DR), tm);
    /*系统函数计算有误*/
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
    
    /*设置时间后需要1到2秒左右才能生效*/
    msleep(2000);

    printk(KERN_ERR"RTC: RTC Now count: %lu\n",get_current_working_time());

    printk(KERN_ERR"RTC: RTC Now g_ulLastSec: %lu, g_ulLastCount: %lu\n",g_ulLastSec,g_ulLastCount);

    return BSP_OK;
}
/********************test end**********************/


