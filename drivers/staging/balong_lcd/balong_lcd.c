/*
 * balong_lcd.c -- support for balong_lcd ,code number:23040204
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 
//#ifdef FEATURE_LCD_ON

#include <asm/io.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/io.h>
#include "BSP.h"
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

#include "balong_lcd.h"

static volatile int power_off_stop_flag=0;

#define LCD_PWR_ON_STATE      1
#define LCD_PWR_OFF_STATE     0
#define SPI_ENABLE_STATE      1
#define SPI_DISABLE_STATE     0
#define BUFFER_SIZE 24576  /*  pixel format 444, 128*128*3/2=24576 */

/*be used to index ioctl functions*/
#define BALONG_LCD_FUNC_NO          (LCD_CMD_MAX - LCD_LIGHT)
#define BALONG_LCD_FUNC_BASE        LCD_LIGHT

#define BALONG_WINTF_BUFFER_SIZE    64

#define BALONG_LCD_REFRESH_NAME     "lcd_workqueue"

static volatile UINT32 g_ulPicCount = 0x00;  /*图片计数*/

static volatile UINT32 g_ulLcdPwrOnState = LCD_PWR_OFF_STATE;  /*oled上下电状态*/
static volatile UINT32 spi_able_flag = SPI_DISABLE_STATE;  /*spi 使能状态 */
/* 同步信号量,关机充电时用到*/
static struct semaphore	lcd_sem_syn;

/*清屏刷屏互斥信号量，向屏写东西时用到*/
static struct semaphore	lcd_sem_mux;

static unsigned int request_timeout = HI_LCD_REQUEST_TIMEOUT;

/* 记录所有写向屏幕的内容 */
static unsigned char *buffer_t = NULL;

/* 保存传进来的要刷的内容 */
static unsigned char * buffer_kernel = NULL;

/* 关机充电标志 */
static volatile UINT32	lcd_poweroff_init_flag = 0;

/* workqueue and worker for (power on)/(power off charge) animation */
static struct workqueue_struct	*lcd_queue;
static struct delayed_work 		lcd_poweron_work;
static struct delayed_work		lcd_pwroff_chg_work;
/*lint -e656*/
static int (*func[BALONG_LCD_FUNC_NO])(struct file *file, unsigned int cmd, unsigned long arg);
/*lint +e656*/

/* spi base address */
static void* balong_lcd_iobase;

static unsigned int x_position = 0;
static unsigned int y_position = 0;

#undef hilcd_trace
#undef hilcd_error
#define HILCD_TRACE_LEVEL       (1)
#define BALONG_HILCD_DEBUG_TAG	"balong_lcd"
#define hilcd_trace(level, format, args...) do { \
	if((level) >= HILCD_TRACE_LEVEL) { \
		printk(format":"BALONG_HILCD_DEBUG_TAG":%s:%d: \n", ## args, __FUNCTION__,  __LINE__); \
	} \
}while(0)

#define hilcd_error(format, args...) do{ \
	printk(KERN_ERR""format":"BALONG_HILCD_DEBUG_TAG" error:%s:%d: \n", ##args, __FUNCTION__, __LINE__); \
}while(0)

/* init data format */
struct balong_lcd_seq {
	unsigned char		format;
	unsigned char		value;
};

/* config spi and enable it */
static void spi_setup_and_enable (BSP_VOID)
{
    unsigned long flag;
    writel(DIVID_BAUD_RATE,balong_lcd_iobase+BAUDR); /* 12M */
    /* 9bit data format,spi,transfer and receive */
    writel(DFS | FRF | SCPH | SCPOL | TMOD,
            balong_lcd_iobase+CTRLR0);

    /* enable spi and slave */
	flag = readl(balong_lcd_iobase+SSIENR);
	flag = flag | SSI_EN;
	writel(flag,balong_lcd_iobase+SSIENR );
	writel(SLAVE_EN, balong_lcd_iobase+SER);
	spi_able_flag = SPI_ENABLE_STATE;
}

/* spi write data  */
static int spi_write_data(const unsigned char *buf, size_t len)
{
    int i = 0;
    unsigned short j;
    int count =0;
    unsigned long cpu_jiffies_timeout;
    if(SPI_ENABLE_STATE != spi_able_flag)
    {
        return -ENODEV;
    }    
    for(i = 0;(unsigned int)i < len; i++)
    {
        j = ((unsigned short)buf[i]) | (1<<8); /* data ,d/c=1 */
        cpu_jiffies_timeout = jiffies + request_timeout;
        for(;;)  //b00198513 Modified for pclint e716          
        {
            if(!time_before(jiffies,cpu_jiffies_timeout))
		    {
		    	hilcd_error("wait cpu transfer complete  timeout!");
		    	hilcd_trace(1,"end");	
		    	return -ETIMEDOUT;	        
		    }
            if((readl(balong_lcd_iobase+SPISR) & 0x2) == 0)/* check full or not  */
            {
                count++;
                /* fifo fulled */
            }
            else
            {
                writel(j,balong_lcd_iobase+SPIDR); /* not full */
                break;
            }
        }
    }
    return BSP_OK;
}/*lint !e550*/

