/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_v200.c
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
1.date:2011-07-27
 question number:
 modify reasion:        create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/


#include "nandc_inc.h"
#include "nandc_v200.h"

/*****************************************************************************/
/* ecc_none */

struct nand_ecclayout nandc2_oob64_ecc4 = 
{
    STU_SET(.eccbytes)  0,
    STU_SET(.eccpos)    {0},
    STU_SET(.oobavail)  64,
    STU_SET(.oobfree)     
    { 
        {4, 2},{16, 6},{32, 6},{48, 6},
    }
}; 

/*NANDC2_REG_CONF_OFFSET                (0x00)	*/
/*-------------------------------------------------------------------------
*  bit 11    RW    edo_en           ->  0: normal mode
                                        1: EDO mode
--------------------------------------------------------------------------                                               
*  bit10:8  RW    ecc_type          ->  000: no ecc
                                        001: 1bit ecc
                                        010: 4bit ecc
                                        011: reserved
                                        100: 24bit for 1KB
---------------------------------------------------------------------------                                               
*  bit 7      RW   rb_sel           ->  0: use share ready/busy signal
                                        1: use independent ready/busy signal
---------------------------------------------------------------------------
*  bit 6      RW   cs_ctrl          ->  0:  busy is 0
                                        1:  busy is 1
---------------------------------------------------------------------------
*  bit 5:4   --    reserved             
---------------------------------------------------------------------------
*  bit 3     RW    bus_width        ->  0:8bit
                                        1:16bit
 --------------------------------------------------------------------------                                              
*  bit 2:1   RW   pagesize          ->  00: 512Byte 
                                        01: 2KByte 
                                        10: 4KByte 
                                        11: 8KByte    
--------------------------------------------------------------------------                                                
*  bit 0     RW    opmode         ->    0:boot mode
                                        1: normal mode   
**************************************************************************/
static struct nandc_reg_cont nandc2_cont_conf[] =  
{
    { 
        nandc_bits_op_mode,   
        {
            0, 1
        }, 
        {
            nandc_op_boot, nandc_op_normal
        },
    },
    
    { 
        nandc_bits_page_size,   
        {
             1, 2
        }, 
        {
            nandc_page_hk, nandc_page_2k, nandc_page_4k, nandc_page_8k
        },
    },

    { 
        nandc_bits_bus_width,   
        {
            3, 1
        }, 
        {
            nandc_bus_08, nandc_bus_16
        },
    },
    { 
        nandc_bits_ecc_select,   
        {
            5, 3
        }, 
        {
            nandc2_ecc_enable, nandc2_ecc_disable
        },
    },

