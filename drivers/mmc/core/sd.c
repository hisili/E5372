/*
 *  linux/drivers/mmc/core/sd.c
 *
 *  Copyright (C) 2003-2004 Russell King, All Rights Reserved.
 *  SD support Copyright (C) 2004 Ian Molton, All Rights Reserved.
 *  Copyright (C) 2005-2007 Pierre Ossman, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>

#include <linux/string.h>   
#include <linux/kernel.h>   
#include <linux/syscalls.h>   
   
#include <asm/uaccess.h>      
#include <asm/unistd.h>      
#include <asm/fcntl.h>       
#include <linux/mmc/sd.h>
#include "SDUpdate.h"/*l00205892  for sd_update*/
#include "core.h"
#include "bus.h"
#include "mmc_ops.h"
#include "sd_ops.h"
#include "BSP.h"
#include "../../nvim/balong_nvim.h"
#include <linux/proc_fs.h>

#include "generated/FeatureConfigDRV.h"

/*at^sd interface 2012-01-16 begin*/
u32 sdmmcATProcessOverFlag = 0;
SDMMC_ATPROCESS_TASK_ARG task_arg = {0};
#if (FEATURE_ON == HUAWEI_PRODUCT_E5372S_32)
#define CONFIG_MMC_PARANOID_SD_INIT
#endif

/*at^sd interface end*/
/*g_inFatChecking: TRUE=sdupdate; FALSE=is not sdupdate.*/
extern BOOL g_inFatChecking; 
extern BSP_S32 DR_NV_Read(BSP_U16 usID,BSP_VOID *pItem,BSP_U32 ulLength);
extern int hi_TFUP_getTFUpdateFlag(void);
extern int hi_TFUP_GetGoOnFlag(void);

extern int BSP_PWRCTRL_SD_LowPowerExit(void);
extern int BSP_PWRCTRL_SD_LowPowerEnter(void);

#ifdef CONFIG_MMC_BALONG
extern int hi_sdio_voltage_switch_check(struct mmc_host* mmc);
extern void hi_sdio_control_uhs_ddr_volt(struct mmc_host* mmc,unsigned int ulDDRMode,unsigned int ulVoltChocie);
extern void hi_sdio_switch_drive_current(int isUHSIflag); /*y00186965*//*lint !e752*/
#else
int hi_sdio_voltage_switch_check(struct mmc_host* mmc)
{
	return -1;
}
void hi_sdio_control_uhs_ddr_volt(struct mmc_host* mmc,unsigned int ulDDRMode,unsigned int ulVoltChocie)
{
	return ;
}
#endif
volatile u32 g_ulUhsSupport = 0;  /*UHS-1卡标志位*/   

#define NV_MMI_TEST               114
#define NV_MMI_TEST_SIZE          78
#define NV_MMI_TEST_OFFSET        24
#define MMI_TEST_SUCCESS         "ST P"
#if (FEATURE_OLED == FEATURE_ON)
extern void oledClearWholeScreen(BSP_VOID);
extern void oledStringDisplay(u8 ucX, u8 ucY, u8 *pucStr);
#elif (FEATURE_TFT == FEATURE_ON)
extern void tftClearWholeScreen(void);
extern void tftStringDisplay(u8 ucX, u8 ucY, u8 *pucStr);
#endif

struct mmc_host *g_host = NULL;

