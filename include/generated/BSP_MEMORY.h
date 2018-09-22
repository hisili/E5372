/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_MEMORY.h
*
*
*   描    述 :  空间分配(内存、FLASH、AXI MEM)
*
*
*
*************************************************************************/
#ifndef __BSP_MEMORY_H__
#define __BSP_MEMORY_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "product_config.h"
#include "memMapGlobal.h"
#include "arm_pbxa9.h"

#ifndef PRODUCT_CFG_IMAGE_TYPE_BOOTLOADER
#include "config.h"
#endif

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#include "FeatureConfigDRV.h"
#endif

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#if defined(__KERNEL__)
#include "generated/product_config.h"
#elif defined(__FASTBOOT__)
#include "../../../../../../include/generated/product_config.h"
#else
#include "product_config.h"
#endif

#define PTABLE_FLASH_SIZE               (PRODUCT_CFG_FLASH_TOTAL_SIZE)
#define PTABLE_VER_STR                  (PRODUCT_CFG_PTABLE_VER_NAME)
#define PTABLE_BOOTROM_VER_STR          (PRODUCT_CFG_PTABLE_BOOTROM_VER)

#define PTABLE_BOOTROM_ENTRY            (PRODUCT_CFG_BOOTROM_RAM_LOW_ADDR)

#define PTABLE_VXWORKS_MCORE_ENTRY      (PRODUCT_CFG_MCORE_RAM_LOW_ADDR)

#define PTABLE_FASTBOOT_ENTRY           (PRODUCT_CFG_FASTBOOT_ENTRY)

#define PTABLE_BOOTLOADER_ENTRY         (AXI_MEM_ADDR)

/* bootload */
#define PTABLE_BOOTLOADER_START          0x0
#define PTABLE_BOOTLOADER_LEN            (PRODUCT_CFG_FLASH_BOOTLOADER_LEN)
#define PTABLE_BOOTLOADER_END            (PTABLE_BOOTLOADER_START + PTABLE_BOOTLOADER_LEN)
    
/* NV LTE */
#define PTABLE_NV_LTE_START              PTABLE_BOOTLOADER_END
#define PTABLE_NV_LTE_LEN                (PRODUCT_CFG_FLASH_NV_LTE_LEN)
#define PTABLE_NV_LTE_END                (PTABLE_NV_LTE_START + PTABLE_NV_LTE_LEN)
    
/* NV GU */
#define PTABLE_NV_GU_START               PTABLE_NV_LTE_END
#define PTABLE_NV_GU_LEN                 (PRODUCT_CFG_FLASH_NV_GU_LEN)
#define PTABLE_NV_GU_END                 (PTABLE_NV_GU_START + PTABLE_NV_GU_LEN)
    
/* bootrom A */
#define PTABLE_BOOTROM_A_START           PTABLE_NV_GU_END
#define PTABLE_BOOTROM_A_LEN             (PRODUCT_CFG_FLASH_BOOTROM_A_LEN)
#define PTABLE_BOOTROM_A_END             (PTABLE_BOOTROM_A_START + PTABLE_BOOTROM_A_LEN)
    
/* bootrom B */
#define PTABLE_BOOTROM_B_START           PTABLE_BOOTROM_A_END
#define PTABLE_BOOTROM_B_LEN             (PRODUCT_CFG_FLASH_BOOTROM_B_LEN)
#define PTABLE_BOOTROM_B_END             (PTABLE_BOOTROM_B_START + PTABLE_BOOTROM_B_LEN)
    
/* vxWorks MCore */
#define PTABLE_VXWORKS_MCORE_START       PTABLE_BOOTROM_B_END
#define PTABLE_VXWORKS_MCORE_LEN         (PRODUCT_CFG_FLASH_VXWORKS_MCORE_LEN)
#define PTABLE_VXWORKS_MCORE_END         (PTABLE_VXWORKS_MCORE_START + PTABLE_VXWORKS_MCORE_LEN)

