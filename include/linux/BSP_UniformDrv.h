/**********************************************************************
* Copyright (C), 2006  Hisilicon Tech. Co., Ltd.                      *
* All Rights Reserved                                                 *
*                                                                     *
* File name: uniformDrv.h                                                 *
*                                                                     *
* Description : Data structure definitions for uniform module.            *
*                                                                     *
*                                                                     *
* Version: v0.1                                                       *
*                                                                     *
* Date:  2011-12-28                                                    *
*                                                                     *
* History:                                                            *
*                                                                     *
*   1. Date:                                                          *
*      Author:                                                        *
*      Modification:                                                  *
*   2. ...                                                            *
***********************************************************************/

#ifndef _UNIFORM_H
#define _UNIFORM_H

#ifdef __cplusplus
extern "C" {
#endif


#define SD_ENABLE_SIGNAL  1
#define SD_DISABLE_SIGNAL  0

typedef enum SCREEN_TYPE
{
    LED = 0,
    OLED,
    TFT,
    ERROR_SCREEN_TYPE
}SCREEN_TYPE_E;

typedef enum MEM_TYPE
{
    MEM_64M = 0,
    MEM_128M,
    ERROR_MEM_TYPE
}MEM_TYPE_E;

typedef struct E5_Product_version
{
    SCREEN_TYPE_E  E5_Screen_Type;
    MEM_TYPE_E E5_Mem_Size;
    BSP_BOOL E5_SD_Enable;
}E5_Product_version_info;

#define E5_VERSION_0        0x00    /*E5352s-2*/
#define E5_VERSION_1        0x10    /*E5352s-6*/
#define E5_VERSION_2        0x40    /*E5332s-2*/
#define E5_VERSION_3        0x70    /*E5351Qs-2*/
#define E5_VERSION_4        0xFF    /*E588目前尚无硬件版本号，等待后续改动*/

typedef enum VER_INDEX
{
    E5352s_INDEX = 0,
    E5332s_INDEX,
    E5351Qs_INDEX,
    E588_INDEX,
    ERROR_INDEX
}VER_INDEX_E;

#define ABB_INIT_CTRL_ADDR       0xdf
#define ABB_INIT_CTRL_ENABLE    0x80

#define GPIO_IOS_ADDR_BASE          0x90000000
#define GPIO_IOSCTRL99_ADDR_OFFSET  0x098c
#define GPIO_IOSCTRL98_ADDR_OFFSET  0x0988  /*复用寄存器0*/
#define GPIO_IOSCTRL25_ADDR_OFFSET  0x0864
#define GPIO_IOSCTRL26_ADDR_OFFSET  0x0868
#define GPIO_IOSCTRL03_ADDR_OFFSET  0x080c
#define GPIO_IOSCTRL04_ADDR_OFFSET  0x0810

#define GPIO0_ADDR_BASE             0x90006000
#define GPIO1_ADDR_BASE             0x90011000
#define GPIO_SWPORT_ADDR_OFFSET      0x04 /* GPIO0 A端口方向寄存器 (R/W) */ 


/*BSP_VOID DrvUniformPMUInit(BSP_VOID);*/
BSP_VOID DrvUniformScreenInit (BSP_VOID);



#ifdef __cplusplus
}
#endif

#endif
