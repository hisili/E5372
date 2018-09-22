#include <generated/FeatureConfig.h>   /*syb*/

/** Multiple synopsys instances  */
#define DWC_SYNOP_CORE_OTG          1
#define DWC_SYNOP_CORE_HSIC         2

#if defined(BOARD_ASIC) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
#define REG_BASE_USB2DVC 0x900C0000
#define REG_USB2DVC_IOSIZE PAGE_ALIGN(0x40000)
#else
#define REG_BASE_USB2DVC 0x82080000
#define REG_USB2DVC_IOSIZE PAGE_ALIGN(0x40000)
#endif

#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
#define REG_BASE_USBHSIC    0x90080000
#define REG_USBHSIC_IOSIZE PAGE_ALIGN(0x20000)
#endif

/*syb */
#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_TFUP_STATUS_FLAG_ADDR       (MEMORY_AXI_SD_UPGRADE_ADDR + 0x00)
#else
#define TCM_FOR_TFUP_STATUS_FLAG_ADDR       (ECS_DTCM_BASE_ADDR + 0x20)
#endif
#define TCM_FOR_TFUP_STATUS_RUNNING_VAL     (0x12348765)
#define TCM_FOR_TFUP_STATUS_DONE_VAL        (0x87651234)

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_TFUP_BOOTROM_FLAG_ADDR      (MEMORY_AXI_SD_UPGRADE_ADDR + 0x04)
#else
#define TCM_FOR_TFUP_BOOTROM_FLAG_ADDR      (ECS_DTCM_BASE_ADDR + 0x24)
#endif
#define TCM_FOR_TFUP_BOOTROM_FLAG_VAL       0x87651234
/*syb end*/
#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_WEBUI_STATUS_FLAG_ADDR  (MEMORY_AXI_SD_UPGRADE_ADDR + 0x0C)
#else
#define TCM_FOR_WEBUI_STATUS_FLAG_ADDR  (ECS_DTCM_BASE_ADDR + 0x80)
#endif
#define TCM_FOR_WEBUI_STATUS_FLAG_VAL    (0x12348765)
#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_UPDATE_NVBACKUP_ADDR    (MEMORY_AXI_SD_UPGRADE_ADDR + 0x18)
#else
#define TCM_FOR_UPDATE_NVBACKUP_ADDR    (ECS_DTCM_BASE_ADDR + 0x94)
#endif
#define UPDATE_NVBACKUP_FLAG    (*((unsigned long*)TCM_FOR_UPDATE_NVBACKUP_ADDR))
#define UPDATE_NVBACKUP_MODE    0xAA5555AA
#define UPDATE_NO_NVBACKUP_MODE    0x0

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_SD_FORCE_UPDATE (MEMORY_AXI_SD_UPGRADE_ADDR + 0x10)
#else
#define TCM_FOR_SD_FORCE_UPDATE (ECS_DTCM_BASE_ADDR + 0xB4)
#endif
#define SD_FORCE_UPDATE_FLAG  (*((volatile UINT32*)TCM_FOR_SD_FORCE_UPDATE))
#define SD_FORCE_UPDATE_VALUE 0xAA5555AA

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_SD_FIRMWARE_JUDGE (MEMORY_AXI_SD_UPGRADE_ADDR + 0x14)
#else
#define TCM_FOR_SD_FIRMWARE_JUDGE (ECS_DTCM_BASE_ADDR + 0xB8)
#endif
#define SD_UPDATE_FIRMWARE_FLAG  (*((volatile UINT32*)TCM_FOR_SD_FIRMWARE_JUDGE))
#define SD_UPDATE_FIRMWARE_FLAG_VALUE 0xAA5555AA

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
#define TCM_FOR_SD_UPDATE_RESULT_FLAG (MEMORY_AXI_SD_UPGRADE_ADDR + 0x08)
#else
#define TCM_FOR_SD_UPDATE_RESULT_FLAG (ECS_DTCM_BASE_ADDR + 0xBC)
#endif
#define SD_UPDATE_RESULT_FLAG  (*((volatile UINT32*)TCM_FOR_SD_UPDATE_RESULT_FLAG))
#define SD_UPDATE_RESULT_FLAG_VALUE 0xAA5555AA

#define TCM_FOR_SDUPDATE_GOON_FLAG          (MEMORY_AXI_DLOAD_ADDR + 0x04)
#define TCM_FOR_ONLINE_GOON_FLAG            (MEMORY_AXI_DLOAD_ADDR + 0x04)
#define SDUPDATE_GOON_FLAG                  (*((volatile UINT32*)TCM_FOR_SDUPDATE_GOON_FLAG))
#define SDUPDATE_GOON_MAGIC                 0x5A5A55AA
#define ONLINE_GOON_FLAG                    (*((volatile UINT32*)TCM_FOR_ONLINE_GOON_FLAG))
#define ONLINE_GOON_MAGIC                   0xA5A5AA55

#define UPGRADE_MIDRESTART_FLAG             (MEMORY_AXI_DLOAD_ADDR + 0x08)
#define UPGRADE_MIDRESTART_MAGIC            0x5A5A1122

/*32 bits register operation*/

#define REG_READ_32(reg,result)	    ((result) = *(volatile UINT32 *)(reg))
#define REG_WRITE_32(reg,data)       (*((volatile UINT32 *)(reg)) = (data))

/*16 bits register operation*/
#define REG_READ_16(reg,result)	    ((result) = *(volatile UINT16 *)(reg))
#define REG_WRITE_16(reg,data)       (*((volatile UINT16 *)(reg)) = (data))

/*8 bits register operation*/
#define REG_READ_8(reg,result)	    ((result) = *(volatile UINT8 *)(reg))
#define REG_WRITE_8(reg,data)         (*((volatile UINT8 *)(reg)) = (data))
