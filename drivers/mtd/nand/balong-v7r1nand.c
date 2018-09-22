/******************************************************************************
*    Copyright (c) 2009-2010 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
/*lint -e713*/
#include <linux/version.h>
#include <asm/io.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#ifdef CONFIG_MTD_PARTITIONS
/*lint -e652 */
#  define mtd_has_partitions()	(1) 
/*lint +e652 */
#else
#  define mtd_has_partitions()	(0)
#endif /* CONFIG_MTD_PARTITIONS */

/* this move to Kconfig file. */
//#define CONFIG_HINFC400_DBG_NAND_PROC_FILE
//#define CONFIG_HINFC400_DBG_NAND_EC_NOTICE
//#define DBG_NAND_PROGRAM

#include "balong-v7r1nand.h"
#include "balong-v7r1nanddbg.c"

/*****************************************************************************/

#define _2K                  (2048)
#define _4K                  (4096)
#define _8K                  (8192)
/*****************************************************************************/

static char *StrOfEccType[] = { "None", "1bit", "4Bytes", "8Bytes", ""};

static char *StrOfPageSize[] = {"512", "2K", "4K", "8K", "" };

static int hinfc400_nand_init(struct hinfc_host *host, struct nand_chip *chip);

extern void (*nand_base_oob_resize)(struct mtd_info *mtd, struct nand_chip *chip);

/*modified for lint e752 
extern void nand_spl_ids_register(void); 
extern int yaffs_dbg; */

void hinfc_dump_reg(void* headstr,struct mtd_info *mtd)
{

	struct nand_chip *chip = mtd->priv;
	struct hinfc_host *host = chip->priv;
	void __iomem * dumpaddr = host->iobase;
	
	char* buf = NULL;
	char* buf2 = kmalloc(64,GFP_KERNEL); 

	
//	return;
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

/*****************************************************************************/

static void hinfc400_ecc_encipher(struct hinfc_host *host)
{
    hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);

    hinfc_write(host, HINFC400_ECC_TEST_ENC_ONLY, HINFC400_ECC_TEST);
	
    while (!(hinfc_read(host, HINFC400_ECC_TEST) & HINFC400_ECC_TEST_ENC_ONLY))
    {
    }

    hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);/*lint !e527*/
}
/*****************************************************************************/

static void hinfc400_ecc_decipher(struct hinfc_host *host)
{
    hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);

    hinfc_write(host, HINFC400_ECC_TEST_DEC_ONLY, HINFC400_ECC_TEST);
    while (!(hinfc_read(host, HINFC400_ECC_TEST) & HINFC400_ECC_TEST_ENC_ONLY))
    {
    }

    hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);/*lint !e527*/
}
/*****************************************************************************/
//qyf add for V3R2
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
static void hinfc400_disable_intReg(struct hinfc_host *host)
{
       //close all interrupt
       hinfc_write(host, HINFC400_INT_MASK_NONE, HINFC400_INT_EN); 
       //clear all interrupt
       hinfc_write(host, HINFC400_INT_MASK_ALL, HINFC400_INT_CTRL); //add by qyf for V3R2
}
#endif
//end by qyf
/*****************************************************************************/
static void hinfc400_read_2k(struct hinfc_host *host, struct mtd_info *mtd)
{
    //hinfc_dump_reg("read2k-in", mtd);
	
    memset(host->chip->IO_ADDR_W, 0xff,HINFC400_BUFFER_SIZE); //qkf48471 add for clear nandc buffer before read operations

    if (host->ecctype != et_ecc_none)
    {
       hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON); 
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
       hinfc_write(host, host->NFC_OP_PARA_ECC_ENABLE, HINFC400_OP_PARA); //add by qyf for V3R2
 #endif 
	}
    hinfc_write(host, HINFC400_BUFFER_SIZE, HINFC400_DATA_NUM);

    hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC400_ADDRL);
    hinfc_write(host, host->addr_value[1], HINFC400_ADDRH);

    hinfc_write(host, NAND_CMD_READSTART << 8 | NAND_CMD_READ0, HINFC400_CMD);

    //hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
    hinfc400_disable_intReg(host);  //add by qyf for V3R2
#endif  
    hinfc_write(host, 
        HINFC400_OP_CMD2_EN
        | HINFC400_OP_ADDR_EN
        | HINFC400_OP_CMD1_EN
        | HINFC400_OP_READ_DATA_EN
        | HINFC400_OP_WAIT_READY_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
        | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
        HINFC400_OP);

    while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
    {;}
    /*
    if (host->ecctype != et_ecc_none)
    {
        hinfc400_ecc_decipher(host);
        dbg_nand_ec_get_status(host, 0);
    }*/
    //hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
	/*lint -e527*/
    dbg_nand_ec_notice(host, _2K);
    dbg_nand_proc_save_logs("R 2K", host->addr_value, host->addr_cycle, 
        host->chip->IO_ADDR_W + (host->addr_value[0] & 0xffff));

    host->addr_cycle = 0x0;
    /*lint +e527*/
    //hinfc_dump_reg("read2k-out", mtd);

}
/*****************************************************************************/

static void hinfc400_program_2k(struct hinfc_host *host, struct mtd_info *mtd)
{    
    if (unlikely(!(host->column)))    /*lint !e730*/
    {
        memset(host->chip->IO_ADDR_W, 0xff, host->column);
    }
    if (host->ecctype != et_ecc_none)
    {
    	 hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);
#ifdef CONFIG_MTD_NAND_BALONG_V3R2		 
	 hinfc_write(host, host->NFC_OP_PARA_ECC_ENABLE, HINFC400_OP_PARA); //add by qyf for V3R2
#endif
        //hinfc400_ecc_encipher(host);
    }

    hinfc_write(host, HINFC400_BUFFER_SIZE, HINFC400_DATA_NUM);

    hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC400_ADDRL);
    hinfc_write(host, host->addr_value[1], HINFC400_ADDRH);
/*
    if (host->ecctype != et_ecc_none)
    {
        hinfc400_ecc_encipher(host);
    }
*/
    hinfc_write(host, NAND_CMD_PAGEPROG << 8  | NAND_CMD_SEQIN |(0x70 << 0x10), HINFC400_CMD);
    //hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
#ifdef CONFIG_MTD_NAND_BALONG_V3R2	
    hinfc400_disable_intReg(host);  //add by qyf for V3R2
