/*
 * himci.c - hisilicon MMC Host driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*autoconf.h has been removed since version 2.6.33*/
//#include <linux/autoconf.h>
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
#include <linux/mmc/sd.h>
#include <linux/kthread.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/switch.h>
#include <linux/fs.h>
#include <linux/slab.h> /*y00186965*/

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

#include <linux/semaphore.h>
#include <mach/pwrctrl/BSP_PWC_PERIMGR.h>
#include "../core/SDUpdate.h"/*l00205892*/
#include "BSP.h"
#include "../core/core.h"


#if ((defined (BOARD_ASIC)||defined(BOARD_SFT)) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
#include <linux/bsp_pmu_drv.h>
#include "../core/mmc_ops.h"
#endif

#define BALONG_SDIO_ARG			(0)

/*mmc controller registers debug*/
#undef BALONG_SDCARD_DEBUG_FS
//#define BALONG_SDCARD_DEBUG_FS   /*tiaoshi*/

/*y00186965 begin*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS))
	#define BALONG_GPIO_DETECT_BOARD_TYPE		BALONG_GPIO_0(7)
#else
	#define BALONG_GPIO_DETECT_BOARD_TYPE		BALONG_GPIO_0(11)
#endif

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
			#define BALONG_GPIO_DETECT_CHIP	 2
			#define BALONG_GPIO_DETECT_PIN	 5
			#define BALONG_GPIO_DETECT		BALONG_GPIO_2(BALONG_GPIO_DETECT_PIN)
		#else
			#define BALONG_GPIO_DETECT_CHIP	 9
			#define BALONG_GPIO_DETECT_PIN	 1
			#define BALONG_GPIO_DETECT		BALONG_GPIO_9(BALONG_GPIO_DETECT_PIN)
		#endif
	#else
		/*T1_MODIFY*/
		#if ((FEATURE_E5 == FEATURE_OFF) && (FEATURE_64M == FEATURE_ON))
			/*GPIO0_9*/
			#define BALONG_GPIO_DETECT_CHIP	 0
			#define BALONG_GPIO_DETECT_PIN	 9
			#define BALONG_GPIO_DETECT		BALONG_GPIO_0(BALONG_GPIO_DETECT_PIN)
		#else
			#define BALONG_GPIO_DETECT_CHIP	 0
			#define BALONG_GPIO_DETECT_PIN	 13
			#define BALONG_GPIO_DETECT		BALONG_GPIO_0(BALONG_GPIO_DETECT_PIN)
		#endif
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920ES))
/*GPIO0_13*/
#define BALONG_GPIO_DETECT_CHIP	 0
#define BALONG_GPIO_DETECT_PIN	 13
#define BALONG_GPIO_DETECT		BALONG_GPIO_0(BALONG_GPIO_DETECT_PIN)
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920CS))
/*GPIO2_16*/
#define BALONG_GPIO_DETECT_CHIP	 2
#define BALONG_GPIO_DETECT_PIN	 16
#define BALONG_GPIO_DETECT		BALONG_GPIO_2(BALONG_GPIO_DETECT_PIN)
#else
/*GPIO2_5*/
#define BALONG_GPIO_DETECT_CHIP	 2
#define BALONG_GPIO_DETECT_PIN	 5
#define BALONG_GPIO_DETECT		BALONG_GPIO_2(BALONG_GPIO_DETECT_PIN)
#endif    /*y00186965 end*/

/*GPIO2_6*/
#define BALONG_GPIO_VOL_CHIP    2
#define BALONG_GPIO_VOL_PIN     6
#define BALONG_GPIO_VOL         BALONG_GPIO_2(BALONG_GPIO_VOL_PIN)

/*1 for externel dma transmisson, 0 for internal dma.*/
/*external dma choice*/

#ifdef BALONG_SDCARD_DEBUG_FS

#include <linux/sysfs.h>
#endif
#include <linux/gpio.h>
#include "hisdio_sys_ctrl.h"

#include "hisdio.h"

#include "sd_idmac.h"
#include <mach/edmacIP.h>
#include <mach/edmacDrv.h>
/*1 for externel dma transmisson, 0 for internal dma.*/
#ifdef CONFIG_BALONG_INTERNAL_DMA
#define BALONG_SDCARD_DMA	(0)
#else
/*external dma choice*/
#define BALONG_SDCARD_DMA	(1)
#define EDMA_FREE_LIST_NODE_NUM    25
LOCAL BALONG_DMA_CB * EDMA_free_list_node = NULL;
dma_addr_t  edma_phy_addr  = 0;
#endif
/*lint -e752 */
//extern unsigned ReadNv(unsigned dwAddress, unsigned char * pDatabuf, unsigned dwLength, unsigned * pBytesReturned);
//extern unsigned WriteNv(unsigned dwAddress, unsigned char * pDatabuf, unsigned dwLength, unsigned * pBytesReturned);
/*lint +e752 */

/*functions declaration*/
static void hi_sdio_shutdown(struct platform_device *dev);
#ifdef CONFIG_SMALL_IMAGE
int is_sdcard_exist(struct hisdio_host *);
#endif
spinlock_t sdio_spin_lock;/*lint !e86*/

struct semaphore g_sdio_lock;
struct workqueue_struct  *psdio_resume_workque;
struct work_struct        sdio_resume_work_suct;
struct work_struct        sd_detect_work;

struct completion hisdio_cmd_complete;
struct completion hisdio_data_busy_complete;

static unsigned int sdio_getcd   = 0;
unsigned int sdio_device_remove = 0;
struct hisdio_host *sdio_host_copy    = NULL;

#if BALONG_SDCARD_DMA
static int dma = 1;
#else
static int dma = 0;/*modify 1 to 0 disable externel dma by zxf*/
#endif
#define BALONG_CPU_COPY (0)
static unsigned int request_timeout = HI_SDIO_REQUEST_TIMEOUT;

int sdio_trace_level = HISDIO_TRACE_LEVEL;

/*lint -e729 */
sdio_args g_wlanargs = {0};   
/*lint +e729 */

int g_suspend   = 0;
int g_bitmode   = 0;
int g_clock     = 0;
int g_intrkmode = 0;
int g_iVoltSwitchInt = 0;
int g_cclok_low_power = 0;
void __iomem  *g_sysctrl_base = 0;

#define SD_STATE_POLL_TIME  10  //SD卡是否在位轮询的最大次数

/*modified for lint e752
extern sdio_handle g_sdio_oob; */

extern struct mmc_host *g_host;  /*y00186965*/

volatile unsigned int g_product_flag = 1;  /*es_e5 poll y00186965s*/

/*调试接口*/  /*tiaoshi*/

volatile int debug_step_t = 1;/*zxf*/
    /*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
static HISDIO_IDOCTRL_ARG * g_stSDLDO10Arg = NULL;    /*y00186965*/
static HISDIO_IFC_MSG_STRU *stSDIFCMsg = NULL; 
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
		#else
	 /*++by pandong cs*/
void __iomem  *g_iomg_base_sd = 0;
	/*--by pandong cs*/
		#endif
	#endif
#endif
	/*--by pandong cs*/
#ifdef CONFIG_SMALL_IMAGE
int is_sdcard_exist(struct hisdio_host * host)
{
	int tmp = 0;
	tmp = readl(host->base+MMC_STATUS);
	printk("is_sdcard_exist: the value of MMC_STATUS  is %x\n", tmp);
	return ((tmp & DATA_3_STATUS) ? BSP_TRUE : BSP_FALSE);
}
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

int is_sdcard_exist(struct hisdio_host * host)
{
	int tmp = 0;
	tmp = readl(host->base+MMC_STATUS);
	
    hisdio_trace(1,"is_sdcard_exist: the value of MMC_STATUS  is %x\n",tmp); 
	return ((tmp & DATA_3_STATUS) ? BSP_TRUE : BSP_FALSE);
}

#endif

 void balong_break_point(void)
{
    debug_step_t = 1;//zxf
	while(debug_step_t)
	{
		schedule();
	}
}

void balong_break_set(int iFlag)
{
	if (iFlag == 0)
	{
		debug_step_t = 0;
	}
	else
	{
		debug_step_t = 1;
	}
}
/*******************************************************************
  Function:      void hisdio_lock(void)
  Description:   完成对sdio操作的互斥功能，获得锁
  Input:         NA     
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************/

static void hisdio_lock(void)
{
    down(&g_sdio_lock);
}

/******************************************************************
  Function:     void hisdio_unlock(void)
  Description:   完成对sdio操作的互斥功能，释放获得的锁
  Input:         NA     
  Output:        NA
  Return:        NA
  Others:        NA
******************************************************************/

static void hisdio_unlock(void)
{
    up(&g_sdio_lock);
}

/*******************************************************************************
  Function:      static void hi_get_sdio_arg(unsigned char *arg,int len)
  Description:   get sdio args
  Input:         arg:the point of args
                 len:the length of args
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
#if (BALONG_SDIO_ARG)
static void hi_get_sdio_arg(unsigned char *arg,int len)
{
	
	unsigned int  bytesreturned;
	unsigned char macadress[MAC_LEN];
	int      rt  = -1;
	int      i   = 0;
	sdio_args *sdioargs = 0;
	
    hisdio_trace(1,"begin");
	
	if(NULL == arg || len < sizeof(sdio_args))
	{
		hisdio_trace(1,"input args is error.");
		hisdio_trace(1,"end");
		return;
	}
	else
	{
		
		sdioargs = (sdio_args *)arg;
		 
		ReadNv(NV_WLANMAC_ADDR,sdioargs->wlanmac, sizeof(sdio_args), &bytesreturned);
		
		memset(macadress,0xFF,MAC_LEN);
		
		rt = memcmp(sdioargs->wlanmac,macadress,MAC_LEN);
		
		if( 0 != rt)
		{
		    memset(macadress,0x00,MAC_LEN);
		    rt = memcmp(sdioargs->wlanmac,macadress,MAC_LEN);
		}
		
		if(0 == rt)
		{
			get_random_bytes(sdioargs->wlanmac,MAC_LEN);
			
			sdioargs->wlanmac[0] = 0x00;
			
            WriteNv(NV_WLANMAC_ADDR,sdioargs->wlanmac, sizeof(sdio_args), &bytesreturned);
		}
		
		sdioargs->wlanmac[0] = 0x00;
		
		for(i = 0; i < MAC_LEN ;i++)
	    {
	        hisdio_trace(5,"wlanmac[%d]: 0x%x.\n",i, sdioargs->wlanmac[i]);
	    }
	}
	hisdio_trace(1,"end");
	return;	
}

#endif


/*******************************************************************************
  Function:      void hi_wlan_arg(unsigned char *arg,int len)
  Description:   wlan args
  Input:         arg:the point of args
                 len:the length of args
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

void hi_wlan_arg(unsigned char *arg,int len)
{
	if(NULL == arg || len < (int)sizeof(sdio_args))
	{
		hisdio_trace(1,"input args is error.");
		return;
	}
	
	memcpy(arg,&g_wlanargs,sizeof(sdio_args));
	
	return;	
}

void PWRCTRL_SD_ClkStatus(void)
{
	int mmc1_clk = 0;
	mmc1_clk = (int)BSP_PWRCTRL_SoftClkStatus(PWRCTRL_MODU_MMC1); /*clean lint e713*/
	printk("MMC1 clk=%d\n", mmc1_clk);
}

int BSP_PWRCTRL_SD_LowPowerEnter(void)
{
	int ret = 0;
	BSP_PWRCTRL_SoftClkDis(PWRCTRL_MODU_MMC1);	
	ret = (int)BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SD); /*clean lint e713*/
	if(BSP_OK != ret)
	{
		hisdio_trace(5,"SD Vote Unlock failed");
		return ret;
	}
	return ret;
}
int BSP_PWRCTRL_SD_LowPowerExit(void)
{
	int ret = 0;
	BSP_PWRCTRL_SoftClkEn(PWRCTRL_MODU_MMC1);	
	ret = (int)BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_SD); /*clean lint e713*/
	if(BSP_OK != ret)
	{
		hisdio_trace(5,"SD Vote Lock failed");
		return ret;
	}
	return ret;
}


    /*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
    
/*****************************************************************************
* 函 数 名  : hi_sdio_send_switch_to_pmu
*
* 功能描述  : SD模块核间通信
*             将PMU IDO10开关参数通过IFC传到C核
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : created by y00186965
*
*****************************************************************************/
int hi_sdio_send_switch_to_pmu(HISDIO_IDOCTRL_ARG *pstSDLDO10Arg)
{
    int ret;
	
	/*消息体变量赋值*/
    stSDIFCMsg->u32ModuleId = IFC_MODULE_BSP;
    stSDIFCMsg->u32FuncId = IFCP_FUNC_SD_LDO_CTRL;
    stSDIFCMsg->stSDLDOArg.LDO_id = pstSDLDO10Arg->LDO_id;
	stSDIFCMsg->stSDLDOArg.is_voltage_switch = pstSDLDO10Arg->is_voltage_switch;
    stSDIFCMsg->stSDLDOArg.open_close = pstSDLDO10Arg->open_close;
	stSDIFCMsg->stSDLDOArg.voltage_val = pstSDLDO10Arg->voltage_val;

    /*通过IFC将消息发出*/
    ret = BSP_IFC_Send((BSP_VOID*)stSDIFCMsg, sizeof(HISDIO_IFC_MSG_STRU));
    if(BSP_OK != ret)
    {
        hisdio_trace(3,"hisdio: ACPU IFC Fail!\n");
        hisdio_trace(3,"hisdio: ACPU IFC Fail!ret=%d\n", ret);
        return BSP_ERROR;
    }

    return BSP_OK;

}


/*****************************************************************************
* 函 数 名  : hi_sdio_switch_drive_current
*
* 功能描述  : 切换IO 驱动电流
*
* 输入参数  : int isUHSIflag
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : created by y00186965
*
*****************************************************************************/
void hi_sdio_switch_drive_current(int isUHSIflag)
{
    u32 tmp_reg = 0;
	if (isUHSIflag)
	{
        tmp_reg = readl(SCTL_BASE_ADDR + 0x0950);
		tmp_reg &= ~0x000000fc;
		tmp_reg |= 0x000000fc; /*配置SD3.0 IO驱动能力，12mA*/
		writel(tmp_reg, SCTL_BASE_ADDR + 0x0950);	        
	}
	else
	{
        tmp_reg = readl(SCTL_BASE_ADDR + 0x0950);
		tmp_reg &= ~0x000000fc;
		tmp_reg |=  0x0000001c; /*配置SD2.0 IO驱动能力，6mA*/
		writel(tmp_reg, SCTL_BASE_ADDR + 0x0950);	 
	}
	mdelay(2);
}
#elif (defined(BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
/*****************************************************************************
* 函 数 名  : hi_sdio_switch_drive_current
*
* 功能描述  : 切换IO 驱动电流
*
* 输入参数  : int isUHSIflag
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : created by y00186965
*
*****************************************************************************/
void hi_sdio_switch_drive_current(int isUHSIflag)
{
    u32 tmp_reg = 0;/*lint -e550*/
	if (isUHSIflag)
	{
        tmp_reg = readl(SCTL_BASE_ADDR + 0x0950);
		tmp_reg &= ~0x000000fc;
		tmp_reg |= 0x000000fc; /*配置SD3.0 IO驱动能力，12mA*/
		writel(tmp_reg, SCTL_BASE_ADDR + 0x0950);	        
	}
	else
	{
        tmp_reg = readl(SCTL_BASE_ADDR + 0x0950);
		tmp_reg &= ~0x000000fc;
		tmp_reg |=  0x0000001c; /*配置SD2.0 IO驱动能力，6mA*/
		writel(tmp_reg, SCTL_BASE_ADDR + 0x0950);	 
	}
	mdelay(2);/*lint !e62*/
}
#endif

/*******************************************************************************
  Function:      static int hi_sdio_wait_cmd(struct hisdio_host *host)
  Description:   check cmd is complete
  Input:         hisdio_host
  Output:        NA
  Return:        0 sucess other error
  Others:        NA
*******************************************************************************/

static int hi_sdio_wait_cmd(struct hisdio_host *host)
{
	int wait_retry_count = 0;
	unsigned int reg_data = 0;
	hisdio_trace(1,"begin");
    for(;;)  //b00198513 Modified for pclint e716          
	{
		/*
		Check if CMD::start_cmd bit is clear.
		start_cmd = 0 means MMC Host controller has loaded registers and
		next command can be loaded in. 
		*/
		 
		reg_data = readl(host->base + MMC_CMD);
		if((reg_data & START_CMD) == 0)
		{
			hisdio_trace(1,"end");
			return 0;
		}

		/* Check if Raw_Intr_Status::HLE bit is set. */
		reg_data = readl(host->base + MMC_RINTSTS);
		if(reg_data & HLE_INT_STATUS)
		{
			reg_data |= HLE_INT_STATUS;
			writel(reg_data,host->base + MMC_RINTSTS);
			hisdio_trace(1,"Other CMD is running,please operate cmd again!");
			hisdio_trace(1,"end");
			return 1;
		}
		
		/* Check if number of retries for this are over. */
		wait_retry_count++;
		if(wait_retry_count >= MAX_RETRY_COUNT)
		{
			hisdio_trace(1,"wait cmd complete is timeout!");
			hisdio_trace(1,"end");
			return -1;
		} 
	}
}



