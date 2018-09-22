/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_mtd.h
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
#ifndef _NANDC_MTD_H_
#define _NANDC_MTD_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"

#include "nandc_v200.h"
#include "nandc_v320.h"
#include "nandc_v400.h"


#if defined(NANDC_USE_V200)

#define NANDC_NAME                  NANDC2_NAME
#define NANDC_REG_SIZE              NANDC2_REG_SIZE
#define NANDC_BUFSIZE_EXTERN        NANDC2_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC2_BUFSIZE_TOTAL

#elif defined (NANDC_USE_V320)
#define NANDC_NAME                  NANDC3_NAME
#define NANDC_REG_SIZE              NANDC3_REG_SIZE
#define NANDC_BUFSIZE_EXTERN        NANDC3_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC3_BUFSIZE_TOTAL

#elif defined (NANDC_USE_V400)

#define NANDC_NAME                  NANDC4_NAME
#define NANDC_REG_SIZE              NANDC4_REG_SIZE
#define NANDC_BUFSIZE_EXTERN        NANDC4_BUFSIZE_EXTERN
#define NANDC_BUFSIZE_TOTAL         NANDC4_BUFSIZE_TOTAL
#else
#error no bsp board defined!!
#endif

#define NANDC_ADDR_CYCLE_MASK                   (0x4)
#define NANDC_CHIP_DELAY                        (25)

/*****************************************************************************/

/*****************************************************************************/


u32  nandc_host_init_mtd(struct nandc_host * host, 
                                    u32 pagesize, 
                                    u32 sparesize,
                                    u32 chipsize, 
                                    u32 erasesize,
                                    u32 buswidth,
                                    u32 numchips,
                                    struct mtd_partition  * ptable,
                                    u32 nr_parts
                                    );
/*****************************************************************************/


/*****************************************************************************/


#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_MTD_H_*/



