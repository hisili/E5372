/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_v200.h
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
#ifndef _NANDC_V200_H_
#define _NANDC_V200_H_
    
#ifdef __cplusplus
extern "C"
{
#endif


#define NANDC2_NAME                             "NANDC_V2.00"   /* TODO:   rename to "hinandv4" */
#define NANDC2_BUFSIZE_BASIC                    (2048)
#define NANDC2_BUFSIZE_EXTERN                   (128)
#define NANDC2_BUFSIZE_TOTAL                    (NANDC2_BUFSIZE_BASIC + NANDC2_BUFSIZE_EXTERN)
#define NANDC2_REG_SIZE                         (0x00B0)

#define NANDC2_REG_BASE_ADDR                    (NANDC_REG_BASE_ADDR)
#define NANDC2_BUFFER_BASE_ADDR                 (NANDC_BUFFER_BASE_ADDR)


#define NANDC2_MAX_CHIPS                        NANDC_MAX_CHIPS
/***********NANDC2_REG_CONF_OFFSET*****************************************
-------------------------------------------------------------------------
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
*****************************************************************/
#define NANDC2_REG_CONF_OFFSET                  (0x00)	

#define NANDC2_REG_PWIDTH                       (0x04)
#define NANDC2_REG_OPIDLE                       (0x08)
/***********NANDC2_REG_CMD*****************************************
---------------------------------------------------------------------------
*  bit 31:24   --    reserved             
---------------------------------------------------------------------------
*  bit 23:16  RW    read_status_cmd      ->  read status command
 --------------------------------------------------------------------------                                              
*  bit 7:0     RW    cmd2         ->   the secend command send to nand device 
--------------------------------------------------------------------------                                                
*  bit 7:0     RW    cmd1         ->   the first command send to nand device 
*****************************************************************/
#define NANDC2_REG_CMD_OFFSET                   (0x0C)

#define NANDC2_REG_ADDRL_OFFSET                 (0x10)
#define NANDC2_REG_ADDRH_OFFSET                 (0x14)	
#define NANDC2_REG_DATA_NUM_OFFSET              (0x18)	

#define NANDC2_REG_OP_OFFSET                    (0x1C)
#define NANDC2_REG_OP_DEFAULT                   (0x1C)

#define NANDC2_REG_STATUS_OFFSET                (0x20)
#define NANDC2_REG_INTEN_OFFSET                 (0x24)	
#define NANDC2_REG_INTS_OFFSET                  (0x28)	
#define NANDC2_REG_INTCLR_OFFSET                (0x2C)

#define NANDC2_REG_LOCK                         (0x30)
#define NANDC2_REG_LOCK_SA0                     (0x34)	
#define NANDC2_REG_LOCK_SA1                     (0x38)
#define NANDC2_REG_LOCK_SA2                     (0x3C)
    
#define NANDC2_REG_LOCK_SA3                     (0x40)	
#define NANDC2_REG_LOCK_EA0                     (0x44)
#define NANDC2_REG_LOCK_EA1                     (0x48)	
#define NANDC2_REG_LOCK_EA2                     (0x4C)

#define NANDC2_REG_LOCK_EA3                     (0x50)	
#define NANDC2_REG_EXPCMD                       (0x54)
#define NANDC2_REG_EXBCMD                       (0x58)
#define NANDC2_REG_ECC_TEST                     (0x5C)

#define NANDC2_REG_ECC_PARA_OFFSET              (0x70)
#define NANDC2_REG_VERSION                      (0x74)
#define NANDC2_REG_SEGMENT_ID                   (0x84)   


#define nandc2_ecc_none                         (0x0)
#define nandc2_ecc_1bit                         (0x1)
#define nandc2_ecc_4smb                         (0x2)
#define nandc2_ecc_8smb                         (0x3)

#define nandc2_ecc_enable                       (0x1)   
#define nandc2_ecc_disable                      (0x0)   

#define nandc2_op_read_id                       (0x66)   /*0110 0110   cmd1_en + addr_en + wait_ready_en  + read_data_en*/  
#define nandc2_op_read_start                    (0x6e)
#define nandc2_op_read_continue                 (0x02)    
#define nandc2_op_read_end                      (0x02)    
#define nandc2_op_wirte_start                   (0x70)
#define nandc2_op_wirte_continue                (0x10)
#define nandc2_op_wirte_end                     (0x1d)
#define nandc2_op_erase                         (0x6d)
#define nandc2_op_reset                         (0x44)   /*0100 0100   cmd1_en + wait_ready_en*/

#define nandc2_int_enable_all                   (0x1ff)
#define nandc2_int_disable_all                  (0x000)
#define nandc2_int_clear_all                    (0x1ff)

/*#define nandc2_segment_first               (0x0)*/
/*#define nandc2_segment_last                (0x1)*/


#define nandc2_ecc_err_none                     (0x0)
#define nandc2_ecc_err_valid                    (0x1)
#define nandc2_ecc_err_invalid                  (0x2)  /*for p500: never check this for all 0xff ecc error report*/


#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_V200_H_*/

