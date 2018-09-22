/*
 * balong_sdio.c - hisilicon balong sdio Host driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

 /******************************************************************
* Copyright (C), 2005-2011, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: Balong_sdio.c                                            *
*                                                                 *
* Description:                                                    *
*     SDIO Host LINUX Driver                                          *
*                                                                 *
* Author:  wangzhenwei                                              *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*                                                                 *
* Date:                                                           *
*                                                                 *
* History:                                                        *
*                                                                 *
*          Init.                                                  *
*******************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/scatterlist.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <linux/mutex.h>
#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/sizes.h>
#include <asm/uaccess.h>


#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <linux/slab.h>

#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2)
#include <mach/edmacIP.h>
#include <mach/edmacDrv.h>
#elif defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#include <mach/edmacIP.h>
		#include <mach/edmacDrv.h>
	#else
	#include "sdio_idmac.h"
	#endif
	/*--by pandong cs*/
#else
#error "there is no file included!"
#endif

#ifndef MMC1_SYSTEM_CTRL
#define MMC1_SYSTEM_CTRL
#endif

#if defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
//w00176398  debug for using TTF SKB
#ifndef USING_TTF_SKB
#define USING_TTF_SKB
#endif
#endif

#define TTF_BUF_MAGIC_SDIO 0xFFFFA55A

#include "hisdio_sys_ctrl.h"
#include "hisdio.h"
#include "wifi_printf.h"

int using_hold = 1;
int is_high_clock = 1;
int using_internal_dma = 1;
int using_internal_dma_cpu = 0;
int using_internal_dma_only = 1;

int dma_debug = 0;
int dma_debug_1 = 0;
int before_dma_delay = 0;
int after_dma_delay = 0;

#if 1 //w00176398 add for testing
unsigned char burst = 0x2;
unsigned char tx_water = 0x8;
unsigned char rx_water = 0x7;
#endif

#define TEST_SDIO
spinlock_t sdio_spinlock;
static struct mutex sdio_mutex;
static struct completion balong_sdio_data_complete;

static unsigned int sdio_getcd = 0;

static struct balong_sdio_host *sdio_host_copy = NULL;

static unsigned int request_timeout = HI_SDIO_REQUEST_TIMEOUT;

static int g_suspend = 0;
static int g_bitmode = 0;
static int g_clock = 0;
static int g_intrkmode = 0;
extern int debug_flag;//w00176398 add for debug
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
#define EDMA_FREE_LIST_NODE_NUM    100//25
LOCAL BALONG_DMA_CB * g_edma_free_list_node = NULL;
dma_addr_t  g_edma_phy_addr  = 0;
#endif
/*--by pandong cs*/
#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS) 
	#if defined(CS_SFT)
	#else
	 /*++by pandong cs*/
void __iomem  *g_iomg_base_wifi = 0;
	/*--by pandong cs*/
	#endif
#endif
int balong_sdio_trace_level = 1;

void dumpSdioReg_1(struct balong_sdio_host *host)
{
	if(balong_sdio_trace_level >= 10)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_CTRL,readl(host->base + MMC_CTRL));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_CLKDIV,readl(host->base + MMC_CLKDIV));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_CTYPE,readl(host->base + MMC_CTYPE));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_CMDARG,readl(host->base + MMC_CMDARG));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_CMD,readl(host->base + MMC_CMD));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_MINTSTS,readl(host->base + MMC_MINTSTS));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_RINTSTS,readl(host->base + MMC_RINTSTS));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_STATUS,readl(host->base + MMC_STATUS));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x: 0x%x \n",MMC_FIFOTH,readl(host->base + MMC_FIFOTH));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",MMC_IDSTS,readl(host->base + MMC_IDSTS));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",MMC_TCBCNT,readl(host->base + MMC_TCBCNT));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",MMC_TBBCNT,readl(host->base + MMC_TBBCNT));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",MMC_BYTCNT,readl(host->base + MMC_BYTCNT));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x80,readl(host->base + 0x80));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x84,readl(host->base + 0x84));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x88,readl(host->base + 0x88));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x8c,readl(host->base + 0x8c));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x90,readl(host->base + 0x90));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x94,readl(host->base + 0x94));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x98,readl(host->base + 0x98));
		balong_sdio_trace(WIFI_DEBUG_ERROR,"reg_%x:0x%x  \n",0x100,readl(host->base + 0x100));
	}
}

void dumpDataInfo_1(struct balong_sdio_host *host)
{
	int i = 0;
	struct scatterlist * sgNode = NULL;

	if(NULL == host->dma_sg)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"sg is NULL!!!");
		return;
	}
	
	balong_sdio_trace(WIFI_DEBUG_WARNING,"sg length:%d",host->mrq->data->sg_len);
	for(i = 0; i < host->mrq->data->sg_len; i++)
	{
		sgNode = &host->dma_sg[i];
		if(sgNode->offset != TTF_BUF_MAGIC_SDIO)
		{
			//system memory, unmap
			balong_sdio_trace(WIFI_DEBUG_WARNING,"SYSTEM SKB, Virt Addr:0x%x length:%d",sg_virt(sgNode),sgNode->length);
			//dma_unmap_single(dev, sgNode->dma_address, sgNode->length,dir);
			balong_sdio_trace(WIFI_DEBUG_WARNING,"SYSTEM SKB, Phy Addr:0x%x length:%d",sgNode->dma_address,sgNode->length);
		}
		else
		{	
			//TTF memory, don't need unmap
			balong_sdio_trace(WIFI_DEBUG_WARNING,"TTF SKB, Phy Addr:0x%x length:%d",sgNode->dma_address,sgNode->length);
		}
	}
}

void dumpSdioReg(struct balong_sdio_host *host)
{
	if(balong_sdio_trace_level >= 6)
	{
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_CTRL,readl(host->base + MMC_CTRL));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_CLKDIV,readl(host->base + MMC_CLKDIV));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_CTYPE,readl(host->base + MMC_CTYPE));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_CMDARG,readl(host->base + MMC_CMDARG));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_CMD,readl(host->base + MMC_CMD));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_MINTSTS,readl(host->base + MMC_MINTSTS));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_RINTSTS,readl(host->base + MMC_RINTSTS));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_STATUS,readl(host->base + MMC_STATUS));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x: 0x%x \n",MMC_FIFOTH,readl(host->base + MMC_FIFOTH));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",MMC_IDSTS,readl(host->base + MMC_IDSTS));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",MMC_TCBCNT,readl(host->base + MMC_TCBCNT));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",MMC_TBBCNT,readl(host->base + MMC_TBBCNT));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",MMC_BYTCNT,readl(host->base + MMC_BYTCNT));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x80,readl(host->base + 0x80));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x84,readl(host->base + 0x84));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x88,readl(host->base + 0x88));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x8c,readl(host->base + 0x8c));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x90,readl(host->base + 0x90));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x94,readl(host->base + 0x94));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x98,readl(host->base + 0x98));
		balong_sdio_trace(WIFI_DEBUG_DEBUG,"reg_%x:0x%x  \n",0x100,readl(host->base + 0x100));
	}
}

void dumpDataInfo(struct balong_sdio_host *host)
{
	if(balong_sdio_trace_level >= 6)
	{
		if(host->mrq->data != NULL)
		{
			balong_sdio_trace(WIFI_DEBUG_DEBUG,"sg_num=%d first sg length=%d !!!\n",host->mrq->data->sg_len, host->dma_sg->length);
			balong_sdio_trace(WIFI_DEBUG_DEBUG,"blksz=%d blocks=%d !!!\n",host->mrq->data->blksz,host->mrq->data->blocks);
			
            struct scatterlist *s;
            int k;
				
            for_each_sg(host->dma_sg, s, host->data->sg_len, k)
            balong_sdio_trace(WIFI_DEBUG_DEBUG,"%s sg_len:%d index:%d virtAddr:0x%x phyAddr:0x%x\n",__func__,s->length,k,sg_virt(s),sg_dma_address(s));
		}
	}
}

/*use cpu transfer data*/
#define USING_CPU_TRANSFER_FUNC(mrq)		\
	((mrq->data->sg_len == 1) && (mrq->data->sg[0].length < MMC_SDIO_CPU_TRXFER_MAX_LEN) && (!(((unsigned)sg_virt(mrq->data->sg)) & 0x03)))
	
/*******************************************************************
  Function:      void balong_sdio_lock(void)
  Description:   完成对sdio操作的互斥功能，获得锁
  Input:         NA     
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************/
static void balong_sdio_lock(void)
{
	mutex_lock(&sdio_mutex);
}

/******************************************************************
  Function:     void balong_sdio_unlock(void)
  Description:   完成对sdio操作的互斥功能，释放获得的锁
  Input:         NA     
  Output:        NA
  Return:        NA
  Others:        NA
******************************************************************/
static void balong_sdio_unlock(void)
{
	mutex_unlock(&sdio_mutex);
}

#ifdef USING_TTF_SKB
 /*******************************************************************************
  Function:      static inline void *sg_virt_exp(struct scatterlist *sg)
  Description:   extend unmap dma addres 
  Input:         arg:0 struct scatterlist *
  Output:        NA
  Return:        Return virtual address of an sg entry
  Others:        NA
*******************************************************************************/
static void *sg_virt_exp(struct scatterlist *sg)
{
	if(0 == sg->dma_address)
	{
		balong_sdio_trace(WIFI_DEBUG_WARNING,"system buf sg_virt:0x%x",sg_virt(sg));
		return sg_virt(sg);
	}
	else
	{
		balong_sdio_trace(WIFI_DEBUG_WARNING,"ttf buf phy:0x%x virt:0x%x",sg->dma_address,TTF_PHY_TO_VIRT(sg->dma_address));
		return (void *)TTF_PHY_TO_VIRT(sg->dma_address);
	}
}

