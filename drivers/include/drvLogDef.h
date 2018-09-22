/******************************************************************
* Copyright (C), 2005-2007, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: drvLogdef.h                                             *
*                                                                 *
* Description:                                                    *
*     driver log related functions                                *
*                                                                 *
* Author:  g57837                                                 *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*   1.  Volume Control functions list                             *
*   2.  Setting functions                                         *
*        setDrvLogFunc                                            *
*                                                                 *
* Date:    2008-01-28                                             *
*                                                                 *
* History:                                                        *
*                                                                 *
*******************************************************************/

#ifndef __DRVLOGF_H__
#define __DRVLOGF_H__


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* 空函数声明 */ 
extern int  OSAL_LogMsg (char *fmt, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);

#define __LOG_RELEASE__          /* 打点方式 */
typedef void  (*VOIDPTR) (int, int, ...);/* pfunction returning void */



#define     LOG_MIN_MODULE_ID_DRV       0xC9
#define     LOG_MAX_MODULE_ID_DRV       0xFA

#define     LOG_MIN_FILE_ID_DRV         0x0800
#define     LOG_MAX_FILE_ID_DRV         0x0FFF

#define DrvLogId( FILEID, LINE ) \
			( ( ( FILEID & 0x0000FFFF ) << 16 ) | ( LINE & 0x0000FFFF ) )

/* 打印级别定义 */
typedef enum
{
	LOG_LEVEL_OFF = 0,   /* 关闭打印     */
	LOG_LEVEL_ERROR,     /* Error级别    */
	LOG_LEVEL_WARNING,   /* Warning级别  */
	LOG_LEVEL_NORMAL,    /* Normal级别   */
	LOG_LEVEL_INFO,      /* Info级别     */
	LOG_LEVEL_BUTT
} LOG_LEVEL_EN;

