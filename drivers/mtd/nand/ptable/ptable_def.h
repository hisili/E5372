/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: flash partition table
*
* Version: v1.0
*
* Filename:     ptable_def.h
* Description:  Balong plantform boot loadr partition image(s) defines header file
*
* Function List:
*
* History:
1.date:2011-12-12
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

#ifndef _PTABLE_DEF_H_
#define _PTABLE_DEF_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _ASMLANGUAGE

#include "nandc_balong.h"

#ifdef __BOOTLOADER__
#define BSPLOGSTR(x) {hiout((char *)x);}
#else
#define BSPLOGSTR(x) {hiout(("%s",(char *)x));}
#endif

#if defined(__FASTBOOT__)
#define BSPLOGU32(x) {hiout(("%x",x));}
#define BSPLOGU16(x) {hiout(("%x",x));}
#define BSPLOGU8(x)  {hiout(("%x",x));}
#elif defined(__BOOTLOADER__)
#define BSPLOGU32(y) {hioutv((y));}
#define BSPLOGU16(y) {hioutv((y));}
#define BSPLOGU8(y)  {hioutv((y));}
#else
#define BSPLOGU32(z) {hiout(("%08x",z));}
#define BSPLOGU16(z) {hiout(("%04x",z));}
#define BSPLOGU8(z)  {hiout(("%02x",z));}
#endif

/* 镜像区域 */
typedef enum IMAGE_TYPE
{
    /*一、产品线静态文件下载方式，涉及产品线修改，暂保留静态下载方式*/
    IMAGE_SFS_BOTTOM            = 0x0,        /*静态文件 开始 */
    IMAGE_CDROMISOVER          = 0x1,
    IMAGE_WEBUIVER             = 0x2,   
    IMAGE_CFDNVTable           = 0x3,
    ZSP_FILE_ADDR              = 0x4,       /*[锁定值],乒乓Bootrom合入，增加了BOOTROM_BACK枚举，
                                                为兼容之前的打包工具定义的ZSP文件头地址*/
    ISO_FILE_ADDR              = 0x5,       /*[锁定值],乒乓Bootrom合入，增加了BOOTROM_BACK枚举，
                                                为兼容之前的打包工具定义的ZSP文件头地址*/
    IMAGE_SFS_TOP              = 0x50,      /*静态文件结束*/
    
    /* 二、动态文件类型映像 */
    IMAGE_DFS_BOTTOM            = 0x64,      /*100*//*动态文件开始*/
    
    IMAGE_WEBUI                = 0x65,      /*101*//*产品线动态文件类型映像*/
    IMAGE_DYN_FS               = 0x66,      /*102*//*底软软动态文件类型映像*/
    
    IMAGE_DFS_TOP              = 0xfe,      /*动态文件结束*/
   
    /* 三 、flash分区映像类型 , 这个分区包含所有需要下载升级的映像标志 */
    IMAGE_PART_BOTTOM           = 0xff,      /*255*//*分区表索引开始*/

    TYPE_TABLE                 = 0x100,     /* 分区表 镜像标志*/
    IMAGE_BOOTLOAD             = 0x101,     /* BOOTLOADER  区    */
    IMAGE_BOOTROM              = 0x102,     /* BOOTROM 区       */
    IMAGE_MCORE                = 0x103,     /* MCORE VXWORKS 区  */
    IMAGE_FASTBOOT             = 0x104,     /* ACORE FASTBOOT 区 */
    IMAGE_ACORE                = 0x105,     /* ACORE LINUX 区  */
    IMAGE_YAFFS0               = 0x106,     /* YAFFS0 镜像 区 */
    IMAGE_YAFFS1               = 0x107,     /* YAFFS1 镜像 区 */
    IMAGE_YAFFS2               = 0x108,     /* YAFFS2 镜像 区 */
    IMAGE_YAFFS3               = 0x109,     /* YAFFS3 镜像 区 */
    IMAGE_ZSP                  = 0x10a,     /* ZSP 镜像 区    */
    IMAGE_HIFI                 = 0x10b,     /* HIFI 镜像 区   */
    IMAGE_CDROMISO             = 0x10c,     /* CDROM ISO 镜像区*/ 
    IMAGE_NVBACKLTE            = 0x10d,     /* LTE NV 备份 镜像区*/ 
    IMAGE_NVBACKGU             = 0x10e,     /* GU  NV 备份 镜像区*/
    IMAGE_VERSION              = 0x10f,     /* 版本号区 */
	IMAGE_LOGO	               = 0x110,     /* LOGO B  区       */
	IMAGE_YAFFS4               = 0x111,     /* YAFFS4 镜像 区 */	
    IMAGE_YAFFS5               = 0x112,     /* YAFFS5 镜像 区 */
	IMAGE_YAFFS6               = 0x113,     /* YAFFS6 镜像 区 */
    IMAGE_TEST                 = 0x114,     /* TEST 镜像 区 */
    IMAGE_PART_TOP             = 0x200      /*分区表索引 结束*/

}IMAGE_TYPE_E;