/*******************************************************************************
  Function:      static void hi_sdio_set_cclk(struct hisdio_host *host,unsigned int cclk)
  Description:   set sdio clock 
  Input:         host:hisdio_host struct
                 cclk:clock number
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void hi_sdio_set_cclk(struct hisdio_host *host,unsigned int cclk)
{
	unsigned int reg_value;
	cmd_arg_s clk_cmd;

	hisdio_trace(1,"begin");   

	/*计算SD卡的分频比*/
#if (defined (BOARD_ASIC)\
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    reg_value = MMC1_CLK / (cclk * 2);  
#else
	reg_value = MMC0_CLK / (cclk * 2);	
#endif
	
	writel(reg_value,host->base + MMC_CLKDIV);

	clk_cmd.cmd_arg = readl(host->base + MMC_CMD); 
	clk_cmd.bits.start_cmd = 1;
	clk_cmd.bits.update_clk_reg_only = 1;

	writel(clk_cmd.cmd_arg,host->base + MMC_CMD);
	
	if(hi_sdio_wait_cmd(host) != 0)
	{
		hisdio_trace(1,"set card clk divider is failed!");
	}
	
	hisdio_trace(1,"end");				
}


static void sdio_clock_ctl(int fg)
{      
    unsigned int  count = TRY_COUNT;       
    int  rty_count = TRY_COUNT>>3;

    hisdio_trace(1, "begin");
    if (fg)
    {
        while (rty_count > 0)    
        {              
            count   = TRY_COUNT; 
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))     
    /*++by pandong cs*/

	#if defined(CHIP_BB_6756CS)
            set_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKEN1));                    
            while (!test_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && count > 0) 
	#else
			set_bit(MMC1_CLK_BIT, (void *)(SCTL_BASE_ADDR + 0x000C));  /*时钟使能y00186965*/
		    while (!test_bit(MMC1_CLK_BIT, (void *)(SCTL_BASE_ADDR + 0x0204)) && count > 0) 
	#endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
			/*q00175519 开启MMC1时钟*/
			BSP_PWRCTRL_TimerOpen(PWRCTRL_MODU_MMC1);
			break;
#else
            set_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKEN1));                    
            while (!test_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && count > 0)  
#endif
            {                
                count--;            
            }

            if (count > 0)            
            {                
                break;       
            }            
            rty_count--;        
        /*END:y00206456 2012-04-26 Modified for pclint e527*/         
        }
    }
    else
    {
        while (rty_count > 0)       
        {              
            count   = TRY_COUNT; 
			
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
            set_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKDIS1));         
            while (test_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && count > 0)  
	#else
			set_bit(MMC1_CLK_BIT, (void *)(SCTL_BASE_ADDR + 0x0010));  /*时钟禁止*/
		    while (!test_bit(MMC1_CLK_BIT, (void *)(SCTL_BASE_ADDR + 0x0204)) && count > 0) 
	#endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
			/*q00175519 关闭MMC1时钟*/
			BSP_PWRCTRL_TimerClose(PWRCTRL_MODU_MMC1);
			break;
#else
            set_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKDIS1));         
            while (test_bit(MMC0_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && count > 0)  
#endif
            {                
                count--;            
            }

            if (count > 0)            
            {                
                break;       
            }

            rty_count--;        
        }
    
    }

    if (rty_count <= 0)
    {
        hisdio_trace(1,"sdio clock operate is  error.\r\n");
    }

    hisdio_trace(1, "begin");
   return ;  
}

static void sys_ctrl_hisdio_reset_ip(void)
{
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
	set_bit(MMC0_SRST, (volatile unsigned long *)((unsigned long)SCTL_SC_PERCTRL0_RESTEN0));
	#else
    set_bit(MMC1_SRST, (volatile unsigned long *)((unsigned long)SCTL_BASE_ADDR + 0x0038));  /*y00186965 软复位*/
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	set_bit(MMC1_SRST, (volatile unsigned long *)((unsigned long)SCTL_BASE_ADDR + 0x0038));	/*q00175519 软复位*/
#else
	set_bit(MMC0_SRST, (volatile unsigned long *)((unsigned long)SCTL_SC_PERCTRL0_RESTEN0));
#endif
    /*END  :y00206456 2012-04-26 Modified for pclint e124*/

    udelay(10);
}

/*******************************************************************************
  Function:      static void sys_ctrl_hisdio_unreset_ip(void)
  Description:   unreset MMC2 host controle
  Input:         NA
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void sys_ctrl_hisdio_unreset_ip(void)
{
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
    /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
	set_bit(MMC0_SRST, (volatile unsigned long *)((unsigned long)SCTL_SC_PERCTRL0_RESTDIS0));
	#else
    clear_bit(MMC1_SRST, (volatile unsigned long *)((unsigned long)SCTL_BASE_ADDR + 0x0038));  /*y00186965解复位*/
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	clear_bit(MMC1_SRST, (volatile unsigned long *)((unsigned long)SCTL_BASE_ADDR + 0x0038));  /*q00175519解复位*/
#else
	set_bit(MMC0_SRST, (volatile unsigned long *)((unsigned long)SCTL_SC_PERCTRL0_RESTDIS0));
    /*END  :y00206456 2012-04-26 Modified for pclint e124*/

#endif
    udelay(10);
}

/*******************************************************************************
  Function:      wlan_clock_ctl
  Description:   SD卡系统控制器时钟控制
  Input:         fg: 1:打开时钟 0: 关闭时钟
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void wlan_clock_ctl(int fg)
{
	unsigned int tmp_reg = 0;

	hisdio_trace(1, "begin");

	if (fg)
	{
	    /*set mmc div clock*/
	    spin_lock(&sdio_spin_lock);
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
        tmp_reg = readl(SCTL_SC_PERCTRL);
	    tmp_reg &= ~(MMC0_DIV << MMC0_DIV_BIT);
	    tmp_reg |= MMC0_DIV_NUM << MMC0_DIV_BIT;
	    writel(tmp_reg, SCTL_SC_PERCTRL);	
	#else
		/*分频默认8分频，48MHz  y00186965*/
        tmp_reg = readl(SCTL_BASE_ADDR + 0x0040);
		tmp_reg &= ~0x00003f00;
		tmp_reg |= 0x00000800; /*配置总线8分频，48MHz*/
		//tmp_reg |= 0x00001000; /*配置总线16分频，24MHz*/
		writel(tmp_reg, SCTL_BASE_ADDR + 0x0040);
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920ES))
	/*分频默认8分频，48MHz	q00175519*/
	tmp_reg = readl(SCTL_BASE_ADDR + 0x0040);
	tmp_reg &= ~0x00003f00;
	tmp_reg |= 0x00000800; /*配置总线8分频，48MHz*/
	//tmp_reg |= 0x00001000; /*配置总线16分频，24MHz*/
	writel(tmp_reg, SCTL_BASE_ADDR + 0x0040);
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920CS))
	/*分频默认10分频，48MHz	 q00175519*/
	tmp_reg = readl(SCTL_BASE_ADDR + 0x0040);
	tmp_reg &= ~0x00003f00;
	tmp_reg |= 0x00000a00; /*配置总线10分频，48MHz*/
	//tmp_reg |= 0x00000800; /*配置总线8分频，60MHz*/
	//tmp_reg |= 0x00001000; /*配置总线16分频，24MHz*/
	writel(tmp_reg, SCTL_BASE_ADDR + 0x0040);

#else
        tmp_reg = readl(SCTL_SC_PERCTRL);
	    tmp_reg &= ~(MMC0_DIV << MMC0_DIV_BIT);
	    tmp_reg |= MMC0_DIV_NUM << MMC0_DIV_BIT;
	    writel(tmp_reg, SCTL_SC_PERCTRL);	
#endif
	    spin_unlock(&sdio_spin_lock);

	    /* enable clock */
	    sdio_clock_ctl(1);

	    /* reset mmc ip */
	    sys_ctrl_hisdio_reset_ip();

	    sys_ctrl_hisdio_unreset_ip();        
    }
    else
    {
        sdio_clock_ctl(0);
        //wlan_input_clock_ctl(0);
    }
    
    hisdio_trace(1,"begin");
}

/*******************************************************************************
  Function:      sys_ctrl_hisdio_init_io
  Description:   SD卡系统控制器
  Input:         fg: 1:打开时钟 0: 关闭时钟
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void sys_ctrl_hisdio_init_io(void)
{
#if (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
#else
    unsigned int reg;
#endif
    hisdio_trace(1, "SD INIT begin");
  
    /* set iocfg ,目前软核不需要配置，产品板需要考虑*/
    /*pin_config(IOCFG_BASE);*/
	/*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
		#else
			hi_sdio_switch_drive_current(0);
		#endif
	#else
    	hi_sdio_switch_drive_current(0);
	#endif
#elif (defined (BOARD_ASIC) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	hi_sdio_switch_drive_current(0);
#endif
	/*--by pandong cs*/
	/*外设pll使能，如果不使能，SD寄存器访问异常，需要在系统启动时配置*/

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		reg = readl(SCTL_SC_PPLL);
		reg |=0x1;   /*PLL 使能*/
		writel(reg, SCTL_SC_PPLL);	
	#else
	     /*PLL 使能 y00186965*/
	    writel(0x00000000, SCTL_BASE_ADDR + 0x0080);
		writel(0x04101050, SCTL_BASE_ADDR + 0x0084);
		writel(0x00000020, SCTL_BASE_ADDR + 0x0088);

	    reg = readl(SCTL_BASE_ADDR + 0x0030);  /*选择PLL=384MHz*/
		reg |= 1UL << 2;
		writel(reg, SCTL_BASE_ADDR + 0x0030);	
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    /*q00175519 MMC1给卡上电*/
    BSP_PMU_LDOVoltSet(LDO10,300);
    BSP_PMU_LDOCtrl(LDO10, BSP_TRUE);

    BSP_PMU_LDOVoltSet(LDO7,300);
    BSP_PMU_LDOCtrl(LDO7, BSP_TRUE);
#else
	reg = readl(SCTL_SC_PPLL);
	reg |=0x1;   /*PLL 使能*/
	writel(reg, SCTL_SC_PPLL);
#endif
	
    /*设置时钟分频比*/
    wlan_clock_ctl(1);

    hisdio_trace(1, "end");
    return ;
}/*lint !e529*/

/*******************************************************************************
  Function:      static void hi_sdio_control_cclk_low_power(struct hisdio_host *host,unsigned int cclk)
  Description:   set sdio clock 
  Input:         host:hisdio_host struct
                 flag:enable:low_power mode ,disable:no_low_power
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
 void hi_sdio_control_cclk_low_power(struct hisdio_host *host,unsigned int flag) 
{
	unsigned int reg;
	cmd_arg_s cmd_reg;

	hisdio_trace(1,"begin");

	reg = readl(host->base + MMC_CLKENA);
	if(flag == ENABLE)
	{
		reg |= CCLK_LOW_POWER;
	}
	else
	{
		reg &= ~CCLK_LOW_POWER;
	}
	
	writel(reg,host->base + MMC_CLKENA);

	cmd_reg.cmd_arg = readl(host->base + MMC_CMD); 
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.update_clk_reg_only = 1;
	
	writel(cmd_reg.cmd_arg,host->base + MMC_CMD);

	if(hi_sdio_wait_cmd(host) != 0)
	{
		hisdio_trace(1,"update sdio clk is timeout!");
	}
	
	hisdio_trace(1,"end");
}

/*******************************************************************************
  Function:      void hi_sdio_control_uhs_ddr_volt(struct hisdio_host *host,unsigned int cclk)
  Description:   set sdio uhs ddr mode and voltage choice.
  Input:         host:hisdio_host struct
                 ulDDRMode:DDR mode 0:non_DDR_MODE 1:DDR_MODE
                 ulVoltChocie:0:3.3 1:1.8
                 
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void hi_sdio_control_uhs_ddr_volt(struct mmc_host* mmc,unsigned int ulDDRMode,unsigned int ulVoltChocie)
{
	/*unsigned int ulReg = 0;*/
	struct hisdio_host *host = mmc_priv(mmc);
    /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
	if ( DDR_MODE_CHOICE == ulDDRMode)
	{
	    set_bit(DDR_MODE, (volatile unsigned long *)((unsigned long)host->base + MMC_UHSREG));
	}
	else
	{
	    clear_bit(DDR_MODE, (volatile unsigned long *)((unsigned long)host->base + MMC_UHSREG));
	}

	if ( VOLT_18V_CHOICE == ulVoltChocie)
	{
	    set_bit(VOLT_REG_18V, (volatile unsigned long *)((unsigned long)host->base + MMC_UHSREG));
	}
	else
	{
	    clear_bit(VOLT_REG_18V, (volatile unsigned long *)((unsigned long)host->base + MMC_UHSREG));
	}  
    /*END  :y00206456 2012-04-26 Modified for pclint e124*/

}

/*******************************************************************************
  Function:      static void hi_sdio_control_cclk(struct hisdio_host *host,unsigned int flag)
  Description:   set sdio clock mode
  Input:         host:hisdio_host struct
                 flag:ENABLE or disable
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

void hi_sdio_control_cclk(struct hisdio_host *host,unsigned int flag)
{
	unsigned int reg;
	cmd_arg_s cmd_reg;

	hisdio_trace(1,"begin");

	reg = readl(host->base + MMC_CLKENA);
	if(flag == ENABLE)
	{
		reg |= CCLK_ENABLE;
	}
	else
	{
		reg &= ~CCLK_ENABLE;
	}
	
	writel(reg,host->base + MMC_CLKENA);

	cmd_reg.cmd_arg = readl(host->base + MMC_CMD); 
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.update_clk_reg_only = 1;
	
	writel(cmd_reg.cmd_arg,host->base + MMC_CMD);

	if(hi_sdio_wait_cmd(host) != 0)
	{
		hisdio_trace(1,"update sdio clk is timeout!");
	}
	
	hisdio_trace(1,"end");
}

/*after cmd11 (volt switch cmd) is sent ,judge the result of the switch */
int hi_sdio_voltage_switch_check(struct mmc_host* mmc)
{    
    struct hisdio_host *host = mmc_priv(mmc);
    u32 cmd_irq_reg = 0;

    BSP_PWRCTRL_SD_LowPowerExit();
	
    /*2ms 以后判断是否产生volt switch int*/
    msleep(2);
	cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
	hisdio_trace(1,"cmd_irq_reg is 0x%x,HTO_int shall be 0x1 << 10",cmd_irq_reg);
	if(cmd_irq_reg & HTO_INT_STATUS)
    {    
        /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
        /*clear the VOLT_SWITCH_INT  state*/
        set_bit(HTO_INT_NUM, (volatile unsigned long *)((unsigned long)host->base + MMC_RINTSTS));
        /*END  :y00206456 2012-04-26 Modified for pclint e124*/
        hi_sdio_control_cclk(host,~ENABLE);
		
		BSP_PWRCTRL_SD_LowPowerExit();
        hi_sdio_control_uhs_ddr_volt(mmc,~DDR_MODE_CHOICE,VOLT_18V_CHOICE);
    /*++by pandong cs*/		
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
			gpio_direction_output(BALONG_GPIO_VOL, 0);
		#else
			hi_sdio_switch_drive_current(1);

	        hisdio_trace(1,"pmu LDO7 switch begin **********\n");
	        g_stSDLDO10Arg->LDO_id = LDO7;
			g_stSDLDO10Arg->is_voltage_switch = VOLT_SWTICH_YES;
			g_stSDLDO10Arg->open_close = OPEN_LDO;
			g_stSDLDO10Arg->voltage_val = LDO10_1_8V;		
			hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
			msleep(1500);
			hisdio_trace(1,"pmu LDO7 switch end **********\n");
		#endif
	#else
		hi_sdio_switch_drive_current(1);

	    hisdio_trace(1,"pmu LDO7 switch begin **********\n");
	    g_stSDLDO10Arg->LDO_id = LDO7;
		g_stSDLDO10Arg->is_voltage_switch = VOLT_SWTICH_YES;
		g_stSDLDO10Arg->open_close = OPEN_LDO;
		g_stSDLDO10Arg->voltage_val = LDO10_1_8V;		
		hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
		msleep(1500);
		hisdio_trace(1,"pmu LDO7 switch end **********\n");
	#endif
	/*--by pandong cs*/
#elif defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
        hisdio_trace(1,"pmu LDO7 switch 1.8V begin **********\n");

		hi_sdio_switch_drive_current(1);

        BSP_PMU_LDOVoltSet(LDO7,180);

		hisdio_trace(1,"pmu LDO7 switch 1.8V end **********\n");
#else      
       /*配置为1.8V,仅SFT需要配置，UDP需要去掉*/
		gpio_direction_output(BALONG_GPIO_VOL, 0);
#endif

        /*等待5ms，打开时钟*/
        msleep(5);
        hi_sdio_control_cclk(host,ENABLE);
        /*等待1ms，判断中断是否产生*/
		BSP_PWRCTRL_SD_LowPowerExit();
        msleep(1);
        /*check VOLT_SWITCH_INT and CD interrrpt*/
        cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
		hisdio_trace(1,"cmd_irq_reg is 0x%x,HTO_int is 0x1 << 10,cd is 0x1 << 2",cmd_irq_reg);
        if((!(cmd_irq_reg & HTO_INT_STATUS))||(!(cmd_irq_reg & CD_INT_STATUS)) )
        {
            /*下电之前需要关闭时钟*/
            hi_sdio_control_cclk(host,~ENABLE);
            /*mmc_set_clock(host,0);*/
            /*power cycle*/
            /*mmc_power_off(host);*/
            sys_ctrl_hisdio_power_io(POWER_OFF);
            /*printk("SD3.0: switch UHS-1 no volt_switch_int and cd!\n");*/
			hisdio_error("sd3.0 switch UHS-1 no volt_switch_int and cd int");
            return -1 ;   
        }
        else
        {
            /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
            /*clear  VOLT_SWITCH_INT and CD state*/
            set_bit(HTO_INT_NUM, (volatile unsigned long *)((unsigned long)host->base + MMC_RINTSTS));
            set_bit(CD_INT_NUM, (volatile unsigned long *)((unsigned long)host->base + MMC_RINTSTS));
            /*END  :y00206456 2012-04-26 Modified for pclint e124*/
            /*设置标志位，表示支持1.8V及是UHS-1卡*/
           /** g_ulUhsSupport = 1;*/
			hisdio_trace(5,"sd3.0 volt switch success!!!");
            
        }         
        
    }
    else
    {
        /*下电之前需要关闭时钟*/
        hi_sdio_control_cclk(host,0);
        /*mmc_set_clock(host,0);*/
        /*power cycle*/
        /*mmc_power_off(host);*/
        sys_ctrl_hisdio_power_io(POWER_OFF);
        /*printk("SD3.0:NO VOLT SWITCH INT!\n");*/
		hisdio_trace(1,"sd3.0 not volt switch int!");
        return -1;
    }
    return 0;
}