static const unsigned int tran_exp[] = {
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const unsigned int tacc_exp[] = {
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*judge the UHS-1 card*/

int mmc_judge_uhs(void)   
{
    if ( 1 == g_ulUhsSupport)
    {
        return SD_SUPPORT_UHS;
    }
    else
    {
        return SD_NO_SUPPORT_UHS;
    }
} 
        
//#define DEBUG_SD30_TEST  /*y00186965*/

#ifdef DEBUG_SD30_TEST  /*tiaoshi*/

 extern balong_break_point(void);

 volatile u32 g_max_fre = 0;
int set_fre(int value)
{
    g_max_fre = value;
    printk("set_fre is %d",g_max_fre);
}

int mmc_set_uhs(void)
{
    g_ulUhsSupport = 1;
    printk("g_ulUhsSupport = %d\n",g_ulUhsSupport);
}
int mmc_get_uhs()
{
    printk("g_ulUhsSupport = %d\n",g_ulUhsSupport);

}

volatile u8  g_ucSwitchGroup0 = 2;
volatile u8 g_ucSwitchGroup2 = 0;
volatile u8 g_ucSwitchGroup3 = 0;

void set_switch_value(u8 value1,u8 value2,u8 value3)
{
    g_ucSwitchGroup0 = value1;
    g_ucSwitchGroup2 = value2;
    g_ucSwitchGroup3 = value3;
    printk("g_ucSwitchGroup0=%d,g_ucSwitchGroup2=%d,g_ucSwitchGroup3=%d\n",g_ucSwitchGroup0,g_ucSwitchGroup2,g_ucSwitchGroup3);
    schedule();
}

#endif        

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static void mmc_decode_cid(struct mmc_card *card)
{
    u32 *resp = card->raw_cid;

	memset(&card->cid, 0, sizeof(struct mmc_cid));

    /*
     * SD doesn't currently have a version field so we will
     * have to assume we can parse this.
     */
    card->cid.manfid        = UNSTUFF_BITS(resp, 120, 8);
    card->cid.oemid            = UNSTUFF_BITS(resp, 104, 16);
    card->cid.prod_name[0]        = UNSTUFF_BITS(resp, 96, 8);
    card->cid.prod_name[1]        = UNSTUFF_BITS(resp, 88, 8);
    card->cid.prod_name[2]        = UNSTUFF_BITS(resp, 80, 8);
    card->cid.prod_name[3]        = UNSTUFF_BITS(resp, 72, 8);
    card->cid.prod_name[4]        = UNSTUFF_BITS(resp, 64, 8);
    card->cid.hwrev            = UNSTUFF_BITS(resp, 60, 4);
    card->cid.fwrev            = UNSTUFF_BITS(resp, 56, 4);
    card->cid.serial        = UNSTUFF_BITS(resp, 24, 32);
    card->cid.year            = UNSTUFF_BITS(resp, 12, 8);
    card->cid.month            = UNSTUFF_BITS(resp, 8, 4);

	card->cid.year += 2000; /* SD cards year offset */
}/*lint !e529*/

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static int mmc_decode_csd(struct mmc_card *card)
{
    struct mmc_csd *csd = &card->csd;
    unsigned int e, m, csd_struct;
    u32 *resp = card->raw_csd;

    csd_struct = UNSTUFF_BITS(resp, 126, 2);

    switch (csd_struct) {
    case 0:
        m = UNSTUFF_BITS(resp, 115, 4);
        e = UNSTUFF_BITS(resp, 112, 3);
        csd->tacc_ns     = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
        csd->tacc_clks     = UNSTUFF_BITS(resp, 104, 8) * 100;

        m = UNSTUFF_BITS(resp, 99, 4);
        e = UNSTUFF_BITS(resp, 96, 3);
        csd->max_dtr      = tran_exp[e] * tran_mant[m];
        csd->cmdclass      = UNSTUFF_BITS(resp, 84, 12);

        e = UNSTUFF_BITS(resp, 47, 3);
        m = UNSTUFF_BITS(resp, 62, 12);
        csd->capacity      = (1 + m) << (e + 2);

        csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
        csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
        csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
        csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
        csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
        csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
        csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
        break;
    case 1:
        /*
         * This is a block-addressed SDHC card or SDXC card. Most
         * interesting fields are unused and have fixed
         * values. To avoid getting tripped by buggy cards,
         * we assume those fixed values ourselves.
         */
        mmc_card_set_blockaddr(card);  /*syb*/  /*设置SDHC标志*/

        csd->tacc_ns     = 0; /* Unused */
        csd->tacc_clks     = 0; /* Unused */

        m = UNSTUFF_BITS(resp, 99, 4);
        e = UNSTUFF_BITS(resp, 96, 3);
        csd->max_dtr      = tran_exp[e] * tran_mant[m];
        csd->cmdclass      = UNSTUFF_BITS(resp, 84, 12);

        printk("sd3.0:mmc_decode_csd:csd->max_dtr = %d,m=%d,e=%d\n",csd->max_dtr,m,e);

        if ( csd->max_dtr == 200000000) 
        {
            /*设置UHS104标志*/
            mmc_card_set_uhs104(card);
        }

        m = UNSTUFF_BITS(resp, 48, 22);
        csd->capacity     = (1 + m) << 10;

        if (m > 0xFFFF)  /*设置SDXC标志*/
        {
            /*设置SDXC标志*/
            mmc_card_set_sdxc(card);
        }

        csd->read_blkbits = 9;
        csd->read_partial = 0;
        csd->write_misalign = 0;
        csd->read_misalign = 0;
        csd->r2w_factor = 4; /* Unused */
        csd->write_blkbits = 9;
        csd->write_partial = 0;
        break;
    default:
        printk(KERN_ERR "%s: unrecognised CSD structure version %d\n",
            mmc_hostname(card->host), csd_struct);
        return -EINVAL;
    }

    return 0;
}/*lint !e529*/

/*
 * Given a 64-bit response, decode to our card SCR structure.
 */
static int mmc_decode_scr(struct mmc_card *card)
{
    struct sd_scr *scr = &card->scr;
    unsigned int scr_struct;
    u32 resp[4];

    resp[3] = card->raw_scr[1];
    resp[2] = card->raw_scr[0];

    scr_struct = UNSTUFF_BITS(resp, 60, 4);
    if (scr_struct != 0) {
        printk(KERN_ERR "%s: unrecognised SCR structure version %d\n",
            mmc_hostname(card->host), scr_struct);
        return -EINVAL;
    }

    scr->sda_vsn = UNSTUFF_BITS(resp, 56, 4);
    scr->bus_widths = UNSTUFF_BITS(resp, 48, 4);
    scr->sda_vsn3  = UNSTUFF_BITS(resp, 47, 1);  

    return 0;
}/*lint !e550*/

/*
 * Fetches and decodes switch information
 */


static int mmc_read_switch(struct mmc_card *card)
{
	int err;
	u8 *status;
	u8 ucSwitchGroup0=0; /* pc-lint 644*/
	u8 ucSwitchGroup2;
	u8 ucSwitchGroup3;

	if (card->scr.sda_vsn < SCR_SPEC_VER_1)
		return 0;

	if (!(card->csd.cmdclass & CCC_SWITCH)) {
		printk(KERN_WARNING "%s: card lacks mandatory switch "
			"function, performance might suffer.\n",
			mmc_hostname(card->host));
		return 0;
	}

	err = -EIO;

	status = kmalloc(64, GFP_KERNEL);
	if (!status) {
		printk(KERN_ERR "%s: could not allocate a buffer for "
			"switch capabilities.\n", mmc_hostname(card->host));
		return -ENOMEM;
	}
    /*区分UHS和NO_UHS card*/   /*syb*/    /*具体驱动能力及电流需要确定 */ 
    if ( SD_SUPPORT_UHS == mmc_judge_uhs())
    {   
        if (mmc_card_uhs104(card))
        {
			ucSwitchGroup0 = 2;  /*芯片确认使用SDR50,SDR104不支持，driver and limit 需要确定*/
			ucSwitchGroup2 = 0;
			ucSwitchGroup3 = 0;
			err  = mmc_sd_switch(card, 0, 2, ucSwitchGroup2, status);
			printk("sd3.0:mmc_read_switch_dirver:status[15]=0x%x,status[25]=0x%x\n",status[15],status[25]);
			err |= mmc_sd30_switch(card,0,ucSwitchGroup0,0xf,ucSwitchGroup3,status);
			printk("sd3.0:mmc_read_switch:status[13]=0x%x,status[9]=0x%x,status[7]=0x%x\n",status[13],status[9],status[7]);
			printk("sd3.0:mmc_read_switch:status[16]=0x%x,status[29]=0x%x\n",status[16],status[29]);
			printk("sd3.0:mmc_read_switch:status[15]=0x%x,status[25]=0x%x,status[23]=0x%x\n",status[15],status[25],status[23]);
			printk("status[13]_400,status[9]_422,status[7]_448,status[16]_376,status[15]_384\n");
        }
        else
        {
    	#ifdef DEBUG_SD30_TEST
			ucSwitchGroup0 = g_ucSwitchGroup0;  /*tiaoshi*/  
			ucSwitchGroup2 = g_ucSwitchGroup2;
			ucSwitchGroup3 = g_ucSwitchGroup3;
		#else			
			ucSwitchGroup0 = 2; 
			ucSwitchGroup2 = 0; 
			ucSwitchGroup3 = 0; 
		#endif
			err  = mmc_sd_switch(card, 0, 2, ucSwitchGroup2, status);
			printk("sd3.0:mmc_read_switch_dirver:status[15]=0x%x,status[25]=0x%x\n",status[15],status[25]);
			err |= mmc_sd30_switch(card,0,ucSwitchGroup0,0xf,ucSwitchGroup3,status);
			printk("sd3.0:mmc_read_switch:status[13]=0x%x,status[9]=0x%x,status[7]=0x%x\n",status[13],status[9],status[7]);
			printk("sd3.0:mmc_read_switch:status[16]=0x%x,status[29]=0x%x\n",status[16],status[29]);
			printk("sd3.0:mmc_read_switch:status[15]=0x%x,status[25]=0x%x,status[23]=0x%x\n",status[15],status[25],status[23]);
			printk("status[13]_400,status[9]_422,status[7]_448,status[16]_376,status[15]_384\n");
        }

    }
    else
    {
        err = mmc_sd_switch(card, 0, 0, 1, status);
    }
    
	
	if (err) {
		/* If the host or the card can't do the switch,
		 * fail more gracefully. */
		if ((err != -EINVAL)
		 && (err != -ENOSYS)
		 && (err != -EFAULT))
			goto out;

		printk(KERN_WARNING "%s: problem reading switch "
			"capabilities, performance might suffer.\n",
			mmc_hostname(card->host));
		err = 0;

		goto out;
	}
	/*查询需要设置的UHS模式是否支持，如果支持设置工作频率*/

	if ( SD_SUPPORT_UHS == mmc_judge_uhs())
	{
		if (4 == ucSwitchGroup0)
		{
			if (status[13] & 0x10)  /*DDR50*/
			{
				card->sw_caps.hs_max_dtr = 50000000;
				printk("sd3.0:mmc_read_switch:DDR50 support!\n");
			}
		}
		else if (3 == ucSwitchGroup0)
		{
			if (status[13] & 0x8)  /*SDR104*/
			{
				card->sw_caps.hs_max_dtr = 200000000;
				printk("sd3.0:mmc_read_switch:SDR104 support!\n");
			}
		}
		else if (2 == ucSwitchGroup0)
		{
			if (status[13] & 0x4)  /*SDR50*/
			{
				card->sw_caps.hs_max_dtr = 100000000;
				printk("sd3.0:mmc_read_switch:SDR50 support!\n");
			}
		}
		else if (1 == ucSwitchGroup0)
		{
			if (status[13] & 0x2)  /*SDR*/
			{
				card->sw_caps.hs_max_dtr = 50000000;
				printk("sd3.0:mmc_read_switch:SDR25 support!\n");
			}
		}
			
	}	
	else  /*查询high_speed是否支持，如果支持设置工作频率*/
	{
		if (status[13] & 0x02)
		{
			card->sw_caps.hs_max_dtr = 50000000;
			printk("sd2.0:mmc_read_switch:high_speed!\n");
		}
		
	}

out:
	kfree(status);

	return err;
}

/*
 * Test if the card supports high-speed mode and, if so, switch to it.
 */
static int mmc_switch_hs(struct mmc_card *card)
{
	int err;
	u8 *status;
	u8 ucSwitchGroup0=0; /*pc-lint 644*/
	u8 ucSwitchGroup2;
	u8 ucSwitchGroup3;

	if (card->scr.sda_vsn < SCR_SPEC_VER_1)
		return 0;

	if (!(card->csd.cmdclass & CCC_SWITCH))
		return 0;

	if (!(card->host->caps & MMC_CAP_SD_HIGHSPEED))
		return 0;

	if (card->sw_caps.hs_max_dtr == 0)
		return 0;

	err = -EIO;

	status = kmalloc(64, GFP_KERNEL);
	if (!status) {
		printk(KERN_ERR "%s: could not allocate a buffer for "
			"switch capabilities.\n", mmc_hostname(card->host));
		return -ENOMEM;
	}
    
    /*区分UHS和NO_UHS card*/   
    if ( SD_SUPPORT_UHS == mmc_judge_uhs())
    {   
        if (mmc_card_uhs104(card))
        {
        	printk("sd3.0:mmc_switch_hs:uhs104!\n");
			
			ucSwitchGroup0 = 2;
			ucSwitchGroup2 = 0;
			ucSwitchGroup3 = 0;
			/*switch driver strength*/
			err  = mmc_sd_switch(card, 1, 2, ucSwitchGroup2, status);
			printk("sd3.0:mmc_switch_hs_driver:status[15]=0x%x,status[25]=0x%x\n",status[15],status[25]);
			/*switch HUS_1 mode and limit ability*/
			err |= mmc_sd30_switch(card, 1,ucSwitchGroup0,0xff,ucSwitchGroup3,status);
			printk("sd3.0:mmc_switch_hs:status[13]=0x%x,status[9]=0x%x,status[7]_448=0x%x\n",status[13],status[9],status[7]);
			printk("sd3.0:mmc_switch_hs:status[16]=0x%x,status[29]=0x%x\n",status[16],status[29]);						
			printk("sd3.0:mmc_switch_hs:status[15]=0x%x,status[25]=0x%x,status[23]=0x%x\n",status[15],status[25],status[23]);
			printk("status[13]_400,status[9]_422,status[7]_448,status[16]_376,status[15]_384\n");
        }
        else
        {
        	printk("sd3.0:mmc_switch_hs:uhs50!\n");	
    	#ifdef DEBUG_SD30_TEST
			ucSwitchGroup0 = g_ucSwitchGroup0;  /*tiaoshi*/  
			ucSwitchGroup2 = g_ucSwitchGroup2;
			ucSwitchGroup3 = g_ucSwitchGroup3;
		#else
			ucSwitchGroup0 = 2;
			ucSwitchGroup2 = 0;
			ucSwitchGroup3 = 0;
		#endif
			/*switch driver strength*/
			err  = mmc_sd_switch(card, 1, 2, ucSwitchGroup2, status);
			printk("sd3.0:mmc_switch_hs_dirver:status[15]=0x%x,status[25]=0x%x\n",status[15],status[25]);
			/*switch HUS_1 mode and limit ability*/
			err |= mmc_sd30_switch(card,1,ucSwitchGroup0,0xf,ucSwitchGroup3,status);
			printk("sd3.0:mmc_switch_hs:status[13]_400=0x%x,status[9]_422=0x%x,status[7]_448=0x%x\n",status[13],status[9],status[7]);
			printk("sd3.0:mmc_switch_hs:status[16]_376=0x%x,status[29]=0x%x\n",status[16],status[29]);						
			printk("sd3.0:mmc_switch_hs:status[15]_384=0x%x,status[25]=0x%x,status[23]=0x%x\n",status[15],status[25],status[23]);
			
        }

    }
    else
    {
        err = mmc_sd_switch(card, 1, 0, 1, status);
    }

	if (err)
	{
		printk("sd3.0:mmc_switch_hs err is 0x%x!\n",err);
		goto out;
	}

	if ( SD_SUPPORT_UHS == mmc_judge_uhs())
	{
		if (4 == ucSwitchGroup0)
		{
			if ((status[16] & 0xF) != 4) {
				printk(KERN_WARNING "%s: Problem switching card "
					"into ddr50 mode!\n",
					mmc_hostname(card->host));
			} else {
				mmc_card_set_highspeed(card);
				mmc_card_set_uhsddr50(card); 				
				mmc_set_timing(card->host, MMC_TIMING_SD_HS);
				printk("sd3.0:mmc_switch_hs:DDR50 switch!\n");
			}
		}
		else if (3 == ucSwitchGroup0)
		{
			if ((status[16] & 0xF) != 3) {
				printk(KERN_WARNING "%s: Problem switching card "
					"into sdr104 mode!\n",
					mmc_hostname(card->host));
			} else {
				mmc_card_set_highspeed(card);				 
				mmc_set_timing(card->host, MMC_TIMING_SD_HS);
				printk("sd3.0:mmc_switch_hs:SDR104 switch!\n");
			}
		}
		else if (2 == ucSwitchGroup0)
		{
			if ((status[16] & 0xF) != 2) {
				printk(KERN_WARNING "%s: Problem switching card "
					"into sdr50 mode!\n",
					mmc_hostname(card->host));
			} else {
				mmc_card_set_highspeed(card);							 
				mmc_set_timing(card->host, MMC_TIMING_SD_HS);
				printk("sd3.0:mmc_switch_hs:SDR50 switch!\n");
			}
		}
		else if (1 == ucSwitchGroup0)
		{
			if ((status[16] & 0xF) != 1) {
				printk(KERN_WARNING "%s: Problem switching card "
					"into sdr25 mode!\n",
					mmc_hostname(card->host));
			} else {
				mmc_card_set_highspeed(card);				
				mmc_set_timing(card->host, MMC_TIMING_SD_HS);
				printk("sd3.0:mmc_switch_hs:SDR25 switch!\n");
			}
		}
			
	}
	else
	{
		if ((status[16] & 0xF) != 1) {
			printk(KERN_WARNING "%s: Problem switching card "
				"into high-speed mode!\n",
				mmc_hostname(card->host));
		} else {
			mmc_card_set_highspeed(card); 
			mmc_set_timing(card->host, MMC_TIMING_SD_HS);			
			printk("sd2.0 switch_hs:mmc_card_set_highspeed\n");
		}

	}


out:
	kfree(status);

	return err;
}

/*sdr50 sdr104 need to tuning*/
static int mmc_sd_switch_tuning(struct mmc_card *card)
{
	int err;
	int i = 0;
	u8 *status;
	err = -EIO;
	status = kmalloc(64, GFP_KERNEL);
	if (!status) {
		printk(KERN_ERR "%s: could not allocate a buffer for "
			"switch tuning.\n", mmc_hostname(card->host));
		return -ENOMEM;
	}
	for (i=0;i<40;i++)  
	{
		err = mmc_send_tuning_pattern(card,status);
		if (err)
		{
			continue;
		}
		else
		{
            kfree(status);
            status = NULL;
			return 0;
		}
	}
    kfree(status);
    status = NULL;
	return err;	

}

/*lint -e413*/
MMC_DEV_ATTR(cid, "%08x%08x%08x%08x\n", card->raw_cid[0], card->raw_cid[1],
	card->raw_cid[2], card->raw_cid[3]);
MMC_DEV_ATTR(csd, "%08x%08x%08x%08x\n", card->raw_csd[0], card->raw_csd[1],
	card->raw_csd[2], card->raw_csd[3]);
MMC_DEV_ATTR(scr, "%08x%08x\n", card->raw_scr[0], card->raw_scr[1]);
MMC_DEV_ATTR(date, "%02d/%04d\n", card->cid.month, card->cid.year);
MMC_DEV_ATTR(fwrev, "0x%x\n", card->cid.fwrev);
MMC_DEV_ATTR(hwrev, "0x%x\n", card->cid.hwrev);
MMC_DEV_ATTR(manfid, "0x%06x\n", card->cid.manfid);
MMC_DEV_ATTR(name, "%s\n", card->cid.prod_name);
MMC_DEV_ATTR(oemid, "0x%04x\n", card->cid.oemid);
MMC_DEV_ATTR(serial, "0x%08x\n", card->cid.serial);
/*lint +e413*/


static struct attribute *sd_std_attrs[] = {
	&dev_attr_cid.attr,
	&dev_attr_csd.attr,
	&dev_attr_scr.attr,
	&dev_attr_date.attr,
	&dev_attr_fwrev.attr,
	&dev_attr_hwrev.attr,
	&dev_attr_manfid.attr,
	&dev_attr_name.attr,
	&dev_attr_oemid.attr,
	&dev_attr_serial.attr,
	NULL,
};

static struct attribute_group sd_std_attr_group = {
	.attrs = sd_std_attrs,
};

static const struct attribute_group *sd_attr_groups[] = {
	&sd_std_attr_group,
	NULL,
};

static struct device_type sd_type = {
	.groups = sd_attr_groups,
};

/*
 * Handle the detection and initialisation of a card.
 *
 * In the case of a resume, "oldcard" will contain the card
 * we're trying to reinitialise.
 */


static int mmc_sd_init_card(struct mmc_host *host, u32 ocr,
	struct mmc_card *oldcard)
{
	struct mmc_card *card;
	int err;
	u32 cid[4];
	unsigned int max_dtr;
    u32 ulResp = 0; 
#ifdef CONFIG_MMC_PARANOID_SD_INIT
	int retries;
#endif
	BUG_ON(!host);          /*lint !e730*/
	WARN_ON(!host->claimed);

	/*
	 * Since we're changing the OCR value, we seem to
	 * need to tell some cards to go back to the idle
	 * state.  We wait 1ms to give cards time to
	 * respond.
	 */
	mmc_go_idle(host);

	/*
	 * If SD_SEND_IF_COND indicates an SD 2.0
	 * compliant card and we should set bit 30
	 * of the ocr to indicate that we can handle
	 * block-addressed SDHC cards.
	 */
	err = mmc_send_if_cond(host, ocr);
    
    printk("ACMD41 parameter is (0xff8000)0x%x! \n",ocr);  
	if (!err)
		ocr |= ((1 << 30)|(1 << 24));  

	err = mmc_send_app_op_cond(host, ocr, &ulResp); 

	printk("ACMD41 resp is 0x%x!\n",ulResp);
	
	if (err)
    {
        printk(" NO SD CARD !\n"); 
        goto err;
    }   
  		
     g_ulUhsSupport = 0;
     
    /*判断是否支持UHS-1卡*/  
    if (ulResp & (1 << 24))
    {
    	printk("UHS ENTER!\n");  
        /*switch to high-power mode*/        
        /*hi_sdio_control_cclk_low_power(hi_host,0);*/
        mmc_set_cclk_low_power(host,MMC_CCLK_NO_LOW_POWER_MODE);		

        /*发送CMD11切换到UHS_1模式*/        
        err = mmc_send_voltage_switch(host,&ulResp);
		printk("cmd11 r1 is 0x%x!\n",ulResp);
        if (err)
        {
            /*下电之前需要关闭时钟*/
            /*hi_sdio_control_cclk(host,~ENABLE);*/
            mmc_set_clock(host,0);
            /*power cycle*/
            mmc_power_off(host);
            printk("SD3.0 :switch UHS-I resp err!\n");
            goto err;            
        }  
      #if 0
        mdelay(2);
		cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
		if(cmd_irq_reg & HTO_INT_STATUS)
        {    
            /*clear the VOLT_SWITCH_INT  state*/
            set_bit(HTO_INT_NUM, host->base + MMC_RINTSTS);
            hi_sdio_control_cclk(hi_host,~ENABLE);
            hi_sdio_control_uhs_ddr_volt(hi_host,~DDR_MODE_CHOICE,VOLT_18V_CHOICE);
            mdelay(5);
            hi_sdio_control_cclk(hi_host,ENABLE);
            mdelay(1);
            /*check VOLT_SWITCH_INT and CD interrrpt*/
            cmd_irq_reg = ALL_INT_CLR & readl(host->base + MMC_RINTSTS);
            if((!(cmd_irq_reg & HTO_INT_STATUS))||(!(cmd_irq_reg & CD_INT_STATUS)) )
            {
                /*下电之前需要关闭时钟*/
                /*hi_sdio_control_cclk(host,~ENABLE);*/
                mmc_set_clock(host,0);
                /*power cycle*/
                mmc_power_off(host);
                printk("SD3.0: switch UHS-1 no volt_switch_int and cd!\n");
                goto err;   
            }
            else
            {
                /*clear  VOLT_SWITCH_INT and CD state*/
                set_bit(HTO_INT_NUM, host->base + MMC_RINTSTS);
                set_bit(CD_INT_NUM, host->base + MMC_RINTSTS);
                /*设置标志位，表示支持1.8V及是UHS-1卡*/
                g_ulUhsSupport = 1;
                
            }         
            
        }
        else
        {
            /*下电之前需要关闭时钟*/
            /*hi_sdio_control_cclk(host,0);*/
            mmc_set_clock(host,0);
            /*power cycle*/
            mmc_power_off(host);
            printk("SD3.0:NO VOLT SWITCH INT!\n");
            goto err;
        }
      #endif

        /*检查UHS_1模式是否切换成功*/
        err = hi_sdio_voltage_switch_check(host);
        if (err)
        {
            printk("SD3.0: hi_sdio_voltage_switch_check fail!\n");
            goto err;
        }
        /*设置标志位，表示支持1.8V及是UHS-1卡*/
        g_ulUhsSupport = 1;	
	
    } 

	/*
	 * Fetch CID from card.
	 */
	if (mmc_host_is_spi(host))
		err = mmc_send_cid(host, cid);
	else
		err = mmc_all_send_cid(host, cid);
		printk("***********cid =0x%x!\n",*cid);		
	if (err)
		goto err;

	if (oldcard) {
		if (memcmp(cid, oldcard->raw_cid, sizeof(cid)) != 0) {
			err = -ENOENT;
			goto err;
		}

		card = oldcard;
	} else {
		/*
		 * Allocate card structure.
		 */
		card = mmc_alloc_card(host, &sd_type);
		if (IS_ERR(card)) {
			err = PTR_ERR(card);
			goto err;
		}

		card->type = MMC_TYPE_SD;
		memcpy(card->raw_cid, cid, sizeof(card->raw_cid));
	}

	/*
	 * For native busses:  get card RCA and quit open drain mode.
	 */	
	if (!mmc_host_is_spi(host)) {
		err = mmc_send_relative_addr(host, &card->rca);
		if (err)
			goto free_card;

		mmc_set_bus_mode(host, MMC_BUSMODE_PUSHPULL);
	}
	printk("***********rca =0x%x!\n",card->rca);
	if (!oldcard) {
		/*
		 * Fetch CSD from card.
		 */
		err = mmc_send_csd(card, card->raw_csd);
		if (err)
			goto free_card;

		err = mmc_decode_csd(card);
		if (err)
			goto free_card;

		mmc_decode_cid(card);
	}

	/*
	 * Select card, as all following commands rely on that.
	 */
		
	if (!mmc_host_is_spi(host)) {
		err = mmc_select_card(card);
		if (err)
			goto free_card;
	}
	
	if (!oldcard) {
		/*
		 * Fetch SCR from card.
		 */
		printk("sd: card->raw_scr 0x%x, card: 0x%p\n", (unsigned int)card->raw_scr, card);
		err = mmc_app_send_scr(card, card->raw_scr);
		if (err)
			goto free_card;

		err = mmc_decode_scr(card);
		if (err < 0)
			goto free_card;
		/*
		 * Fetch switch information from card.
		 */
#ifdef DEBUG_SD30_TEST
		balong_break_point();  /*tiaoshi*/
#endif
#ifdef CONFIG_MMC_PARANOID_SD_INIT
		for (retries = 1; retries <= 3; retries++) {
			err = mmc_read_switch(card);
			if (!err) {
				if (retries > 1) {
					printk(KERN_WARNING
					       "%s: recovered\n", 
					       mmc_hostname(host));
				}
				break;
			} else {
				printk(KERN_WARNING
				       "%s: read switch failed (attempt %d)\n",
				       mmc_hostname(host), retries);
			}
		}
#else
		err = mmc_read_switch(card);
#endif

		if (err)
			goto free_card;
	}

	/*
	 * For SPI, enable CRC as appropriate.
	 * This CRC enable is located AFTER the reading of the
	 * card registers because some SDHC cards are not able
	 * to provide valid CRCs for non-512-byte blocks.
	 */
	if (mmc_host_is_spi(host)) {
		err = mmc_spi_set_crc(host, use_spi_crc);
		if (err)
			goto free_card;
	}

	/*
	 * Attempt to change to high-speed (if supported) or SDR 
	 */
	err = mmc_switch_hs(card);
	if (err)
		goto free_card;

	/*
	 * Compute bus speed.
	 */
	max_dtr = (unsigned int)-1;	
	if (mmc_card_highspeed(card)) {
		if (max_dtr > card->sw_caps.hs_max_dtr)
			max_dtr = card->sw_caps.hs_max_dtr;
	} else if (max_dtr > card->csd.max_dtr) {
		max_dtr = card->csd.max_dtr;
		printk("highspeed don't identify ,enter default fre :card->csd.max_dtr\n");
	}
	
	printk("####### sd final fre = %d########\n",max_dtr); 
#ifdef DEBUG_SD30_TEST  /*tiaoshi*/
	balong_break_point();	
	mmc_set_clock(host, g_max_fre);  
#else
	mmc_set_clock(host, max_dtr);
#endif

	/*
	 * Switch to wider bus (if supported).
	 */
	if ((host->caps & MMC_CAP_4_BIT_DATA) &&
		(card->scr.bus_widths & SD_SCR_BUS_WIDTH_4)) {
		err = mmc_app_set_bus_width(card, MMC_BUS_WIDTH_4);
		if (err)
			goto free_card;

		mmc_set_bus_width(host, MMC_BUS_WIDTH_4);
	}

   
    if ( SD_SUPPORT_UHS == mmc_judge_uhs())
    {
       	if (mmc_card_uhsddr50(card))
   		{
   			/*切换到DDR50模式*/
			hi_sdio_control_uhs_ddr_volt(host,1,1); 
			printk("sd3.0:bus speed mode is DDR50,not tuning!\n");
		}
		else
		{
			err = mmc_sd_switch_tuning(card);  /*no ddr50 switch tuning pattern  */ 
			if (err)
			{
				printk("sd3.0:mmc_sd_switch_tuning:switch 40 times fail!\n");
				goto free_card;
			}
		}
        
    }
    
	/*
	 * Check if read-only switch is active.
	 */
	if (!oldcard) {
		if (!host->ops->get_ro || host->ops->get_ro(host) < 0) {
			printk(KERN_WARNING "%s: host does not "
				"support reading read-only "
				"switch. assuming write-enable.\n",
				mmc_hostname(host));
		} else {
			if (host->ops->get_ro(host) > 0)
				mmc_card_set_readonly(card);
		}
	}

	if (!oldcard)
		host->card = card;

	return 0;

free_card:
	if (!oldcard)
		mmc_remove_card(card);
err:

	return err;
}

/*
 * Host is being removed. Free up the current card.
 */
static void mmc_sd_remove(struct mmc_host *host)
{
	BUG_ON(!host);      /*lint !e730*/
	BUG_ON(!host->card);    /*lint !e730*/

	mmc_remove_card(host->card);
	host->card = NULL;
}

/*
 * Card detection callback from host.
 */
static void mmc_sd_detect(struct mmc_host *host)
{
	int err = 0;
#ifdef CONFIG_MMC_PARANOID_SD_INIT
        int retries = 5;
#endif

	BUG_ON(!host);       /*lint !e730*/
	BUG_ON(!host->card);        /*lint !e730*/
       
	mmc_claim_host(host);

	/*
	 * Just check if our card has been removed.
	 */
#ifdef CONFIG_MMC_PARANOID_SD_INIT
	while(retries) {
		err = mmc_send_status(host->card, NULL);
		if (err) {
			retries--;
			udelay(5);
			continue;
		}
		break;
	}
	if (!retries) {
		printk(KERN_ERR "%s(%s): Unable to re-detect card (%d)\n",
		       __func__, mmc_hostname(host), err);
	}
#else
	err = mmc_send_status(host->card, NULL);
#endif
	mmc_release_host(host);

	if (err) {
		mmc_sd_remove(host);

		mmc_claim_host(host);
		mmc_detach_bus(host);
		mmc_release_host(host);
	}
}

/*
 * Suspend callback from host.
 */
static int mmc_sd_suspend(struct mmc_host *host)
{
	BUG_ON(!host);       /*lint !e730*/
	BUG_ON(!host->card);         /*lint !e730*/

	mmc_claim_host(host);
	if (!mmc_host_is_spi(host))
		mmc_deselect_cards(host);
	host->card->state &= ~MMC_STATE_HIGHSPEED;
	mmc_release_host(host);

	return 0;
}

/*
 * Resume callback from host.
 *
 * This function tries to determine if the same card is still present
 * and, if so, restore all state to it.
 */
static int mmc_sd_resume(struct mmc_host *host)
{
	int err;
#ifdef CONFIG_MMC_PARANOID_SD_INIT
	int retries;
#endif

	BUG_ON(!host);        /*lint !e730*/
	BUG_ON(!host->card);        /*lint !e730*/

	mmc_claim_host(host);
#ifdef CONFIG_MMC_PARANOID_SD_INIT
	retries = 5;
	while (retries) {
		err = mmc_sd_init_card(host, host->ocr, host->card);

		if (err) {
			printk(KERN_ERR "%s: Re-init card rc = %d (retries = %d)\n",
			       mmc_hostname(host), err, retries);
			mdelay(5);
			retries--;
			continue;
		}
		break;
	}
#else
	err = mmc_sd_init_card(host, host->ocr, host->card);
#endif
	mmc_release_host(host);

	return err;
}

static void mmc_sd_power_restore(struct mmc_host *host)
{
	host->card->state &= ~MMC_STATE_HIGHSPEED;
	mmc_claim_host(host);
	mmc_sd_init_card(host, host->ocr, host->card);
	mmc_release_host(host);
}

static const struct mmc_bus_ops mmc_sd_ops = {
	.remove = mmc_sd_remove,
	.detect = mmc_sd_detect,
	.suspend = NULL,
	.resume = NULL,
	.power_restore = mmc_sd_power_restore,
};

static const struct mmc_bus_ops mmc_sd_ops_unsafe = {
	.remove = mmc_sd_remove,
	.detect = mmc_sd_detect,
	.suspend = mmc_sd_suspend,
	.resume = mmc_sd_resume,
	.power_restore = mmc_sd_power_restore,
};

static void mmc_sd_attach_bus_ops(struct mmc_host *host)
{
	const struct mmc_bus_ops *bus_ops;

	if (host->caps & MMC_CAP_NONREMOVABLE || !mmc_assume_removable)
		bus_ops = &mmc_sd_ops_unsafe;
	else
		bus_ops = &mmc_sd_ops;
	mmc_attach_bus(host, bus_ops);
}

static int NvResume_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int NvResume_release(struct inode * inode, struct file * filp)
{
	remove_proc_entry("NvResume", NULL);
	return 0;	
}

#if(FEATURE_SDUPDATE == FEATURE_ON)
static const struct file_operations NvResume_fops =
{
	.owner = THIS_MODULE,
	.open = NvResume_open,
	.release = NvResume_release,
};

static struct proc_dir_entry *proc_flag;
#endif

/*****************************************************************************
 * 函 数 名  : creat_NvResume
 *
 * 功能描述  : SD 卡升级第二次重启NVID = 4 时创建NvResume 空文件
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 0:创建成功；
 *             		1:创建失败
 *
 * 其它说明  : NA
 *****************************************************************************/
int creat_NvResume(void)
{    
#if(FEATURE_SDUPDATE == FEATURE_ON)
    int ret = OSAL_ERROR;

    if(TFUP_DONE == hi_TFUP_getTFUpdateFlag())
    {
        ret = (BSP_U32)BSP_SYNC_Give(SYNC_MODULE_TFUPDATE); /* 通知M核,A核已创建好NV Resume proc文件系统 */
        if(OSAL_OK != ret)
        {
            printk("creat_NvResume: BSP_SYNC_Give %d failed.\n",SYNC_MODULE_TFUPDATE);
            return ret;
        }
        
        proc_flag = proc_create("NvResume", 0660, NULL, &NvResume_fops);
        if(!proc_flag)
        {
        	printk("Proc file fail.\n");
        	return -1;
        }
        
		return 0;
	}
	else
	{
        return -1;
	}
#else
    return 0;
#endif
}

#if defined (FEATURE_FLASH_LESS)
//#error zhuzheng_imei_is_valid
#else
/*****************************************************************************
 * 函 数 名  : sd_update_mode
 *
 * 功能描述  : 判断SD卡升级模式
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 0:烧片版本；
 *             1:出厂版本
 *
 * 其它说明  : NA
 *****************************************************************************/
int sd_update_mode(void)
{
    int ret;
    unsigned long sw_flag  = 0 ;

    ret = DR_NV_Read(36, &sw_flag, 1);
    if (BSP_OK != ret)
    {
        printk(KERN_ERR"NVIM:NV Read Fail! ret=%d\n", ret);
    }
    else
    {
        printk(KERN_INFO"NVIM:NV Read OK!\nNV value is: ");
	    printk(KERN_INFO"%x\t\n", (unsigned int)sw_flag);
    }
    
	return (sw_flag ? 1 : 0);
}


/*****************************************************************************
 * 函 数 名  : imei_is_valid
 *
 * 功能描述  : 判断IMEI是否有效
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 0:无效
 *             1:有效
 *
 * 其它说明  : NA
 *****************************************************************************/
int imei_is_valid(void)
{
    int ret;
    int i;
    int imei_valid_flag = 0;
    unsigned long imei[16] = {0};
    
    ret = DR_NV_Read(0, imei, 16);
    if (BSP_OK != ret)
    {
        printk(KERN_ERR"NVIM:NV Read Fail! ret=%d\n", ret);
    }
    else
    {
        printk(KERN_DEBUG"NVIM:NV Read OK!\nNV value is: ");        
	    for(i = 0; i < 16; i++)
		{
			printk(KERN_DEBUG"%d\t", (int)imei[i]);
		}

        /*判断IMEI号是否是全0*/
        for(i = 0; i < 16; i++)
		{
            if(0 == imei[i])
            {
                ;
            }
            else
            {
                imei_valid_flag = 1;
                break;
            }
		}
    }

    printk(KERN_DEBUG"imei_valid_flag = %d\n", imei_valid_flag);
    return (imei_valid_flag ? 1 : 0);
}
#endif
/*****************************************************************************
 * 函 数 名  : do_final_check
 *
 * 功能描述  : SD卡升级任务
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 其它说明  : NA
 *****************************************************************************/
void do_final_check(void)
{    
#if (FEATURE_SDUPDATE == FEATURE_ON)
    printk("do_final_check:enter...\n");

    if(TFUPUNDOFLAG == hi_TFUP_GetSuccessFlag())
    {
        printk("SD Update is : TFUPUNDOFLAG\n");
        hi_TFUP_CheckTask();
    }
    else
    {
        printk("SD Update is : SUCCESSFLAG\n");
    }
#endif	
}

/*****************************************************************************
 * 函 数 名  : sd_mmi_check
 *
 * 功能描述  : 判断是否完成MMI测试
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 0:成功；
 *             -1:失败
 *
 * 其它说明  : NA
 *****************************************************************************/
int sd_mmi_check(void)
{
    int ret;
    char aucFactoryInfo[NV_MMI_TEST_SIZE] = {0};

    ret = DR_NV_Read(NV_MMI_TEST, aucFactoryInfo, NV_MMI_TEST_SIZE);
    if (BSP_OK != ret)
    {
        printk(KERN_ERR "NVIM:NV Read Fail! ret=%d\n", ret);
        return ERROR;
    }
    else
    {
        printk(KERN_INFO "NVIM:NV Read OK!\n");
    }
    printk( KERN_INFO " 000aucFactoryInfo is %s\r\n ", &aucFactoryInfo[NV_MMI_TEST_OFFSET] );
   
    ret = strncmp(MMI_TEST_SUCCESS, &aucFactoryInfo[NV_MMI_TEST_OFFSET], strlen(MMI_TEST_SUCCESS));
    printk(KERN_INFO "%s: Exit!!! ret is : %d!!!\r\n", __func__, ret);
    return ret? ERROR: OK;
}
#if defined (FEATURE_FLASH_LESS)
//#error zhuzheng_sd_upgrade_check
#else
/*****************************************************************************
 * 函 数 名  : sd_upgrade_check
 *
 * 功能描述  : 通过读NV判断烧片版本升级还是出厂版本升级
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 其它说明  : NA
 *****************************************************************************/
void sd_upgrade_check(void)
{
    printk("sd_upgrade_check:enter...\n");
    
  	if(0 == sd_update_mode())
    {
        printk("sd_upgrade_check:now is factory mode.\n");
        
        /* Factroy mode */
        if(1 == imei_is_valid())
        {            
            do_final_check();
        }
        else
        {
            #if ( FEATURE_MMI_TEST == FEATURE_ON )
            if(OK == sd_mmi_check())
            {
            #if (FEATURE_OLED == FEATURE_ON)
            oledClearWholeScreen();
            oledStringDisplay(16, 16, (UINT8 *)"NO IMEI");
            #elif (FEATURE_TFT == FEATURE_ON)
            tftClearWholeScreen();
            tftStringDisplay(16, 84, (UINT8 *)"NO IMEI");
            #endif
            }
            #endif
            printk("NO IMEI!\n");
        }
    }
    else
    {
        #if (FEATURE_SDUPDATE == FEATURE_ON)
        if(hi_TFUP_GetGoOnFlag())
        {
            printk("sd_upgrade_check:custom mode loop test start.\n");
            do_final_check();        
        }
        #endif
    }     
}

#endif 
/*
 * Starting point for SD card init.
 */
int mmc_attach_sd(struct mmc_host *host, u32 ocr)
{
	int err;
 	//char mmc_dev_name[20] = "/dev/block/mmcblk0";  /*y00186965 for sd_update*/
	//char blkno_str[5];
//	unsigned int blkno = 0;
#ifdef CONFIG_MMC_PARANOID_SD_INIT
	int retries;
#endif

	BUG_ON(!host);         /*lint !e730*/
	WARN_ON(!host->claimed);

	mmc_sd_attach_bus_ops(host);

	/*
	 * We need to get OCR a different way for SPI.
	 */
	if (mmc_host_is_spi(host)) {
		mmc_go_idle(host);

		err = mmc_spi_read_ocr(host, 0, &ocr);
		if (err)
			goto err;
	}

	/*
	 * Sanity check the voltages that the card claims to
	 * support.
	 */
	if (ocr & 0x7F) {
		printk(KERN_WARNING "%s: card claims to support voltages "
		       "below the defined range. These will be ignored.\n",
		       mmc_hostname(host));
		ocr &= ~0x7F;
	}

	if (ocr & MMC_VDD_165_195) {
		printk(KERN_WARNING "%s: SD card claims to support the "
		       "incompletely defined 'low voltage range'. This "
		       "will be ignored.\n", mmc_hostname(host));
		ocr &= ~MMC_VDD_165_195;
	}

	host->ocr = mmc_select_voltage(host, ocr);

	/*
	 * Can we support the voltage(s) of the card(s)?
	 */
	if (!host->ocr) {
		err = -EINVAL;
		goto err;
	}

	/*
	 * Detect and init the card.
	 */
#ifdef CONFIG_MMC_PARANOID_SD_INIT
	retries = 5;
	while (retries) {
		err = mmc_sd_init_card(host, host->ocr, NULL);
		if (err) {
			retries--;
			continue;
		}
		break;
	}

	if (!retries) {
		printk(KERN_ERR "%s: mmc_sd_init_card() failure (err = %d)\n",
		       mmc_hostname(host), err);
		goto err;
	}
#else
	err = mmc_sd_init_card(host, host->ocr, NULL);
	if (err)
		goto err;
#endif

	mmc_release_host(host);

	err = mmc_add_card(host->card);
	
	if (err)
		goto remove_card;

	else      /*挂接文件系统与SD_TRACE冲突?*/
	{
		g_host = host;
#if(FEATURE_SDUPDATE == FEATURE_ON)
        printk("creat_NvResume:enter V7R1...\n");
        /*将创建NVResume文件放在hi_sdio_init模块中，防止升级完后立即拔掉SD卡出现异常*/
        /*creat_NvResume();*/
        sd_upgrade_check();//SD卡升级入口  
#endif
		/*此时SD卡已经注册成功，调用接口读取升级NV*/
    }

#if (FEATURE_E5 == FEATURE_ON)
	/*SD 中断上报netlink*/
	report_sd_state(DEVICE_ID_SD,SD_ATTATCH, "SD_ATTATCH");/*l00205892*/
#endif

	BSP_PWRCTRL_SD_LowPowerEnter();

	return 0;

remove_card:
	mmc_remove_card(host->card);
	host->card = NULL;
	mmc_claim_host(host);
err:
	mmc_detach_bus(host);
	mmc_release_host(host);

	printk(KERN_ERR "%s: error %d whilst initialising SD card\n",
		mmc_hostname(host), err);
	
	BSP_PWRCTRL_SD_LowPowerEnter();
	return err;
}


/*SD read/write interface for OM trace save fun*/

#include <linux/scatterlist.h>

#define RESULT_OK		0
#define RESULT_FAIL		1
#define RESULT_UNSUP_HOST	2
#define RESULT_UNSUP_CARD	3
#define SD_BUFFER_SIZE    0x4000//0x400//

 struct scatterlist *st_sg;
 struct scatterlist *st_r_sg;
 
 unsigned char *s_buffer = NULL;
 unsigned char *d_buffer = NULL;
 unsigned char *c_buffer = NULL;


 static int sd_wait_busy(struct mmc_card *card)
 {
	 int ret, busy;
	 struct mmc_command cmd;
 
	 busy = 0;
	 do {
		 memset(&cmd, 0, sizeof(struct mmc_command));
 
		 cmd.opcode = MMC_SEND_STATUS;
		 cmd.arg = card->rca << 16;
		 cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
 
		 ret = mmc_wait_for_cmd(card->host, &cmd, 0);
		 if (ret)
			 break;
 
		 if (!busy && !(cmd.resp[0] & R1_READY_FOR_DATA)) {
			 busy = 1;
			 printk(KERN_INFO "%s: Warning: Host did not "
				 "wait for busy state to end.\n",
				 mmc_hostname(card->host));
		 }
	 } while (!(cmd.resp[0] & R1_READY_FOR_DATA));
 
	 return ret;
 }


 /*lint -e713*/
 static int sd_check_result(struct mmc_card *card,
	 struct mmc_request *mrq)
 {
	 int ret;
 
	 BUG_ON(!mrq || !mrq->cmd || !mrq->data);        /*lint !e730*/
 
	 ret = 0;

	 /*上面已经有BUG_ON判断*/
	 /*lint -e613*/
	 if (!ret && mrq->cmd->error)
		 ret = mrq->cmd->error;
	 if (!ret && mrq->data->error)
		 ret = mrq->data->error;
	 if (!ret && mrq->stop && mrq->stop->error)
		 ret = mrq->stop->error;
	 if (!ret && mrq->data->bytes_xfered !=
		 mrq->data->blocks * mrq->data->blksz)
		 ret = RESULT_FAIL;
	 /*lint +e613*/
 
	 if (ret == -EINVAL)
		 ret = RESULT_UNSUP_HOST;

	 printk("sd_transfer is over!\n");
	 	
	 return ret;

 }
 /*lint +e713*/

 static void sd_prepare_mrq(struct mmc_card *card,
	 struct mmc_request *mrq, struct scatterlist *sg, unsigned sg_len,
	 unsigned dev_addr, unsigned blocks, unsigned blksz, int write)
 {
	 BUG_ON(!mrq || !mrq->cmd || !mrq->data || !mrq->stop);       /*lint !e730*/

     /*上面已经有BUG_ON判断*/
     /*lint -e613*/
	 if (blocks > 1) {
		 mrq->cmd->opcode = write ?
			 MMC_WRITE_MULTIPLE_BLOCK : MMC_READ_MULTIPLE_BLOCK;
	 } else {
		 mrq->cmd->opcode = write ?
			 MMC_WRITE_BLOCK : MMC_READ_SINGLE_BLOCK;
	 }
 
	 mrq->cmd->arg = dev_addr;
	 if (!mmc_card_blockaddr(card))
		 mrq->cmd->arg <<= 9;
 
	 mrq->cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;
 
	 if (blocks == 1)
		 mrq->stop = NULL;
	 else {
		 mrq->stop->opcode = MMC_STOP_TRANSMISSION;
		 mrq->stop->arg = 0;
		 mrq->stop->flags = MMC_RSP_R1B | MMC_CMD_AC;
	 }
 
	 mrq->data->blksz = blksz;
	 mrq->data->blocks = blocks;
	 mrq->data->flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
	 mrq->data->sg = sg;
	 mrq->data->sg_len = sg_len;
 
	 mmc_set_data_timeout(mrq->data, card);
	 /*lint +e613*/
 }

 
 /*****************************************************************************
 * 函 数 名  : sd_transfer
 *
 * 功能描述  : 数据传输
 *
 * 输入参数  : struct scatterlist *sg	 待传输数据结构体指针
							 unsigned dev_addr	 待写入的SD block 地址
							 unsigned blocks	待写入的block个数
							 unsigned blksz 	 每个block的大小，单位字节
							 int wrflags	读写标志位，写:WRFlAG ; 读:RDFlAG
 * 输出参数  : NA
 *
 * 返 回 值  : 0 : 成功；其它: 失败
 *
 * 其它说明  : NA
 *
 *****************************************************************************/
 int sd_transfer(	struct scatterlist *sg,unsigned dev_addr,
	unsigned blocks, unsigned blksz, int write)
{
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_command stop;
	struct mmc_data data;
    int ret;

	static unsigned int ploop = 0;

	if (NULL == g_host)
	{
		printk("sd_transfer g_host is null!\n");
		return -1;
	}

	BSP_PWRCTRL_SD_LowPowerExit();

	mmc_claim_host(g_host); /*y00186965*/
	
    printk("[%x:%.8x:%x]\n",write,dev_addr,(blksz * blocks));
    if (++ploop%8 == 0)
        printk("\n");
	
	memset(&mrq, 0, sizeof(struct mmc_request));
	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));
	memset(&stop, 0, sizeof(struct mmc_command));

	mrq.cmd = &cmd;
	mrq.data = &data;
	mrq.stop = &stop;

	sd_prepare_mrq(g_host->card,&mrq, sg, 1, dev_addr,
		blocks, blksz, write);

	mmc_wait_for_req(g_host, &mrq);
	
	sd_wait_busy(g_host->card);

	mmc_release_host(g_host);  /*y00186965*/

    ret = sd_check_result(g_host->card, &mrq);

    BSP_PWRCTRL_SD_LowPowerEnter();
    
	return ret;
}
EXPORT_SYMBOL(sd_transfer);


