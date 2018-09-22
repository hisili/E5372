/*
 *  linux/drivers/mmc/sdio_ops.c
 *
 *  Copyright 2006-2007 Pierre Ossman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */

#include <linux/scatterlist.h>

#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sdio.h>

#include "core.h"
#include "sdio_ops.h"
#include <linux/skbuff.h>
extern int debug_flag;
#ifdef USING_TTF_SKB
#include "BSP.h"
#endif

#define MAX_SG_LIST_NUM 100
struct scatterlist gSg_list[MAX_SG_LIST_NUM];

bool balong_sg_init(struct scatterlist* sg,unsigned char* buf,unsigned int sg_num,unsigned int total_len)
{
	BCMDATA bcmData = (BCMDATA)buf;
	unsigned int i = 0;
	unsigned int sg_total_len = 0;
	int ret = 0;
	unsigned int temp_sg_num = sg_num;
	
	sg_init_table(sg, sg_num);
	while(bcmData != NULL && temp_sg_num > 0)
	{	
		//sg_set_buf(sg, bcmData->buf + bcmData->offset, bcmData->len);
		sg_total_len += bcmData->len;
		//add for testing  2011-11-15
#ifdef USING_TTF_SKB
		if(debug_flag == 1)
		{
			printk("SKB buf:0x%x len:%d dma_addr:0x%x dma_len:%d\n",(bcmData->buf + bcmData->offset),
				bcmData->len,sg->dma_address,sg->length);
			
			printk("bcmData->private_mem.enType:%d\n",bcmData->private_mem.enType);
		}
		if ( MEM_TYPE_SYS_DEFINED != bcmData->private_mem.enType )
		{
			sg->dma_address = TTF_VIRT_TO_PHY((bcmData->buf + bcmData->offset));
			sg->length = bcmData->len;
			if(debug_flag == 1)
			{
				printk("%s USING TTF SKB buf:0x%x len:%d dma_addr:0x%x dma_len:%d\n",__func__,(bcmData->buf + bcmData->offset),
					bcmData->len,sg->dma_address,sg->length);
			}
		}
		else
		{
			sg_set_buf(sg, bcmData->buf + bcmData->offset, bcmData->len);
			sg->dma_address = 0;
			if(debug_flag == 1)
			{
				printk("%s USING SYSTEM SKB buf:0x%x len:%d dma_addr:0x%x dma_len:%d\n",__func__,(bcmData->buf + bcmData->offset),
					bcmData->len,sg->dma_address,sg->length);
			}
		}
#else
        sg_set_buf(sg, bcmData->buf + bcmData->offset, bcmData->len);
#endif

		bcmData = bcmData->next;
		sg++;
		i++;
		temp_sg_num--;
	}

	if(i != sg_num)
	{
		printk("%s i:%d sg_num:%d\n",__func__,i,sg_num);
		ret = -1;
	}

	if(total_len != sg_total_len)
	{
		printk("%s total_len:%d sg_total_len:%d\n",__func__,total_len,sg_total_len);
		ret = -1;
	}

	if(debug_flag == 1)
	{
		printk("%s i:%d sg_num:%d\n",__func__,i,sg_num);
		printk("%s total_len:%d sg_total_len:%d\n",__func__,total_len,sg_total_len);
	}
	
	if(ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int mmc_send_io_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr)
{
	struct mmc_command cmd;
	int i, err = 0;

	BUG_ON(!host);     /*lint !e730*/

	memset(&cmd, 0, sizeof(struct mmc_command));

	cmd.opcode = SD_IO_SEND_OP_COND;
	cmd.arg = ocr;
	cmd.flags = MMC_RSP_SPI_R4 | MMC_RSP_R4 | MMC_CMD_BCR;

	for (i = 100; i; i--) {
		err = mmc_wait_for_cmd(host, &cmd, MMC_CMD_RETRIES);
		if (err)
			break;

		/* if we're just probing, do a single pass */
		if (ocr == 0)
			break;

		/* otherwise wait until reset completes */
		if (mmc_host_is_spi(host)) {
			/*
			 * Both R1_SPI_IDLE and MMC_CARD_BUSY indicate
			 * an initialized card under SPI, but some cards
			 * (Marvell's) only behave when looking at this
			 * one.
			 */
			if (cmd.resp[1] & MMC_CARD_BUSY)
				break;
		} else {
			if (cmd.resp[0] & MMC_CARD_BUSY)
				break;
		}

		err = -ETIMEDOUT;

		mmc_delay(10);
	}

	if (rocr)
		*rocr = cmd.resp[mmc_host_is_spi(host) ? 1 : 0];

	return err;
}

static int mmc_io_rw_direct_host(struct mmc_host *host, int write, unsigned fn,
	unsigned addr, u8 in, u8 *out)
{
	struct mmc_command cmd;
	int err;

	BUG_ON(!host);        /*lint !e730*/
	BUG_ON(fn > 7);        /*lint !e730*/

	/* sanity check */
	if (addr & ~0x1FFFF)
		return -EINVAL;

	memset(&cmd, 0, sizeof(struct mmc_command));

	cmd.opcode = SD_IO_RW_DIRECT;
	cmd.arg = write ? 0x80000000 : 0x00000000;
	cmd.arg |= fn << 28;
	cmd.arg |= (write && out) ? 0x08000000 : 0x00000000;
	cmd.arg |= addr << 9;
	cmd.arg |= in;
	cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_AC;

	err = mmc_wait_for_cmd(host, &cmd, 0);
	if (err)
		return err;

	if (mmc_host_is_spi(host)) {
		/* host driver already reported errors */
	} else {
		if (cmd.resp[0] & R5_ERROR)
			return -EIO;
		if (cmd.resp[0] & R5_FUNCTION_NUMBER)
			return -EINVAL;
		if (cmd.resp[0] & R5_OUT_OF_RANGE)
			return -ERANGE;
	}

	if (out) {
		if (mmc_host_is_spi(host))
			*out = (cmd.resp[0] >> 8) & 0xFF;
		else
			*out = cmd.resp[0] & 0xFF;
	}

	return 0;
}

int mmc_io_rw_direct(struct mmc_card *card, int write, unsigned fn,
	unsigned addr, u8 in, u8 *out)
{
	BUG_ON(!card);          /*lint !e730*/
	return mmc_io_rw_direct_host(card->host, write, fn, addr, in, out);
}

/**
 * sg_init_one - Initialize a single entry sg list
 * @sg:		 SG entry
 * @buf:	 Virtual address for IO
 * @buflen:	 IO length
 *
 **/
void sg_init_one_exp(struct scatterlist *sg, const void *buf, unsigned int buflen)
{
	sg_init_table(sg, 1);
    
#ifdef USING_TTF_SKB
	if(debug_flag == 1)
	{
		printk("sg_init_one_exp SKB buf:0x%x len:%d dma_addr:0x%x dma_len:%d\n",buf,
		buflen,sg->dma_address,sg->length);
	}

	if(buf >= TTFA9_DRAM_SHARE_VIRT)
	{
		sg->dma_address = TTF_VIRT_TO_PHY(buf);
		sg->length = buflen;
		if(debug_flag == 1)
		{
			printk("sg_init_one_exp USING TTF SKB buf:0x%x len:%d dma_addr:0x%x dma_len:%d\n",buf,
			buflen,sg->dma_address,sg->length);
		}
	}	
	else
	{
		sg_set_buf(sg, buf, buflen);
		sg->dma_address = 0;
		if(debug_flag == 1)
		{
			printk("sg_init_one_exp USING SYSTEM SKB buf:0x%x len:%d dma_addr:0x%x dma_len:%d\n",buf,
				buflen,sg->dma_address,sg->length);
		}
	}
#else
    sg_set_buf(sg, buf, buflen);
#endif
}

int mmc_io_rw_extended(struct mmc_card *card, int write, unsigned fn,
	unsigned addr, int incr_addr, u8 *buf, unsigned blocks, unsigned blksz)
{
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
	struct scatterlist sg;

	BUG_ON(!card);        /*lint !e730*/
	BUG_ON(fn > 7);       /*lint !e730*/
	BUG_ON(blocks == 1 && blksz > 512);    /*lint !e730*/
	WARN_ON(blocks == 0);
	WARN_ON(blksz == 0);

	/* sanity check */
	if (addr & ~0x1FFFF)
		return -EINVAL;

	memset(&mrq, 0, sizeof(struct mmc_request));
	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));

	mrq.cmd = &cmd;
	mrq.data = &data;

	cmd.opcode = SD_IO_RW_EXTENDED;
	cmd.arg = write ? 0x80000000 : 0x00000000;
	cmd.arg |= fn << 28;
	cmd.arg |= incr_addr ? 0x04000000 : 0x00000000;
	cmd.arg |= addr << 9;
	if (blocks == 1 && blksz <= 512)
		cmd.arg |= (blksz == 512) ? 0 : blksz;	/* byte mode */
	else
		cmd.arg |= 0x08000000 | blocks;		/* block mode */
	cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

	data.blksz = blksz;
	data.blocks = blocks;
	data.flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;

	//sg_init_one(&sg, buf, blksz * blocks);
	//wzw
	sg_init_one_exp(&sg, buf, blksz * blocks);
	
	mmc_set_data_timeout(&data, card);

	mmc_wait_for_req(card->host, &mrq);

	if (cmd.error)
		return (int)(cmd.error); /*clean lint e713*/
	if (data.error)
		return (int)(data.error); /*clean lint e713*/

	if (mmc_host_is_spi(card->host)) {
		/* host driver already reported errors */
	} else {
		if (cmd.resp[0] & R5_ERROR)
			return -EIO;
		if (cmd.resp[0] & R5_FUNCTION_NUMBER)
			return -EINVAL;
		if (cmd.resp[0] & R5_OUT_OF_RANGE)
			return -ERANGE;
	}

	return 0;
}

