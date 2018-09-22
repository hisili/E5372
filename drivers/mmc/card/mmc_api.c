#include <linux/mmc/core.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>

#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <linux/random.h>
#include <linux/module.h>
#include "BSP.h"
#include "../host/hisdio.h"
#include "mmc_api.h"


#if 0


BSP_S32 sd_mmc_format(BSP_VOID)
{
}




#endif



extern struct mmc_host *g_host; 
extern struct hisdio_host *sdio_host_copy;

unsigned char* g_ulbakAddr;

ST_SOC_REG_ADDR_INFO g_astSocSDIORegTable[] =
{
    {MMC_CTRL,         4},
    {MMC_PWREN,        4},
    {MMC_CLKDIV,       4},
    {MMC_CLKSRC,       4},
    {MMC_TMOUT,        4},
    {MMC_CTYPE,        4},
    {MMC_BLKSIZ,       4},
    {MMC_INTMASK,      4},
    {MMC_FIFOTH,       4},
    {MMC_DEBNCE,       4},
    {MMC_UHSREG,       4},
    {MMC_BMOD,         4},
    {MMC_DBADDR,       4},
    {MMC_IDINTEN,      4},
};

static int mmc_wait_busy(struct mmc_card *card)
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

		if (!busy && !(cmd.resp[0] & R1_READY_FOR_DATA)) 
		{
			busy = 1;
			printk(KERN_INFO "%s: Warning: Host did not "
			 "wait for busy state to end.\n",
			 mmc_hostname(card->host));
		}
	} while (!(cmd.resp[0] & R1_READY_FOR_DATA));

	return ret;
}


/*lint -e713*/
static int mmc_check_result(struct mmc_card *card,struct mmc_request *mrq)
{
	int ret;

	BUG_ON(!mrq || !mrq->cmd || !mrq->data);/*lint !e730 !e746*/

	ret = 0;

    /*上面已经有BUG_ON判断*/
    /*lint -e613*/
	if (!ret && mrq->cmd->error)
	{
		printk("mmc_check_result: cmd->error!\n");
		ret = mrq->cmd->error;
	}
	if (!ret && mrq->data->error)
	{
		printk("mmc_check_result: data->error!\n");
		ret = mrq->data->error;
	}
	if (!ret && mrq->stop && mrq->stop->error)
	{	
		printk("mmc_check_result: stop->error!\n");
		ret = mrq->stop->error;
	}
	if (!ret && (mrq->data->bytes_xfered != mrq->data->blocks * mrq->data->blksz))
	{
		printk("mmc_check_result: RESULT_FAIL!\n");
		printk("ret=%#x,bytes_xfered=%#x,blocks=%#x,blksz=%#x\n",
			ret,mrq->data->bytes_xfered,mrq->data->blocks,mrq->data->blksz);
		ret = RESULT_FAIL;
	}

	if (ret == -EINVAL)
	{
		printk("mmc_check_result: RESULT_UNSUP_HOST!\n");
		
		ret = RESULT_UNSUP_HOST;
	}
    /*lint +e613*/
	
	return ret;

}
/*lint +e713*/

static void mmc_prepare_mrq(struct mmc_card *card,
struct mmc_request *mrq, struct scatterlist *sg, unsigned sg_len,
unsigned dev_addr, unsigned blocks, unsigned blksz, int write)
{
	BUG_ON(!mrq || !mrq->cmd || !mrq->data || !mrq->stop);          /*lint !e730*/

    /*上面已经有BUG_ON判断*/
    /*lint -e613*/
	if (blocks > 1) 
	{
		mrq->cmd->opcode = write ?
			MMC_WRITE_MULTIPLE_BLOCK : MMC_READ_MULTIPLE_BLOCK;
	} 
	else 
	{
		mrq->cmd->opcode = write ?
			MMC_WRITE_BLOCK : MMC_READ_SINGLE_BLOCK;
	}

	mrq->cmd->arg = dev_addr;
	if (!mmc_card_blockaddr(card))
	{
		mrq->cmd->arg <<= 9;
	}

	mrq->cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;

	if (blocks == 1)
	{
		mrq->stop = NULL;
	}
	else 
	{
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
int mmc_transfer(struct scatterlist *sg,unsigned dev_addr,
	unsigned blocks, unsigned blksz, int write)
{
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_command stop;
	struct mmc_data data;

	//static unsigned int ploop = 0;
	if(blocks > 256)
	{
		/*单次传输的最大长度为128KByte*/
		printk("mmc_transfer: The max block num in one req is 256!\n");
		return -1;
	}
	if (NULL == g_host)
	{
		printk("mmc_transfer: g_host is null!\n");
		return -1;
	}

	mmc_claim_host(g_host); 

	//printk("[%x:%.8x:%x]\n",write,dev_addr,(blksz * blocks));
	//if (++ploop%8 == 0)
	//    printk("\n");

	memset(&mrq, 0, sizeof(struct mmc_request));
	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));
	memset(&stop, 0, sizeof(struct mmc_command));

	mrq.cmd = &cmd;
	mrq.data = &data;
	mrq.stop = &stop;

	mmc_prepare_mrq(g_host->card,&mrq, sg, 1, dev_addr,
		blocks, blksz, write);

	mmc_wait_for_req(g_host, &mrq);

	mmc_wait_busy(g_host->card);

	mmc_release_host(g_host); 

	return mmc_check_result(g_host->card, &mrq);
}


