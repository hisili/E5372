/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: softTimer.h                                                       */
/*                                                                           */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date:                                                            */
/*                                                                           */
/* Description: implement timer                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:                                                                  */
/*    Author:                                                                */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#ifndef _SOFT_TIMER_H
#define _SOFT_TIMER_H
/*--s00184745-modified for v7r1-20120214
#include "../../arch/arm/mach-balong/include/mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h"
*/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/***************************************�궨��*********************************/
#ifndef LOCAL
#define LOCAL static
#endif

#define _BSP_DEBUG_
/*���Ժ����꿪��*/
#define SOFTTIMER_DEBUG  

#define SOFTTIMER_CTRLBLK_NUM     40

#define SOFTTIMER_NULL_PTR            0L

#define SOFTTIMER_NO_LOOP            0x0   
#define SOFTTIMER_LOOP                   1     

#define SOFTTIMER_OK                      0
#define SOFTTIMER_ERR                     1

#define SOFTTIMER_NOT_USED            0x00
#define SOFTTIMER_USED                    0x01

#define SEM_FULL     (1)  //�ź�������
#define SEM_EMPTY   (0) //�ź���������

/* errno definiens */
#define SOFTTIMER_ERRNO_START_NOIDLETIMER            0x01
#define SOFTTIMER_ERRNO_STOP_TIMERINVALID            0x02
#define SOFTTIMER_ERRNO_FREE_TIMERNOTUSED           0x03
#define SOFTTIMER_ERRNO_COUNT_ERR           0x04
#define SOFT_TIMER_CLK                      32768

#define SOFTTIMER_MAX_LENGTH  (0xFFFFFFFF/timer_input_clk_get(ACPU_SOFTTIMER_ID)) 

#define TIMER_ONCE_COUNT     0
#define TIMER_PERIOD_COUNT  1

#define TIMER_DEFAULT_TICKS  100

#define TIMER_MAX_SECOND  100000

#define TIMER_CLK_RATE_MAX 10000

#define CLK_INT_VALID  0x01   	/* Judged the INT IS VALID or NOT */

#define CLK_REGOFF_RAWINTSTATUS 0x0A8	/* INT RAW STATUS (R/O) */

#define ELAPESD_TIME_INVAILD   0xFFFFFFFF

/***************************************Ӳ����ʱ����ַ*********************************/
/*s00184745--modified for  v7r1---20120214*/
//#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
//    #define ACPU_SOFTTIMER_ID (2)	 /*Ӳ����ʱ��ID*/	
//#elif defined (BOARD_SFT) && defined (VERSION_V3R2)    
//    #define ACPU_SOFTTIMER_ID (2)	 /*Ӳ����ʱ��ID*/
//#else                                
//    #define ACPU_SOFTTIMER_ID (0)	    /*Ӳ����ʱ��ID*/
//#endif


#define ACPU_SOFTTIMER_ID  (2) /*Ӳ����ʱ��ID*/
//#define PWRCTRL_SOC_ADDR_TIMER0_0    IO_ADDRESS(0x90002000)/*timer0 �Ļ�ַת��*/
//clean warning
//#define ACPU_SOFTTIMER_BASE  (PWRCTRL_SOC_ADDR_TIMER0_0+ACPU_SOFTTIMER_ID*0x14)//�Ѿ����е�ַת��


#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */


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
#define CLK_DEF_TC_DISABLE	0     /* 0��Timer��ֹ*/
	  

/* �û�ʱ��ʹ��*/
#define CLK_DEF_ENABLE   (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTENABLE|	CLK_DEF_TC_ENABLE)
/* �û�ʱ��ȥʹ��*/
#define CLK_DEF_DISABLE  (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTDISABLE | CLK_DEF_TC_DISABLE)

#define INT_LVL_TIMER_2            (79) /*Ӳ����ʱ��2�жϺ�*/

#define SPECIAL_TIMER_VOTE_SLEEP_TIME (1000)
#define SPECIAL_TIMER_TIME_UNIT_SECOND (1000)

/***************************************�ṹ����*********************************/

#ifndef __DRV_INTERFACE_H__
typedef unsigned int UINT32;
typedef unsigned char UINT8;
#endif

	

typedef void  (*SOFTTIMER_FUNC)(UINT32);
typedef int * HTIMER;

