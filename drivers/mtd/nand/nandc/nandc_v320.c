/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_v320.c
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
1.date:2011-07-27
 question number:
 modify reasion:         create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/


#include "nandc_inc.h"
#include "nandc_v320.h"

/*****************************************************************************/
/* ecc_none */

struct nand_ecclayout nandc3_oob64_ecc_none = 
{
    STU_SET(.eccbytes)  0,
    STU_SET(.eccpos)    {0},
    STU_SET(.oobavail)  64,
    STU_SET(.oobfree)     
    { 
        {4, 2},{16, 6},{32, 6},{48, 6},
    }
}; 


struct nand_ecclayout nandc3_oob64_ecc4 = 
{
    STU_SET(.eccbytes)  0,
    STU_SET(.eccpos)    {0},
    STU_SET(.oobavail)  64,
    STU_SET(.oobfree)     
    { 
        {4, 2},{16, 6},{32, 6},{48, 6},
    }
}; 

struct nand_ecclayout nandc3_oob128_ecc4 = 
{
    STU_SET(.eccbytes)  0,
    STU_SET(.eccpos)    {0},
    STU_SET(.oobavail)   128,
    STU_SET(.oobfree)     
    { 
        {4,  2},{16, 6},{32, 6},{48, 6},
        {64, 6},{80, 6},{96, 6},{112,6},
    }
}; 

struct nand_ecclayout nandc3_oob208_ecc4 = 
{
    STU_SET(.eccbytes)  0,
    STU_SET(.eccpos)    {0},
    STU_SET(.oobavail)   208,
    STU_SET(.oobfree)     
    { 
        {4,  2},{26, 6},{52, 6},{78, 6},
        {104,6},{130,6},{156,6},{182,6},
    }
}; 

