/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
s*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_mtd.c
*
* Description: nand controller operations in dependence on  hardware,
*              this source file is only for linux platform not for  
*              platform follow:
*              __VXWORKS__ 
*              __BOOTLOADER__              
*              BSP_IMAGE_BOOTROM              
*              __RVDS__
*              __FASTBOOT__  
*            
* Description: nand controller operations in dependence on  hardware,
*              this source file is only for linux platform not for vxworks
*              platform.
*
*                   |                           Layer: MTD                   
*              nand_base.c
*                   |
*                   |                           Layer: porting 
*              nandc_mtd.c
*                   |
*                   |
*          --------------------
*          |                  | 
*          |(init)            |(operation)
*       nandc_host.c       nandc_ctrl.c                  Layer: controller   
*                     ----------------------- 
*                             |
*                       nandc_vxxx.c(nandc_v400.c)       Layer: hardware                  
*
*
* Function List:
*
* History:
* date:2011-07-27
* question number:
* modify reasion:         create
*******************************************************************************/
#include "nandc_mtd.h"
#include "nandc_balong.h"


#define NFCDBGLVL(LVL)      (NANDC_TRACE_MTD|NANDC_TRACE_##LVL)
extern struct nand_bus  nand_handle;

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_dump_reg() - view the value of nand controller's registers.
 *
 * PARAMETER:
 * @headstr - [input]it is a character string describe which function calls it.
 * @mtd - [input]through this structure pointer to get nand controler registers's base 
 *        address,and it is virtual address.
 *
 * DESCRIPTION:
 * This function is a debug function.you can see nand controller registers's values 
 * by calling it.
 * 
 * CALL FUNC:
 * () - 
 * 
 ********************************************************************************/
void nandc_dump_reg(char* headstr,struct mtd_info *mtd)
{

	struct nand_chip *chip = mtd->priv;
	struct nandc_host *host = chip->priv;
	void __iomem * dumpaddr = host->regbase;
	
	char* buf = NULL;
	char* buf2 = NULL; 
    
	return;
    
	buf2 = kmalloc(64,GFP_KERNEL);
	if(0 == buf2)/*&&(yaffs_dbg != 1))*/
		return;
    
	memcpy(buf2,dumpaddr,64);
	buf = buf2;
	
	printk("%s\n",headstr);
		
	printk("dump reg:\n0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
	          *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
	          *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
	buf+=32;
	printk("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
              *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
              *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));

	kfree(buf2);

}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_cmd_ctrl() - transfer command and parameter to low layer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @dat - [input]it contains address or command.
 * @ctrl - [input] control bit mask:
 *                 NAND_NCE:select the nand flash chip.
 *                 NAND_CLE:latch the command.
 *                 NAND_CTRL_CHANGE:command change.
 *                 NAND_ALE:latch the address.
 *                 
 * DESCRIPTION:
 * This function sets operation address to host->addr_physics[],and address is written to 
 * nand controller address register by low layer interface (NANDC_SET_REG_BITS).be the 
 * same as,the function also sets command to host->command,then command will be written to 
 * nand controller command register by low layer interface (NANDC_SET_REG_BITS).
 * 
 * CALL FUNC:
 * nand_command_lp() - /drivers/mtd/nand/nand_base.c
 * 
 ********************************************************************************/