int sd_r_w_prepare(void)
{
    int i = 0;    
	unsigned char *buffer1 = NULL;
	unsigned char *buffer2=  NULL;
		
	st_sg = kzalloc(sizeof(struct scatterlist), GFP_KERNEL);
	st_r_sg = kzalloc(sizeof(struct scatterlist), GFP_KERNEL);
	d_buffer = kzalloc(SD_BUFFER_SIZE, GFP_KERNEL);
	s_buffer = kzalloc(SD_BUFFER_SIZE, GFP_KERNEL);
	c_buffer = kzalloc(SD_BUFFER_SIZE, GFP_KERNEL);

	buffer2 = c_buffer;
	buffer1 = s_buffer;
    for(i = 0; i < SD_BUFFER_SIZE; i++)
    {
        *(buffer1++) = i % 10;
		*(buffer2++) = i % 10;		
    }	    
	sg_init_one(st_sg, s_buffer, SD_BUFFER_SIZE);   
	sg_init_one(st_r_sg, d_buffer, SD_BUFFER_SIZE);

    return 0;
}

int sd_test_w_test(void)
{
	int ret,i;	
	unsigned char *bufferw =  NULL;
	bufferw = s_buffer;
	for(i=0;i<300;i++)
	{
        printk("s_buffer_pre[%d]=%d\n",i,*bufferw++);
	}
	ret = sd_transfer(st_sg, 0, (SD_BUFFER_SIZE/512), 512, 1);
    if (ret)
	{
		printk("sd_test_write err!\n");
		kzfree(d_buffer);
		kzfree(s_buffer);
		kzfree(c_buffer);
		kzfree(st_sg);
		return ret;	 
	}
	for(i=0;i<300;i++)
	{
        printk("s_buffer_aft[%d]=%d\n",i,*bufferw++);
	}
	return 0;
}