/*******************************************************************************
  Function:      static void dma_unmap_sg_exp(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir)
  Description:   extend unmap dma addres 
  Input:         arg:0 struct device *
  		      arg:1 struct scatterlist *
  		      arg:2 int nents
  		      arg:3 enum dma_data_direction
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void dma_unmap_sg_exp(struct device *dev, struct scatterlist *sg,
				      int nents, enum dma_data_direction dir)
{
	int i = 0;
	struct scatterlist * sgNode = NULL;

	if(NULL == sg)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"sg is NULL!!!");
		return;
	}
	
	balong_sdio_trace(WIFI_DEBUG_WARNING,"sg length:%d",nents);
	for(i = 0; i < nents; i++)
	{
		sgNode = &sg[i];
		if(sgNode->offset != TTF_BUF_MAGIC_SDIO)
		{
			//system memory, unmap
			balong_sdio_trace(WIFI_DEBUG_WARNING,"unmap SYSTEM SKB, Virt Addr:0x%x length:%d",sg_virt(sgNode),sgNode->length);
			dma_unmap_single(dev, sgNode->dma_address, sgNode->length,dir);
			balong_sdio_trace(WIFI_DEBUG_WARNING,"unmap SYSTEM SKB, Phy Addr:0x%x length:%d",sgNode->dma_address,sgNode->length);
		}
		else
		{	
			//TTF memory, don't need unmap
			balong_sdio_trace(WIFI_DEBUG_WARNING,"unmap TTF SKB, Phy Addr:0x%x length:%d",sgNode->dma_address,sgNode->length);
		}
	}
}
/*******************************************************************************
  Function:      static dma_addr_t sg_dma_address_exp(struct balong_sdio_host *host,struct scatterlist *sgNode)
  Description:   get dma addres 
  Input:         arg:0 struct balong_sdio_host *
  		      arg:1 struct scatterlist *
  Output:        NA
  Return:        dma address
  Others:        NA
*******************************************************************************/
static dma_addr_t sg_dma_address_exp(struct balong_sdio_host *host,struct scatterlist *sgNode)
{
	if(sgNode->dma_address == 0)
	{
		//system memory,map virtual to dma address
		balong_sdio_trace(WIFI_DEBUG_WARNING,"using SYSTEM SKB, Virt Addr:0x%x length:%d",sg_virt(sgNode),sgNode->length);
		sgNode->dma_address = dma_map_single(mmc_dev(host->mmc), sg_virt(sgNode), sgNode->length,host->dma_dir);
		balong_sdio_trace(WIFI_DEBUG_WARNING,"using SYSTEM SKB, Phy Addr:0x%x length:%d",sgNode->dma_address,sgNode->length);
	}
	else
	{	
		//TTF memory, set TTF memory flag that used when unmap dma address
		sgNode->offset = TTF_BUF_MAGIC_SDIO;
		balong_sdio_trace(WIFI_DEBUG_WARNING,"using TTF SKB, Phy Addr:0x%x length:%d",sgNode->dma_address,sgNode->length);
	}

	return sgNode->dma_address;
}
/*******************************************************************************
  Macro:      get sg length
*******************************************************************************/
#define sg_dma_len_exp(sg) ((sg)->length)
#endif


/*******************************************************************************
  Function:      static int balong_sdio_wait_cmd(struct balong_sdio_host *host)
  Description:   check cmd is complete
  Input:         balong_sdio_host
  Output:        NA
  Return:        0 sucess other error
  Others:        NA
*******************************************************************************/
static int balong_sdio_wait_cmd(struct balong_sdio_host *host)
{
	int wait_retry_count = 0;
	unsigned int reg_data = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	for(;;)
	{
		/*
		Check if CMD::start_cmd bit is clear.
		start_cmd = 0 means MMC Host controller has loaded registers and
		next command can be loaded in. 
		*/
		reg_data = readl(host->base + MMC_CMD);
		if ((reg_data & START_CMD) == 0)
		{
			balong_sdio_trace(WIFI_DEBUG_INFO,"end");
			return 0;
		}

		/* Check if Raw_Intr_Status::HLE bit is set. */
		reg_data = readl(host->base + MMC_RINTSTS);
		if (reg_data & HLE_INT_STATUS)
		{
			reg_data |= HLE_INT_STATUS;
			writel(reg_data, host->base + MMC_RINTSTS);
			balong_sdio_trace(WIFI_DEBUG_ERROR,"Other CMD is running,please operate cmd again!");
			balong_sdio_trace(WIFI_DEBUG_ERROR,"end");
			return 1;
		}
		
		/* Check if number of retries for this are over. */
		wait_retry_count++;
		if (wait_retry_count >= MAX_RETRY_COUNT)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR,"wait cmd complete is timeout!");
			balong_sdio_trace(WIFI_DEBUG_ERROR,"end");
			return -1;
		} 
	}
}

/*******************************************************************************
  Function:      static void balong_sdio_control_cclk(struct balong_sdio_host *host,unsigned int flag)
  Description:   set sdio clock mode
  Input:         host:balong_sdio_host struct
                 flag:ENABLE or disable
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_control_cclk(struct balong_sdio_host *host,unsigned int flag)
{
	unsigned int reg;
	cmd_arg_s cmd_reg;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");

	reg = readl(host->base + MMC_CLKENA);
	if (flag == ENABLE)
	{
		reg |= CCLK_ENABLE;
	}
	else
	{
		reg &= ~CCLK_ENABLE;
	}
	writel(reg, host->base + MMC_CLKENA);
	balong_sdio_trace(WIFI_DEBUG_DEBUG,"sdio clk enable reg:0x%x",reg);

	cmd_reg.cmd_arg = readl(host->base + MMC_CMD); 
	cmd_reg.bits.start_cmd = 1;
	cmd_reg.bits.update_clk_reg_only = 1;
	
	writel(cmd_reg.cmd_arg, host->base + MMC_CMD);

	if (balong_sdio_wait_cmd(host) != 0)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"update sdio clk is timeout!");
	}
	
	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
}

/*******************************************************************************
  Function:      static void balong_sdio_set_cclk(struct balong_sdio_host *host,unsigned int cclk)
  Description:   set sdio clock 
  Input:         host:balong_sdio_host struct
                 cclk:clock number
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_set_cclk(struct balong_sdio_host *host,unsigned int cclk)
{
	unsigned int reg_value = 0;
	cmd_arg_s clk_cmd;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");

	if((cclk < MMC1_CCLK_MIN) || (cclk > MMC1_CCLK_MAX) )
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"invalid clock value! clock:%d",cclk);
		return ;
	}

	/* set clock divider */
       reg_value = MMC1_CLK / (cclk * 2);  /*SDXC?*/

/*for debug only*/
	if(is_high_clock != 1)	
	{
		if(0 == reg_value)
		{
			balong_sdio_trace(WIFI_DEBUG_DEBUG,"cclk:%d reg_value:%d\n",cclk,reg_value);
			reg_value = 1;
		}
	}	
	balong_sdio_trace(WIFI_DEBUG_DEBUG,"cclk:%d reg_value:%d\n",cclk,reg_value);
		
	writel(reg_value, host->base + MMC_CLKDIV);

	clk_cmd.cmd_arg = readl(host->base + MMC_CMD); 
	clk_cmd.bits.start_cmd = 1;
	clk_cmd.bits.update_clk_reg_only = 1;
	writel(clk_cmd.cmd_arg, host->base + MMC_CMD);

	if (balong_sdio_wait_cmd(host) != 0)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"set card clk divider is failed!");
	}

	balong_sdio_trace(WIFI_DEBUG_INFO,"end");				
}

/*******************************************************************************
  Function:      static void balong_sdio_bus_init(struct balong_sdio_host *host)
  Description:   init sdio card 
  Input:         host:balong_sdio_host struct
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_bus_init(struct balong_sdio_host *host)
{
	unsigned int tmp_reg; 

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");

	/* MASK MMC host intr */
	tmp_reg = 0x0;/*FIXME for which card is connected*/
	writel(tmp_reg, host->base + MMC_INTMASK);

	tmp_reg = 0xfffe;
	/* clear raw irqs */
	writel(tmp_reg, host->base  + MMC_RINTSTS);

	/* enable intr of MMC host controler */
	tmp_reg = readl(host->base + MMC_CTRL);

	tmp_reg |= DMA_ENABLE;
	tmp_reg |= INT_ENABLE;
	
	writel(tmp_reg,host->base + MMC_CTRL);

	/* set timeout param */
	writel(DATA_TIMEOUT | RESPONSE_TIMEOUT, host->base + MMC_TMOUT);

	/* set FIFO param */
	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | RX_WMARK | TX_WMARK;
	writel(tmp_reg, host->base + MMC_FIFOTH);

	/*set clk 400k to enum*/
	balong_sdio_control_cclk(host, DISABLE);
	balong_sdio_set_cclk(host, 400000);
	balong_sdio_control_cclk(host, ENABLE);
	udelay(20);

	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
}