/* logo */
#define PTABLE_LOGO_START                PTABLE_VXWORKS_MCORE_END
#define PTABLE_LOGO_LEN                  (PRODUCT_CFG_FLASH_LOGO_LEN)
#define PTABLE_LOGO_END                  (PTABLE_LOGO_START + PTABLE_LOGO_LEN)

/* YAFFS MCore */
#define PTABLE_YAFFS_MCORE_START         PTABLE_LOGO_END
#define PTABLE_YAFFS_MCORE_LEN           (PRODUCT_CFG_FLASH_YAFFS_MCORE_LEN)
#define PTABLE_YAFFS_MCORE_END           (PTABLE_YAFFS_MCORE_START+PTABLE_YAFFS_MCORE_LEN)

/* Android Fastboot */
#define PTABLE_ANDROID_FASTBOOT_START    PTABLE_YAFFS_MCORE_END
#define PTABLE_ANDROID_FASTBOOT_LEN      (PRODUCT_CFG_FLASH_ANDROID_FASTBOOT_LEN)
#define PTABLE_ANDROID_FASTBOOT_END      (PTABLE_ANDROID_FASTBOOT_START + PTABLE_ANDROID_FASTBOOT_LEN)
#define PTABLE_ANDROID_FASTBOOT_LOADSIZE_LEN (PRODUCT_CFG_FLASH_ANDROID_FASTBOOT_LOADSIZE_LEN)

/* Android Bootimage */
#define PTABLE_ANDROID_BOOTIMAGE_START   PTABLE_ANDROID_FASTBOOT_END
#define PTABLE_ANDROID_BOOTIMAGE_LEN     (PRODUCT_CFG_FLASH_ANDROID_BOOTIMAGE_LEN)
#define PTABLE_ANDROID_BOOTIMAGE_END     (PTABLE_ANDROID_BOOTIMAGE_START + PTABLE_ANDROID_BOOTIMAGE_LEN)    
    
/* YAFFS ACore */
#define PTABLE_YAFFS_ACORE_START         PTABLE_ANDROID_BOOTIMAGE_END
#define PTABLE_YAFFS_ACORE_LEN           (PRODUCT_CFG_FLASH_YAFFS_ACORE_LEN)
#define PTABLE_YAFFS_ACORE_END           (PTABLE_YAFFS_ACORE_START+PTABLE_YAFFS_ACORE_LEN)

/* YAFFS USER CODE */
#define PTABLE_YAFFS_APP_START          PTABLE_YAFFS_ACORE_END
#define PTABLE_YAFFS_APP_LEN            (PRODUCT_CFG_FLASH_YAFFS_APP_LEN)
#define PTABLE_YAFFS_APP_END            (PTABLE_YAFFS_APP_START+PTABLE_YAFFS_APP_LEN)

/* YAFFS OEM */
#define PTABLE_YAFFS_OEM_START           PTABLE_YAFFS_APP_END
#define PTABLE_YAFFS_OEM_LEN             (PRODUCT_CFG_FLASH_YAFFS_OEM_LEN)
#define PTABLE_YAFFS_OEM_END             (PTABLE_YAFFS_OEM_START+PTABLE_YAFFS_OEM_LEN)

/* YAFFS USER DATA */
#define PTABLE_YAFFS_USERDATA_START      PTABLE_YAFFS_OEM_END
#define PTABLE_YAFFS_USERDATA_LEN        (PRODUCT_CFG_FLASH_YAFFS_USERDATA_LEN)
#define PTABLE_YAFFS_USERDATA_END        (PTABLE_YAFFS_USERDATA_START+PTABLE_YAFFS_USERDATA_LEN)

/* YAFFS WEBUI */
//#define PTABLE_YAFFS_WEBUI_START          PTABLE_YAFFS_USERDATA_END
//#define PTABLE_YAFFS_WEBUI_LEN            (PRODUCT_CFG_FLASH_YAFFS_WEBUI_LEN)
//#define PTABLE_YAFFS_WEBUI_END            (PTABLE_YAFFS_WEBUI_START+PTABLE_YAFFS_WEBUI_LEN)