/*******************************************************************************
  Function:      static void hi_sdio_init_card(struct hisdio_host *host)
  Description:   init sdio card 
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void hi_sdio_init_card(struct hisdio_host *host)
{
	unsigned int tmp_reg; 
	unsigned int rx_wmark; 
	unsigned int tx_wmark;
	
	hisdio_trace(1,"begin");
    
	/* MASK MMC host intr */
	tmp_reg = 0x0;/*FIXME for which card is connected*/
	writel(tmp_reg,host->base + MMC_INTMASK);
    tmp_reg = 0xfffe;
    writel(tmp_reg,host->base  + MMC_RINTSTS);
    
	/* enable DMA mode and enable intr of MMC host controler */
	tmp_reg = readl(host->base + MMC_CTRL);
	
	if(dma)
	{
		tmp_reg |= DMA_ENABLE;
	}
	tmp_reg |= INT_ENABLE;
	
	writel(tmp_reg,host->base + MMC_CTRL);

	/* set timeout param */
	writel(DATA_TIMEOUT | RESPONSE_TIMEOUT,host->base + MMC_TMOUT);

	/* set FIFO param */	
	rx_wmark = host->fifo_depth/2 - 1;
	tx_wmark = host->fifo_depth/2;
	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | (rx_wmark<<16) | tx_wmark;     
	writel(tmp_reg,host->base + MMC_FIFOTH);
	hisdio_trace(1,"fifo_depth=%#x, rx_wmark=%#x, tx_wmark=%#x\n",host->fifo_depth,rx_wmark,tx_wmark);

	/*set clk 400k to enum*/      /*SDXC?*/
	hi_sdio_control_cclk(host,DISABLE);
	hi_sdio_set_cclk(host,400000);
	hi_sdio_control_cclk(host,ENABLE);
    
	hisdio_trace(1,"end");
}


/*******************************************************************************
  Function:      void hi_sdio_detectcard_to_core(unsigned int arg)
  Description:   notify sdio core cart status 
  Input:         arg:0 out 1 insert
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void hi_sdio_detectcard_to_core(unsigned int arg)
{
	hisdio_trace(1,"begin");
	if(1 == arg)
	{
		sdio_getcd = 1;
	
		/*卡在位，打开LDIO10  y00186965*/
		//BSP_IFC_Send(BSP_VOID * pMspBody,BSP_U32 u32Len);
#if defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
    	BSP_PMU_LDOCtrl(LDO10, BSP_TRUE);
        BSP_PMU_LDOCtrl(LDO7, BSP_TRUE);
#endif

		/* BEGIN:added by wzs in 2012.4.27 */
        wlan_clock_ctl(1);
		/* END:added by wzs in 2012.4.27 */

		hi_sdio_init_card(sdio_host_copy);
	}
	else
	{
		sdio_getcd = 0;
		g_host = NULL;
        
	#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
        || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
		#if defined(CHIP_BB_6756CS)
			#if defined(CS_SFT)
			#else
				hi_sdio_switch_drive_current(0);
			#endif
		#else
			hi_sdio_switch_drive_current(0);
		#endif
	/*--by pandong cs*/
	#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
		hi_sdio_switch_drive_current(0);
    #endif
    
        #if defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
    	BSP_PMU_LDOCtrl(LDO10, BSP_FALSE);
        BSP_PMU_LDOCtrl(LDO7, BSP_FALSE);
        #endif
		/*卡不在位，关闭LDIO10  y00186965*/
		//BSP_IFC_Send(BSP_VOID * pMspBody,BSP_U32 u32Len);
	}
    
	/*mmc_detect_change(sdio_host_copy->mmc, 0);*/   
	hisdio_trace(1,"end");
	return;
}

/*配置打印优先级*/
int hisdio_printk_level_set(int iValue)
{
	if (iValue < 0 || iValue > 3)
	{
		printk("pirntk level is over!\n");
		return -1;
	}
	switch (iValue)
	{
		case 0:
			sdio_trace_level = 0;
			break;
		case 1:
			sdio_trace_level = 1;
			break;
		case 2:
			sdio_trace_level = 2;
			break;
		case 3:
			sdio_trace_level = 3;
			break;
		default:
			break;

	}
	return sdio_trace_level;

}



#if !(BALONG_SDCARD_DMA)
/*zxf*/
static void transfer_idma_config(struct hisdio_host *host)
{
	unsigned long  ireg = 0;
	unsigned int   tmp_reg = 0;
	unsigned int rx_wmark; 
	unsigned int tx_wmark;
    SD_IDMAC_config_init();
	/*before use internal DMA, must diable RX & TX request interrupt*/
	ireg = 0;
	ireg = readl(host->base + MMC_INTMASK);
	ireg &=~(INTMASK_RXMASK |INTMASK_TXMASK);
	writel(ireg, host->base + MMC_INTMASK);

	/* set FIFO param */	
	rx_wmark = host->fifo_depth/2 - 1;
	tx_wmark = host->fifo_depth/2;
	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | (rx_wmark<<16) | tx_wmark;     
	writel(tmp_reg,host->base + MMC_FIFOTH);
	hisdio_trace(1,"fifo_depth=%#x, rx_wmark=%#x, tx_wmark=%#x\n",host->fifo_depth,rx_wmark,tx_wmark);
	/*enable internal dma & enable all host controller interrupt*/
	ireg = 0;
	ireg = readl(host->base + MMC_CTRL);
	ireg |= IDMA_ENABLE ;
	ireg |= INT_ENABLE;
	writel(ireg, host->base + MMC_CTRL);
}


/*******************************************************************************
  Function:      static void hi_sdio_dma_start(struct hisdio_host *host)
  Description:   start dma
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static int hi_sdio_dma_start(struct hisdio_host *host)
{
	struct scatterlist      *sgHeader;
	hisdio_trace(1,"begin");
	sgHeader = host->dma_sg;
	if (host->data->flags & MMC_DATA_READ) 
	{
		host->dma_dir = DMA_FROM_DEVICE;
	}
	else
	{
		host->dma_dir = DMA_TO_DEVICE;
	}

  	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
	/*lint -e64*/
	host->dma_len = (unsigned int)(dma_map_sg(mmc_dev(host->mmc), host->dma_sg, (int)host->data->sg_len,host->dma_dir)); /*clean lint e713*/
	/*lint +e64*/
	/*end*/
	transfer_idma_config(host);
	hisdio_trace(1,"end");
	return SD_IDMAC_SingleChannelStart(sgHeader, host->dma_len, host->dma_dir);	/*lint !e64*/
}
#endif
#if (BALONG_SDCARD_DMA)
/*y00186965*/
static void transfer_edma_config(struct hisdio_host *host)
{	
    unsigned long  ireg = 0;    
    unsigned long  tmp_reg = 0;    /*set edmac basic transfer config*/	
	unsigned int rx_wmark; 
	unsigned int tx_wmark;
	
	/* set FIFO param */	
	host->fifo_depth = ((readl(host->base + MMC_FIFOTH) & 0x0fff0000) >> 16) + 1;
	rx_wmark = host->fifo_depth/2 - 1;
	tx_wmark = host->fifo_depth/2;
	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | (rx_wmark<<16) | tx_wmark;    
	writel(tmp_reg,host->base + MMC_FIFOTH);
	hisdio_trace(1,"fifo_depth=%#x, rx_wmark=%#x, tx_wmark=%#x\n",host->fifo_depth,rx_wmark,tx_wmark);
    ireg = 0;	
    ireg = readl(host->base + MMC_CTRL);	
    ireg |= DMA_ENABLE ;    
    ireg &= ~IDMA_ENABLE ;	
    ireg |= INT_ENABLE;	
    writel(ireg, host->base + MMC_CTRL);
}
static int hi_sdio_edma_start(struct hisdio_host *host, BALONG_DMA_REQ req)
{
	struct scatterlist      *sgHeader;
	struct scatterlist      *sgNode;
    int ret_id = 0;
    int i      = 0;
    BALONG_DMA_CB *psttemp    = NULL;
    BALONG_DMA_CB *FirstNode   = NULL;
	int edma_trans_ret = 0;      /*y00186965*/
    sgHeader = host->dma_sg;
	sgNode = sgHeader;
    transfer_edma_config(host);
	hisdio_trace(1,"begin edma start ############");
    FirstNode = EDMA_free_list_node;
    psttemp = EDMA_free_list_node;
    if (host->data->flags & MMC_DATA_READ) 
	{		
		host->dma_dir = DMA_FROM_DEVICE;
        host->dma_len = dma_map_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
        for_each_sg(sgHeader, sgNode, host->dma_len, i)
        {
            psttemp->lli = BALONG_DMA_SET_LLI(edma_phy_addr + (i+1) * sizeof(BALONG_DMA_CB), ((i < host->dma_len - 1)?0:1));
            psttemp->config = BALONG_DMA_SET_CONFIG(req, BALONG_DMA_P2M, 2, 7); /*burst_size = 32,burst_len = 8*/
            psttemp->src_addr = 0x900ad200;  /*FIFO物理地址*/
            psttemp->des_addr = sg_dma_address(sgNode);  /*物理地址*/
            psttemp->cnt0 = sg_dma_len(sgNode);
            psttemp->bindx = 0;
            psttemp->cindx = 0;
            psttemp->cnt1  = 0;
            psttemp++;
        }
	}	
	else	
	{		
		host->dma_dir = DMA_TO_DEVICE;	
        host->dma_len = dma_map_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
        for_each_sg(sgHeader, sgNode, host->dma_len, i)
        {
            psttemp->lli = BALONG_DMA_SET_LLI(edma_phy_addr + (i+1) * sizeof(BALONG_DMA_CB), ((i < host->dma_len - 1)?0:1));
            psttemp->config = BALONG_DMA_SET_CONFIG(req, BALONG_DMA_M2P, 2, 7); /*burst_size = 32,burst_len = 8*/
            psttemp->src_addr = sg_dma_address(sgNode);  /*物理地址*/
            psttemp->des_addr = 0x900ad200;  /*FIFO物理地址*/
            psttemp->cnt0 = sg_dma_len(sgNode);
            psttemp->bindx = 0;
            psttemp->cindx = 0;
            psttemp->cnt1  = 0;
            psttemp++;
        }
	}    
    hisdio_trace(3,"LII list init is success!\n"); 
    ret_id = balong_dma_channel_init(req, NULL, 0, 0);
    if (ret_id < 0)
    {
        hisdio_trace(1,"-----------error ret_id = 0x%X\n",ret_id);
        return DMA_CHANNEL_INVALID;
    }
    psttemp = balong_dma_channel_get_lli_addr(ret_id);
    if (NULL == psttemp)
    {
        hisdio_trace(1,"---balong_dma_channel_get_lli_addr failed!\n");
        return DMA_CHANNEL_INVALID;
    }
    psttemp->config = 0;  /*配置前确保通道disable*/
    psttemp->lli = FirstNode->lli;
    psttemp->config = FirstNode->config & 0xFFFFFFFE; 
    psttemp->src_addr = FirstNode->src_addr;  /*物理地址*/
    psttemp->des_addr = FirstNode->des_addr;  /*物理地址*/
    psttemp->cnt0 = FirstNode->cnt0;
    psttemp->bindx = 0;
    psttemp->cindx = 0;
    psttemp->cnt1  = 0; 
    edma_trans_ret = balong_dma_channel_lli_start(ret_id);
    if (edma_trans_ret)
    {
        hisdio_trace(1,"balong_dma_channel_lli_start FAILED!\n");
        return edma_trans_ret;
    }
    hisdio_trace(1,"eDMA end!\n");
    return edma_trans_ret;
}
#endif

#if (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#else

/*zxf cpu copy only when command data need under 512 bytes transfer*/
static int  hi_sdio_cpu_start(struct hisdio_host *host)
{
    /*before cpu transfer, must disable dma_enable.*/
    unsigned int ireg = 0;
    struct scatterlist      *sg;
    unsigned int  *sg_vir = NULL;
    //char* sg_vir_t = NULL;
    unsigned int len = 0;
    unsigned long cpu_jiffies_timeout;
    ireg = readl(host->base + MMC_CTRL);
    ireg &= ~IDMA_ENABLE ;
    writel(ireg, host->base + MMC_CTRL);
    hisdio_trace(1,"begin");
    if(host->data->flags & MMC_DATA_READ)
    {
        /*only for command, one sg list*/    
        sg = &host->data->sg[0];
        len = sg->length;
        sg_vir = (unsigned int*)sg_virt(sg);
        hisdio_trace(2, "sg-buffer: 0x%p, sg_len: %u, phy : 0x%lx", sg_vir, sg->length, virt_to_phys(sg_vir));
        cpu_jiffies_timeout = jiffies + request_timeout;
        for(;;)  //b00198513 Modified for pclint e716          
        {
		    if(!time_before(jiffies,cpu_jiffies_timeout))
		    {
		    	hisdio_trace(1,"wait cpu transfer complete  timeout!");
		    	hisdio_trace(1,"end");	
		    	return -1;	        
		    }
        
            //hisdio_trace(2, "mmc status : 0x%x", readl(host->base + MMC_STATUS));
            if(len == 0)
                break;            
            if((readl(host->base + MMC_STATUS) & DMA_FIFO_EMPTY_MASK))
            {
                schedule();
                continue;
            }          
            hisdio_trace(2, "mmc status fifo count : 0x%x", (readl(host->base + MMC_STATUS) & 0x3FFE0000) >> 17);        
            hisdio_trace(2, "mmc status : 0x%x", readl(host->base + MMC_STATUS));
            *(sg_vir++) = readl(host->base + 0x200);
            len = len -4;
            hisdio_trace(2, " cpu read: 0x%x ,len:%d", *(sg_vir-1),len);
        }
    }    
    else
    {
        /*write block size must be 512*/
        return -EINVAL;
    }
    hisdio_trace(1,"end");
    return 0;
}
#endif
/*******************************************************************************
  Function:      static int hi_sdio_exec_cmd(struct hisdio_host *host, 
                                  struct mmc_command *cmd, struct mmc_data *data)
  Description:   send cmd to card
  Input:         host:hisdio_host struct
                 mmc_command:sdio cmd
                 mmc_data:the point of data
  Output:        NA
  Return:        0 sucess 1 do not support cmd other error
  Others:        NA
*******************************************************************************/
static int hi_sdio_exec_cmd(struct hisdio_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
	volatile cmd_arg_s  cmd_regs;
	unsigned int blk_size,tmp_reg,fifo_count = 0;	
      
	host->cmd = cmd;
    
    hisdio_trace(1,"cmd->opcode = %d, sdio_getcd %d",cmd->opcode, sdio_getcd);
    #if 0   /*syb*/
	if(8 == cmd->opcode  || 55 == cmd->opcode ||(0 == sdio_getcd && 7 == cmd->opcode))
	{
		//hisdio_trace(5,"cmd->opcode = %d, sdio_getcd %d",cmd->opcode, sdio_getcd);
	    return 1;
	}
    #endif
	
	if(data)
	{
		tmp_reg = readl(host->base + MMC_CTRL);
		tmp_reg |= FIFO_RESET;
		writel(tmp_reg,host->base + MMC_CTRL);
		
		do{
			tmp_reg = readl(host->base + MMC_CTRL);
			fifo_count++;
			
			if(fifo_count >= MAX_RETRY_COUNT)
			{
				hisdio_trace(5,"reset is timeout!");	
				return -EINVAL;
			} 
		}while((tmp_reg&FIFO_RESET)!=0);

		blk_size = data->blksz * data->blocks;
		writel(blk_size,host->base + MMC_BYTCNT);
		writel(data->blksz,host->base + MMC_BLKSIZ);
	}
	else
	{	    
		writel(0,host->base + MMC_BYTCNT);
		writel(0,host->base + MMC_BLKSIZ);
	}
	writel(cmd->arg,host->base + MMC_CMDARG);
	
	cmd_regs.cmd_arg = readl(host->base + MMC_CMD);
	if(data)
	{
		cmd_regs.bits.data_transfer_expected = 1;
		cmd_regs.bits.use_hold_reg = 1;
		if (data->flags & (MMC_DATA_WRITE | MMC_DATA_READ))
		{
			cmd_regs.bits.transfer_mode = 0;
		}
		
		if (data->flags & MMC_DATA_STREAM)
		{
			cmd_regs.bits.transfer_mode = 1;
		}
		
		if(data->flags & MMC_DATA_WRITE)
		{
		    hisdio_trace(1,"MMC_DATA_write");
			cmd_regs.bits.read_write = 1;
		}
		else if(data->flags & MMC_DATA_READ)
		{
			hisdio_trace(1,"MMC_DATA_READ");
			cmd_regs.bits.read_write = 0;
		}
	}
	else
	{
		cmd_regs.bits.data_transfer_expected = 0;
		cmd_regs.bits.transfer_mode = 0;
		cmd_regs.bits.read_write = 0;
		cmd_regs.bits.use_hold_reg = 1;
	}

	if(cmd == host->mrq->stop)
	{
		cmd_regs.bits.stop_abort_cmd = 1;
		cmd_regs.bits.wait_prvdata_complete = 0;
	}
	else
	{
		cmd_regs.bits.stop_abort_cmd = 0;
		cmd_regs.bits.wait_prvdata_complete = 1;
	}

	switch (mmc_resp_type(cmd)) 
	{
		case MMC_RSP_NONE:
			cmd_regs.bits.response_expect = 0;
			cmd_regs.bits.response_length = 0;
			cmd_regs.bits.check_response_crc = 0;
			break;
		case MMC_RSP_R1:
		/* case MMC_RSP_R5: */
		case MMC_RSP_R1B:
			cmd_regs.bits.response_expect = 1;
			cmd_regs.bits.response_length = 0;
			cmd_regs.bits.check_response_crc = 1;
			break;
		case MMC_RSP_R2:
			cmd_regs.bits.response_expect = 1;
			cmd_regs.bits.response_length = 1;
			cmd_regs.bits.check_response_crc = 1;
			break;
		case MMC_RSP_R3:
			cmd_regs.bits.response_expect = 1;
			cmd_regs.bits.response_length = 0;
			cmd_regs.bits.check_response_crc = 0;
			break;
		default:
			hisdio_trace(1,"hi_mci: unhandled response type %02x\n",mmc_resp_type(cmd));
			return -EINVAL;

	}
		
	if(cmd->opcode == 5 )
	{
		cmd_regs.bits.check_response_crc = 0;
		cmd_regs.bits.response_expect    = 1;
		cmd_regs.bits.response_length    = 0;
		cmd_regs.bits.stop_abort_cmd     = 0;         
	}
	
	if(cmd->opcode == MMC_GO_IDLE_STATE)
	{
		cmd_regs.bits.send_initialization = 1;
	}
	else
	{
		cmd_regs.bits.send_initialization = 0;
	}

    /*syb*/
    if ( cmd->opcode == 11)
    {        
        cmd_regs.bits.volt_switch = 1;        
    } 
	else 
	{
		cmd_regs.bits.volt_switch = 0;  
	}

	cmd_regs.bits.card_number = 0;
	cmd_regs.bits.cmd_index = cmd->opcode;
	cmd_regs.bits.send_auto_stop = 0;
	cmd_regs.bits.start_cmd = 1;
	cmd_regs.bits.update_clk_reg_only = 0;
	
	
	writel(cmd_regs.cmd_arg,host->base + MMC_CMD);	
	if(hi_sdio_wait_cmd(host) != 0)
	{
		hisdio_trace(5,"send card cmd is failed!");
		return -EINVAL;            
	}
	hisdio_trace(1,"end");
	return 0;
}