/*******************************************************************************
  Function:      void hi_sdio_detectcard_to_core(unsigned int arg)
  Description:   notify sdio core cart status 
  Input:         arg:0 out 1 insert
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void balong_sdio_detectcard_to_core(unsigned int arg)
{
	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	if (1 == arg)
	{
		sdio_getcd = 1;
	}
	else
	{
		sdio_getcd = 0;
	}
	
	balong_sdio_trace(WIFI_DEBUG_INFO,"end");

	return;
}
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
/*******************************************************************************
  Function:      static void transfer_edma_config(struct balong_sdio_host *host)
  Description:   config dma operater
  Input:         arg:0 struct balong_sdio_host
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void transfer_edma_config(struct balong_sdio_host *host)
{
	unsigned long  ireg = 0;
	unsigned long  tmp_reg = 0;

	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | RX_WMARK | TX_WMARK;    /*burst = 8*/	
	writel(tmp_reg, host->base + MMC_FIFOTH);

	ireg = 0;
	ireg = readl(host->base + MMC_CTRL);
	ireg |= DMA_ENABLE ; //enable dma transfer
	ireg &= ~IDMA_ENABLE ;
	ireg |= INT_ENABLE;
	writel(ireg, host->base + MMC_CTRL);
}
/*******************************************************************************
  Function:      void balong_sdio_edma_start(struct balong_sdio_host *host, UINT32 req)
  Description:   start dma transfer 
  Input:         arg:0 struct balong_sdio_host
  		      arg:1 dma request num
  Output:        NA
  Return:        0 sucess other error
  Others:        NA
*******************************************************************************/
static int balong_sdio_edma_start(struct balong_sdio_host *host, UINT32 req)
{
    struct scatterlist      *sgHeader;
    struct scatterlist      *sgNode;

    int ret_id = 0;
    int i      = 0;
    BALONG_DMA_CB *psttemp    = NULL;
    BALONG_DMA_CB *FirstNode   = NULL;
    int edma_trans_ret = 0;
    sgHeader = host->dma_sg;
    sgNode = sgHeader;

    /*使用EDMA前的配置*/
    transfer_edma_config(host);
    
    balong_sdio_trace(WIFI_DEBUG_INFO,"begin edma start ############");
    
    FirstNode = g_edma_free_list_node;
    psttemp = g_edma_free_list_node;

    balong_sdio_trace(WIFI_DEBUG_DEBUG,"sg_len:%d",host->data->sg_len);

   if(debug_flag == 1)
   {
    	struct scatterlist *s;
   	    int k;
    	for_each_sg(host->dma_sg, s, host->data->sg_len, k)
    	if(s == NULL || debug_flag == 1)
    	{
            balong_sdio_trace(WIFI_DEBUG_DEBUG,"%s sg_len:%d index:%d data:0x%x\n",__func__,host->data->sg_len,k,sg_virt(s));
    	}
   }
    /*根据scatterlist个数创建链表*/
    if (host->data->flags & MMC_DATA_READ) 
    {		
		 host->dma_dir = (int)DMA_FROM_DEVICE;
	#ifndef USING_TTF_SKB
	        host->dma_len = dma_map_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
		    balong_sdio_trace(WIFI_DEBUG_DEBUG,"Read host->dma_len:%d",host->dma_len);
	        for_each_sg(sgHeader, sgNode, host->dma_len, i)
	        {
	#else
		 host->dma_len = host->data->sg_len;
		 for(i = 0; i < host->dma_len; i++)
		 {
			sgNode = &sgHeader[i];
	#endif
	        psttemp->lli = BALONG_DMA_SET_LLI(g_edma_phy_addr + (i+1) * sizeof(BALONG_DMA_CB), ((i < host->dma_len - 1)?0:1));
	        psttemp->config = BALONG_DMA_SET_CONFIG(req, BALONG_DMA_P2M, 2, (RX_WMARK>>16) -1); /*burst_size = 32,burst_len = 7*/
	        psttemp->src_addr = HI_MCI_BASE1 + MMC_DATA1;  /*FIFO物理地址*/

	#ifndef USING_TTF_SKB			
	        psttemp->des_addr = sg_dma_address(sgNode);  /*物理地址*/
	#else
		    psttemp->des_addr = sg_dma_address_exp(host,sgNode);  /*物理地址*/
	#endif

		     balong_sdio_trace(WIFI_DEBUG_DEBUG,"Read des_addr:0x%x dma_addr:0x%x sg_dma_len:%d\n",psttemp->des_addr,sgNode->dma_address,sg_dma_len(sgNode));
		     if(debug_flag == 1)
		     {
		     		 balong_sdio_trace(WIFI_DEBUG_DEBUG,"Read des_addr:0x%x dma_addr:0x%x sg_dma_len:%d\n",psttemp->des_addr,sgNode->dma_address,sg_dma_len(sgNode));
		     }
			 
	#ifndef USING_TTF_SKB		 
	         psttemp->cnt0 = sg_dma_len(sgNode);
	#else
		     psttemp->cnt0 = sg_dma_len_exp(sgNode);
	#endif
	         psttemp->bindx = 0;
	         psttemp->cindx = 0;
	         psttemp->cnt1  = 0;
	         psttemp++;
	     }
	}	
	else	
	{		
	#ifndef USING_TTF_SKB
        host->dma_dir = (int)DMA_TO_DEVICE;	
        host->dma_len = dma_map_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
        balong_sdio_trace(WIFI_DEBUG_DEBUG,"Write host->dma_len:%d",host->dma_len);
        for_each_sg(sgHeader, sgNode, host->dma_len, i)
        {
	#else
		host->dma_len = host->data->sg_len;
		for(i = 0; i < host->dma_len; i++)
		{
		     sgNode = &sgHeader[i];	
	#endif
	         psttemp->lli = BALONG_DMA_SET_LLI(g_edma_phy_addr + (i+1) * sizeof(BALONG_DMA_CB), ((i < host->dma_len - 1)?0:1));
	         psttemp->config = BALONG_DMA_SET_CONFIG(req, BALONG_DMA_M2P, 2, TX_WMARK - 1); /*burst_size = 32,burst_len = 8*/
	#ifndef USING_TTF_SKB
		     psttemp->src_addr = sg_dma_address(sgNode);  /*物理地址*/
	#else
		     psttemp->des_addr = sg_dma_address_exp(host,sgNode);  /*物理地址*/
	#endif

	        balong_sdio_trace(WIFI_DEBUG_DEBUG,"Write src_addr:0x%x dma_addr:0x%x sg_dma_len:%d\n",psttemp->src_addr,sgNode->dma_address,sg_dma_len(sgNode));
         
		     if(debug_flag == 1)
		     {
		     	balong_sdio_trace(WIFI_DEBUG_DEBUG,"Write src_addr:0x%x dma_addr:0x%x sg_dma_len:%d\n",psttemp->src_addr,sgNode->dma_address,sg_dma_len(sgNode));
		     }	 
		     	 
	         psttemp->des_addr = HI_MCI_BASE1 + MMC_DATA1;  /*FIFO物理地址*/
	 #ifndef USING_TTF_SKB			
	         psttemp->cnt0 = sg_dma_len(sgNode);
	 #else
		     psttemp->cnt0 = sg_dma_len_exp(sgNode);
	 #endif
	         psttemp->bindx = 0;
	         psttemp->cindx = 0;
	         psttemp->cnt1  = 0;
	         psttemp++;
	     }
	}    
    balong_sdio_trace(WIFI_DEBUG_INFO,"LII list init is success!\n"); 

    /*申请通道，初始化传输完成信号量*/
    ret_id = balong_dma_channel_init(req, NULL, 0, 0);
    if (ret_id < 0)
    {
        balong_sdio_trace(WIFI_DEBUG_ERROR,"-----------error ret_id = 0x%X\n",ret_id);
        return -1;
    }

    /*获取首节点寄存器地址*/
    psttemp = balong_dma_channel_get_lli_addr(ret_id);
    if (NULL == psttemp)
    {
        balong_sdio_trace(WIFI_DEBUG_ERROR,"---balong_dma_channel_get_lli_addr failed!\n");
        return -1;
    }

    /*配置首节点寄存器*/
    psttemp->config = 0;  /*配置前确保通道disable*/
    psttemp->lli = FirstNode->lli;
    psttemp->config = FirstNode->config & 0xFFFFFFFE; 
    psttemp->src_addr = FirstNode->src_addr;  /*物理地址*/
    psttemp->des_addr = FirstNode->des_addr;  /*物理地址*/
    psttemp->cnt0 = FirstNode->cnt0;
    psttemp->bindx = 0;
    psttemp->cindx = 0;
    psttemp->cnt1  = 0; 

    /*启动EDMA传输，等待传输完成信号量释放后返回*/
    edma_trans_ret = balong_dma_channel_lli_start(ret_id);
#if 0 //w00176398 2011-11-26 add for testing
#ifndef USING_TTF_SKB
    dma_unmap_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
#else
    dma_unmap_sg_exp(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
#endif
#endif

    if (edma_trans_ret)
    {
        balong_sdio_trace(WIFI_DEBUG_ERROR,"balong_dma_channel_lli_start FAILED!\n");
        return edma_trans_ret;
    }
    balong_sdio_trace(WIFI_DEBUG_INFO,"eDMA end!\n");
    return edma_trans_ret;
}
#elif defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
static void transfer_idma_config(struct balong_sdio_host *host)
{
	unsigned long  ireg = 0;
	unsigned int   tmp_reg = 0;

    	IDMAC_config_init();
	/*before use internal DMA, must diable RX & TX request interrupt*/
	ireg = 0;
	ireg = readl(host->base + MMC_INTMASK);
	ireg &=~(INTMASK_RXMASK |INTMASK_TXMASK);
	writel(ireg, host->base + MMC_INTMASK);

	/* set FIFO param */
	tmp_reg = 0;
	tmp_reg |= BURST_SIZE | RX_WMARK | TX_WMARK;       /*burst = 8*/
	writel(tmp_reg,host->base + MMC_FIFOTH);
	
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
static int balong_sdio_dma_start(struct balong_sdio_host *host)
{
	struct scatterlist      *sgHeader;
	struct scatterlist      *sgNode;
	BSP_S32 ret = 0;
	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	sgHeader = host->dma_sg;
	sgNode = sgHeader;
	if (host->data->flags & MMC_DATA_READ) 
	{
		host->dma_dir = DMA_FROM_DEVICE;
	}
	else
	{
		host->dma_dir = DMA_TO_DEVICE;
	}

#ifndef USING_TTF_SKB
	host->dma_len = dma_map_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
#else
	//if (host->data->flags & MMC_DATA_READ) 
	int i = 0;
	host->dma_len = host->data->sg_len;
	balong_sdio_trace(WIFI_DEBUG_WARNING,"sg length:%d",host->dma_len);
	for(i = 0; i < host->dma_len; i++)
	{
		sg_dma_address_exp(host, &host->dma_sg[i]);
	}
	
#endif
	transfer_idma_config(host);
	ret = IDMAC_SingleChannelStart(sgHeader, host->dma_len, host->dma_dir);
	if(BSP_OK != ret)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"IDMAC transfer failed!!!");
	}
	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
	return ret;
}

/*******************************************************************************
  Function:      static void balong_sdio_idma_transfer(struct hisdio_host *host)
  Description:   start dma
  Input:         host:hisdio_host struct
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static int balong_sdio_idma_transfer(struct balong_sdio_host *host, struct mmc_request* mrq)
{
	int ret = -1;
	
	if(mrq->data)
   	{
   		balong_sdio_trace(WIFI_DEBUG_INFO,"using internal dma 111");
       	host->data = mrq->data;
		host->dma_sg = mrq->data->sg;

        if(using_internal_dma_only || !USING_CPU_TRANSFER_FUNC(mrq))
		{
		    balong_sdio_trace(WIFI_DEBUG_INFO,"start dma 111");
		    dumpDataInfo(host);
		    dumpSdioReg(host);
		    ret = balong_sdio_dma_start(host);
		    if(ret)//dma error
		    {
		        balong_sdio_trace(WIFI_DEBUG_ERROR,"dma error! address alignment error or NULL point error!");
		        mrq->data->error = -ENOBUFS;
		        goto request_end;
		    }
		    dumpDataInfo(host);
		    dumpSdioReg(host);
		    balong_sdio_trace(WIFI_DEBUG_INFO,"end dma 111");	
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_INFO,"don't using internal dma");
		}
    	}
request_end:
	return ret;
}
		
#endif

/*******************************************************************************
  Function:      static int balong_sdio_config_cmd(struct balong_sdio_host *host, 
                                  struct mmc_command *cmd, struct mmc_data *data)
  Description:   send cmd to card
  Input:         host:balong_sdio_host struct
                 mmc_command:sdio cmd
                 mmc_data:the point of data
  Output:        NA
  Return:        0 sucess 1 do not support cmd other error
  Others:        NA
*******************************************************************************/
static int balong_sdio_config_cmd(struct balong_sdio_host *host, struct mmc_command *cmd, struct mmc_data *data, volatile cmd_arg_s* cmd_regs)
{
       balong_sdio_trace(WIFI_DEBUG_DEBUG,"cmd->opcode = %d",cmd->opcode);

	if(cmd == host->mrq->stop)
	{
		cmd_regs->bits.stop_abort_cmd = 1;
		cmd_regs->bits.wait_prvdata_complete = 0;
	}
	else
	{
		cmd_regs->bits.stop_abort_cmd = 0;
		cmd_regs->bits.wait_prvdata_complete = 1;
	}

	switch (mmc_resp_type(cmd)) 
	{
		case MMC_RSP_NONE:
			cmd_regs->bits.response_expect = 0;
			cmd_regs->bits.response_length = 0;
			cmd_regs->bits.check_response_crc = 0;
			break;
		case MMC_RSP_R1:
		/* case MMC_RSP_R5: */
		case MMC_RSP_R1B:
			cmd_regs->bits.response_expect = 1;
			cmd_regs->bits.response_length = 0;
			cmd_regs->bits.check_response_crc = 1;
			break;
		case MMC_RSP_R2:
			cmd_regs->bits.response_expect = 1;
			cmd_regs->bits.response_length = 1;
			cmd_regs->bits.check_response_crc = 1;
			break;
		case MMC_RSP_R3:
			cmd_regs->bits.response_expect = 1;
			cmd_regs->bits.response_length = 0;
			cmd_regs->bits.check_response_crc = 0;
			break;
		default:
			balong_sdio_trace(WIFI_DEBUG_ERROR,"balong_mci: unhandled response type %02x\n", mmc_resp_type(cmd));
			return -EINVAL;

	}


	if (cmd->opcode == MMC_GO_IDLE_STATE)
	{
		cmd_regs->bits.send_initialization = 1;
	}
	else
	{
		cmd_regs->bits.send_initialization = 0;
	}

	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
	return 0;
}

static int balong_sdio_exec_cmd_new(struct balong_sdio_host *host, struct mmc_command *cmd, struct mmc_data *data)
{
	volatile cmd_arg_s  cmd_regs;
	unsigned int blk_size,tmp_reg,fifo_count = 0;
       unsigned int cmd_irq_reg = 0;

	host->cmd = cmd;
    
       balong_sdio_trace(WIFI_DEBUG_DEBUG,"cmd->opcode = %d",cmd->opcode);
	
	if(data)
	{
		/*did need to do DMA_RESET ?,confirm by zxf*/
		tmp_reg = readl(host->base + MMC_CTRL);
		tmp_reg |= FIFO_RESET;
		writel(tmp_reg,host->base + MMC_CTRL);

		do{
			tmp_reg = readl(host->base + MMC_CTRL);
			fifo_count++;

			if(fifo_count >= MAX_RETRY_COUNT)
			{
				balong_sdio_trace(WIFI_DEBUG_ERROR,"reset is timeout!");	
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
		//if(using_hold)	
			cmd_regs.bits.use_hold_reg = 1;
		//else
		//	cmd_regs.bits.use_hold_reg = 0;
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
			cmd_regs.bits.read_write = 1;
		}
		else if(data->flags & MMC_DATA_READ)
		{
			cmd_regs.bits.read_write = 0;
		}
	}
	else
	{
		cmd_regs.bits.data_transfer_expected = 0;
		cmd_regs.bits.transfer_mode = 0;
		cmd_regs.bits.read_write = 0;
		//if(using_hold)	
			cmd_regs.bits.use_hold_reg = 1;
		//else
		//	cmd_regs.bits.use_hold_reg = 0;
		///balong_sdio_trace(1, "cmd use_hold_reg = 0x%x!\n",cmd_regs.bits.use_hold_reg);  /*tiaoshi*/
	}
	
	balong_sdio_config_cmd(host, cmd, data, &cmd_regs);

	cmd_regs.bits.card_number = 0;
	cmd_regs.bits.cmd_index = cmd->opcode;
	cmd_regs.bits.send_auto_stop = 0;
	cmd_regs.bits.start_cmd = 1;
	cmd_regs.bits.update_clk_reg_only = 0;
	cmd_regs.bits.check_response_crc = 0;
	
	cmd_irq_reg = ALL_INT_CLR1 & readl(host->base + MMC_RINTSTS);
	if(cmd_irq_reg != 0)
	balong_sdio_trace(WIFI_DEBUG_DEBUG,"cmd_irq_reg:0x%x", cmd_irq_reg);;
	writel(cmd_regs.cmd_arg, host->base + MMC_CMD);
	
	if (balong_sdio_wait_cmd(host) != 0)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"send card cmd is failed!");
		return -EINVAL;            
	}
	
	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
	return 0;
}

/*******************************************************************************
  Function:      static void balong_sdio_finish_request(struct balong_sdio_host *host, struct mmc_request *mrq)
  Description:   sdio cmd finish
  Input:         NA
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_finish_request(struct balong_sdio_host *host, struct mmc_request *mrq)
{
	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	
	host->mrq = NULL;
	host->cmd = NULL;
	host->data = NULL;
	mmc_request_done(host->mmc, mrq);
	
	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
}

/*******************************************************************************
  Function:      static void balong_sdio_cmd_done(struct balong_sdio_host *host, unsigned int stat)
  Description:   read card resp
  Input:         host:balong_sdio_host struct
                 stat 
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_cmd_done(struct balong_sdio_host *host, unsigned int stat)
{
	unsigned int i;
	struct mmc_command *cmd = host->cmd;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	
	host->cmd = NULL;

	for (i=0; i<4; i++)
	{
		if (mmc_resp_type(cmd) == MMC_RSP_R2)
		{	
			cmd->resp[i] = readl(host->base + MMC_RESP3 - i * 0x4); 
		}
		else
		{
			cmd->resp[i] = readl(host->base + MMC_RESP0 + i * 0x4); 
		}
	}

	if (stat & RTO_INT_STATUS)
	{
		cmd->error = -ETIMEDOUT;
		balong_sdio_trace(WIFI_DEBUG_ERROR,"irq cmd status stat = 0x%x is timeout error", stat);
	}
	else if (stat & (RCRC_INT_STATUS | RE_INT_STATUS))
	{
		cmd->error = -EILSEQ;
		balong_sdio_trace(WIFI_DEBUG_ERROR,"irq cmd status stat = 0x%x is response error!", stat);
	}

	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
}

/*******************************************************************************
  Function:      static void balong_sdio_data_done(struct balong_sdio_host *host, unsigned int stat)
  Description:   sdio data handle
  Input:         host:balong_sdio_host struct
                 stat 
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_data_done(struct balong_sdio_host *host, unsigned int stat)
{
	struct mmc_data *data = host->data;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	
	if (stat & (HTO_INT_STATUS | DRTO_INT_STATUS))
	{
		data->error = -ETIMEDOUT;
		balong_sdio_trace(WIFI_DEBUG_ERROR,"irq data status stat = 0x%x is timeout error!", stat);
		dumpSdioReg(host);
		dumpDataInfo(host);
	}
	else if (stat & (EBE_INT_STATUS | SBE_INT_STATUS | FRUN_INT_STATUS | DCRC_INT_STATUS))
	{
		data->error = -EILSEQ;
		balong_sdio_trace(WIFI_DEBUG_ERROR,"irq data status stat = 0x%x is data error!", stat);
		dumpSdioReg_1(host);
		dumpDataInfo_1(host);
	}
	if (!data->error)
	{
		data->bytes_xfered = data->blocks * data->blksz;

	}
	else
	{
		data->bytes_xfered = 0;
	}
#if 0//w00176398 2011-11-26 add for testing	
	host->data = NULL;
#endif	
	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
}


/*******************************************************************************
  Function:      static int balong_sdio_wait_cmd_complete(struct balong_sdio_host *host)
  Description:   sdio data handle
  Input:         host:balong_sdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/
static int balong_sdio_wait_cmd_complete(struct balong_sdio_host *host)
{
	unsigned int cmd_retry_count = 0;
	unsigned long cmd_jiffies_timeout;
	unsigned int cmd_irq_reg = 0;
	struct mmc_command *cmd = host->cmd;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
	
	cmd_jiffies_timeout = jiffies + request_timeout;
	
	for(;;)
	{
		if (!time_before(jiffies, cmd_jiffies_timeout))
		{
			cmd->error = -ETIMEDOUT;
			balong_sdio_trace(WIFI_DEBUG_ERROR,"wait cmd request complete is timeout!");
			balong_sdio_trace(WIFI_DEBUG_WARNING,"end");	
			return -1;	        
		}
		
		do{
			cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
			if (cmd_irq_reg & CD_INT_STATUS)
			{
				cmd_irq_reg &= CD_INT_STATUS;
				/* clear interrupt */
				writel(cmd_irq_reg, host->base + MMC_RINTSTS);
				balong_sdio_cmd_done(host, cmd_irq_reg);
				balong_sdio_trace(WIFI_DEBUG_INFO,"end");
				return 0;
			}
			cmd_retry_count++;
		}while(cmd_retry_count < MAX_RETRY_COUNT);
		schedule();
	}
}