#endif  
    hinfc_write(host,  
        HINFC400_OP_READ_STATUS_EN
        | HINFC400_OP_WAIT_READY_EN
        | HINFC400_OP_CMD1_EN
        | HINFC400_OP_CMD2_EN
        | HINFC400_OP_ADDR_EN
        | HINFC400_OP_WRITE_DATA_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
        | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
        HINFC400_OP);
   
    while((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
    { 
    }
    /*lint -e527*/
    dbg_nand_proc_save_logs("W 2K", host->addr_value, host->addr_cycle, 
        host->chip->IO_ADDR_W);

    host->addr_cycle = 0x0;
    /*lint +e527*/
}
/*****************************************************************************/

static void hinfc400_read_4k(struct hinfc_host *host, struct mtd_info *mtd)
{
    hinfc_write(host, HINFC400_BUFFER_SIZE, HINFC400_DATA_NUM);

    hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC400_ADDRL);
    hinfc_write(host, host->addr_value[1], HINFC400_ADDRH);

    hinfc_write(host, NAND_CMD_READSTART << 8 | NAND_CMD_READ0, HINFC400_CMD);

    if (host->ecctype != et_ecc_none)
    {
        hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);
    }
    else
    {
        hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
    }

    hinfc_write(host, 
        HINFC400_OP_CMD2_EN
        | HINFC400_OP_ADDR_EN
        | HINFC400_OP_CMD1_EN
        | HINFC400_OP_READ_DATA_EN
        | HINFC400_OP_WAIT_READY_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
        | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
        HINFC400_OP);
    /*lint -e527*/
    while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
    {
    }

    if (host->ecctype != et_ecc_none)
    {
        memcpy(host->buffer, (unsigned char *)(host->chip->IO_ADDR_R), _2K);
        /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
         memcpy(host->buffer + _4K, 
            (unsigned char *)((unsigned long)host->chip->IO_ADDR_R + _2K), mtd->oobsize/2);
        /*END:y00206456 2012-04-26 Modified for pclint e124*/
        dbg_nand_ec_get_status(host, 0);
    }
    else
    {
        memcpy(host->buffer, 
            (unsigned char *)(host->chip->IO_ADDR_R), HINFC400_BUFFER_SIZE);
    }

    hinfc_write(host, 
        HINFC400_OP_READ_DATA_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT), 
        HINFC400_OP);

    while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
        ;

    if (host->ecctype != et_ecc_none)
    {
        memcpy(host->buffer + _2K,
            (unsigned char *)(host->chip->IO_ADDR_R), _2K);
        memcpy(host->buffer + _4K + mtd->oobsize/2, 
            (unsigned char *)(host->chip->IO_ADDR_R) + _2K, mtd->oobsize/2);
        dbg_nand_ec_get_status(host, 1);
    }
    else
    {
        memcpy((host->buffer + HINFC400_BUFFER_SIZE), 
            (unsigned char *)(host->chip->IO_ADDR_R), HINFC400_BUFFER_SIZE);
    }

    dbg_nand_ec_notice(host, _4K);
    dbg_nand_proc_save_logs("R 4K", host->addr_value, host->addr_cycle, 
        host->buffer + (host->addr_value[0] & 0xffff));

    host->addr_cycle = 0x0;
    /*lint +e527*/
}
/*****************************************************************************/

static void hinfc400_program_4k(struct hinfc_host *host, struct mtd_info *mtd)
{
    if (unlikely(!(host->column)))    /*lint !e730*/
    {
        memset(host->buffer, 0xff, host->column);
    }

    hinfc_write(host, HINFC400_BUFFER_SIZE, HINFC400_DATA_NUM);

    if (host->ecctype != et_ecc_none)
    {
        hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);
        memcpy((unsigned char *)(host->chip->IO_ADDR_W), 
            host->buffer, _2K);
        memcpy((unsigned char *)(host->chip->IO_ADDR_W) + _2K,
            host->buffer + _4K, mtd->oobsize / 2);
    }
    else
    {
        hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
        memcpy((unsigned char *)(host->chip->IO_ADDR_W),
            host->buffer, HINFC400_BUFFER_SIZE);
    }

    hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC400_ADDRL);
    hinfc_write(host, host->addr_value[1], HINFC400_ADDRH);
    hinfc_write(host, NAND_CMD_PAGEPROG << 8 |  NAND_CMD_SEQIN, HINFC400_CMD);

    hinfc_write(host,  
        HINFC400_OP_CMD1_EN
        | HINFC400_OP_ADDR_EN
        | HINFC400_OP_WRITE_DATA_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
        | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
        HINFC400_OP);

    while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
    {}
    /*lint -e527*/
    if (host->ecctype != et_ecc_none)
    {
        memcpy((unsigned char *)(host->chip->IO_ADDR_W),
            host->buffer + _2K, _2K);
        memcpy((unsigned char *)(host->chip->IO_ADDR_W) + _2K,
            host->buffer + _4K + mtd->oobsize/2, mtd->oobsize/2);
    }
    else
    {
        memcpy((unsigned char *)(host->chip->IO_ADDR_W),
            (host->buffer + HINFC400_BUFFER_SIZE), HINFC400_BUFFER_SIZE);
    }

    hinfc_write(host,  
        HINFC400_OP_CMD2_EN
        | HINFC400_OP_WAIT_READY_EN
        | HINFC400_OP_WRITE_DATA_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT),
        HINFC400_OP);

    while ((hinfc_read(host, HINFC400_STATUS) & 0x1) == 0x0)
        ;

    dbg_nand_proc_save_logs("W 4K", host->addr_value, host->addr_cycle, 
        host->buffer);

    host->addr_cycle = 0x0;
    /*lint +e527*/
}
/*****************************************************************************/

