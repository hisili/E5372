/******************************************************************************
*                                                                             *
* Copyright (C), 2006  Hisilicon Tech. Co., Ltd.                              *
* All Rights Reserved                                                         *
*                                                                             *
* File Name     : sdmmcDrv.h                                                  *
*                                                                             *
* Description   : BLOCK DEVICE DRIVER                                         *
*                                                                             *
* Version       : V1.3                                                        *
*                                                                             *
* Date          : 2006/6/20                                                   *
*                                                                             *
* Author        : m53980                                                      *
*                                                                             *
* Function List :                                                             *
*                                                                             *
* History       :                                                             *
* 1.Date        : 2006/6/20                                                   *
*   Author      : m53980                                                      *
*   Modification: Created file                                                *
* 2.Date        : 2006/08/29                                                  *
*   Author      : m53980                                                      *
*   Modification: coding style, add file header,modify comment method         *
*                 (PR:A32D05831)                                              *
* 3.Date        : 2006/10/18                                                  *
*   Author      : m53980                                                      *
*   Modification: add new function sdmmcGetStatus()                           *
*                 (PR:A32D05071)                                              *
* 4.Date        : 2006/10/24                                                  *
*   Author      : m53980                                                      *
*   Modification: 1,add new function sdmmcGetCapacity()                       *
*                 2,add new function sdmmcGetPath()                           *
*                 (PR:A32D06926)                                              *
*                                                                             *
******************************************************************************/

#ifndef SDMMC_DRV_H
#define SDMMC_DRV_H

#include <blkIo.h>

/* define device name */

#define SDMMC_NAME "/sd/hi"
#define SDMMC_PATH_LEN (sizeof(SDMMC_NAME)+4)

#define SDMMC_BUSY (1)
#define SDMMC_FREE (0)
#define SDMMC_BUSY_ERR (-2)

#if (BALONG_HW_CORE == FEATURE_ON)
#define SDMMC_GPIO_INT_ID  8
#define SDMMC_GPIO_INT_PIN 5
#elif (BALONG_SW_CORE == FEATURE_ON)
#define SDMMC_GPIO_INT_ID  2
#define SDMMC_GPIO_INT_PIN 7
#elif (BALONG_UDP == FEATURE_ON)

#if (FEATURE_E5_PRODUCT == FEATURE_ON)
#define SDMMC_GPIO_INT_ID  8
#define SDMMC_GPIO_INT_PIN 0
#define SDMMC_GPIO_INT_ID_E560  6
#define SDMMC_GPIO_INT_PIN_E560 1
 #else

#define SDMMC_GPIO_INT_ID  1
#define SDMMC_GPIO_INT_PIN 7
 #endif

#endif

#define SDMMC_MAX_HEAD_NUM 16
#define SDMMC_FILE_HEAD_LEN 92
#define SDMMC_UNIT_HEAD_FLAG 0xA55AAA55

#define SDMMC_EXC_BASE_ADDR   EXCH_SD_ADDR
#define SDMMC_EXC_CARD_ADDR   SDMMC_EXC_BASE_ADDR
#define SDMMC_EXC_INFO_START_ADDR   (SDMMC_EXC_CARD_ADDR + 0x100)
#define SDMMC_EXC_INFO_UNIT_SIZE      0x80
#define SDMMC_EXC_INFO_END_ADDR       (EXCH_SD_ADDR + EXCH_SD_SIZE)



typedef enum sdmmc_hook_type 
{
    SDMMC_HOOK_UPDATE_NOTIFY = 0,
    SDMMC_HOOK_INSERT_NOTIFY = 1,
    SDMMC_HOOK_REMOVE_NOTIFY = 2,
    SDMMC_HOOK_USB_IN_NOTIFY = 3,
    SDMMC_HOOK_UPVERIFY_NOTIFY =4, /*syb*/
    SDMMC_HOOK_BUFF
} SDMMC_HOOK_TYPE;