/*******************************************************************************
  Function:      static int balong_sdio_wait_data_complete(struct balong_sdio_host *host)
  Description:   sdio data handle
  Input:         host:balong_sdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/
static int balong_sdio_wait_data_complete(struct balong_sdio_host *host)
{
	unsigned int data_irq_reg = 0;
	int ret = 0; 
	struct mmc_data *data = host->data;
	unsigned int data_retry_count = 0;
	unsigned long data_jiffies_timeout;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");

	if (data->sg->length <= SDIO_DATA_MAX_POLL)
	{
		request_timeout = SDIO_DATA_TIMEOUT;

		data_jiffies_timeout = jiffies + request_timeout;
	
		while(1)
		{
			if (!time_before(jiffies, data_jiffies_timeout))
			{
				data->error = -ETIMEDOUT;
				balong_sdio_trace(WIFI_DEBUG_ERROR,"1 wait data request complete is timeout! irq reg:0x%x",data_irq_reg);
				dumpSdioReg(host);
				dumpDataInfo(host);
				balong_sdio_trace(WIFI_DEBUG_WARNING,"end");
				return -1;	        
			}

			do
			{
				data_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);

				if (data_irq_reg & DTO_INT_STATUS)
				{
					/* clear interrupt */
					writel(data_irq_reg, host->base + MMC_RINTSTS);
					balong_sdio_data_done(host, data_irq_reg);
					balong_sdio_trace(WIFI_DEBUG_INFO,"end");
					return 0;
				}
				data_retry_count++;
			}while(data_retry_count < MAX_RETRY_COUNT);

			schedule();
		}
	}
	else
	{
		/*open DTO interrupt*/
		set_bit(DTO_INT_NUM, host->base + MMC_INTMASK);

		ret = wait_for_completion_timeout(&balong_sdio_data_complete, SDIO_DATA_TIMEOUT);
		
		data_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);

		/* clear interrupt */
		writel(data_irq_reg, host->base + MMC_RINTSTS);	

		if (!ret) 
		{
			data->error = -ETIMEDOUT;
			balong_sdio_trace(WIFI_DEBUG_ERROR,"1 wait data request complete is timeout! irq reg:0x%x",data_irq_reg);
			dumpSdioReg(host);
			dumpDataInfo(host);
			balong_sdio_trace(0,"end");
			return -1;	
		}
		else
		{
			balong_sdio_data_done(host, data_irq_reg);
			balong_sdio_trace(WIFI_DEBUG_INFO,"end");
			return 0;
		}	
	}	
}