BSP_S32 sd_mmc_blk_w(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len)
{
	int ret;	
	struct scatterlist sg;
	
	sg_init_one(&sg, pu8DataBuf, u32Len);   
	
	ret = mmc_transfer(&sg,u32StartBlk,u32Len/512,512,1);
	if (ret)
	{
		printk("mmc_transfer err! ret = %#x\n",ret);
		return ret;  
	}

	return BSP_OK;
}

BSP_S32 sd_mmc_blk_r(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len)
{
	int ret;	
	struct scatterlist sg;
	
	sg_init_one(&sg, pu8DataBuf, u32Len);   
	
	ret = mmc_transfer(&sg,u32StartBlk,u32Len/512,512,0);
	if (ret)
	{
		printk("mmc_transfer err!\n");
		return ret;  
	}

	return BSP_OK;
}

/* Send SD_ERASE_GROUP_START command*/  /*qiuxilong*/
BSP_S32 sd_mmc_send_erase_grp_start_cmd(struct mmc_host *host,BSP_U32 u32StartBlk )
{
    struct mmc_command cmd;
	int err;
	
	cmd.opcode = mmc_card_mmc(host->card)?MMC_ERASE_GROUP_START : SD_ERASE_GROUP_START;
	cmd.flags =  MMC_RSP_R1 | MMC_CMD_AC;
	
	cmd.arg = u32StartBlk;
	if (!mmc_card_blockaddr(host->card))
	{
		cmd.arg <<= 9;
	}
        
	err = mmc_wait_for_cmd(host, &cmd, 0); 
	if (err)
		return err;

	return 0;

}

/* Send SD_ERASE_GROUP_END command*/  /*qiuxilong*/
BSP_S32 sd_mmc_send_erase_grp_end_cmd(struct mmc_host *host,BSP_U32 u32EndBlk)
{
    struct mmc_command cmd;
	int err;
	
	cmd.opcode = mmc_card_mmc(host->card)?MMC_ERASE_GROUP_END : SD_ERASE_GROUP_END;
	cmd.flags =  MMC_RSP_R1 | MMC_CMD_AC;
	
	cmd.arg = u32EndBlk;
	if (!mmc_card_blockaddr(host->card))
	{
		cmd.arg <<= 9;
	}
        
	err = mmc_wait_for_cmd(host, &cmd, 0); 
	if (err)
		return err;

	return 0;

}


/* Send SD_MMC_ERASE command*/  /*qiuxilong*/
BSP_S32 sd_mmc_send_erase_cmd(struct mmc_host *host)
{
    struct mmc_command cmd;
	int err;
	
	cmd.opcode = MMC_ERASE;
	cmd.flags =  MMC_RSP_R1B | MMC_CMD_AC;
	cmd.arg = 0;
        
	err = mmc_wait_for_cmd(host, &cmd, 0); 
	if (err)
		return err;

	return 0;

}



BSP_S32 sd_mmc_blk_erase(BSP_U32 u32StartBlk,BSP_U32 u32EndBlk)
{
	int ret;	
	printk("enter sd_mmc_erase! \n");
	mmc_claim_host(g_host); 
	
	ret = sd_mmc_send_erase_grp_start_cmd(g_host,u32StartBlk);	
	if (ret)
	{
		printk("sd_mmc_send_erase_grp_start_cmd err! ret = %#x\n",ret);
		return ret;  
	}

	ret = sd_mmc_send_erase_grp_end_cmd(g_host,u32EndBlk);	
	if (ret)
	{
		printk("sd_mmc_send_erase_grp_start_cmd err! ret = %#x\n",ret);
		return ret;  
	}
	
	ret = sd_mmc_send_erase_cmd(g_host);	
	if (ret)
	{
		printk("sd_mmc_send_erase_grp_start_cmd err! ret = %#x\n",ret);
		return ret;  
	}

	
	mmc_wait_busy(g_host->card);
	
	mmc_release_host(g_host); 
	
	printk("leave sd_mmc_erase! \n");
	return 0;

}



