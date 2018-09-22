/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Boot Loader
*
* Version: v1.0
*
* Filename:    parttable.c
* Description:  Balong plantform boot loadr partition image(s) loading function
*
* Function List:
*
* History:
1.date:2011-11-19
 question number:
 modify reasion:         create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
/*******************************���ⵥ�޸ļ�¼********************************
����            ���ⵥ��            �޸���          �޸�����
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include "ptable_def.h"

/*----------------------- |  0 byte
 |"pTableHead"            | 
 *----------------------- | 16 byte (partition head flag string)
 | the property of table  |
 *----------------------- | 20 byte (partition head flag string)
 |"V3R2_FPGA" (example.)  |
 *----------------------- | 48 byte (partition table version name)
 | <partition info>       |
 |  (size 32byte)         |
 *----------------------- | 96 byte 
 | < partition info >     |
 |  (size 32byte)         |
 |------------------------| 144 byte
 :    .............       :             
 :    .............       :            
 |------------------------| 48 x N byte
 | < partition info >     |
 |  (size 32byte)         |
 |------------------------| 48 x (N+1) byte
 | "T"  (table end flag)  |
 |                        |
 |------------------------| */
 
/*table head begin*/
char ptable_head[PTABLE_HEAD_STR_SIZE] = {PTABLE_HEAD_STR};

/*Ԥ�������������Ա�ʶλ�ռ�*/
int  ptable_property[PTABLE_HEAD_PROPERTY_SIZE/sizeof(int)] = {0x80000000}; 

/*����bootrom�汾��*/
/*bootrom history list*/
/*
2012.03.27  ver: 01.00     create  
2012.04.27  ver: 01.02     �������Թ������ź���
*/
char ptable_bootrom_version[PTABLE_BOOTROM_VER_SIZE] = {PTABLE_BOOTROM_VER_STR}; 

/*table version string*/
char ptable_name[PTABLE_NAME_SIZE ] = {PTABLE_VER_STR};

#ifdef FEATURE_HUAWEI_E5_FLASH_PTABLE

#define HW_TERMINEL_FLASH_BOOTLOADER_LEN 0x20000 

#define HW_TERMINEL_FLASH_NV_LTE_LEN 0x180000 

#define HW_TERMINEL_FLASH_NV_GU_LEN 0x100000 

#define HW_TERMINEL_FLASH_BOOTROM_A_LEN 0x300000 

#define HW_TERMINEL_FLASH_BOOTROM_B_LEN 0x300000 

#define HW_TERMINEL_FLASH_VXWORKS_MCORE_LEN 0x1B00000 

#define HW_TERMINEL_FLASH_LOGO_LEN 0x100000 

#define HW_TERMINEL_FLASH_YAFFS_MCORE_LEN	 0xC00000 

#define HW_TERMINEL_FLASH_ANDROID_FASTBOOT_LEN 0x80000 

#define HW_TERMINEL_FLASH_ANDROID_BOOTIMAGE_LEN 0x400000 
	
#define HW_TERMINEL_FLASH_YAFFS_WEBUI_LEN 		 0x1700000 

#define HW_TERMINEL_FLASH_YAFFS_ACORE_LEN 0x1C00000 
      	
#define HW_TERMINEL_FLASH_YAFFS_APP_LEN        	 0x700000 

#define HW_TERMINEL_FLASH_YAFFS_OEM_LEN 0x200000 

#define HW_TERMINEL_FLASH_YAFFS_USERDATA_LEN 0xA00000 
	
#define HW_TERMINEL_FLASH_YAFFS_ONLINE_LEN 		 0x5F00000 

#define HW_TERMINEL_FLASH_ISO_LEN 0x25E0000 