/* spi write cmd */
static int spi_write_cmd(const unsigned char *buf, size_t len)
{
    int i = 0;
    unsigned short j;
    int count =0;
    unsigned long cpu_jiffies_timeout;
    if(SPI_ENABLE_STATE != spi_able_flag)
    {
        return -ENODEV;
    }    
    for(i = 0;(unsigned int)i < len; i++)
    {
        j = ((unsigned short)buf[i]) & ~(1 << 8); /* command,d/c=0 */
        cpu_jiffies_timeout = jiffies + request_timeout;
        for(;;)  //b00198513 Modified for pclint e716          
        {
            if(!time_before(jiffies,cpu_jiffies_timeout))
		    {
		    	hilcd_error("wait cpu transfer complete  timeout!");
		    	hilcd_trace(1,"end");	
		    	return -ETIMEDOUT;	        
		    }
            if((readl(balong_lcd_iobase+SPISR) & 0x2) == 0) /* check full or not  */
            {
                count++;
                /* fifo fulled */
            }
            else
            {
                writel(j,balong_lcd_iobase+SPIDR); /* not full */
                break;
            }
        }
    }
    return BSP_OK;
    
}/*lint !e550*/

/* Magic sequences supplied by manufacturer, for details refer to datasheet */
static struct balong_lcd_seq initseq[] = 
{
    {'c',0x0011}, 
    {'w', 120},

    {'c',0x003A},
    {'d',0x0053},  //65K  OR 262k selectiong  12bit control

    //{'c', 0x0026}, //Set Default Gamma
    //{'d', 0x0004},

    {'c',0x00C0},
    {'d',0x000C},
    {'d',0x0005},
    {'w', 10},
   
    {'c',0x00C1},
    {'d',0x0006},
    {'w', 10},

    {'c',0x00C2}, //set the amount of current in operate
    {'d',0x0004},
    {'w', 10},
  
    {'c',0x00C5},
    {'d',0x002C},
    {'d',0x0035},
    {'w', 10},

    {'c',0x00C7},
    {'d',0x00C7},

    {'c',0x00F2},
    {'d',0x0001},

    {'c',0x00E0},   //  Gamma setting
    {'d',0x003F},
    {'d',0x001C},
    {'d',0x0018},
    {'d',0x0025},
    {'d',0x001E},
    {'d',0x000C},
    {'d',0x0042},
    {'d',0x00D8},
    {'d',0x002B},
    {'d',0x0013},
    {'d',0x0012},
    {'d',0x0008},
    {'d',0x0010},
    {'d',0x0003},
    {'d',0x0000},

    {'c',0x00E1},
    {'d',0x0000},
    {'d',0x0023},
    {'d',0x0027},
    {'d',0x000A},
    {'d',0x0011},
    {'d',0x0013},
    {'d',0x003D},
    {'d',0x0072},
    {'d',0x0054},
    {'d',0x000C},
    {'d',0x001D},
    {'d',0x0027},
    {'d',0x002F},
    {'d',0x003C},
    {'d',0x003F},

    {'c',0x0036},
    {'d',0x0008}, //modified by yangying

    {'c',0x00B1},
    {'d',0x000a},
    {'d',0x0014},

    // Use the default value
    {'c',0x002A},
    {'d',0x0000},
    {'d',0x0000}, //modified by yangying
    {'d',0x0000},
    {'d',0x007F}, //modified by yangying

    {'c',0x002B},
    {'d',0x0000},
    {'d',0x0000}, //modified by yangying
    {'d',0x0000},
    {'d',0x007F}, //modified by yangying

    {'c',0x0029},
    {'w', 5},
 
    {'c',0x002C},
    {'w', 120}
};

/* for initialize */
static int balong_lcd_toggle(struct balong_lcd_seq *seq, int sz)
{
	unsigned char buf[1];
	int ret = 0;
	int i;
	for (i = 0; i < sz; i++)
	{
		buf[0] = seq[i].value;
		if(seq[i].format == 'c')
		{
			ret = spi_write_cmd(buf, 1);  /* command */
			if(ret)
			{
			    hilcd_error("spi write command error!");
			    return ret;
			}
		}
		else if(seq[i].format == 'd')
		{
			ret = spi_write_data(buf, 1); /* data */
			if(ret)
			{
			    hilcd_error("spi write data error!");
			    return ret;
			}
		}
		else if(seq[i].format=='w')
		{   
            /*解决参数类型不对的问题-- linux内核*/
            /*lint -e516*/
		    /*lint -e62*/
			mdelay(seq[i].value); /* delay */
			/*lint +e62*/
			/*lint +e516*/
		}
	}
	return BSP_OK;
}

/* image will display in limit window  */
static int balong_lcd_window_set(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1)
{
    int ret = 0;
    unsigned char buf[1];    
    if((x0>LCD_HEIGHT)|(x1>LCD_HEIGHT)|(y0>LCD_WIDTH)|(y1>LCD_WIDTH))
    {
        hilcd_error("paramter error,[x0:%d][x1:%d][y0:%d][y1:%d]",
                    x0,x1,y0,y1);
        return -EINVAL;
    }
    buf[0] = COLUMN_ADDRESS_SET;
    ret = spi_write_cmd(buf, 1);
	if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
	
    buf[0] = 0x0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = x0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = 0x0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = x1;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
	
    buf[0] = PAGE_ADDRESS_SET;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
	
    buf[0] = 0x0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = y0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = 0x0;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = y1;
    ret = spi_write_data(buf, 1); /* data */
	if(ret)
	{
	    hilcd_error("spi write data error!");
	    return ret;
	}
    buf[0] = MEMORY_WRITE;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
   return BSP_OK;
}

