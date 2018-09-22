
#include <asm/io.h>
#include <linux/spinlock_types.h>
#include <mach/hardware.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>

#ifndef MMC1_SYSTEM_CTRL
#define MMC1_SYSTEM_CTRL
#endif

#include "hisdio_sys_ctrl.h"
#include "hisdio.h"
#include "wifi_printf.h"

extern spinlock_t sdio_spinlock;
    /*++by pandong cs*/
#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2) || \
	defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
	/*--by pandong cs*/
/*******************************************************************************
  Function:	static void sys_ctrl_balong_sdio_reset_ip(void)
  Description:	reset MMC1 host controle
  Input:		NA
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void sys_ctrl_balong_sdio_reset_ip(void)
{
	set_bit(MMC1_SRST, (void *)(SCTL_SC_PERCTRL0_RESTEN0));
	udelay(10);
}

/*******************************************************************************
  Function:	static void sys_ctrl_balong_sdio_unreset_ip(void)
  Description:	unreset MMC1 host controle
  Input:		NA
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void sys_ctrl_balong_sdio_unreset_ip(void)
{
	set_bit(MMC1_SRST, (void *)(SCTL_SC_PERCTRL0_RESTDIS0));
	udelay(10);
}


/*******************************************************************************
  Function:	static void balong_sdio_clock_ctl(int fg)
  Description:	sdio clock ctl
  Input:		fg:0 close sdio clock,1 open sdio clock
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void balong_sdio_clock_ctl(int fg)
{
	unsigned int  count = TRY_COUNT;
	int rty_count = TRY_COUNT >> 3;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");
	if (fg)
	{
		while (rty_count > 0)
		{
			count = TRY_COUNT;
			set_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKEN1));

			while (!test_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && (count > 0))
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
	else
	{
		while (rty_count > 0)
		{
			count   = TRY_COUNT;
			set_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKDIS1));

			while (test_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && (count > 0))
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
		hisdio_error("sdio clock operate is  error.\r\n");
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
}


/*******************************************************************************
  Function:      static void balong_wlan_clock_ctl(int fg)
  Description:   wlan clock ctl 
  Input:         fg:0 close  clock,1 open  clock
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void balong_wlan_clock_ctl(int fg)
{
	unsigned int tmp_reg = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");

	if (fg)
	{
		/*set mmc1 div clock*/
		spin_lock(&sdio_spinlock);
		tmp_reg = readl(SCTL_SC_PERCTRL);
		tmp_reg &= ~(MMC1_DIV << MMC1_DIV_BIT);
		tmp_reg |= (MMC1_DIV_NUM << MMC1_DIV_BIT);
		writel(tmp_reg, SCTL_SC_PERCTRL);
		spin_unlock(&sdio_spinlock);

		/*enable clock*/
		balong_sdio_clock_ctl(1);

		/*reset mmc1 ip*/
		sys_ctrl_balong_sdio_reset_ip();

		sys_ctrl_balong_sdio_unreset_ip();
	}
	else
	{
		balong_sdio_clock_ctl(0);
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
}
EXPORT_SYMBOL(balong_wlan_clock_ctl);

void sys_ctrl_balong_sdio_exit_io(void)
{
	return;
}

void sys_ctrl_balong_sdio_init_io(void)
{
	unsigned int reg;

	balong_sdio_trace(WIFI_DEBUG_INFO, "WIFI SDIO INIT begin");

	/*pin_config(IOCFG_BASE);*/

	reg = readl(SCTL_SC_PPLL);
	reg |=0x1;
	writel(reg, SCTL_SC_PPLL);

	balong_wlan_clock_ctl(1);

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");

	return ;
}

void sys_ctrl_balong_sdio_power_io(unsigned int flag)
{
	return;
}

unsigned int sys_ctrl_balong_sdio_card_detect_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}