/*******************************************************************************
  Function:      static void hi_sdio_finish_request(struct hisdio_host *host, struct mmc_request *mrq)
  Description:   sdio cmd finish
  Input:         NA
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void hi_sdio_finish_request(struct hisdio_host *host, struct mmc_request *mrq)
{
	hisdio_trace(1,"begin");
	
	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;
	mmc_request_done(host->mmc, mrq);
	
	hisdio_trace(1,"end");
}

/*******************************************************************************
  Function:      static void hi_sdio_cmd_done(struct hisdio_host *host, unsigned int stat)
  Description:   read card resp
  Input:         host:hisdio_host struct
                 stat 
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void hi_sdio_cmd_done(struct hisdio_host *host, unsigned int stat)
{
	unsigned int i;
	struct mmc_command *cmd = host->cmd;

	hisdio_trace(1,"begin");
	
	host->cmd = NULL;

	for(i=0;i<4;i++)
	{
		if(mmc_resp_type(cmd) == MMC_RSP_R2)
		{	
			cmd->resp[i] = readl(host->base + MMC_RESP3 - i * 0x4); 
		}
		else
		{
			cmd->resp[i] = readl(host->base + MMC_RESP0 + i * 0x4); 
		}
	}
	if(stat & RTO_INT_STATUS)
	{
		cmd->error = (unsigned int)(-ETIMEDOUT);
        if(host->mmc->caps & MMC_CAP_NEEDS_POLL)
        {
            hisdio_trace(4,"irq cmd status stat = 0x%x is timeout error!",stat);
        }
        else
        {
            hisdio_trace(5,"irq cmd status stat = 0x%x is timeout error!",stat);
        }
		
	}
	else if(stat & (RCRC_INT_STATUS | RE_INT_STATUS))
	{
		cmd->error = (unsigned int)(-EILSEQ);
        if(host->mmc->caps & MMC_CAP_NEEDS_POLL)
        {
            hisdio_trace(5,"irq cmd status stat = 0x%x is response error!",stat);
        }
        else
        {
            hisdio_trace(5,"irq cmd status stat = 0x%x is response error!",stat);
        }		
	}
	hisdio_trace(1,"end");
}

/*******************************************************************************
  Function:      static void hi_sdio_data_done(struct hisdio_host *host, unsigned int stat)
  Description:   sdio data handle
  Input:         host:hisdio_host struct
                 stat 
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void hi_sdio_data_done(struct hisdio_host *host, unsigned int stat)
{
	struct mmc_data *data = host->data;

	hisdio_trace(1,"begin");
	/*数据传输低于512Bytes，CPU搬运*/
	if(host->data && (!(host->data->sg_len == 1 && (host->data->flags &  MMC_DATA_READ) && host->data->sg[0].length < 512)))
	{
	    dma_unmap_sg(mmc_dev(host->mmc), data->sg, (int)host->dma_len,host->dma_dir); /*lint !e64*//*clean lint e713*/
	}

	if(stat & (HTO_INT_STATUS | DRTO_INT_STATUS))
	{
		data->error = (unsigned int)(-ETIMEDOUT);
		hisdio_trace(5,"irq data status stat = 0x%x is timeout error!",stat);
	}
	else if(stat & (EBE_INT_STATUS | SBE_INT_STATUS | FRUN_INT_STATUS | DCRC_INT_STATUS))
	{
		data->error = (unsigned int)(-EILSEQ);
		hisdio_trace(5,"irq data status stat = 0x%x is data error!",stat);
	}
	if (!data->error)
	{
		data->bytes_xfered = data->blocks * data->blksz;

	}
	else
	{
		data->bytes_xfered = 0;
	}
	host->data = NULL;
	
	hisdio_trace(1,"end");
}


/*******************************************************************************
  Function:      static int hi_sdio_wait_cmd_complete(struct hisdio_host *host)
  Description:   sdio data handle
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/

static int hi_sdio_wait_cmd_complete(struct hisdio_host *host)
{
	unsigned int cmd_retry_count = 0;
	unsigned long cmd_jiffies_timeout;
	unsigned int cmd_irq_reg = 0;
	struct mmc_command *cmd = host->cmd;

	hisdio_trace(1,"begin");
	
	cmd_jiffies_timeout = jiffies + request_timeout;
	
    for(;;)  //b00198513 Modified for pclint e716          
	{
		if(!time_before(jiffies,cmd_jiffies_timeout))
		{
			cmd->error = (unsigned int)(-ETIMEDOUT);
			hisdio_trace(5,"wait cmd request complete is timeout!");
			hisdio_trace(1,"end");	
			return -1;	        
		}
		
		do{
			cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
			//cmd_irq_reg = (cmd_irq_reg & ~(0x1<<3));/*do not clear DTO interrupt by zxf*/
			/*CMD11 doesn't cause the CD int*/
			if (cmd->opcode == 11)
			{
				udelay(100);
				hisdio_trace(1,"cmd11 cmd_irq_reg = 0x%x",cmd_irq_reg);
				hi_sdio_cmd_done(host,cmd_irq_reg);
				hisdio_trace(1,"cmd11 doesn't cause cd int!");
				return 0;
			}
			if(cmd_irq_reg & CD_INT_STATUS)
			{
				/* clear interrupt */
				//writel(cmd_irq_reg,host->base + MMC_RINTSTS);
				writel(CD_INT_STATUS,host->base + MMC_RINTSTS);
				hi_sdio_cmd_done(host,cmd_irq_reg);
				hisdio_trace(1,"end");
				return 0;
			}
			cmd_retry_count++;		    
			//udelay(10);
		}while(cmd_retry_count < MAX_RETRY_COUNT);
		schedule();
	}
}

/*******************************************************************************
  Function:      static int hi_sdio_wait_data_complete(struct hisdio_host *host)
  Description:   sdio data handle
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/

static int hi_sdio_wait_data_complete(struct hisdio_host *host)
{
	unsigned int data_irq_reg = 0;
	int ret = 0; 
	struct mmc_data *data = host->data;

    /*unsigned int data_retry_count = 0;
	unsigned long data_jiffies_timeout;*/
    
    
	hisdio_trace(1,"begin");
#if 0	    
	if(data->sg->length <= SDIO_DATA_MAX_POLL)  /**SDXC?*/
	{
		request_timeout = SDIO_DATA_TIMEOUT;
		
		data_jiffies_timeout = jiffies + request_timeout;
		while(1)
		{
			if(!time_before(jiffies,data_jiffies_timeout))
			{
				data->error = -ETIMEDOUT;
				hisdio_trace(5,"wait data request complete is timeout!");
				hisdio_trace(1,"end");
				return -1;	        
			}
			do
			{
				data_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
				
				if(data_irq_reg & DTO_INT_STATUS)
				{
					/* clear interrupt */
					writel(data_irq_reg,host->base + MMC_RINTSTS);   	
					hi_sdio_data_done(host,data_irq_reg);
					hisdio_trace(1,"end");
					return 0;
				}
				data_retry_count++;		    
			}while(data_retry_count < MAX_RETRY_COUNT);

			schedule();
		}	
	}
	else
#endif	
	{
        /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
        /*open DTO interrupt*/
		set_bit(DTO_INT_NUM, (volatile unsigned long *)((unsigned long)host->base + MMC_INTMASK));
		/*END  :y00206456 2012-04-26 Modified for pclint e124*/
		ret = (int)wait_for_completion_timeout(&hisdio_cmd_complete, SDIO_DATA_TIMEOUT); /*clean lint e713*/
		
		data_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
		
		/* clear interrupt */
	    writel(data_irq_reg,host->base + MMC_RINTSTS);	
	    
		if (!ret) 
		{
			data->error = (unsigned int)(-ETIMEDOUT);
			hisdio_trace(5,"wait data request complete is timeout!");
			hisdio_trace(1,"end");
			return -1;	
		}
		else
		{
			hi_sdio_data_done(host,data_irq_reg);
			hisdio_trace(1,"end");
			return 0;
		}	
	}	
}

/*******************************************************************************
  Function:      static int hi_sdio_wait_card_complete(struct hisdio_host *host)
  Description:   sdio data handle
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/
/*SD busy 轮询*/
static int hi_sdio_wait_card_complete(struct hisdio_host *host)
{
	unsigned int card_retry_count = 0;
	unsigned long card_jiffies_timeout;
	unsigned int card_status_reg = 0;

	hisdio_trace(1,"begin");
	
	card_jiffies_timeout = jiffies + request_timeout;
    for(;;)  //b00198513 Modified for pclint e716          
	{
		if(!time_before(jiffies,card_jiffies_timeout))
		{
			hisdio_trace(5,"wait card ready complete is timeout!");
			hisdio_trace(1,"end");
			return -1;	        
		}
		
		do
		{
			card_status_reg = readl(host->base + MMC_STATUS);
			if(!(card_status_reg & DATA_BUSY))
			{
				hisdio_trace(1,"end");
				return 0;
			}
			card_retry_count++;		    
		    //udelay(10);
		}while(card_retry_count < MAX_RETRY_COUNT);
		schedule();
	}
}

/*******************************************************************************
  Function:      static int hi_sdio_wait_data_busy_complete(struct hisdio_host *host)
  Description:   sdio data busy check handle
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/
    /*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && !defined(CHIP_BB_6756CS)) || (defined(BOARD_SFT) && defined(VERSION_V7R1)) /*y00186965*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#else
static int hi_sdio_wait_data_busy_complete(struct hisdio_host *host)
{
	unsigned int data_irq_reg = 0;
	int ret = 0; 

	/*首先需要清除BUSY中断，规避芯片读操作误报busy完成中断*/	
	set_bit(MMC0_BUSY_INT_CLEAR, SCTL_INT_CLEAR);
	
	/*检查card data busy是否存在，不存在则直接返回*/
	data_irq_reg = readl(host->base + MMC_STATUS);

	if (!(data_irq_reg & DATA_BUSY))
	{
		hisdio_trace(1,"end");
		return 0;
	}
	/*open data busy check interrupt*/
	set_bit(MMC0_BUSY_INT_EN, SCTL_INT_EN1);
	
	ret = wait_for_completion_timeout(&hisdio_data_busy_complete, SDIO_DATA_BUSY_CHECK_TIMEOUT);	
	
	/* clear interrupt */
    set_bit(MMC0_BUSY_INT_CLEAR, SCTL_INT_CLEAR);
    
	if (!ret) 
	{		
		hisdio_trace(5,"wait data busy check complete is timeout!");
		hisdio_trace(1,"end");
		return -1;	
	}
	else
	{		
		hisdio_trace(1,"end");
		return 0;
	}	

}
#endif
	/*--by pandong cs*/
/*******************************************************************************
  Function:      static irqreturn_t hisdio_irq(int irq, void *devid)
  Description:   sdio interrupt handle
  Input:         irq:irq number
                 devid:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static irqreturn_t hisdio_irq(int irq, void *devid)
{
	struct hisdio_host *host = devid;
    int ireg = 0;
    unsigned int mask_ints = 0;
    int handle = 0; 	
	ireg = readl(host->base + MMC_MINTSTS);
	mask_ints = readl(host->base + MMC_IDSTS);
    hisdio_trace(1,"begin : ireg: 0x%x, raw: 0x%x, intmask: 0x%x, reg8c:0x%x ", ireg,readl(host->base + 0x44),
                readl(host->base + MMC_INTMASK), readl(host->base + 0x8c));
    #if 0  /*syb*/
	if (ireg & SDIO_INT_STATUS) 
	{
		handle = 1;
		ireg = readl(host->base + MMC_INTMASK);
		ireg &= ~0xffff0000;
		writel(ireg, host->base + MMC_INTMASK);
		
		mmc_signal_sdio_irq(host->mmc);
	}
    #endif
  
	if(ireg & DTO_INT_STATUS)
	{
		handle = 1;
		 /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
		/*mask DTO interrupt*/
	    clear_bit(DTO_INT_NUM, (volatile unsigned long *)((unsigned long)host->base + MMC_INTMASK));
		 /*END  :y00206456 2012-04-26 Modified for pclint e124*/
		complete(&hisdio_cmd_complete);
		hisdio_trace(1, "DTO complete");
	}

	if(mask_ints &(DmaIntAbnormal | DmaIntNormal | DmaIntCardErrSum |DmaIntBusError | DmaIntRxCompleted | DmaIntTxCompleted))
	{
	    hisdio_trace(1,"internal dma error : ireg: 0x%x, raw: 0x%x, intmask: 0x%x, reg8c:0x%x ", ireg,readl(host->base + 0x44),
                readl(host->base + MMC_INTMASK), readl(host->base + 0x8c));
        writel(mask_ints, host->base + SDIO_IDSTS);        
	}
    	
	if(handle)
	{
		return IRQ_HANDLED;
	}
	
	return IRQ_NONE;
}