/* YAFFS ONLINE */
//#define PTABLE_YAFFS_ONLINE_START        PTABLE_YAFFS_WEBUI_END
#define PTABLE_YAFFS_ONLINE_START        PTABLE_YAFFS_USERDATA_END
#define PTABLE_YAFFS_ONLINE_LEN          (PRODUCT_CFG_FLASH_YAFFS_ONLINE_LEN)
#define PTABLE_YAFFS_ONLINE_END          (PTABLE_YAFFS_ONLINE_START + PTABLE_YAFFS_ONLINE_LEN)

/* ISO  */
#define PTABLE_ISO_START                (PTABLE_YAFFS_ONLINE_END)
#define PTABLE_ISO_LEN                  PRODUCT_CFG_FLASH_ISO_LEN
#define PTABLE_ISO_END                  (PTABLE_ISO_START + PTABLE_ISO_LEN)

/*YAFFS WEBUI*/
#define PTABLE_YAFFS_WEBUI_START PTABLE_ISO_END
#define PTABLE_YAFFS_WEBUI_LEN (PRODUCT_CFG_FLASH_YAFFS_WEBUI_LEN)
#define PTABLE_YAFFS_END    (PTABLE_YAFFS_WEBUI_START + PTABLE_YAFFS_WEBUI_LEN)

#endif


/***********************************************************************
 * RAM空间分配 
 ***********************************************************************/
/*-------------------------------------------------------------------------------
                            共享内存,V7R1
  -------------------------------------------------------------------------------
                          ----------------------- 0x38400000
                         |  SYNC(0x60)           |
                          ----------------------- 0x383FFFA0
                         |  MEM spinlock(0x20)   |
                          ----------------------- 0x383FFF80
                         |  ICC(0x80)            |
                          ----------------------- 0x383FFF00
                         |  MULT BAND(0x8000)   |
                          ----------------------- 0x383F7F00
                         |  IPC(0x20)           |
                          ----------------------- 0x383F7EE0
                         |  WAN(0x5000)         |
                          ----------------------- 0x383F2EE0
                         |  MEM MGR M            |
                          ----------------------- 0x38000000
-------------------------------------------------------------------------------*/
/* 从顶往下分配，以下地址和大小不可改变 */
#define MEMORY_RAM_CORESHARE_ADDR                   CORESHARE_MEM_ADDR
#define MEMORY_RAM_CORESHARE_SIZE                   CORESHARE_MEM_SIZE
#define MEMORY_RAM_CORESHARE_TOP_ADDR               CORESHARE_MEM_TOP_ADDR

#define MEMORY_RAM_CORESHARE_SYNC_RESV              CORESHARE_MEM_SYNC_SIZE
#define MEMORY_RAM_CORESHARE_SYNC_ADDR             	CORESHARE_MEM_SYNC_ADDR
    
#define MEMORY_RAM_CORESHARE_MEM_RESV               CORESHARE_MEM_SPINLOCK_SIZE
#define MEMORY_RAM_CORESHARE_MEM_ADDR            	CORESHARE_MEM_SPINLOCK_ADDR

#define MEMORY_RAM_CORESHARE_ICC_RESV               CORESHARE_MEM_ICC_SIZE
#define MEMORY_RAM_CORESHARE_ICC_ADDR              	CORESHARE_MEM_ICC_ADDR

#define MEMORY_RAM_CORESHARE_TENCILICA_MULT_BAND_RESV CORESHARE_MEM_TENCILICA_MULT_BAND_SIZE
#define MEMORY_RAM_CORESHARE_TENCILICA_MULT_BAND_ADDR CORESHARE_MEM_TENCILICA_MULT_BAND_ADDR

/* 以下地址和大小可以动态改变 */
#define MEMORY_RAM_CORESHARE_IPC_RESV               CORESHARE_MEM_IPC_SIZE
#define MEMORY_RAM_CORESHARE_IPC_ADDR              	CORESHARE_MEM_IPC_ADDR