#define IMAGE_BOOTROM_MAX_NUM   2

/**********************************************************
*  the bit map of struct ST_PART_TBL.property
*----------------------------------------------------------
* bit:8 ~ bit:32           using property
*----------------------------------------------------------
*          | bit 12 ~ 32    --  not used yet 
*          | bit 11         --  protected
*          | bit 10         --  mount in bootrom 
*          | bit 9          --  mount in vxworks
*          | bit 8          --  auto load 
*----------------------------------------------------------
*  bit 4~7                 -- not used yet 
*----------------------------------------------------------
* bit:3                    -- image  data validity
*----------------------------------------------------------
*         | value           1 -- image is invalid 
*                           0 -- image is valid 
*----------------------------------------------------------
* bit:0 ~ bit:2            -- image  data type
*----------------------------------------------------------
*          value            1 -- indicate　yaffs image data
*         |                 0 -- indicate　normal data 
*-----------------------------------------------------------
************************************************************/
#define PTABLE_IMAGE_TYPE_MASK          (0x7)

typedef enum FALSH_DATA_TYPE
{
    DATA_NORMAL                 = 0,    /*表示此分区映象为普通二进制数据,不占用sp*/
    DATA_YAFFS                  = 1     /*表示此分区映象为yaffs映象,占用spare区*/ 
}FLASH_DATA_TYPE_E;


#define PTABLE_VALIDITY_OFFSET          (0x3)
#define PTABLE_VALIDITY_MASK            (0x1)

typedef enum FALSH_DATA_VALIDITY
{
    DATA_VALID                 = 0,    /*表示此分区映象数据完整有效,可以使用(默认)*/
    DATA_INVALID               = 1     /*表示此分区映象数据可能不完整,不可以使用*/ 
}FALSH_DATA_VALIDITY_E;


#define PTABLE_PROPERTY_OFFSET          (0x8)

typedef enum 
{
    PTABLE_AUTOLOAD_OFFSET              = PTABLE_PROPERTY_OFFSET,
    PTABLE_MOUNTVXWORKS_OFFSET,
    PTABLE_MOUNTBOOTROM_OFFSET,
    PTABLE_PROTECTED_OFFSET,
    PTABLE_PACKSCAN_OFFSET,
    PTABLE_FACTORY_OFFSET,  
    PTABLE_MTD_OFFSET
}PTABLE_OFFSET;

#define PTABLE_FLAG_AUTOLOAD        (1<<PTABLE_AUTOLOAD_OFFSET)    /*设此标志位 启动需要加载到对应的ddr地址*/
#define MOUNT_VXWORKS               (1<<PTABLE_MOUNTVXWORKS_OFFSET)/*设此标志位 VxWorks启动需要挂接载到对应的文件系统*/
#define MOUNT_BOOTROM               (1<<PTABLE_MOUNTBOOTROM_OFFSET)/*设此标志位 Bootrom启动需要挂接载到对应的文件系统*/
#define PTABLE_PROTECTED            (1<<PTABLE_PROTECTED_OFFSET)   /*设此标志位 数据在分区结构变化时受到保护*/
#define MOUNT_FACTORY               (1<<PTABLE_FACTORY_OFFSET)     /*设此标志位 在产品小镜像mount此分区*/
#define MOUNT_MTD                  (1<<PTABLE_MTD_OFFSET)         /*设此标志位 在A核MTD初始化的时候挂载此分区为MTD设备*/
#ifdef PRODUCT_CFG_SUPPORT_MOUNT_MTD
#define PTABLE_MOUNT_MTD            MOUNT_MTD
#else
#define PTABLE_MOUNT_MTD            0
#endif

#define TRACE_NORMAL(p)             NANDC_TRACE(NFCDBGLVL(NORMAL),p)
#define TRACE_NORMAL_P0(p)  
#define TRACE_NORMAL_P1(p)    
#define TRACE_NORMAL_P2(p)  

#define TRACE_ERROR(p)              NANDC_TRACE(NFCDBGLVL(ERRO),p)
#define TRACE_ERROR_P0(p)      
#define TRACE_ERROR_P1(p)     
#define TRACE_ERROR_P2(p)     

#define TRACE_WARNING(p)        NANDC_TRACE(NFCDBGLVL(WARNING),p)
#define TRACE_WARNING_P0(p)    
#define TRACE_WARNING_P1(p)    
#define TRACE_WARNING_P2(p)    



typedef struct ST_PART_TBL
{
    char    name[16];            /*name of this partition, uniquely*/
    u32     offset;             /*offset in flash of this partition*/
    u32     loadsize;           /*real data size of this partition, must smaller than "capacity"*/
    u32     capacity;           /*total size of this partition*/
    u32     loadaddr;           /*ram address of this this partition*/
    u32     entry;              /*if the partition is executable,this is entry address of this partition*/
    u32     image;              /*enum IMAGE_TYPE */
    u32     property;           /*property of this partition, eg. exe, yaffs, fat, ram image ...*/
    u32     count;              /*count for partition download or write*/
}ST_PART_TBL_ST;