    { 
        nandc_bits_ecc_type,   
        {
            10, 2
        }, 
        {
            nandc2_ecc_none , nandc2_ecc_1bit, nandc2_ecc_4smb, nandc2_ecc_8smb 
        },
    },

    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_CMD */
static struct nandc_reg_cont nandc2_cont_cmd[] =  
{
    { 
        nandc_bits_cmd,   
        {
            0, 32
        }, 
        {0},
    },
    
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_ADDRL */
static struct nandc_reg_cont nandc2_cont_addrl[] =  
{
    { 
        nandc_bits_addr_low,   
        {
            0, 32
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_ADDRH*/
static struct nandc_reg_cont nandc2_cont_addrh[] =  
{
    { 
        nandc_bits_addr_high,   
        {
            0, 32
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_ADDRH*/
static struct nandc_reg_cont nandc2_cont_datanum[] =  
{
    { 
        nandc_bits_data_num,   
        {
            0, 32
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_OP_OFFSET */
static struct nandc_reg_cont nandc2_cont_op[] =  
{
    { 
        nandc_bits_operation,   
        {
            0, 7
        }, 
        {nandc2_op_read_start, nandc2_op_read_id, nandc2_op_wirte_start, nandc2_op_erase },
    },

    { 
        nandc_bits_chip_select,   
        {
            7, 2
        }, 
        {nandc_cs_00, nandc_cs_01, nandc_cs_02, nandc_cs_03},
    },

    { 
        nandc_bits_addr_cycle,   
        {
            9, 3
        }, 
        {0},
    },
    
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },    
};


/*define for register NANDC2_REG_STATUS*/
static struct nandc_reg_cont nandc2_cont_opstatus[] =  
{
    { 
        nandc_bits_op_status,   
        {
            0, 1
        }, 
        {0},
    },

    { 
        nandc_bits_nf_status,   
        {
            5, 8
        }, 
        {0},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};


/*define for register NANDC2_REG_ADDRH*/
static struct nandc_reg_cont nandc2_cont_inten[] =  
{
    { 
        nandc_bits_int_enable,   
        {
            0, 32
        }, 
        {nandc2_int_enable_all,  nandc2_int_disable_all},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_ADDRH*/
static struct nandc_reg_cont nandc2_cont_intstatus[] =  
{
    { 
        nandc_bits_int_status,   
        {
            0, 1
        }, 
        {0},
    },

    { 
        nandc_bits_ecc_result,   
        {
            5, 2
        }, 
        {nandc2_ecc_err_none, nandc2_ecc_err_valid, nandc2_ecc_err_invalid },
    },

    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

/*define for register NANDC2_REG_ADDRH*/
static struct nandc_reg_cont nandc2_cont_intclr[] =  
{
    { 
        nandc_bits_int_clear,   
        {
            0, 32
        }, 
        {nandc2_int_clear_all},
    },
   
    { 
        nandc_bits_enum_end,   
        {
            0,0
        }, 
        {
            0
        },
    },
};

struct nandc_reg_desc nandc2_reg_desc_table[] = 
{
    {
        NANDC2_REG_CONF_OFFSET,    /* 0x00 */
        0x2a2,
        nandc2_cont_conf
    },
    
    {
        NANDC2_REG_CMD_OFFSET,    /* 0x0c */
        0x703000,
        nandc2_cont_cmd
    },

    {
        NANDC2_REG_ADDRL_OFFSET,    /* 0x10 */
        0,
        nandc2_cont_addrl
    },

    {
        NANDC2_REG_ADDRH_OFFSET,   /* 0x14 */
        0,
        nandc2_cont_addrh
    },

    {
        NANDC2_REG_DATA_NUM_OFFSET,   /* 0x18 */
        0,
        nandc2_cont_datanum
    },
                
    {
        NANDC2_REG_OP_OFFSET,       /* 0x1c */
        0,
        nandc2_cont_op
    },


    {
        NANDC2_REG_STATUS_OFFSET,   /* 0x20 */
        0,
        nandc2_cont_opstatus
    },
    
    {
        NANDC2_REG_INTEN_OFFSET,
        0,
        nandc2_cont_inten
    },

    {
        NANDC2_REG_INTS_OFFSET,
        0,
        nandc2_cont_intstatus
    },

    {
        NANDC2_REG_INTCLR_OFFSET,
        0,
        nandc2_cont_intclr
    },
    
    {
        NANDC_NULL,
        NANDC_NULL,
        NANDC_NULL
    },
};

struct nandc_dmap datamap_ecc4_v2  =                 /*the data map(page + oob)of the controller buffer, nandc2_dmap*/
{
    STU_SET(.bad2k) 512,      
    STU_SET(.pmap2k) 
    {
        {
            {0,    2048},
            {2048, 64  },   
        }
    },

/* 4k and 8k is not used */
    STU_SET(.bad4k) (512+12)*4 + 512,      
    STU_SET(.pmap4k) 
    {
        {
            {0, 0},
            {0, 0},     
        },
        {
            {0, 0},
            {0, 0},     
        }
    },

    STU_SET(.bad8k) ((512+12)*4*3 + 512),      
    STU_SET(.pmap8k) 
    {
        {
            {0, 0},
            {0, 0},     
        },
    },
/* 4k and 8k is not used */
};

struct nandc_dmap_lookup nandc2_dmap_lookup[] =
{
    {
        &datamap_ecc4_v2,
        nandc2_ecc_4smb
    },

    {NANDC_NULL, 0}
};

u8 nandc2_bitcmd[] =
{
    nandc_bits_cmd ,
    nandc_bits_bus_width,
    nandc_bits_addr_high,
    nandc_bits_addr_low,
    nandc_bits_addr_cycle,
    nandc_bits_chip_select,
    nandc_bits_operation,
    nandc_bits_op_status ,
    nandc_bits_int_status,
    nandc_bits_data_num,
    nandc_bits_ecc_type ,
    nandc_bits_ecc_select,
    nandc_bits_page_size,
    nandc_bits_op_mode ,
    nandc_bits_int_enable,
    nandc_bits_int_clear,
    /*nandc_bits_segment,*/
    nandc_bits_ecc_result,
    nandc_bits_nf_status,
    
    nandc_bits_enum_end 
};

u32 nandc2_ctrl_init(struct nandc_bit_cmd  *  bitcmd, struct nandc_host *host)
{


        bitcmd->op_erase           =   nandc2_op_erase;
        bitcmd->op_reset           =   nandc2_op_reset;
        bitcmd->op_read_id         =   nandc2_op_read_id;
        bitcmd->op_read_start      =   nandc2_op_read_start;
        
        bitcmd->op_read_continue   =   nandc2_op_read_continue;
        bitcmd->op_read_end        =   nandc2_op_read_end;  
        bitcmd->op_write_start     =   nandc2_op_wirte_start;
        bitcmd->op_write_continue  =   nandc2_op_wirte_continue;
        
        bitcmd->op_write_end       =   nandc2_op_wirte_end;
        /*bitcmd->op_segment_start   =   nandc2_segment_first;*/
        /*bitcmd->op_segment_end     =   nandc2_segment_last;*/
        bitcmd->intmask_all        =   nandc2_int_enable_all;
        
        bitcmd->intdis_all         =   nandc2_int_disable_all;
        bitcmd->intclr_all         =   nandc2_int_clear_all;
        bitcmd->ecc_err_none       =   nandc2_ecc_err_none;
        bitcmd->ecc_err_valid      =   nandc2_ecc_err_valid;
        
        bitcmd->ecc_err_invalid    =   nandc2_ecc_err_invalid;
        bitcmd->ecc_all            =   nandc2_ecc_enable;
        bitcmd->ecc_none           =   nandc2_ecc_disable;
        bitcmd->ecc_oob            =   nandc2_ecc_disable;
   
       /* For Hisi V200 nand flash controller,it needs to enable extend data(spare area)
          ecc calibration and correction,before this operation it needs to ioremap     
          nand controller register first for Linux kernel,because it needs to read and
          write the virtual address of nand flash controller registers for linux kernel.
          but for __VXWORKS__,__FASTBOOT__,__BOOTLOADER__,the MMU is disabled.the 
          virtual address is the same as physical address,so the follow operation also 
          has no problem. */      
       NANDC_REG_SETBIT32((u32)host->regbase + NANDC2_REG_CONF_OFFSET , 9, 1, 1);
       
       return NANDC_OK;
}

struct nandc_ecc_info nandc2_eccinfo[] =
{ 
    {nandc_size_2k,     64,     nandc2_ecc_4smb,        &nandc2_oob64_ecc4     },
    
    {0,0,0,0},
};

struct nandc_ctrl_desc nandc2_spec = 
{
    &nandc2_reg_desc_table[0],
    NANDC2_NAME,
    NANDC2_REG_BASE_ADDR, 
    NANDC2_REG_SIZE, 
    NANDC2_BUFFER_BASE_ADDR,
    NANDC2_BUFSIZE_TOTAL,
    NANDC2_MAX_CHIPS
};

/*
struct usr_feature   nand_feature_v2 =
{
    NANDC_NULL, NANDC_NULL
};
*/

struct nandc_ctrl_func   nandc2_operations =
{
    nandc2_ctrl_init,    
};


struct nandc_init_info nandc2_init_info = 
{
    &nandc2_spec,
    &nandc2_operations,
    &nandc2_eccinfo[0],
    &nandc2_bitcmd[0],
    &nandc2_dmap_lookup[0],
    NANDC_NULL,    /* &nand_feature_v2 */
};