/* bootload */
#define HW_TERMINEL_PTABLE_BOOTLOADER_START          0x0
#define HW_TERMINEL_PTABLE_BOOTLOADER_LEN            (HW_TERMINEL_FLASH_BOOTLOADER_LEN)
#define HW_TERMINEL_PTABLE_BOOTLOADER_END            (HW_TERMINEL_PTABLE_BOOTLOADER_START + HW_TERMINEL_PTABLE_BOOTLOADER_LEN)
    
/* NV LTE */
#define HW_TERMINEL_PTABLE_NV_LTE_START              HW_TERMINEL_PTABLE_BOOTLOADER_END
#define HW_TERMINEL_PTABLE_NV_LTE_LEN                (HW_TERMINEL_FLASH_NV_LTE_LEN)
#define HW_TERMINEL_PTABLE_NV_LTE_END                (HW_TERMINEL_PTABLE_NV_LTE_START + HW_TERMINEL_PTABLE_NV_LTE_LEN)
    
/* NV GU */
#define HW_TERMINEL_PTABLE_NV_GU_START               HW_TERMINEL_PTABLE_NV_LTE_END
#define HW_TERMINEL_PTABLE_NV_GU_LEN                 (HW_TERMINEL_FLASH_NV_GU_LEN)
#define HW_TERMINEL_PTABLE_NV_GU_END                 (HW_TERMINEL_PTABLE_NV_GU_START + HW_TERMINEL_PTABLE_NV_GU_LEN)
    
/* bootrom A */
#define HW_TERMINEL_PTABLE_BOOTROM_A_START           HW_TERMINEL_PTABLE_NV_GU_END
#define HW_TERMINEL_PTABLE_BOOTROM_A_LEN             (HW_TERMINEL_FLASH_BOOTROM_A_LEN)
#define HW_TERMINEL_PTABLE_BOOTROM_A_END             (HW_TERMINEL_PTABLE_BOOTROM_A_START + HW_TERMINEL_PTABLE_BOOTROM_A_LEN)
    
/* bootrom B */
#define HW_TERMINEL_PTABLE_BOOTROM_B_START           HW_TERMINEL_PTABLE_BOOTROM_A_END
#define HW_TERMINEL_PTABLE_BOOTROM_B_LEN             (HW_TERMINEL_FLASH_BOOTROM_B_LEN)
#define HW_TERMINEL_PTABLE_BOOTROM_B_END             (HW_TERMINEL_PTABLE_BOOTROM_B_START + HW_TERMINEL_PTABLE_BOOTROM_B_LEN)

/* YAFFS USER DATA */
#define HW_TERMINEL_PTABLE_YAFFS_USERDATA_START      HW_TERMINEL_PTABLE_BOOTROM_B_END
#define HW_TERMINEL_PTABLE_YAFFS_USERDATA_LEN        (HW_TERMINEL_FLASH_YAFFS_USERDATA_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_USERDATA_END        (HW_TERMINEL_PTABLE_YAFFS_USERDATA_START+HW_TERMINEL_PTABLE_YAFFS_USERDATA_LEN)

/* YAFFS ONLINE */
#define HW_TERMINEL_PTABLE_YAFFS_ONLINE_START        HW_TERMINEL_PTABLE_YAFFS_USERDATA_END
#define HW_TERMINEL_PTABLE_YAFFS_ONLINE_LEN          (HW_TERMINEL_FLASH_YAFFS_ONLINE_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_ONLINE_END          (HW_TERMINEL_PTABLE_YAFFS_ONLINE_START + HW_TERMINEL_PTABLE_YAFFS_ONLINE_LEN)

/* YAFFS MCore */
#define HW_TERMINEL_PTABLE_YAFFS_MCORE_START         HW_TERMINEL_PTABLE_YAFFS_ONLINE_END
#define HW_TERMINEL_PTABLE_YAFFS_MCORE_LEN           (HW_TERMINEL_FLASH_YAFFS_MCORE_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_MCORE_END           (HW_TERMINEL_PTABLE_YAFFS_MCORE_START+HW_TERMINEL_PTABLE_YAFFS_MCORE_LEN)

