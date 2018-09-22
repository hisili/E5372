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

#ifndef    LCD_DRV_H
#define    LCD_DRV_H

#include <mach/balong_v100r001.h>

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#if defined(CHIP_BB_6920ES)
#define LCD_VEC_ID_GPIO_0 		 19		/*GPIO_1_19*/
#define LCD_VEC_ID_GPIO_1		 20		/*GPIO_1_20*/
#elif defined(CHIP_BB_6920CS)
#define LCD_VEC_ID_GPIO_0 		 6		/*GPIO_4_6*/
#define LCD_VEC_ID_GPIO_1		 7		/*GPIO_4_7*/
#else
#error "invalid defined"
#endif

#define LCD_ILITEK				0x2		/*天马LCD*/
#define LCD_CHIMEI				0x0		/*奇美LCD*/

/*寄存器偏移*/
#define EMI_IDCYR0_OFFSET       0x00
#define EMI_WST1R0_OFFSET       0x04
#define EMI_WST2R0_OFFSET       0x08
#define EMI_WSTOENR0_OFFSET     0x0c
#define EMI_WSTWENR0_OFFSET     0x10
#define EMI_CR0_OFFSET          0x14
#define EMI_SR0_OFFSET          0x18

#define CRG_CTRL0_OFFSET        0x0000          /*时钟使能，bit19，默认打开*/
#define CRG_CTRL1_OFFSET        0x0004          /*时钟使能，bit19，默认无效*/
#define CRG_CTRL2_OFFSET        0x0008          /*时钟状态，bit19，只读*/
#define CRG_CTRL14_OFFSET       0x0038          /*复位，bit25，默认不复位*/
#define CS_CTRL3_OFFSET         0x040c          /*LCD复位寄存器，0-复位，默认1不复位*/
#define IOS_CTRL71_OFFSET       0x091c          /*设置复用的DC\WR\RD信号*/
#define IOS_CTRL98_OFFSET       0x0988          /*设置复用片选和复位信号*/
#define IOS_CTRL99_OFFSET       0x098C          /*设置复用片选和复位信号*/

#define IOS_CTRL92_OFFSET       0x0970          /*控制lte_rf_spi_line4*/
#define IOS_CTRL100_OFFSET      0x0990          /*控制lte_rf_ssi_ctrl,lte_rf_ctrl,前默认0x0,后0x1,联合控制gpio3_1,控制LCD背光板*/

#define EMI_BUF_BASE_ADDR       0x900a3000
#define EMI_BUF_WRT_CMD         0           /*读出/写入命令,相对EMI_BUF_BASE_ADDR偏移地址*/
#define EMI_WRT_DATA_8BIT       0x01        /*读出/写入数据, 8-bit,相对EMI_BUF_BASE_ADDR偏移地址*/
#define EMI_WRT_DATA_16BIT      0x02        /*读出/写入数据, 16-bit,相对EMI_BUF_BASE_ADDR偏移地址*/

#define CRG_CLK_VALID           0x1 
#define CRG_CLK_INVALID         0x0
#define CRG_RST_EN              0x1 
#define CRG_RST_DIS             0x0 

#define CRG_CLK_OFFSET          (19)
#define CRG_RST_OFFSET          (25)

#define EMI_PAD_DIR_INPUT       (0)
#define EMI_PAD_DIR_OUTPUT      (1)

#define EMI_LEVEL_HIGH          (1)
#define EMI_LEVEL_LOW           (0)

#define EMI_REG_WRITE(reg, data)                BSP_REG_WRITE(emi_ctrl_base_addr_v, reg, data)
#define EMI_REG_READ(reg, result)               BSP_REG_READ(emi_ctrl_base_addr_v, reg, result)
#define EMI_REG_SETBITS(reg,pos,bits,val)       BSP_REG_SETBITS(emi_ctrl_base_addr_v, reg, pos, bits, val)
#define EMI_REG_GETBITS(reg,pos,bits)           BSP_REG_GETBITS(emi_ctrl_base_addr_v, reg, pos, bits)

#define CRG_REG_WRITE(reg, data)                BSP_REG_WRITE(V7R1_SC_VA_BASE, reg, data)
#define CRG_REG_READ(reg, result)               BSP_REG_READ(V7R1_SC_VA_BASE, reg, result)
#define CRG_REG_SETBITS(reg,pos,bits,val)       BSP_REG_SETBITS(V7R1_SC_VA_BASE, reg, pos, bits, val)
#define CRG_REG_GETBITS(reg,pos,bits)           BSP_REG_GETBITS(V7R1_SC_VA_BASE, reg, pos, bits)

/*错误码定义*/
#define BSP_ERR_LCD_MODULE_NOT_INITED   BSP_DEF_ERR(BSP_MODU_LCD, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_LCD_INVALID_PARA        BSP_DEF_ERR(BSP_MODU_LCD, BSP_ERR_INVALID_PARA)

#define BSP_ERR_LCD_PADSET        BSP_DEF_ERR(BSP_MODU_LCD, BSP_ERR_PADSET)
#define BSP_ERR_LCD_READ        BSP_DEF_ERR(BSP_MODU_LCD, BSP_ERR_READ)
#define BSP_ERR_LCD_WRITE        BSP_DEF_ERR(BSP_MODU_LCD, BSP_ERR_WRITE)

#undef LCD_HIGHT
#undef LCD_WIDTH
#define  LCD_HIGHT   (128)
#define  LCD_WIDTH   (128)
#define  PAGE_NUM     (8)/* BB5D02632,z67631 2009/9/16 */
#define  BYTE_NUMBER_PER_LINE   ((LCD_WIDTH + 7)/8)
#define  LCD_IMAGE_SIZE (LCD_HIGHT*BYTE_NUMBER_PER_LINE)