static void hinfc400_read_8k(struct hinfc_host *host, struct mtd_info *mtd)
{
    int   ix;
    char *pData = host->buffer;
    char *pOob  = host->buffer + _8K;
    int   NumOfData = _2K;
    int   NumOfOob  = mtd->oobsize / 4;

    hinfc_write(host, NumOfData + NumOfOob, HINFC400_DATA_NUM);

    if (host->ecctype != et_ecc_none)
    {
        hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);
    }
    else
    {
        hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
    }

    hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC400_ADDRL);
    hinfc_write(host, host->addr_value[1], HINFC400_ADDRH);

    hinfc_write(host, NAND_CMD_READSTART << 8 | NAND_CMD_READ0, HINFC400_CMD);

    hinfc_write(host, 
        HINFC400_OP_CMD2_EN
        | HINFC400_OP_ADDR_EN
        | HINFC400_OP_CMD1_EN
        | HINFC400_OP_READ_DATA_EN
        | HINFC400_OP_WAIT_READY_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
        | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
        HINFC400_OP);

    while ((hinfc_read(host, HINFC400_STATUS) & 0x1) == 0x0)
    {
    }
    /*lint -e527*/
    if (host->ecctype != et_ecc_none)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy(pData, (unsigned char *)(host->chip->IO_ADDR_R),
            (size_t)NumOfData);
        /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
        memcpy(pOob,  (unsigned char *)((unsigned long)host->chip->IO_ADDR_R + _2K),
            (size_t)NumOfOob);
        /*END  :y00206456 2012-04-26 Modified for pclint e124*/
		/*end*/
        pData += NumOfData;
        pOob  += NumOfOob;

        dbg_nand_ec_get_status(host, 0);
    }
    else
    {	
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy(pData, (unsigned char *)(host->chip->IO_ADDR_R),
            (size_t)(NumOfData + NumOfOob));
		/*end*/
        pData += (NumOfData + NumOfOob);
    }

    for (ix = 0; ix < 3; ix++)
    {
        hinfc_write(host, 
            HINFC400_OP_READ_DATA_EN
            | (host->chipselect << HINFC400_OP_NF_CS_SHIFT),
            HINFC400_OP);
        while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
        {
        }

        if (host->ecctype != et_ecc_none)
        {
        	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
            memcpy(pData, (unsigned char *)(host->chip->IO_ADDR_R),
                (size_t)NumOfData);
            /*BEGIN:y00206456 2012-04-26 Modified for pclint e124*/
            memcpy(pOob,  (unsigned char *)((unsigned long)host->chip->IO_ADDR_R + _2K), 
                (size_t)NumOfOob);
            /*END  :y00206456 2012-04-26 Modified for pclint e124*/
			/*end*/

            pData += NumOfData;
            pOob  += NumOfOob;

            dbg_nand_ec_get_status(host, (ix + 1));
        }
        else
        {
        	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
            memcpy(pData, (unsigned char *)(host->chip->IO_ADDR_R), 
                (size_t)(NumOfData + NumOfOob));
			/*end*/
            pData += (NumOfData + NumOfOob);
        }
    }

    dbg_nand_ec_notice(host, _8K);
    dbg_nand_proc_save_logs("R 8K", host->addr_value, host->addr_cycle, 
        host->buffer + (host->addr_value[0] & 0xffff));

    host->addr_cycle = 0x0;
    /*lint +e527*/
}
/*****************************************************************************/

static void hinfc400_program_8k(struct hinfc_host *host, struct mtd_info *mtd)
{
    int   ix;
    char *pData = host->buffer;
    char *pOob  = host->buffer + _8K;
    int   NumOfData = _2K;
    int   NumOfOob  = mtd->oobsize / 4;

    if (unlikely(!(host->column)))     /*lint !e730*/
    {
       memset(host->buffer, 0xff, host->column);
    }

    hinfc_write(host, NumOfData + NumOfOob, HINFC400_DATA_NUM);

   if (host->ecctype != et_ecc_none)
    {
        hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy((unsigned char *)(host->chip->IO_ADDR_W), pData, (size_t)NumOfData);
        memcpy((unsigned char *)(host->chip->IO_ADDR_W) + _2K, pOob, (size_t)NumOfOob);
		/*end*/
        pData += NumOfData;
        pOob  += NumOfOob;
    }
    else
    {
        hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON);
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy((unsigned char *)(host->chip->IO_ADDR_W), pData,
            (size_t)(NumOfData + NumOfOob));
		/*end*/
        pData += NumOfData + NumOfOob;
    }

    hinfc_write(host, host->addr_value[0] & 0xffff0000, HINFC400_ADDRL);
    hinfc_write(host, host->addr_value[1], HINFC400_ADDRH);

    hinfc_write(host, NAND_CMD_PAGEPROG << 8 |  NAND_CMD_SEQIN, HINFC400_CMD);

    hinfc_write(host,  
        HINFC400_OP_CMD1_EN
        | HINFC400_OP_ADDR_EN
        | HINFC400_OP_WRITE_DATA_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
        | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
        HINFC400_OP);

    while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
        ;

    for (ix = 0; ix < 2; ix++)
    {
        if (host->ecctype != et_ecc_none)
        {
        	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
            memcpy((unsigned char *)(host->chip->IO_ADDR_W), pData, (size_t)NumOfData);
            memcpy((unsigned char *)(host->chip->IO_ADDR_W) + _2K, pOob, (size_t)NumOfOob);
			/*end*/
            pData += NumOfData;
            pOob  += NumOfOob;
        }
        else
        {
        	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
            memcpy((unsigned char *)(host->chip->IO_ADDR_W), pData,
                (size_t)(NumOfData + NumOfOob));
			/*end*/
            pData += NumOfData + NumOfOob;
       }

        hinfc_write(host,  
            HINFC400_OP_WRITE_DATA_EN
            | (host->chipselect << HINFC400_OP_NF_CS_SHIFT),
            HINFC400_OP);

        while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
            ;
    }

    if (host->ecctype != et_ecc_none)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy((unsigned char *)(host->chip->IO_ADDR_W), pData, (size_t)NumOfData);
        memcpy((unsigned char *)(host->chip->IO_ADDR_W) + _2K, pOob, (size_t)NumOfOob);
		/*end*/
        pData += NumOfData;
        pOob  += NumOfOob;
    }
    else
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy((unsigned char *)(host->chip->IO_ADDR_W), pData,
            (size_t)(NumOfData + NumOfOob));
		/*end*/
        pData += NumOfData + NumOfOob;
    }

    hinfc_write(host,  
        HINFC400_OP_CMD2_EN
        | HINFC400_OP_WAIT_READY_EN
        | HINFC400_OP_WRITE_DATA_EN
        | (host->chipselect << HINFC400_OP_NF_CS_SHIFT),
        HINFC400_OP);

    while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
    {
	}
    /*lint -e527*/
    dbg_nand_proc_save_logs("W 8K", host->addr_value, host->addr_cycle, 
        host->buffer);

    host->addr_cycle = 0x0;
    /*lint +e527*/
}
/*****************************************************************************/