int sd_test_r_test(void)
{
	int ret,i;	
	unsigned char *bufferr = NULL;
	bufferr = d_buffer;
	ret = sd_transfer(st_r_sg, 0, (SD_BUFFER_SIZE/512), 512, 0);
    if (ret)
	{
		printk("sd_test_write err!\n");
		kzfree(d_buffer);
		kzfree(s_buffer);
		kzfree(c_buffer);
		kzfree(st_sg);
		return ret;	 
	}
	for(i=0;i<1024;i++)
	{
        printk("d_buffer[%d]=%d\n",i,*bufferr++);
	}
	return 0;
}

int sd_test_w_r_run(unsigned int ulTimes)
{
	int ret,i,j;	
	unsigned char *buffer1 = NULL;
	unsigned char *buffer2 = NULL;	

	//mmc_claim_host(g_host);  /*y00186965*/
	for(i=0;i<(int)ulTimes;i++)
	{
		buffer1 = d_buffer;
		buffer2 = c_buffer;			

		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 2) (int to unsigned int))*/
	    ret = sd_transfer(st_sg, (unsigned int)(i*(SD_BUFFER_SIZE/512)), (SD_BUFFER_SIZE/512), 512, 1);
		/*end*/
	    if (ret)
    	{
			printk("sd_test_write err!\n");
			kzfree(d_buffer);
			kzfree(s_buffer);
			kzfree(c_buffer);
			kzfree(st_sg);
			return ret;	 
		}	
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 2) (int to unsigned int))*/
		ret = sd_transfer(st_r_sg,(unsigned int)(i*(SD_BUFFER_SIZE/512)), SD_BUFFER_SIZE/512, 512, 0);
		/*end*/
		if (ret)
        {
        	printk("sd_test read err!\n");
			kzfree(d_buffer);
			kzfree(s_buffer);
			kzfree(c_buffer);
			kzfree(st_sg);
		    return ret;	
        }		    
		else
		{
		    /*compare with write data*/
		    for(j=0;j<SD_BUFFER_SIZE;j++)
		    {
		        if(*(buffer1++) != *(buffer2++))
	        	{
	        		    		
					printk("read don't = write!\n");
					kzfree(d_buffer);
					kzfree(s_buffer);
					kzfree(c_buffer);
					kzfree(st_sg);
					return RESULT_FAIL;
				}
				if (j<255)
    			{
					printk("d_buffer[%d]=%d\n\r",j,*buffer1);
				}	    
		            
		    }
		}
	}

    return 0;
}