#define LCD_128_48_OFFSET      (16*16)
#define TIME_ANIMATION_POWER_ON  (6) 
#define LCD_128x48_SIZE        (LCD_IMAGE_SIZE-LCD_128_48_OFFSET)
#define LCD_X_OFFSET_128    128
#define LCD_Y_OFFSET_48    48
/* BEIGN ,Added by xuchao,2012/2/14 */
#define LCD_Y_OFFSET_128    128
/* END   ,Added by xuchao,2012/2/14 */
#define MAX_OFF_CHARGING_ANI    (5)
#define MAX_POWER_ON_ANIMATON   (5)

#define LCD_PWR_ON_STATE      1
#define LCD_PWR_OFF_STATE      0


/* LCD 类型选择*/
typedef enum tagLCD_TYPE_E
{
    LCD_TYPE_TFT            = 0,        /* tft lcd */
    LCD_TYPE_LCD           = 1,        /* oled */
    LCD_TYPE_STN,                       /* stn lcd */
    LCD_TYPE_BUTT
}LCD_TYPE_E;

/* LCD 接口选择*/
typedef enum tagLCD_IF_E
{
    LCD_IF_EMI              = 0,        /* emi 接口，默认 */
    LCD_IF_SPI              = 1,        /* spi 接口 */
    LCD_IF_BUTT
}LCD_IF_E;

/* used for user application write image through ioctl */
typedef struct tag_tft_write_data {
	unsigned int x_start;
	unsigned int y_start;
	unsigned int x_offset;
	unsigned int y_offset;
	unsigned char *image;
	unsigned int size;
}tft_write_data;

/* RAM 参数配置 */
typedef struct tagLCD_RAM_PARA_S
{
    BSP_U16      startc;       
    BSP_U16      startr;       
    BSP_U16      endc;  
    BSP_U16      endr;
}LCD_RAM_PARA_S;

/* 时序选择*/
typedef enum tagEMI_SCDUL_MODE_E
{
    EMI_TFT_8BIT       = 0,         /* TFT I8080时序,8-bit数据格式*/
    EMI_TFT_9BIT,                   /* TFT I8080时序,9-bit数据格式*/
    EMI_STN_8BIT_M6800,             /* STN M6800时序,8-bit数据格式*/
    EMI_STN_8BIT_I8080,             /* LCD I8080时序,8-bit数据格式*/
    EMI_LCD_8BIT,                  /* LCD M6800时序,8-bit数据格式*/
    EMI_SCHEDULING_BUTT
}EMI_SCDUL_MODE_E;

/* 时钟选择*/
typedef enum tagEMI_CLK_MODE_E
{
    EMI_CLK_EN             = 0,  /* 时钟使能，该枚举表明ctrl0的bit19置1，默认值，同时代表ctrl2的状态之一 */
    EMI_CLK_DIS,                 /* 时钟禁止，该枚举表明ctrl1的bit19置1，非默认值，同时代表ctrl2的状态之一 */
    EMI_CLK_VALID,               /* 时钟无效，该枚举表明ctrl0/ctrl1的bit19置0 */
    EMI_CLK_BUTT
}EMI_CLK_MODE_E;

/* 复位选择 */
typedef enum tagEMI_RST_MODE_E
{
    EMI_RST_DIS            = 0,  /* 复位请求无效，该枚举表明ctrl14的bit25置0*/
    EMI_RST_EN,                  /* 复位请求有效，该枚举表明ctrl14的bit25置1*/
    EMI_RST_BUTT
}EMI_RST_MODE_E;

/* 写入命令/数据选择*/
typedef enum tagEMI_DCX_MODE_E
{
    EMI_DCX_CMD          = 0,        /* 命令 */
    EMI_DCX_DATA         = 1,        /* 数据或命令参数 */
    EMI_DCX_BUTT
}EMI_DCX_MODE_E;

/* 8/16比特 类型选择*/
typedef enum tagEMI_IF_WDTH_E
{
    EMI_WDTH_8BIT            = 0,        /* 8-bit */
    EMI_WDTH_16BIT           = 1,        /* 16-bit */
    EMI_WDTH_BUTT
}EMI_IF_WDTH_E;

/* 时序参数配置 */
typedef struct tagEMI_SCDUL_PARA_S
{
    BSP_U32      u32Wst1;       
    BSP_U32      u32Wst2;       
    BSP_U32      u32Wst3;        
    BSP_U32      u32OEn;       
    BSP_U32      u32OEnDelay;       
    BSP_U32      u32WEn;        
    BSP_U32      u32WEnDelay;       
    BSP_U32      u32UpDlyr;        
    BSP_U32      u32DdnDlyr; 
    BSP_U32      u32UpDlyw;        
    BSP_U32      u32DdnDlyw; 
}EMI_SCDUL_PARA_S;


/*****************************************************************************
* 函 数 名  : emiInit
*
* 功能描述  : EMI初始化
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功或者失败
*****************************************************************************/
BSP_S32 emiInit(BSP_VOID);


/*****************************************************************************
* 函 数 名  : emiSetBackLight
*
* 功能描述  : LCD背光板控制
*
* 输入参数  : 
*
* 输出参数  : 无
*
* 返 回 值  : 设置成功或者失败
*****************************************************************************/
BSP_S32 emiSetBackLight(BSP_BOOL bOn);

void tftUpdateDisplay(int UpdateStatus);
BSP_S32 tftRefreshByBit(BSP_U32 ulXStart,BSP_U32 ulYStart,BSP_U32 ulXOffset,BSP_U32 ulYOffset,BSP_U8 * pucBuffer);
BSP_S32 balong_tft_ioctl(struct file *file, BSP_U32 cmd, unsigned long arg);
BSP_VOID tftStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of LCD_DRV_H */