static void hinfc400_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
    struct nand_chip *chip = mtd->priv;
    struct hinfc_host *host = chip->priv;
	
    if (ctrl & NAND_ALE)
    {
        unsigned int addr_value = 0;
        unsigned int addr_offset = 0;

        if (ctrl & NAND_CTRL_CHANGE)
        {
            host->addr_cycle = 0x0;
            host->addr_value[0] = 0x0;
            host->addr_value[1] = 0x0;
        }
        addr_offset =  host->addr_cycle << 3;

        if (host->addr_cycle >= HINFC400_ADDR_CYCLE_MASK)
        {
            addr_offset = (host->addr_cycle - HINFC400_ADDR_CYCLE_MASK) << 3;
            addr_value = 1;
        }

        host->addr_value[addr_value] |= ((dat & 0xff) << addr_offset);

        host->addr_cycle ++;
    }

    if ((ctrl & NAND_CLE) && (ctrl & NAND_CTRL_CHANGE))
    {
        host->command = dat & 0xff;
        switch (host->command)
        {
        case NAND_CMD_PAGEPROG:
            host->program(host, mtd);
            dbg_nand_program(GET_PAGE_INDEX(host));
            break;

        case NAND_CMD_READSTART: 
            host->read(host, mtd);
            break;

        case NAND_CMD_ERASE2:
            hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON); 
#ifdef CONFIG_MTD_NAND_BALONG_V3R2			
            hinfc_write(host, host->NFC_OP_PARA_ECC_DISABLE, HINFC400_OP_PARA); //add by qyf for V3R2
#endif
	    hinfc_write(host, host->addr_value[0], HINFC400_ADDRL);

            hinfc_write(host, (host->command << 8) | NAND_CMD_ERASE1, HINFC400_CMD);

            hinfc_write(host,
                HINFC400_OP_WAIT_READY_EN
                | HINFC400_OP_CMD2_EN
                | HINFC400_OP_CMD1_EN
                | HINFC400_OP_ADDR_EN
                | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
                | (host->addr_cycle << HINFC400_OP_ADDR_CYCLE_SHIFT),
                HINFC400_OP);

            while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
            {
			}
            /*lint -e527*/
           dbg_nand_erase(host->addr_value[0]);
            dbg_nand_proc_save_logs("E   ", host->addr_value, host->addr_cycle, NULL);

            break;
            /*lint +e527*/
        case NAND_CMD_READID:

            memset((unsigned char *)(chip->IO_ADDR_R), 0, 0x10);

            hinfc_write(host, host->command, HINFC400_CMD);

            hinfc_write(host, host->NFC_CON_ECC_ENABLE, HINFC400_CON);
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
            hinfc_write(host, host->NFC_OP_PARA_ECC_ENABLE, HINFC400_OP_PARA); //add by qyf for V3R2
#endif
            hinfc_write(host,  
                HINFC400_OP_CMD1_EN
                | HINFC400_OP_ADDR_EN
                | HINFC400_OP_READ_DATA_EN
                | HINFC400_OP_WAIT_READY_EN
                | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
                | (1 << HINFC400_OP_ADDR_CYCLE_SHIFT),
                HINFC400_OP);

            host->addr_cycle = 0x0;
            while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
            {
            }
			/*lint -e527*/
            if (host->IsUseBuffer)
            {
                memcpy(host->buffer, (unsigned char *)(chip->IO_ADDR_R), 0x10);
            }

            break;
            /*lint +e527*/

        case NAND_CMD_STATUS:

            hinfc_write(host, NAND_CMD_STATUS, HINFC400_CMD);

            hinfc_write(host, 4, HINFC400_DATA_NUM);

            hinfc_write(host, host->NFC_CON_ECC_DISABLE, HINFC400_CON); 
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
            hinfc_write(host, host->NFC_OP_PARA_ECC_DISABLE, HINFC400_OP_PARA); //add by qyf for V3R2
#endif
            hinfc_write(host,
                HINFC400_OP_CMD1_EN
                | HINFC400_OP_READ_DATA_EN
                | HINFC400_OP_WAIT_READY_EN
                | (host->chipselect << HINFC400_OP_NF_CS_SHIFT),
                HINFC400_OP);

            while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
            {
			}

            break;/*lint !e527*/

        case NAND_CMD_SEQIN:
        case NAND_CMD_ERASE1:
        case NAND_CMD_READ0:
            break;
        case NAND_CMD_RESET:

            hinfc_write(host, NAND_CMD_RESET, HINFC400_CMD);

            hinfc_write(host,
                HINFC400_OP_CMD1_EN
                | (host->chipselect << HINFC400_OP_NF_CS_SHIFT)
                | HINFC400_OP_WAIT_READY_EN ,
               HINFC400_OP);

            while ((hinfc_read( host, HINFC400_STATUS ) & 0x1) == 0x0)
            {
			}
            break;/*lint !e527*/

        default :
            break;
        }
    }

    if ((dat == NAND_CMD_NONE) && host->addr_cycle)
    {
        if (host->command == NAND_CMD_SEQIN 
            || host->command == NAND_CMD_READ0 
            || host->command == NAND_CMD_READID)
        {
            host->offset = 0x0;
            host->column = host->addr_value[0] & 0xffff;
        }
    }
}
/*****************************************************************************/

static int hinfc400_dev_ready(struct mtd_info *mtd)
{
    return 0x1;
}
/*****************************************************************************/

static void hinfc400_select_chip(struct mtd_info *mtd, int chipselect)
{
    struct nand_chip *chip = mtd->priv;
    struct hinfc_host *host = chip->priv;

    if (chipselect < 0)
        return;

    if (chipselect > 2)
    {
        DBG_BUG("invalid chipselect: %d\n", chipselect);
    }

    host->chipselect = chipselect;
}
/*****************************************************************************/

static uint8_t hinfc400_read_byte(struct mtd_info *mtd)
{
    /* modified for lint e578 
    unsigned uint8_t value = 0;*/ //pclint 734
    struct nand_chip *chip = mtd->priv;
    struct hinfc_host *host = chip->priv;

    if (host->command == NAND_CMD_STATUS)
    {
        return readb(chip->IO_ADDR_R);
    }

    if (host->IsUseBuffer)
    {
        value = readb(host->buffer + host->column + host->offset);  /*lint !e63 */
    }
    else
    {
        value = readb(chip->IO_ADDR_R + host->column + host->offset); /*lint !e63 */
    }

    host->offset++;
    return value;
}
/*****************************************************************************/

