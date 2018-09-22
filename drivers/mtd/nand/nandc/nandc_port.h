/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_port.h
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
#ifndef _NANDC_PORT_H_
#define _NANDC_PORT_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef BUILD_ONEBUILDER
#include "product_config.h"
#endif

#ifndef __KERNEL__
/* keep them for compatibility */
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef unsigned long long u64;

#ifndef FLASHLESS_MACRO
#define FLASHLESS_MACRO
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#endif

#endif

#define NANDC_DEBUG                                 (0x00000001)
#define NANDC_MULTITASK                             (0x00000002)
#define NANDC_DUALCORE                              (0x00000004)
#define NANDC_USE_ASSERT                            (0x00000008)
#define NANDC_USE_MEMPOOL                           (0x00000010)
#define NANDC_READ_ONLY                             (0x00000020)
#define NANDC_LOW_POWER                             (0x00000040)
#define NANDC_USE_MTD                               (0x00000080)

extern void put_str(char *pt,...);

#if defined(WIN32)
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NANDC_COMPILE_FLAG                          (NANDC_DEBUG | NANDC_USE_ASSERT | NANDC_USE_MEMPOOL)

#define STU_SET(element) 
#define __iomem
#define himalloc(p)                                 nandc_malloc(p)
#define hifree(p)                                   nandc_free(p)
#define hiassert(p)                                 do{if(!p){*(volatile int *)(0xffffffff) = 0;}}while(1)
#define hiout(p)                                    (printf p)
#define __func__                                    (0)
#define snprintf                                    _snprintf

struct reg_debuger
{
    void    *handle;
    char    *name;
    u32     regsize;
    u32     simubase;
    u32     chipbase;
    u32     buffaddr;
    FILE    *plogf;
    char    *logname;
    struct reg_debuger *p_next;
};

u32 hiwin_regdebug_write32(void* handle, u32 address, u32 value);
u32 hiwin_regdebug_read32(void* handle, u32 address);
void hiwin_regdebug_bitsset(void* handle, u32 regaddr, u32 bitoffset, u32 bitlength, u32 bitval);
void hiwin_regdebug_bitswrite( void* handle, u32 regaddr, u32 bitoffset, u32 bitlength, u32 bitval);
u32 hiwin_regdebug_bitsget( void* handle, u32 regaddr, u32 bitoffset, u32 bitlength);

#define NANDC_REG_READ32(addr)                              hiwin_regdebug_read32(host->debuger, (addr))
#define NANDC_REG_WRITE32(addr, val)                        hiwin_regdebug_write32(host->debuger, (addr) , (val))
#define NANDC_REG_SETBIT32(addr, pos, bits, val)            hiwin_regdebug_bitsset(host->debuger, addr, pos, bits,val)
#define NANDC_REG_WRITEBIT32(addr, pos, bits, val)          hiwin_regdebug_bitswrite(host->debuger, addr, pos, bits,val)
#define NANDC_REG_GETBIT32(addr, pos, bits)                 hiwin_regdebug_bitsget(host->debuger, addr, pos, bits)

#else  /*not defined(WIN32)*/
#define NANDC_REG_READ32(addr)                              (*(volatile u32*)(addr) )
#define NANDC_REG_WRITE32(addr, val)                        (*(volatile u32 *)(addr) = (val))

#define NANDC_REG_SETBIT32(addr, pos, bits, val)       \
                (*(volatile u32 *)(addr) = (*(volatile u32 *)(addr) & \
                (~((((u32)1 << (bits)) - 1) << (pos)))) \
                 | ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))

#define NANDC_REG_WRITEBIT32(addr, pos, bits, val)       \
                (*(volatile u32 *)(addr) =  ((u32)((val) & (((u32)1 << (bits)) - 1)) << (pos)))



#define NANDC_REG_GETBIT32(addr, pos, bits)     \
                 ((*(volatile u32 *)(addr)  >> (pos)) & \
                 (((u32)1 << (bits)) - 1))


#ifdef __KERNEL__ 
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

#include "BSP.h"
#include <mach/common/bsp_memory.h>

#define STU_SET(element)                            element = 

#define NANDC_COMPILE_FLAG                          (NANDC_DEBUG | NANDC_USE_ASSERT | NANDC_USE_MTD | NANDC_LOW_POWER | NANDC_DUALCORE)

#define __iomem
#define himalloc(p)                                 kmalloc(p, GFP_KERNEL)
#define hifree(p)                                   kfree(p)
#define hiassert(p)                                 BUG_ON(1)
#define hiout(p)                                    printk p

