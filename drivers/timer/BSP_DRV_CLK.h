/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  CLK.h
*
*   ��    �� :  liumengcun
*
*   ��    �� :  �û�ʱ��ͷ�ļ�
*
*   �޸ļ�¼ :  2009��3��5��  v1.00  liumengcun  ����
*
*************************************************************************/

#ifndef _CLK_H_
#define _CLK_H_

#ifdef __cplusplus
extern "C" 
{
#endif
#include <linux/interrupt.h>
#include "BSP.h"

#define CLK_REGBASE_0TO7             IO_ADDRESS(0x90002000)
//#define INT_LVL_TIMER_7                84
//#define USR_TIMER_INT_LVL           (INT_LVL_TIMER_7)
//#define PBXA9_TIMERS_CLK            (19200000)           /* Real time clock */

#define BSP_USR_CLK_MUX_NUM  1

//typedef BSP_S32 (*USRCLK_FUNCPTR)(BSP_S32);

#define USR_CLK_NUM     7
#define TIMER_BASE(ClkId)   (CLK_REGBASE_0TO7 + ClkId*0x14)

typedef struct tagUSRCLK_CTX_S
{
    USRCLK_FUNCPTR pUsrClkRoutine;
    BSP_S32 s32UsrClkArg;
    BSP_S32 s32UsrClkRunning;
    BSP_S32 s32UsrClkTicksPerSecond;
    BSP_S32 s32UsrClkIsUsed;
}USRCLK_CTX_S;

typedef enum tagUSRCLK_NUM_E
{
    USRCLK_NUM_0,   
    USRCLK_NUM_1,
    USRCLK_NUM_23,    
    USRCLK_NUM_4567,    
    USRCLK_NUM_BUTT
}USRCLK_NUM_E;


#define CLK_DEFULT_TICKS_PERSECOND 100

/* �û�ʱ��֧�ֵ�������ֵ*/
/*(0xffffffff * 1000)/19.2M*/
#define CLK_DEF_MAX_MILLSECOND  223696
#define CLK_DEF_MIN_MILLSECOND  1

#define CLK_DEF_MAX_COUNTER  0xFFFFFFFF  //���ֵΪ2TIMER_WIDTH_N - 1,TIMER_WIDTH_NΪ32λ

/* Ŀ���ʱ��Ƶ��Ϊ48M */
#define USR_TIMER_CLK	                  PBXA9_TIMERS_CLK

#define AMBA_RELOAD_TICKS	0
#define USR_CLK_RATE_MIN   1
#define USR_CLK_RATE_MAX   8000

#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */


/* ��ʱ��1 ���������ƼĴ���,����ɶ�ʱ�������������󣬸üĴ����Զ�����*/
#define CLK_DEF_TC_UNLOCK	0       /* 0��������*/
#define CLK_DEF_TC_LOCK		(1<<3)  /* 1������ʱ���ĵ�ǰֵ������TIMER1_CURRENTVALUE��*/

/* TimerXRIS�ж�����λ*/
#define CLK_DEF_TC_INTENABLE	0       /* 0�������θ��ж�*/
#define CLK_DEF_TC_INTDISABLE	(1<<2)  /* 1�����θ��ж� */

/* ��ʱ���ļ���ģʽλ*/
#define CLK_DEF_TC_PERIODIC	(1<<1)          /* 1��user-defined count mode*/
#define CLK_DEF_TC_FREERUN	0               /* 0��free-running mode*/

/* ��ʱ��ʹ��λ*/
#define CLK_DEF_TC_ENABLE	1     /* 1��Timerʹ��*/
#define	CLK_DEF_TC_DISABLE	0     /* 0��Timer��ֹ*/
	  
/* �û�ʱ��ʹ��*/
#define CLK_DEF_ENABLE   (CLK_DEF_TC_PERIODIC | CLK_DEF_TC_INTENABLE | CLK_DEF_TC_ENABLE)
/* �û�ʱ��ȥʹ��*/
#define CLK_DEF_DISABLE  (CLK_DEF_TC_PERIODIC | CLK_DEF_TC_INTDISABLE | CLK_DEF_TC_DISABLE)

irqreturn_t USRClkInt_Handle (int irq, void* dev_id);
BSP_VOID USRCLK_GetClkId(BSP_U32 u32ClkNum, BSP_U32 *pu32ClkId);
BSP_VOID USRCLK_GetClkNum(BSP_U32 u32ClkId, BSP_U32 *pu32ClkNum);
BSP_U32 USRCLK_GetCurTimerValue(BSP_U32 u32ClkNum);

#ifdef __cplusplus
}
#endif

#endif /*end #define _CLK_H_*/