static u16 hinfc400_read_word(struct mtd_info *mtd)
{
    u16 value = 0;
    struct nand_chip  *chip = mtd->priv;
    struct hinfc_host *host = chip->priv;

    if (host->IsUseBuffer)
    {
        value = readw(host->buffer + host->column + host->offset);
    }
    else
    {
        value = readw(chip->IO_ADDR_R + host->column + host->offset);
    }

    host->offset += 2;
    return value;
}
/*****************************************************************************/

static void hinfc400_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
    struct nand_chip *chip = mtd->priv;
    struct hinfc_host *host = chip->priv;

    if (host->IsUseBuffer)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy(buf, host->buffer + host->column + host->offset, (size_t)len);
		/*end*/
    }
    else
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy(buf, (unsigned char *)(chip->IO_ADDR_R)  
            + host->column + host->offset, (size_t)len);
		/*end*/
    }
    host->offset += (unsigned int)len;
}
/*****************************************************************************/

static void hinfc400_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
    struct nand_chip  *chip = mtd->priv;
    struct hinfc_host *host = chip->priv;

    if (host->IsUseBuffer)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy(host->buffer + host->column + host->offset, buf, (size_t)len);
		/*end*/
    }
    else
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy((unsigned char *)(chip->IO_ADDR_W) 
            + host->column + host->offset, buf, (size_t)len);
		/*end*/
    }

    host->offset += (unsigned int)len;
}
/*****************************************************************************/

/* ecc_none */
static struct nand_ecclayout nand_oob_none = 
{
    .eccbytes    = 0,
    .oobfree     = {
        {2, 30},
    }
};

/* 2K 1bit */
#ifndef CONFIG_MTD_NAND_BALONG_V3R2
static struct nand_ecclayout nand_oob_64 = 
{
    .eccbytes = 20,
    .eccpos   = {
        11, 12, 13, 14, 15,
        27, 28, 29, 30, 31,
        43, 44, 45, 46, 47,
        59, 60, 61, 62, 63
    },
    .oobfree  = {
        //{2, 9}, {16, 11}, {32, 11}, {48, 11}
        {2, 4}, {16, 6}, {32, 6}, {48, 3}
    }
};
#else
static struct nand_ecclayout nand_oob_64 = 
{
    .eccbytes = 40,
    .eccpos   = {
        24,25,26,27,28,29,30,31,32,33,
	34,35,36,37,38,39,40,41,42,43,
	44,45,46,47,48,49,50,51,52,53,
	54,55,56,57,58,59,60,61,62,63

	},
    .oobfree  = {
        //{2, 9}, {16, 11}, {32, 11}, {48, 11}
        {2,16}
    }
};
#endif

/* 4K 4Bytes */
static struct nand_ecclayout nand_oob_128 =
{
    .eccbytes = 20,
    .eccpos   = {
        6,  7,  8,   9, 10, 11, 12, 13, 14, 15,
        22, 23, 24, 25, 26, 27, 28, 29, 30, 31
    },
    .oobfree  = {
        {2, 4}, {16, 6}, {32, 6}, {48, 6}, {64, 6}, {80, 6}
    }
};

/* 4K 8Bytes */
static struct nand_ecclayout nand_oob_208 =
{
    .eccbytes = 20,
    .eccpos   = {
        6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
    },
    .oobfree  = {
        {2, 4}, {26, 6}, {52, 6}, {78, 6}, {104, 6},{130, 6}
    }
};

/*****************************************************************************/

struct page_ecc_info
{
    unsigned int pagesize;
    enum page_type pagetype;

    unsigned int oobsize;
    enum ecc_type  ecctype;

    struct nand_ecclayout *layout;
    void (*program)(struct hinfc_host *host, struct mtd_info *mtd);
    void (*read)(struct hinfc_host *host, struct mtd_info *mtd);
    int IsUseBuffer;
};
/*****************************************************************************/

static struct page_ecc_info page_ecc_info[] =
{
    {_8K, pt_pagesize_8K, 416, et_ecc_8bytes, &nand_oob_208,  hinfc400_program_8k, hinfc400_read_8k, 1},
    {_8K, pt_pagesize_8K, 256, et_ecc_4bytes, &nand_oob_128,  hinfc400_program_8k, hinfc400_read_8k, 1},
#ifdef CONFIG_HINFC400_SUPPROT_8K_ECC_NONE    
    {_8K, pt_pagesize_8K, 32,  et_ecc_none,   &nand_oob_none, hinfc400_program_8k, hinfc400_read_8k, 1},
#endif /* CONFIG_HINFC400_SUPPROT_8K_ECC_NONE */

    {_4K, pt_pagesize_4K, 208, et_ecc_8bytes, &nand_oob_208,  hinfc400_program_4k, hinfc400_read_4k, 1},
    {_4K, pt_pagesize_4K, 128, et_ecc_4bytes, &nand_oob_128,  hinfc400_program_4k, hinfc400_read_4k, 1},
#ifdef CONFIG_HINFC400_SUPPROT_4K_ECC_NONE  
    {_4K, pt_pagesize_4K, 32,  et_ecc_none,   &nand_oob_none, hinfc400_program_4k, hinfc400_read_4k, 1},
#endif /* CONFIG_HINFC400_SUPPROT_4K_ECC_NONE */

    //{_2K, pt_pagesize_2K, 64,  et_ecc_1bit,   &nand_oob_64,   hinfc400_program_2k, hinfc400_read_2k, 0},
    {_2K, pt_pagesize_2K, 64,  et_ecc_4bytes,   &nand_oob_64,   hinfc400_program_2k, hinfc400_read_2k, 0},
#ifdef CONFIG_HINFC400_SUPPROT_2K_ECC_NONE    
    {_2K, pt_pagesize_2K, 32,  et_ecc_none,   &nand_oob_none, hinfc400_program_2k, hinfc400_read_2k, 0},
#endif /* CONFIG_HINFC400_SUPPROT_2K_ECC_NONE */

    {0},
};
/*****************************************************************************/
/* 
 * used the best correct arithmetic.
 */