static void nandc_mtd_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;

    /*set the operation(read/write/erase) address in nand flash chip*/
    if (ctrl & NAND_ALE)
    {
        /*addr_value is 0 or 1,corresponding to host->addr_physics[0] and host->addr_physics[1].
        host->addr_physics[0] and host->addr_physics[1] is 32 bits.*/
        unsigned int addr_value = 0;
        unsigned int addr_offset = 0;

        if (ctrl & NAND_CTRL_CHANGE)
        {
            host->addr_cycle = 0x0;
            host->addr_physics[0] = 0x0;
            host->addr_physics[1] = 0x0;
        }
        addr_offset =  host->addr_cycle << 3;  /* 2 power of 3 equal 8,the result is mutiple of 8 .*/

        if (host->addr_cycle >= NANDC_ADDR_CYCLE_MASK)
        {
            addr_offset = (host->addr_cycle - NANDC_ADDR_CYCLE_MASK) << 3;
            addr_value = 1;
        }

        host->addr_physics[addr_value] |= ((dat & 0xff) << addr_offset);

        host->addr_cycle ++;
    }

    if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE))
    {
        host->command = dat & 0xff;
        if((chip->options & NAND_QUARY_BAD) && (dat == NAND_CMD_READSTART))
        {
            host->options = NANDC_OPTION_ECC_FORBID;
            host->length = mtd->writesize + mtd->oobsize;
            chip->options &= ~NAND_QUARY_BAD;
        }
        else
        {
            host->options = 0; /*NANDC_OPTION_DMA_ENABLE;*/
        }
       
        host->addr_real = NANDC_ADDR_INVALID;
        
        if((NAND_CMD_ERASE2 != host->command) && (NAND_CMD_ERASE1 != host->command)
            &&(!(NAND_QUARY_BAD & chip->options)))
        host->addr_physics[0] &= 0xffff0000; /*mask offset in page*/

        
        nandc_ctrl_entry(host);
    }

    if ((dat == NAND_CMD_NONE) && host->addr_cycle)
    {
        if (host->command == NAND_CMD_SEQIN 
            || host->command == NAND_CMD_READ0 
            || host->command == NAND_CMD_READID)
        {
            host->offset = 0x0;
            host->column = host->addr_physics[0] & 0xffff;
        }
    }
}
/*****************************************************************************/

static int nandc_mtd_dev_ready(struct mtd_info *mtd)
{
    return 0x1;
}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_select_chip() - determines which nand flash chip to operate. 
 *
 * PARAMETER:
 * @mtd - [input]through it to get the main structure host.
 * @chipselect - [input]chip number.
 * 
 * DESCRIPTION:
 * This function determines which nand flash chip to operate.(There may be more than
 * one nand flash chip)  
 *
 * CALL FUNC:
 * chip->select_chip()- 
 * 
 ********************************************************************************/
static void nandc_mtd_select_chip(struct mtd_info *mtd, int chipselect)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;

    if (chipselect < 0)
        return;

    if (chipselect >= NANDC_MAX_CHIPS)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("invalid chipselect: %d\n", chipselect));/*lint !e778*/
    }

	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
    host->chipselect = (u32)chipselect;
	/*end*/
}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_read_byte() - read byte from nand controller buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 *
 * DESCRIPTION:
 * This function reads byte from nand controller buffer.
 * host->databuf is virtual address for nand controller buffer.
 * 
 * CALL FUNC:
 * chip->read_byte() - /drivers/mtd/nand/nand_base.c 
 * 
 ********************************************************************************/