/* vxWorks MCore */
#define HW_TERMINEL_PTABLE_VXWORKS_MCORE_START       HW_TERMINEL_PTABLE_YAFFS_MCORE_END
#define HW_TERMINEL_PTABLE_VXWORKS_MCORE_LEN         (HW_TERMINEL_FLASH_VXWORKS_MCORE_LEN)
#define HW_TERMINEL_PTABLE_VXWORKS_MCORE_END         (HW_TERMINEL_PTABLE_VXWORKS_MCORE_START + HW_TERMINEL_PTABLE_VXWORKS_MCORE_LEN)

/* Android Fastboot */
#define HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_START    HW_TERMINEL_PTABLE_VXWORKS_MCORE_END
#define HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LEN      (HW_TERMINEL_FLASH_ANDROID_FASTBOOT_LEN)
#define HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_END      (HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_START + HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LEN)

/* Android Bootimage */
#define HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_START   HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_END
#define HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_LEN     (HW_TERMINEL_FLASH_ANDROID_BOOTIMAGE_LEN)
#define HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_END     (HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_START + HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_LEN)    

/* YAFFS ACore */
#define HW_TERMINEL_PTABLE_YAFFS_ACORE_START         HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_END
#define HW_TERMINEL_PTABLE_YAFFS_ACORE_LEN           (HW_TERMINEL_FLASH_YAFFS_ACORE_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_ACORE_END           (HW_TERMINEL_PTABLE_YAFFS_ACORE_START+HW_TERMINEL_PTABLE_YAFFS_ACORE_LEN)

/* YAFFS USER CODE */
#define HW_TERMINEL_PTABLE_YAFFS_APP_START          HW_TERMINEL_PTABLE_YAFFS_ACORE_END
#define HW_TERMINEL_PTABLE_YAFFS_APP_LEN            (HW_TERMINEL_FLASH_YAFFS_APP_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_APP_END            (HW_TERMINEL_PTABLE_YAFFS_APP_START+HW_TERMINEL_PTABLE_YAFFS_APP_LEN)

/* YAFFS WEBUI */
#define HW_TERMINEL_PTABLE_YAFFS_WEBUI_START          HW_TERMINEL_PTABLE_YAFFS_APP_END
#define HW_TERMINEL_PTABLE_YAFFS_WEBUI_LEN            (HW_TERMINEL_FLASH_YAFFS_WEBUI_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_WEBUI_END            (HW_TERMINEL_PTABLE_YAFFS_WEBUI_START+HW_TERMINEL_PTABLE_YAFFS_WEBUI_LEN)

/* YAFFS OEM */
#define HW_TERMINEL_PTABLE_YAFFS_OEM_START           HW_TERMINEL_PTABLE_YAFFS_WEBUI_END
#define HW_TERMINEL_PTABLE_YAFFS_OEM_LEN             (HW_TERMINEL_FLASH_YAFFS_OEM_LEN)
#define HW_TERMINEL_PTABLE_YAFFS_OEM_END             (HW_TERMINEL_PTABLE_YAFFS_OEM_START+HW_TERMINEL_PTABLE_YAFFS_OEM_LEN)

/* logo */
#define HW_TERMINEL_PTABLE_LOGO_START                HW_TERMINEL_PTABLE_YAFFS_OEM_END
#define HW_TERMINEL_PTABLE_LOGO_LEN                  (HW_TERMINEL_FLASH_LOGO_LEN)
#define HW_TERMINEL_PTABLE_LOGO_END                  (HW_TERMINEL_PTABLE_LOGO_START + HW_TERMINEL_PTABLE_LOGO_LEN)


#define HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LOADSIZE_LEN (PRODUCT_CFG_FLASH_ANDROID_FASTBOOT_LOADSIZE_LEN)