/*******************************************************************************
  Function:      sd_detect_handler(struct work_struct *data)
  Description:   sd detect handle work
  Input:         data:work_struct
                 devid:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void sd_detect_handler(struct work_struct *data)
{    
	struct hisdio_host *host
	= container_of(data, struct hisdio_host,
			sd_detect_work);    
    unsigned int ucData = 0;//pclint 734
	/*comment spin lock, when sleep , can not use this lock*/
    //spin_lock(&host->lock);
   
    /*查询SD卡识别中断状态*/
    ucData = gpio_get_value(BALONG_GPIO_DETECT);/*lint !e732 */
    
    if (!ucData)   /*插入中断*/
    {
        printk(KERN_INFO"sd_detect_handler:card may insert!\n");
    
        /*防抖处理*/
        mdelay(1000);
        ucData = gpio_get_value(BALONG_GPIO_DETECT);/*lint !e732 */
        if (ucData) /*假中断，不处理*/
        {
	        gpio_int_unmask_set(BALONG_GPIO_DETECT);
            //spin_unlock(&host->lock); 
            printk("sd_detect_tasklet_handler: GPIO dithering,not a ture instert int !\n");
            return;
        }

        printk(KERN_INFO"sd_detect_handler:card do insert!\n");
        
#if (FEATURE_E5 == FEATURE_ON)
	    /*上报netlink */
        report_sd_state(DEVICE_ID_SD, KEY_WAKEUP, "SD WakeUp");  /*l00205892*/
#endif

        /*设置下次中断的触发条件，高电平触发拔出中断*/
        gpio_int_trigger_set(BALONG_GPIO_DETECT, IRQ_TYPE_LEVEL_HIGH);

		BSP_PWRCTRL_SD_LowPowerExit();
		
        sys_ctrl_hisdio_init_io();
		
        /*设置卡是否在位标志并初始化卡*/
        hi_sdio_detectcard_to_core(1);

		/*配置SD IO电压为3.3V*/
        hi_sdio_control_uhs_ddr_volt(host->mmc,~DDR_MODE_CHOICE,~VOLT_18V_CHOICE); /*lint !e413*//*y00186965*/

    /*y00186965  LDO*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
			gpio_direction_output(BALONG_GPIO_VOL, 1); 
		#else
			hisdio_trace(1,"pmu LDO7 switch 3.0V begin **********\n");
			g_stSDLDO10Arg->LDO_id = LDO7;	
			g_stSDLDO10Arg->is_voltage_switch = VOLT_SWTICH_YES;
			g_stSDLDO10Arg->open_close = OPEN_LDO;
			g_stSDLDO10Arg->voltage_val = LDO10_3_0V;	
			hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
	        msleep(5);
			hisdio_trace(1,"pmu LDO7 switch 3.0V end **********\n");
		#endif
	#else
        /*y00186965 初始化时，电压已切换为3.0v*/
		hisdio_trace(1,"pmu LDO7 switch 3.0V begin **********\n");
		g_stSDLDO10Arg->LDO_id = LDO7;	
		g_stSDLDO10Arg->is_voltage_switch = VOLT_SWTICH_YES;
		g_stSDLDO10Arg->open_close = OPEN_LDO;
		g_stSDLDO10Arg->voltage_val = LDO10_3_0V;	
		hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
        msleep(5);
		hisdio_trace(1,"pmu LDO7 switch 3.0V end **********\n");

		//hisdio_trace(1,"pmu LDO10 open begin **********\n");		
		//g_stSDLDO10Arg->LDO_id = LDO10;
		//hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
		//msleep(5);
		//hisdio_trace(1,"pmu LDO710 open end **********\n");
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
		hisdio_trace(1,"pmu LDO7 switch 3.0V begin **********\n");
        BSP_PMU_LDOVoltSet(LDO7,300);
		hisdio_trace(1,"pmu LDO7 switch 3.0V end **********\n");
#else
		/*配置为3V,仅SFT需要配置，UDP需要去掉*/
		gpio_direction_output(BALONG_GPIO_VOL, 1); /*y00186965*/
#endif   

    }
    else  /*拔出中断*/
    {
        printk(KERN_INFO"sd_detect_handler:card may eject!\n");
        
        mdelay(1000);
        ucData = gpio_get_value(BALONG_GPIO_DETECT);/*lint !e732 */
        if(!ucData) /*假中断，不处理*/
        {
	        gpio_int_unmask_set(BALONG_GPIO_DETECT);
            //spin_unlock(&host->lock);
            printk(KERN_ERR"sd_detect_tasklet_handler: GPIO dithering,not a ture draw int !\n");
            return;
        }

        printk(KERN_INFO"sd_detect_handler:card do eject!\n");

#if (FEATURE_E5 == FEATURE_ON)
	 	/*上报netlink */
        report_sd_state(DEVICE_ID_SD, SD_REMOVE, "SD remove");  /*l00205892*/
#endif        
		/*设置下次中断的触发条件，低电平触发插入中断*/
	    gpio_int_trigger_set(BALONG_GPIO_DETECT,IRQ_TYPE_LEVEL_LOW);

		BSP_PWRCTRL_SD_LowPowerExit();

        /*设置卡是否在位标志*/
        hi_sdio_detectcard_to_core(0); 
    }     
    mmc_detect_change(host->mmc, msecs_to_jiffies(500)); /*lint !e413*/

    /*开中断*/
    gpio_int_unmask_set(BALONG_GPIO_DETECT);
#if 0	
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) || (defined(BOARD_SFT) && defined(VERSION_V7R1)) /*y00186965  LDO*/
    hisdio_trace(1,"pmu LDO10 close begin **********\n");
    g_stSDLDO10Arg->LDO_id = LDO10;	
	g_stSDLDO10Arg->is_voltage_switch = VOLT_SWTICH_NO;
	g_stSDLDO10Arg->open_close = CLOSE_LDO;
	g_stSDLDO10Arg->voltage_val = 0;	
	hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
    msleep(5);
	hisdio_trace(1,"pmu LDO10 close end **********\n");
	
	hisdio_trace(1,"pmu LDO7 close begin **********\n");
	g_stSDLDO10Arg->LDO_id = LDO7;
	hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
	msleep(5);
	hisdio_trace(1,"pmu LDO7 close end **********\n");
#else

#endif
#endif
    //spin_unlock(&host->lock);  

}

/*******************************************************************************
  Function:      hi_sd_detect_irq_cd(int irq, void *dev_id)
  Description:   sd  detect interrpt handle
  Input:         irq: irq number
                 devid: device id
  Output:        irq handle result
  Return:        NA
  Others:        NA
*******************************************************************************/

static irqreturn_t hi_sd_detect_irq_cd(int irq, void *dev_id)    
{
	struct hisdio_host *host = (struct hisdio_host *)dev_id;    
    unsigned int ucData = 0;  
	
	hisdio_trace(1,"card detect");
    /*判断是否为GPIO2_5的中断*/
    gpio_int_state_get(BALONG_GPIO_DETECT, (unsigned*)&ucData);
    if (!ucData)
    {
        return IRQ_NONE;
    }

	BSP_PWRCTRL_SD_LowPowerExit();
    /*锁中断，清状态*/
    gpio_int_mask_set(BALONG_GPIO_DETECT);
    gpio_int_state_clear(BALONG_GPIO_DETECT);
        
    /*schedule_work(&host->sd_detect_work);*/
	    queue_work(host->detect_queue, &host->sd_detect_work);
    return IRQ_HANDLED;
}

/*******************************************************************************
  Function:      hi_sd_busy_detect_irq_cd(int irq,void *dev_id)
  Description:   sd  busy detect interrpt handle
  Input:         irq: irq number
                 devid: device id
  Output:        irq handle result
  Return:        NA
  Others:        NA
*******************************************************************************/
    /*++by pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
	#if defined(CHIP_BB_6756CS)
	static irqreturn_t hi_sd_busy_detect_irq_cd(int irq,void *dev_id)
	{	
		u32 ulData = 0;
		/*struct hisdio_host *host = (struct hisdio_host *)dev_id;*/  
		/*判断是否为SD卡的busy查询中断*/
		ulData = readl(SCTL_INT_STATM1);
		if (!(ulData & SDMMC0_WR_OVER_INT_STAT))
		{
			return IRQ_NONE;
		}
		hisdio_trace(1, "sd busy detect int complete");
		/*锁中断，清状态*/
		clear_bit(MMC0_BUSY_INT_EN, SCTL_INT_EN1);
		set_bit(MMC0_BUSY_INT_CLEAR, SCTL_INT_CLEAR);	
		
		complete(&hisdio_data_busy_complete);
		return IRQ_HANDLED;

	}
	#endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#else
static irqreturn_t hi_sd_busy_detect_irq_cd(int irq,void *dev_id)
{	
	u32 ulData = 0;
	/*struct hisdio_host *host = (struct hisdio_host *)dev_id;*/  
	/*判断是否为SD卡的busy查询中断*/
	ulData = readl(SCTL_INT_STATM1);
	if (!(ulData & SDMMC0_WR_OVER_INT_STAT))
	{
		return IRQ_NONE;
	}
	hisdio_trace(1, "sd busy detect int complete");
	/*锁中断，清状态*/
	clear_bit(MMC0_BUSY_INT_EN, SCTL_INT_EN1);
	set_bit(MMC0_BUSY_INT_CLEAR, SCTL_INT_CLEAR);	
	
	complete(&hisdio_data_busy_complete);
	return IRQ_HANDLED;

}
#endif
	/*--by pandong cs*/

/*******************************************************************************
  Function:      static void hi_sdio_request(struct mmc_host* mmc, struct mmc_request* mrq)
  Description:   sdio request handle
  Input:         mmc:mmc_host struct
                 mrq:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void hi_sdio_request(struct mmc_host* mmc, struct mmc_request* mrq)
{
	struct hisdio_host *host = mmc_priv(mmc);
	int ret = 0;
    unsigned int cmd_irq_reg = 0;
#if (BALONG_SDCARD_DMA)
	int edma_ret = 0;
#endif
    
	hisdio_trace(1,"begin");
    BSP_PWRCTRL_SD_LowPowerExit();
    if(g_suspend)
    {
    	hisdio_trace(5,"sdio is not resume!");
    	return;
    }
    if(NULL == host || NULL == mrq)
    {
    	hisdio_trace(5,"input param is error!");
    	return ;
    }
    
	host->mrq = mrq;
    

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
		#else
		    ret = hi_sdio_wait_card_complete(host);
		    
		    if(ret)
			{
				mrq->cmd->error = ret;

		    	goto request_end;
			}
		#endif
	#else
   /*轮询查询busy状态y00186965*/
    ret = hi_sdio_wait_card_complete(host);
    
    if(ret)
	{
		mrq->cmd->error = ret;

    	goto request_end;
	}
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	/*轮询查询busy状态q00175519*/
	ret = hi_sdio_wait_card_complete(host);
	if(ret)
	{
		/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
		mrq->cmd->error = (unsigned int)ret;
		/*end*/
		goto request_end;
	}
#endif

    /*clear all interrupt*/
    cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
    writel(cmd_irq_reg,host->base + MMC_RINTSTS);

    if(mrq->data)
    {
        host->data = mrq->data;
		host->dma_sg = mrq->data->sg;
		
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
	 	if(!(mrq->data->sg_len == 1 && (mrq->data->flags &  MMC_DATA_READ) && mrq->data->sg[0].length < 512))
		{
		    /*clear all interrupt*/
    		//cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
    		//writel(cmd_irq_reg,host->base + MMC_RINTSTS);
    		#if !(BALONG_SDCARD_DMA)
		    ret = hi_sdio_dma_start(host);
		    if(ret)//dma error
		    {
		        hisdio_trace(5,"dma error! address alignment error or NULL point error!");
		        mrq->data->error = -ENOBUFS;
		        goto request_end;
		    }
    		#endif
		}
	#else
     #if !(BALONG_SDCARD_DMA)
		    ret = hi_sdio_dma_start(host);
		    if(ret)//dma error
		    {
		        hisdio_trace(5,"dma error! address alignment error or NULL point error!");
		        mrq->data->error = -ENOBUFS;
		        goto request_end;
		    }
     #endif
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    #if !(BALONG_SDCARD_DMA)
		ret = hi_sdio_dma_start(host);
		if(ret)//dma error
		{
		   hisdio_trace(5,"dma error! address alignment error or NULL point error!");
		   mrq->data->error = (unsigned int)(-ENOBUFS);
		   goto request_end;
		}
    #endif	
#else
	 	if(!(mrq->data->sg_len == 1 && (mrq->data->flags &  MMC_DATA_READ) && mrq->data->sg[0].length < 512))
		{
		    /*clear all interrupt*/
    		//cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
    		//writel(cmd_irq_reg,host->base + MMC_RINTSTS);
    #if !(BALONG_SDCARD_DMA)
		    ret = hi_sdio_dma_start(host);
		    if(ret)//dma error
		    {
		        hisdio_trace(5,"dma error! address alignment error or NULL point error!");
		        mrq->data->error = -ENOBUFS;
		        goto request_end;
		    }
    #endif
		}
#endif
    }
    
    /* send command */
	ret = hi_sdio_exec_cmd(host,mrq->cmd,mrq->data);
	if(ret)
	{	
		/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
		mrq->cmd->error = (unsigned int)ret;
		/*end*/
		hisdio_trace(5,"cmd execute is error!");
		goto request_end;
	}

	/* wait command send complete */
	ret = hi_sdio_wait_cmd_complete(host);
	if(ret)
	{
		/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
		mrq->cmd->error = (unsigned int)ret;
		/*end*/

		goto request_end;
	}
	
	if(!(mrq->data && !mrq->cmd->error))
	{
		goto request_end;    
	}

	/* start data transfer */
	if(mrq->data)
	{
		host->data = mrq->data;
		host->dma_sg = mrq->data->sg;
		
		/*clear all interrupt*/
        hisdio_trace(1,"data sg len %d ,sg[0].len: %d, block size: %d, blzs: %d", mrq->data->sg_len, mrq->data->sg[0].length, mrq->data->blksz, mrq->data->blocks);      

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
			if(mrq->data->sg_len == 1 && (mrq->data->flags &  MMC_DATA_READ) && mrq->data->sg[0].length < 512)
        	{
            	ret = hi_sdio_cpu_start(host);
		    	if(ret)
		    	{
		    	mrq->cmd->error = ret;
		    	goto request_end;
		    	}
                
        	}				
    		#if BALONG_SDCARD_DMA
        	else
        	{
            	edma_ret = hi_sdio_edma_start(host,EDMA_MMC0);
            	if(edma_ret)
            	{
                hisdio_trace(5,"dma error! ");
                mrq->data->error = -ENOBUFS;
                goto request_end;
            	}
            	hisdio_trace(1,"hisdio_edma_trans_complete!");
        	}
    		#endif
	#else
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#else
		if(mrq->data->sg_len == 1 && (mrq->data->flags &  MMC_DATA_READ) && mrq->data->sg[0].length < 512)
        {
            ret = hi_sdio_cpu_start(host);
		    if(ret)
		    {
		    	mrq->cmd->error = ret;
		    	goto request_end;
		    }
                
        }				
    #if BALONG_SDCARD_DMA
        else
        {
            edma_ret = hi_sdio_edma_start(host,EDMA_MMC0);
            if(edma_ret)
            {
                hisdio_trace(5,"dma error! ");
                mrq->data->error = -ENOBUFS;
                goto request_end;
            }
            hisdio_trace(1,"hisdio_edma_trans_complete!");
        }
    #endif
#endif
		/* wait data transfer complete */	
		ret = hi_sdio_wait_data_complete(host);
		
		hisdio_trace(1,"hi_mci_wait_data_complete OK\n");
		
		if(ret)
		{
			/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
			mrq->cmd->error = (unsigned int)ret;
			/*end*/
			goto request_end;
		}
		if (mrq->stop)
		{
		    hisdio_trace(1, "mrq->stop");
			/* send stop command */
			ret = hi_sdio_exec_cmd(host,host->mrq->stop,host->data);
			if(ret)
			{
				/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
				mrq->cmd->error = (unsigned int)ret;
				/*end*/
				hisdio_trace(1,"end");	
				goto request_end;						
			}
			
			ret = hi_sdio_wait_cmd_complete(host);
			if(ret)
			{
				goto request_end;
			}
		}
		if (mrq->data->flags & MMC_DATA_WRITE)
		{
			/* wait card write data complete */
			//hi_sdio_wait_card_complete(host,mrq->data);
			/*等待卡写完成中断*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
			ret  = hi_sdio_wait_data_busy_complete(host);
			if (ret)
			{
				mrq->cmd->error = ret;
		    	goto request_end;
			}

	#else
		    /*轮询查询busy状态y00186965*/
			ret = hi_sdio_wait_card_complete(host);
		    
		    if(ret)
			{
				mrq->cmd->error = ret;

		    	goto request_end;
			}
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

			/*轮询查询busy状态q00175519*/
			ret = hi_sdio_wait_card_complete(host);
			if(ret)
			{
				/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
				mrq->cmd->error = (unsigned int)ret;
				/*end*/
				goto request_end;
			}
#else
		    
			ret  = hi_sdio_wait_data_busy_complete(host);
			if (ret)
			{
				mrq->cmd->error = ret;
		    	goto request_end;
			}
#endif
		}
	}
    
request_end:	
	hi_sdio_finish_request(host,mrq);
	if(SD_SEND_VOLTAGE_SWITCH != mrq->cmd->opcode)
	{
		BSP_PWRCTRL_SD_LowPowerEnter();
	}
    
	hisdio_trace(1,"end");
}

