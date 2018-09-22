/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcCommon.h
* Description:
*                sleep managerment
*
* Author:        ¡ı”¿∏ª
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		¡ı”¿∏ª
* Date:			2011-09-20
* Description:	Initial version
*
* Author:		w00169995
* Date:			2012-01-01
* Description:	V7R1 Code 

*******************************************************************************/
#ifndef PWRCTRL_ACPU_SLEEP_COMMON
#define PWRCTRL_ACPU_SLEEP_COMMON

#include "BSP.h"
#include <mach/balong_v100r001.h>
#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
#include <mach/pwrctrl/BSP_PWC_SYSFUNC.h>

/*******************************************************/
typedef void (*PWRCTRLVOIDFUNCPTR)(void);

typedef unsigned int  PWRCTRL_UINT32;



/******************************************************/
typedef struct ST_PWC_SWITCH_STRU_S
{
    unsigned int deepsleep  :1; /*bit 0*/
    unsigned int lightsleep :1; /*bit 1*/
    unsigned int dfs        :1; /*bit 2*/
    unsigned int hifi       :1; /*bit 3*/
    
    unsigned int drxAbb     :1; /*bit 4*/
    unsigned int drxZspCore :1; /*bit 5*/
    unsigned int drxZspPll  :1; /*bit 6*/
    unsigned int drxWLBbpPll :1; /*bit 7*/
    unsigned int drxGBbpPll :1; /*bit 8*/
    unsigned int drxRf      :1; /*bit 9*/
    unsigned int drxPa      :1; /*bit 10*/    
    unsigned int drxGuBbpPd :1; /*bit11*/
	unsigned int drxDspPd   :1; /*bit12*/
	unsigned int drxLBbpPd  :1; /*bit13*/
    unsigned int drxPmuEco  :1; /*bit14*/
	unsigned int drxPeriPd :1; /*bit15*/         
    unsigned int mntn       :1; /*bit 16*/
    unsigned int bugChk     :1; /*bit 17*/
	unsigned int pmuSwitch  :1; /*bit 18*/
	unsigned int drxLdsp    :1; /*bit 19*/
	unsigned int matserTDSpd  :1; /*bit 20*/
	unsigned int tdsClk    :1; /*bit 21*/
    unsigned int slaveTDSpd   :1; /*bit 22*/
	unsigned int slow	   :1;/*bit23*/
	unsigned int reserved1    :8; /*bit 24-31*/

}ST_PWC_SWITCH_STRU;



extern ST_PWC_SWITCH_STRU g_stPwrctrlSwitch;
extern unsigned int g_ulPwrctrlDebugTrace;



#define __PWRCTRL_VERSION_CONTROL__

#define DEBUG_UART                      (0)
#define DEBUG_USB                       (1)
#define DEBUG_OFF                       (2)
#define PWRCTRL_SWITCH_OFF              (0)
#define PWRCTRL_SWITCH_ON               (1)

#define  PWRCTRL_ERROR                  (-1)
#define  PWRCTRL_OK                     (0)


#define PWRCTRL_FALSE					(0)
#define PWRCTRL_TRUE					(1)

#define PWC_WAKEUP_TIMER4				(1 << 12)


#define PWRCTRL_CLR_BIT32(reg,value)        \
    ((*((volatile unsigned int *)(reg))) = (*(volatile unsigned int *)(reg))& ~(unsigned int)(value))
#define PWRCTRL_SET_BIT32(reg,value)        \
    ((*((volatile unsigned int *)(reg))) = (*(volatile unsigned int *)(reg))|(unsigned int)(value))

#define PWRCTRL_REG_READ32(reg, result)	    (result = * ((volatile unsigned int*)(reg)))
#define PWRCTRL_REG_WRITE32(reg, data)	    (*((volatile unsigned int*)(reg)) = data)

#define BSP_REG_CLRBIT(base, reg, value)   (BSP_REG(base, reg) &= ~(BSP_U32)(value)) 

#define PWRCTRL_REG32(addr)         (*((volatile UINT32 *)addr))

#define SetBitReg32(reg, value)     (*((volatile UINT32 *)(reg)) = (*(volatile UINT32 *)(reg)) | (UINT32)(value))
#define ClearBitReg32(reg, value)   (*((volatile UINT32 *)(reg)) = (*(volatile UINT32 *)(reg)) & ~(UINT32)(value))


#if defined __PWRCTRL_VERSION_CONTROL__

#define PWRCTRL_DEBUG_TRACE(data) \
{\
    if(DEBUG_UART == g_ulPwrctrlDebugTrace)\
    {\
        *(volatile unsigned char *)(PWRCTRL_UART0)= data;\
    }\
    if(DEBUG_USB == g_ulPwrctrlDebugTrace)\
    {\
        printk("%c",data);\
    }\
}
#else
#define PWRCTRL_DEBUG_TRACE(data) 

#endif

/*****************************************************************************/
/*function prototype*/
/*****************************************************************************/
extern void PWRCTRL_WriteReg32( UINT32 ulRegAddr, UINT32 ulRegVal);
extern UINT32 PWRCTRL_ReadReg32( UINT32 ulRegAddr );
extern void PWRCTRL_ClearReg32Mask(UINT32 ulRegAddr, UINT32 ulMask);
extern void PWRCTRL_WriteReg32Mask(UINT32 ulRegAddr, UINT32 ulMask);

#endif