/* 硬件信息保留区，根据HKADC动态填充 */
#define MEMORY_RAM_HW_INFO_SIZE             CORESHARE_MEM_HW_INFO_SIZE
#define MEMORY_RAM_HW_INFO_ADDR             CORESHARE_MEM_HW_INFO_ADDR

#define MEMORY_RAM_CORESHARE_WAN_RESV               CORESHARE_MEM_WAN_SIZE
#define MEMORY_RAM_CORESHARE_WAN_ADDR              	CORESHARE_MEM_WAN_ADDR

#define MEMORY_RAM_CORESHARE_MEMMGR_ADDR           	CORESHARE_MEM_MEMMGR_ADDR
#define MEMORY_RAM_CORESHARE_MEMMGR_SIZE            CORESHARE_MEM_MEMMGR_SIZE


/*-------------------------------------------------------------------------------
                            非共享内存
  -------------------------------------------------------------------------------                            
  MCORE BSP RESV BASE:0xC7EFA000
  SCORE BSP RESV BASE:0xCBFBA000
                         ------------------- 0xC7EFA1A0
                        |  TRACE(256)       |
                         ------------------- 0xC7EFA0A0
                        |  BOOT VER(160)    |
                        -------------------- 0xC7EFA000
-------------------------------------------------------------------------------*/
/* 需要和C核RAM_HIGH_ADRS保持一致*/
#define MEMORY_RAM_BSP_RESV_ADDR            GLOBAL_MEM_MCORE_USER_RESERVED_ADDR

/* bootloader版本号 */
#define MEMORY_RAM_BOOTLOAD_VER_ADDR        (MEMORY_RAM_BSP_RESV_ADDR + 0x0)
#define MEMORY_RAM_BOOTLOAD_VER_LEN         (128 + 32)

/* trace版本号 */
#define MEMORY_RAM_TRACE_ADDR               (MEMORY_RAM_BOOTLOAD_VER_ADDR + MEMORY_RAM_BOOTLOAD_VER_LEN)
#define MEMORY_RAM_TRACE_SIZE               (4*256)

