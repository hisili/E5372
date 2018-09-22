/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : MemoryMap.h
  版 本 号   : 初稿
  作    者   : file
  生成日期   : 2010年06月25日
  最近修改   :
  功能描述   : Sytem Memory Map
  函数列表   :
  修改历史   :
  1.日    期   : 2010年06月25日
    修改内容   : 创建文件

******************************************************************************/
#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include "memMapGlobal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
/*#include "MemoryConfig.h"*/


#pragma pack(4)

#define IO_ADDRESS(x)         ((((x) & 0xF0000000) >> 4) | ((x) & 0x00FFFFFF) | 0xF0000000)

/*IPF Register*/
#define IPF_PHY_BASE_ADDR                   (0x900AC000)

/*AXI Memory Base*/
#define AXI_PHY_BASE_ADDR                   (0x30000000)

/*TTF DICC Spinlock Memory*/
#define ECS_TTF_DICC_SPINLOCK_ADDR          (GLOBAL_AXI_DICC_ADDR)
#define ECS_TTF_ACPU_FREE_MEM_CNT_ADDR      (GLOBAL_AXI_DICC_ADDR + 4)
#define ECS_TTF_DICC_SPINLOCK_SIZE          (GLOBAL_AXI_DICC_SIZE)

#define ECS_MDDR_RESERVE_ADDR               (GLOBAL_MEM_GU_RESERVED_ADDR)
#define ECS_MDDR_RESERVE_VIRT_ADDR          (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT)
#define ECS_MDDR_RESERVE_SIZE               (GLOBAL_MEM_GU_RESERVED_SIZE)
#define ECS_MDDR_RESERVE_BASE_ADDR          (GLOBAL_MEM_GU_RESERVED_ADDR)
#define ECS_MDDR_RESERVE_BASE_ADDR_VIRT     (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT)

#define ECS_TTF_SIZE                        (0x00C00000)
#define ECS_ARM_DSP_BUFFER_SIZE             (0x00300000)
#define ECS_UPA_SIZE                        (0x00020000)
#define ECS_CQI_SIZE                        (0x00003400)
#define ECS_APT_SIZE                        (0x00008400)
#define ECS_ET_SIZE                         (0x00004800)
#define ECS_BBP_MASTER_SIZE                 (0x00000000)
#define ECS_HIFI_CARM_MBOX_SIZE             (0x00000000)
#define ECS_NV_SIZE                         (0x00030000)
#define ECS_TTF_DICC_SIZE                   (0x00010000)

#define ECS_TTF_BASE_ADDR                   (ECS_MDDR_RESERVE_ADDR)
#define ECS_ARM_DSP_BUFFER_BASE_ADDR        (ECS_TTF_BASE_ADDR + ECS_TTF_SIZE)
#define ECS_UPA_BASE_ADDR                   (ECS_ARM_DSP_BUFFER_BASE_ADDR + ECS_ARM_DSP_BUFFER_SIZE)
#define ECS_CQI_BASE_ADDR                   (ECS_UPA_BASE_ADDR            + ECS_UPA_SIZE)
#define ECS_APT_BASE_ADDR                   (ECS_CQI_BASE_ADDR            + ECS_CQI_SIZE)
#define ECS_ET_BASE_ADDR                    (ECS_APT_BASE_ADDR            + ECS_APT_SIZE)
#define ECS_BBP_MASTER_BASE_ADDR            (ECS_ET_BASE_ADDR             + ECS_ET_SIZE)
#define ECS_HIFI_CARM_MBOX_BASE_ADDR        (ECS_BBP_MASTER_BASE_ADDR + ECS_BBP_MASTER_SIZE)
#define ECS_NV_BASE_ADDR                    (ECS_HIFI_CARM_MBOX_BASE_ADDR + ECS_HIFI_CARM_MBOX_SIZE)
#define ECS_TTF_DICC_ADDR                   (ECS_NV_BASE_ADDR  + ECS_NV_SIZE)

/*TTF Memory Virtual Address*/
#define ECS_TTF_BASE_ADDR_VIRT              (ECS_MDDR_RESERVE_BASE_ADDR_VIRT)

/*NV Virtual Address*/
#define ECS_NV_BASE_ADDR_VIRT               (ECS_NV_BASE_ADDR - ECS_MDDR_RESERVE_ADDR \
                                                + ECS_MDDR_RESERVE_VIRT_ADDR)
    
/*DICC Virtual Address*/
#define ECS_TTF_DICC_ADDR_VIRT              (ECS_TTF_DICC_ADDR - ECS_MDDR_RESERVE_BASE_ADDR \
                                                + ECS_MDDR_RESERVE_BASE_ADDR_VIRT)  

/*TTF Memory Map*/
#define TTFA9_DRAM_SHARE_PHYS               (ECS_MDDR_RESERVE_ADDR)
#define TTFA9_DRAM_SHARE_VIRT               (ECS_MDDR_RESERVE_VIRT_ADDR)

/*IPF Register Map*/
#define IPF_VIRT_BASE_ADDR    IO_ADDRESS(IPF_PHY_BASE_ADDR)

/*AXI Memory Map*/
#define AXI_VIRT_BASE_ADDR    IO_ADDRESS(AXI_PHY_BASE_ADDR)
//#define AXI_VIRT_TO_PHY(var)  (((unsigned int)(var) - AXI_VIRT_BASE_ADDR) + AXI_PHY_BASE_ADDR)
//#define AXI_PHY_TO_VIRT(var)  IO_ADDRESS(var)


#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* MEMORY_MAP_H */