/* Driver模块ID号,共31个 */
#if 0
typedef enum
{
	DRV_MODULE_ID_AMTS = LOG_MIN_MODULE_ID_DRV,
	DRV_MODULE_ID_AUDIO,
	DRV_MODULE_ID_BSP,
	DRV_MODULE_ID_CAMERA,
	DRV_MODULE_ID_DM,
	DRV_MODULE_ID_DMA,
	DRV_MODULE_ID_DSPC,
	DRV_MODULE_ID_END,
	DRV_MODULE_ID_FDI,
	DRV_MODULE_ID_FLASH,
	DRV_MODULE_ID_FM,
	DRV_MODULE_ID_GPIO,
	DRV_MODULE_ID_HICONTRL,
	DRV_MODULE_ID_HKADC,
	DRV_MODULE_ID_I2C,
	DRV_MODULE_ID_KEYPAD,
	DRV_MODULE_ID_LCD,
	DRV_MODULE_ID_LOGGER,
	/*DRV_MODULE_ID_MNTN,*/
	DRV_MODULE_ID_PM,
	DRV_MODULE_ID_RTC,
	DRV_MODULE_ID_SCI,
	DRV_MODULE_ID_SDMMC,
	DRV_MODULE_ID_SPI,
	DRV_MODULE_ID_USB,
	DRV_MODULE_ID_WDT,
       DRV_MODULE_ID_NVIM,
       DRV_MODULE_ID_SYSCTRL,
       DRV_MODULE_ID_MNTN=0x801c,
       DRV_MODULE_ID_TIMER,
       DRV_MODULE_ID_VIC,
       DRV_MODULE_ID_YAFFS,
    DRV_MODULE_ID_SDIO,
    DRV_MODULE_ID_WIFIEND,
    DRV_MODULE_ID_WIFIMVL,
    DRV_MODULE_ID_OLED,
    DRV_MODULE_ID_ONOFF,
    
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    DRV_MODULE_ID_ICC = 0x8029,
    #endif

}LOG_MODULE_ID_EN;
#endif
/* DRV源文件ID号，共192个 */
typedef enum
{
        DRV_FILE_ID_AMBADMTIMERDRV_C = LOG_MIN_FILE_ID_DRV,
        DRV_FILE_ID_AMBAINTRCTL_C,
        DRV_FILE_ID_AMBASIO_C,
        DRV_FILE_ID_AMBASYSCTRL_C,
        DRV_FILE_ID_AMBATIMER_C,
        DRV_FILE_ID_AMBAWDT_C,
        DRV_FILE_ID_AMTS_C,
        DRV_FILE_ID_BOOTCONFIG_C,
        DRV_FILE_ID_BOOTINIT_C,
        DRV_FILE_ID_CONTRLDRV_C,
        DRV_FILE_ID_DBG_C,
        DRV_FILE_ID_DEVICE_C,
        DRV_FILE_ID_DIRLIB_C,
        DRV_FILE_ID_DMA_C,
        DRV_FILE_ID_DMAPI_C,
        DRV_FILE_ID_DMCORE_C,
        DRV_FILE_ID_DMDRV_C,
        DRV_FILE_ID_DMERTCDRV_C,
        DRV_FILE_ID_DMHKADCDRV_C,
        DRV_FILE_ID_DMIRTCDRV_C,
        DRV_FILE_ID_DMKEYPADDRV_C,
        DRV_FILE_ID_DMLIB_C,
        DRV_FILE_ID_DMLOGCTRL_C,
        DRV_FILE_ID_DMSPY_C,
        DRV_FILE_ID_DMSPY2_C,
        DRV_FILE_ID_DOSFSFAT_C,
        DRV_FILE_ID_DOSVDIRLIB_C,
        DRV_FILE_ID_DRVFLASH_C,
        DRV_FILE_ID_DRVLOG_C,
        DRV_FILE_ID_DRVRTC_C,
        DRV_FILE_ID_DWLD_L18_C,
        DRV_FILE_ID_DWLD_M18_C,
        DRV_FILE_ID_ECSUSBDEVICEDL_C,
        DRV_FILE_ID_ECSUSBDEVICE_C,
        DRV_FILE_ID_ECSUSBDMADL_C,
        DRV_FILE_ID_ECSUSBDMA_C,
        DRV_FILE_ID_ECSUSBINITDL_C,
        DRV_FILE_ID_ECSUSBINIT_C,
        DRV_FILE_ID_ECSUSBNET_C,
        DRV_FILE_ID_ECSUSBTARGLIBDL_C,
        DRV_FILE_ID_ECSUSBTARGLIB_C,
        DRV_FILE_ID_ECSVERSION_C,
        DRV_FILE_ID_EDMREG_C,
        DRV_FILE_ID_EDRVNAMEPUBLIC_C,
        DRV_FILE_ID_EDRVNAME_C,
        DRV_FILE_ID_ENC28J60_C,
        DRV_FILE_ID_ENC28J60END_C,
        DRV_FILE_ID_EXCDRV_C,
        DRV_FILE_ID_FDIDRV_C,
        DRV_FILE_ID_FDI_ACCTEST_C,
        DRV_FILE_ID_FDI_FILE_TEST_C,
        DRV_FILE_ID_FDI_INIT_MOUNT_C,
        DRV_FILE_ID_FFS_HASHTBL_C,
        DRV_FILE_ID_FFS_MALLOC_C,
        DRV_FILE_ID_FI_RTOS_C,
        DRV_FILE_ID_FLASHDAVAPI_C,
        DRV_FILE_ID_GPIO_C,
        DRV_FILE_ID_GPIOINT_C,
        DRV_FILE_ID_HI6411SPI_C,
        DRV_FILE_ID_HI_SD_MMC_BUS_C,
        DRV_FILE_ID_HI_SD_MMC_HOST_C,
        DRV_FILE_ID_HI_SD_MMC_HS_C,
        DRV_FILE_ID_HKADCDRV_C,
        DRV_FILE_ID_I28F256L18FLASH_C,
        DRV_FILE_ID_I28F256M18FLASH_C,
        DRV_FILE_ID_I28F256MTD_C,
        DRV_FILE_ID_I2CDRV_C,
        DRV_FILE_ID_I2C_CORE_C,
        DRV_FILE_ID_IFM_API_C,
        DRV_FILE_ID_IFM_CMN_C,
        DRV_FILE_ID_IFM_MFM_C,
        DRV_FILE_ID_IFM_SYS_C,
        DRV_FILE_ID_IFM_VM_C,
        DRV_FILE_ID_INT_C,
        DRV_FILE_ID_INTTST_C,
        DRV_FILE_ID_IOLIB_C,
        DRV_FILE_ID_IOSLIB_C,
        DRV_FILE_ID_KEYPAD_C,
        DRV_FILE_ID_KPDGPIO_C,
        DRV_FILE_ID_LOGGER_C,
        DRV_FILE_ID_MAIN_C,
        DRV_FILE_ID_MFM_FM_API_C,
        DRV_FILE_ID_MFM_INT_DLT_C,
        DRV_FILE_ID_MFM_INT_FLT_C,
        DRV_FILE_ID_MFM_INT_FORMAT_C,
        DRV_FILE_ID_MFM_INT_OST_C,
        DRV_FILE_ID_MFM_INT_PBT_C,
        DRV_FILE_ID_MFM_INT_RBAPI_C,
        DRV_FILE_ID_MFM_INT_RBFSH_C,
        DRV_FILE_ID_MFM_INT_RBINT_C,
        DRV_FILE_ID_MFM_INT_RECLAIM_C,
        DRV_FILE_ID_MFM_INT_SCAN_C,
        DRV_FILE_ID_MFM_INT_SYSTEM_C,
        DRV_FILE_ID_MFM_LOW_FLASH_C,
        DRV_FILE_ID_MFM_MUSB_API_C,
        DRV_FILE_ID_MFM_MUSB_BOOT_C,
        DRV_FILE_ID_MFM_MUSB_CACHE_C,
        DRV_FILE_ID_MFM_MUSB_COMMON_C,
        DRV_FILE_ID_MFM_MUSB_DCF_C,
        DRV_FILE_ID_MFM_MUSB_DECF_C,
        DRV_FILE_ID_MFM_MUSB_ENTRY_C,
        DRV_FILE_ID_MFM_MUSB_EXT_C,
        DRV_FILE_ID_MFM_MUSB_FILE_C,
        DRV_FILE_ID_MFM_MUSB_FS_WRAP_C,
        DRV_FILE_ID_MFM_MUSB_INFO_C,
        DRV_FILE_ID_MFM_MUSB_LIST_C,
        DRV_FILE_ID_MFM_MUSB_PLR_C,
        DRV_FILE_ID_MFM_MUSB_REGROUP_C,
        DRV_FILE_ID_MFM_MUSB_SCANDIR_C,
        DRV_FILE_ID_MFM_MUSB_UCF_C,
        DRV_FILE_ID_MFM_MUSB_VFAT_C,
        DRV_FILE_ID_MNTNDRV_C,
        DRV_FILE_ID_MTD_CARR_C,
        DRV_FILE_ID_MTD_CFI_C,
        DRV_FILE_ID_MTD_CFIR_C,
        DRV_FILE_ID_MTD_GEN_C,
        DRV_FILE_ID_MTD_GENR_C,
        DRV_FILE_ID_MTD_INIT_C,
        DRV_FILE_ID_MTD_LIB_C,
        DRV_FILE_ID_MTD_PARR_C,
        DRV_FILE_ID_MTD_SIB_C,
        DRV_FILE_ID_MTD_SIBR_C,
        DRV_FILE_ID_MTD_STARR_C,
        DRV_FILE_ID_MUSB_DEMO_C,
        DRV_FILE_ID_NAND_C,
        DRV_FILE_ID_NAND512_C,
        DRV_FILE_ID_NAND2K_C,
        DRV_FILE_ID_OS_C,
        DRV_FILE_ID_OSSLIBDL_C,
        DRV_FILE_ID_OSSLIB_C,
        DRV_FILE_ID_OS_API_C,
        DRV_FILE_ID_PL131_C,
        DRV_FILE_ID_PMHI6402_C,
        DRV_FILE_ID_PMHI6431_C,
        DRV_FILE_ID_PMUDRV_C,
        DRV_FILE_ID_POS_API_C,
        DRV_FILE_ID_PWRINTERFACE_C,
        DRV_FILE_ID_RTC_C,
        DRV_FILE_ID_RTCBOTH_C,
        DRV_FILE_ID_RTCHI6402_C,
        DRV_FILE_ID_SCI_C,
        DRV_FILE_ID_SCIINTERFACE_C,
        DRV_FILE_ID_SCIRECORD_C,
        DRV_FILE_ID_SPIDRV_C,
        DRV_FILE_ID_STANDLIB_C,
        DRV_FILE_ID_SYSSERIAL_C,
        DRV_FILE_ID_SYSTFFS_C,
        DRV_FILE_ID_TEST_C,
        DRV_FILE_ID_TEST_BOOT_DIS_C,
        DRV_FILE_ID_TFFSCONFIG_C,
        DRV_FILE_ID_TIMER_C,
        DRV_FILE_ID_TIMERTST_C,
        DRV_FILE_ID_TT_ANALYSECALLSTACK_C,
        DRV_FILE_ID_USB4DOWNLOAD_C,
        DRV_FILE_ID_USBCDROM_C,
        DRV_FILE_ID_USBCOM_C,
        DRV_FILE_ID_USBDESCRCOPYLIBDL_C,
        DRV_FILE_ID_USBDESCRCOPYLIB_C,
        DRV_FILE_ID_USBDISK_C,
        DRV_FILE_ID_USBHANDLELIBDL_C,
        DRV_FILE_ID_USBHANDLELIB_C,
        DRV_FILE_ID_USBLISTLIBDL_C,
        DRV_FILE_ID_USBLISTLIB_C,
        DRV_FILE_ID_USBMODEMBULK_C,
        DRV_FILE_ID_USBMODEMINT_C,
        DRV_FILE_ID_USBNDISINT_C,
        DRV_FILE_ID_USBNDISBULK_C,
        DRV_FILE_ID_USBMUX_C,
        DRV_FILE_ID_USBSERIAL_C,
        DRV_FILE_ID_USBTARGLIBDL_C,
        DRV_FILE_ID_USBTARGLIB_C,
        DRV_FILE_ID_USBTCDLIBDL_C,
        DRV_FILE_ID_USBTCDLIB_C,
        DRV_FILE_ID_USBEXEC_C,
        DRV_FILE_ID_USIMDMINTF_C,
        DRV_FILE_ID_USIMSYSTEST_C,
        DRV_FILE_ID_USRAPPINIT_C,
        DRV_FILE_ID_USRCONFIG_C,
        DRV_FILE_ID_USRENTRY_C,
        DRV_FILE_ID_USRFSLIB_C,
        DRV_FILE_ID_UTILITY_FS_C,
        DRV_FILE_ID_VMBASELIB_C,
        DRV_FILE_ID_WDTDRV_C,
        DRV_FILE_ID_WDTTEST_C,
        DRV_FILE_ID_UARTCOM_C,
        DRV_FILE_ID_NVIMDRV_C,
        DRV_FILE_ID_YAFFSDRV_C,
        DRV_FILE_ID_USB_JOS_MEM_C,
        DRV_FILE_ID_USB_JOS_GENERAL_C,
        DRV_FILE_ID_USB_JOS_ENTRY_C,
        DRV_FILE_ID_USB_JOS_DRIVER_C,
        DRV_FILE_ID_USB_JOS_DEVICE_C,
        DRV_FILE_ID_USB_JOS_BUS_C,
        DRV_FILE_ID_USB_JINIT_C,
        DRV_FILE_ID_USB_TEST_PARM_C,
        DRV_FILE_ID_USB_JOS_SYNC_C,
        DRV_FILE_ID_USB_TEST_MULTI_EXEC_C,
        DRV_FILE_ID_USB_TEST_MAIN_C,
        DRV_FILE_ID_USB_TEST_DICT_C,
        DRV_FILE_ID_USB_UW_ARGS_C,
        DRV_FILE_ID_USB_NET_TEST_C,
        DRV_FILE_ID_USB_APP_INIT_C,
        DRV_FILE_ID_USB_SYNC_C,
        DRV_FILE_ID_USB_ENTRY_C,
        DRV_FILE_ID_USB_BUS_C,
        DRV_FILE_ID_USB_DCD_MUSBHSFC_C,
        DRV_FILE_ID_USB_JUSB_CORE_C,
        DRV_FILE_ID_USB_JUSB_CHP9_C,
        DRV_FILE_ID_USB_JSLAVE_INIT_C,
        DRV_FILE_ID_USB_TEST_SYNC_C,
        DRV_FILE_ID_USB_TEST_STDLIB_C,
        DRV_FILE_ID_USB_TEST_MEM_C,
        DRV_FILE_ID_USB_TEST_MULTI_C,
        DRV_FILE_ID_USB_FD_STORAGE_USB_C,
        DRV_FILE_ID_USB_FD_STORAGE_SCSI_C,
        DRV_FILE_ID_USB_NCM_FD_C,
        DRV_FILE_ID_USB_ECM_FD_C,
        DRV_FILE_ID_USB_RNDIS_FD_C,
        DRV_FILE_ID_USB_CDC_NET_LIB_C,
        DRV_FILE_ID_USB_CDC_FD_C,
        DRV_FILE_ID_USB_ACM_FD_C,
        DRV_FILE_ID_USB_FD_AUDIO_C,
        DRV_FILE_ID_USB_FD_CCID_C,
        DRV_FILE_ID_USB_MASS_SD_C,
        DRV_FILE_ID_USB_MASS_SAMPLE_C,
        DRV_FILE_ID_USB_MASS_CDROM_C,
        DRV_FILE_ID_USB_CDROM_DISK_C,
        DRV_FILE_ID_USB_NCM_SAMPLE_C,
        DRV_FILE_ID_USB_ECM_SAMPLE_C,
        DRV_FILE_ID_USB_ECM_NDIS_C,
        DRV_FILE_ID_USB_NCM_NDIS_C,
        DRV_FILE_ID_USB_NET_MANAGER_C,
        DRV_FILE_ID_USB_RNDIS_APP_C,
        DRV_FILE_ID_USB_ACM_SAMPLE_C,
        DRV_FILE_ID_USB_ACM_MODEM_C,
        DRV_FILE_ID_USB_ACM_COM_C,
        DRV_FILE_ID_USB_AUDIO_SAMPLE_C,
        DRV_FILE_ID_USB_AUDIO_LOOPBACK_C,
        DRV_FILE_ID_USB_CCID_SAMPLE_C,
        DRV_FILE_ID_USB_CCID_HUAWEI_C,
        DRV_FILE_ID_NETSTARTUP_C,
        DRV_FILE_ID_WIFIMLAN_C,
        DRV_FILE_ID_WIFI_MAIN_C,
        DRV_FILE_ID_WIFI_END_C,
        DRV_FILE_ID_SDIO_HOST_C,
        DRV_FILE_ID_SDIO_INTERFACE_C,
        DRV_FILE_ID_WM8990_C,
        DRV_FILE_ID_WM8990_I2C,
        DRV_FILE_ID_USB_MNTN_C,
        DRV_FILE_ID_PMU_6451_C,
        LOG_SUBMOD_ID_BUTT
}DRV_FILE_ID_DEFINE_ENUM;