/* lcd sleep */
static int balong_lcd_sleep_in(BSP_VOID)
{   
    int ret = 0;
    unsigned char buf[1];
    buf[0] = SLEEP_IN;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
    msleep(120);
    return BSP_OK;
}

/* lcd out of sleep */
static int balong_lcd_sleep_out(BSP_VOID)
{   
    int ret = 0;
    unsigned char buf[1];
    buf[0] = SLEEP_OUT;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
    msleep(120);
    return BSP_OK;
}

/* lcd display on relative display off */
static int lcdLight(BSP_VOID)
{   
    int ret = 0;
    unsigned char buf[1];
    buf[0] = LIGHT_ON;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
	return BSP_OK;
}

/* lcd display off relative display on */
static int lcdSleep(BSP_VOID)
{   
    int ret = 0;
    unsigned char buf[1];
    buf[0] = LIGHT_OFF;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
	return BSP_OK;
}

/* lcd reset */
static int lcdReset(BSP_VOID)
{
    int ret = 0;
    unsigned long flag;
    unsigned char buf[1];
    ret = cancel_delayed_work_sync(&lcd_pwroff_chg_work);
	if (0 > ret)
	{
		hilcd_error("cancel_delayed_work_sync failed.");
	}
    ret  = cancel_delayed_work_sync(&lcd_pwroff_chg_work);
	if (0 > ret)
	{
		hilcd_error("cancel_delayed_work_sync failed.\n");
	}

    buf[0] = 0x01;
    ret = spi_write_cmd(buf, 1);
    if(ret)
	{
	    hilcd_error("spi write command error!");
	    return ret;
	}
	flag = readl(BALONG_LCD_SC_BASE);
    flag = flag|0x0;
    writel(flag,BALONG_LCD_SC_BASE);   
    flag = flag|0x1;
    writel(flag,BALONG_LCD_SC_BASE);    /* lcd reset  */

    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
	ret = balong_lcd_toggle(initseq, ARRAY_SIZE(initseq));/*lint !e30 !e84 !e806*/ /* initialize */
	/*lint +e516*/
    if(ret)
	{
	    hilcd_error("lcd init error!");
	    return ret;
	}
    return BSP_OK;
}

/* lcd display black at all window */
static int balong_lcd_display_allblack(BSP_VOID)
{
    int i,ret;
    unsigned char data[1];
    unsigned long len = LCD_WIDTH*LCD_HEIGHT*3; /* pixel format 444 */
    ret = balong_lcd_window_set(0,0,LCD_HEIGHT,LCD_WIDTH);
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    down(&lcd_sem_mux);
    for(i=0;(unsigned int)i<len;i++)
    {
        data[0] = 0x0;     /* black rgb 0 */
        ret = spi_write_data(data,1);
        if(ret)
	    {
	        hilcd_error("spi write data error!");
	        return ret;
	    }
    }
    memset(buffer_t,0x0,BUFFER_SIZE); /* syn buffer_t */
    up(&lcd_sem_mux);
    return BSP_OK;
}

/* lcd display white at all window */
static int balong_lcd_display_allwhite(BSP_VOID)
{
    int i;
    int ret = 0;
    unsigned char data[1];
    unsigned long len = LCD_WIDTH*LCD_HEIGHT*3;
    balong_lcd_window_set(0,0,LCD_HEIGHT,LCD_WIDTH);
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    down(&lcd_sem_mux);
    for(i=0;(unsigned int)i<len;i++)
    {
        data[0] = 0xff;  /* white rgb ff */
        spi_write_data(data,1);
        if(ret)
	    {
	        hilcd_error("spi write data error!");
	        return ret;
	    }
    }
    memset(buffer_t,0xff,BUFFER_SIZE);  /* syn buffer_t */
    up(&lcd_sem_mux);
    return BSP_OK;
}

/* function of display image,be used by other function */
int lcdRefresh(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,const unsigned char * test_buf, unsigned int len)
{
    int i=0;
    int j=0;
    int ret = 0;
    down(&lcd_sem_mux);
    /* set window before write image */
    balong_lcd_window_set(x0, y0, x1, y1);
    if(ret)
    {
        hilcd_error("lcd window set error");
        return ret;
    }
    /* pixel format 444,so *3/2 */
    for(i = ((y0*(LCD_WIDTH+1)+x0)*3)/2; i < ((y1*(LCD_WIDTH+1)+x1+1)*3)/2; i++)
    {
	       buffer_t[i] = test_buf[j++];
	}
    ret = spi_write_data(test_buf,len);  
    if(ret)
    {
        hilcd_error("lcd refresh error");
        return ret;
    }
    up(&lcd_sem_mux);
    return BSP_OK;
}

static BSP_S32 lcdInit(BSP_VOID)
{
    int ret = 0;
    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
    ret=balong_lcd_toggle(initseq, ARRAY_SIZE(initseq));/*lint !e30 !e84 !e806*/
	/*lint +e516*/
    if(ret)
    {
        hilcd_error("lcd init error");
    }
    return ret;
}

/* lcd power on */
static int lcdPwrOn(BSP_VOID)
{
    int ret = 0;
	/*已是上电状态，无需重新上电*/
	if (LCD_PWR_ON_STATE == g_ulLcdPwrOnState)
	{
		printk("lcd has been powered on.\n");
		return BSP_OK;
	}

	/*100ms以上*/
	msleep(110);
	ret = lcdInit();
    if(ret)
    {
        hilcd_error("lcd init failed [ret:%d]",ret);
        return ret;
    }
	g_ulLcdPwrOnState = LCD_PWR_ON_STATE;
	return BSP_OK;
}