unsigned int sys_ctrl_balong_sdio_card_readonly_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}
#elif ((defined (BOARD_ASIC)||defined(BOARD_SFT)) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
/*SDIO 使用MMC2，调试时需要重新配置*/
/*******************************************************************************
  Function:	static void sys_ctrl_balong_sdio_reset_ip(void)
  Description:	reset MMC1 host controle
  Input:		NA
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void sys_ctrl_balong_sdio_reset_ip(void)
{
	set_bit(MMC1_SRST, (void *)(SCTL_SC_PERCTRL0_RESTEN0));
	udelay(10);
}

/*******************************************************************************
  Function:	static void sys_ctrl_balong_sdio_unreset_ip(void)
  Description:	unreset MMC1 host controle
  Input:		NA
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void sys_ctrl_balong_sdio_unreset_ip(void)
{
	set_bit(MMC1_SRST, (void *)(SCTL_SC_PERCTRL0_RESTDIS0));
	udelay(10);
}


/*******************************************************************************
  Function:	static void balong_sdio_clock_ctl(int fg)
  Description:	sdio clock ctl
  Input:		fg:0 close sdio clock,1 open sdio clock
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void balong_sdio_clock_ctl(int fg)
{
	unsigned int  count = TRY_COUNT;
	int rty_count = TRY_COUNT >> 3;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");   /*lint !e730 !e746*/ 
	if (fg)
	{
		while (rty_count > 0)
		{
			count = TRY_COUNT;
			set_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKEN1));

			while (!test_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && (count > 0))
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
	else
	{
		while (rty_count > 0)
		{
			count   = TRY_COUNT;
			set_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKDIS1));

			while (test_bit(MMC1_CLK_BIT, (void *)(SCTL_SC_CLKSTAT1)) && (count > 0))
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
		hisdio_error("sdio clock operate is  error.\r\n");
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");   /*lint !e730*/
}


/*******************************************************************************
  Function:      static void balong_wlan_clock_ctl(int fg)
  Description:   wlan clock ctl 
  Input:         fg:0 close  clock,1 open  clock
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void balong_wlan_clock_ctl(int fg)
{
	unsigned int tmp_reg = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");   /*lint !e730*/

	if (fg)
	{
		/*set mmc1 div clock*/
		spin_lock(&sdio_spinlock);
		tmp_reg = readl(SCTL_SC_PERCTRL);
        /*Error 63: (Error -- Expected an lvalue)*/
		(int)tmp_reg &= ~(MMC1_DIV << MMC1_DIV_BIT);/*lint !e63 !e701*/
		tmp_reg |= (MMC1_DIV_NUM << MMC1_DIV_BIT);/*lint !e701*/
		writel(tmp_reg, SCTL_SC_PERCTRL);
		spin_unlock(&sdio_spinlock);

		/*enable clock*/
		balong_sdio_clock_ctl(1);

		/*reset mmc1 ip*/
		sys_ctrl_balong_sdio_reset_ip();

		sys_ctrl_balong_sdio_unreset_ip();
	}
	else
	{
		balong_sdio_clock_ctl(0);
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");    /*lint !e730*/
}
EXPORT_SYMBOL(balong_wlan_clock_ctl);

void sys_ctrl_balong_sdio_exit_io(void)
{
	return;
}

void sys_ctrl_balong_sdio_init_io(void)
{
	unsigned int reg;

	balong_sdio_trace(WIFI_DEBUG_INFO, "WIFI SDIO INIT begin"); /*lint !e730*/

	/*pin_config(IOCFG_BASE);*/

	reg = readl(SCTL_SC_PPLL);
	reg |=0x1;
	writel(reg, SCTL_SC_PPLL);

	balong_wlan_clock_ctl(1);

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");     /*lint !e730*/

	return ;
}/*lint !e550*/

void sys_ctrl_balong_sdio_power_io(unsigned int flag)
{
	return;
}

unsigned int sys_ctrl_balong_sdio_card_detect_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}