/*可维可测空间*/
#define MEMORY_RAM_DEBUG_ADDR              (MEMORY_RAM_BSP_RESV_ADDR + 0x1000)	//和A核统一,page对齐
#define MEMORY_RAM_DEBUG_SIZE               (0x100000)

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
    && (defined (BOARD_SFT)||defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
/***********************************************************************
 | V7R1 VxWorks| AUTOINSTALL和安全标记部分不能移动和更改
 -----------------------------------------------------------------------
                    AXI空间分配
             --------------------------- 0x30000000
            |  安全标记(32)        |
             --------------------------- 0x2FFFFFE0
            |  AUTOINSTALL标记(256)  |
             --------------------------- 0x2FFFFFE0
            |  DICC(32)              |
             --------------------------- 0x2FFFFEC0
            |  动态分配内存     |
             --------------------------- 0x2FFCD2A0
            |  MEMMGR(1024)            |
             --------------------------- 0x2FFCCEA0
            |  DRX(10K)                 |
             --------------------------- 0x2FFCA6A0
            |  IPF(30K )                |
             --------------------------- 0x2FFC2EA0
            |  DSP(10K )                |
             --------------------------- 0x2FFC06A0
            |  ICC(256)                 |
             --------------------------- 0x2FFC02A0
            |  MNTN(128)                |
             --------------------------- 0x2FFC0240
            |  IFC(32)                  |
             --------------------------- 0x2FFC0220
            |  FLASH互斥(32)          |
             --------------------------- 0x2FFC0200
            |  异常向量表(512)     |
             --------------------------- 0x2FFC0000
***********************************************************************/
#define MEMORY_AXI_EXEC_VEC_ADDR            (AXI_MEM_ADDR)
#define MEMORY_AXI_EXEC_VEC_SIZE            (512)

#define MEMORY_AXI_FLASH_SEM_ADDR           (MEMORY_AXI_EXEC_VEC_ADDR + MEMORY_AXI_EXEC_VEC_SIZE)
#define MEMORY_AXI_FLASH_SEM_SIZE           (32)

#define MEMORY_AXI_IFC_ADDR                 (MEMORY_AXI_FLASH_SEM_ADDR + MEMORY_AXI_FLASH_SEM_SIZE)
#define MEMORY_AXI_IFC_SIZE                 (32)

/*可维可测空间*/
#define MEMORY_AXI_MNTN_ADDR                (MEMORY_AXI_IFC_ADDR + MEMORY_AXI_IFC_SIZE)
#define MEMORY_AXI_MNTN_SIZE                (128)

#define MEMORY_AXI_ICC_ADDR                 (MEMORY_AXI_MNTN_ADDR + MEMORY_AXI_MNTN_SIZE)
#define MEMORY_AXI_ICC_SIZE                 (1024)

#define MEMORY_AXI_DSP_ADDR                 (MEMORY_AXI_ICC_ADDR + MEMORY_AXI_ICC_SIZE)
#define MEMORY_AXI_DSP_SIZE                 (10*1024)

#define MEMORY_AXI_IPF_ADDR                  (MEMORY_AXI_DSP_ADDR + MEMORY_AXI_DSP_SIZE)
#define MEMORY_AXI_IPF_SIZE                   (30 * 1024)

#define MEMORY_AXI_ICC_OM_ADDR              (MEMORY_AXI_IPF_ADDR + MEMORY_AXI_IPF_SIZE)
#define MEMORY_AXI_ICC_OM_SIZE              (4*1024)

#define MEMORY_AXI_DRXM_ADDR                (AXI_MEM_ADDR + 0x10000)// (MEMORY_AXI_CIPHER_ADDR + MEMORY_AXI_CIPHER_SIZE)
#define MEMORY_AXI_DRXM_SIZE                 (30*1024)

#define MEMORY_AXI_EXC_ASHELL_ADDR          (MEMORY_AXI_DRXM_ADDR + MEMORY_AXI_DRXM_SIZE)
#define MEMORY_AXI_EXC_ASHELL_SIZE          (16*1024)

#define MEMORY_AXI_EXC_CSHELL_ADDR          (MEMORY_AXI_EXC_ASHELL_ADDR + MEMORY_AXI_EXC_ASHELL_SIZE)
#define MEMORY_AXI_EXC_CSHELL_SIZE          (16*1024)
#define MEMORY_AXI_EXC_ADDR                 (MEMORY_AXI_EXC_CSHELL_ADDR + MEMORY_AXI_EXC_CSHELL_SIZE)
#define MEMORY_AXI_EXC_SIZE                 (1*1024)
#define MEMORY_AXI_DDM_LOAD_ADDR          (MEMORY_AXI_EXC_ADDR + MEMORY_AXI_EXC_SIZE)
#define MEMORY_AXI_DDM_LOAD_SIZE          (1*1024)

#define MEMORY_AXI_DRXA_ADDR                (AXI_MEM_ADDR + 0x20000)// (MEMORY_AXI_CIPHER_ADDR + MEMORY_AXI_CIPHER_SIZE)
#define MEMORY_AXI_DRXA_SIZE                 (30*1024)

#define MEMORY_AXI_MEMMGR_FLAG_ADDR         (MEMORY_AXI_DRXA_ADDR + MEMORY_AXI_DRXA_SIZE)
#define MEMORY_AXI_MEMMGR_FLAG_SIZE         (1024)

#define MEMORY_AXI_DYNAMIC_SEC_ADDR         (MEMORY_AXI_MEMMGR_FLAG_ADDR + MEMORY_AXI_MEMMGR_FLAG_SIZE)
#define MEMORY_AXI_DYNAMIC_SEC_SIZE         (MEMORY_AXI_AF10_ADDR - MEMORY_AXI_DYNAMIC_SEC_ADDR)

/* AF10是否在位标志 */
#define MEMORY_AXI_AF10_ADDR                (MEMORY_AXI_DspPower_ADDR - MEMORY_AXI_AF10_SIZE)
#define MEMORY_AXI_AF10_SIZE                  (4)

#define MEMORY_AXI_DspPower_ADDR          (MEMORY_AXI_SLEEP_STUB_ADDRM - MEMORY_AXI_DspPower_SIZE)
#define MEMORY_AXI_DspPower_SIZE          (256)

#define MEMORY_AXI_SLEEP_STUB_ADDRM          (MEMORY_AXI_WIFI_HSIC_PERI_ADDR - MEMORY_AXI_SLEEP_STUB_SIZEM)
#define MEMORY_AXI_SLEEP_STUB_SIZEM          (768)

/* WIFI，HSIC SLAVE外设掉电标记 */
#define MEMORY_AXI_WIFI_HSIC_PERI_ADDR      (MEMORY_AXI_PERI_DOWN_FLAG_ADDR - MEMORY_AXI_WIFI_HSIC_PERI_SIZE)
#define MEMORY_AXI_WIFI_HSIC_PERI_SIZE     (4) 

/* 外设掉电标记 */
#define MEMORY_AXI_PERI_DOWN_FLAG_ADDR      (MEMORY_AXI_DSP_PS_ADDR - MEMORY_AXI_PERI_DOWN_FLAG_SIZE)
#define MEMORY_AXI_PERI_DOWN_FLAG_SIZE     (4) 

#define MEMORY_AXI_DSP_PS_ADDR         (MEMORY_AXI_DSP_MSP_ADDR - MEMORY_AXI_DSP_PS_SIZE)
#define MEMORY_AXI_DSP_PS_SIZE         (4)

#define MEMORY_AXI_DSP_MSP_ADDR         (MEMORY_AXI_CHARGE_ADDR - MEMORY_AXI_DSP_MSP_SIZE)
#define MEMORY_AXI_DSP_MSP_SIZE         (4)

/* 充放电模块共享电压等信息用,前四个字节存放电压值，后面暂未使用 */
#define MEMORY_AXI_CHARGE_ADDR                (MEMORY_AXI_SD_UPGRADE_ADDR - MEMORY_AXI_CHARGE_SIZE)
#define MEMORY_AXI_CHARGE_SIZE                (32) 

#define MEMORY_AXI_SD_UPGRADE_ADDR          (MEMORY_AXI_USB_INOUT_ADDR - MEMORY_AXI_SD_UPGRADE_SIZE) /*0x2FFFFE40*/
#define MEMORY_AXI_SD_UPGRADE_SIZE          (32)

/* 以下定义需要掉电非易失,不能被覆盖,不能移动位置 */
#define MEMORY_AXI_USB_INOUT_ADDR			(MEMORY_AXI_USB_CSHELL_ADDR - MEMORY_AXI_USB_INOUT_SIZE)
#define MEMORY_AXI_USB_INOUT_SIZE 			(4)

/* cshell axi-memory */
#define MEMORY_AXI_USB_CSHELL_ADDR          (MEMORY_AXI_TEMP_PROTECT_ADDR - MEMORY_AXI_USB_CSHELL_SIZE)
#define MEMORY_AXI_USB_CSHELL_SIZE          (4)

/* 温保特性OM与GUL DSP共享地址，用于传递温度值，需要与memMapGlobal.h保持一致，该地址不能被修改*/
#define MEMORY_AXI_TEMP_PROTECT_ADDR        (MEMORY_AXI_HIFI_ADDR - MEMORY_AXI_TEMP_PROTECT_SIZE) /*0x2FFFFE40*/
#define MEMORY_AXI_TEMP_PROTECT_SIZE        (128)

#define MEMORY_AXI_HIFI_ADDR                (MEMORY_AXI_PRT_FLAG_ADDR - MEMORY_AXI_HIFI_SIZE)
#define MEMORY_AXI_HIFI_SIZE 				(6*1024)

#define MEMORY_AXI_PRT_FLAG_ADDR            (MEMORY_AXI_PTABLE_ADDR - MEMORY_AXI_PRT_FLAG_SIZE)
#define MEMORY_AXI_PRT_FLAG_SIZE             4

#define MEMORY_AXI_PTABLE_ADDR				(MEMORY_AXI_HW_ID_ADDR - MEMORY_AXI_PTABLE_SIZE) /*0x2FFFFEC0*/
#define MEMORY_AXI_PTABLE_SIZE				(1024)

#define MEMORY_AXI_HW_ID_ADDR               (MEMORY_AXI_ONOFF_ADDR - MEMORY_AXI_HW_ID_SIZE)
#define MEMORY_AXI_HW_ID_SIZE               4

#define MEMORY_AXI_ONOFF_ADDR               (MEMORY_AXI_DICC_ADDR - MEMORY_AXI_ONOFF_SIZE)
#define MEMORY_AXI_ONOFF_SIZE               (32)

/* GU使用的DICC特性地址，需要与memMapGlobal.h保持一致，该地址不能被修改*/
#define MEMORY_AXI_DICC_ADDR                (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR - MEMORY_AXI_DICC_SIZE) /*0x2FFFFEC0*/
#define MEMORY_AXI_DICC_SIZE                (32)

#define MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR   (MEMORY_AXI_DLOAD_ADDR - MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE)
#if defined (CHIP_BB_6920ES)
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (160)
#else
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (156)
#endif

#define MEMORY_AXI_DLOAD_ADDR               (MEMORY_AXI_SEC_SHARE_ADDR - MEMORY_AXI_DLOAD_SIZE)
#define MEMORY_AXI_DLOAD_SIZE               (96)

/* 安全共享数据区放在AXI Memory的顶部位置,VxWorks启动后需要在其他人使用该区域前拷贝走 */
#define MEMORY_AXI_SEC_SHARE_ADDR           (AXI_MEM_ADDR + AXI_MEM_SIZE - MEMORY_AXI_SEC_SHARE_SIZE)
#if defined (CHIP_BB_6920ES)
#define MEMORY_AXI_SEC_SHARE_SIZE           32
#else
#define MEMORY_AXI_SEC_SHARE_SIZE           36
#endif
#define OCR_INITED_FLAG_ADDR                (AXI_MEM_ADDR + AXI_MEM_SIZE - 4)
#define BL_INITED_FLAG_VALUE                0x98765432  /* BootLoader运行过，用于先贴后烧bootrom全擦除 nand*/

#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
       && defined (BOARD_FPGA_P500))