/* lcd power off ,sleep in */
static int lcdPwrOff(BSP_VOID)
{
    int ret = 0;
    ret = cancel_delayed_work_sync(&lcd_pwroff_chg_work);
	if (0 > ret)
	{
	    hilcd_error("cancel_delayed_work_sync failed.");
		return ret;
	}
    ret  = cancel_delayed_work_sync(&lcd_pwroff_chg_work);
    if (0 > ret)
    {
    	hilcd_error("cancel_delayed_work_sync failed.");
    	return ret;
    }
	/*已是下电状态，无需重新下电*/
	if (LCD_PWR_OFF_STATE == g_ulLcdPwrOnState)
	{
		printk(KERN_WARNING"lcd has been powered off.\n");
		return BSP_OK;
	}

	/*关闭显示*/
	ret = balong_lcd_sleep_in();
	if(0>ret)
	{
        hilcd_error("lcd sleepin error");
	}
	g_ulLcdPwrOnState = LCD_PWR_OFF_STATE;
	return BSP_OK;
}

/* lcd display a static image */
static int lcdInitImage(BSP_VOID)
{   
    int ret = 0;
	ret = lcdRefresh(0,0,LCD_HEIGHT,LCD_WIDTH,gImage_qie_444,BUFFER_SIZE);
    if(ret)
    {
        hilcd_error("lcd refresh error");
        return ret;
    }
	return BSP_OK;
}

/* clear screen eque all black */
BSP_VOID lcdClearWholeScreen(BSP_VOID)
{
    int ret = 0;
	ret = balong_lcd_display_allblack();
	if(ret)
	{
	    hilcd_error("lcd clear screen error");
	    return;
	}
}

static BSP_VOID lcdPwrOnAniIsr(struct work_struct *work)
{
	BSP_S32 ret = 0;

	if (NULL == work)
	{
		printk(KERN_WARNING"work is NULL and return.\n");
		return;
	}

	if (g_ulPicCount >= MAX_OFF_CHARGING_ANI)
	{
		g_ulPicCount = 0;
	}
	/*顺序显示开机动画*/
    ret = lcdRefresh(0,0,LCD_HEIGHT,LCD_WIDTH,g_pucPwrOffChgAniImage[g_ulPicCount],BUFFER_SIZE);
    if(ret)
    {
        hilcd_error("lcd refresh error [ret:%d]",ret);
        return;
    }
	g_ulPicCount++;

	if(lcd_queue)
	{
	    ret = queue_delayed_work(lcd_queue, &lcd_poweron_work, msecs_to_jiffies(1000));//FIXME? 1000 to macro
        if (!ret)
    	{
    		hilcd_error("queue_delayed_work failed.");
    	}
	}
}

BSP_VOID lcdPwrOnAniInstall(BSP_VOID)
{
	BSP_S32 ret = 0;
	if(lcd_queue)
	{
    	ret = queue_delayed_work(lcd_queue, &lcd_poweron_work, msecs_to_jiffies(1000));//FIXME? 1000 to macro
    	if (!ret)
    	{
    		hilcd_error("queue_delayed_work failed.");
    	}
	}
}

static BSP_VOID lcdPwrOffChgAniIsr(struct work_struct *work)
{
	BSP_S32 ret = 0;

	if (NULL == work)
	{
		printk(KERN_WARNING"work is NULL and return.\n");
		return;
	}
    if(lcd_queue)
    {
    	ret = queue_delayed_work(lcd_queue, &lcd_pwroff_chg_work, msecs_to_jiffies(1000));
    	if (!ret)
    	{
    		hilcd_error("queue_delayed_work failed.");
    	}
    }
	up(&lcd_sem_syn);
}

static BSP_S32 lcdChgTaskFunc(BSP_VOID *arg)
{
	BSP_S32 ret = 0;
	/* for pclint */
	if (NULL == arg)
	{
		;
	}
	for(;;)
	{
	    if(1!=lcd_poweroff_init_flag)
	    {
	        break;
	    }
		down(&lcd_sem_syn);
		if (g_ulPicCount >= MAX_OFF_CHARGING_ANI)
		{
			g_ulPicCount = 0;
		}
    	/*顺序显示开机动画*/
    	ret = lcdRefresh(0,0,LCD_HEIGHT,LCD_WIDTH,g_pucPwrOffChgAniImage[g_ulPicCount],BUFFER_SIZE);
        if(ret)
        {
            hilcd_error("lcd refresh error [ret:%d]",ret);
            return ret;
        }
        g_ulPicCount++;
	}
	return ret;
}

BSP_S32 lcdPwrOffChgAniInstall(BSP_VOID)
{
	BSP_S32 ret = 0;
    struct task_struct *charge_task;
	/*创建任务循环显示充电动画*/
	charge_task = kthread_run(lcdChgTaskFunc, NULL, "lcd_thread");
	if (IS_ERR(charge_task))
	{
		hilcd_error("create kthread charge_task failed!\n");
		return -ESRCH;
	}
	/*启动充电动画*/
	ret = queue_delayed_work(lcd_queue, &lcd_pwroff_chg_work, msecs_to_jiffies(1000));
	if (!ret)
	{
		hilcd_error("queue delayed work failed.\n");
		return ret;
	}
	return BSP_OK;
}