struct page_ecc_info *hinfc400_get_best_ecc(struct mtd_info *mtd)
{
    struct page_ecc_info *best = NULL;
    struct page_ecc_info *info = page_ecc_info;

    for (; info->pagesize; info++)
    {
        if (info->pagesize != mtd->writesize)
            continue;

        if (mtd->oobsize < info->oobsize)
            continue;

        if (!best || (best->oobsize < info->oobsize))
        {
            best = info;
        }
    }

    if (!best)
    {
        DBG_BUG("not support this pagesize(%d) and oobsize(%d).\n",
            mtd->writesize, mtd->oobsize);
    }

    return best;
}
/*****************************************************************************/
/* 
 *  select ecc type as hardware set
 */
struct page_ecc_info *hinfc400_get_hardware_ecc(struct hinfc_host *host,
    struct mtd_info *mtd)
{
    int shift;
    int hw_pagesize;
    int hw_oobsize;
    unsigned int regval;
    unsigned int hw_ecctype;
    unsigned int hw_pagetype;
    struct page_ecc_info *fit  = NULL;
    struct page_ecc_info *info = page_ecc_info;

    regval = hinfc_read(host, HINFC400_CON);
    hw_ecctype  = ((regval >> 10) & 0x03);
    hw_pagetype = ((regval >> 1) & 0x03);

    for (; info->pagesize; info++)
    {
        if (info->pagetype == hw_pagetype
            && info->ecctype == hw_ecctype)
        {
            fit = info;
            break;
        }
    }

    if (!fit)
    {
        DBG_BUG("hardware config error, pagesize:%s, ecctype:%s\n",
            StrOfPageSize[hw_pagetype], StrOfEccType[hw_ecctype]);
        return NULL;
    }

    shift = (int)("\x00\x02\x03\x04"[hw_pagetype]);
    hw_pagesize = (512 << shift);

    if ((unsigned int)hw_pagesize != mtd->writesize)
    {
        DBG_BUG("hardware is inconsistent, require pagesize(%d), "
            "but in fact, the nand pageszie is %d\n",
            hw_pagesize, mtd->writesize);
        return NULL;
    }
    hw_oobsize = ((unsigned int)("\x00\x10\x10\x1A"[hw_ecctype]) << shift); /*lint !e571 */

	if ((unsigned int)hw_oobsize > mtd->oobsize)
    {
        DBG_BUG("hardware is inconsistent, require oobsize(%d), "
            "but in fact, the nand oobsize is %d, littler than require.\n",
            hw_oobsize, mtd->oobsize);
        return NULL;
    }

    return fit;
}
/*****************************************************************************/
/*
 *  force the pagesize and ecctype, some test maybe using this.
 */
struct page_ecc_info *hinfc400_force_ecc(struct mtd_info *mtd, 
    enum page_type pagetype, enum ecc_type ecctype)
{
    struct page_ecc_info *best = NULL;
    struct page_ecc_info *info = page_ecc_info;

    for (; info->pagesize; info++)
    {
        if (info->pagetype != pagetype
            || info->ecctype != ecctype)
            continue;

        if (info->pagesize > mtd->writesize)
            break;

        if (mtd->writesize == info->pagesize
            && mtd->oobsize < info->oobsize)
            break;

        best = info;
        mtd->writesize = best->pagesize;
        break;
    }

    if (!best)
    {
        DBG_BUG("nand not support current configure ecctype or pagetype.\n"
            "nand pageszie:%d, oobsize:%d; configure ecctype:%s, pagetype:%s\n",
            mtd->writesize, mtd->oobsize, 
            StrOfEccType[ecctype], StrOfPageSize[pagetype]);
    }

    return best;
}

/*****************************************************************************/

static char *ToSizeString(unsigned long long size)
{
    int ix;
    static char buffer[20]; 
    char *fmt[] = {"%uByte", "%uK", "%uM", "%uG", "%uT", "%uT"};

    for (ix = 0; (ix < 5) && (size >= 1024); ix++)
    {
        size = (size >> 10);
    }
    sprintf(buffer, fmt[ix], size);
    return buffer;
}
/*****************************************************************************/