int mmc_io_rw_extended_chain(struct mmc_card *card, int write, unsigned fn,
	unsigned addr, int incr_addr, u8 *buf, unsigned blocks, unsigned blksz)
{
	struct mmc_request mrq;
	struct mmc_command cmd;
	struct mmc_data data;
	struct scatterlist sg;
	struct balong_sdio_host* sdio_host = NULL;//w00176398
	bool bRet = false;

	BUG_ON(!card);       /*lint !e730*/
	BUG_ON(fn > 7);       /*lint !e730*/
	BUG_ON(blocks == 1 && blksz > 512);   /*lint !e730*/
	WARN_ON(blocks == 0);
	WARN_ON(blksz == 0);

	/* sanity check */
	if (addr & ~0x1FFFF)
		return -EINVAL;

	memset(&mrq, 0, sizeof(struct mmc_request));
	memset(&cmd, 0, sizeof(struct mmc_command));
	memset(&data, 0, sizeof(struct mmc_data));

	mrq.cmd = &cmd;
	mrq.data = &data;

	cmd.opcode = SD_IO_RW_EXTENDED;
	cmd.arg = write ? 0x80000000 : 0x00000000;
	cmd.arg |= fn << 28;
	cmd.arg |= incr_addr ? 0x04000000 : 0x00000000;
	cmd.arg |= addr << 9;
	if (blocks == 1 && blksz <= 512)
		cmd.arg |= (blksz == 512) ? 0 : blksz;	/* byte mode */
	else
		cmd.arg |= 0x08000000 | blocks;		/* block mode */
	cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

	data.blksz = blksz;
	data.blocks = blocks;
	data.flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
	data.sg = &sg;
	data.sg_len = 1;

	sdio_host = mmc_priv(card->host);
	if(NULL == sdio_host)
	{
		printk("%s:Get sdio host pointer failed!!!\n",__func__);
		return -EINVAL;
	}

	memset(gSg_list, 0 , sizeof(gSg_list));

	bRet = balong_sg_init(gSg_list,buf,sdio_host->buf_dma_len,blksz * blocks);
	if(!bRet)
	{
		printk("%s sg init failed!!!\n",__func__);
		return -EINVAL;
	}
	data.sg = gSg_list;
	data.sg_len = sdio_host->buf_dma_len;

	mmc_set_data_timeout(&data, card);

	mmc_wait_for_req(card->host, &mrq);

	if (cmd.error)
		return (int)(cmd.error); /*clean lint e713*/
	if (data.error)
		return (int)(data.error); /*clean lint e713*/

	if (mmc_host_is_spi(card->host)) {
		/* host driver already reported errors */
	} else {
		if (cmd.resp[0] & R5_ERROR)
			return -EIO;
		if (cmd.resp[0] & R5_FUNCTION_NUMBER)
			return -EINVAL;
		if (cmd.resp[0] & R5_OUT_OF_RANGE)
			return -ERANGE;
	}

	return 0;
}
int sdio_reset(struct mmc_host *host)
{
	int ret;
	u8 abort;

	/* SDIO Simplified Specification V2.0, 4.4 Reset for SDIO */

	ret = mmc_io_rw_direct_host(host, 0, 0, SDIO_CCCR_ABORT, 0, &abort);
	if (ret)
		abort = 0x08;
	else
		abort |= 0x08;

	ret = mmc_io_rw_direct_host(host, 1, 0, SDIO_CCCR_ABORT, abort, NULL);
	return ret;
}