/*NANDC3_REG_CONF_OFFSET                (0x00)	*/
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
static struct nandc_reg_cont nandc3_cont_conf[] =  
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
            5, 1
        }, 
        {
            nandc3_ecc_enable, nandc3_ecc_disable
        },
    },

    { 
        nandc_bits_ecc_type,   
        {
            10, 3
        }, 
        {
            nandc3_ecc_none , nandc3_ecc_1bit, nandc3_ecc_4smb, nandc3_ecc_24p1kbit 
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

/*define for register NANDC3_REG_CMD */
static struct nandc_reg_cont nandc3_cont_cmd[] =  
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

/*define for register NANDC3_REG_ADDRL */
static struct nandc_reg_cont nandc3_cont_addrl[] =  
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

/*define for register NANDC3_REG_ADDRH*/
static struct nandc_reg_cont nandc3_cont_addrh[] =  
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

/*define for register NANDC3_REG_ADDRH*/
static struct nandc_reg_cont nandc3_cont_datanum[] =  
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

/*define for register NANDC3_REG_OP_OFFSET */
static struct nandc_reg_cont nandc3_cont_op[] =  
{
    { 
        nandc_bits_operation,   
        {
            0, 7
        }, 
        {nandc3_op_read_start, nandc3_op_read_id, nandc3_op_wirte_start, nandc3_op_erase },
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


/*define for register NANDC3_REG_STATUS*/
static struct nandc_reg_cont nandc3_cont_opstatus[] =  
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


/*define for register NANDC3_REG_ADDRH*/
static struct nandc_reg_cont nandc3_cont_inten[] =  
{
    { 
        nandc_bits_int_enable,   
        {
            0, 32
        }, 
        {nandc3_int_enable_all,  nandc3_int_disable_all},
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

/*define for register NANDC3_REG_ADDRH*/
static struct nandc_reg_cont nandc3_cont_intstatus[] =  
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
        {nandc3_ecc_err_none, nandc3_ecc_err_valid, nandc3_ecc_err_invalid },
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

/*define for register NANDC3_REG_ADDRH*/
static struct nandc_reg_cont nandc3_cont_intclr[] =  
{
    { 
        nandc_bits_int_clear,   
        {
            0, 9
        }, 
        {nandc3_int_clear_all},
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

struct nandc_reg_desc nandc3_reg_desc_table[] = 
{
    {
        NANDC3_REG_CONF_OFFSET,
        0x2a2,
        nandc3_cont_conf
    },
    
    {
        NANDC3_REG_CMD_OFFSET,
        0x703000,
        nandc3_cont_cmd
    },

    {
        NANDC3_REG_ADDRL_OFFSET,
        0,
        nandc3_cont_addrl
    },

    {
        NANDC3_REG_ADDRH_OFFSET,
        0,
        nandc3_cont_addrh
    },

    {
        NANDC3_REG_DATA_NUM_OFFSET,
        0,
        nandc3_cont_datanum
    },
                
    {
        NANDC3_REG_OP_OFFSET,
        0,
        nandc3_cont_op
    },


    {
        NANDC3_REG_STATUS_OFFSET,
        0,
        nandc3_cont_opstatus
    },
    
    {
        NANDC3_REG_INTEN_OFFSET,
        0,
        nandc3_cont_inten
    },

    {
        NANDC3_REG_INTS_OFFSET,
        0,
        nandc3_cont_intstatus
    },

    {
        NANDC3_REG_INTCLR_OFFSET,
        0,
        nandc3_cont_intclr
    },
    
    {
        NANDC_NULL,
        NANDC_NULL,
        NANDC_NULL
    },
};

struct nandc_dmap datamap_eccnone_v3  =                 /*the data map(page + oob)of the controller buffer, nandc3_dmap*/
{
    STU_SET(.bad2k) 2048,      
    STU_SET(.pmap2k) 
    {
        {
            STU_SET(.data) {0,    2048},
            STU_SET(.oob)  {2048, 64  },   
        }
    },

};

struct nandc_dmap datamap_ecc4_v3  =                 /*the data map(page + oob)of the controller buffer, nandc3_dmap*/
{
    STU_SET(.bad2k) 512,      
    STU_SET(.pmap2k) 
    {
        {
            STU_SET(.data) {0,    2048},
            STU_SET(.oob)  {2048, 64  },   
        }
    },

    STU_SET(.bad4k) (512+12)*4 + 512,      
    STU_SET(.pmap4k) 
    {
        {
            {0,    2048},
            {2048, 64   },     
        },
        {
            {0,    2048},
            {2048, 64  },     
        }
    },

    STU_SET(.bad8k) (512+12)*4*3 + 512,      
    STU_SET(.pmap8k) 
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

};


struct nandc_dmap datamap_ecc8_v3  =                 /*the data map(page + oob)of the controller buffer, nandc3_dmap*/
{
    STU_SET(.bad2k) 512,      
    STU_SET(.pmap2k) 
    {
        {
            {0,    2048},
            {2048, 104  },   
        }
    },

    STU_SET(.bad4k) (512+26)*4 + 512,      
    STU_SET(.pmap4k)
    {
        {
            {0,    2048},
            {2048, 104  },   
        },
        {
            {0,    2048},
            {2048, 104  },   
        }
    },

    STU_SET(.bad8k) (512+26)*4*3 + 512,      
    STU_SET(.pmap8k) 
    {
        {
            {0, 0},
            {0, 0},     
        },
        {
            {0, 0},
            {0, 0},     
        }
    }
};

struct nandc_dmap_lookup nandc3_dmap_lookup[] =
{
    {
        &datamap_eccnone_v3,
        nandc3_ecc_none
    },
    
    {
        &datamap_ecc4_v3,
        nandc3_ecc_4smb
    },

    {
        &datamap_ecc8_v3,
        nandc3_ecc_8smb
    },

    {NANDC_NULL, 0}
};

u8 nandc3_bitcmd[] =
{
    nandc_bits_cmd ,
    /*
    nandc_bits_cmd2 ,
    nandc_bits_cmd3 ,
    */
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

u32 nandc3_ctrl_init(struct nandc_bit_cmd  *  bitcmd, struct nandc_host *host)
{


    HI_NOTUSED(host);

    bitcmd->op_erase           =   nandc3_op_erase;
    bitcmd->op_reset           =   nandc3_op_reset;
    bitcmd->op_read_id         =   nandc3_op_read_id;
    bitcmd->op_read_start      =   nandc3_op_read_start;
    
    bitcmd->op_read_continue   =   nandc3_op_read_continue;
    bitcmd->op_read_end        =   nandc3_op_read_end;  
    bitcmd->op_write_start     =   nandc3_op_wirte_start;
    bitcmd->op_write_continue  =   nandc3_op_wirte_continue;
    
    bitcmd->op_write_end       =   nandc3_op_wirte_end;
    bitcmd->intmask_all        =   nandc3_int_enable_all;
    
    bitcmd->intdis_all         =   nandc3_int_disable_all;
    bitcmd->intclr_all         =   nandc3_int_clear_all;
    bitcmd->ecc_err_none       =   nandc3_ecc_err_none;
    bitcmd->ecc_err_valid      =   nandc3_ecc_err_valid;
    
    bitcmd->ecc_err_invalid    =   nandc3_ecc_err_invalid;
    bitcmd->ecc_all            =   nandc3_ecc_enable;
    bitcmd->ecc_none           =   nandc3_ecc_disable;
    bitcmd->ecc_oob            =   nandc3_ecc_disable;

    /*set NANDC3_REG_PWIDTH to adapt driver ability impact by EBI with LCD's EMI  */
#ifndef WIN32
#if defined(BOOT_OPTI_NAND_CONF)
    *(volatile int*)((int)host->regbase + NANDC3_REG_OPIDLE) = 0x0035F553;
#if (FEATURE_HILINK == FEATURE_ON)
    *(volatile int*)((int)host->regbase + NANDC3_REG_PWIDTH) = 0x00000333;
#else
    *(volatile int*)((int)host->regbase + NANDC3_REG_PWIDTH) = 0x00000555;    
#endif
#else
    *(volatile int*)((int)host->regbase + NANDC3_REG_PWIDTH) = 0x00000555;
#endif
#endif

    return NANDC_OK;
}

struct nandc_ecc_info nandc3_eccinfo[] =
{ 
    {nandc_size_4k,     208,    nandc3_ecc_8smb,        &nandc3_oob208_ecc4    },
    {nandc_size_4k,     128,    nandc3_ecc_4smb,        &nandc3_oob128_ecc4    },

    {nandc_size_2k,     64,     nandc3_ecc_4smb,        &nandc3_oob64_ecc4     },
    {nandc_size_2k,     64,     nandc3_ecc_none,        &nandc3_oob64_ecc_none },


    {0,0,0,0},
};

struct nandc_ctrl_desc nandc3_spec = 
{
    &nandc3_reg_desc_table[0],
        
    NANDC3_NAME,
    
    NANDC3_REG_BASE_ADDR, 
    
    NANDC3_REG_SIZE, 
    
    NANDC3_BUFFER_BASE_ADDR,
    
    NANDC3_BUFSIZE_TOTAL,
    
    NANDC3_MAX_CHIPS
};


struct nandc_ctrl_func   nandc3_operations =
{
    nandc3_ctrl_init,    
};


struct nandc_init_info nandc3_init_info = 
{
    &nandc3_spec,
        
    &nandc3_operations,
    
    &nandc3_eccinfo[0],
    
    &nandc3_bitcmd[0],
    
    &nandc3_dmap_lookup[0],
    
    NANDC_NULL,    /* &nand_feature_v3 */
};