/*******************************************************************************
  Function:      static int balong_sdio_wait_card_complete(struct balong_sdio_host *host)
  Description:   sdio data handle
  Input:         host:balong_sdio_host struct
  Output:        NA
  Return:        0 sucess other is error.
  Others:        NA
*******************************************************************************/
static int balong_sdio_wait_card_complete(struct balong_sdio_host *host)
{
	unsigned int card_retry_count = 0;
	unsigned long card_jiffies_timeout;
	unsigned int card_status_reg = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");

	card_jiffies_timeout = jiffies + request_timeout;
	while(1)
	{
		if (!time_before(jiffies,card_jiffies_timeout))
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR,"wait card ready complete is timeout!");
			balong_sdio_trace(WIFI_DEBUG_WARNING,"end");
			return -1;	        
		}

		do
		{
			card_status_reg = readl(host->base + MMC_STATUS);
			if (!(card_status_reg & DATA_BUSY))
			{
				balong_sdio_trace(WIFI_DEBUG_INFO,"end");
				return 0;
			}
			card_retry_count++;
		}while(card_retry_count < MAX_RETRY_COUNT);

		schedule();
	}
}

/*******************************************************************************
  Function:      static irqreturn_t hisdio_irq(int irq, void *devid)
  Description:   sdio interrupt handle
  Input:         irq:irq number
                 devid:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static irqreturn_t balong_sdio_irq(int irq, void *devid)
{
	struct balong_sdio_host *host = devid;
	unsigned int ireg = 0;
	unsigned int ireg_org_intr = 0;
	unsigned int ireg_mintsts = 0;
	unsigned int ireg_rintsts = 0;
	unsigned int ireg_msk = 0;
	int handle = 0;

	balong_sdio_print(WIFI_DEBUG_INFO,"begin");

	ireg = readl(host->base + MMC_MINTSTS);
	
	ireg_org_intr = readl(host->base + MMC_MINTSTS);
 	ireg_rintsts = readl(host->base + MMC_RINTSTS);
	ireg_msk = readl(host->base + MMC_INTMASK);
	
	if (ireg & SDIO_INT_STATUS) 
	{	
		balong_sdio_print(WIFI_DEBUG_INFO,"Interrupt from card!!!");
		handle = 1;
		ireg = readl(host->base + MMC_INTMASK);
		ireg &= ~0xffff0000;
		writel(ireg, host->base + MMC_INTMASK);

		ireg = SDIO_INT_STATUS;
		writel(ireg, host->base + MMC_RINTSTS);

		mmc_signal_sdio_irq(host->mmc);
	}

	if (ireg & DTO_INT_STATUS)
	{
		balong_sdio_print(WIFI_DEBUG_INFO,"DTO complete");
		handle = 1;

		/*mask DTO interrupt*/
		clear_bit(DTO_INT_NUM, host->base + MMC_INTMASK);

		complete(&balong_sdio_data_complete);
	}
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	balong_sdio_print(WIFI_DEBUG_INFO, "using extern dma");
	/*--by pandong cs*/
#elif defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
	unsigned int mask_ints= 0;
	mask_ints = readl(host->base + MMC_IDSTS);
	if(mask_ints &(DmaIntAbnormal | DmaIntNormal | DmaIntCardErrSum |DmaIntBusError | DmaIntRxCompleted | DmaIntTxCompleted))
	{
		balong_sdio_print(WIFI_DEBUG_ERROR,"internal dma error : ireg: 0x%x, raw: 0x%x, intmask: 0x%x, reg8c:0x%x ", ireg,readl(host->base + 0x44),
       	readl(host->base + MMC_INTMASK), readl(host->base + 0x8c));
		balong_sdio_print(WIFI_DEBUG_ERROR,"before int handle__ internal dma error1: ireg_org_intr:0x%x ireg_rintsts:0x%x ireg_msk:0x%x",
			ireg_org_intr,ireg_rintsts,ireg_msk);
        	writel(mask_ints, host->base + SDIO_IDSTS);        
	}
#endif

	if (handle)
	{
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}
//unsigned int fifoWidth = 32;
//unsigned int fifoDepth = 128;
/*******************************************************************************
  Function:    static unsigned int balong_sdio_rw_fifo(struct balong_sdio_host* host,unsigned char * ucBuf, unsigned int ulByteCount, bool rw)
  Description:   read/write sdio fifo
  Input:         host:balong_sdio_host struct
  		      ucBuf: data buf
  		      ulByteCount: bytes of data buf
  		      rw: 0 read; 1 write
  Output:        NA
  Return:        0 sucess other error
  Others:        NA
*******************************************************************************/
static unsigned int balong_sdio_rw_fifo(struct balong_sdio_host* host,unsigned char * ucBuf, unsigned int ulByteCount, bool rw)
{
	SDIO_STATUS_REG statusReg = {0};
	unsigned int ulDataRemain = 0;
	unsigned int ulNeedSpace = 0;
	unsigned int ulRet = 0;
	unsigned int ulFifoWidthByte = 0;
	unsigned int i = 0;

	ulFifoWidthByte = host->fifoWidth >> 3;
	statusReg.ulReg = readl(host->base + MMC_STATUS);
	
	if (rw)
	{
	       /*Fifo满，无法写入，返回*/
		if (statusReg.Bits.fifo_full)
		{
			balong_sdio_trace(WIFI_DEBUG_WARNING,"fifo is full, statusReg:0x%x", statusReg.ulReg);
			return 0;
		}
		
		/*Fifo空，可以写入*/
		if (statusReg.Bits.fifo_empty)
		{
			balong_sdio_trace(WIFI_DEBUG_INFO,"fifo is empty, statusReg:0x%x", statusReg.ulReg);
			ulDataRemain = host->fifoDepth;
		}
		else/*Fifo有数据，未满，可继续写入*/
		{
			balong_sdio_trace(WIFI_DEBUG_DEBUG,"fifo have data, statusReg:0x%x", statusReg.ulReg);
			ulDataRemain = host->fifoDepth - statusReg.Bits.fifo_count;
		}

		ulNeedSpace = ulByteCount / (ulFifoWidthByte);

		if (ulDataRemain > ulNeedSpace)
		{
			ulDataRemain = ulNeedSpace;
		}

		for (i = 0; i < ulDataRemain; i++)
		{
			writel( *(unsigned int*)(ucBuf + (i*ulFifoWidthByte)), host->base + MMC_DATA1);
		}
	}
	else
	{	
		/*Fifo空，没有数据可以读取返回*/
		if (statusReg.Bits.fifo_empty)
		{
			balong_sdio_trace(WIFI_DEBUG_WARNING,"fifo is empty, statusReg:0x%x",statusReg.ulReg);
			return 0;
		}
		
	       /*Fifo有数据，读取*/
		if (statusReg.Bits.fifo_full)
		{
			balong_sdio_trace(WIFI_DEBUG_DEBUG,"fifo is full, statusReg:0x%x",statusReg.ulReg);
			ulDataRemain = host->fifoDepth;
		}
		else
		{
			ulDataRemain = statusReg.Bits.fifo_count;
		}

		ulNeedSpace = ulByteCount / (ulFifoWidthByte);

		if (ulDataRemain > ulNeedSpace)
		{
			ulDataRemain = ulNeedSpace;
		}

		for (i = 0; i < ulDataRemain; i++)
		{
			*(unsigned int*)(ucBuf + (i*ulFifoWidthByte)) = readl(host->base + MMC_DATA1);
		}
	}

	ulRet = ulDataRemain * ulFifoWidthByte;
	balong_sdio_trace(WIFI_DEBUG_DEBUG,"data w/r byte count:%d", ulRet);
	return ulRet;
}

/*******************************************************************************
  Function:    static int balong_sdio_cpu_rw(struct balong_sdio_host* host,
							  unsigned char * ucBuf,
							  unsigned int ulByteCount,
							  bool rw)
  Description:   cpu transfer
  Input:         host:balong_sdio_host struct
  		      ucBuf: data buf
  		      ulByteCount: bytes of data buf
  		      rw: 0 read; 1 write
  Output:        NA
  Return:        0 sucess other error
  Others:        NA
*******************************************************************************/
static int balong_sdio_cpu_rw(struct balong_sdio_host* host,
							  unsigned char * ucBuf,
							  unsigned int ulByteCount,
							  bool rw)
{
	int i = 0;
	unsigned int ulRemainedByte = ulByteCount;
	unsigned int ulActualByte = 0;
	
	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");

	while(ulRemainedByte > 0)
	{	
		ulActualByte =  balong_sdio_rw_fifo(host, ucBuf + ulByteCount - ulRemainedByte, ulRemainedByte, rw);

		if (0 == ulActualByte)
		{
			if (i++ > 0x1000)
			{
				balong_sdio_trace(WIFI_DEBUG_ERROR, "Error!!! can't access sdio fifo, return");
				return -1;
			}
			udelay(10);
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_DEBUG, "ulActualByte:0x%x",ulActualByte);
			i = 0;
			ulRemainedByte -= ulActualByte;
		}
	}
	
	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
	return 0;
}

/*******************************************************************************
  Function:    static int balong_sdio_cpu_start(struct balong_sdio_host *host)
  Description:   start cpu transfer
  Input:         host:balong_sdio_host struct
  Output:        NA
  Return:        0 sucess other error
  Others:        NA
*******************************************************************************/
static int balong_sdio_cpu_start(struct balong_sdio_host *host)
{
	UINT8* ucBuf = NULL;
	bool rw = 0;
	int ret = 0;

	balong_sdio_trace(WIFI_DEBUG_DEBUG, "data w/r byte count:%d", host->dma_sg->length);
	if (host->data->flags & MMC_DATA_READ) 
	{
		rw = MMC_SDIO_READ;
	}
	else
	{
		rw = MMC_SDIO_WRITE;
	}

#ifndef USING_TTF_SKB
	ucBuf = (unsigned char*)sg_virt(host->dma_sg);
#else
	ucBuf = (unsigned char*)sg_virt_exp(host->dma_sg);
#endif
	
	if(NULL == ucBuf)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"get virtual addr from sg failed!");
		return -1;
	}
	
	ret = balong_sdio_cpu_rw(host, ucBuf, host->dma_sg->length, rw);
	
	return ret;

}

