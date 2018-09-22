/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepTest.h
* Description:
*                sleep managerment
*
* Author:        刘永富
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		刘永富
* Date:			2011-09-20
* Description:	Initial version
*
*******************************************************************************/

#ifndef PWRCTRL_ACPU_SLEEP_TEST
#define PWRCTRL_ACPU_SLEEP_TEST

#define PV500V1_TIMER0_BASE (0x90026000)


#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */


/* 定时器1 的锁定控制寄存器,当完成定时器的锁定操作后，该寄存器自动清零*/
#define CLK_DEF_TC_UNLOCK	0       /* 0：不锁定*/
#define CLK_DEF_TC_LOCK		(1<<3)  /* 1：将定时器的当前值锁定到TIMER1_CURRENTVALUE中*/

/* TimerXRIS中断屏蔽位*/
#define CLK_DEF_TC_INTENABLE	0       /* 0：不屏蔽该中断*/
#define CLK_DEF_TC_INTDISABLE	(1<<2)  /* 1：屏蔽该中断 */

/* 定时器的计数模式位*/
#define CLK_DEF_TC_PERIODIC	(1<<1)          /* 1：user-defined count mode*/
#define CLK_DEF_TC_FREERUN	0               /* 0：free-running mode*/

/* 定时器使能位*/
#define CLK_DEF_TC_ENABLE	1     /* 1：Timer使能*/
#define	CLK_DEF_TC_DISABLE	0     /* 0：Timer禁止*/
	  

/* 用户时钟使能*/
#define CLK_DEF_ENABLE   (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTENABLE|	CLK_DEF_TC_ENABLE)
/* 用户时钟去使能*/
#define CLK_DEF_DISABLE  (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTDISABLE | CLK_DEF_TC_DISABLE)


#endif