/*******************************************************************************
  Function:      static void hi_sdio_set_ios(struct mmc_host* mmc, struct mmc_ios* ios)
  Description:   sdio request handle
  Input:         mmc:mmc_host struct
                 mrq:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void hi_sdio_set_ios(struct mmc_host* mmc, struct mmc_ios* ios)
{
	struct hisdio_host *host = mmc_priv(mmc);
	unsigned int tmp_reg;
	
	hisdio_trace(1," begin");
	
	BSP_PWRCTRL_SD_LowPowerExit();
	
	hisdio_trace(1,"ios->power_mode = %d ",ios->power_mode);
	
	if(g_suspend)
    {
    	hisdio_trace(5,"sdio is not resume!");
    	return;
    }
	
	switch(ios->power_mode)
	{
		case MMC_POWER_OFF:
			sys_ctrl_hisdio_power_io(POWER_OFF);   /*未实现*/
			break;
		case MMC_POWER_UP:
		case MMC_POWER_ON: 
			sys_ctrl_hisdio_power_io(POWER_ON);  /*未实现*/
			break;
		/*pc-lint 744*/
		default:
			break;
	}
    
    if(ios->bus_width != g_bitmode)
	{
		if (ios->bus_width == MMC_BUS_WIDTH_4)
		{
			tmp_reg = readl(host->base + MMC_CTYPE);
			tmp_reg |= CARD_WIDTH;
			writel(tmp_reg,host->base + MMC_CTYPE);
			g_bitmode = MMC_BUS_WIDTH_4;
		}
		else
		{
			tmp_reg = readl(host->base + MMC_CTYPE);
			tmp_reg &= ~CARD_WIDTH;
			writel(tmp_reg,host->base + MMC_CTYPE);
			g_bitmode = MMC_BUS_WIDTH_1;
		}
		msleep(5);
	}
	
	hisdio_trace(1,"ios->clock = %d ",ios->clock);
	
	if((int)ios->clock != g_clock)
	{
		g_clock = (int)ios->clock; /*clean lint e713*/
	
		if (ios->clock) 
		{
			hi_sdio_control_cclk(host,DISABLE);
			hi_sdio_set_cclk(host,ios->clock);
			hi_sdio_control_cclk(host,ENABLE);
		}
		else
		{
			hi_sdio_control_cclk(host,DISABLE);
		}
		
		msleep(20);
	}

    hisdio_trace(1,"ios->cclk_low_power = %d,g_cclok_low_power = %d ",ios->cclk_low_power,g_cclok_low_power);  	
    if (ios->cclk_low_power != g_cclok_low_power)  
    {
        if (MMC_CCLK_LOW_POWER_MODE == ios->cclk_low_power)
        {
            hi_sdio_control_cclk_low_power(host,ENABLE);
            g_cclok_low_power = MMC_CCLK_LOW_POWER_MODE;
        }
        else
        {
            hi_sdio_control_cclk_low_power(host,DISABLE);
            g_cclok_low_power = MMC_CCLK_LOW_POWER_MODE;
        }        
    }
	
	BSP_PWRCTRL_SD_LowPowerEnter();
    
	hisdio_trace(1,"end");
}

/*******************************************************************************
  Function:      static void hi_sdio_set_ios(struct mmc_host* mmc, struct mmc_ios* ios)
  Description:   sdio request handle
  Input:         mmc:mmc_host struct
                 mrq:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static int hi_sdio_get_ro(struct mmc_host *mmc)
{
	unsigned ret;

	hisdio_trace(1,"begin");
	ret = sys_ctrl_hisdio_card_readonly_io();
	hisdio_trace(1,"end");
	return ret; /*lint !e713*/
}

/*******************************************************************************
  Function:      static void hisdio_enable_sdio_irq(struct mmc_host *host, int enable)
  Description:   sdio interrupt ctl
  Input:         mmc:mmc_host struct
                 enable:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void hisdio_enable_sdio_irq(struct mmc_host *host, int enable)
{
	struct hisdio_host *hi_host = mmc_priv(host);
	unsigned int reg_value;
    
    hisdio_trace(1,"begin");
	
	BSP_PWRCTRL_SD_LowPowerExit();
#if 0    
    if(g_suspend)
    {
    	hisdio_trace(5,"sdio is not resume!");
    	return;
    }
#endif
    
    if (enable)
	{
		reg_value = readl(hi_host->base + MMC_RINTSTS);
		reg_value &= 0x10000;
		writel(reg_value, hi_host->base + MMC_RINTSTS);

		reg_value = readl(hi_host->base + MMC_INTMASK);
		reg_value |= 0x10000;
		writel(reg_value, hi_host->base + MMC_INTMASK);
		g_intrkmode = 1;
	}
	else
	{
		reg_value = readl(hi_host->base + MMC_INTMASK);
		reg_value &= ~0xffff0000;
		writel(reg_value, hi_host->base + MMC_INTMASK);
		g_intrkmode = 0;
	}
	
	BSP_PWRCTRL_SD_LowPowerEnter();
	hisdio_trace(1,"end");
}/*lint !e529*/

/*******************************************************************************
  Function:      static int hi_sdio_get_cd(struct mmc_host *mmc)
  Description:   get cart status
  Input:         NA
                 NA
  Output:        NA
  Return:        1 insert 0 out 
  Others:        NA
*******************************************************************************/
#if defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
static int hi_sdio_get_cd(struct mmc_host *mmc)
{
	struct hisdio_host *host = mmc_priv(mmc);
    
    
	u32 ulSlotStatus;
    /* modifid for lint 732*/
	BSP_S32 ret;
    
    unsigned int ucData = 0;//pclint 734
	
#if (FEATURE_ON == HUAWEI_PRODUCT_E5372S_32)
	static int iPollTime = 0;
    if(iPollTime > SD_STATE_POLL_TIME)
    {
        if(sdio_getcd)
        {            
        	return 1;
        }
    	else
    	{
    		return 0;
    	}
    }

    iPollTime++;
#endif

    hisdio_trace(1,"begin"); 
	if (host->mmc->caps & MMC_CAP_NEEDS_POLL)
	{
		BSP_PWRCTRL_SD_LowPowerExit();
		/*给卡上电*/   
		BSP_PMU_LDOCtrl(LDO10, BSP_TRUE);	
		
		BSP_PMU_LDOCtrl(LDO7, BSP_TRUE);	
		ret = sd_mmc_get_status();
		if(!ret)//上次卡在位
		{
			/*查询DAT3,判断卡是否在位*/
			ucData = is_sdcard_exist(host);/*lint !e732 */
			if (BSP_TRUE == ucData && (sdio_device_remove == 0)) 
			{
			
				/*检查SD卡状态*/
				/*
				err = mmc_send_status(host->mmc->card,&resp);
				if (err)
				{
					printk(KERN_ERR "(%s): mmc_send_status failed (%d)\n",
						   __func__, err);
									
					hisdio_trace(1,"1:SD_MMC_CARD_REMOVED\n"); 
					ulSlotStatus = SD_MMC_CARD_REMOVED;
				}
				*/
				
				hisdio_trace(1,"SD_MMC_CARD_NO_REMOVE\n"); 
				ulSlotStatus = SD_MMC_CARD_NO_REMOVE;
			}
			else
			{
			
				hisdio_trace(1,"SD_MMC_CARD_REMOVED\n"); 
				ulSlotStatus = SD_MMC_CARD_REMOVED;
				sdio_device_remove = 0;
			}
		}
		else//上次卡不在位
		{
			hi_sdio_init_card(sdio_host_copy);
			
			/*查询DAT3,判断卡是否在位*/
			ucData = is_sdcard_exist(host);/*lint !e732 */
			if (BSP_TRUE == ucData)   
			{
			
				hisdio_trace(1,"SD_MMC_CARD_INSERTED\n"); 
				ulSlotStatus = SD_MMC_CARD_INSERTED;
			}
			else
			{
				hisdio_trace(1,"SD_MMC_CARD_NO_INSERT\n"); 
				ulSlotStatus = SD_MMC_CARD_NO_INSERT;
			}
		}
		
		switch (ulSlotStatus)
		{
		case SD_MMC_CARD_NO_REMOVE:
			break;
		case SD_MMC_CARD_NO_INSERT:
			hi_sdio_detectcard_to_core(0);
			break;
		case SD_MMC_CARD_REMOVED:
			hi_sdio_detectcard_to_core(0);			
			break;		
		case SD_MMC_CARD_INSERTED:
			/*设置卡是否在位标志并初始化卡*/
			hi_sdio_detectcard_to_core(1);
			/*配置SD IO电压为3.3V*/
			hi_sdio_control_uhs_ddr_volt(host->mmc,~DDR_MODE_CHOICE,~VOLT_18V_CHOICE); 
			hisdio_trace(1,"pmu LDO7 switch 3.0V begin **********\n");
			BSP_PMU_LDOVoltSet(LDO7,300);
			hisdio_trace(1,"pmu LDO7 switch 3.0V end **********\n");			
			break;
		default:
			break;
		}
		BSP_PWRCTRL_SD_LowPowerEnter();

	}
	
    if(sdio_getcd)
    {
        hisdio_trace(1, "card insert");
    	return 1;
    }
	else
	{
    	hisdio_trace(1, "card out");
		return 0;
	}
	//hisdio_trace(1,"end");

}



#else
static int hi_sdio_get_cd(struct mmc_host *mmc)
{
    hisdio_trace(1,"begin"); 
    if(sdio_getcd)
    {
        hisdio_trace(1, "card insert");
    	return 1;
    }
	else
	{
    	hisdio_trace(1, "card out");
		return 0;
	}
	hisdio_trace(1,"end");

}
#endif

#ifdef BALONG_SDCARD_DEBUG_FS
#define BALONG_DEBUG_BIN_MAX	256
static int balong_convert = 0;
static ssize_t sd_export_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size);
static int  check_register_region(unsigned int offset);
static void convert_to_binary(char* to , unsigned int source);

static ssize_t sd_unexport_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
//	ssize_t			status;
//	struct mmc_host *mmc  = platform_get_drvdata(pdev);
//	struct hisdio_host *host = mmc_priv(mmc);
//	return status ? : size;
	return 0;
}

static ssize_t sd_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t status = 0;
	int i = 0x0;
	unsigned int reg;
	char char_t[BALONG_DEBUG_BIN_MAX]={0};
	struct mmc_host *mmc  = dev_get_drvdata(dev);
	struct hisdio_host *host = mmc_priv(mmc);
	for(i = 0; i < 0x99; i=i+4)
	{
        if(i!=0 && (status + status/i)>4096)
        {
            hisdio_trace(1, "out of show buf to i : 0x%x", i);
    	    break;
        }	    
		reg = readl(host->base + i);
		if(balong_convert)
		{	
		    memset(char_t, 0, BALONG_DEBUG_BIN_MAX);
			convert_to_binary(char_t, reg);
			status += sprintf(buf+status, "reg:0x%2x val: 0x%8x bin:%s\n",i,reg, char_t);
			//hisdio_trace(1, "0x%x , status: %d", i, status);
		}
		else
		{
			status += sprintf(buf+status, "0x%3x:0x%8x\n",i,reg);
			//hisdio_trace(1, "0x%x , status: %d", i, status);
		}
	}
	return status;
}

static ssize_t sd_convert_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t			status;
	long convert_t;
//	struct mmc_host *mmc  = dev_get_drvdata(dev);
//	struct hisdio_host *host = mmc_priv(mmc);
	status = strict_strtol(buf, 0, &convert_t);
	if (status < 0)
		return size;
	balong_convert = !!convert_t ;
	return status ? : size;
}

static const DEVICE_ATTR(export, 0644,
		NULL, sd_export_store);
static const DEVICE_ATTR(unexport, 0644,
		NULL, sd_unexport_store);
static const DEVICE_ATTR(value, 0644,
		sd_value_show, NULL);
static const DEVICE_ATTR(convert, 0644,
		NULL, sd_convert_store);

static const struct attribute *sd_attrs[] = {
	&dev_attr_export.attr,
	&dev_attr_unexport.attr,
	&dev_attr_value.attr,
	&dev_attr_convert.attr,
	NULL,
};

static const struct attribute_group sd_attr_group = {
	.attrs = (struct attribute **) sd_attrs,
};

static int  check_register_region(unsigned int offset)
{
	if(offset < 0x0 || offset > 0x100)
		return false;
	return true;
}

static int check_bit_region(unsigned int bit)
{
	if(bit < 0  || bit > 31)
		return false;
	return true;
}

/*just for show*/
static void convert_to_binary(char* to , unsigned int source)
{
	int i = 31;
	int j =0;
	char bit;
	for(i = 31,j=0; i  >  -1; i--,j++ )
	{
		bit = source&(1 << i)? '1':'0'; 
		if(i%4 == 3)
		{
			j = j + sprintf(to +j, " [%d:%d]",i,i-3);
		}
		to[j] = bit;
	}
}

static ssize_t sd_reg_set_bit_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t			status;
	unsigned int offset;
	long bit;
	const char* name = dev_name(dev);
	unsigned long reg;
	struct mmc_host *mmc  = dev_get_drvdata(dev);
	struct hisdio_host *host = mmc_priv(mmc);
	status = strict_strtol(buf, 0, &bit);//set bit
	if (status < 0)
		return size;
	if(!check_bit_region(bit))
		return size;
	sscanf(name, "%x", &offset);//get host base offset->register
	hisdio_trace(2, "set_bit name: %s, host->base:%p, offset:%x, bit:%ld", 
			name, host->base,offset , bit);
	if(!check_register_region(offset))
		return size;
	reg = readl(host->base + offset);
	reg |=(1<<bit);
	writel(reg, host->base+offset);
	return status ? : size;
}

static ssize_t sd_reg_clear_bit_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t			status;
	unsigned int offset;
	long bit;
	const char* name = dev_name(dev);
	unsigned int reg;
	struct mmc_host *mmc  = dev_get_drvdata(dev);
	struct hisdio_host *host = mmc_priv(mmc);
	status = strict_strtol(buf, 0, &bit);//set bit
	if (status < 0)
		return size;
	if(!check_bit_region(bit))
		return size;
	sscanf(name, "%x", &offset);//get host base offset->register
	hisdio_trace(1, "set_bit name: %s, host->base:%p, offset:%x, bit:%ld", 
				name, host->base,offset , bit);	
	if(!check_register_region(offset))
		return -EINVAL;
	reg = readl(host->base + offset);
	reg &=~(1<<bit);
	writel(reg, host->base+offset);
	return status ? : size;
}



static ssize_t sd_reg_value_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t			status;
	unsigned int offset;
	const char* name = dev_name(dev);
	unsigned int reg;
	char char_t[BALONG_DEBUG_BIN_MAX]={0};
	struct mmc_host *mmc  = dev_get_drvdata(dev);
	struct hisdio_host *host = mmc_priv(mmc);
	sscanf(name, "%x", &offset);//get host base offset->register
	//if(!check_register_region(offset))
	//	return -EINVAL;
	//status = strict_strtol(buf, 0, &offset);//offset value
	reg = readl(host->base + offset);
	if(balong_convert)
	{	
	    memset(char_t, 0, BALONG_DEBUG_BIN_MAX);
		convert_to_binary(char_t, reg);
		status = sprintf(buf, "0x%x:%s\n", reg,char_t);
	}
	else
	{
		status = sprintf(buf, "0x%x\n",reg);
	}
	hisdio_trace(2, "set_bit name: %s, host->base:0x%p, offset:0x%x, reg:0x%x, status:%d", 
			name, host->base,offset , reg, status);
	
	return status;
}

static ssize_t sd_reg_value_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t			status;
	unsigned int offset;
	long reg;
	const char* name = dev_name(dev);
	struct mmc_host *mmc  = dev_get_drvdata(dev);
	struct hisdio_host *host = mmc_priv(mmc);
	status = strict_strtol(buf, 0, &reg);//set bit
	if (status < 0)
		return size;
	sscanf(name, "%x", &offset);//get host base offset->register
	//if(!check_register_region(offset))
	//	return size;
	hisdio_trace(2, "set_bit name: %s, host->base:%p, offset:%x, reg:%ld", 
			name, host->base,offset , reg);
	writel(reg, host->base+offset);
	return status ? : size;
}
static const DEVICE_ATTR(set_bit, 0644,
		NULL, sd_reg_set_bit_store);
static const DEVICE_ATTR(clear_bit, 0644,
		NULL, sd_reg_clear_bit_store);
static const struct device_attribute dev_attr_reg_value =
				__ATTR(value, 0644, sd_reg_value_show, sd_reg_value_store);

static const struct attribute *sd_reg_attrs[] = {
	&dev_attr_set_bit.attr,
	&dev_attr_clear_bit.attr,
	&dev_attr_reg_value.attr,
	NULL,
};

static const struct attribute_group sd_reg_attr_group = {
	.attrs = (struct attribute **) sd_reg_attrs,
};

static struct class sd_debug_class =
{
    .name = "sd_host",
};

static ssize_t sd_export_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t size)
{
	ssize_t			status = 0;
	unsigned int offset;
	struct device	*dev_t;
	struct mmc_host *mmc  = dev_get_drvdata(dev);
//	struct hisdio_host *host = mmc_priv(mmc);
	//status = strict_strtol(buf, 0, &offset);
	sscanf(buf, "%x", &offset);
	hisdio_trace(1, "export value : %x ", offset);
	dev_t = device_create(&sd_debug_class, dev, MKDEV(0, 0),
				mmc, "%x", offset);
	if (!IS_ERR(dev_t)) {
		sysfs_create_group(&dev_t->kobj, &sd_reg_attr_group);
	}
	return status ? : size;
}
#endif