/* get memory content */
static int lcd_get_frame_buffer(unsigned long arg)
{
    unsigned char* buffer;
    long int ret = 0; /*clean lint e713*/
    buffer = (unsigned char*)arg;
    if(buffer)
    {
        ret = copy_to_user(buffer, buffer_t, BUFFER_SIZE);
        if(ret)
        {
            hilcd_error("buffer_t to user error [ret:%d]",ret);
            return ret;
        }
    }
    return BSP_OK;
}

static int lcd_del_timer(BSP_VOID)
{ 
    int ret = 0;
    ret = cancel_delayed_work_sync(&lcd_poweron_work);
	if (ret)
	{
		hilcd_error("cancel_delayed_work_sync failed. [ret:%d]",ret);
		return ret;
	}
	return BSP_OK;
}

static int lcd_power_off_charing_start(BSP_VOID)
{
    int ret = 0;
    g_ulPicCount = 0;
    if (0 == lcd_poweroff_init_flag)
    {
    	ret = lcdPwrOffChgAniInstall();
    	if (ret)
		{
			hilcd_error("install power off charge failed.");
			return ret;
		}	
    	lcd_poweroff_init_flag = 1;		
    }
    /*启动充电动画*/
    up(&lcd_sem_syn);
    return BSP_OK;
}

static int lcd_power_off_charing_stop(unsigned long arg)
{
    /* stop power off charging picture */
    int ret = 0;	
    ret = cancel_delayed_work_sync(&lcd_pwroff_chg_work);
	if (!ret)
	{
		hilcd_error("cancel_delayed_work_sync failed.");
		return -EBUSY;
	}
	lcd_poweroff_init_flag = 0;	    
	/*添加显示充电满图片*/
	if (arg >= MAX_OFF_CHARGING_ANI)
	{
		arg = MAX_OFF_CHARGING_ANI -1;
	}
    ret = lcdRefresh(0,0,LCD_HEIGHT,LCD_WIDTH,g_pucPwrOffChgAniImage[arg],BUFFER_SIZE);
    if(ret)
    {
        hilcd_error("lcd refresh error");
        return ret;
    }
    return BSP_OK;
}

static int lcd_write_image(struct file *file, unsigned int cmd, unsigned long arg)
{
    lcd_write_data image_data;
    long int ret = 0; /*clean lint e713*/
    buffer_kernel = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    if(file)
    {
        ret = copy_from_user(&image_data, (BSP_VOID __user *)arg,
             sizeof(lcd_write_data));
        if (ret)
	    {
		    kfree(buffer_kernel);
		    hilcd_error("struct copy error! [ret:%d]",ret);
			return ret;
		}
		ret = copy_from_user(buffer_kernel, 
		                    (BSP_VOID __user *)image_data.image,image_data.size);
		if((!image_data.image) || ret)
		{
		    kfree(buffer_kernel);
		    hilcd_error("image copy error! [ret:%d]",ret);
		    return -EINVAL;
		}
		/*pclint 734*/
        ret = lcdRefresh( (unsigned char)image_data.x_start, (unsigned char)image_data.y_start, 
                          (unsigned char)(image_data.x_offset+image_data.x_start), 
                          (unsigned char)(image_data.y_offset+image_data.y_start),
                          buffer_kernel,image_data.size);
        if(ret)
        {
            hilcd_error("lcd refresh failed! [ret:%d]",ret);
            kfree(buffer_kernel);
            return ret;
        }
    }
    else
    {
        /*ioctl called from kernel space*/
        memcpy(&image_data, (void*)arg, sizeof(lcd_write_data));
		/*pclint 734*/
        ret = lcdRefresh((unsigned char)image_data.x_start, (unsigned char)image_data.y_start, 
                         (unsigned char)(image_data.x_offset+image_data.x_start), 
                         (unsigned char)(image_data.y_offset+image_data.y_start),
                         image_data.image,image_data.size);
        if(ret)
        {
            hilcd_error("lcd refresh failed! [ret:%d]",ret);
            kfree(buffer_kernel);
            return ret;
        }                         
    }
    kfree(buffer_kernel);
    return BSP_OK;
}

static int lcdLight_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcdLight();
    if(ret)
    {
        hilcd_error("lcdLight error");
        return ret;
    }
    return BSP_OK;
}

static int lcdSleep_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcdSleep();
    if(0>ret)
    {
        hilcd_error("lcdSleep error");
        return ret;
    }
    return BSP_OK;
}

static int lcdReset_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcdReset();
    if(ret)
    {
        hilcd_error("lcdReset error");
        return ret;
    }
    return BSP_OK;
}

static int lcdPwrOn_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcdPwrOn();
    if(ret)
    {
        hilcd_error("lcdPwrOn error");
        return ret;
    }
    return BSP_OK;
}

static int lcdPwrOff_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcdPwrOff();
    if(ret)
    {
        hilcd_error("lcdPwrOff error");
        return ret;
    }
    return BSP_OK;
}

static int lcd_del_timer_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcd_del_timer();
    if(ret)
    {
        hilcd_error("lcd_del_timer error");
        return ret;
    }
    return BSP_OK;
}

static int lcdInitImage_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcdInitImage();
    if(ret)
    {
        hilcd_error("lcdInitImage error");
        return ret;
    }
    return BSP_OK;
}

static int lcd_power_off_charing_start_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcd_power_off_charing_start();
    if(ret)
    {
        hilcd_error("lcd_power_off_charing_start error");
        return ret;
    }
    return BSP_OK;
}

