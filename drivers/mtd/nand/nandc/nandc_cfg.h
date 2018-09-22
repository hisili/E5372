/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_cfg.h
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
#ifndef _NANDC_CFG_H_
#define _NANDC_CFG_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define NANDC_BUFSIZE_BASIC                    (2048)


#define NANDC_RANDOM_ACCESS_THRESHOLD           (512)
#define NANDC_ECC_ACCESS_THRESHOLD              (NANDC_BUFSIZE_BASIC)


/* keep them for compatibility */
#define NANDC_STATUS_TIMEOUT	                (10)
#define NANDC_READID_SIZE	                    (8)
#define NANDC_MAX_PAGE_SWAP                     (8)

#define NANDC_MAX_CHIPS                         (1)

#define NANDC_ERASE_FLAG_PAGE_OFFSET            (4)
#define NANDC_BAD_FLAG_PAGE_OFFSET              (0)
#define NANDC_BAD_FLAG_PAGE1_OFFSET             (1)
#define NANDC_BAD_FLAG_PAGE_LAST_OFFSET         (63)
#define NANDC_BAD_FLAG_BYTE_SIZE                (4)



#define  NANDC_NULL_ASSERT_FORMAT               "0x%x,file:%s; line:%d \n"
#define  NANDC_NULL_PARAM_FORMAT                "NULL parameter\n "

#define NANDC_RD_RESULT_TIMEOUT                 (100000)
#define NANDC_PRG_RESULT_TIMEOUT                (1000000)
#define NANDC_ERASE_RESULT_TIMEOUT              (10000000)
#define NANDC_RESULT_TIME_OUT                   (10000000)

#define NANDC_RD_RETRY_CNT                      (3)

#define NANDC_MARK_SIZE                         (64)  /*in byte*/
#define NANDC_BADBLOCK_FLAG_SIZE                NANDC_MARK_SIZE  /*in byte*/
#define NANDC_ERASED_FLAG_SIZE                  NANDC_MARK_SIZE  /*in byte*/

#define NANDC_MAX_BLOCK_MASK                    (0xFFFFF)  /*(1MB)*/
#define NANDC_MAX_PAGE_MASK                     (0x1FFF)  /*(8kB)*/

/*#define NANDC_NAND_SIZE_WIDTH64                 */

#ifndef CONFIG_YAFFS_NO_YAFFS1
#define YAFFS_BYTES_PER_SPARE                   16
#endif


#ifndef NANDC_CTRL_DEBUG
#define NANDC_CTRL_DEBUG
#undef NANDC_CTRL_DEBUG
#endif


#if defined(BOARD_FPGA) && defined(VERSION_V3R2)
#define NANDC_USE_V200
#define NANDC_REG_BASE_ADDR                    (0x80020000)
#define NANDC_BUFFER_BASE_ADDR                 (0x90000000)
#define NANDC_USE_SPINLOCK
#define RVDS_TRACE_WRITE_BUFF                  (0xc6000000)

#elif defined (BOARD_SFT) && defined(VERSION_V7R1)
#define NANDC_USE_V320
#define NANDC_REG_BASE_ADDR                    (0x900A4000)
#define NANDC_BUFFER_BASE_ADDR                 (0xa0000000)
#define NANDC_USE_IPC
#define RVDS_TRACE_WRITE_BUFF                  (0x2ffe0000)

#elif (defined (BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
    /* ASIC平台ES和CS无关的代码 by monan cs    */
    #if defined(CHIP_BB_6756CS)
        #define NANDC_USE_V400
        #define NANDC_REG_BASE_ADDR                    (0xa0100000)
        #define NANDC_BUFFER_BASE_ADDR                 (0xa0000000)
        #define NANDC_USE_IPC
        #define RVDS_TRACE_WRITE_BUFF                  (0x33f00000)
    #else
       /*由于以前没有使用V7R1ES芯片套片宏，这个else分支就是V7R1ES芯片相关的独有代码*/
        #define NANDC_USE_V320
        #define NANDC_REG_BASE_ADDR                    (0x900A4000)
        #define NANDC_BUFFER_BASE_ADDR                 (0xa0000000)
        #define NANDC_USE_IPC
        #define RVDS_TRACE_WRITE_BUFF                  (0x2ffe0000)
    #endif
#elif (defined (CHIP_BB_6920ES)\
    	&& (defined (BOARD_SFT)||defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
#define NANDC_USE_V320
#define NANDC_REG_BASE_ADDR                    (0x900A4000)
#define NANDC_BUFFER_BASE_ADDR                 (0xa0000000)
#define NANDC_USE_IPC
#define NANDC_SKIP_DEBUG
#define RVDS_TRACE_WRITE_BUFF                  (0x33f00000)
#elif (defined(BOARD_SFT) && defined(VERSION_V3R2))
#define NANDC_USE_V400
#define NANDC_REG_BASE_ADDR                    (0xa0100000)
#define NANDC_BUFFER_BASE_ADDR                 (0xa0000000)
#define NANDC_USE_IPC
#define RVDS_TRACE_WRITE_BUFF                  (0x33f00000)
#elif (defined (CHIP_BB_6920CS)\
	  && (defined (BOARD_SFT)||defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
#define NANDC_USE_V400
#define NANDC_REG_BASE_ADDR                    (0x900A4000)
#define NANDC_BUFFER_BASE_ADDR                 (0xa0000000)
#define NANDC_USE_IPC
#define NANDC_SKIP_DEBUG
#define RVDS_TRACE_WRITE_BUFF                  (0x33f00000)
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
       && defined (BOARD_FPGA_P500))
#define NANDC_USE_V200
#define NANDC_REG_BASE_ADDR                    (0x80020000)
#define NANDC_BUFFER_BASE_ADDR                 (0x90000000)
#define NANDC_USE_SPINLOCK
#define NANDC_SKIP_DEBUG
#define RVDS_TRACE_WRITE_BUFF                  (0xc6000000)
#else
#error no bsp board defined!!
#endif


#ifdef NANDC_USE_V200
#define ECC_CHECK_FRESH_PAGE
#define NANDC_READ_RESULT_DELAY
#elif defined(NANDC_USE_V320)
#define NANDC_READ_RESULT_DELAY
/*
#define BAD_FLAG_SWAP_2K
#define ECC_USE_HARDWEARE_SELETE
*/
#elif defined(NANDC_USE_V400)
#define NANDC_READ_RESULT_DELAY
#else
#error no nand controller defined!!
#endif

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/