/***********************************************************************
 | P500 |, 升级自动安装部分不能移动,否则不能进入正常升级
 -----------------------------------------------------------------------
                    AXI空间分配
             --------------------------- 0x30020000
            |  动态分配内存      |
             --------------------------- 0x3000D3E0
            |  DICC      |
             --------------------------- 0x3000D3C0
            |  内存管理相关变量(1024)   |
             --------------------------- 0x3000CFC0
            |  DRX(10K)                 |
             --------------------------- 0x3000a7c0
            |  IPF(30K)                   |
             --------------------------- 0x30002fc0
            |  DSP(10K )                |
             --------------------------- 0x300007c0
            |  ICC(1024)                 |
             --------------------------- 0x300003C0
            |  升级/自动安装(256) |
             --------------------------- 0x300002C0
            |  MNTN(128)                  |
             --------------------------- 0x30000240
            |  IFC(32)                  |
             --------------------------- 0x30000220
            |  FLASH互斥(32)            |
             --------------------------- 0x30000200
            |  异常向量表(512)          |
             --------------------------- 0x30000000
***********************************************************************/
#define MEMORY_AXI_EXEC_VEC_ADDR            (AXI_MEM_ADDR)
#define MEMORY_AXI_EXEC_VEC_SIZE            (512)

#define MEMORY_AXI_FLASH_SEM_ADDR           (MEMORY_AXI_EXEC_VEC_ADDR + MEMORY_AXI_EXEC_VEC_SIZE)
#define MEMORY_AXI_FLASH_SEM_SIZE           (32)