//extern VOIDPTR OM_Log_ptr;
VOIDPTR OM_Log_ptr;
extern VOIDPTR OM_Log1_ptr;
extern VOIDPTR OM_Log2_ptr;
extern VOIDPTR OM_Log3_ptr;
extern VOIDPTR OM_Log4_ptr;
//extern LOG_LEVEL_EN g_drvLev;
LOG_LEVEL_EN g_drvLev;

    /* 字符串打印方式 */
#ifdef __LOG_BBIT__
#define  USB_LOG(Level, message)\
    {  \
        if(NULL != OM_Log_ptr){(*OM_Log_ptr)(__FILE__, __LINE__, DRV_MODULE_ID_USB, 0, (Level > LOG_LEVEL_INFO)?LOG_LEVEL_INFO:Level, NULL);} \
    }

#define DRV_LOG(ModulePID, SubMod, Level, String)                                \
	do {                                                                         \
	    if(NULL != OM_Log_ptr)                                                   \
	    { (*OM_Log_ptr)(__FILE__, __LINE__, ModulePID, SubMod, Level, String); } \
	    else                                                                     \
	    { if( (g_drvLev) >= (Level) ) {(OSAL_LogMsg)(String, 0, 0, 0, 0, 0, 0); }}    \
	   }while((0))