static uint8_t nandc_mtd_read_byte(struct mtd_info *mtd)
{
    unsigned char value = 0;//pclint 734
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    
    if (host->command == NAND_CMD_STATUS)
    {
        return readb(chip->IO_ADDR_R);
    }

    if (host->IsUseBuffer)
    {
        value = readb(host->databuf + host->column + host->offset);
    }
    else
    {
        value = readb(chip->IO_ADDR_R + host->column + host->offset);
    }

#if 0
	if(0)
	{
	    char *buf;
		printk("chip->IO_ADDR_R:0x%x  column:0x%x offset:0x%x,0x%x\n",(u32)chip->IO_ADDR_R , host->column, host->offset,value);

		buf = (char*)((u32)chip->IO_ADDR_R + (u32)host->column + (u32)host->offset);
		printk("read byte:\n0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			                  *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
			                  *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
		buf+=32;
		printk("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			                  *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
			                  *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
		nandc_dump_reg("read byte",mtd);
	}
#endif

    host->offset++;
    
    return value;
}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_read_word() -  read word from nand controller buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * 
 * DESCRIPTION:
 * This function reads word from nand controller buffer.
 * host->databuf is virtual address for nand controller buffer.
 *  
 * CALL FUNC:
 * chip->read_word() - /drivers/mtd/nand/nand_base.c
 * 
 ********************************************************************************/
static u16 nandc_mtd_read_word(struct mtd_info *mtd)
{
    u16 value = 0;
    struct nand_chip  *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 comoffset = host->column << 1;


    if (host->IsUseBuffer)
    {
        value = readw(host->databuf + comoffset + host->offset);
    }
    else
    {
        value = readw(chip->IO_ADDR_R + comoffset + host->offset);
    }


#if 0
	if(0)
	{
	    char* buf = 0;
		printk("word chip->IO_ADDR_R:0x%x  column:0x%x offset:0x%x,0x%x\n", (u32)chip->IO_ADDR_R , (u32)comoffset, host->offset,value);
		buf = (char*)((u32)chip->IO_ADDR_R + (u32)host->column + (u32)host->offset);
		printk("read word:\n0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			                  *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
			                  *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
		buf+=32;
		printk("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			                  *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
			                  *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
	}
#endif

    host->offset += 2;
    
    return value;
}/*lint !e529*/

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_read_buf() - read data from nand controller buffer to ram buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @buf - [input]data destination address in ram.
 * @len - [input]data len.
 * 
 * DESCRIPTION:
 * This function read data from nand controller buffer to ram buffer.  
 * 
 * CALL FUNC:
 * chip->read_buf() - /drivers/mtd/nand/nand_base.c
 * 
 ********************************************************************************/
static void nandc_mtd_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 comoffset = host->column;

    if( nandc_bus_16 == host->nandchip->busset)
    {
        comoffset = comoffset << 1;
    }
    
    if (host->IsUseBuffer)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy(buf, host->databuf + comoffset + host->offset, (unsigned int)len);
		/*end*/
    }
    else
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy(buf, (unsigned char *)(chip->IO_ADDR_R)  
            + comoffset + host->offset, (unsigned int)len);
		/*end*/
    }

#if 0
	if(0)
	{
		printk("buf chip->IO_ADDR_R:0x%x  column:0x%x offset:0x%x,0x%x\n", (u32)chip->IO_ADDR_R , (u32)comoffset, host->offset,len);
		//buf = chip->IO_ADDR_R + host->column + host->offset;

		printk("read buf:\n0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			                  *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
			                  *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
		buf+=32;
		printk("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\
0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
			                  *(buf+0),*(buf+1),*(buf+2),*(buf+3),*(buf+4), *(buf+5),*(buf+6),*(buf+7),*(buf+8),*(buf+9),*(buf+10),*(buf+11),*(buf+12),*(buf+13),*(buf+14),*(buf+15),
			                  *(buf+16),*(buf+17),*(buf+18),*(buf+19), *(buf+20),*(buf+21),*(buf+22),*(buf+23), *(buf+24),*(buf+25),*(buf+26),*(buf+27), *(buf+28),*(buf+29),*(buf+30),*(buf+31));
		nandc_dump_reg("read buf",mtd);
	}
#endif

    host->offset += (unsigned int)len;
    
}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_write_buf() - write data from ram buffer to nand controller buffer.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @buf - [input]data source address in ram.
 * @len - [input]data len.
 * 
 * DESCRIPTION:
 * This function writes data from ram buffer to nand controller buffer.
 * 
 * CALL FUNC:
 * chip->write_buf() - /drivers/mtd/nand/nand_base.c 
 * 
 ********************************************************************************/
static void nandc_mtd_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
    struct nand_chip  *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 comoffset = host->column;
    
    if(nandc_bus_16 == host->nandchip->busset)
    {
        comoffset  <<= 1;
    }
   
    if (host->IsUseBuffer)
    {
	    /*TODO: bad flag must clear, for nand page size large then 2k, 4k, 8k*/	
		
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy(host->databuf + comoffset + host->offset, buf, (unsigned int)len);
		/*end*/
    }
    else
    {
	    /*bad flag must clear, for 2k page flash*/
   		memset(chip->IO_ADDR_W + NANDC_BUFSIZE_BASIC, 0xff, NANDC_BAD_FLAG_BYTE_SIZE);
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        memcpy((unsigned char *)(chip->IO_ADDR_W) 
            + comoffset + host->offset, buf, (unsigned int)len);
		/*end*/
    }

    host->offset += (unsigned int)len;
    
}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_nand_init() - initialize host -> nandchip 
 *
 * PARAMETER:
 * @mtd - [input]through it to get the host structure.
 * @parts - [input]store information for each partition in part[i]
 * @nr_parts - [nr_parts]partition amount
 *
 * DESCRIPTION:
 * This function initlizes host -> nandchip.
 *  
 * CALL FUNC:
 * nandc_mtd_probe() - 
 * 
 ********************************************************************************/
int nandc_mtd_nand_init(struct mtd_info *mtd, struct mtd_partition *parts, u32 nr_parts )
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = chip->priv;
    u32 buswidth = NAND_BUSWIDTH_16;

    host->IsUseBuffer = NANDC_FALSE ;
    host->chip  = chip;
    host->chipselect    = 0;

    if(mtd->writesize > nandc_size_2k)
    {
       host->IsUseBuffer = NANDC_TRUE;
    }

    if(NANDC_TRUE == host->IsUseBuffer)
    {
        host->databuf = kmalloc((NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE), GFP_KERNEL);
        //host->databuf = dma_alloc_coherent(NANDC_NULL, NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE, &host->dma_addr,GFP_ATOMIC)
        if (!host->databuf)
            return -ENOMEM;
        host->oobbuf = host->databuf + NAND_MAX_PAGESIZE;
    }

    buswidth = chip->options & NAND_BUSWIDTH_16;
   /*begin lint-Info 732: (Info -- Loss of sign (arg. no. 7) (int to unsigned int))*/
    if( !nandc_host_init_mtd( host,
                            mtd->writesize,
                            mtd->oobsize,
                            (u32)mtd->size,
                            mtd->erasesize,
                            buswidth,
                            (u32)chip->numchips,
                            parts,
                            nr_parts))         /*end*/
    {
        
        chip->ecc.layout  =  host->curpart->ecclayout;
        return 0;
        
    }
    else
    {
        return -ENOMEM;
    }

}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_host_init() - initialize the nand_chip structure.
 *
 * PARAMETER:
 * @mtd - [input]through it to get the nand_chip structure.
 * @pltdev - [input]platform data structure.
 * @phost - [input/output]
 *
 * DESCRIPTION:
 * This function initializes the nand_chip structure which contains many functions such 
 * as chip->read_buf and chip->write_buf that will be used for operation to nand 
 * (eg read,write etc.)
 *  
 * CALL FUNC:
 * nandc_mtd_probe() - 
 * 
 ********************************************************************************/