/* ISO  */
#define HW_TERMINEL_PTABLE_ISO_START               HW_TERMINEL_PTABLE_LOGO_END
#define HW_TERMINEL_PTABLE_ISO_LEN                  HW_TERMINEL_FLASH_ISO_LEN
#define HW_TERMINEL_PTABLE_ISO_END                  (HW_TERMINEL_PTABLE_ISO_START + HW_TERMINEL_PTABLE_ISO_LEN)

#ifndef PTABLE_BOOTLOADER_ENTRY
#define PTABLE_BOOTLOADER_ENTRY  (AXI_MEM_ADDR)
#endif
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
/*table partition begin*/
struct ST_PART_TBL ptable_product[PTABLE_PARTITION_MAX] =
{
    /*name*/        /*offset*/      /*loadsize*/    /*capacity*/   /*loadaddr*/   /*entry*/     /*type*/            /*property*/
    {"BootLoad"     ,HW_TERMINEL_PTABLE_BOOTLOADER_START    ,0x00000000   ,HW_TERMINEL_PTABLE_BOOTLOADER_LEN,   PTABLE_BOOTLOADER_ENTRY,  PTABLE_BOOTLOADER_ENTRY   ,IMAGE_BOOTLOAD  ,DATA_NORMAL                       ,   0},
    {"NvBackLTE"    ,HW_TERMINEL_PTABLE_NV_LTE_START   ,0x00000000  ,HW_TERMINEL_PTABLE_NV_LTE_LEN,   0x00000000,  0x00000000   ,IMAGE_NVBACKLTE ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"NvBackGU"     ,HW_TERMINEL_PTABLE_NV_GU_START   ,0x00000000   ,HW_TERMINEL_PTABLE_NV_GU_LEN,   0x00000000,  0x00000000   ,IMAGE_NVBACKGU  ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"BootRom"      ,HW_TERMINEL_PTABLE_BOOTROM_A_START   ,(HW_TERMINEL_PTABLE_BOOTROM_A_LEN-0x40000)  ,HW_TERMINEL_PTABLE_BOOTROM_A_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"BootRom"      ,HW_TERMINEL_PTABLE_BOOTROM_B_START   ,(HW_TERMINEL_PTABLE_BOOTROM_B_LEN-0x40000)   ,HW_TERMINEL_PTABLE_BOOTROM_B_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"/yaffs2"      ,HW_TERMINEL_PTABLE_YAFFS_USERDATA_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_USERDATA_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS2    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs5"      ,HW_TERMINEL_PTABLE_YAFFS_ONLINE_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_ONLINE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS5    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD      ,   0},
    {"/yaffs0"      ,HW_TERMINEL_PTABLE_YAFFS_MCORE_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_MCORE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS0     ,DATA_YAFFS | MOUNT_VXWORKS                       ,   0}, 
    {"VxWorks"      ,HW_TERMINEL_PTABLE_VXWORKS_MCORE_START   ,(HW_TERMINEL_PTABLE_VXWORKS_MCORE_LEN-0x60000)   ,HW_TERMINEL_PTABLE_VXWORKS_MCORE_LEN,   (PTABLE_VXWORKS_MCORE_ENTRY-0x80),  PTABLE_VXWORKS_MCORE_ENTRY   ,IMAGE_MCORE     ,DATA_NORMAL                       ,   0},
    {"FastBoot"     ,HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_START ,HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LOADSIZE_LEN   ,HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LEN,   PTABLE_FASTBOOT_ENTRY,  PTABLE_FASTBOOT_ENTRY   ,IMAGE_FASTBOOT    ,        DATA_NORMAL,   0},
    {"kernel"       ,HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_START ,0x00000000   ,HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_LEN,   0x00000000,  0x00000000   ,IMAGE_ACORE    ,DATA_NORMAL                        ,   0},
    {"/yaffs1"      ,HW_TERMINEL_PTABLE_YAFFS_ACORE_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_ACORE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS1    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs3"      ,HW_TERMINEL_PTABLE_YAFFS_APP_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_APP_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS3    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs6"      ,HW_TERMINEL_PTABLE_YAFFS_WEBUI_START   ,0x00000000   ,HW_TERMINEL_FLASH_YAFFS_WEBUI_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS6    ,DATA_YAFFS | PTABLE_MOUNT_MTD |MOUNT_BOOTROM       ,   0},
    {"/yaffs4"      ,HW_TERMINEL_PTABLE_YAFFS_OEM_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_OEM_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS4    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"Logo"         ,HW_TERMINEL_PTABLE_LOGO_START   ,0x00000000   ,HW_TERMINEL_PTABLE_LOGO_LEN,   0x00000000,  0x00000000   ,IMAGE_LOGO  ,DATA_NORMAL                       ,   0},    
    {"cdromiso"    ,HW_TERMINEL_PTABLE_ISO_START   ,0x00000000   ,HW_TERMINEL_PTABLE_ISO_LEN,   0x00000000,  0x00000000   ,IMAGE_CDROMISO    ,DATA_NORMAL | PTABLE_MOUNT_MTD       ,   0},
    {PTABLE_END_STR ,0x00000000   ,0x00000000   ,0x00000000,   0x00000000,  0x00000000   ,IMAGE_PART_TOP  ,DATA_YAFFS                        ,   0}
};
#else

struct ST_PART_TBL ptable_product[PTABLE_PARTITION_MAX] =
{
    /*name*/        /*offset*/      /*loadsize*/    /*capacity*/   /*loadaddr*/   /*entry*/     /*type*/            /*property*/
    {"BootLoad"     ,HW_TERMINEL_PTABLE_BOOTLOADER_START    ,0x00000000   ,HW_TERMINEL_PTABLE_BOOTLOADER_LEN,   PTABLE_BOOTLOADER_ENTRY,  PTABLE_BOOTLOADER_ENTRY   ,IMAGE_BOOTLOAD  ,DATA_NORMAL                       ,   0},
    {"NvBackLTE"    ,HW_TERMINEL_PTABLE_NV_LTE_START   ,0x00000000  ,HW_TERMINEL_PTABLE_NV_LTE_LEN,   0x00000000,  0x00000000   ,IMAGE_NVBACKLTE ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"NvBackGU"     ,HW_TERMINEL_PTABLE_NV_GU_START   ,0x00000000   ,HW_TERMINEL_PTABLE_NV_GU_LEN,   0x00000000,  0x00000000   ,IMAGE_NVBACKGU  ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"BootRom"      ,HW_TERMINEL_PTABLE_BOOTROM_A_START   ,(HW_TERMINEL_PTABLE_BOOTROM_A_LEN-0x40000)  ,HW_TERMINEL_PTABLE_BOOTROM_A_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"BootRom"      ,HW_TERMINEL_PTABLE_BOOTROM_B_START   ,(HW_TERMINEL_PTABLE_BOOTROM_B_LEN-0x40000)   ,HW_TERMINEL_PTABLE_BOOTROM_B_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"/yaffs2"      ,HW_TERMINEL_PTABLE_YAFFS_USERDATA_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_USERDATA_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS2    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs5"      ,HW_TERMINEL_PTABLE_YAFFS_ONLINE_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_ONLINE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS5    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD      ,   0},
    {"/yaffs0"      ,HW_TERMINEL_PTABLE_YAFFS_MCORE_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_MCORE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS0     ,DATA_YAFFS | MOUNT_VXWORKS                       ,   0}, 
    {"VxWorks"      ,HW_TERMINEL_PTABLE_VXWORKS_MCORE_START   ,(HW_TERMINEL_PTABLE_VXWORKS_MCORE_LEN-0x60000)   ,HW_TERMINEL_PTABLE_VXWORKS_MCORE_LEN,   (PTABLE_VXWORKS_MCORE_ENTRY-0x80),  PTABLE_VXWORKS_MCORE_ENTRY   ,IMAGE_MCORE     ,DATA_NORMAL                       ,   0},
    {"FastBoot"     ,HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_START ,HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LOADSIZE_LEN   ,HW_TERMINEL_PTABLE_ANDROID_FASTBOOT_LEN,   PTABLE_FASTBOOT_ENTRY,  PTABLE_FASTBOOT_ENTRY   ,IMAGE_FASTBOOT    ,        DATA_NORMAL,   0},
    {"kernel"       ,HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_START ,0x00000000   ,HW_TERMINEL_PTABLE_ANDROID_BOOTIMAGE_LEN,   0x00000000,  0x00000000   ,IMAGE_ACORE    ,DATA_NORMAL                        ,   0},
    {"/yaffs1"      ,HW_TERMINEL_PTABLE_YAFFS_ACORE_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_ACORE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS1    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs3"      ,HW_TERMINEL_PTABLE_YAFFS_APP_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_APP_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS3    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs6"      ,HW_TERMINEL_PTABLE_YAFFS_WEBUI_START   ,0x00000000   ,HW_TERMINEL_FLASH_YAFFS_WEBUI_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS6    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs4"      ,HW_TERMINEL_PTABLE_YAFFS_OEM_START   ,0x00000000   ,HW_TERMINEL_PTABLE_YAFFS_OEM_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS4    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"Logo"         ,HW_TERMINEL_PTABLE_LOGO_START   ,0x00000000   ,HW_TERMINEL_PTABLE_LOGO_LEN,   0x00000000,  0x00000000   ,IMAGE_LOGO  ,DATA_NORMAL                       ,   0},    
    {"cdromiso"    ,HW_TERMINEL_PTABLE_ISO_START   ,0x00000000   ,HW_TERMINEL_PTABLE_ISO_LEN,   0x00000000,  0x00000000   ,IMAGE_CDROMISO    ,DATA_NORMAL | PTABLE_MOUNT_MTD       ,   0},
    {PTABLE_END_STR ,0x00000000   ,0x00000000   ,0x00000000,   0x00000000,  0x00000000   ,IMAGE_PART_TOP  ,DATA_YAFFS                        ,   0}
};
#endif
#else /*FEATURE_HUAWEI_E5_FLASH_PTABLE*/
/*table partition begin*/
struct ST_PART_TBL ptable_product[PTABLE_PARTITION_MAX] =
{
    /*name*/        /*offset*/      /*loadsize*/    /*capacity*/   /*loadaddr*/   /*entry*/     /*type*/            /*property*/
    {"BootLoad"     ,PTABLE_BOOTLOADER_START    ,0x00000000   ,PTABLE_BOOTLOADER_LEN,   PTABLE_BOOTLOADER_ENTRY,  PTABLE_BOOTLOADER_ENTRY   ,IMAGE_BOOTLOAD  ,DATA_NORMAL                       ,   0},
    {"NvBackLTE"    ,PTABLE_NV_LTE_START   ,0x00000000  ,PTABLE_NV_LTE_LEN,   0x00000000,  0x00000000   ,IMAGE_NVBACKLTE ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"NvBackGU"     ,PTABLE_NV_GU_START   ,0x00000000   ,PTABLE_NV_GU_LEN,   0x00000000,  0x00000000   ,IMAGE_NVBACKGU  ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"BootRom"      ,PTABLE_BOOTROM_A_START   ,(PTABLE_BOOTROM_A_LEN-0x40000)  ,PTABLE_BOOTROM_A_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"BootRom"      ,PTABLE_BOOTROM_B_START   ,(PTABLE_BOOTROM_B_LEN-0x40000)   ,PTABLE_BOOTROM_B_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"VxWorks"      ,PTABLE_VXWORKS_MCORE_START   ,(PTABLE_VXWORKS_MCORE_LEN-0x60000)   ,PTABLE_VXWORKS_MCORE_LEN,   (PTABLE_VXWORKS_MCORE_ENTRY-0x80),  PTABLE_VXWORKS_MCORE_ENTRY   ,IMAGE_MCORE     ,DATA_NORMAL                       ,   0},
    {"Logo"         ,PTABLE_LOGO_START   ,0x00000000   ,PTABLE_LOGO_LEN,   0x00000000,  0x00000000   ,IMAGE_LOGO  ,DATA_NORMAL                       ,   0},
    {"/yaffs0"      ,PTABLE_YAFFS_MCORE_START   ,0x00000000   ,PTABLE_YAFFS_MCORE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS0     ,DATA_YAFFS | MOUNT_VXWORKS                       ,   0}, 

    {"FastBoot"     ,PTABLE_ANDROID_FASTBOOT_START ,PTABLE_ANDROID_FASTBOOT_LOADSIZE_LEN   ,PTABLE_ANDROID_FASTBOOT_LEN,   PTABLE_FASTBOOT_ENTRY,  PTABLE_FASTBOOT_ENTRY   ,IMAGE_FASTBOOT    ,        DATA_NORMAL,   0},
    {"kernel"       ,PTABLE_ANDROID_BOOTIMAGE_START ,0x00000000   ,PTABLE_ANDROID_BOOTIMAGE_LEN,   0x00000000,  0x00000000   ,IMAGE_ACORE    ,DATA_NORMAL                        ,   0},
    {"/yaffs1"      ,PTABLE_YAFFS_ACORE_START   ,0x00000000   ,PTABLE_YAFFS_ACORE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS1    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs3"      ,PTABLE_YAFFS_APP_START   ,0x00000000   ,PTABLE_YAFFS_APP_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS3    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs4"      ,PTABLE_YAFFS_OEM_START   ,0x00000000   ,PTABLE_YAFFS_OEM_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS4    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
	{"/yaffs2"      ,PTABLE_YAFFS_USERDATA_START   ,0x00000000   ,PTABLE_YAFFS_USERDATA_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS2    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
#if (FEATURE_HILINK == FEATURE_ON)
    {"/yaffs6"      ,PTABLE_YAFFS_WEBUI_START   ,0x00000000   ,PTABLE_YAFFS_WEBUI_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS6    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD     ,   0},
#else
    //{"/yaffs6"      ,PTABLE_YAFFS_WEBUI_START   ,0x00000000   ,PTABLE_YAFFS_WEBUI_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS6    ,DATA_YAFFS | PTABLE_MOUNT_MTD     ,   0},
#endif
    {"/yaffs5"      ,PTABLE_YAFFS_ONLINE_START   ,0x00000000   ,PTABLE_YAFFS_ONLINE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS5    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD      ,   0},
    {"cdromiso"    ,PTABLE_ISO_START   ,0x00000000   ,PTABLE_ISO_LEN,   0x00000000,  0x00000000   ,IMAGE_CDROMISO    ,DATA_NORMAL | PTABLE_MOUNT_MTD       ,   0},
    /* Added by wangrui for adding webui for -32 */
	{"/yaffs6"      ,PTABLE_YAFFS_WEBUI_START   ,0x00000000     ,PTABLE_YAFFS_WEBUI_LEN, 0x00000000,  0x00000000, IMAGE_YAFFS6,  DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD, 0},
    {PTABLE_END_STR ,0x00000000   ,0x00000000   ,0x00000000,   0x00000000,  0x00000000   ,IMAGE_PART_TOP  ,DATA_YAFFS                        ,   0}
};
#endif /*FEATURE_HUAWEI_E5_FLASH_PTABLE*/
char ptable_tail[PTABLE_TAIL_SIZE] = {PTABLE_TAIL_STR};

int ptable_size(void)
{
    return sizeof(ptable_product);
}


#ifdef __cplusplus
}
#endif