#define DRV_LOG1(ModulePID, SubMod, Level, String, Para1)                                \
	do {                                                                                 \
	    if(NULL != OM_Log1_ptr)                                                          \
	    { (*OM_Log1_ptr)(__FILE__, __LINE__, ModulePID, SubMod, Level, String, Para1); } \
	    else                                                                             \
	    { if( (g_drvLev) >= (Level) ) {(OSAL_LogMsg)(String, Para1, 0, 0, 0, 0, 0); }}        \
	   }while((0))

#define DRV_LOG2(ModulePID, SubMod, Level, String, Para1, Para2)                                \
	do {                                                                                        \
	    if(NULL != OM_Log2_ptr)                                                                 \
	    { (*OM_Log2_ptr)(__FILE__, __LINE__, ModulePID, SubMod, Level, String, Para1, Para2); } \
	    else                                                                                    \
	    { if( (g_drvLev) >= (Level) ) {(OSAL_LogMsg)(String, Para1, Para2, 0, 0, 0, 0); }}           \
	   }while((0))

#define DRV_LOG3(ModulePID, SubMod, Level, String, Para1, Para2, Para3)                                \
	do {                                                                                               \
	    if(NULL != OM_Log3_ptr)                                                                        \
	    { (*OM_Log3_ptr)(__FILE__, __LINE__, ModulePID, SubMod, Level, String, Para1, Para2, Para3); } \
	    else                                                                                           \
	    { if( (g_drvLev) >= (Level) ) {(OSAL_LogMsg)(String, Para1, Para2, Para3, 0, 0, 0); }}              \
	   }while((0))