static void hinfc400_ecc_probe(struct mtd_info *mtd, struct nand_chip *chip)
{
    struct page_ecc_info *best;
    unsigned int writesize = mtd->writesize;
    struct hinfc_host *host = chip->priv;

    best = hinfc400_get_best_ecc(mtd);

//     best = hinfc400_get_hardware_ecc(host, mtd);

//     best = hinfc400_force_ecc(mtd, pt_pagesize_4K, et_ecc_4bytes);

    if (best->ecctype != et_ecc_none)
        mtd->oobsize = best->oobsize;
    chip->ecc.layout = best->layout;

    host->ecctype = best->ecctype;
    host->program = best->program;
    host->read    = best->read;
    host->IsUseBuffer = best->IsUseBuffer;

#ifdef CONFIG_MTD_NAND_BALONG_V3R2
    host->NFC_CON_ECC_ENABLE = (0x700 & (best->ecctype << 8))  //(0xC00 & (best->ecctype << 10))  //modify by qyf for V3R2
        /*| HINFC400_CON_EXT_DATA_ECC_EN*/ //modify by qyf for V3R2
       /* | HINFC400_CON_READY_BUSY_SEL*//*m00176101 SYNC WITH Mcore-vxworks*/
        /*| HINFC400_CON_ECC_EN*/ //modify by qyf for V3R2
        | HINFC400_CON_OP_MODE
        | HINFC400_CON_BUS_WIDTH_16    //add by qyf for V3R2
        | (0x6 & (best->pagetype << 1));

    if (best->pagetype == pt_pagesize_2K)
    {
	#if 0 //m00176101 sync with vxworks
        host->NFC_CON_ECC_ENABLE &= ~HINFC400_CON_EXT_DATA_ECC_EN;
	#else
	host->NFC_CON_ECC_ENABLE |= HINFC400_CON_EXT_DATA_ECC_EN;
	#endif

       host->NFC_OP_PARA_ECC_ENABLE = NFC4_ECC_PARA_EN_ALL;  // add by qyf for V3R2
     } 
    host->NFC_CON_ECC_DISABLE = HINFC400_CON_READY_BUSY_SEL
        | HINFC400_CON_OP_MODE
        | (0x6 & (best->pagetype << 1));

     host->NFC_OP_PARA_ECC_DISABLE =  NFC4_ECC_PARA_DISABLE;  // add by qyf for V3R2
#else
    host->NFC_CON_ECC_ENABLE = (0xC00 & (best->ecctype << 10)) 
        | HINFC400_CON_EXT_DATA_ECC_EN
       /* | HINFC400_CON_READY_BUSY_SEL*//*m00176101 SYNC WITH Mcore-vxworks*/
        | HINFC400_CON_ECC_EN
        | HINFC400_CON_OP_MODE
        | (0x6 & (best->pagetype << 1));

    if (best->pagetype == pt_pagesize_2K)
	#if 0 //m00176101 sync with vxworks
        host->NFC_CON_ECC_ENABLE &= ~HINFC400_CON_EXT_DATA_ECC_EN;
	#else
	host->NFC_CON_ECC_ENABLE |= HINFC400_CON_EXT_DATA_ECC_EN;
	#endif
	 
    host->NFC_CON_ECC_DISABLE = HINFC400_CON_READY_BUSY_SEL
        | HINFC400_CON_OP_MODE
        | (0x6 & (best->pagetype << 1));
#endif  
	printk("nandc reg ECC enable:0x%x \n", host->NFC_CON_ECC_ENABLE);
	printk("nandc reg ECC disable:0x%x \n", host->NFC_CON_ECC_DISABLE);

	
    if (mtd->writesize > NAND_MAX_PAGESIZE 
        || mtd->oobsize > NAND_MAX_OOBSIZE)
    {
        DBG_BUG("kernel not support this NAND. "
            "please increase NAND_MAX_PAGESIZE and NAND_MAX_OOBSIZE.\n");
    }

    if (mtd->writesize != writesize)
    {
        unsigned int shift = 0;
        while (writesize > mtd->writesize)
        {
            writesize >>= 1;
            shift++;
        }
        chip->chipsize = chip->chipsize >> shift;
        mtd->erasesize = mtd->erasesize >> shift;
        printk("Nand divide into 1/%u\n", (1 << shift));
    }

    dbg_nand_program_init(mtd->erasesize / mtd->writesize);
    dbg_nand_ec_init();

    printk("Nand: ");
    printk("Block:%s ", ToSizeString((unsigned long long)mtd->erasesize));
    printk("Page:%s ",  ToSizeString((unsigned long long)mtd->writesize));
    printk("Ecc:%s ",   StrOfEccType[best->ecctype]);
    printk("Chip:%s ",  ToSizeString(chip->chipsize));
    printk("OOB:%s",    ToSizeString((unsigned long long)mtd->oobsize));
    printk("\n");
}
/*****************************************************************************/

static int hinfc400_nand_init(struct hinfc_host *host, struct nand_chip *chip)
{
    chip->priv        = host;
    chip->cmd_ctrl    = hinfc400_cmd_ctrl;
    chip->dev_ready   = hinfc400_dev_ready;
    chip->select_chip = hinfc400_select_chip;
    chip->read_byte   = hinfc400_read_byte;
    chip->read_word   = hinfc400_read_word;
    chip->write_buf   = hinfc400_write_buf;
    chip->read_buf    = hinfc400_read_buf;
    
    chip->chip_delay = HINFC400_CHIP_DELAY;
#ifndef CONFIG_MTD_NAND_BALONG_V3R2
    chip->options    = NAND_NO_AUTOINCR |  NAND_SKIP_BBTSCAN;
#else    
	chip->options    = NAND_NO_AUTOINCR|NAND_SKIP_BBTSCAN|NAND_BUSWIDTH_16; //modify by qyf for V3R2
#endif
    chip->ecc.layout = NULL;
    chip->ecc.mode   = NAND_ECC_NONE;

    host->chip          = chip;
    host->addr_cycle    = 0;
    host->addr_value[0] = 0;
    host->addr_value[1] = 0;
    host->program       = hinfc400_program_2k;
    host->read          = hinfc400_read_2k;
    host->chipselect    = 0;

    host->buffer = kmalloc((NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE), GFP_KERNEL);
    if (!host->buffer)
        return 1;

    memset((char *)chip->IO_ADDR_R, 0xff, HINFC400_BUFFER_SIZE);
    memset(host->buffer, 0xff, (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE));

    nand_base_oob_resize = hinfc400_ecc_probe;

    return 0;
}
/*****************************************************************************/
static struct mtd_partition balong_default_nand_part[] = {
	[0] = {
		.name	= "bootsec",
		.size	= SZ_2K,
		.offset	= 0,
	},
	[1] = {
		.name	= "uboot",
		.size	= SZ_4M,
		.offset	= SZ_2K,
	},
	[2] = {
		.name	= "kernel",
		.size	= SZ_2M,
		.offset	= SZ_2K + SZ_4M,
	},
	[3] = {
		.name	= "rootfs",
		.size	= MTDPART_SIZ_FULL,
		.offset	= SZ_2K + SZ_4M + SZ_2M,
	},
};