typedef struct SOFTTIMER_CONTROL_STRU
{
    UINT32      TimerId;            /* timer ID */
    UINT32      ulUsedFlag;       /* whether be used or not */
    UINT32      Para;                /* timer's paremate */
    SOFTTIMER_FUNC        CallBackFunc;/* timer's callback function */
    HTIMER      *phTm;               /* user's pointer which point the real timer room */
    UINT32      TimeOutValueInMilliSeconds;
    UINT32      TimeOutValueInCycle;
    UINT8       Mode;                      /* timer's mode */
    UINT8       Reserved[3];            /* for 4 byte aligned */
    char         ulFileID[70];                 /* alloc file ID */
    UINT32     ulLineNo;               /* alloc line no. */
    struct  SOFTTIMER_CONTROL_STRU *next;
    struct  SOFTTIMER_CONTROL_STRU *previous;
} SOFTTIMER_CONTROL_BLOCK_STRU;
typedef enum CHG_ON_OFF_LINE_STATUS_tag
{
    STATUS_OFFLINE = 0,//�����/usb��λ
    STATUS_ONLINE = 1, //�����/usb����λ
}CHG_ON_OFF_LINE_STATUS;

typedef enum CHARGE_TYPE_ENUM_tag
{
    CHARGE_TYPE_NON_CHECK = -1,          /* ��δ�жϳ������������� */
    CHARGE_TYPE_HW = 0,                      /* ��Ϊ�����             */
    CHARGE_TYPE_USB = 1,                  /* USB������������           */
    
}CHARGE_TYPE_ENUM;

typedef struct CHG_STATUS_STRU
{
    CHARGE_TYPE_ENUM chg_type;//���������
    CHG_ON_OFF_LINE_STATUS chg_on_off_line_flag;//�Ƿ���λ
}CHG_STATUS;

/* the number of timer control block */
extern UINT32  s_ulSoftTimerCtrlBlkNumber;

/* the number of free timer control block */
extern UINT32  s_ulSoftTimerIdleCtrlBlkNumber;

/* the start address of timer control block */
extern SOFTTIMER_CONTROL_BLOCK_STRU   *s_pSoftTimerCtrlBlk;

/* the start address of free timer control block list */
extern SOFTTIMER_CONTROL_BLOCK_STRU   *s_pSoftTimerIdleCtrlBlk;

/* the begin address of timer control block */
extern void  *s_pSoftTimerCtrlBlkBegin;

/* the end address of timer control block */
extern void   *s_pSoftTimerCtrlBlkEnd;




/***************************************��������*********************************/


/*****************************************************************************
 Function   : softtimer_create_timer
 Description: allocate and start a relative timer using callback function.
 Input      : ulLength       -- expire time. unit is millsecond
              ulParam        -- additional parameter to be pass to app
              ucMode         -- timer work mode
                                SOFTTIMER_LOOP  -- start periodically
                                SOFTTIMER_NOLOOP -- start once time
              TimeOutRoutine -- Callback function when time out
 Output     : phTm           -- timer pointer which system retuns to app
 Return     : SOFTTIMER_OK on success and errno on failure
 *****************************************************************************/

extern UINT32 softtimer_create_timer( HTIMER *phTm, UINT32 ulLength, 
                 UINT8 ucMode, SOFTTIMER_FUNC TimeOutRoutine, UINT32 ulParam, char *ulFileID, 
                 UINT32 ulLineNo);

#define SOFTTIMER_CREATE_TIMER(phTm, ulLength, ucMode, TimeOutRoutine, ulParam)\
    softtimer_create_timer((phTm), (ulLength), (ucMode), (TimeOutRoutine), (ulParam), __FILE__, __LINE__)



/*****************************************************************************
 Function   : softtimer_del_timer
 Description: stop a 32K relative timer which was previously started.
 Input      : phTm -- where store the timer to be stopped
              ulFileID --- File name, for debug;
              usLineNo --  line number, for debug;
 Return     :  SOFTTIMER_OK on success or errno on failure
 *****************************************************************************/

extern UINT32 softtimer_del_timer( HTIMER *phTm, char *ulFileID, UINT32 usLineNo );
#define SOFTTIMER_DEL_TIMER( phTm )\
    softtimer_del_timer( (phTm), __FILE__, __LINE__)


/*************************************************
  Function:      drv_sleep
  Description:   ʹ��always on ��ʱ��ʵ��sleep����
  Calls:
  Called By:
  Data Accessed: NONE
  Data Updated:  NONE
  Input:    ulLength ��sleep ��λ��ĿǰΪ10ms
            ulFileID:  File name, for debug;
            usLineNo:  Line number, for debug;
  Output:         
  Return: 
  Others:
          
************************************************/
extern void drv_sleep( UINT32 ulLength, char *ulFileID, UINT32 usLineNo);
#define  DRV_SLEEP(tick)   drv_sleep((tick), __FILE__, __LINE__)



#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _SOFT_TIMER_H */

