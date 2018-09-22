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
/*******************************问题单修改记录********************************
日期            问题单号            修改人          修改内容
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

/*预留分区表总属性标识位空间*/
int  ptable_property[PTABLE_HEAD_PROPERTY_SIZE/sizeof(int)] = {0x80000000}; 

/*定义bootrom版本号*/
/*bootrom history list*/
/*
2012.03.27  ver: 01.00     create  
2012.04.27  ver: 01.02     升级特性功能扩张合入
*/
char ptable_bootrom_version[PTABLE_BOOTROM_VER_SIZE] = {PTABLE_BOOTROM_VER_STR}; 

/*table version string*/
char ptable_name[PTABLE_NAME_SIZE ] = {PTABLE_VER_STR};

/*table partition begin*/
struct ST_PART_TBL ptable_product[PTABLE_PARTITION_MAX] =
{
    /*name*/        /*offset*/      /*loadsize*/    /*capacity*/   /*loadaddr*/   /*entry*/     /*type*/            /*property*/
    {"BootLoad"     ,PTABLE_BOOTLOADER_START    ,0x00000000   ,PTABLE_BOOTLOADER_LEN,   PTABLE_BOOTLOADER_ENTRY,  PTABLE_BOOTLOADER_ENTRY   ,IMAGE_BOOTLOAD  ,DATA_NORMAL                       ,   0},
    {"NvBackLTE"    ,PTABLE_NV_LTE_START   ,0x00000000  ,(PTABLE_NV_LTE_LEN+0x40000),   0x00000000,  0x00000000   ,IMAGE_NVBACKLTE ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"NvBackGU"     ,(PTABLE_NV_GU_START+0x40000)   ,0x00000000   ,(PTABLE_NV_GU_LEN-0x20000),   0x00000000,  0x00000000   ,IMAGE_NVBACKGU  ,DATA_NORMAL | PTABLE_PROTECTED    ,   0},
    {"BootRom"      ,(PTABLE_BOOTROM_A_START+0x20000)   ,(PTABLE_BOOTROM_A_LEN-0x40000)  ,PTABLE_BOOTROM_A_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"BootRom"      ,(PTABLE_BOOTROM_B_START+0x20000)   ,(PTABLE_BOOTROM_B_LEN-0x40000)   ,PTABLE_BOOTROM_B_LEN,   (PTABLE_BOOTROM_ENTRY-0x80),  PTABLE_BOOTROM_ENTRY   ,IMAGE_BOOTROM   ,DATA_NORMAL                       ,   0},
    {"VxWorks"      ,(PTABLE_VXWORKS_MCORE_START+0x20000)   ,(PTABLE_VXWORKS_MCORE_LEN-0x60000)   ,(PTABLE_VXWORKS_MCORE_LEN),   (PTABLE_VXWORKS_MCORE_ENTRY-0x80),  PTABLE_VXWORKS_MCORE_ENTRY   ,IMAGE_MCORE     ,DATA_NORMAL                       ,   0},
    {"Logo"         ,(PTABLE_LOGO_START+0x20000)   ,0x00000000   ,PTABLE_LOGO_LEN,   0x00000000,  0x00000000   ,IMAGE_LOGO  ,DATA_NORMAL                       ,   0},
    {"/yaffs0"      ,(PTABLE_YAFFS_MCORE_START+0x20000)   ,0x00000000   ,(PTABLE_YAFFS_MCORE_LEN),   0x00000000,  0x00000000   ,IMAGE_YAFFS0     ,DATA_YAFFS | MOUNT_VXWORKS                       ,   0}, 
    {"FastBoot"     ,(PTABLE_ANDROID_FASTBOOT_START+0x20000)   ,PTABLE_ANDROID_FASTBOOT_LEN   ,PTABLE_ANDROID_FASTBOOT_LEN,   PTABLE_FASTBOOT_ENTRY,  PTABLE_FASTBOOT_ENTRY   ,IMAGE_FASTBOOT    ,        DATA_NORMAL,   0},
    {"kernel"       ,(PTABLE_ANDROID_BOOTIMAGE_START+0x20000) ,0x00000000   ,PTABLE_ANDROID_BOOTIMAGE_LEN,   0x00000000,  0x00000000   ,IMAGE_ACORE    ,DATA_NORMAL                        ,   0},
    {"/yaffs1"      ,(PTABLE_YAFFS_ACORE_START+0x20000)   ,0x00000000   ,(PTABLE_YAFFS_ACORE_LEN-0x220000),   0x00000000,  0x00000000   ,IMAGE_YAFFS1    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs3"      ,(PTABLE_YAFFS_APP_START-0x200000)   ,0x00000000   ,PTABLE_YAFFS_APP_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS3    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs4"      ,(PTABLE_YAFFS_OEM_START-0x200000)   ,0x00000000   ,PTABLE_YAFFS_OEM_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS4    ,DATA_YAFFS | PTABLE_MOUNT_MTD       ,   0},
	{"/yaffs2"      ,(PTABLE_YAFFS_USERDATA_START-0x200000)   ,0x00000000   ,(PTABLE_YAFFS_USERDATA_LEN+0x200000),   0x00000000,  0x00000000   ,IMAGE_YAFFS2    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD       ,   0},
    {"/yaffs5"      ,PTABLE_YAFFS_ONLINE_START   ,0x00000000   ,PTABLE_YAFFS_ONLINE_LEN,   0x00000000,  0x00000000   ,IMAGE_YAFFS5    ,DATA_YAFFS | MOUNT_BOOTROM | PTABLE_MOUNT_MTD      ,   0},
    {"cdromiso"     ,(PTABLE_ISO_START)   ,0x00000000   ,(PTABLE_ISO_LEN),   0x00000000,  0x00000000   ,IMAGE_CDROMISO    ,DATA_NORMAL | PTABLE_MOUNT_MTD,   0},
    {PTABLE_END_STR ,0x00000000   ,0x00000000   ,0x00000000,   0x00000000,  0x00000000   ,IMAGE_PART_TOP  ,DATA_YAFFS                        ,   0}
};

char ptable_tail[PTABLE_TAIL_SIZE] = {PTABLE_TAIL_STR};

int ptable_size(void)
{
    return sizeof(ptable_product);
}


#ifdef __cplusplus
}
#endif

