/******************************************************************
* Copyright (C), 2005-2008, HISILICON Tech. Co., Ltd.             *
*                                                                                           *
* File name: balong_oledDrv.h                                                                  *
*                                                                                              *
* Description:                                                    *
*                                           *
*                                                                 *
* Author:                                                        *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *                                *
*                                                                 *
* Date:                                                           *
*                                                                 *
* History:                                                        *
                                                    *
*
*******************************************************************/

#include "BSP.h"

#ifndef    OLED_DRV_H
#define    OLED_DRV_H

#include <mach/balong_v100r001.h>

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#undef OLED_HIGHT
#undef OLED_WIDTH
#define  OLED_HIGHT   (64)
#define  OLED_WIDTH   (128)
#define  PAGE_NUM     (8)/* BB5D02632,z67631 2009/9/16 */
#define  BYTE_NUMBER_PER_LINE   ((OLED_WIDTH + 7)/8)
#define  OLED_IMAGE_SIZE (OLED_HIGHT*BYTE_NUMBER_PER_LINE)

#define  BYTE_NUMBER_PER_PAGE   (8*BYTE_NUMBER_PER_LINE)


#define OLED_128_48_OFFSET      (16*16)
#define TIME_ANIMATION_POWER_ON  (6) 
#define OLED_128x48_SIZE        (OLED_IMAGE_SIZE-OLED_128_48_OFFSET)
#define OLED_X_OFFSET_128    128
#define OLED_Y_OFFSET_48    48
#define MAX_OFF_CHARGING_ANI    (5)
#define MAX_POWER_ON_ANIMATON   (5)

#define OLED_PWR_ON_STATE      1
#define OLED_PWR_OFF_STATE      0
/*++by pandong cs*/
#if defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS)
#define IOMG_BASE_ADDR 			0x90004000
#define IOMG_REG_SIZE				0x0ff
#endif
/*--by pandong cs*/


#define EMI_SMBIDCYR0		0x0
#define EMI_SMBWST1R0		0x4
#define EMI_SMBWST2R0		0x8
#define EMI_SMBWSTOENR0		0xc
#define EMI_SMBWSTWENR0		0x10
#define EMI_SMBCR0			0x14
#define EMI_SMBSR0			0x18

/* used for user application write image through ioctl */
typedef struct tag_oled_write_data {
	unsigned int x_start;
	unsigned int y_start;
	unsigned int x_offset;
	unsigned int y_offset;
	unsigned char *image;
	unsigned int size;
}oled_write_data;

#if 0
/*函数声明*/
/*****************************************************************************
  Function    : oledInit
  Description : oledInit
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_S32 oledInit(BSP_VOID);

/*****************************************************************************
  Function    : oledPwrOn
  Description : oledPwrOn
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID  oledPwrOn(BSP_VOID);

/*****************************************************************************
  Function    : oledPwrOff
  Description : oledPwrOff
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID oledPwrOff(BSP_VOID);

/*****************************************************************************
  Function    : oledReset
  Description : oledReset
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID oledReset(BSP_VOID);

/*****************************************************************************
  Function    : oledLight
  Description : oledLight
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID oledLight(BSP_VOID);

/*****************************************************************************
  Function    : oledSleep
  Description : oledSleep
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID oledSleep(BSP_VOID);

/*****************************************************************************
  Function    : oledDim
  Description : oledDim
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID oledDim(BSP_VOID);

/*****************************************************************************
  Function    : oledInitImage
  Description : oledInitImage
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL     
  Author      : c54299  
  Date:          2010-09-16
*****************************************************************************/
BSP_VOID oledInitImage(BSP_VOID);

/*************************************************
*  Function:  oledClear
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*          UINT32 ulXStart, UINT32 ulYStart－－起始坐标
*          UINT32 ulXEnd, UINT32 ulYEnd－－横向纵向偏移量
*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
BSP_S32 oledClear(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset);

/*************************************************
*  Function:  oledClearWholeScreen
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
BSP_VOID oledClearWholeScreen(BSP_VOID);

/*************************************************
*  Function:  oledRefresh
*  Description: oled refresh *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*          UINT32 ulXStart, UINT32 ulYStart－－起始坐标
*          UINT32 ulXOffset, UINT32 ulYOffset－－横向纵向偏移量
*          UINT8 *pucBuffer－－显示数据

*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
BSP_S32 oledRefresh(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset, UINT8 *pucBuffer);

/*************************************************
*  Function:  balong_oled_ioctl
*  Description: oled ioctrl *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*          int cmd --command ID
*          arg--para

*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
static long balong_oled_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


/*****************************************************************************
  Function    : oledCharDisplay
  Description : oledCharDisplay
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : OK              ERROR      
  Author      : c54299  2007-03-15
*****************************************************************************/
BSP_VOID oledCharDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucChar);

/*****************************************************************************
  Function    : oledStringDisplay
  Description : oledStringDisplay
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : OK              ERROR      
  Author      : c54299  2007-03-15
*****************************************************************************/
BSP_VOID oledStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*****************************************************************************
  Function    : oledPwrOnAniInstall
  Description : 开机动画注册
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : OK              ERROR      
  Author      : c54299  2007-03-15
*****************************************************************************/
BSP_VOID oledPwrOnAniInstall(BSP_VOID);

/*****************************************************************************
  Function    : oledPwrOffChgAniInstall
  Description : 关机充电动画注册
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : OK              ERROR      
  Author      : c54299  2007-03-15
*****************************************************************************/
BSP_S32 oledPwrOffChgAniInstall(BSP_VOID);
#endif
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of OLED_DRV_H */