static int lcd_power_off_charing_stop_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcd_power_off_charing_stop(arg);
    if(ret)
    {
        hilcd_error("lcd_power_off_charing_stop error");
        return ret;
    }
    return BSP_OK;
}

static int lcd_write_image_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcd_write_image(file,cmd,arg);
    if(ret)
    {
        hilcd_error("lcd_write_image error");
        return ret;
    }
    return BSP_OK;
}

static int lcd_get_frame_buffer_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = lcd_get_frame_buffer(arg);
    if(ret)
    {
        hilcd_error("lcd_get_frame_buffer error");
        return ret;
    }
    return BSP_OK;
}

static int balong_lcd_display_allwhite_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = balong_lcd_display_allwhite();
    if(ret)
    {
        hilcd_error("balong_lcd_display_allwhite error");
        return ret;
    }
    return BSP_OK;
}

static int balong_lcd_display_allblack_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    ret = balong_lcd_display_allblack();
    if(ret)
    {
        hilcd_error("balong_lcd_display_allblack error");
        return ret;
    }
    return BSP_OK;
}

/* just for test */
static BSP_VOID buffer_test(BSP_VOID)
{
    int j;
    lcdRefresh(0,0,31,31,gImage_meinv_32x32_444,sizeof(gImage_meinv_32x32_444));

    for(j=1000;j<1010;j++)
        printk("---%x\n",buffer_t[j]);


    for(j=1000;j<1010;j++)
        printk("---%x\n",gImage_meinv_32x32_444[j]);
            
    lcdRefresh(0,32,31,63,buffer_t,sizeof(gImage_meinv_32x32_444));

}

static BSP_VOID test_part_window_disp(BSP_VOID)
{
    int e,f;
    int j =0;
    unsigned char a =0;
    unsigned char b =0;
    unsigned char c =7;
    unsigned char d =7; 
    while(j<4)
    {
        for( e =0; e<16; e++)
        {
            for(f =0; f<16 ;f++)
            {
                msleep(20);
                balong_lcd_display_allblack();

                lcdRefresh(a,b,c,d,gImage_red_8x8_444,sizeof(gImage_red_8x8_444));
                b=b+8;
                d=d+8;
            }
             b=0;
             d=7;
             a=a+8;
             c=c+8;
        }
        a=0;
        b=0;
        c=7;
        d=7;
        j++;
    }
}
/* just for test */

static ssize_t write_intf(struct device* dev,
			struct device_attribute* attr,
			const char *buf,
			size_t count)
{
    BSP_CHAR *command = NULL;
	UINT32 val = 0;
	BSP_S32 ret = 0;

	/* for pclint */
	if (NULL == dev && NULL == attr)
	{
		;
	}

	command = kzalloc(BALONG_WINTF_BUFFER_SIZE, GFP_KERNEL);
	if (NULL == command)
	{
		printk("malloc command failed\n");
		return (ssize_t)count;
	}

	sscanf(buf, "%s %d", command, &val);
	printk("command=%s val=%x\n", command, val);
	if (!strncmp(command, "lcd_light", strlen("lcd_light")))
	{
		printk("begin to test: lcd_light\n");
		ret = lcdLight();
		if(0>ret)
	    {
	       hilcd_error("lcd light error");
	    }
	}
	if (!strncmp(command, "lcd_close", strlen("lcd_close")))
	{
		printk("begin to test: lcd_close\n");
		ret = lcdSleep();
		if(0>ret)
	    {
	       hilcd_error("lcd close error");
	    }
	}
	if (!strncmp(command, "lcd_sleepin", strlen("lcd_sleepin")))
	{
		printk("begin to test: lcd_sleepin\n");
		ret = balong_lcd_sleep_in();
		if(0>ret)
	    {
	       hilcd_error("lcd sleepin error");
	    }
	}
	if (!strncmp(command, "lcd_sleepout", strlen("lcd_sleepout")))
	{
		printk("begin to test: lcd_sleepout\n");
		ret = balong_lcd_sleep_out();
		if(0>ret)
	    {
	       hilcd_error("lcd sleepout error");
	    }
	}
	if (!strncmp(command, "lcdClearWholeScreen", strlen("oledClearWholeScreen")))
	{
		printk("begin to test: lcdClearWholeScreen\n");
		lcdClearWholeScreen();
	}
	if (!strncmp(command, "spi_test_data", strlen("spi_test_data")))
	{
	    unsigned char buf[1];/*lint !e578*/
		printk("begin to test: spi_data\n");
        buf[0] = 0x55;
        spi_write_data(buf, 1);	
	}
    if (!strncmp(command, "spi_test_cmd", strlen("spi_test_cmd")))
	{
        unsigned char buf[1];/*lint !e578*/
		printk("begin to test: spi_cmd\n");
        buf[0] = 0xaa;
        spi_write_data(buf, 1);
		
	}
	if (!strncmp(command, "lcdInitImage", strlen("lcdInitImage")))
	{
		printk("begin to test: lcdInitImage\n");
		ret = lcdInitImage();
		if(0>ret)
	    {
	       hilcd_error("lcd init image error");
	    }
	}
	if (!strncmp(command, "lcdPwrOnAniInstall", strlen("lcdPwrOnAniInstall")))
	{
		printk("begin to test: lcdPwrOnAniInstall\n");
		lcdPwrOnAniInstall();
	}
	if (!strncmp(command, "lcdPwrOffChgAniInstall", strlen("lcdPwrOffChgAniInstall")))
	{
		printk("begin to test: lcdPwrOffChgAniInstall\n");
		ret = lcd_power_off_charing_start();
		if(0>ret)
	    {
	       hilcd_error("lcd power off install error");
	    }
	}
	if (!strncmp(command, "lcd_PwrOn", strlen("lcdPwrOn")))
	{
		printk("begin to test: lcdPwrOn\n");
		ret = lcdPwrOn();
		if(0>ret)
	    {
	       hilcd_error("lcd poweron error");
	    }
	}
	if (!strncmp(command, "lcd_PwrOff", strlen("lcdPwrOff")))
	{
		printk("begin to test: lcdPwrOff\n");
		ret = lcdPwrOff();
		if(0>ret)
	    {
	       hilcd_error("lcd poweroff error");
	    }
	}
	if (!strncmp(command, "lcdWhite", strlen("lcdWhite")))
	{
		printk("begin to test: lcdWhite\n");
        ret = balong_lcd_display_allwhite();
        if(0>ret)
	    {
	       hilcd_error("lcd white error");
	    }
	}
	if (!strncmp(command, "lcdBlack", strlen("lcdBlack")))
	{
		printk("begin to test: lcdWhite\n");
        ret = balong_lcd_display_allblack();
        if(0>ret)
	    {
	       hilcd_error("lcd black error");
	    }
	}
	if (!strncmp(command, "cancel_poweron_work", strlen("cancel_poweron_work")))
	{
		printk("begin to test: cancel_poweron_work\n");
		ret = cancel_delayed_work_sync(&lcd_poweron_work);
		
		if (0 > ret)
		{
			printk("cancel_delayed_work_sync failed.\n");
		}
	}
	if (!strncmp(command, "cancel_poweroff_work", strlen("cancel_poweroff_work")))
	{
		printk("begin to test: cancel_poweroff_work\n");
		ret  = cancel_delayed_work_sync(&lcd_pwroff_chg_work);
		if (0 > ret)
		{
			printk("cancel_delayed_work_sync failed.\n");
		}
		lcd_poweroff_init_flag = 0;
	}
	if (!strncmp(command, "lcd_restet", strlen("lcd_reset")))
	{
         printk("begin to test: lcd reset\n");
         ret = lcdReset();
         if(0>ret)
	     {
	        hilcd_error("lcd reset error");
	     }
	}
	if (!strncmp(command, "lcd_test_buffer", strlen("lcd_test_buffer")))
	{
		printk("begin to test: lcd buffer\n");
		buffer_test();
	}
	if (!strncmp(command, "lcd_test_part_window", strlen("lcd_test_part_window")))
	{
		printk("begin to test: lcd part window\n");
		test_part_window_disp();
	}
	kfree(command);
	return (ssize_t)count;
}
DEVICE_ATTR(intf, 0644, NULL, write_intf);