/*****************************************************************************
* 函 数 名  : sd_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_status(void)
{
	if (NULL == g_host)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
EXPORT_SYMBOL(sd_get_status);

/*****************************************************************************
* 函 数 名  : sd_get_status_for_usb(void)
*
* 功能描述  : 查询卡是否在升级
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_status_for_usb(void)
{
    return ((!sd_get_status() && !g_inFatChecking)?OK:ERROR);
}
EXPORT_SYMBOL(sd_get_status_for_usb);

/*****************************************************************************
* 函 数 名  : sd_get_capacity
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_capacity(void) /*clean lint e713*/
{	/*lint !e18 */
	if (g_host)
	{
		return (g_host->card->csd.capacity << (g_host->card->csd.read_blkbits - 9));
	}
	else
	{
		return 0;
	}
	
}
EXPORT_SYMBOL(sd_get_capacity);


int sd_check_transfer_state(struct mmc_card *card)
{
    u32 resp = 0;
    u32 ulSdStatus = 0;
    int err = 0;

    /*检查SD卡状态*/
    err = mmc_send_status(card,&resp);
    if (err)
    {
        printk(KERN_ERR "(%s): Unable to re-detect card (%d)\n",
		       __func__, err);
        return err;
    }

    /*判断卡的状态*/

    ulSdStatus = R1_CURRENT_STATE(resp);
    
    if ( CARD_STATE_STBY == ulSdStatus)
    {
        err = mmc_select_card(card);
        if (err)
        {
            printk(KERN_ERR "(%s): Unable to select card (%d)\n",
		       __func__, err);
            return err;
        }
        err = mmc_send_status(card,&resp);
        if (err)
        {
            printk(KERN_ERR "(%s): Unable to re-detect card (%d)\n",
		       __func__, err);
            return err;
        }
        ulSdStatus = R1_CURRENT_STATE(resp);
        if (CARD_STATE_TRAN != ulSdStatus)
        {
            printk(KERN_ERR "card status is (%s): Unable to re-detect card (%d)\n",
		       __func__, ulSdStatus);
            return RESULT_FAIL;
        }
        
    }
    else if ( CARD_STATE_TRAN != ulSdStatus)
    {
        printk(KERN_ERR "card status is (%s): Unable to re-detect card (%d)\n",
		       __func__, ulSdStatus);
        return RESULT_FAIL;
    }

    return RESULT_OK;

}