#define DRV_LOG4(ModulePID, SubMod, Level, String, Para1, Para2, Para3, Para4)                                \
	do {                                                                                                      \
	    if(NULL != OM_Log4_ptr)                                                                               \
	    { (*OM_Log4_ptr)(__FILE__, __LINE__, ModulePID, SubMod, Level, String, Para1, Para2, Para3, Para4); } \
	    else                                                                                                  \
	    { if( (g_drvLev) >= (Level) ) {(OSAL_LogMsg)(String, Para1, Para2, Para3, Para4, 0, 0); }}                 \
	   }while((0))
	   
#endif    /* __LOG_BBIT__ */

    /* 打点方式 */
#ifdef __LOG_RELEASE__
    /********A32D13656 k60638  2007-11-28  modify begin*********/
#define  USB_LOG(Level, message)\
    {  \
        if(NULL != OM_Log_ptr){(*OM_Log_ptr)(DRV_MODULE_ID_USB, 0, (Level > LOG_LEVEL_INFO)?LOG_LEVEL_INFO:Level, DrvLogId(THIS_FILE_ID, __LINE__));} \
    }

#define DRV_LOG(ModulePID, SubMod, Level, String)   \
    {  \
        if(NULL != OM_Log_ptr){(*OM_Log_ptr)(ModulePID, SubMod, Level, DrvLogId(THIS_FILE_ID, __LINE__));} \
        else{if( (g_drvLev) >= (Level) ){(OSAL_LogMsg)(String, 0, 0, 0, 0, 0, 0);}} \
    }