static const struct mmc_host_ops hi_sdio_ops = {
	.request	              = hi_sdio_request,
	.set_ios	              = hi_sdio_set_ios,          /*电源、时钟及总线宽度控制*/  
	.get_ro		              = hi_sdio_get_ro,
	.enable_sdio_irq	      = hisdio_enable_sdio_irq,
	.get_cd                   = hi_sdio_get_cd,           /*卡存在*/
};

struct hisdio_host  *g_phost = NULL;/*l00205892*/
static int __devinit hi_sdio_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc;
	struct hisdio_host *host = NULL;
	struct resource *res;
	int ret,irq;
	int ucData = 0; 
	unsigned short ucHwVer = 0;
    
	hisdio_trace(1,"begin");
	
#if (defined (BOARD_ASIC) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	ucHwVer = BSP_HwGetVerMain();
    printk(KERN_INFO"hi_sdio_probe:ucHwVer=%#x\n",ucHwVer);
#endif

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);
	
	if(!res || irq < 0)
	{
		return  -ENXIO;
	}
	
	res = request_mem_region(res->start, SZ_4K, DRIVER_NAME);
	if (!res)
	{
		hisdio_trace(5,"request_mem_region is error!");
		return -EBUSY;
	}
	
	mmc = mmc_alloc_host(sizeof(struct hisdio_host), &pdev->dev);
	if (!mmc) 
	{
		hisdio_trace(5,"no mem for hi mci host controller!");
		ret = -ENOMEM;
		goto out;
	}

	mmc->ops = &hi_sdio_ops;
#if (defined (BOARD_ASIC)\
	 && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	mmc->f_min = MMC1_CCLK_MIN;
	mmc->f_max = MMC1_CCLK_MAX;
#else
	mmc->f_min = MMC0_CCLK_MIN;
	mmc->f_max = MMC0_CCLK_MAX;
#endif

    mmc->caps |= MMC_CAP_4_BIT_DATA | MMC_CAP_SDIO_IRQ | MMC_CAP_SD_HIGHSPEED;

#if (defined (BOARD_ASIC) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    /*BEGIN DST2012092001529 liangshukun 20121025 modified*/
	if ((HW_VER_PRODUCT_UDP == ucHwVer) || \
		(HW_VER_PRODUCT_E5776_EM == ucHwVer) || \
		(HW_VER_PRODUCT_E5_SBM == ucHwVer) || \
		(HW_VER_PRODUCT_E5_CMCC == ucHwVer)|| \
		(HW_VER_PRODUCT_E5_CMCC_CY == ucHwVer) || \
		(HW_VER_PRODUCT_E5371_DCM == ucHwVer ) || \
		(HW_VER_PRODUCT_E5375 == ucHwVer ) || \
        (HW_VER_PRODUCT_E5375_SEC == ucHwVer) || \
        (HW_VER_PRODUCT_E5775S_925 == ucHwVer))
    /*END DST2012092001529 liangshukun 20121025 modified*/
	{
		//mmc->caps |= MMC_CAP_NEEDS_POLL;//上库时删掉本行
	}
	else if ((HW_VER_PRODUCT_E392S_U == ucHwVer) || \
			(HW_VER_PRODUCT_E5372_32 == ucHwVer) || \
             (HW_VER_PRODUCT_E3276S_150 == ucHwVer) ||\
             (HW_VER_PRODUCT_E5372_601 == ucHwVer) || \
             (HW_VER_PRODUCT_E5372TS_32 == ucHwVer) || \
             (HW_VER_PRODUCT_E5372S_22 == ucHwVer) || \
			 (HW_VER_PRODUCT_R215 == ucHwVer))
	{
		mmc->caps |= MMC_CAP_NEEDS_POLL;
	}
	else	
	{
		mmc->caps |= MMC_CAP_NEEDS_POLL;
	}
#endif

	/* reload by this controller */
	mmc->max_blk_count    = 512;
	
	/*Sets an upper limit of the maximum number of
	hw data segments in a request (i.e. the maximum number of address/length
	pairs the host adapter can actually hand to the device at once)*/
	mmc->max_hw_segs      = 1024;
	//mmc->max_hw_segs      = IDMAC_DESC_SIZE;
	/*Sets an upper limit on the maximum number
	of physical data segments in a request (i.e. the largest sized scatter list
	a driver could handle)*/
	mmc->max_phys_segs    = 1024;
	mmc->max_phys_segs    = mmc->max_hw_segs;
	mmc->max_req_size     = 65536 * 2;//FIXME? 65535 or 65536?  /*y00186965 */
	/*Maximum size of a clustered segment, 64kB default.*/
	//mmc->max_seg_size = IDMAC_BUF_MAXSIZE; 
	hisdio_trace(2, "[max_seg_size: %d], [max_hw_segs: %d], [max_phy_segs:  %d], [max_req_size:  %d], [max_blk_size:  %d], [max_blk_count:  %d]", 
			mmc->max_seg_size, mmc->max_hw_segs, mmc->max_phys_segs, mmc->max_req_size, mmc->max_blk_size, mmc->max_blk_count);
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_34_35 | MMC_VDD_35_36; 
	mmc->ocr = mmc->ocr_avail;

	host = mmc_priv(mmc);

	host->mmc = mmc;
	host->irq = irq;
	host->base = ioremap_nocache(res->start, HI_MCI_IO_SIZE);

	g_sysctrl_base = ioremap_nocache(SCTL_BASE,SCTL_SC_SIZE);

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
		#else
		    if (gpio_request(BALONG_GPIO_DETECT_BOARD_TYPE, "board_type_detect"))
			{
		        hisdio_trace(3,"GPIO is busy!!! **********\n");  
			}
			else
			{
		        ucData = gpio_get_value(BALONG_GPIO_DETECT_BOARD_TYPE);
				if (ucData)
				{
		            g_product_flag = 1; /*UDP*/
					hisdio_trace(1,"UDP Board!!! **********\n");
				}
				else
				{
		            g_product_flag = 0; /*es*/
					hisdio_trace(1,"stick Board!!! **********\n");
				}
				gpio_free(BALONG_GPIO_DETECT_BOARD_TYPE);
			}
			hisdio_trace(1,"pmu LDO7 open begin **********\n");
			stSDIFCMsg = kzalloc(sizeof(HISDIO_IFC_MSG_STRU), GFP_KERNEL);
			if (NULL == stSDIFCMsg)
			{
			     hisdio_trace(5,"no mem for stSDIFCMsg!");
			     goto out;
			}
			g_stSDLDO10Arg = kzalloc(sizeof(HISDIO_IDOCTRL_ARG), GFP_KERNEL);
			if (NULL == g_stSDLDO10Arg)
			{
			     hisdio_trace(5,"no mem for g_stSDLDO10Arg!");
			     goto out;
			}
		#endif
	#else
    if (gpio_request(BALONG_GPIO_DETECT_BOARD_TYPE, "board_type_detect"))
	{
        hisdio_trace(3,"GPIO is busy!!! **********\n");  
	}
	else
	{
        ucData = gpio_get_value(BALONG_GPIO_DETECT_BOARD_TYPE);
		if (ucData)
		{
            g_product_flag = 1; /*UDP*/
			hisdio_trace(1,"UDP Board!!! **********\n");
		}
		else
		{
            g_product_flag = 0; /*es*/
			hisdio_trace(1,"stick Board!!! **********\n");
		}
		gpio_free(BALONG_GPIO_DETECT_BOARD_TYPE);
	}
	hisdio_trace(1,"pmu LDO7 open begin **********\n");
	stSDIFCMsg = kzalloc(sizeof(HISDIO_IFC_MSG_STRU), GFP_KERNEL);
	if (NULL == stSDIFCMsg)
	{
	     hisdio_trace(5,"no mem for stSDIFCMsg!");
	     goto out;
	}
	g_stSDLDO10Arg = kzalloc(sizeof(HISDIO_IDOCTRL_ARG), GFP_KERNEL);
	if (NULL == g_stSDLDO10Arg)
	{
	     hisdio_trace(5,"no mem for g_stSDLDO10Arg!");
	     goto out;
	}

#if 0

	g_stSDLDO10Arg->LDO_id = LDO7;  
	g_stSDLDO10Arg->is_voltage_switch = VOLT_SWTICH_NO;
	g_stSDLDO10Arg->open_close = OPEN_LDO;
	g_stSDLDO10Arg->voltage_val = LDO10_3_0V;	
	
	//msleep(1000);/*test for 双核通信*/


	
	hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);	
	msleep(50);
	hisdio_trace(1,"pmu LDO7 open end **********\n");

	hisdio_trace(1,"pmu LDO10 open begin **********\n");
	g_stSDLDO10Arg->LDO_id = LDO10;
	hi_sdio_send_switch_to_pmu(g_stSDLDO10Arg);
	msleep(50);
	hisdio_trace(1,"pmu LDO10 open end **********\n");

	 /*将GPIO_1_21~26复用为MMC1(SD3.0)*/
	(*((volatile BSP_U32 *)(g_sysctrl_base + 0x988)) |= (1UL << 22));
	(*((volatile BSP_U32 *)(g_sysctrl_base + 0x988)) &= ~(1UL << 1));
	
#endif
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
	if ((HW_VER_PRODUCT_UDP == ucHwVer) || \
		(HW_VER_PRODUCT_E5776_EM == ucHwVer) || \
		(HW_VER_PRODUCT_E5_SBM == ucHwVer) || \
		(HW_VER_PRODUCT_E5_CMCC == ucHwVer) || \
        (HW_VER_PRODUCT_E5_CMCC_CY == ucHwVer) || \
		(HW_VER_PRODUCT_E5371_DCM == ucHwVer ) || \
		(HW_VER_PRODUCT_E5375 == ucHwVer) || \
        (HW_VER_PRODUCT_E5375_SEC == ucHwVer) || \
        (HW_VER_PRODUCT_E5775S_925 == ucHwVer))
	{
	    g_product_flag = 1;
		hisdio_trace(5,"GPIO card detect!!! **********\n");
	}
	else if ((HW_VER_PRODUCT_E392S_U == ucHwVer) || \
         	(HW_VER_PRODUCT_E5372_32 == ucHwVer) || \
             (HW_VER_PRODUCT_E3276S_150 == ucHwVer) || \
             (HW_VER_PRODUCT_E5372_601 == ucHwVer) ||\
             (HW_VER_PRODUCT_E5372TS_32 == ucHwVer) || \
             (HW_VER_PRODUCT_E5372S_22 == ucHwVer) || \
			 (HW_VER_PRODUCT_R215 == ucHwVer))
	{
	    g_product_flag = 0; 
		hisdio_trace(5,"DAT3 card detect!!! **********\n");
	}
	else	
	{
	    g_product_flag = 0; 
		hisdio_trace(5,"DAT3 card detect!!! **********\n");
	}
#endif
	/*SD卡与系统控制器相关的配置*/
	sys_ctrl_hisdio_init_io();	  
	
	if(!host->base)
	{
		hisdio_trace(5,"no mem for himci base!");
		ret = -ENOMEM;
		goto out;
	}

	spin_lock_init(&host->lock);

#if !(BALONG_SDCARD_DMA)
	if(1)
	{
		hisdio_trace(2,"dmac_channel_allocate");
		ret = SD_IDMAC_Init(host->base);
		if(ret)
		{
			hisdio_trace(5,"internal dmac init failed!");
			goto out;
		}
	}
	else
	{
		hisdio_trace(2,"dma is zero");
		dma = 0;
	}
#else
    EDMA_free_list_node = (BALONG_DMA_CB *)dma_alloc_coherent(NULL,(EDMA_FREE_LIST_NODE_NUM * sizeof(BALONG_DMA_CB)),
                                                                  &edma_phy_addr, GFP_DMA|__GFP_WAIT);
    if (NULL == EDMA_free_list_node)
    {		
        hisdio_trace(3,"LII list init is failed!"); 
    	return DMA_MEMORY_ALLOCATE_ERROR;
    }
    hisdio_trace(3,"LII list virt_address = 0x%X,phy_addr = 0x%X!\n",(UINT32)EDMA_free_list_node,(UINT32)edma_phy_addr); 
#endif
	
	host->card_status = sys_ctrl_hisdio_card_detect_io();
	host->fifo_depth = ((readl(host->base + MMC_FIFOTH) & 0x0fff0000) >> 16) + 1;/*lint !e572*/
	sdio_host_copy=(struct hisdio_host *)host;
    g_phost = host;/*l00205892*/
	hi_sdio_init_card(host);

	/*tasklet_init(&host->detect_tasklet,sd_detect_tasklet_handler,(unsigned long)host);*/
	if (g_product_flag)  /*es_e5 poll y00186965*/
    {
		INIT_WORK(&host->sd_detect_work, (void *)sd_detect_handler);
		host->detect_queue = create_workqueue(DRIVER_NAME);
		if (NULL == host->detect_queue) 
		{
			hisdio_trace(5,"failed to create workqueue host->detect_queue");
			goto out;
		}
	}
	
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) || (defined(BOARD_SFT) && defined(VERSION_V7R1)) /*y00186965 for sd_update*/
    #if (FEATURE_SDUPDATE == FEATURE_ON) && (FEATURE_E5 == FEATURE_ON)
	    INIT_WORK(&host->sdup_detect_work, (void *)hi_TFUP_CheckTask);
		host->sdupdate_queue = create_workqueue("keybard_sdupdate");
	#endif
#elif ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)) \
        && (defined(BOARD_ASIC) || defined(BOARD_SFT)))
    #if ((FEATURE_SDUPDATE == FEATURE_ON) && (FEATURE_E5 == FEATURE_ON))
	INIT_WORK(&host->sdup_detect_work, (void *)hi_TFUP_CheckTask);
	host->sdupdate_queue = create_workqueue("keybard_sdupdate");
	if (NULL == host->sdupdate_queue) 
	{
		hisdio_trace(5,"failed to create workqueue host->sdupdate_queue");
		goto out;
	}
	#endif
#endif

	/*挂接SD卡中断*/	
	ret = request_irq(host->irq, hisdio_irq, 0, DRIVER_NAME, host);/*lint !e732 */
	if (ret)
	{
		hisdio_trace(5,"request sdio irq error!");
		goto out;
	}
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) 
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		/*挂接SD卡busy完成查询中断*/
		ret = request_irq(INT_SCTL,hi_sd_busy_detect_irq_cd,IRQF_SHARED	,DETECT_SD_BUSY_CHECK_NAME, host);
		if (ret)
		{
			hisdio_trace(5,"request sdio busy check irq error!");
			goto out;
		}
	#endif
	/*--by pandong cs*/
    /*V3R2	ES	不支持busy中断查询*/
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))   
    //6920es不支持busy中断    
#else
	/*挂接SD卡busy完成查询中断*/
	ret = request_irq(INT_SCTL,hi_sd_busy_detect_irq_cd,IRQF_SHARED	,DETECT_SD_BUSY_CHECK_NAME, host);
	if (ret)
	{
		hisdio_trace(5,"request sdio busy check irq error!");
		goto out;
	}
#endif
/*p00196546*/
#ifdef CONFIG_SMALL_IMAGE
	if (BSP_FALSE == is_sdcard_exist(host))
	{
		hisdio_trace(6,"not start");
		goto END;
	}
#endif
if (g_product_flag)  /*es_e5 poll y00186965*/
{
	if(gpio_request(BALONG_GPIO_DETECT, "sd-detect"))
	{
		hisdio_trace(5,"request gpio is busy!");
		goto out;
	}
	/*gic did not implement this function*/
    /*disable_irq(INT_GPIO_2);*/
    spin_lock(&host->lock);
	
    /*判断SD卡是否在位,从而设置触发条件*/    
	ucData = gpio_get_value(BALONG_GPIO_DETECT);/*卡在位低电平*/
    if (ucData)
    {
        /*卡拔出GPIO为高电平*/        
		gpio_int_trigger_set(BALONG_GPIO_DETECT,IRQ_TYPE_LEVEL_LOW);
		sdio_getcd = 0;
    }
    else
    {       
        gpio_int_trigger_set(BALONG_GPIO_DETECT,IRQ_TYPE_LEVEL_HIGH);
		sdio_getcd = 1;
    }

    /*挂接SD卡检测中断*/       
	gpio_direction_input(BALONG_GPIO_DETECT);     
	gpio_int_unmask_set(BALONG_GPIO_DETECT);
    gpio_int_state_clear(BALONG_GPIO_DETECT);
    gpio_set_function(BALONG_GPIO_DETECT,GPIO_INTERRUPT);
	
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
    if (request_irq(INT_GPIO_2, hi_sd_detect_irq_cd,IRQF_SHARED	,DETECT_NAME, host)) 
		#else
    if (request_irq(INT_GPIO_9, hi_sd_detect_irq_cd,IRQF_SHARED	,DETECT_NAME, host)) 
		#endif
	#else
    if (request_irq(INT_GPIO_0, hi_sd_detect_irq_cd,IRQF_SHARED	,DETECT_NAME, host)) 
	#endif
	/*--by pandong cs*/
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920ES))
	if (request_irq(INT_GPIO_0, hi_sd_detect_irq_cd,IRQF_SHARED ,DETECT_NAME, host)) 
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920CS))
	if (request_irq(INT_GPIO_2, hi_sd_detect_irq_cd,IRQF_SHARED ,DETECT_NAME, host)) 		