/*定义分区表总体数据有效性标志位*/
typedef enum
{
    PTABLE_HEAD_VALIDITY_OFFSET = 0,
    PTABLE_HEAD_DEFAULT_OFFSET = 30
}PTABLE_HEAD_OFFSET;

#define PTABLE_FLAG_VALIDITY        (1<<PTABLE_HEAD_VALIDITY_OFFSET) /*设此标志位，启动（不管冷热）的时候进入强制加载模式*/
#define PTABLE_FLAG_DEFAULT         (1<<PTABLE_HEAD_DEFAULT_OFFSET)  /*设此标志位，表示系统使用的是默认分区表 */

/********************************************************************************/

/*----------------------- |  0 byte
 |"pTableHead"            | 
 *----------------------- | 12 byte (partition head flag string)
 | the property of table  |
 *----------------------- | 16 byte (partition table property)
 | bootrom version        |
 *----------------------- | 32 byte (partition bootrom ver string) 
 | ptable version         |
 *----------------------- | 48 byte (partition ptable ver string)
ptabe head define 48 bytess*/
#define PTABLE_HEAD_SIZE                sizeof(struct ST_PART_TBL)
#define PTABLE_HEAD_STR                "pTableHead"
#define PTABLE_HEAD_STR_SIZE           12
#define PTABLE_HEAD_PROPERTY_OFFSET     PTABLE_HEAD_STR_SIZE
#define PTABLE_HEAD_PROPERTY_SIZE       4
#define PTABLE_BOOTROM_VER_OFFSET       (PTABLE_HEAD_STR_SIZE + PTABLE_HEAD_PROPERTY_SIZE)
#define PTABLE_BOOTROM_VER_SIZE         16       
#define PTABLE_NAME_OFFSET              (PTABLE_HEAD_STR_SIZE + PTABLE_HEAD_PROPERTY_SIZE + PTABLE_BOOTROM_VER_SIZE)
#define PTABLE_NAME_SIZE                (PTABLE_HEAD_SIZE - PTABLE_HEAD_STR_SIZE - PTABLE_HEAD_PROPERTY_SIZE - PTABLE_BOOTROM_VER_SIZE)

/*ptabe TAIL define */
#define PTABLE_END_STR          "T" /*"pTableTail"*/

/*should move table head space*/
#define  PTABLE_PARTITION_MAX ((PTABLE_RAM_TABLE_SIZE - PTABLE_HEAD_SIZE) / sizeof(struct ST_PART_TBL))

/*尾部填充数据，使得编译出来的镜像正好占用DDRPTABLE_RAM_TABLE_SIZE的剩余空间*/
#define PTABLE_TAIL_SIZE   (PTABLE_RAM_TABLE_SIZE - (PTABLE_PARTITION_MAX) * sizeof(struct ST_PART_TBL) - PTABLE_HEAD_SIZE)
#define PTABLE_TAIL_STR     "pTableTail"

typedef void (*funcptr)(void); 

extern char ptable_head[];
extern int  ptable_property[]; 
extern char ptable_bootrom_version[]; 
extern char ptable_name[] ;
extern struct ST_PART_TBL ptable_product[]; 
#endif /*_ASMLANGUAGE*/

/*定义这个宏，将使用AXI memory传递flash分区表, 否则使用.data段ptable_product数据*/
#define PTABLE_AXI_USE_BSPMEMORY_H      

#define PART_TABLE_NAND_OFFSET  (0x1E000)   /*分区表在flash存储空间的偏移值*/
#define AXI_PTABLE_SIZE_MAX     (0x800)     /*分区表的下次升级最大扩展大小,这个大小加上MEMORY_AXI_PTABLE_ADDR不能超过AXI的最大值 */

#ifdef PTABLE_AXI_USE_BSPMEMORY_H
#define PTABLE_RAM_TABLE_SIZE       (MEMORY_AXI_PTABLE_SIZE)    /*分区表空间大小*/
#define PTABLE_RAM_TABLE_ADDR       (MEMORY_AXI_PTABLE_ADDR)    /*分区表运行存储RAM空间地址*/ 
#define PTABLE_BOOT_LOAD_AXI_ADDR   (MEMORY_AXI_PTABLE_ADDR)    /*分区表加载存储RAM空间地址*/ 

#else /*不使用bsp_memory.h 定义共享空间的情况，*/
#define PTABLE_RAM_TABLE_SIZE       (1024)                          /*分区表空间大小*/
#define PTABLE_RAM_TABLE_ADDR       (ptable_head)                   /*分区表运行存储RAM空间地址，使用数组符号地质*/ 
#define PTABLE_BOOT_LOAD_AXI_ADDR   (AXI_MEM_ADDR + AXI_MEM_SIZE - PTABLE_RAM_TABLE_SIZE) /*分区表加载存储RAM空间地址*/ 
#endif

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/