int nandc_mtd_host_init(struct mtd_info *mtd, struct platform_device *pltdev, struct nandc_host **phost)
{
    struct nand_chip *chip = mtd->priv;
    struct nandc_host *host = NULL;
    struct nand_bus* handle = &nand_handle;
    int result = 0;

    chip->cmd_ctrl      =   nandc_mtd_cmd_ctrl;
    chip->dev_ready     =   nandc_mtd_dev_ready;
    chip->select_chip   =   nandc_mtd_select_chip;
    chip->read_byte     =   nandc_mtd_read_byte;
    chip->read_word     =   nandc_mtd_read_word;
    chip->write_buf     =   nandc_mtd_write_buf;
    chip->read_buf      =   nandc_mtd_read_buf;
    
    chip->chip_delay    =   NANDC_CHIP_DELAY;
    chip->options       =   NAND_NO_AUTOINCR | NAND_SKIP_BBTSCAN;

    chip->ecc.layout    =   NULL;
    chip->ecc.mode      =   NAND_ECC_NONE;

    /*if(nandc_nand_create_host(&host))*/
    host = nandc_native_host_create(nandc_init_seed);
    nandc_nand_host = host;
    if(NULL == host)
    {
        dev_err(&pltdev->dev, "failed to nandc_nand_create_host.\n");
        return -ENOMEM;
    }
    (void)nand_balong_port_mtd();
    
    host->chip = chip ;
    host->mtd  = mtd;

    chip->priv = host;

    /*get virtual address for nand controller registers from physical address*/
    host->regbase = ioremap(pltdev->resource[0].start,
                    pltdev->resource[0].end - pltdev->resource[0].start + 1);
    
    if (!host->regbase)
    {
        dev_err(&pltdev->dev, "ioremap failed\n");
        kfree(host);
        return -EIO;
    }
    
    (void)nand_mutex_in(handle);

   /* nandc2_ctrl_init ,nandc3_ctrl_init or nandc4_ctrl_init 
      It saves configuration value for erery operation,when we do 
      one operation to nand flash chip later, we write the value
      to corresponding register.*/
    if(host->ctrlfunc->init_cmd)
    {
        host->ctrlfunc->init_cmd(&host->bitcmd,  host);
    }
    /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
    host->databuf  = (u8*)host->chip->IO_ADDR_R;
    host->oobbuf  = (u8*)host->chip->IO_ADDR_R + nandc_size_2k;
    host->bufbase = (u8*)host->chip->IO_ADDR_R;
    /*END  :y00206456 2012-04-26 Modified for pclint e124*/

    /*first ,try to find 8 bit width nand chip*/
    chip->options &= ~NAND_BUSWIDTH_16;

    if(nand_scan(mtd, NANDC_MAX_CHIPS)) 
    {
        /*if failed ,try to find 16 bit width nand chip*/
        chip->options |= NAND_BUSWIDTH_16;
        
        if (nand_scan(mtd, NANDC_MAX_CHIPS)) 
        {
            result = -ENXIO;
        }
    }
    
    (void)nand_mutex_out(handle);
    
    *phost = host;

    return result;
}

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_probe() - linux MTD layer initialization function.
 *
 * PARAMETER:
 * @dev - [input]through it to get the platform_device structure.
 *
 * DESCRIPTION:
 * This function mallocs ram space for the main structure corelative with MTD layer.
 *  
 * CALL FUNC:
 * driver_register() - 
 * 
 ********************************************************************************/