int sd_erase_block(struct mmc_card *card,u32 dwBlkStart,u32 dwBlkCount)
{
    int err = 0;
    u32 eraseStart = 0;
    u32 eraseCount = 0;
    if (NULL == g_host)
    {
        printk(KERN_ERR "g_host is null!\n");
        return RESULT_FAIL;
    }

    mmc_claim_host(g_host); 

    /*SDSC:byte,SDHC/SDXC:blk*/
    if (mmc_card_blockaddr(card))
    {
        eraseStart = dwBlkStart;
        eraseCount = dwBlkCount;
    }
    else
    {
        eraseStart =  dwBlkStart<<9;
        eraseCount =  dwBlkCount<<9;
    }
    
    /*check trans status*/
    err = sd_check_transfer_state(card);
    
    /*Send Erase group start command*/
    
    err |= mmc_sd_send_erase_group_start(card,eraseStart);   

    /*Send Erase group End command*/
    err |= mmc_sd_send_erase_group_end(card,eraseStart+eraseCount-1);    

    /*Start erase command.*/
    err |= mmc_send_erase(card);    

    /*check erase result*/
    err |= sd_wait_busy(card); 
    
    mmc_release_host(g_host);

    return err;
    
}

EXPORT_SYMBOL(sd_erase_block);

int sd_erase_test(u32 dwBlkStart,u32 dwBlkCount)
{
    int err = 0;
    err = sd_erase_block(g_host->card,dwBlkStart,dwBlkCount);
    return err;

}
EXPORT_SYMBOL(sd_erase_test);


/*sd multi transfer , max size is 128KB, min size is 512B*/
int g_bufferSize = 0x10000;
#define SD_TRACE_CLUSTER_SIZE  0x8000
#define SD_MULTI_BUFFER_SIZE  g_bufferSize 
#define SD_TRACE_TEST
struct scatterlist *g_sd_sg;
int g_sgcnt = 0;
struct sd_trace_dbg_s
{
	unsigned long trans_total; /* Block 数量 */
	unsigned long trans_success;
	unsigned long trans_fail;
};

struct sd_trace_dbg_s g_sd_trace_dbg = {0};

void sd_trace_show(void)
{
	printk("sd_trace_show	:\n");
	printk("trans_total		:%d\n",g_sd_trace_dbg.trans_total);
	printk("trans_ok		:%d\n",g_sd_trace_dbg.trans_success);
	printk("trans_fail		:%d\n",g_sd_trace_dbg.trans_fail);
}