/*****************************************************************************
* 函 数 名  : sd_mmc_get_status
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
BSP_S32 sd_mmc_get_status(BSP_VOID)
{
	if (NULL == g_host)
	{
		//printk("sd card not enumerated\n"); 
		return -1;
	}
	else
	{
		//printk("sd card enumerated\n"); 
		return 0;
	}
}
/*低功耗MMC 寄存器保存接口*/
BSP_U32 BSP_MMC_DrxBakReg(BSP_VOID)
{
    unsigned int ulStarAddr;
    unsigned char * ulbakAddr;
    unsigned int i;
    
	if (NULL == sdio_host_copy)
	{
		printk("BSP_MMC_DrxBakReg: g_host is null!\n");
		return (BSP_U32)BSP_ERROR;
	}
    g_ulbakAddr = kzalloc(sizeof(g_astSocSDIORegTable),GFP_KERNEL);
    if(NULL == g_ulbakAddr)
    {
        return (BSP_U32)BSP_ERROR;
    }

    ulbakAddr = g_ulbakAddr;
    for (i = 0; i < (sizeof(g_astSocSDIORegTable)/sizeof(ST_SOC_REG_ADDR_INFO)); i++)
    {
        ulStarAddr = (unsigned int)sdio_host_copy->base + g_astSocSDIORegTable[i].ulStartAddr;

        memcpy((void *)ulbakAddr, (void *)ulStarAddr, g_astSocSDIORegTable[i].ulLength);
        ulbakAddr += g_astSocSDIORegTable[i].ulLength;
    }

    return BSP_OK;
}
    
/*低功耗MMC 寄存器恢复 接口*/
BSP_VOID BSP_MMC_DrxRestoreReg(BSP_VOID)
{
    unsigned int ulStarAddr;
    unsigned char * ulbakAddr;
    unsigned int i;
    
    ulbakAddr = g_ulbakAddr;
    
    for (i = 0; i < (sizeof(g_astSocSDIORegTable)/sizeof(ST_SOC_REG_ADDR_INFO)); i++)
    {
        ulStarAddr = (unsigned int)sdio_host_copy->base + g_astSocSDIORegTable[i].ulStartAddr;
        memcpy((void *)ulStarAddr, (void *)ulbakAddr, g_astSocSDIORegTable[i].ulLength);
        ulbakAddr += g_astSocSDIORegTable[i].ulLength;
    }
    
    kzfree(g_ulbakAddr);
    return;

}
EXPORT_SYMBOL(sd_mmc_blk_w);
EXPORT_SYMBOL(sd_mmc_blk_r);
EXPORT_SYMBOL(sd_mmc_get_status);
EXPORT_SYMBOL(sd_mmc_blk_erase);
EXPORT_SYMBOL(BSP_MMC_DrxBakReg);
EXPORT_SYMBOL(BSP_MMC_DrxRestoreReg);


BSP_S32 st_sd_mmc_rw(BSP_U32 u32StartBlk, BSP_S32 s32BlkNum, BSP_S32 s32BlkSize,BSP_S32 s32Times)
{
	int ret,i,j;
    BSP_U8 *pu8RBuf = BSP_NULL;
    BSP_U8 *pu8WBuf = BSP_NULL;
	
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	pu8RBuf = kzalloc((size_t)(s32BlkNum * s32BlkSize), GFP_KERNEL);
	/*end*/
    if (BSP_NULL == pu8RBuf)
    {
        printk("malloc failed\n");
        return BSP_ERROR;
    }
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	pu8WBuf = kzalloc((size_t)(s32BlkNum * s32BlkSize), GFP_KERNEL);
	/*end*/
    if (BSP_NULL == pu8WBuf)
    {
        printk("malloc failed\n");
        kzfree(pu8RBuf);
        return BSP_ERROR;
    }

    if(s32Times <= 0)
    {
        s32Times = 1;
    }

	for (j = 0; j < s32BlkNum * s32BlkSize; j++)
	{
		pu8WBuf[j] = random32()&0xff;
	}


	for(i=0;i<s32Times;i++)
	{
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
	    ret = sd_mmc_blk_w(u32StartBlk, pu8WBuf, (BSP_U32)(s32BlkNum * s32BlkSize));
		/*end*/
	    if (ret)
    	{
			printk("sd_mmc_blk_w err! ret = %#x\n",ret);
			kzfree(pu8RBuf);
			kzfree(pu8WBuf);
			return ret;	 
		}		    
		
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
	    ret = sd_mmc_blk_r(u32StartBlk, pu8RBuf, (BSP_U32)(s32BlkNum * s32BlkSize));
		/*end*/
		if (ret)
        {
        	printk("sd_mmc_blk_r err! ret = %#x\n",ret);
			kzfree(pu8RBuf);
			kzfree(pu8WBuf);
		    return ret;	
        }		    
		else
		{
			for (j = 0; j < s32BlkNum * s32BlkSize; j++)
			{
				if (pu8RBuf[j] != pu8WBuf[j])
				{
					printk("[rw verify failed!]pu8RBuf[%d](=%#x) != %#x\n", 
						j,pu8RBuf[j], pu8WBuf[j]);

				}
			}
		}
	}
	
	kzfree(pu8RBuf);
	kzfree(pu8WBuf);
    return BSP_OK;
}