static int nandc_mtd_probe(struct device* dev)
{
    struct nandc_host *host = NULL;
    struct nand_chip  *chip = NULL;
    struct mtd_info   *mtd  = NULL;
    struct platform_device *pltdev = to_platform_device(dev); /* nandc_mtd_pltdev */
    struct mtd_partition *parts = NULL;
    struct mtd_partition *temt_parts = NULL ;
    static const char *part_probes[] = { "cmdlinepart", NULL, };
    int nr_parts = 0;
    int result = 0;
    int i;
    
    int size = sizeof(struct nand_chip) + sizeof(struct mtd_info);

	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
    chip = (struct nand_chip*)kmalloc((size_t)size, GFP_KERNEL);
    /*end*/
    if (!chip)
    {
        dev_err(&pltdev->dev, "nandc_mtd_probe failed to allocate device structure.\n");
        return -ENOMEM;
    }
    memset((void *)chip, 0, size); /*clean the pclint e516*/
    mtd  = (struct mtd_info *)&chip[1];
    mtd->priv  = chip;
    mtd->owner = THIS_MODULE;
    mtd->name  = (char*)(pltdev->name);

    /*get virtual address for nand controller buffer from physical address*/
    chip->IO_ADDR_R = chip->IO_ADDR_W = ioremap_nocache(
                                    pltdev->resource[1].start,
                                    pltdev->resource[1].end - pltdev->resource[1].start + 1);
                                    
    if (!chip->IO_ADDR_R)
    {
        dev_err(&pltdev->dev, "ioremap failed\n");
        kfree(chip);
        return -EIO;
    }
    
    if (nandc_mtd_host_init(mtd, pltdev, &host))
    {
        dev_err(&pltdev->dev, "failed to nandc_mtd_host_init.\n");
        result = -ENOMEM;
        goto err;
    }
    dev->platform_data = host;

    mtd->name  = NANDC_NULL;
    /*get partition table from C-core bootloader flash partition table*/
    if(ptable_parse_mtd_partitions(&parts, &nr_parts))/*lint !e64*/
    {
        dev_err(&pltdev->dev, "failed to ptable_parse_mtd_partitions.\n");
        result = -ENOMEM;
        goto err;
    }   
    mtd->name  = (char*)(pltdev->name);


    if (nr_parts > 0) 
    {
        temt_parts = parts;
        for (i = 0; i < nr_parts; i++) 
        {
#if defined(NANDC_OPEN_DEBUG)
            printk("partitions[%d] = "
            "{.name = %s, .offset = 0x%.8llx, "".size = 0x%.8llx (%lluKiB) }\n",
            i,
            temt_parts->name,
            temt_parts->offset,
            temt_parts->size,
            (temt_parts->size)/1024);
#endif
            temt_parts++;
        }
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no.3) (int to unsigned int))*/
        if (nandc_mtd_nand_init(mtd, parts, (u32)nr_parts))
		/*end*/	
        {
            printk("ERROR ! nandc_mtd_nand_init!!!\n");
            result = -ENODEV;
            goto err;
        }
        
        return add_mtd_partitions(mtd, parts, nr_parts);
    }
    else
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("ERROR !No Partition !!!!\n"));/*lint !e778*/
        result = -ENODEV;
        goto err;
    }
    /*lint -e527*/
    if (!add_mtd_device(mtd))
        return 0;

    result = -ENODEV;
        nand_release(mtd);
   /*lint +e527*/ 