/*****************************************************************************
* 函 数 名  : sd_sg_init_table
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  :  const void *buf        待操作的buffer地址
               unsigned int buflen    待操作的buffer大小,小于32K，为512B的整数倍;
                                      大于32K，为32KB的整数倍，最大128K
                            
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_sg_init_table(const void *buf,unsigned int buflen)
{
    int cnt = 0;
    int sgcnt = 0; 
    u8 *dataBuf = buf;
    struct scatterlist *sg;
    struct scatterlist *sgNode; 
    g_sd_sg = NULL;
    g_sgcnt = 0;
	
	if ((NULL == buflen)||(NULL == buf)|| (0 != (buflen % 512)))
    {
        printk("sd_sg_init_table para is err!\n");
        return -1;
    }
    if (buflen < SD_TRACE_CLUSTER_SIZE)
    {
        sgcnt = 1;
    }
    else if ( 0 == buflen % SD_TRACE_CLUSTER_SIZE )
    {
        sgcnt = ( buflen/SD_TRACE_CLUSTER_SIZE );
    }
    else
    {
        printk("sd_sg_init_table buf isn't n*32k!\n");
        return -1;
    } 
    
    sg = kzalloc(sizeof(struct scatterlist)*sgcnt, GFP_KERNEL);
    if (NULL == sg)
    {
        printk("sg kmalloc fail!\n");
        return -1;
    }
    sg_init_table(sg, sgcnt); 

    if (1 == sgcnt)
    {
        sg_set_buf(sg, dataBuf, buflen);
    }
    else
    {
        for_each_sg(sg, sgNode, sgcnt, cnt)
        {
            sg_set_buf(sgNode, dataBuf, SD_TRACE_CLUSTER_SIZE);
            dataBuf += SD_TRACE_CLUSTER_SIZE;        
        } 

        
    }
    
    g_sd_sg = sg;
    g_sgcnt = sgcnt;

    return 0;

}

/*****************************************************************************
* 函 数 名  : sd_multi_transfer
*
* 功能描述  : SD多块数据传输。
*
* 输入参数  : unsigned dev_addr   待写入的SD block 地址
              unsigned blocks     待写入的block个数
              unsigned blksz      每个block的大小，单位字节
              int write           读写标志位，写:1 ; 读:0
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/

int sd_multi_transfer(unsigned dev_addr, unsigned blocks, unsigned blksz, int write)
{
    struct mmc_request mrq;
    struct mmc_command cmd;
    struct mmc_command stop;
    struct mmc_data data;
    int ret;

    static unsigned int ploop = 0;

	if((dev_addr != 0)&&(write == 1))
	{
		g_sd_trace_dbg.trans_total += blocks;
	}

    if (NULL == g_sd_sg)
    {
        printk("sd_multi_transfer para g_sd_sg=NULL!\n");
        return -1;
    }
    if ((NULL == g_host)||(0 == g_sgcnt))
    {
        kzfree(g_sd_sg);
        printk("sd_multi_transfer para is err,g_host =%d,g_sgcnt =%d!\n",g_host,g_sgcnt);
        return -1;
    }

    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_SD);

    mmc_claim_host(g_host); 

    g_inFatChecking = TRUE;

    printk(KERN_DEBUG"[%x:%.8x:%x]\n",write,dev_addr,(blksz * blocks));
    if (++ploop%8 == 0)
        printk(KERN_DEBUG"\n");

    memset(&mrq, 0, sizeof(struct mmc_request));
    memset(&cmd, 0, sizeof(struct mmc_command));
    memset(&data, 0, sizeof(struct mmc_data));
    memset(&stop, 0, sizeof(struct mmc_command));

    mrq.cmd = &cmd;
    mrq.data = &data;
    mrq.stop = &stop;

    sd_prepare_mrq(g_host->card,&mrq, g_sd_sg, g_sgcnt, dev_addr,
        blocks, blksz, write);

    mmc_wait_for_req(g_host, &mrq);

    sd_wait_busy(g_host->card);

    mmc_release_host(g_host); 

    ret = sd_check_result(g_host->card, &mrq); 
	if(ret)
	{
		if((dev_addr != 0)&&(write == 1))
		{
			g_sd_trace_dbg.trans_fail += blocks;
		}
	}
    else
   	{
		if((dev_addr != 0)&&(write == 1))
		{
   			g_sd_trace_dbg.trans_success += blocks;
		}
   	}
    
    /*传输完成，需要删除sg链表*/
    kzfree(g_sd_sg);
    g_sd_sg = NULL;
    BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SD);
    return ret;
}


#ifdef SD_TRACE_TEST

void set_buffer_size(int size)
{
    g_bufferSize = size;

}

int get_buffer_size(void)
{
    return g_bufferSize;

}

int sd_multi_w_prepare(unsigned char value1, unsigned char value2)
{
    int i = 0;    
    unsigned char *buffer = NULL;    
    int ret  = 0;

    s_buffer = kzalloc(SD_MULTI_BUFFER_SIZE, GFP_KERNEL);
    if (NULL == s_buffer)
    {
        printk("s_buffer kmalloc fail!\n");
        return -1;
    }
    buffer = s_buffer;
    for(i = 0; i < (SD_MULTI_BUFFER_SIZE >> 1); i++)
    {
        *(buffer+2*i) = value1;
        *(buffer+2*i+1) = value2;
    }
    ret = sd_sg_init_table(s_buffer,SD_MULTI_BUFFER_SIZE); 
    return ret;
}


int sd_multi_r_prepare(void)
{     
    int ret = 0;    
    d_buffer = kzalloc(SD_MULTI_BUFFER_SIZE, GFP_KERNEL); 
    if (NULL == d_buffer)
    {
        printk("d_buffer kmalloc fail!\n");
        return -1;
    }
    ret = sd_sg_init_table(d_buffer,SD_MULTI_BUFFER_SIZE);
    return ret;
}

int sd_multi_r_compare_prepare(unsigned char value1, unsigned char value2)
{
    int i = 0;    
    unsigned char *buffer = NULL;
    int ret = 0;
    
    d_buffer = kzalloc(SD_MULTI_BUFFER_SIZE, GFP_KERNEL);
    c_buffer = kzalloc(SD_MULTI_BUFFER_SIZE, GFP_KERNEL);
    
    buffer = c_buffer;    
    for(i = 0; i < (SD_MULTI_BUFFER_SIZE >> 1); i++)
    {
        *(buffer+2*i) = value1;
        *(buffer+2*i+1) = value2;
    }
    
    ret = sd_sg_init_table(d_buffer,SD_MULTI_BUFFER_SIZE);
    return ret;
}


int sd_test_multi_w_test(unsigned int blkAddr)
{
    int ret,capacity,blknum;

    blknum = SD_MULTI_BUFFER_SIZE/SDMMC_BLOCK_BYTE;
    capacity = sd_get_capacity();    
        
    if((blkAddr + blknum) > capacity)
    {
        printk("block number is over capacity!\n");
        return -1;
    }
    ret = sd_multi_transfer(blkAddr, blknum, SDMMC_BLOCK_BYTE, 1);
    if (ret)
    {
        printk("sd_test_write err!\n");        
        kzfree(s_buffer);        
        s_buffer = NULL;
        return ret;     
    }    
    
    return 0;
}

int sd_test_multi_r_test(unsigned int blkAddr)
{
    int ret,i,capacity,blknum;    
    unsigned char *buffer = NULL;        
    capacity = sd_get_capacity();
    blknum = SD_MULTI_BUFFER_SIZE/SDMMC_BLOCK_BYTE;
    
    if((blkAddr + blknum) > capacity)
    {
        printk("block number is over capacity!\n");
        return -1;
    }

    buffer = d_buffer;        
    ret = sd_multi_transfer(blkAddr, blknum, SDMMC_BLOCK_BYTE, 0);
    if (ret)
    {
        printk("sd_test_read err!\n");
        kzfree(d_buffer);        
        d_buffer = NULL;        
        return ret;     
    }        
        
    else
    {
        /*printk 512B*/
        for(i=0;i<512;i++)
        {
            printk("d_buffer[%d]=%d\n\r",i,*buffer++);        
                
        }
    }

    kzfree(d_buffer);    
    d_buffer = NULL;
        
    return 0;
}


int sd_test_multi_r_compare_test(unsigned int blkAddr)
{
    int ret,j,capacity,blknum;    
    unsigned char *buffer1 = NULL;
    unsigned char *buffer2 = NULL;    
    capacity = sd_get_capacity();
    blknum = SD_MULTI_BUFFER_SIZE/SDMMC_BLOCK_BYTE;
    
    if((blkAddr + blknum) > capacity)
    {
        printk("block number is over capacity!\n");
        return (u32)ERROR;
    }

    buffer1 = d_buffer;
    buffer2 = c_buffer;            
    
    ret = sd_multi_transfer(blkAddr, blknum, SDMMC_BLOCK_BYTE, 0);
    if (ret)
    {
        printk("sd_test_read err!\n");
        kzfree(d_buffer);        
        kzfree(c_buffer);        
        d_buffer = NULL;
        c_buffer = NULL;
        return ret;     
    }        
        
    else
    {
        /*compare with write data*/
        for(j=0;j<SD_MULTI_BUFFER_SIZE;j++)
        {
            if(*(buffer1++) != *(buffer2++))
            {

                printk("read don't = write!,read [%d] data is %d,write data is %d\n",j,*buffer1,*buffer2);
                kzfree(d_buffer);
                kzfree(c_buffer);
                d_buffer = NULL;
                c_buffer = NULL;
                return -1;
            }
            if (j<512)
            {
                printk("d_buffer[%d]=%d\n\r",j,*buffer1);
            }        
                
        }
    }

    kzfree(d_buffer);
    kzfree(c_buffer);
    d_buffer = NULL;
    c_buffer = NULL;    
    return 0;
}

#endif

EXPORT_SYMBOL(sd_sg_init_table);
EXPORT_SYMBOL(sd_multi_transfer);
/*at^sd interface 2012-01-16 begin*/