/*******************************************************************************
  Function:      static void balong_sdio_transfer_data_start(struct mmc_host* mmc, struct mmc_request* mrq)
  Description:   sdio request handle
  Input:         host:balong_sdio_host struct
                 mrq:mmc_request
  Output:        NA
  Return:        0 success others error
  Others:        NA
*******************************************************************************/
static int balong_sdio_transfer_data_start(struct balong_sdio_host *host, struct mmc_request* mrq)
{
	int ret = 0;
	unsigned int cmd_irq_reg = 0;
	int edma_ret = 0;

	if(NULL == host || NULL == mrq)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR, "Error Host or mrq is NULL\n");
		return -1;
	}
	/* start data transfer */
	host->data = mrq->data;
	host->dma_sg = mrq->data->sg;

	/*clear all interrupt*/
	cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
	//balong_sdio_trace(3,"cmd_irq_reg:0x%x\n",cmd_irq_reg);
	//cmd_irq_reg &= 0x04;
	cmd_irq_reg &= MMC_CMD_DONE;
	writel(cmd_irq_reg, host->base + MMC_RINTSTS);

	balong_sdio_trace(WIFI_DEBUG_DEBUG,"balong_sdio_request ------------------sg_num=%d first sg length=%d\n",mrq->data->sg_len, host->dma_sg->length);
	if (host->data->flags & MMC_DATA_READ) 
	{
		balong_sdio_trace(WIFI_DEBUG_INFO, "balong_sdio_request ------------------ Read\n");
	}
	else
	{
			balong_sdio_trace(WIFI_DEBUG_INFO, "balong_sdio_request ------------------ Write\n");
	}
    /*++by pandong cs*/	
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
	if(USING_CPU_TRANSFER_FUNC(mrq))  	
	{
		ret = balong_sdio_cpu_start(host);
		if (ret)
		{	
			balong_sdio_trace(WIFI_DEBUG_ERROR,"cpu data transfer error!");	
			mrq->data->error = -ENOBUFS;
			goto request_end;	
		}
	}
	else
	{
		edma_ret = balong_sdio_edma_start(host,host->dma_req_num);
		if (edma_ret)
		{	
			balong_sdio_trace(WIFI_DEBUG_ERROR,"dma error! address alignment error or NULL point error!");	
			mrq->data->error = -ENOBUFS;
			goto request_end;	
		}
	}
#else
	unsigned int ireg = 0;

	ireg = readl(host->base + MMC_CTRL);
    	ireg &= ~IDMA_ENABLE ;
    	writel(ireg, host->base + MMC_CTRL);
	
	ret = balong_sdio_cpu_start(host);
	if (ret)
	{	
		balong_sdio_trace(WIFI_DEBUG_ERROR,"cpu data transfer error!");	
		mrq->data->error = -ENOBUFS;
		goto request_end;	
	}
#endif
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
	/* wait data transfer complete */	
	ret = balong_sdio_wait_data_complete(host);
	if (ret)
	{
		mrq->cmd->error = ret;
		goto request_end;
	}
	balong_sdio_trace(WIFI_DEBUG_INFO,"balong_sdio_wait_data_complete OK\n");
#endif

request_end:
	return ret;
}

/*******************************************************************************
  Function:      static void balong_sdio_transfer_data(struct mmc_host* mmc, struct mmc_request* mrq)
  Description:   sdio request handle
  Input:         host:balong_sdio_host struct
                 mrq:mmc_request
  Output:        NA
  Return:        0 success others error
  Others:        NA
*******************************************************************************/
static int balong_sdio_transfer_data(struct balong_sdio_host *host, struct mmc_request* mrq)
{
		int ret = -1;
		
		balong_sdio_trace(WIFI_DEBUG_INFO, "using cpu");
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
		//if((((unsigned)sg_virt(host->dma_sg)) & 0x0FFF) || (mrq->data->sg_len == 1) && (mrq->data->sg[0].length < MMC_SDIO_CPU_TRXFER_MAX_LEN) && (!(((unsigned)sg_virt(host->dma_sg)) & 0x03))  || (mrq->data->sg[0].length == 576) || (sg_virt(host->dma_sg) == 0xc1f10c00) || (sg_virt(host->dma_sg) == 0xc1ed4020)) 
		//if(0 &&  (mrq->data->sg_len == 1) && (mrq->data->sg[0].length < MMC_SDIO_CPU_TRXFER_MAX_LEN) && (!(((unsigned)sg_virt(host->dma_sg)) & 0x03)))
		if(!using_internal_dma_only &&  USING_CPU_TRANSFER_FUNC(mrq))
		{
#endif		
		balong_sdio_trace(WIFI_DEBUG_INFO, "start cpu ......");
		dumpDataInfo(host);
		dumpSdioReg(host);
		ret = balong_sdio_transfer_data_start(host,mrq);
		if(ret != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR,"transfer data falied!!!");
			goto request_end;
		}
		balong_sdio_trace(WIFI_DEBUG_INFO, "end cpu ......");
		dumpDataInfo(host);
		dumpSdioReg(host);
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
		}
#endif
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
		/* wait data transfer complete */	
		ret = balong_sdio_wait_data_complete(host);
		if (ret)
		{
			mrq->cmd->error = ret;
			goto request_end;
		}
		balong_sdio_trace(WIFI_DEBUG_INFO,"balong_sdio_wait_data_complete OK\n");
#endif

request_end:
	return ret;
}
/*******************************************************************************
  Function:      static void balong_sdio_request(struct mmc_host* mmc, struct mmc_request* mrq)
  Description:   sdio request handle
  Input:         mmc:mmc_host struct
                 mrq:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_request(struct mmc_host* mmc, struct mmc_request* mrq)
{
	struct balong_sdio_host *host = mmc_priv(mmc);
	int ret = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");

	if (g_suspend)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"sdio is not resume!");
		return;
	}

	if (NULL == host || NULL == mrq)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"input param is error!");
		return;
	}

	host->mrq = mrq;

	ret = balong_sdio_wait_card_complete(host);

	if (ret)
	{
		mrq->cmd->error = ret;
		goto request_end;
	}
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
	unsigned int cmd_irq_reg = 0;
	
	/*clear all interrupt*/
    cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
    writel(cmd_irq_reg,host->base + MMC_RINTSTS);
	balong_sdio_idma_transfer(host, mrq);
#endif		
	/* send command */
	//ret = balong_sdio_exec_cmd(host, mrq->cmd, mrq->data);
	ret = balong_sdio_exec_cmd_new(host, mrq->cmd, mrq->data);
	if (ret)
	{
		mrq->cmd->error = ret;
		balong_sdio_trace(WIFI_DEBUG_ERROR,"cmd execute is error!");
		goto request_end;
	}

	/* wait command send complete */
	ret = balong_sdio_wait_cmd_complete(host);

	if (ret)
	{
		mrq->cmd->error = ret;
		goto request_end;
	}

	if (!(mrq->data && !mrq->cmd->error))
	{
		goto request_end;    
	}

	/* start data transfer */
	if (mrq->data)
	{
		ret = balong_sdio_transfer_data(host,mrq);
		if(ret != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR,"transfer data falied!!!");
			goto request_end;
		}
		if (mrq->stop)
		{
			balong_sdio_trace(WIFI_DEBUG_DEBUG, "111mrq->stop");
			/* send stop command */
			//ret = balong_sdio_exec_cmd(host,host->mrq->stop,host->data);
			ret = balong_sdio_exec_cmd_new(host,host->mrq->stop,host->data);
			if (ret)
			{
				mrq->cmd->error = ret;
				balong_sdio_trace(WIFI_DEBUG_WARNING,"end");	
				goto request_end;						
			}

			ret = balong_sdio_wait_cmd_complete(host);
			if (ret)
			{
				goto request_end;
			}
		}
	}
#if 1	
//w00176398 2011-11-26 add for unmap
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)	&& !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
 	if(using_internal_dma_only || !USING_CPU_TRANSFER_FUNC(mrq))
 	{
    #ifndef USING_TTF_SKB
       dma_unmap_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
    #else
       dma_unmap_sg_exp(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
    #endif
 	}
#else
	if(!USING_CPU_TRANSFER_FUNC(mrq))
	{
    //w00176398 2011-11-26 add for unmap
    #ifndef USING_TTF_SKB
        dma_unmap_sg(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
    #else
        dma_unmap_sg_exp(mmc_dev(host->mmc), host->dma_sg, host->data->sg_len,host->dma_dir);
    #endif
	}
#endif
#endif
request_end:

	balong_sdio_finish_request(host,mrq);
	balong_sdio_trace(WIFI_DEBUG_WARNING,"end");
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
static void balong_sdio_set_ios(struct mmc_host* mmc, struct mmc_ios* ios)
{
	struct balong_sdio_host *host = mmc_priv(mmc);
	unsigned int tmp_reg;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");

	balong_sdio_trace(WIFI_DEBUG_DEBUG, "ios->power_mode = %d ", ios->power_mode);

	if (g_suspend)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"sdio is not resume!");
		return;
	}

	switch(ios->power_mode)
	{
		case MMC_POWER_OFF:
			sys_ctrl_balong_sdio_power_io(POWER_OFF);
			break;
		case MMC_POWER_UP:
		case MMC_POWER_ON:
			sys_ctrl_balong_sdio_power_io(POWER_ON);
			break;
	}

	if (ios->bus_width != g_bitmode)
	{
		if (ios->bus_width == MMC_BUS_WIDTH_4)
		{
			tmp_reg = readl(host->base + MMC_CTYPE);
			tmp_reg |= CARD_WIDTH;
			writel(tmp_reg, host->base + MMC_CTYPE);
			g_bitmode = MMC_BUS_WIDTH_4;
		}
		else
		{
			tmp_reg = readl(host->base + MMC_CTYPE);
			tmp_reg &= ~CARD_WIDTH;
			writel(tmp_reg, host->base + MMC_CTYPE);
			g_bitmode = MMC_BUS_WIDTH_1;
		}
		msleep(5);
	}

	balong_sdio_trace(WIFI_DEBUG_DEBUG, "ios->clock = %d ", ios->clock);

    printk("[balong_sdio_set_ios]:ios->clock = %d ", ios->clock);

	if((ios->clock > MMC1_CCLK_MAX) || (ios->clock < MMC1_CCLK_MIN))
	{
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "Error!!! Invalid clock:%d",ios->clock);
		return;
	}

	if (ios->clock != g_clock)
	{
		g_clock = ios->clock;
	
		if (ios->clock) 
		{
			balong_sdio_control_cclk(host, DISABLE);
			balong_sdio_set_cclk(host, ios->clock);
			balong_sdio_control_cclk(host, ENABLE);
		}
		else
		{
			balong_sdio_control_cclk(host, DISABLE);
		}

		msleep(20);
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
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
static int balong_sdio_get_ro(struct mmc_host *mmc)
{
	int ret;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");
	ret = sys_ctrl_balong_sdio_card_readonly_io();
	balong_sdio_trace(WIFI_DEBUG_INFO, "end");

	return ret;
}

/*******************************************************************************
  Function:      static void balong_sdio_enable_sdio_irq(struct mmc_host *host, int enable)
  Description:   sdio interrupt ctl
  Input:         mmc:mmc_host struct
                 enable:
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
static void balong_sdio_enable_sdio_irq(struct mmc_host *host, int enable)
{
	struct balong_sdio_host *balong_host = mmc_priv(host);
	unsigned int reg_value;

	balong_sdio_print(WIFI_DEBUG_INFO, "begin\n");

	if (enable)
	{
		balong_sdio_print(WIFI_DEBUG_INFO, "begin ----------------- enable irq\n");

		/* enable sdio irq */
		reg_value = readl(balong_host->base + MMC_INTMASK);
		reg_value |= 0x10000;
		writel(reg_value, balong_host->base + MMC_INTMASK);
		g_intrkmode = 1;
	}
	else
	{
		balong_sdio_print(WIFI_DEBUG_INFO, "begin ----------------- disable irq\n");
		reg_value = readl(balong_host->base + MMC_INTMASK);
		reg_value &= ~0xffff0000;
		writel(reg_value, balong_host->base + MMC_INTMASK);
		g_intrkmode = 0;
	}
	balong_sdio_print(WIFI_DEBUG_INFO, "end\n");
}