BSP_S32 st_sd_mmc_blk_erase(BSP_U32 u32StartBlk, BSP_U32 u32BlkNum)
{
    BSP_U32 s32BlkSize = 512;
	int ret=0;
	int j;
    BSP_U8 *pu8RBuf = BSP_NULL;
    BSP_U8 *pu8WBuf = BSP_NULL;
	
	pu8RBuf = kzalloc(u32BlkNum * s32BlkSize, GFP_KERNEL);
    if (BSP_NULL == pu8RBuf)
    {
        printk("malloc failed\n");
        return BSP_ERROR;
    }
	
	pu8WBuf = kzalloc(u32BlkNum * s32BlkSize, GFP_KERNEL);
    if (BSP_NULL == pu8WBuf)
    {
        printk("malloc failed\n");
        kzfree(pu8RBuf);
        return BSP_ERROR;
    }

    /*modified for lint e665 */
    memset((BSP_VOID *)pu8WBuf,0x55,(u32BlkNum * s32BlkSize));
	
	ret = sd_mmc_blk_w(u32StartBlk, pu8WBuf, u32BlkNum * s32BlkSize);
	if (ret)
	{
		printk("st_sd_mmc_blk_erase:sd_mmc_blk_w(s32StartBlk %d,s32BlkNum %d) failed,line %d!\n",u32StartBlk,u32BlkNum,__LINE__);
		kzfree(pu8RBuf);
		kzfree(pu8WBuf);
		return ret;  
	}			
    
    ret = sd_mmc_blk_erase(u32StartBlk, u32StartBlk+u32BlkNum-1);
    if(ret)
    {
		printk("st_sd_mmc_blk_erase:sd_mmc_blk_w(s32StartBlk %d,s32BlkNum %d) failed,line %d!\n",u32StartBlk,u32BlkNum,__LINE__);
		kzfree(pu8RBuf);
		kzfree(pu8WBuf);
        return BSP_ERROR;
    }

    /*modified for lint e665 */
    memset((BSP_VOID *)pu8RBuf,0xAA,(u32BlkNum * s32BlkSize));
	
	ret = sd_mmc_blk_r(u32StartBlk, pu8RBuf, u32BlkNum * s32BlkSize);
	if (ret)
	{
		printk("st_sd_mmc_blk_erase:sd_mmc_blk_r(s32StartBlk %d,s32BlkNum %d) failed,line %d!\n",u32StartBlk,u32BlkNum,__LINE__);
		kzfree(pu8RBuf);
		kzfree(pu8WBuf);
		return ret;  
	}			

	for (j = 0; j < (int)(u32BlkNum * s32BlkSize); j++)
	{
	
		printk("pu8RBuf[%d](=%#x),pu8WBuf[%d](=%#x)\n", j,pu8RBuf[j], j,pu8WBuf[j]);
		/*
		if (pu8RBuf[j] != pu8WBuf[j])
		{
			printk("[rw verify failed!]pu8RBuf[%d](=%#x) != %#x\n", 
				j,pu8RBuf[j], pu8WBuf[j]);
	
		}
		*/
	}

    printk("st_sd_mmc_blk_erase:(s32StartBlk %d,s32BlkNum %d) pass,line %d!\n",u32StartBlk,u32BlkNum,__LINE__);
	kzfree(pu8RBuf);
	kzfree(pu8WBuf);
    return BSP_OK;
}