err:
    if (host->databuf)
        kfree(host->databuf);
    
    iounmap(chip->IO_ADDR_W);
    iounmap(host->regbase);
    kfree(host);
    dev->platform_data = NULL;

    return result;
}/*lint !e550 !e529*/

 /*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_remove() - 
 *
 * PARAMETER:
 * @dev - [input]the linux standard data structure.
 *
 * DESCRIPTION:
 *  
 * CALL FUNC:
 * () -  
 ********************************************************************************/
static int nandc_mtd_remove(struct device *dev)
{
    struct nandc_host *host = dev->platform_data;

    nand_release(host->mtd);
    kfree(host->databuf);
    iounmap(host->chip->IO_ADDR_W);
    iounmap(host->regbase);
    kfree(host);
    dev->platform_data = NULL;

    return 0;
}
/*****************************************************************************/

static void nandc_mtd_pltdev_release (struct device *dev)
{
    return ;
}
/*****************************************************************************/

static struct device_driver nandc_mtd_driver =
{
    .name       =   NANDC_NAME,
    .bus        =   &platform_bus_type,
    .probe      =   nandc_mtd_probe,
    .remove     =   nandc_mtd_remove,
};
/*****************************************************************************/

static struct resource nandc_mtd_resources[] = 
{
    [0] = {
        .start  = NANDC_REG_BASE_ADDR,
        .end    = NANDC_REG_BASE_ADDR + NANDC_REG_SIZE - 1,
        .flags  = IORESOURCE_MEM,
    },

    [1] = {
        .start  = NANDC_BUFFER_BASE_ADDR,
        .end    = NANDC_BUFFER_BASE_ADDR + NANDC_BUFSIZE_TOTAL - 1,
        .flags  = IORESOURCE_MEM,
    },
};
/*****************************************************************************/

static struct platform_device nandc_mtd_pltdev =
{
    .name           =   NANDC_NAME,
    .id             =   -1,

    .dev.platform_data     = NULL,
    .dev.dma_mask          = (u64*)~0,
    .dev.coherent_dma_mask = (u64) ~0,
    .dev.release           = nandc_mtd_pltdev_release,
    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
    .num_resources  = ARRAY_SIZE(nandc_mtd_resources),/*lint !e30 !e806*/
    /*lint +e516*/
    .resource       = nandc_mtd_resources,
};

/*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_module_init() - register dirver and device for nand driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver initialization interface.
 * 
 * CALL FUNC:
 * () - 
 * 
 ********************************************************************************/
static int __init nandc_mtd_module_init(void)
{
    int result;

    printk("Hisilicon Nand Flash Controller  Driver, Version 1.00\n");

    result = driver_register(&nandc_mtd_driver);
    if (result < 0)
    {
        return result;
    }

    result = platform_device_register(&nandc_mtd_pltdev);
    if (result < 0)
    {
        driver_unregister(&nandc_mtd_driver);
        return result;
    }

    return result;
}

/*******************************************************************************
 * FUNC NAME:  
 * nandc_mtd_module_exit() - unregister dirver and device for nand driver
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * Linux standard driver de_initialization interface. 
 * 
 * CALL FUNC:
 * () - 
 * 
 ********************************************************************************/
static void __exit nandc_mtd_module_exit (void)
{
    driver_unregister(&nandc_mtd_driver);
    platform_device_unregister(&nandc_mtd_pltdev);
}
/*****************************************************************************/
#if defined (FEATURE_FLASH_LESS)
#else
module_init(nandc_mtd_module_init);
#endif
module_exit(nandc_mtd_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<monan@huawei.com>");
MODULE_DESCRIPTION("BalongV3R2 Hisilicon Nand Flash  Controller driver");