#define DRV_LOG1(ModulePID, SubMod, Level, String, Para1) \
        {   \
            if(NULL != OM_Log1_ptr){(*OM_Log1_ptr)(ModulePID, SubMod, Level, DrvLogId(THIS_FILE_ID, __LINE__), Para1);} \
            else {if( (g_drvLev) >= (Level) ){(OSAL_LogMsg)(String, Para1, 0, 0, 0, 0, 0);}}\
        }


#define DRV_LOG2(ModulePID, SubMod, Level, String, Para1, Para2) \
            { \
                if(NULL != OM_Log2_ptr){(*OM_Log2_ptr)(ModulePID, SubMod, Level, DrvLogId(THIS_FILE_ID, __LINE__), Para1, Para2);}\
                    else\
                    {\
                        if( (g_drvLev) >= (Level) )\
                        {\
                            (OSAL_LogMsg)(String, Para1, Para2, 0, 0, 0, 0);\
                        }\
                    }\
            }

#define DRV_LOG3(ModulePID, SubMod, Level, String, Para1, Para2, Para3) \
                    {\
                        if(NULL != OM_Log3_ptr)\
                        {\
                            (*OM_Log3_ptr)(ModulePID, SubMod, Level, DrvLogId(THIS_FILE_ID, __LINE__), Para1, Para2, Para3);\
                        }\
                        else\
                        {\
                            if( (g_drvLev) >= (Level) )\
                            {\
                                (OSAL_LogMsg)(String, Para1, Para2, Para3, 0, 0, 0);\
                            }\
                        }\
                    }

#define DRV_LOG4(ModulePID, SubMod, Level, String, Para1, Para2, Para3, Para4)  \
              {\
                        if(NULL != OM_Log4_ptr)\
                        {\
                            (*OM_Log4_ptr)(ModulePID, SubMod, Level, DrvLogId(THIS_FILE_ID, __LINE__), Para1, Para2, Para3, Para4);\
                        }\
                        else\
                        {\
                            if( (g_drvLev) >= (Level) )\
                            {\
                                (OSAL_LogMsg)(String, Para1, Para2, Para3, Para4, 0, 0);\
                            }\
                        }\
             }
/********A32D13656 k60638  2007-11-28  modify end*********/
#endif    /* __LOG_RELEASE__ */


extern void Log_SelfTask(unsigned long ulPara1, unsigned long ulPara2,
                             unsigned long ulPara3, unsigned long ulPara4);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __DRVLOGF_H__ */