static int hinfc400_probe(struct device* dev)
{
    int result = 0;

	struct hinfc_host *host;
    struct nand_chip  *chip;
	struct mtd_info   *mtd;
   	struct platform_device *pltdev = to_platform_device(dev);

    if (NULL == pltdev)/*e413*/
    {
        return -ENOMEM;
    }

    int size = sizeof(struct hinfc_host) + sizeof(struct nand_chip)
        + sizeof(struct mtd_info);

	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	host = kmalloc((size_t)size, GFP_KERNEL);
	/*end*/
	if (!host)
    {
		dev_err(&pltdev->dev, "failed to allocate device structure.\n");
		return -ENOMEM;
	}
    memset((char *)host, 0, size);

	dev->platform_data = host;

    host->chip = chip = (struct nand_chip *)&host[1];
    host->mtd  = mtd  = (struct mtd_info *)&chip[1];
	host->iobase = ioremap(pltdev->resource[0].start,
        pltdev->resource[0].end - pltdev->resource[0].start + 1);
	if (!host->iobase)
	{
		dev_err(&pltdev->dev, "ioremap failed\n");
		kfree(host);
	return -EIO;
	}

    mtd->priv  = chip;
    mtd->owner = THIS_MODULE;
    mtd->name  = (char*)(pltdev->name);

	chip->IO_ADDR_R = chip->IO_ADDR_W = ioremap_nocache(
        pltdev->resource[1].start,
        pltdev->resource[1].end - pltdev->resource[1].start + 1);
	if (!chip->IO_ADDR_R)
    {
		dev_err(&pltdev->dev, "ioremap failed\n");
		iounmap(host->iobase);
		kfree(host);
		return -EIO;
	}

    if (hinfc400_nand_init(host, chip))
    {
		dev_err(&pltdev->dev, "failed to allocate device buffer.\n");
        result = -ENOMEM;
        goto err;
    }

   /*
    *  this nand controller only support 1 or 2 nand flash chip.
    */
#if (CONFIG_HINFC400_MAX_CHIP != 1) && (CONFIG_HINFC400_MAX_CHIP != 2)
#  error please run make menuconfig reconfig CONFIG_HINFC400_MAX_CHIP. this nand controller only \
    support 1 or 2 nand flash chip, but your config other value
#endif /* (CONFIG_HINFC400_MAX_CHIP != 1) && (CONFIG_HINFC400_MAX_CHIP != 2) */

	if (nand_scan(mtd, CONFIG_HINFC400_MAX_CHIP)) 
    {
		result = -ENXIO;
		goto err;
	}
	
	if (mtd_has_partitions()) 
    {
        int i;
    	int nr_parts = 0;
    	struct mtd_partition *parts = NULL;

#ifdef CONFIG_MTD_CMDLINE_PARTS
        parts = balong_default_nand_part;
    	//static const char *part_probes[] = { "bootsec", "uboot", "kernel", "rootfs", NULL, };
    	static const char *part_probes[] = { "cmdlinepart", NULL, };
    	nr_parts = parse_mtd_partitions(mtd, part_probes, &parts, 0);
#endif

    	if (nr_parts > 0) 
        {
		for (i = 0; i < nr_parts; i++) 
		{
    			//DEBUG(MTD_DEBUG_LEVEL2, "partitions[%d] = "
    			printk("partitions[%d] = "
    				"{.name = %s, .offset = 0x%.8llx, "
    					".size = 0x%.8llx (%lluKiB) }\n",
    				i, parts[i].name,
    				parts[i].offset,
    				parts[i].size,
    				parts[i].size / 1024);
    		}
    		return add_mtd_partitions(mtd, parts, nr_parts);
    	}
        else
        {
		printk("No Partition !!!!\n");
		printk("Create new partitions !!!!\n");
        	parts = balong_default_nand_part;
        /*解决typeof参数类型不对的问题-- linux内核*/
        /*lint -e516 -e84*/	
		nr_parts = ARRAY_SIZE(balong_default_nand_part);/*lint !e30 !e806*/
		/*lint +e516 +e84*/
		return add_mtd_partitions(mtd, parts, nr_parts);
        }
    }

    if (!add_mtd_device(mtd))
        return 0;

    result = -ENODEV;
	nand_release(mtd);
    
err:
    if (host->buffer)
        kfree(host->buffer);
	iounmap(chip->IO_ADDR_W);
	iounmap(host->iobase);
	kfree(host);
	dev->platform_data = NULL;

	return result;
}
/*****************************************************************************/

int hinfc400_remove(struct device *dev)
{
	struct hinfc_host *host = dev->platform_data;

	nand_release(host->mtd);
    kfree(host->buffer);
    iounmap(host->chip->IO_ADDR_W);
	iounmap(host->iobase);
	kfree(host);
	dev->platform_data = NULL;

    return 0;
}
/*****************************************************************************/

static void hinfc400_pltdev_release (struct device *dev)
{
}
/*****************************************************************************/

static struct device_driver hinfc400_driver =
{
	.name   = "hinand",
	.bus    = &platform_bus_type,
	.probe  = hinfc400_probe,
	.remove = hinfc400_remove,
};
/*****************************************************************************/

/*Error 34: (Error -- Non-constant Initializer)*/
/*Error 40: (Error -- Undeclared identifier 'CONFIG_HINFC400_REG_BASE_ADDRESS')*/
/*在Kconfig 中有定义*/
/*lint -e34*/
/*lint -e40*/
static struct resource hinfc400_resources[] = 
{
	[0] = {
		.start  = CONFIG_HINFC400_REG_BASE_ADDRESS,
		.end    = CONFIG_HINFC400_REG_BASE_ADDRESS + HINFC400_REG_BASE_ADDRESS_LEN - 1,
		.flags  = IORESOURCE_MEM,
	},

	[1] = {
		.start  = CONFIG_HINFC400_BUFFER_BASE_ADDRESS,
		.end    = CONFIG_HINFC400_BUFFER_BASE_ADDRESS + HINFC400_BUFFER_BASE_ADDRESS_LEN - 1,// add by kqf48471 for V3R2
		.flags  = IORESOURCE_MEM,
	},
};
/*lint +e40*/
/*lint +e34*/

/*****************************************************************************/

static struct platform_device hinfc400_pltdev =
{
	.name           = "hinand",
	.id             = -1,

	.dev.platform_data     = NULL,
    .dev.dma_mask          = (u64*)~0,
    .dev.coherent_dma_mask = (u64) ~0,
    .dev.release           = hinfc400_pltdev_release,
    /*解决typeof参数类型不对的问题-- linux内核*/
    /*lint -e516 -e84*/
	.num_resources  = ARRAY_SIZE(hinfc400_resources),/*lint !e30 !e806*/
	/*lint +e516 +e84*/
	.resource       = hinfc400_resources,
};
/*****************************************************************************/

static int __init hinfc400_module_init(void)
{
	int result;

    printk("Hisilicon Nand Flash Controller V400 Driver, Version 1.00\n");

    dbg_nand_proc_init();

    result = driver_register(&hinfc400_driver);
	if (result < 0)
	{
        return result;
	}

	result = platform_device_register(&hinfc400_pltdev);
	if (result < 0)
	{
        driver_unregister(&hinfc400_driver);
		return result;
	}

	return result;
}
/*****************************************************************************/

static void __exit hinfc400_module_exit (void)
{
	driver_unregister(&hinfc400_driver);
	platform_device_unregister(&hinfc400_pltdev);
}
/*****************************************************************************/

module_init(hinfc400_module_init);
module_exit(hinfc400_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hisilicon Tech.Co.,Ltd.<czluo@huawei.com>");
MODULE_DESCRIPTION("BalongV7R1 Nand Flash driver");