unsigned int sys_ctrl_balong_sdio_card_readonly_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}
#else //V7R1 UDP
/*******************************************************************************
  Function:	static void sys_ctrl_balong_sdio_reset_ip(void)
  Description:	reset MMC1 host controle
  Input:		NA
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void sys_ctrl_balong_sdio_reset_ip(void)
{
	set_bit(MMC0_SRST, (void *)(SCTL_CGR_CTRL14));
	udelay(10);
}

/*******************************************************************************
  Function:	static void sys_ctrl_balong_sdio_unreset_ip(void)
  Description:	unreset MMC1 host controle
  Input:		NA
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void sys_ctrl_balong_sdio_unreset_ip(void)
{
	clear_bit(MMC0_SRST, (void *)(SCTL_CGR_CTRL14));
	udelay(10);
}


/*******************************************************************************
  Function:	static void balong_sdio_clock_ctl(int fg)
  Description:	sdio clock ctl
  Input:		fg:0 close sdio clock,1 open sdio clock
  Output:		NA
  Return:		NA
  Others:		NA
*******************************************************************************/
static void balong_sdio_clock_ctl(int fg)
{
	unsigned int  count = TRY_COUNT;
	int rty_count = TRY_COUNT >> 3;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");
	if (fg)
	{
		while (rty_count > 0)
		{
			count = TRY_COUNT;
			set_bit(MMC0_CLK_BIT, (void *)(SCTL_CGR_CTRL3));

			while (!test_bit(MMC0_CLK_BIT, (void *)(SCTL_CGR_STAT1)) && (count > 0))
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
	else
	{
		while (rty_count > 0)
		{
			count   = TRY_COUNT;
			set_bit(MMC0_CLK_BIT, (void *)(SCTL_CGR_CTRL4));

			while (test_bit(MMC0_CLK_BIT, (void *)(SCTL_CGR_STAT1)) && (count > 0))
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
		balong_sdio_trace(WIFI_DEBUG_ERROR,"sdio clock operate is  error.\r\n");
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
}

/*******************************************************************************
  Function:      static void balong_wlan_clock_ctl(int fg)
  Description:   wlan clock ctl 
  Input:         fg:0 close  clock,1 open  clock
  Output:        NA
  Return:        NA
  Others:        NA
*******************************************************************************/
void balong_wlan_clock_ctl(int fg)
{
	unsigned int tmp_reg = 0;

	balong_sdio_trace(WIFI_DEBUG_INFO, "begin");

	if (fg)
	{
		/*set mmc1 div clock*/
		spin_lock(&sdio_spinlock);
		tmp_reg = readl(SCTL_CGR_CTRL12);
		tmp_reg &= ~(0x1 << MMC_CLK_SRC_BIT);
		tmp_reg |= (MMC_CLK_SRC_384M << MMC_CLK_SRC_BIT);
		writel(tmp_reg, SCTL_CGR_CTRL12);

		tmp_reg = readl(SCTL_CGR_CTRL16);
		tmp_reg &= ~(MMC0_CLK_DIV_MASK << MMC0_CLK_DIV_BIT);
		tmp_reg |= (MMC0_CLK_DIV << MMC0_CLK_DIV_BIT);
		writel(tmp_reg, SCTL_CGR_CTRL16);
		spin_unlock(&sdio_spinlock);

		/*enable clock*/
		balong_sdio_clock_ctl(1);

		/*reset mmc1 ip*/
		sys_ctrl_balong_sdio_reset_ip();

		sys_ctrl_balong_sdio_unreset_ip();
	}
	else
	{
		balong_sdio_clock_ctl(0);
	}

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");
}

void sys_ctrl_balong_sdio_exit_io(void)
{
	return;
}

void sys_ctrl_balong_sdio_init_io(void)
{
	unsigned int reg;

	balong_sdio_trace(WIFI_DEBUG_INFO, "WIFI SDIO INIT begin");

	/*pin_config(IOCFG_BASE);*/
	spin_lock(&sdio_spinlock);
	clear_bit(IOS_GMII_CTRL_BIT,SCTL_IOS_CTRL98);
	set_bit(IOS_MMC0_CTRL_BIT,SCTL_IOS_CTRL98);
	clear_bit(IOS_MMC0_CTRL1_BIT,SCTL_IOS_CTRL98);

    reg = readl(SCTL_IOS_CTRL83);
    printk("%s reg:0x%x  1\n",__func__,reg);
    reg = (reg & ~(0xFC)) | (0x3C);
    printk("%s reg:0x%x  2\n",__func__,reg);
    writel(reg,SCTL_IOS_CTRL83);
    
	spin_unlock(&sdio_spinlock);
	
	balong_wlan_clock_ctl(1);

	balong_sdio_trace(WIFI_DEBUG_INFO, "end");

	return ;
}

void sys_ctrl_balong_sdio_power_io(unsigned int flag)
{
	return;
}

unsigned int sys_ctrl_balong_sdio_card_detect_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}

unsigned int sys_ctrl_balong_sdio_card_readonly_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}
#endif