#define MEMORY_AXI_IFC_ADDR                 (MEMORY_AXI_FLASH_SEM_ADDR + MEMORY_AXI_FLASH_SEM_SIZE)
#define MEMORY_AXI_IFC_SIZE                 (32)

/*可维可测空间*/
#define MEMORY_AXI_MNTN_ADDR                (MEMORY_AXI_IFC_ADDR + MEMORY_AXI_IFC_SIZE)
#define MEMORY_AXI_MNTN_SIZE                (128)

#define MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR   (MEMORY_AXI_MNTN_ADDR + MEMORY_AXI_MNTN_SIZE)
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (256)

#define MEMORY_AXI_ICC_ADDR                 (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR + MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE)
#define MEMORY_AXI_ICC_SIZE                 (1024)

#define MEMORY_AXI_DSP_ADDR                 (MEMORY_AXI_ICC_ADDR + MEMORY_AXI_ICC_SIZE)
#define MEMORY_AXI_DSP_SIZE                 (10*1024)

#define MEMORY_AXI_IPF_ADDR                  (MEMORY_AXI_DSP_ADDR + MEMORY_AXI_DSP_SIZE)
#define MEMORY_AXI_IPF_SIZE                   (30 * 1024)

#define MEMORY_AXI_DRX_ADDR                 (MEMORY_AXI_IPF_ADDR + MEMORY_AXI_IPF_SIZE)
#define MEMORY_AXI_DRX_SIZE                 (10*1024)