static struct attribute *lcd_attrs[] = {
	&dev_attr_intf.attr,
	NULL,
};

static struct attribute_group balong_lcd_group = {
	.attrs	=	lcd_attrs,
};
static struct platform_device *balong_lcd_intf;

static void test_init(BSP_VOID){
	balong_lcd_intf = platform_device_register_simple("balong_lcd_intf", -1, NULL, 0);		
	if (IS_ERR(balong_lcd_intf)){
		PTR_ERR(balong_lcd_intf);
		hilcd_error("Init error.register simple failed!");
	}
	sysfs_create_group (&balong_lcd_intf->dev.kobj, &balong_lcd_group);
	hilcd_trace(1,"balong_lcd_intf init OK.");
}
/* test code end */

static loff_t balong_lcd_lseek(struct file *file, loff_t off, int whence)
{
	/* for pclint */
	if (NULL == file && 0 == whence)
	{
		;
	}
	
	x_position = (unsigned int )off % LCD_WIDTH;
	y_position = (unsigned int)off / LCD_HEIGHT;	
	hilcd_trace(1,"off=%d  x_position=%d  y_position=%d",(int)off,
	            x_position, y_position);
	return 0;
}

static ssize_t balong_lcd_write(struct file *file, const char __user *buf, size_t count,
					loff_t *ppos)
{
	UINT8 *image = NULL;
	BSP_S32 ret = 0;

	/* for pclint */
	if (NULL == file && NULL == ppos)
	{
		;
	}

	image = kzalloc((count + 1), GFP_KERNEL);
	if (NULL == image)
	{
		hilcd_error("malloc write image buffer failed.");
		return -EBUSY;
	}
	
	hilcd_trace(1,"count=%ul    x_position=%u   y_position=%u\n",
	            count, x_position, y_position);
	if(copy_from_user((void*)image, buf, count))
	{
	    kfree(image);
		return -EFAULT;
	}
    /*解决空指针问题*/
	if(NULL != ppos)
	{
       hilcd_trace(1,"ppos=%d   length=%ul\n",(int)*ppos, strlen(image));/*lint !e64*/
	}
	/*pclint 734*/
    ret = lcdRefresh((unsigned char)x_position, (unsigned char)y_position, LCD_WIDTH, LCD_HEIGHT,image, count);
    if(ret)
    {
        hilcd_error("lcd refresh error");
        kfree(image);
        return ret;
    }
    kfree(image);
	return (ssize_t)count;
}

