/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_def.h
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
#ifndef _NANDC_DEF_H_
#define _NANDC_DEF_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#define nandc_latch_none                    (0)
#define nandc_read_latch_start              (1)
#define nandc_read_latch_done               (2)
#define nandc_write_latch_start             (3)
#define nandc_write_latch_done              (4)

#define nandc_size_hk                       (512)
#define nandc_size_2k                       (2048)
#define nandc_size_4k                       (4096)
#define nandc_size_8k                       (8192)

#define nandc_block_64                      (64)
#define nandc_block_128                     (128)

#define nandc_size_1Gbit                    (128 << 20) /*128MB*/

#define NANDC_ADDR_INVALID                  (0xFFFFFFFF)

/***********NANDC ctrl option bit value start*****************************************
---------------------------------------------------------------------------
*  bit 31:5   --    reserved             
---------------------------------------------------------------------------
*  bit 4:3     RW    read type      ->   read  data or oob
---------------------------------------------------------------------------
*  bit 2:2     RW    ECC             ->  use default ecc or forbid it (no ecc)
 --------------------------------------------------------------------------                                              
*  bit 1:1     RW    WAIT           ->   waite operation complete: int mode or poll mode 
--------------------------------------------------------------------------                                                
*  bit 0:0     RW    DMA            ->   dma mode or normal mode 
*****************************************************************/
#define NANDC_OPTION_DMA_DISABLE            (0x0000)
#define NANDC_OPTION_DMA_ENABLE             (0x0001)

#define NANDC_OPTION_WAIT_POLL              (0x0000)
#define NANDC_OPTION_WAIT_INT               (0x0002)


#define NANDC_OPTION_ECC_DEFAULT            (0x0000)
#define NANDC_OPTION_ECC_FORBID             (0x0004)

#define NANDC_OPTION_DATA_OOB               (0x0000)
#define NANDC_OPTION_OOB_ONLY               (0x0007)
#define NANDC_OPTION_DATA_ONLY              (0x0010)

/***********NANDC ctrl option bit value end*****************************************/

#define NANDC_GOOD_BLOCK                    (0)
#define NANDC_BAD_BLOCK                     (1)

#define NANDC_EARSED_BLOCK                  (1)
#define NANDC_DIRTY_BLOCK                   (0)


#define NAND_CMD_NOP		                (0)

#define nandc_bits_enum_start               (0) 
#define nandc_bits_cmd                      (1)
#define nandc_bits_cmd2                     (2)
#define nandc_bits_cmd3                     (3)
#define nandc_bits_bus_width                (4)
#define nandc_bits_addr_high                (5)
#define nandc_bits_addr_low                 (6)
#define nandc_bits_addr_cycle               (7)
#define nandc_bits_chip_select              (8)
#define nandc_bits_operation                (9)
#define nandc_bits_op_status                (10)
#define nandc_bits_int_status               (11)
#define nandc_bits_data_num                 (12)
#define nandc_bits_ecc_type                 (13)
#define nandc_bits_ecc_select               (14)
#define nandc_bits_page_size                (15)
#define nandc_bits_op_mode                  (16)
#define nandc_bits_int_enable               (17)
#define nandc_bits_int_clear                (18)
#define nandc_bits_segment                  (19)
#define nandc_bits_ecc_result               (20)
#define nandc_bits_nf_status                (21)

#define nandc_bits_enum_end                 (0xff)   /*don't add after this*/

#define nandc_alige_size(p, a)              (((p)+  ((a) - 1)) & ~ ((a) - 1))

#define nandc_reg_desc_max                  (6)


#define nandc_addrcycle_0                   (0)
#define nandc_addrcycle_1                   (1)
#define nandc_addrcycle_4                   (4)
#define nandc_addrcycle_5                   (5)

#define nandc_op_boot                       (0)
#define nandc_op_normal                     (1)

#define nandc_page_hk                       (0)       /*half of kilo bytes = 512 bytes*/
#define nandc_page_2k                       (1)
#define nandc_page_4k                       (2)
#define nandc_page_8k                       (3)


#define nandc_bus_08                        (0)
#define nandc_bus_16                        (1)

#define nandc_cs_00                         (0)
#define nandc_cs_01                         (1)
#define nandc_cs_02                         (2)
#define nandc_cs_03                         (3)

#define nandc_status_op_done		        (1)
#define nandc_status_int_done		        (1)

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/