#pragma pack(1)
typedef struct SAppFileUnitHeader
{
    UINT32 dwMagicNum;	//取0xA5 5A AA 55
    UINT32 dwHeaderLen;	//本文件头长度，从dwMagicNum开始的长度,包括CRC 
    UINT32 dwHeaderVersion; //针对本结构填1，以后升级时增加
    UINT8  UnlockCode[8]; //区分待下载数据的code，8字节的任意数据，不同平台使用不同的值
    UINT32 dwDataType;//本段数据类型
    UINT32 dwDataLen;//本段数据的实际有效长度，不含填充字节
    UINT8  szDate[16];//制作日期字符串表示	
    UINT8  szTime[16];//制作时间字符串表示		
    UINT8  szDispName[32];	//本UNIT数据产品版本号、目标单板型号和单板软件版本号
    UINT16  wHeadCRC;		//从dwMagicNum校验到dwBlockSize
    UINT32 dwBlockSize;	//帧大小，目前各产品均为4096,每一dwBlockSize数据计算CRC校验
} FileUnitHeader;
#pragma pack() 


/* define SDMMC_DEV struct */
typedef struct sdmmc_dev_tag
{
    BLK_DEV      blkDev;
    int         drive;
    int         blkOffset;
}SDMMC_DEV;

/* initialise sdmmc host and card */
int sdmmcInit(void);

/* exit sdmmc module */
void sdmmcExit(void);

/* read/write block data */
int sdmmcBlkDataRW(int nCardNo,UINT32 dwBlkAddr,UINT32 nBlkCount,
                               UINT8  *pbDataBuff,int    nFlags);

/* erase block data */
int sdmmcEraseBlk(int nCardno,UINT32 dwBlkStart,UINT32 dwBlkCount);

/* install block device driver */
int sdmmcDrv(void);

/* create device */
BLK_DEV *sdmmcDevCreate(void);

/* block read */
int sdmmcBlkRd(SDMMC_DEV *pDev,int startBlk,int nBlks,char *pBuf);

/* block write */
int sdmmcBlkWrt(SDMMC_DEV *pDev,int startBlk,int nBlks,char *pBuf);

/* io control */
int sdmmcIoctl(SDMMC_DEV *pDev,int cmd,int arg);

/* device reset */
int sdmmcReset(SDMMC_DEV *pDev);

/*  device status check */
int sdmmcStatus(SDMMC_DEV *pDev);

/* create dos FAT16 file system */
int sdmmcCreateFs(void);

#if 0 /*memOpt*/
/* module init */
int usrSdmmcInit(void);
#endif

/* card status poll */
UINT32 sdmmcGetStatus(void);

/* get card capacity */
UINT32 sdmmcGetCapacity(void);

#if 0  /*memOpt*/
/* get device path name */
char * sdmmcGetPath(char * pbuf);
#endif

/* ---------------follows provide for sd mass------------------------------- */

#if 0 /*memOpt*/
/* mount for sd mass for windows */
int sdmmcUsbMount(void);
#endif

#if 0 /*memOpt*/
/* unmount for sd mass for windows */
int sdmmcUsbUnmount(void);
#endif

/* sd mass read sector */
int sdmmcUsbSecRead (UINT32 SecAddr, UINT16 SecCount, UINT8 * data_ptr); 

/* sd mass write sector */
int sdmmcUsbSecWrite(UINT32 SecAddr, UINT16 SecCount, UINT8 * data_ptr); 

/* sd mass get sd card capacity */
int sdmmcUsbReadCapacity(UINT32 * sec_count_ptr,UINT32 * sec_size_ptr);

UINT32 sdmmcUsbGetStatus(void);
#if 0  /*memOpt*/
int sdmmcIsInUse(void); 
#endif
void sdmmcUpdateExit();

int sdmmcUpdateIdentify();

int sdmmcGetUpdateAllUnitHead (FileUnitHeader * headData, UINT8 * headNum );

int sdmmcGetUpdateUnitData (UINT8 headId , UINT8 * data_ptr, UINT32 blockId);

int sdmmcAddHook (SDMMC_HOOK_TYPE hookType , void * p);

extern void sdmmcRegRead(UINT32 address);

extern void sdmmcCardInfo(UINT32 address);

extern void sdmmcExcInit(void);


/*
    l00131505, support DRX.
    if SD module enable system enter sleep, return 1.
    If don't allow system enter sleep, return 0;
*/
int sdmmcIsEnableSleep( void );

/*s00136931 ,LDO10 over current protect*/
void   sdmmcOverCurrentProtect(void);
void sdmmcDrvHwInit(void);



#endif