/*******************************************************************************
  Function:      static int hi_sdio_get_cd(struct mmc_host *mmc)
  Description:   get cart status
  Input:         NA
                 NA
  Output:        NA
  Return:        1 insert 0 out 
  Others:        NA
*******************************************************************************/
static int balong_sdio_get_cd(struct mmc_host *mmc)
{
	balong_sdio_trace(WIFI_DEBUG_INFO, "begin\n"); 
	if (sdio_getcd)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	balong_sdio_trace(WIFI_DEBUG_INFO, "end\n");
}

static const struct mmc_host_ops balong_sdio_ops = {
	.request			= balong_sdio_request,
	.set_ios			= balong_sdio_set_ios,
	.get_ro			= balong_sdio_get_ro,
	.enable_sdio_irq	= balong_sdio_enable_sdio_irq,
	.get_cd			= balong_sdio_get_cd,
};

extern void __iomem *g_edmac_base_addr;
static ssize_t balong_sdio_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	char *command = NULL;
	char *reg = NULL;
	UINT32 r_value = 0;
	UINT32 r_address = 0;
	UINT32 r_offset = 0;
	UINT32 r_base = 0;

	command = kzalloc(32, GFP_KERNEL);
	if (NULL == command)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR, "malloc command failed\n");
		return (ssize_t)count;
	}

	reg = kzalloc(32, GFP_KERNEL);
	if (NULL == reg)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR, "malloc reg failed\n");
		kfree(command);
		return (ssize_t)count;
	}

	/* get command, register and value from buf */
	sscanf(buf, "%s %s %x", command, reg, &r_value);
	balong_sdio_trace(WIFI_DEBUG_ERROR, "command=%s  reg=%s  r_value=%x\n", command, reg, r_value);

	if (*command == 'h')
	{
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s ------------ begin sdio test\n", __func__);
		gpio_direction_output(BALONG_GPIO_WIFI_RESET, 1);
		gpio_direction_output(BALONG_GPIO_WIFI_RESET, 0);
		gpio_direction_output(BALONG_GPIO_WIFI_RESET, 1);
		mdelay(40);
		//balong_sdio_bus_init(sdio_host_copy);
		kfree(command);
		kfree(reg);
		return (ssize_t)count;
	}
	else if (*command == 'l')
	{
		gpio_direction_output(BALONG_GPIO_WIFI_RESET, 0);
		mdelay(40);
	}

	if (*command == 'a')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old dma_debug:%d new dma_debug:%d\n", dma_debug,r_value);
			dma_debug = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current dma_debug:%d\n", dma_debug);
		}
	}

	if (*command == 'v')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old dma_debug_1:%d new dma_debug_1:%d\n", dma_debug_1,r_value);
			dma_debug_1 = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current dma_debug_1:%d\n", dma_debug_1);
		}
	}

	if (*command == 'b')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old before_dma_delay:%d new before_dma_delay:%d\n", before_dma_delay,r_value);
			before_dma_delay = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current before_dma_delay:%d\n", before_dma_delay);
		}
	}

	if (*command == 'i')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old after_dma_delay:%d new after_dma_delay:%d\n", after_dma_delay,r_value);
			after_dma_delay = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current after_dma_delay:%d\n", after_dma_delay);
		}
	}
	
	if (*command == 'x')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old burst:%d new burst:%d\n", burst,r_value);
			burst = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current burst:%d\n", burst);
		}
	}

	if (*command == 'y')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old rx_water:%d new rx_water:%d\n", rx_water,r_value);
			rx_water = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current rx_water:%d\n", rx_water);
		}
	}

	if (*command == 'z')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old tx_water:%d new tx_water:%d\n", tx_water,r_value);
			tx_water = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current tx_water:%d\n", tx_water);
		}
	}
	
	if (*command == 'd')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old debug level:%d new debug level:%d\n", balong_sdio_trace_level,r_value);
			balong_sdio_trace_level = r_value;
		}
		else//show current debug level
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "current debug level:%d\n", balong_sdio_trace_level);
		}
	}

	if (*command == 'e')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old debug flag:%d new debug flag:%d\n", debug_flag,r_value);
			debug_flag = r_value;
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old debug flag:%d \n", debug_flag);
		}
	}

	if (*command == 'f')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_hold flag:%d new using_hold flag:%d\n", using_hold,r_value);
			using_hold = r_value;
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_hold flag:%d \n", using_hold);
		}
	}

	if (*command == 'g')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old is_high_clock flag:%d new is_high_clock flag:%d\n", is_high_clock,r_value);
			is_high_clock = r_value;
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old is_high_clock flag:%d \n", is_high_clock);
		}
	}

	if (*command == 'u')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_internal_dma:%d new using_internal_dma:%d\n", using_internal_dma,r_value);
			using_internal_dma = r_value;
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old is_high_clock flag:%d \n", using_internal_dma);
		}
	}

	if (*command == 'o')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_internal_dma_only:%d new using_internal_dma_only:%d\n", using_internal_dma_only,r_value);
			using_internal_dma_only = r_value;
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_internal_dma_only:%d \n", using_internal_dma_only);
		}
	}

	if (*command == 'c')
	{
		/*set debug level*/
		if(reg != 0)
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_internal_dma_cpu:%d new using_internal_dma_cpu:%d\n", using_internal_dma_cpu,r_value);
			using_internal_dma_cpu = r_value;
		}
		else
		{
			balong_sdio_trace(WIFI_DEBUG_ERROR, "old using_internal_dma_cpu:%d \n", using_internal_dma_cpu);
		}
	}
	/* convert register address string to hex int and get offset address */
	sscanf((reg + 2), "%08x", &r_address);
	r_offset = r_address & 0xfff;

	if ((r_address & 0xfffff000) == SCTL_BASE)
	{
		r_base = SCTL_BASE_ADDR;
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s ------------ system control register r_base=0x%x\n", __func__, r_base);
	}
	else if ((r_address & 0xfffff000) == HI_MCI_BASE1)
	{
		r_base = (UINT32)sdio_host_copy->base;
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s ------------ MMC1 register r_base=0x%x\n", __func__, r_base);
	}
	else if ((r_address & 0xfffff000) == 0x90181000)
	{
		r_base = g_edmac_base_addr;
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s ------------ MMC1 register r_base=0x%x\n", __func__, r_base);
	}
	else
	{
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s 22 ------------ not support register:0x%x\n", __func__, r_address);
		kfree(command);
		kfree(reg);
		return (ssize_t)count;
	}

	if (*command == 'r')
	{
		r_value = readl(r_base + r_offset);
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s ------------ read  r_address=0x%x  r_value=0x%x\n", __func__, r_address, r_value);
	}
	else if (*command == 'w')
	{
		balong_sdio_trace(WIFI_DEBUG_DEBUG, "%s ------------ write  r_address=0x%x  r_value=0x%x\n", __func__, r_address, r_value);
		writel(r_value, (r_base + r_offset));
	}
	
	kfree(command);
	kfree(reg);

	return (ssize_t)count;
}

static DEVICE_ATTR(balong_sdio, 0666, NULL, balong_sdio_store);

static struct attribute *balong_sdio_attributes[] = {
        &dev_attr_balong_sdio.attr,
	NULL
};

static const struct attribute_group balong_sdio_group = {
	.attrs = balong_sdio_attributes,
};