#define MEMORY_AXI_MEMMGR_FLAG_ADDR         (MEMORY_AXI_DRX_ADDR + MEMORY_AXI_DRX_SIZE)
#define MEMORY_AXI_MEMMGR_FLAG_SIZE         (1024)

/* GU使用的DICC特性地址，需要与memMapGlobal.h保持一致，该地址不能被修改*/
#define MEMORY_AXI_DICC_ADDR               (MEMORY_AXI_MEMMGR_FLAG_ADDR + MEMORY_AXI_MEMMGR_FLAG_SIZE)/* 0x3000D3C0*/
#define MEMORY_AXI_DICC_SIZE               (32)

/* 温保特性OM与GUL DSP共享地址，用于传递温度值，需要与memMapGlobal.h保持一致，该地址不能被修改*/
#define MEMORY_AXI_TEMP_PROTECT_ADDR         (MEMORY_AXI_DICC_ADDR + MEMORY_AXI_DICC_SIZE) /* 0x3000D3E0 */
#define MEMORY_AXI_TEMP_PROTECT_SIZE         (128)

#define MEMORY_AXI_SD_UPGRADE_ADDR          (MEMORY_AXI_TEMP_PROTECT_ADDR + MEMORY_AXI_TEMP_PROTECT_SIZE)
#define MEMORY_AXI_SD_UPGRADE_SIZE          (32)

#define MEMORY_AXI_DYNAMIC_SEC_ADDR         (MEMORY_AXI_SD_UPGRADE_ADDR + MEMORY_AXI_SD_UPGRADE_SIZE)
#define MEMORY_AXI_DYNAMIC_SEC_SIZE         (MEMORY_AXI_HW_ID_ADDR - MEMORY_AXI_DYNAMIC_SEC_ADDR)


#define MEMORY_AXI_HW_ID_ADDR               (MEMORY_AXI_PTABLE_ADDR - MEMORY_AXI_HW_ID_SIZE)
#define MEMORY_AXI_HW_ID_SIZE               4

#define MEMORY_AXI_PTABLE_ADDR				(AXI_MEM_ADDR + AXI_MEM_SIZE - MEMORY_AXI_PTABLE_SIZE)
#define MEMORY_AXI_PTABLE_SIZE				(1024)

#else
    #error "unknown product macro"
#endif

#ifdef __cplusplus
}
#endif

#endif
