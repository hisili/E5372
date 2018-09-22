/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepTest.h
* Description:
*                sleep managerment
*
* Author:        ������
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		������
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
#define CLK_DEF_ENABLE   (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTENABLE|	CLK_DEF_TC_ENABLE)
/* �û�ʱ��ȥʹ��*/
#define CLK_DEF_DISABLE  (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTDISABLE | CLK_DEF_TC_DISABLE)


#endif