static int __devinit balong_sdio_probe(struct platform_device *pdev)
{
	struct mmc_host *mmc = NULL;
	struct balong_sdio_host *host = NULL;
	struct resource *res = NULL;
	int ret = 0;
	int irq = 0;
	unsigned int tempData = 0;

	balong_sdio_trace(1, "begin"); 

	/* create sys filesystem node for testing */
	ret = sysfs_create_group(&pdev->dev.kobj, &balong_sdio_group);
	if (0 != ret)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"create sdio sys filesystem node failed.\n");
		return -1;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	irq = platform_get_irq(pdev, 0);
	if (!res || irq < 0)
	{
		sysfs_remove_group(&pdev->dev.kobj, &balong_sdio_group);
		return  -ENXIO;
	}
	
	res = request_mem_region(res->start, SZ_4K, SDIO_DRIVER_NAME);
	if (!res)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"request_mem_region is error!");
		sysfs_remove_group(&pdev->dev.kobj, &balong_sdio_group);
		return -EBUSY;
	}
	
	mmc = mmc_alloc_host(sizeof(struct balong_sdio_host), &pdev->dev);
	if (!mmc) 
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"no mem for balong sdio host controller!");
		ret = -ENOMEM;
		goto out;
	}

	mmc->ops	= &balong_sdio_ops;
	mmc->f_min	= MMC1_CCLK_MIN;
	mmc->f_max	= MMC1_CCLK_MAX;
	mmc->caps	|= MMC_CAP_4_BIT_DATA | MMC_CAP_SDIO_IRQ | MMC_CAP_SD_HIGHSPEED;

	/* reload by this controller */
	mmc->max_blk_count	= 512;
	mmc->max_blk_size	= 65535;
	mmc->max_hw_segs	= 1024;
	mmc->max_phys_segs	= 1024;
	mmc->max_phys_segs	= mmc->max_hw_segs;
	mmc->max_req_size	= 65535;/* see IP manual */
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_34_35 | MMC_VDD_35_36; 
	mmc->ocr = mmc->ocr_avail;

	host = mmc_priv(mmc);

	host->mmc = mmc;
	host->irq = irq;
	host->base = ioremap_nocache(res->start, HI_MCI_IO_SIZE);
	if (!host->base)
	{
	 	balong_sdio_trace(WIFI_DEBUG_ERROR,"no mem for balong sdio base!");
	 	ret = -ENOMEM;
	 	goto out;
	}

	spin_lock_init(&host->lock);

	host->card_status = sys_ctrl_balong_sdio_card_detect_io();

	sdio_host_copy = (struct balong_sdio_host *)host;

	ret = request_irq(host->irq, balong_sdio_irq, 0, SDIO_DRIVER_NAME, host);

	platform_set_drvdata(pdev, mmc);

	sys_ctrl_balong_sdio_init_io();

	/* reset wifi */
	if (gpio_request(BALONG_GPIO_WIFI_RESET, "wifi-reset"))
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"request gpio is busy!");
		goto out;
	}
	gpio_direction_output(BALONG_GPIO_WIFI_RESET, 1);
    mdelay(100);	
	gpio_direction_output(BALONG_GPIO_WIFI_RESET, 0);
    mdelay(100);
	gpio_direction_output(BALONG_GPIO_WIFI_RESET, 1);
	mdelay(100);
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
 	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/	
	host->dma_req_num = MMC1_DMA_REQ_NUM;
	g_edma_free_list_node = (BALONG_DMA_CB *)dma_alloc_coherent(NULL,(EDMA_FREE_LIST_NODE_NUM * sizeof(BALONG_DMA_CB)),
                                                                  &g_edma_phy_addr, GFP_DMA|__GFP_WAIT);
	if (NULL == g_edma_free_list_node)
	{		
	     balong_sdio_trace(WIFI_DEBUG_ERROR,"LII list init is failed!"); 
	     return -ENOMEM;
	}
   	balong_sdio_trace(WIFI_DEBUG_DEBUG,"LII list virt_address = 0x%X,phy_addr = 0x%X!\n",(UINT32)g_edma_free_list_node,(UINT32)g_edma_phy_addr);
#else //using internal dma
	ret = IDMAC_Init(host->base);
	if(ret)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"internal dmac init failed!");
		goto out;
	}
#endif

	tempData = readl(host->base + HCON_OFF) ;
	tempData =  (tempData & 0x00000380)>>7;
       if(0 == tempData)
	{
		host->fifoWidth = 16;
       }
	else if(1 == tempData)
	{
		host->fifoWidth = 32;
	}
	else if(2 == tempData)
	{
		host->fifoWidth = 64;
	}
	
       host->fifoDepth =( (readl(host->base + MMC_FIFOTH)&0x0fff0000)>>16) + 1;

	balong_sdio_trace(WIFI_DEBUG_DEBUG, "FifoDepth:%d FifoWidth:%d\n",host->fifoDepth,host->fifoWidth);

	balong_sdio_bus_init(sdio_host_copy);
	balong_sdio_detectcard_to_core(1);
	mmc_add_host(mmc);

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");

	return 0;	
out:

	if (host)
	{
		if (host->base)
		{
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
			IDMAC_Exit(host->base);
#endif
			iounmap(host->base);
		}
	}
	if (mmc)
	{
		mmc_free_host(mmc);
	}
	sysfs_remove_group(&pdev->dev.kobj, &balong_sdio_group);

	return ret;
}

static int balong_sdio_suspend(struct platform_device *dev, pm_message_t state)
{
	int ret = 0;

	balong_sdio_trace(WIFI_DEBUG_DEBUG, "begin %d", sdio_getcd);

	balong_sdio_lock();

	if (1 == sdio_getcd)
	{   
		disable_irq(sdio_host_copy->irq);
		g_suspend = 1;
		sys_ctrl_balong_sdio_exit_io();
	}

	balong_sdio_trace(WIFI_DEBUG_INFO,"end");

	return ret;
}

static int balong_sdio_resume(struct platform_device *dev)
{
	int ret = 0;
	unsigned int tmp_reg = 0;

	balong_sdio_trace(WIFI_DEBUG_DEBUG,"begin %d",sdio_getcd);

	if (1 == sdio_getcd)
	{   	
		sys_ctrl_balong_sdio_init_io();

		balong_sdio_bus_init(sdio_host_copy);

		if (MMC_BUS_WIDTH_4 == g_bitmode)
		{
			tmp_reg = readl(sdio_host_copy->base + MMC_CTYPE);
			tmp_reg |= CARD_WIDTH;
			writel(tmp_reg, sdio_host_copy->base + MMC_CTYPE);	
		}
		else
		{
			tmp_reg = readl(sdio_host_copy->base + MMC_CTYPE);
			tmp_reg &= ~CARD_WIDTH;
			writel(tmp_reg, sdio_host_copy->base + MMC_CTYPE);
		}

		if (g_clock)
		{
			balong_sdio_control_cclk(sdio_host_copy, DISABLE);
			balong_sdio_set_cclk(sdio_host_copy, g_clock);
			balong_sdio_control_cclk(sdio_host_copy, ENABLE);
		}
		else
		{
			balong_sdio_control_cclk(sdio_host_copy, DISABLE);
		}

		msleep(20);
		if (g_intrkmode)
		{
			/* enable sdio irq */
			tmp_reg = readl(sdio_host_copy->base + MMC_INTMASK);
			tmp_reg |= 0x10000;
			writel(tmp_reg, sdio_host_copy->base + MMC_INTMASK);
		}

		enable_irq(sdio_host_copy->irq);

		if (sdio_host_copy->mmc)
		{
			//mmc_resume_host(sdio_host_copy->mmc);
		}
		g_suspend = 0;
	}

	balong_sdio_unlock(); 

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
	return ret;
}

static void balong_sdio_shutdown(struct platform_device *dev)
{	
	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");

	disable_irq(sdio_host_copy->irq);
	sys_ctrl_balong_sdio_exit_io();

	balong_sdio_trace(WIFI_DEBUG_INFO,"end");
}

static int __devexit balong_sdio_remove(struct platform_device *pdev)
{
	struct resource  *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	struct mmc_host *mmc  = platform_get_drvdata(pdev);
	struct balong_sdio_host *host = mmc_priv(mmc);

	balong_sdio_shutdown(pdev);

	mmc_free_host(mmc);

	free_irq(host->irq, host);
    /*++by pandong cs*/	
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
 	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
	if(g_edma_free_list_node != NULL)
	{
		dma_free_coherent(NULL,(EDMA_FREE_LIST_NODE_NUM * sizeof(BALONG_DMA_CB)),(void*)g_edma_free_list_node, g_edma_phy_addr);
    		balong_sdio_trace(WIFI_DEBUG_INFO,"LII list is released!");
	}
#endif

	iounmap(host->base);
	release_mem_region(res->start, SZ_4K);

	return 0;
}
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
 	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
static struct resource balong_sdio1_io_resources[] = {
	[0] = {
		.start	= HI_MCI_BASE1,     
		.end		= HI_MCI_BASE1 + HI_MCI_IO_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= HI_MCI_INTR1,
		.end		= HI_MCI_INTR1,
		.flags	= IORESOURCE_IRQ,
	},
};
    /*++by pandong cs*/
#elif defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && !defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
static struct resource balong_sdio1_io_resources[] = {
	[0] = {
		.start	= HI_MCI_BASE0,     
		.end		= HI_MCI_BASE0 + HI_MCI_IO_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= HI_MCI_INTR0,
		.end		= HI_MCI_INTR0,
		.flags	= IORESOURCE_IRQ,
	},
};
#else
#error "no valid platform!"
#endif

static void balong_sdio_platdev_release(struct device *dev)
{
}

static struct platform_device balong_sdio1_device = {
	.name		= SDIO_DRIVER_NAME,
	.id			= 1,
	.dev			= {
		.release	= balong_sdio_platdev_release,
	},
	.num_resources = ARRAY_SIZE(balong_sdio1_io_resources),
	.resource		= balong_sdio1_io_resources,
};

static struct platform_driver balong_sdio_driver = {
	.probe		= balong_sdio_probe,
	.remove		= __devexit_p(balong_sdio_remove),
	.shutdown	= balong_sdio_shutdown,
	.suspend		= balong_sdio_suspend,
	.resume		= balong_sdio_resume,
	.driver		= 
	{
		.name	= SDIO_DRIVER_NAME,
	},
};

static int __init balong_sdio_init(void)
{
	int ret = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO,"begin");
/*++by pandong cs*/
#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	#if defined(CS_SFT)
	#else
	g_iomg_base_wifi= ioremap_nocache(IOMG_BASE_ADDR,IOMG_REG_SIZE);
	/*配置GPIO复用*/
	writel(4,g_iomg_base_wifi+ 46*4);//WIFI1_SDIO_CMD
	writel(4,g_iomg_base_wifi+ 47*4);//WIFI1_SDIO_CLK
	writel(4,g_iomg_base_wifi+ 43*4);//WIFI1_SDIO_DATA0-4
	#endif
#endif
/*--by pandong cs*/
	spin_lock_init(&sdio_spinlock);
	
	/* interrupt involved */
	init_completion(&balong_sdio_data_complete);

	mutex_init(&sdio_mutex);

	ret = platform_device_register(&balong_sdio1_device);
	if (ret)
	{
		balong_sdio_trace(WIFI_DEBUG_ERROR,"Platform device 1 register is failed!");
		return ret;
	}

	ret = platform_driver_register(&balong_sdio_driver);
	if (ret)
	{
		platform_device_unregister(&balong_sdio1_device);
		balong_sdio_trace(WIFI_DEBUG_ERROR,"Platform driver register is failed!");
		return ret;
	}

	balong_sdio_trace(WIFI_DEBUG_DEBUG,"end");
	return ret;
}

static void __exit balong_sdio_exit(void)
{
	balong_sdio_trace(1,"begin");
/*++by pandong cs*/
#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	#if defined(CS_SFT)
	#else
	iounmap(g_iomg_base_wifi);
	#endif
#endif
/*--by pandong cs*/
	platform_driver_unregister(&balong_sdio_driver);
	platform_device_unregister(&balong_sdio1_device);

	balong_sdio_trace(1,"end");
}


module_init(balong_sdio_init);
module_exit(balong_sdio_exit);

MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("MMC/SDIO driver for the Hisilicon MMC/SD Host Controller");
MODULE_LICENSE("GPL");

