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

#define LCD_ILITEK				0x2		/*����LCD*/
#define LCD_CHIMEI				0x0		/*����LCD*/

/*�Ĵ���ƫ��*/
#define EMI_IDCYR0_OFFSET       0x00
#define EMI_WST1R0_OFFSET       0x04
#define EMI_WST2R0_OFFSET       0x08
#define EMI_WSTOENR0_OFFSET     0x0c
#define EMI_WSTWENR0_OFFSET     0x10
#define EMI_CR0_OFFSET          0x14
#define EMI_SR0_OFFSET          0x18

#define CRG_CTRL0_OFFSET        0x0000          /*ʱ��ʹ�ܣ�bit19��Ĭ�ϴ�*/
#define CRG_CTRL1_OFFSET        0x0004          /*ʱ��ʹ�ܣ�bit19��Ĭ����Ч*/
#define CRG_CTRL2_OFFSET        0x0008          /*ʱ��״̬��bit19��ֻ��*/
#define CRG_CTRL14_OFFSET       0x0038          /*��λ��bit25��Ĭ�ϲ���λ*/
#define CS_CTRL3_OFFSET         0x040c          /*LCD��λ�Ĵ�����0-��λ��Ĭ��1����λ*/
#define IOS_CTRL71_OFFSET       0x091c          /*���ø��õ�DC\WR\RD�ź�*/
#define IOS_CTRL98_OFFSET       0x0988          /*���ø���Ƭѡ�͸�λ�ź�*/
#define IOS_CTRL99_OFFSET       0x098C          /*���ø���Ƭѡ�͸�λ�ź�*/

#define IOS_CTRL92_OFFSET       0x0970          /*����lte_rf_spi_line4*/
#define IOS_CTRL100_OFFSET      0x0990          /*����lte_rf_ssi_ctrl,lte_rf_ctrl,ǰĬ��0x0,��0x1,���Ͽ���gpio3_1,����LCD�����*/

#define EMI_BUF_BASE_ADDR       0x900a3000
#define EMI_BUF_WRT_CMD         0           /*����/д������,���EMI_BUF_BASE_ADDRƫ�Ƶ�ַ*/
#define EMI_WRT_DATA_8BIT       0x01        /*����/д������, 8-bit,���EMI_BUF_BASE_ADDRƫ�Ƶ�ַ*/
#define EMI_WRT_DATA_16BIT      0x02        /*����/д������, 16-bit,���EMI_BUF_BASE_ADDRƫ�Ƶ�ַ*/

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

/*�����붨��*/
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


/* LCD ����ѡ��*/
typedef enum tagLCD_TYPE_E
{
    LCD_TYPE_TFT            = 0,        /* tft lcd */
    LCD_TYPE_LCD           = 1,        /* oled */
    LCD_TYPE_STN,                       /* stn lcd */
    LCD_TYPE_BUTT
}LCD_TYPE_E;

/* LCD �ӿ�ѡ��*/
typedef enum tagLCD_IF_E
{
    LCD_IF_EMI              = 0,        /* emi �ӿڣ�Ĭ�� */
    LCD_IF_SPI              = 1,        /* spi �ӿ� */
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

/* RAM �������� */
typedef struct tagLCD_RAM_PARA_S
{
    BSP_U16      startc;       
    BSP_U16      startr;       
    BSP_U16      endc;  
    BSP_U16      endr;
}LCD_RAM_PARA_S;

/* ʱ��ѡ��*/
typedef enum tagEMI_SCDUL_MODE_E
{
    EMI_TFT_8BIT       = 0,         /* TFT I8080ʱ��,8-bit���ݸ�ʽ*/
    EMI_TFT_9BIT,                   /* TFT I8080ʱ��,9-bit���ݸ�ʽ*/
    EMI_STN_8BIT_M6800,             /* STN M6800ʱ��,8-bit���ݸ�ʽ*/
    EMI_STN_8BIT_I8080,             /* LCD I8080ʱ��,8-bit���ݸ�ʽ*/
    EMI_LCD_8BIT,                  /* LCD M6800ʱ��,8-bit���ݸ�ʽ*/
    EMI_SCHEDULING_BUTT
}EMI_SCDUL_MODE_E;

/* ʱ��ѡ��*/
typedef enum tagEMI_CLK_MODE_E
{
    EMI_CLK_EN             = 0,  /* ʱ��ʹ�ܣ���ö�ٱ���ctrl0��bit19��1��Ĭ��ֵ��ͬʱ����ctrl2��״̬֮һ */
    EMI_CLK_DIS,                 /* ʱ�ӽ�ֹ����ö�ٱ���ctrl1��bit19��1����Ĭ��ֵ��ͬʱ����ctrl2��״̬֮һ */
    EMI_CLK_VALID,               /* ʱ����Ч����ö�ٱ���ctrl0/ctrl1��bit19��0 */
    EMI_CLK_BUTT
}EMI_CLK_MODE_E;

/* ��λѡ�� */
typedef enum tagEMI_RST_MODE_E
{
    EMI_RST_DIS            = 0,  /* ��λ������Ч����ö�ٱ���ctrl14��bit25��0*/
    EMI_RST_EN,                  /* ��λ������Ч����ö�ٱ���ctrl14��bit25��1*/
    EMI_RST_BUTT
}EMI_RST_MODE_E;

/* д������/����ѡ��*/
typedef enum tagEMI_DCX_MODE_E
{
    EMI_DCX_CMD          = 0,        /* ���� */
    EMI_DCX_DATA         = 1,        /* ���ݻ�������� */
    EMI_DCX_BUTT
}EMI_DCX_MODE_E;

/* 8/16���� ����ѡ��*/
typedef enum tagEMI_IF_WDTH_E
{
    EMI_WDTH_8BIT            = 0,        /* 8-bit */
    EMI_WDTH_16BIT           = 1,        /* 16-bit */
    EMI_WDTH_BUTT
}EMI_IF_WDTH_E;

/* ʱ��������� */
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
* �� �� ��  : emiInit
*
* ��������  : EMI��ʼ��
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : ��ʼ���ɹ�����ʧ��
*****************************************************************************/
BSP_S32 emiInit(BSP_VOID);


/*****************************************************************************
* �� �� ��  : emiSetBackLight
*
* ��������  : LCD��������
*
* �������  : 
*
* �������  : ��
*
* �� �� ֵ  : ���óɹ�����ʧ��
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