#elif defined(__BOOTLOADER__)
#include <stdio.h>
#include <string.h>
#include "BSP_MEMORY.h"

#define NANDC_COMPILE_FLAG                          (NANDC_USE_MEMPOOL | NANDC_READ_ONLY)

#define STU_SET(element) 
#define __iomem
#define himalloc(p)                                 nandc_malloc(p)
#define hifree(p)                                   nandc_free(p)
#define hiout(p)                                    print_info(p)
#define hioutv(p)                                   print_u32(p)
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
/*启动优化宏开关*/
#define BOOT_OPTI_NAND_CONF
#define memcpy(d,s,l)                               __rt_memcpy((char*)(d), (const char*)(s), (u32)(l))
#endif

#ifdef BOOT_OPTI_NANDC_CPY
#define memcpy(d,s,l)                               __rt_memcpy((char*)(d), (const char*)(s), (u32)(l))
#endif

#ifdef BOOT_OPTI_NANDC_CPY
#define memcpy(d,s,l)                               __rt_memcpy((char*)(d), (const char*)(s), (u32)(l))
#endif

#elif defined(BSP_IMAGE_BOOTROM)
#include <vxworks.h>
#include <stdio.h>
#include <stdLib.h>
#include <string.h>
#include <logLib.h>
#include <semLib.h>
#include <memlib.h>
#include <sysLib.h>
#include <taskLib.h>
#include "BSP.h"
#include "BSP_MEMORY.h"

#ifndef __inline
#define __inline                                    __inline__
#endif

#ifndef __func__
#define __func__                                    __FUNCTION__
#endif

#define NANDC_COMPILE_FLAG                          (NANDC_MULTITASK | NANDC_DEBUG)

#define STU_SET(element) 
#define __iomem
#define himalloc(p)                                 malloc(p)
#define hifree(p)                                   free(p)
#define hiassert(p)                                 (*(volatile int*)(0xffffffff) = 0)
#define hiout(p)                                    printf p

#elif defined(__VXWORKS__)
#include <vxworks.h>
#include <stdio.h>
#include <stdLib.h>
#include <string.h>
#include <logLib.h>
#include <semLib.h>
#include <memlib.h>
#include <sysLib.h>
#include <taskLib.h>
#include "BSP.h"
#include "BSP_MEMORY.h"

#ifndef __inline
#define __inline                                    __inline__
#endif

#ifndef __func__
#define __func__                                    __FUNCTION__
#endif

#define NANDC_COMPILE_FLAG                          (NANDC_MULTITASK | NANDC_DUALCORE | NANDC_LOW_POWER | NANDC_USE_ASSERT | NANDC_DEBUG)


#define STU_SET(element) 
#define __iomem
#define himalloc(p)                                 malloc(p)
#define hifree(p)                                   free(p)
#define hiassert(p)                                 (*(volatile int*)(0xffffffff) = 0)
#define hiout(p)                                    printf p

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
/*启动优化宏开关*/
//#if ((FEATURE_E5 == FEATURE_ON) || (FEATURE_LCARD == FEATURE_ON))
#define BOOT_OPTI_NAND_CONF
#define memcpy(d,s,l)                               __rt_memcpy((char*)(d), (const char*)(s), (u32)(l))
//#endif
#endif

#elif defined(__RVDS__)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#define NANDC_COMPILE_FLAG                          (NANDC_USE_ASSERT | NANDC_USE_MEMPOOL)

#define STU_SET(element) 
#define __iomem
#define himalloc(p)                                 nandc_malloc(p)
#define hifree(p)                                   nandc_free(p)
#define hiassert(p)                                 (*(volatile int*)(0xffffffff) = 0)

#define hiout(p)                                    

#define memcpy(d,s,l)                               nandc_byte_cpy((char*)(d), (const char*)(s), (u32)(l))

#elif defined(__FASTBOOT__)
#include "string.h"
#include "balong.h"
#include "bsp_memory.h"

#define NANDC_COMPILE_FLAG                          (NANDC_USE_ASSERT | NANDC_USE_MEMPOOL | NANDC_DEBUG)

#define STU_SET(element)                            element = 
#define __iomem
#define himalloc(p)                                 nandc_malloc(p)
#define hifree(p)                                   nandc_free(p)
#define hiassert(p)                                 (*(volatile int*)(0xffffffff) = 0)
#define hiout(p)                                    b_printf p

#else
#error no plant macor defined
#endif

#endif  /*defined(WIN32)*/

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_PORT_H_*/