static long balong_lcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	hilcd_trace(1,"cmd=%u----arg=%lu",cmd, arg);
	if((cmd>LCD_CMD_MAX)|(cmd<BALONG_LCD_FUNC_BASE))
	{
	    hilcd_error("command error [cmd:%u]",cmd);
	    return -EFAULT;
	}
    if(func[cmd-BALONG_LCD_FUNC_BASE])
    {
        ret = func[cmd-BALONG_LCD_FUNC_BASE](file ,cmd ,arg);
        return (long)ret;
    }
    else
    {
        hilcd_error("function [cmd:%d] not initialize",cmd);
        return -EINVAL;
    }
}

static int balong_lcd_open(struct inode *inode, struct file *file)
{
	hilcd_trace(1,"+++++++++lcd open");
	/* for pclint */
	if (NULL != inode && NULL != file)
	{
		;
	}
	return 0;
}

static int balong_lcd_release(struct inode *inode, struct file *file)
{
	hilcd_trace(1,"+++++release");
	/* for pclint */
	if (NULL != inode && NULL != file)
	{
		;
	}
	return 0;
}

static struct file_operations misc_balong_lcd_ops = 
{
    .owner = THIS_MODULE,
    .llseek = balong_lcd_lseek,
    .write = balong_lcd_write,
    .unlocked_ioctl = balong_lcd_ioctl,
    .open = balong_lcd_open,
    .release = balong_lcd_release,
};

static struct miscdevice lcd_misc = 
{
    .name = "misc_balong_lcd",
    .fops = &misc_balong_lcd_ops
};

static __init int balong_lcd_init(BSP_VOID)
{
    int ret =0;
    unsigned long flag;
    memset(func,0,BALONG_LCD_FUNC_NO);
    /*function definition initialize*/
    /*lint -e656*/
    func[LCD_LIGHT-BALONG_LCD_FUNC_BASE] = lcdLight_ioctl;/*lint !e778*/
    func[LCD_SLEEP-BALONG_LCD_FUNC_BASE] = lcdSleep_ioctl;
    func[LCD_RESET-BALONG_LCD_FUNC_BASE] = lcdReset_ioctl;
    func[LCD_POWER_ON-BALONG_LCD_FUNC_BASE] = lcdPwrOn_ioctl;
    func[LCD_POWER_OFF-BALONG_LCD_FUNC_BASE] = lcdPwrOff_ioctl;
    func[LCD_DEL_TIMER-BALONG_LCD_FUNC_BASE] = lcd_del_timer_ioctl;
    func[LCD_SYS_POWER_DOWN-BALONG_LCD_FUNC_BASE] = lcdPwrOff_ioctl;
    func[LCD_INIT_IMAGE-BALONG_LCD_FUNC_BASE] = lcdInitImage_ioctl;
    func[LCD_POWER_OFF_CHARGING_START-BALONG_LCD_FUNC_BASE] = lcd_power_off_charing_start_ioctl;
    func[LCD_POWER_OFF_CHARGING_STOP-BALONG_LCD_FUNC_BASE] = lcd_power_off_charing_stop_ioctl;
    func[LCD_WRITE_IMAGE-BALONG_LCD_FUNC_BASE] = lcd_write_image_ioctl;
    func[LCD_GET_FRAME_BUFFER-BALONG_LCD_FUNC_BASE] = lcd_get_frame_buffer_ioctl;
    func[LCD_ALL_WHITE-BALONG_LCD_FUNC_BASE] = balong_lcd_display_allwhite_ioctl;
    func[LCD_ALL_BLACK-BALONG_LCD_FUNC_BASE] = balong_lcd_display_allblack_ioctl;
    /*lint +e656*/
    buffer_t = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    if(!buffer_t)
    {
        ret = -EBUSY;
        goto err_flag;
    }
    
    sema_init(&lcd_sem_syn,SEM_FULL);
    sema_init(&lcd_sem_mux,SEM_FULL);

    lcd_queue=create_workqueue(BALONG_LCD_REFRESH_NAME);
    if (NULL == lcd_queue) {
		hilcd_error("failed to create workqueue lcd_queue");
		goto err_flag;
	}

	/* init delayed work for power on/power off charge animation */
	INIT_DELAYED_WORK(&lcd_poweron_work, lcdPwrOnAniIsr);
	INIT_DELAYED_WORK(&lcd_pwroff_chg_work, lcdPwrOffChgAniIsr);
	
    balong_lcd_iobase = ioremap(BALONG_SPI1_BASE, SZ_4K);

	test_init();
    spi_setup_and_enable();  /* enable spi and slave */
    flag = readl(BALONG_LCD_SC_BASE);
    flag = flag|0x1;
    writel(flag,BALONG_LCD_SC_BASE);    /* lcd reset  */
	//lcdPwrOn();              /* power on */
	
    ret = misc_register(&lcd_misc); /* register device */
    if(ret)
    {
        goto err_flag;
    }
	return ret;
err_flag:
    if(buffer_t)
    {
        kfree(buffer_t);
    }
    if(lcd_queue)
    {
        destroy_workqueue(lcd_queue);
    }
    return ret;

}

static __exit void balong_lcd_exit(BSP_VOID)
{   
    misc_deregister(&lcd_misc);
    sysfs_remove_group(&balong_lcd_intf->dev.kobj, &balong_lcd_group);
    platform_device_unregister(balong_lcd_intf);
    destroy_workqueue(lcd_queue);
    if(buffer_t){
        kfree(buffer_t);
        buffer_t = NULL;
    } 
}

module_init(balong_lcd_init);
module_exit(balong_lcd_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("LCD driver for the Hisilicon LCD");
MODULE_LICENSE("GPL");
//#endif
