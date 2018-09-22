/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_GLOBAL.h
*
*   作    者 :  wuzhen
*
*   描    述 :  公共定义头文件
*
*   修改记录 :  2009年3月12日  v1.00  wuzhen      创建
*              2009年3月25日  v2.00  wangxuesong 修改
*              2009年06月09日 v2.01  liumengcun 修改
*  BJ9D01075   修改BSP_ASSERT和BSP_TRACE实现
*************************************************************************/

#ifndef _BSP_GLOBAL_H_
#define _BSP_GLOBAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "product_config.h"
#include "BSP.h"
#include "arm_pbxa9.h"
#include "FeatureConfigDRV.h"

#if defined COMPILE_ENV_ARM && EXCLUDE_MATH_LIB
#undef __BSP_DEBUG__
#elif defined COMPILE_ENV_GNU && EXCLUDE_MATH_LIB
#undef __BSP_DEBUG__
#else
#define __BSP_DEBUG__ /* 如果需要调试信息，请打开此宏 */
#undef __BSP_DEBUG__
#endif

#ifdef PRODUCT_CFG_IMAGE_TYPE_BOOTLOADER
#define BSP_TRACE(enLogLevel, enLogModule, fmt, s32Arg1, s32Arg2, \
               s32Arg3, s32Arg4, s32Arg5, s32Arg6)
#else
#ifndef FEATURE_OBJ_CMP /* 当用于文件二进制比较时打开，否则会由于__FILE__,__LINE__造成大量差异 */
VOID BSP_TRACE(BSP_LOG_LEVEL_E enLogLevel, BSP_U32 enLogModule, const char *fmt, ...);
#else
#define BSP_TRACE(enLogLevel, enLogModule, fmt, s32Arg1, s32Arg2, \
               s32Arg3, s32Arg4, s32Arg5, s32Arg6)
#endif
#endif

#ifndef EXCLUDE_MATH_LIB
extern BSP_LOG_LEVEL_E g_enLogLevel;
extern BSP_U64 g_u64LogModule;
extern BSP_BOOL g_bPrintPos;
#endif

#if (defined (CHIP_BB_6920ES) && defined (BOARD_SFT))
#define ARM_FREQ    50 //MHZ
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
        && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
#define ARM_FREQ    450 //MHZ
#elif (defined (CHIP_BB_6920ES) && defined (BOARD_FPGA_P500))\
        ||(defined (CHIP_BB_6920CS) && defined (BOARD_FPGA_P500))
#define ARM_FREQ    600 //MHZ
#elif (defined (CHIP_BB_6920CS) && defined (BOARD_SFT))
#define ARM_FREQ    50 //MHZ
#else
    #error "unknown product macro"
#endif

#define UNIT_US      1
#define UNIT_MS      1000
#define ArmDelay(Delay,eUnit) do{\
    int i=0;\
    int loop = 0;\
	loop = Delay*ARM_FREQ*eUnit/4;\
    for(i=loop;i>=0;i--)\
    {\
        asm("nop");\
    }\
}while(0)

#define PRT_FLAG_EN_MAGIC 0x24680135
#define PRT_FLAG_EN_MAGIC_M 0x24680136
#define PRT_FLAG_EN_MAGIC_A 0x24680137

typedef enum tagIOS_MODULE
{
    IOS_GMII = 0,
    IOS_SDIO = 1,
    IOS_JTAG1 = 2,
    IOS_PTM = 3,
    IOS_MMC0 = 4,
    IOS_MMC1 = 5,
    IOS_MMC2 = 6,
    IOS_LCD = 7,
    IOS_HSUART = 8,
    IOS_I2C = 9,
    IOS_UART0 = 10,
    IOS_UART1 = 11,
    IOS_UART2 = 12,
    IOS_UART3 = 13,
    IOS_USIM0,
    IOS_USIM1,
    IOS_SSI0,
    IOS_SSI1,
    IOS_SSP0,
    IOS_SSP1,
    IOS_UICC,
    IOS_EFUSE,
    IOS_PMU,
    IOS_PCM
}IOS_MODULE;
BSP_VOID IOS_Config(IOS_MODULE Module);
#define ArmDelayUs(Delay) ArmDelay(Delay,UNIT_US)
#define ArmDelayMs(Delay) ArmDelay(Delay,UNIT_MS)

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_GLOBAL_H_*/