#else
    if (request_irq(INT_GPIO_2, hi_sd_detect_irq_cd,IRQF_SHARED	,DETECT_NAME, host)) 
#endif
    {
        spin_unlock(&host->lock);
        hisdio_trace(5,"can't get card detect irq.\n");
        goto out;
    }
    
    /*gic did not implement this function*/
    /*enable_irq(INT_GPIO_2);*/
    spin_unlock(&host->lock);
}
else
{
    sdio_getcd = 1;
}

//#if 0
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
		    if(gpio_request(BALONG_GPIO_VOL, "sd-vol"))
			{
				hisdio_trace(5,"request gpio is busy!");
				goto out;
			}
			/*配置SD卡IO供电电压为3V，默认为1.8V*/
			gpio_direction_output(BALONG_GPIO_VOL, 1);
		#else
		#endif
	#else
	#endif
	/*--by pandong cs*/
#elif(defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#else
    if(gpio_request(BALONG_GPIO_VOL, "sd-vol"))
	{
		hisdio_trace(5,"request gpio is busy!");
		goto out;
	}
	/*配置SD卡IO供电电压为3V，默认为1.8V*/
	gpio_direction_output(BALONG_GPIO_VOL, 1);
#endif
//#endif

	platform_set_drvdata(pdev, mmc);
	if(0 == sdio_getcd)
	{
		BSP_PMU_LDOCtrl(LDO10, BSP_FALSE);
		BSP_PMU_LDOVoltSet(LDO10,300);
		BSP_PMU_LDOCtrl(LDO7, BSP_FALSE);
		BSP_PMU_LDOVoltSet(LDO7,300);

	}
	mmc_add_host(mmc);
#ifdef BALONG_SDCARD_DEBUG_FS
    	class_register(&sd_debug_class);
	sysfs_create_group(&pdev->dev.kobj,
						&sd_attr_group);
#endif
if ((HW_VER_PRODUCT_E5372_32 == ucHwVer) || (HW_VER_PRODUCT_E5372_601 == ucHwVer)||(HW_VER_PRODUCT_E5372TS_32 == ucHwVer)\
    || (HW_VER_PRODUCT_E5372S_22 == ucHwVer)|| (HW_VER_PRODUCT_R215 == ucHwVer))
{
    mmc_detect_change(host->mmc, 0);
}
	hisdio_trace(1,"end");

/*modified for lint e563 */
#ifdef CONFIG_SMALL_IMAGE
END:
#endif	
/*modified for lint e563 */

	return 0;	
out:
	if(host)
	{
		if(1)
		{
#if 0
			dmac_channel_free(host->dma_channel);
#else
#if !(BALONG_SDCARD_DMA)
			SD_IDMAC_Exit(host->base);
#endif
#endif
		}
		if(host->base)
		{
			iounmap(host->base);
		}
	}
	if(mmc)
	{
		mmc_free_host(mmc);
	}
	return ret;
}

/*************************************************************************
*函数功能: 按键响应
*
*
*l00205892
************************************************************************/
 int hi_keyboard_respond(void)
{
    int ret;

    if(g_phost != NULL)
    {
    #if (FEATURE_SDUPDATE == FEATURE_ON) && (FEATURE_E5 == FEATURE_ON)
        ret = queue_work(g_phost->sdupdate_queue, &g_phost->sdup_detect_work);
	    if(0 == ret)
	    {
	        hisdio_trace(5,"queue_work failure!\n");
		    return OSAL_ERROR;
	    }
    #endif
        return OSAL_OK;
    }
    else
    {
        hisdio_trace(5,"host is invalid point!\n");
   	    return OSAL_ERROR;
   	}
}

//add remove function
static int __devexit hi_sdio_remove(struct platform_device *pdev)
{
	struct resource  *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	struct mmc_host *mmc  = platform_get_drvdata(pdev);
	struct hisdio_host *host = mmc_priv(mmc);
	hi_sdio_shutdown(pdev);
	if (g_product_flag)  /*es_e5 poll y00186965*/
	{
	    destroy_workqueue(host->detect_queue);
	}
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	free_irq((unsigned int)(host->irq), host);	
	/*end*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
			free_irq(INT_SCTL,host);
		    free_irq(INT_GPIO_2,host);
		#else
			if (g_product_flag)  /*es_e5 poll y00186965*/
			{
			    free_irq(INT_GPIO_9,host);  
			}
			kfree(stSDIFCMsg);
			kfree(g_stSDLDO10Arg);
		#endif
	#else
		if (g_product_flag)  /*es_e5 poll y00186965*/
		{
		    free_irq(INT_GPIO_0,host);  
		}
		kfree(stSDIFCMsg);
		kfree(g_stSDLDO10Arg);
	#endif
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920ES))
    if (g_product_flag) 
    {
        free_irq(INT_GPIO_0,host);  
    }
	
	#if (FEATURE_SDUPDATE == FEATURE_ON) && (FEATURE_E5 == FEATURE_ON) /*y00186965 for sd_update*/
    destroy_workqueue(host->sdupdate_queue); 
	#endif	
#elif (defined (BOARD_ASIC) && defined(CHIP_BB_6920CS))
	if (g_product_flag) 
	{
		free_irq(INT_GPIO_2,host);
	}
	
	#if (FEATURE_SDUPDATE == FEATURE_ON) && (FEATURE_E5 == FEATURE_ON) /*y00186965 for sd_update*/
    destroy_workqueue(host->sdupdate_queue); 
	#endif
#else
	free_irq(INT_SCTL,host);
    free_irq(INT_GPIO_2,host);
#endif

#if !(BALONG_SDCARD_DMA)
	SD_IDMAC_Exit(host->base);
#else
    dma_free_coherent(NULL,(EDMA_FREE_LIST_NODE_NUM * sizeof(BALONG_DMA_CB)),(void*)EDMA_free_list_node, edma_phy_addr);
    hisdio_trace(3,"LII list is released!");
#endif
	iounmap(host->base);
	release_mem_region(res->start, SZ_4K);
	gpio_free(BALONG_GPIO_DETECT);
	mmc_remove_host(mmc);
	mmc_free_host(mmc);
	return 0;
}

static int hi_sdio_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
    
    int ret = 0;
    
	hisdio_trace(5,"begin %d",sdio_getcd);
    
    hisdio_lock();
    
	if(1 == sdio_getcd)
	{   
		if (mmc)
		{
			//atomic_set(&mmc->sdio_irq_thread_abort , 0);
			//ret = mmc_suspend_host(mmc, state);
		}
		/*gic did not implement this function*/
	    /*disable_irq(sdio_host_copy->irq);*/
		g_suspend = 1;
		//sys_ctrl_hisdio_exit_io();

	}
	
	hisdio_trace(5,"end");
	return ret;
}


static int hi_sdio_resume(struct platform_device *dev)
{
	int ret = 0;
	unsigned int tmp_reg = 0;
	
	hisdio_trace(5,"begin %d",sdio_getcd);
	
	//queue_work(psdio_resume_workque,&sdio_resume_work_suct);
	
	
	if(1 == sdio_getcd)
	{   	
		wlan_clock_ctl(1);
		
		//sys_ctrl_hisdio_init_io();
		
		hi_sdio_init_card(sdio_host_copy);
    
	    if(MMC_BUS_WIDTH_4 == g_bitmode)
	    {
	    	tmp_reg = readl(sdio_host_copy->base + MMC_CTYPE);
			tmp_reg |= CARD_WIDTH;
			writel(tmp_reg,sdio_host_copy->base + MMC_CTYPE);	
			
	    }
	    else
	    {
	    	tmp_reg = readl(sdio_host_copy->base + MMC_CTYPE);
			tmp_reg &= ~CARD_WIDTH;
			writel(tmp_reg,sdio_host_copy->base + MMC_CTYPE);
	    }
	    
		if(g_clock)
		{
			hi_sdio_control_cclk(sdio_host_copy,DISABLE);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 2) (int to unsigned int))*/
			hi_sdio_set_cclk(sdio_host_copy,(unsigned int)g_clock);
			/*end*/
			hi_sdio_control_cclk(sdio_host_copy,ENABLE);
		
		}
		else
		{
			hi_sdio_control_cclk(sdio_host_copy,DISABLE);
		}
	    
	    msleep(20);
	    if(g_intrkmode)
	    {
			tmp_reg = readl(sdio_host_copy->base + MMC_INTMASK);
			tmp_reg |= 0x10000;
        	writel(tmp_reg, sdio_host_copy->base + MMC_INTMASK);
	    }
	    /*gic did not implement this function*/
	    /*enable_irq(sdio_host_copy->irq);*/
		
		if (sdio_host_copy->mmc)
		{
			//mmc_resume_host(sdio_host_copy->mmc);
		}
		g_suspend = 0;
	}
	
	hisdio_unlock(); 
	
	hisdio_trace(5,"end");
	return ret;
}


/*******************************************************************************
  Function:      static void sdio_resume_thread(void)
  Description:   sdio  resume thread  
  Input:         NA
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/

static void sdio_resume_thread(void)
{
    unsigned int tmp_reg = 0;
	
	if(1 == sdio_getcd)
	{   	
		wlan_clock_ctl(1);
		
		//sys_ctrl_hisdio_init_io();
		
		hi_sdio_init_card(sdio_host_copy);
    
	    if(MMC_BUS_WIDTH_4 == g_bitmode)
	    {
	    	tmp_reg = readl(sdio_host_copy->base + MMC_CTYPE);
			tmp_reg |= CARD_WIDTH;
			writel(tmp_reg,sdio_host_copy->base + MMC_CTYPE);	
			
	    }
	    else
	    {
	    	tmp_reg = readl(sdio_host_copy->base + MMC_CTYPE);
			tmp_reg &= ~CARD_WIDTH;
			writel(tmp_reg,sdio_host_copy->base + MMC_CTYPE);
	    }
	    
		if(g_clock)
		{
			hi_sdio_control_cclk(sdio_host_copy,DISABLE);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 2) (int to unsigned int))*/
			hi_sdio_set_cclk(sdio_host_copy,(unsigned int)g_clock);
			/*end*/
			hi_sdio_control_cclk(sdio_host_copy,ENABLE);
		
		}
		else
		{
			hi_sdio_control_cclk(sdio_host_copy,DISABLE);
		}
	    
	    if(g_intrkmode)
	    {
			tmp_reg = readl(sdio_host_copy->base + MMC_INTMASK);
			tmp_reg |= 0x10000;
        	writel(tmp_reg, sdio_host_copy->base + MMC_INTMASK);
	    }
	    
	    msleep(20);
	    /*gic did not implement this function*/
	    /*enable_irq(sdio_host_copy->irq);*/
		
		if (sdio_host_copy->mmc)
		{
			//mmc_resume_host(sdio_host_copy->mmc);
		}
		g_suspend = 0;
	}
	
	hisdio_unlock();
	return;
}/*lint +e550*/

static void hi_sdio_shutdown(struct platform_device *dev)
{	
     hisdio_trace(1,"begin");
     /*gic did not implement this function*/
     /*disable_irq(sdio_host_copy->irq);*/
     sys_ctrl_hisdio_exit_io();
	 hisdio_trace(1,"end");	
	 	
	 return;
}

static struct resource hi_sdio_io_resources[] = {
	[0] = {
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
	#if defined(CHIP_BB_6756CS)
        .start          = HI_MCI_BASE0,     
		.end            = HI_MCI_BASE0 + HI_MCI_IO_SIZE - 1,
	#else
        .start          = HI_MCI_BASE1,     
		.end            = HI_MCI_BASE1 + HI_MCI_IO_SIZE - 1,
	#endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

		.start			= HI_MCI_BASE1, 	
		.end			= HI_MCI_BASE1 + HI_MCI_IO_SIZE - 1,

#else
        .start          = HI_MCI_BASE0,     
		.end            = HI_MCI_BASE0 + HI_MCI_IO_SIZE - 1,
#endif
		.flags          = IORESOURCE_MEM,
	},
	[1] = {
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
	#if defined(CHIP_BB_6756CS)
		.start          = HI_MCI_INTR0,
		.end            = HI_MCI_INTR0,
	#else
		.start          = HI_MCI_INTR1,
		.end            = HI_MCI_INTR1,
	#endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
		.start			= HI_MCI_INTR1,
		.end			= HI_MCI_INTR1,
#else
		.start          = HI_MCI_INTR0,
		.end            = HI_MCI_INTR0,
#endif
		.flags          = IORESOURCE_IRQ,
	},

};


static void hi_sdio_platdev_release(struct device *dev)
{
}

static struct platform_device hi_sdio_device = {
	.name           = DRIVER_NAME,
	.id             = 1,
	.dev = {
		.release      = hi_sdio_platdev_release,
	},
    /*解决typeof参数类型不对的问题-- linux内核*/
	/*lint -e516*/
	.num_resources  = ARRAY_SIZE(hi_sdio_io_resources),/*lint !e30 !e84 !e806*/
	/*lint +e516*/
	.resource       = hi_sdio_io_resources,
};


static struct platform_driver hi_sdio_driver = {
	.probe           = hi_sdio_probe,
	.remove 	     =__devexit_p(hi_sdio_remove),
	.shutdown        = hi_sdio_shutdown,
	.suspend        = hi_sdio_suspend,
	.resume        = hi_sdio_resume,
	.driver        = 
	{
		.name          = DRIVER_NAME,
	},
};
int g_ShortOnoffMmc = 0;
struct work_struct short_work_mmc;

void BSP_Short_Onoff_MMC(struct work_struct *w)
{
	if(g_ShortOnoffMmc == 1)
	{
		gpio_int_chip_unmask_set(BALONG_GPIO_DETECT_CHIP, BALONG_GPIO_DETECT_PIN);
		mmc_start_host(sdio_host_copy->mmc);	
	}
	else if(g_ShortOnoffMmc == 2)
	{
		mmc_stop_host(sdio_host_copy->mmc);
		gpio_int_chip_mask_set(BALONG_GPIO_DETECT_CHIP, BALONG_GPIO_DETECT_PIN);
	}
	g_ShortOnoffMmc = 0;
		
}

void BSP_Short_On_MMC(void)
{
	g_ShortOnoffMmc = 1;
	sdio_host_copy->mmc->removed = 0; /*lint !e63*/
	schedule_work(&short_work_mmc);
}
EXPORT_SYMBOL(BSP_Short_On_MMC);


void BSP_Short_Off_MMC(void)
{
	g_ShortOnoffMmc = 2;
	/*如果卡在位,上报netlink */
	if(sdio_getcd == 1)
	{
    	report_sd_state(DEVICE_ID_SD, SD_REMOVE, "SD remove");  

	}
	schedule_work(&short_work_mmc);
}
EXPORT_SYMBOL(BSP_Short_Off_MMC);


void BSP_Short_Debug(int level)
{
	sdio_trace_level = level;
	printk("sdio_getcd = %d\n",sdio_getcd);
}
EXPORT_SYMBOL(BSP_Short_Debug);

static int __init hi_sdio_init(void)
{
	int ret = 0;	

  
	hisdio_trace(1,"begin"); 
/*++by pandong cs*/
#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	#if defined(CS_SFT)
	#else
	g_iomg_base_sd= ioremap_nocache(IOMG_BASE_ADDR,IOMG_REG_SIZE);
	/*配置GPIO复用*/
	writel(0,g_iomg_base_sd+ 39*4);
	#endif
#endif
/*--by pandong cs*/

    spin_lock_init(&sdio_spin_lock) ; 
 #if (BALONG_SDIO_ARG)
    hi_get_sdio_arg((unsigned char *)&g_wlanargs,sizeof(g_wlanargs));
 #endif
    
     /* interrupt involved */
	init_completion(&hisdio_cmd_complete);
	init_completion(&hisdio_data_busy_complete);
	
	init_MUTEX(&g_sdio_lock);
	
	psdio_resume_workque = create_singlethread_workqueue(DRIVER_RESUME_THREAD);
	
	INIT_WORK(&sdio_resume_work_suct, (void *)sdio_resume_thread);	
	INIT_WORK(&short_work_mmc, BSP_Short_Onoff_MMC);	/*lint !e69*/
#ifdef FEATURE_SD_ENABLE
	ret = platform_device_register(&hi_sdio_device);
	if(ret)
	{
  	
		hisdio_trace(5,"Platform device register is failed!");
		return ret;
	}

	ret = platform_driver_register(&hi_sdio_driver);
	if(ret)
	{
		platform_device_unregister(&hi_sdio_device);

		hisdio_trace(5,"Platform driver register is failed!");
		return ret;
	}
#endif
    creat_NvResume();
	hisdio_trace(1,"end");
	return ret;
}

static void __exit hi_sdio_exit(void)
{
	hisdio_trace(1,"begin");
/*++by pandong cs*/
#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	#if defined(CS_SFT)
	#else
	iounmap(g_iomg_base_sd);
	#endif
#endif
/*--by pandong cs*/
#ifdef FEATURE_SD_ENABLE
	platform_driver_unregister(&hi_sdio_driver);
	platform_device_unregister(&hi_sdio_device);
#endif

	hisdio_trace(1,"end");
}

#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
#else
module_init(hi_sdio_init);
module_exit(hi_sdio_exit);
#endif
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("MMC/SDIO driver for the Hisilicon MMC/SD Host Controller");
MODULE_LICENSE("GPL");