/*****************************************************************************                                           
 函 数 名  : sdmmcATProcessTask                                                                                         
 功能描述  : at^sd,SD卡操作任务                                                                                  
 输入参数  :                                                                                                           
 输出参数  : 无                                                                                                          
 返 回 值  :                                                                                                             
            0: SUCCES;                                                                                     
            1: ERROR                                                                                                   
*****************************************************************************/ 
s32 sdmmcATProcessTask(void * args)
{
	SDMMC_ATPROCESS_TASK_ARG ptask_args = task_arg;
	u32 sdmmcCap = 0x0; 
	int ret = 0x0;
	u8  *buffer = NULL;
	u8  *buffer_align = NULL;
	struct scatterlist *sg;
	
	/*分配内存*/
	buffer = (u8 *)kmalloc(SDMMC_BLOCK_BYTE * 2, GFP_KERNEL);
    if(NULL == buffer)
    {
        printk(KERN_INFO "sdmmcATProcessTask: buffer kmalloc fail!\n");
        return (u32)ERROR;
    }
	/*BLOCK对齐*/
	buffer_align = (u8 *)(( (u32)buffer + SDMMC_BLOCK_BYTE ) & ((u32)0xFFFFFE00));
	
	switch(ptask_args.ulOp)
	{
		/*格式化整张卡*/
		case (SDMMC_AT_FORMAT):
		/*擦除整张卡*/ 
		case (SDMMC_AT_ERASE_ALL):
		{  
			/*获取卡的容量*/ 
			
			/*begin lint-Info 732: (Info -- Loss of sign (assignment)  (int to unsigned int))*/
			sdmmcCap = (u32)(sd_get_capacity());
			/*end*/
			
			/*调用擦除接口函数*/					
			ret = sd_erase_block(g_host->card, 1, sdmmcCap);
			if (OK != ret)
			{
				*(ptask_args.pulErr) = SDMMC_OTHER_ERROR;
				sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
				kfree(buffer);
				return ERROR;
			}
			break;
		}
		/*擦除指定地址的内容*/
		case (SDMMC_AT_ERASE_ADDR):
		{			
			/*调用擦除接口函数*/
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
			ret = sd_erase_block(g_host->card, (u32)(ptask_args.ulAddr), 1);
			/*end*/
			if (OK != ret)
			{
				sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;				
				*(ptask_args.pulErr) = SDMMC_OTHER_ERROR;
				kfree(buffer);
				return ERROR;
			}
			break;

		}
		/*指定地址写入指定内容*/
		case (SDMMC_AT_WRTIE_ADDR):
		{
			/*参数判断*/
			if (ptask_args.ulData >= SDMMC_DATA_BUTT)
			{
				*(ptask_args.pulErr) = SDMMC_OTHER_ERROR;
				sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
				kfree(buffer);
				return ERROR;
			}
		    
			/*根据参数选择写入内容【00，55，AA，FF】*/
			switch(ptask_args.ulData)
			{
				case (SDMMC_DATA0): 
				{
					memset(buffer_align, 0x00, SDMMC_BLOCK_BYTE);
					break;
				}
				case (SDMMC_DATA1):
				{
					memset(buffer_align, 0x55, SDMMC_BLOCK_BYTE);
					break;
				}
				case (SDMMC_DATA2):
				{
					memset(buffer_align, 0xAA, SDMMC_BLOCK_BYTE);
					break;
				}
				case (SDMMC_DATA3):
				{
					memset(buffer_align, 0xFF, SDMMC_BLOCK_BYTE);
					break;
				}
				default:
				{
				     ;
				}
			}
			
			/*调用写接口函数*/ 
			sg = kzalloc(sizeof(struct scatterlist), GFP_KERNEL);
            if(NULL == sg)
            {
                kfree(buffer);
                buffer = NULL;
                printk(KERN_INFO "####### sdmmcATProcessTask: sg kzalloc fail! #######\n");
                return (u32)ERROR;
            }
			sg_init_one(sg, buffer_align, SDMMC_BLOCK_BYTE);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 2) (int to unsigned int))*/
			ret = sd_transfer(sg, (unsigned)(ptask_args.ulAddr), 1, SDMMC_BLOCK_BYTE, 1);
			/*end*/
			if (OK != ret)
			{		   
				*(ptask_args.pulErr) = SDMMC_OTHER_ERROR;
				sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
				kfree(buffer);
				kfree(sg);
				return ERROR;
			}
			kfree(sg);
			break;
		}
		default:
		{
			;
		}       

	}
	sdmmcATProcessOverFlag = SDMMC_PROCESS_OVER_OR_UNDO;
	kfree(buffer);	
	return OK;

}

/*****************************************************************************                                           
 函 数 名  : BSP_SDMMC_ATProcess                                                                                             
 功能描述  : at^sd,SD卡操作，写，擦除，格式化操作                                                                        
             0:                                                                                                          
 输入参数  : 操作类型 ulOp:                                                                                              
            0  格式化SD卡内                                                                                              
            1  擦除整个SD卡内容；                                                                                        
            2  用于指定地址内容的擦除操作，指定擦除的内容长度为512字节。擦除后的地址中写全1                              
            3  写数据到SD卡的指定地址中，需要带第二个和第三个参数                                                        
            4  读数据到SD卡的指定地址中
            
            ulAddr < address >  地址，以512BYTE为一个单位，用数字n表示                                                   
                                                                                                                         
            ulData                                                                                                       
             < data >            数据内容，表示512BYTE的内容，每个字节的内容均相同。                                     
             0       字节内容为0x00                                                                                      
             1       字节内容为0x55                                                                                      
             2       字节内容为0xAA                                                                                      
             3       字节内容为0xFF                                                                                      
                                                                                                                         
 输出参数  : pulErr                                                                                                      
 返 回 值  : 0 ：OK  非 0 ：Error                                                                                        
                                                                                                                         
            具体的错误值填充在*pulErr中                                                                                  
            0 表示SD卡不在位                                                                                             
            1 表示SD卡初始化失败                                                                                         
            2 表示<opr>参数非法，对应操作不支持(该错误由AT使用,不需要底软使用)                                           
            3 表示<address>地址非法，超过SD卡本身容量                                                                    
            4 其他未知错误                                                                                               
*****************************************************************************/   
unsigned long BSP_SDMMC_ATProcess(unsigned int ulOp,unsigned long ulAddr, unsigned long ulData,u8 *pucBuffer,unsigned long *pulErr)
{
	int ret = 0x0;		
	struct scatterlist *sg;
	u8 *pucBufferlocal;

	/*参数判断*/
	if (NULL == pulErr)
	{
		return (u32)ERROR;
	}

	/*防止在任务执行中又敲入AT命令*/
	if (SDMMC_PROCESS_IN == sdmmcATProcessOverFlag)
	{
		*pulErr = SDMMC_OTHER_ERROR;
		return (u32)ERROR;
	}

	sdmmcATProcessOverFlag = SDMMC_PROCESS_IN;

	/*参数判断*/
	if (ulOp >= SDMMC_AT_BUTT)
	{
		*pulErr = SDMMC_PARAMETER_ERROR;
		sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;	
		return (u32)ERROR;   
	}

	/*判断卡是否在位*/
	if (OK != sd_get_status())
	{
		*pulErr = SDMMC_NOT_IN;
		sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
		return (u32)ERROR;
	}

	/*判断擦除，写，读命令是否超出卡的容量*/ 
	switch (ulOp)
	{
		case (SDMMC_AT_ERASE_ADDR) :
		case (SDMMC_AT_WRTIE_ADDR) :
		case (SDMMC_AT_READ)       :   
		{
			
			if ((ulAddr + SDMMC_ONE_BLOCK) > (unsigned long)sd_get_capacity())
			{
				sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
				*pulErr = SDMMC_ADDR_ERROR;
				return (u32)ERROR;
			}	

			break;
		}       
		default :
		{
		    ;
		}
	}

	/*读操作*/
	if (SDMMC_AT_READ == ulOp)
	{
		sg = kzalloc(sizeof(struct scatterlist), GFP_KERNEL);
        if(NULL == sg)
        {
            printk(KERN_INFO "####### BSP_SDMMC_ATProcess: sg kzalloc fail! #######\n");
            return (u32)ERROR;
        }
        pucBufferlocal = (u8 *)kmalloc(SDMMC_BLOCK_BYTE, GFP_KERNEL);
        if(NULL == pucBufferlocal)
        {
            kfree(sg);
            sg = NULL;
            printk(KERN_INFO "####### BSP_SDMMC_ATProcess: pucBufferlocal kzalloc fail! #######\n");
            return (u32)ERROR;
        }
		sg_init_one(sg, pucBufferlocal, SDMMC_BLOCK_BYTE);
		ret = sd_transfer(sg, ulAddr, 1, SDMMC_BLOCK_BYTE, 0);
		if (OK != ret)
		{
			*pulErr = SDMMC_OTHER_ERROR;
			sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
			kfree(sg);
			kfree(pucBufferlocal);
			return (u32)ERROR;
		}
		memcpy(pucBuffer, pucBufferlocal, SDMMC_BLOCK_BYTE);
		sdmmcATProcessOverFlag = SDMMC_PROCESS_OVER_OR_UNDO;
		kfree(sg);
		kfree(pucBufferlocal);
		return SUCCESS;
	}
	
	//SDMMC_ATPROCESS_TASK_ARG task_arg;
	task_arg.ulOp = (int)ulOp;
	task_arg.ulAddr = (int)ulAddr;  
	task_arg.ulData = (int)ulData;
	task_arg.pulErr = (int *)pulErr;
	/*格式化，擦除，写操作*/
	ret = kernel_thread(sdmmcATProcessTask, 0, CLONE_KERNEL);
	if (ret < 0)
	{
		*pulErr = SDMMC_OTHER_ERROR;
		sdmmcATProcessOverFlag = SDMMC_PROCESS_ERROR;
		return (u32)ERROR;
	}
	return SUCCESS;
}

/*****************************************************************************                                           
 函 数 名  : BSP_SDMMC_GetOprtStatus                                                                                          
 功能描述  : at^sd,SD卡当前操作状态                                                                                      
 输入参数  : 无                                                                                                          
 输出参数  : 无                                                                                                          
 返 回 值  :                                                                                                             
            0: 未操作或操作已经完成;                                                                                     
            1: 操作中 
            2: 上次操作失败
*****************************************************************************/ 
unsigned long BSP_SDMMC_GetOprtStatus(void)
{
	return sdmmcATProcessOverFlag;
}

/*****************************************************************************
 函 数 名  : BSP_SDMMC_GetSDStatus
 功能描述  : 返回SD卡在位状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   1在位
            0不在位

*****************************************************************************/
int BSP_SDMMC_GetSDStatus(void)
{
	return (1 + sd_get_status());
}

EXPORT_SYMBOL(BSP_SDMMC_ATProcess);
EXPORT_SYMBOL(BSP_SDMMC_GetOprtStatus);
EXPORT_SYMBOL(BSP_SDMMC_GetSDStatus);


/*just for test*/
u32 sdmmcAtTest(u32 ulOp,u32 ulAddr, u32 ulData)
{
	int ret = 0;
	unsigned char *pucBuffer = NULL;
	u32 ulErr = 0;

	pucBuffer = kzalloc(SD_BUFFER_SIZE, GFP_KERNEL);
    if(NULL == pucBuffer)
    {
        printk(KERN_INFO "######ST^SD TEST: pucBuffer kzalloc fail!#######\n");
        return (u32)1;
    }
	ret = (int)BSP_SDMMC_ATProcess(ulOp, ulAddr, ulData, pucBuffer, &ulErr); /*clean lnit e713*/
    /*lint +e64*/
	if(SUCCESS != ret)
	{
        kzfree(pucBuffer);
        pucBuffer = NULL;
		printk("######ST^SD TEST: command SDMMC_AT fail!#######\n");
		return (u32)1;
	}

    kzfree(pucBuffer);
    pucBuffer = NULL;
	return (u32)0;
}

/*at^sd interface end*/

