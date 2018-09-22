/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2010, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: SoftTimer.c                                                       */
/*                                                                           */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2011-12                                                            */
/*                                                                           */
/* Description: implement timer                                              */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date:    2011.12.20                                                              */
/*    Modification: Create this file                                         */
/*****************************************************************************/


#include <linux/sched.h>
#include <linux/kthread.h>
//#include <linux/err.h>
#include <linux/semaphore.h>
#include <linux/irq.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include<linux/netlink.h>
#include <linux/BSP_CHGC_DRV.h>
#include <mach/pwrctrl/BSP_PWC_COMMON.h>
#include "SoftTimer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#ifdef SOFTTIMER_DEBUG
    #define VAR_ATTR
#else
    #define VAR_ATTR  LOCAL
#endif

/* the tag of  softtimer_ctrl_blk_free */
#define THE_FIRST_SOFTTIMER_TAG         1    //expire
#define THE_SECOND_SOFTTIMER_TAG    2    //delete

#define SPECIAL_TIMER_START (1)
#define SPECIAL_TIMER_STOP (0)
#define CHG_FALG_MASK (0x0FFFFFFFC)
#define CHG_ON_OFF_LINE_MASK (0x3)
/*用于应用起休眠能唤醒的定时器*/
static HTIMER            s_special_softtimer = NULL;   
/*用于special timer超时以后投LCD的反对休眠票，和超时后投允许休眠票*/
static HTIMER            s_special_timer_vote_sleep = NULL;  

/***************************************全局变量*********************************/

/* the number of timer control block */
VAR_ATTR UINT32  s_ulSoftTimerCtrlBlkNumber;

/* the number of free timer control block */
VAR_ATTR UINT32  s_ulSoftTimerIdleCtrlBlkNumber;

/* the start address of timer control block */
VAR_ATTR SOFTTIMER_CONTROL_BLOCK_STRU   *s_pSoftTimerCtrlBlk;

/* the start address of free timer control block list */
VAR_ATTR SOFTTIMER_CONTROL_BLOCK_STRU   *s_pSoftTimerIdleCtrlBlk;

/* the begin address of timer control block */
VAR_ATTR void  *s_pSoftTimerCtrlBlkBegin;

/* the end address of timer control block */
VAR_ATTR void   *s_pSoftTimerCtrlBlkEnd;

/* the Min usage of timer */
VAR_ATTR UINT32   s_ulSoftTimerMinTimerIdUsed;

/* the head of the running timer list */
VAR_ATTR SOFTTIMER_CONTROL_BLOCK_STRU   *s_pSoftTimerHeadPtr = SOFTTIMER_NULL_PTR;

/*record the hard timer start value */
VAR_ATTR UINT32    s_ulSoftTimerStartValue = ELAPESD_TIME_INVAILD;

/* softtimer module init or not */
VAR_ATTR UINT8          s_ucSoftTimerModuleInit = 0;

/*the timer thread struct*/
struct task_struct  *softtimer_task;

struct semaphore s_SoftTimerSem;

/*记录最近创建定时器的10个文件名*/
char timer_rec[10][70]={0};
int rec_num = 0;


/***************************************函数声明*********************************/
void softtimer_show_used_info( void );
static void softtimer_add_timer_to_list( SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer);
void timer_load_set(UINT32 value,UINT8 timerId);
UINT32 timer_load_get(UINT8 timerId);
UINT32 timer_value_get(UINT8 timerId);
void timer_start(UINT32 mode,UINT8 timerId);
void timer_stop(UINT8 timerId);
UINT32  timer_input_clk_get(UINT8);
UINT32 TIMER_BASE(UINT8 u32ClkNum);
void softtimer_show_info( void );


/*****************************************************************************
 Function   : softtimer_interrupt_call_back
 Description: the entry will be called when HARD TIMER  interrupt occures
 Input      :  NO
 Return    :  NO
 Other      :  NO
 *****************************************************************************/
static irqreturn_t softtimer_interrupt_call_back( int len,void *ptr)
{    
    unsigned int readValue;    
    /*读取硬件定时器的中断状态*/
    PWRCTRL_REG_READ32(TIMER_BASE(ACPU_SOFTTIMER_ID) + CLK_REGOFF_INTSTATUS, readValue);
	
    /*printk("%s--the timer2 INTSTATUS register value is:0x%08x\n",__FUNCTION__,readValue);*/

	
    if(0 != readValue)
    {		
           /*清中断*/        
          
           PWRCTRL_REG_READ32(TIMER_BASE(ACPU_SOFTTIMER_ID) + CLK_REGOFF_CLEAR,readValue);
           /*printk("%s--the timer2 CLEAR register value is:0x%08x\n",__FUNCTION__,readValue);*/
         
	  
	 /* 释放信号量*/
          up( &s_SoftTimerSem );
	 
    } 
    return IRQ_HANDLED;
}

/*****************************************************************************
 Function     : softtimer_ctrl_blk_init
 Description: Init timer control block
 Input           : ulTimerCtrlBlkNumber -- TIMER number
 Return         : SOFTTIMER_OK(0)-- success
                         SOFTTIMER_ERR(1)--fail
 Other          :  NO
 *****************************************************************************/
static UINT32 softtimer_ctrl_blk_init(UINT32 ulTimerCtrlBlkNumber)
{
    UINT32  i;
    /*init timer blk number*/
    s_ulSoftTimerCtrlBlkNumber  = ulTimerCtrlBlkNumber;
    s_ulSoftTimerIdleCtrlBlkNumber = ulTimerCtrlBlkNumber;
	
 
	
    /*定时器控制块内存申请*/
    s_pSoftTimerCtrlBlk = (SOFTTIMER_CONTROL_BLOCK_STRU*)kmalloc(sizeof(SOFTTIMER_CONTROL_BLOCK_STRU)
		                            *s_ulSoftTimerCtrlBlkNumber, GFP_KERNEL);                 
   
    if(SOFTTIMER_NULL_PTR == s_pSoftTimerCtrlBlk)
    {  
        return (SOFTTIMER_ERR);
    }
	
    /*将所有定时器结构初始化为零*/
    /*modified for lint e665 */
    memset(s_pSoftTimerCtrlBlk, 0x0, 
            (s_ulSoftTimerCtrlBlkNumber * sizeof(SOFTTIMER_CONTROL_BLOCK_STRU)));
	
    /*软定时器块的起始和结束地址*/
    s_pSoftTimerIdleCtrlBlk  = s_pSoftTimerCtrlBlk;
    s_pSoftTimerCtrlBlkBegin = (void *)s_pSoftTimerCtrlBlk;
    s_pSoftTimerCtrlBlkEnd   = (void*)( (UINT32)(s_pSoftTimerCtrlBlk) +
        (s_ulSoftTimerCtrlBlkNumber * sizeof(SOFTTIMER_CONTROL_BLOCK_STRU)) );

    
    /*软定时器块的初始化*/
    for(i = 0; i < s_ulSoftTimerCtrlBlkNumber; i++)
    {
        s_pSoftTimerCtrlBlk[i].ulUsedFlag   = SOFTTIMER_NOT_USED;
        s_pSoftTimerCtrlBlk[i].TimerId      = i;
        s_pSoftTimerCtrlBlk[i].phTm         = SOFTTIMER_NULL_PTR;
        s_pSoftTimerCtrlBlk[i].CallBackFunc = SOFTTIMER_NULL_PTR;
        s_pSoftTimerCtrlBlk[i].previous     = SOFTTIMER_NULL_PTR;
	/*定时器的NEXT指向下一个定时器结构*/
        s_pSoftTimerCtrlBlk[i].next         = s_pSoftTimerCtrlBlk + i + 1;
    }

    /*初始化表尾的NEXT成员*/
    s_pSoftTimerCtrlBlk[s_ulSoftTimerCtrlBlkNumber-1].next   = SOFTTIMER_NULL_PTR;

    s_ulSoftTimerMinTimerIdUsed = s_ulSoftTimerCtrlBlkNumber;

    /*初始化信量不可用*/
    sema_init(&s_SoftTimerSem,SEM_EMPTY);
   
    return (SOFTTIMER_OK);
}


/*****************************************************************************
 Function     : softtimer_start_hard_timer
 Description: start hard timer
 Input           : value -- timer's value.uint is 32K cycle.
 Return         : NO
 Other           : NO
 *****************************************************************************/
static void softtimer_start_hard_timer( UINT32 ulvalue )
{
    
    s_ulSoftTimerStartValue = ulvalue;

    //printk("The hard timer start value  is  : %d  \n",s_ulSoftTimerStartValue);
    ulvalue = (ulvalue > 2) ? ulvalue : 2;

    /* f00164371. if Not stop the timer, maybe the timer will be expired before start .
       and we want to start a new timer, so, stop it first.
    */
    /*printk("strat value  is :0x%08x\n",ulvalue);*/
    timer_stop(ACPU_SOFTTIMER_ID); 
    timer_load_set(ulvalue,ACPU_SOFTTIMER_ID);
    timer_start(TIMER_ONCE_COUNT,ACPU_SOFTTIMER_ID);

}

/*****************************************************************************
 Function     : softtimer_stop_hard_timer
 Description: stop hard timer
 Input          :  NO
 Return        :  NO
 Other          :  NO
 *****************************************************************************/
static void softtimer_stop_hard_timer(void )
{
    timer_stop(ACPU_SOFTTIMER_ID);
    s_ulSoftTimerStartValue = ELAPESD_TIME_INVAILD;
}

/*****************************************************************************
 Function     :  softtimer_hard_timer_elapsed_time
 Description:  get the elapsed time from hard timer
 Input          :  NO
 Return        :  the elapsed timer COUNT from the latest hard timer is started
 Other          :  NO
 *****************************************************************************/
static UINT32 softtimer_hard_timer_elapsed_time(void)
{
    UINT32 ulTempValue = 0;
     

    if ( ELAPESD_TIME_INVAILD == s_ulSoftTimerStartValue )
    {
        return 0;
    }
	
   
    /*PWRCTRL_REG_READ32(TIMER_BASE(ACPU_SOFTTIMER_ID) + CLK_REGOFF_CTRL,result);*/
    /*printk("the timer2 control register value is :0x%08x\n",result);*/
    

    ulTempValue = timer_value_get(ACPU_SOFTTIMER_ID);
	
    
    /*PWRCTRL_REG_READ32(TIMER_BASE(ACPU_SOFTTIMER_ID) + CLK_REGOFF_CTRL,result);
    printk("the timer2 control register value is :0x%08x\n",result);*/
    

    return ((s_ulSoftTimerStartValue <= ulTempValue) ? 0 : (s_ulSoftTimerStartValue - ulTempValue));
    
}

/*****************************************************************************
 Function     : softtimer_ctrl_blk_get
 Description: allocate a block
 Input           : FileId:     File Name, For debug
                        LineNo:   line number, for debug
 Return        : address
 Other          : NO
 *****************************************************************************/
static SOFTTIMER_CONTROL_BLOCK_STRU *softtimer_ctrl_blk_get(char* ulFileID, 
                                             UINT32 ulLineNo )
{
    SOFTTIMER_CONTROL_BLOCK_STRU  *pst_temp_Timer_Ctrl_Ptr;

     /*无空闲时钟块，返回空指针*/
    if( 0 == s_ulSoftTimerIdleCtrlBlkNumber )
    {
        printk("# no Idle timer  blk!!!!.\r\n");
        
        return ((SOFTTIMER_CONTROL_BLOCK_STRU*)SOFTTIMER_NULL_PTR);
    }
    else
    {
        /*空软定时器的数量减一*/
        s_ulSoftTimerIdleCtrlBlkNumber--;
        pst_temp_Timer_Ctrl_Ptr = s_pSoftTimerIdleCtrlBlk;
        pst_temp_Timer_Ctrl_Ptr->ulUsedFlag  = SOFTTIMER_USED;
        s_pSoftTimerIdleCtrlBlk = s_pSoftTimerIdleCtrlBlk->next;
    }


    /* record the usage of timer control block */
    if ( s_ulSoftTimerIdleCtrlBlkNumber < s_ulSoftTimerMinTimerIdUsed )
    {  
        s_ulSoftTimerMinTimerIdUsed = s_ulSoftTimerIdleCtrlBlkNumber;
    }

    /*初始化获取定时器结构的成员*/
    pst_temp_Timer_Ctrl_Ptr->next = SOFTTIMER_NULL_PTR;
    pst_temp_Timer_Ctrl_Ptr->previous = SOFTTIMER_NULL_PTR;
    strncpy(pst_temp_Timer_Ctrl_Ptr->ulFileID, ulFileID, 70);
    //pst_temp_Timer_Ctrl_Ptr->ulFileID = ulFileID;
    pst_temp_Timer_Ctrl_Ptr->ulLineNo = (UINT32)ulLineNo;

    /*printk("AFTER BLK GET-- IDLE number is:%d   \n",s_ulSoftTimerIdleCtrlBlkNumber);*/
   
    return pst_temp_Timer_Ctrl_Ptr;
}

/*****************************************************************************
 Function     : softtimer_ctrl_blk_give_back
 Description: free the timer blk
 Input          :  pst_Timer:   
                       ucTag:  free flag
 Return        : NO
 Other          : NO
 *****************************************************************************/
static void softtimer_ctrl_blk_give_back(SOFTTIMER_CONTROL_BLOCK_STRU *pst_Timer, UINT8 ucTag)
{

    pst_Timer->ulUsedFlag      = SOFTTIMER_NOT_USED;
    pst_Timer->Reserved[0]    = ucTag;
    pst_Timer->next              = s_pSoftTimerIdleCtrlBlk;
    s_pSoftTimerIdleCtrlBlk   = pst_Timer;
	
    //add the free timer blk number
    s_ulSoftTimerIdleCtrlBlkNumber++;

}


/*****************************************************************************
 Function     : softtimer_ctrl_blk_free
 Description: free a block
 Input           : pst -- address
                        ucTag -- where call this function.this should be deleted when release
 Return         : 
 Other           : 
 *****************************************************************************/
static UINT32 softtimer_ctrl_blk_free(SOFTTIMER_CONTROL_BLOCK_STRU *pst, UINT8 ucTag )
{

    if ( ((UINT32)pst < (UINT32)s_pSoftTimerCtrlBlkBegin)
        || ((UINT32)pst > (UINT32)s_pSoftTimerCtrlBlkEnd) )
    {
        printk("##free control block address check error.##\r\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL, "# #free control block address check error.##\r\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }

    if ( SOFTTIMER_NOT_USED == pst->ulUsedFlag )
    {
        printk("# #free timer block repection. Previous is %d. Now is %d.##\r\n",
            pst->Reserved[0], ucTag);
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"##free timer block repection. Previous is %d. Now is %d.##\r\n",
         //   pst->Reserved[0], ucTag,0,0,0,0); 

        return SOFTTIMER_ERRNO_FREE_TIMERNOTUSED;
    }
    
    softtimer_ctrl_blk_give_back(pst, ucTag);
    /*printk("AFTER BLK FREE  IDLE number is:%d   \n",s_ulSoftTimerIdleCtrlBlkNumber);*/

	
    return SOFTTIMER_OK;
}

/*****************************************************************************
 Function     : softtimer_new_timer
 Description: allocate a timer blk
 Input          :  ulLength-expire time
                        TimeOutRoutine-expire call back
 Return      : 
 Other       :  
 *****************************************************************************/
static UINT32 softtimer_new_timer(HTIMER *phTm, UINT32 ulLength, UINT8 ucMode, 
    SOFTTIMER_FUNC TimeOutRoutine, UINT32 ulParam, char* ulFileID, UINT32 ulLineNo)
{
    SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer;

    /*get a free timer ctrl blk*/
    pst_Timer = softtimer_ctrl_blk_get(ulFileID, ulLineNo);
        
    if(SOFTTIMER_NULL_PTR == pst_Timer)
    {
        printk("###softtimer_ctrl_blk_get NULL.###\r\n"); 
       // BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"###softtimer_ctrl_blk_get NULL.###\r\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }
    else
    {  
        /*init the timer blk param*/
        pst_Timer->Para = ulParam;
        pst_Timer->Mode = ucMode;
        pst_Timer->phTm = phTm;  
        pst_Timer->TimeOutValueInMilliSeconds  = ulLength;
	 /*记录调用创建的文件*/
        if(rec_num<10)
	 {
            strncpy(timer_rec[rec_num], ulFileID, 70);
	      rec_num++;
        }
	  else
	  {
	      rec_num = 0;
	  }
	  	          
        /*("ulength is: %d--SOFTTIMER_MAX_LENGTH is %d \n",ulLength,SOFTTIMER_MAX_LENGTH);  */
     
        if((ulLength) < (SOFTTIMER_MAX_LENGTH))  
        {  
            pst_Timer->TimeOutValueInCycle = (UINT32)((timer_input_clk_get(ACPU_SOFTTIMER_ID) * ulLength)/1000);
        }
        else /* 防止乘法溢出 */ 
        {
            pst_Timer->TimeOutValueInCycle = (UINT32)(timer_input_clk_get(ACPU_SOFTTIMER_ID) * (ulLength/1000));
        }

        /*printk("pst_Timer->TimeOutValueInCycle is: %d \n",pst_Timer->TimeOutValueInCycle);  */ 
 
        /*add the expire callback*/
        pst_Timer->CallBackFunc = TimeOutRoutine;     
        /*return the use timerID*/
        *(pst_Timer->phTm) = (HTIMER)(&(pst_Timer->TimerId));
		
        /*add the timer to the running timer list*/
        softtimer_add_timer_to_list( pst_Timer );
        return SOFTTIMER_OK;	
    }
}

/*****************************************************************************
 Function     : softtimer_loop_mode_handler
 Description: soft timer loop mode handle
 Input           : pst_head_ptr: the node to add to the list
 Return         : void
 Other           :
 *****************************************************************************/
static void softtimer_loop_mode_handler(SOFTTIMER_CONTROL_BLOCK_STRU  *pst_head_ptr)
{   
    UINT32 returnval=0;
    if (NULL == pst_head_ptr)
    {
         //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"pst_head_ptr is NULL!!\n",0,0,0,0,0,0);
         printk("pst_head_ptr is NULL!!\n");
		 return;
    }
    if( SOFTTIMER_LOOP == pst_head_ptr->Mode )
    {
        returnval=softtimer_new_timer(pst_head_ptr->phTm, 
                                pst_head_ptr->TimeOutValueInMilliSeconds,
                                pst_head_ptr->Mode, 
                                pst_head_ptr->CallBackFunc, 
                                pst_head_ptr->Para, 
                                pst_head_ptr->ulFileID,
                                pst_head_ptr->ulLineNo);
        if (returnval !=SOFTTIMER_OK )
        {
             printk( "### create loop new timer error.###\r\n");
	    //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"### create new timer  error.###\r\n",0,0,0,0,0,0);
        }       
    }
  
}


/*****************************************************************************
 Function     : softtimer_expire_node_handler
 Description: soft timer expire timer handle
 Input           : pst_TimerCtrlBlkexpired: the node of the list to handle;
 Return         : void
 Other           :
 *****************************************************************************/
static void softtimer_expire_node_handler(SOFTTIMER_CONTROL_BLOCK_STRU  *pst_TimerCtrlBlkexpired)
{
    SOFTTIMER_CONTROL_BLOCK_STRU     *pst_head_ptr;
    UINT32 value = 0;
    /*printk("start to handle the expire timer  callback!!!!\n");*/
    
    while ( SOFTTIMER_NULL_PTR != pst_TimerCtrlBlkexpired )
    {
        pst_head_ptr = pst_TimerCtrlBlkexpired;
        /*run the exipire timer callback*/
        pst_TimerCtrlBlkexpired->CallBackFunc(pst_TimerCtrlBlkexpired->Para);

        pst_TimerCtrlBlkexpired = pst_TimerCtrlBlkexpired->next;
		
        /*free the expire timer blk*/
        value = softtimer_ctrl_blk_free(pst_head_ptr, THE_FIRST_SOFTTIMER_TAG);
    }
}/*lint !e550*/

/*****************************************************************************
 Function     : softtimer_task_func
 Description: Soft timer task entry
 Input           : data: not used
 Return         : void
 Other          :
 *****************************************************************************/
int  softtimer_task_func( void * data )
{
    SOFTTIMER_CONTROL_BLOCK_STRU     *pst_head_ptr;
    /* the timer control which expire */
    SOFTTIMER_CONTROL_BLOCK_STRU     *pst_TimerCtrlBlkCurrent;
    /* the timer head control which expire */
    SOFTTIMER_CONTROL_BLOCK_STRU     *pst_TimerCtrlBlkexpired = SOFTTIMER_NULL_PTR;
    SOFTTIMER_CONTROL_BLOCK_STRU     *pst_TimerCtrlBlkexpiredTail = SOFTTIMER_NULL_PTR;

    unsigned long        keylock = 0;
    UINT32              ulTempCount;
    UINT32              ulElapsedCycles;    

    for( ; ; )
    {
	  /*获取信号量*/
        down(&s_SoftTimerSem);

        local_irq_save(keylock);
        
        ulElapsedCycles = s_ulSoftTimerStartValue;
        s_ulSoftTimerStartValue = ELAPESD_TIME_INVAILD;
       

        pst_head_ptr = s_pSoftTimerHeadPtr;

        /*解决参数类型不对的问题-- linux内核*/
        /*lint -e516*/
        if ( pst_head_ptr != SOFTTIMER_NULL_PTR)
        {
            /* sub timer value */
            pst_head_ptr->TimeOutValueInCycle -= ulElapsedCycles;

            ulTempCount = 0;

            /* check the left timer */
            while ( ( SOFTTIMER_NULL_PTR != pst_head_ptr )
                && ( 0 == pst_head_ptr->TimeOutValueInCycle ) )
            {
                ulTempCount++;  
                if ( s_ulSoftTimerCtrlBlkNumber < ulTempCount )
                {
                    local_irq_restore(keylock);
                    printk("ulTempCount number :%d \n",ulTempCount);
                    //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"ulTempCount number :%d \n",ulTempCount,0,0,0,0,0);
					return 0;
                }

                pst_TimerCtrlBlkCurrent = pst_head_ptr;

                *(pst_head_ptr->phTm) = SOFTTIMER_NULL_PTR;
                /*re-run the loop mode timer*/
                softtimer_loop_mode_handler(pst_head_ptr);

                pst_head_ptr = pst_TimerCtrlBlkCurrent->next;

                pst_TimerCtrlBlkCurrent->next = SOFTTIMER_NULL_PTR;
                /*add all expire timer to the expire list*/
                if ( SOFTTIMER_NULL_PTR == pst_TimerCtrlBlkexpired )
                {
                    pst_TimerCtrlBlkexpired = pst_TimerCtrlBlkCurrent;
                    pst_TimerCtrlBlkexpiredTail = pst_TimerCtrlBlkCurrent;
                }
                else
                {
                    if( SOFTTIMER_NULL_PTR != pst_TimerCtrlBlkexpiredTail )
                    {
                        pst_TimerCtrlBlkexpiredTail->next = pst_TimerCtrlBlkCurrent;
                    }
                    pst_TimerCtrlBlkexpiredTail = pst_TimerCtrlBlkCurrent;
					
                }

                s_pSoftTimerHeadPtr = pst_head_ptr;
            }

            if ( SOFTTIMER_NULL_PTR != s_pSoftTimerHeadPtr )
            {  
                s_pSoftTimerHeadPtr->previous = SOFTTIMER_NULL_PTR;
                softtimer_start_hard_timer(s_pSoftTimerHeadPtr->TimeOutValueInCycle);
            }    

            /*run the exipre timer callback*/
            softtimer_expire_node_handler(pst_TimerCtrlBlkexpired);            
            pst_TimerCtrlBlkexpired = SOFTTIMER_NULL_PTR;
            local_irq_restore(keylock);
        }
	  else
	  {
	      local_irq_restore(keylock);
	  }
	  /*lint +e516*/
    }
	return 0;   
}

/*****************************************************************************
 Function     : softtimer_module_start
 Description: soft timer module start
 Input           : no
 Return        : SOFTTIMER_ERR or SOFTTIMER_OK;
 Other           :
 *****************************************************************************/
UINT32 softtimer_module_start(void)
{
    if(SOFTTIMER_ERR == softtimer_ctrl_blk_init(SOFTTIMER_CTRLBLK_NUM))
    {
        printk("!!softtimer ctrl block init error!! -%s- \n", __FUNCTION__);
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"!!softtimer ctrl block init error!! \n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }   

    s_ucSoftTimerModuleInit = 1;
	
    printk( "softtimer_module_start_success-,%d-- >>>>>>>>>>>>>>\n",s_ucSoftTimerModuleInit);
    

    return SOFTTIMER_OK;
}

/*****************************************************************************
 Function     : softtimer_add_timer_to_list
 Description: add a timer to list
 Input          : pst_Timer -- the tiemr's adddress
 Return       : void
 Other        :
 *****************************************************************************/
static void softtimer_add_timer_to_list( SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer)
{
    SOFTTIMER_CONTROL_BLOCK_STRU  *temp_Ptr;
    SOFTTIMER_CONTROL_BLOCK_STRU  *pre_temp_Ptr;
    UINT32 ul_ElapsedCycles = 0;

    unsigned int intstatus;
    
    /*读取硬件定时器的中断状态*/
    PWRCTRL_REG_READ32(TIMER_BASE(ACPU_SOFTTIMER_ID) + CLK_REGOFF_INTSTATUS, intstatus);

    if ( SOFTTIMER_NULL_PTR == s_pSoftTimerHeadPtr )
    {
        /*若当前链表为空，则插入表头*/
        s_pSoftTimerHeadPtr = pst_Timer;
    }
    else
    {      
        if ( 0 != intstatus )
        {
            pst_Timer->TimeOutValueInCycle += s_pSoftTimerHeadPtr->TimeOutValueInCycle;           
        }
        else
        {
           ul_ElapsedCycles = softtimer_hard_timer_elapsed_time();
           /*printk( "ul_ElapsedCycles  is : %d  !!!\n",ul_ElapsedCycles);*/

           pst_Timer->TimeOutValueInCycle += ul_ElapsedCycles;
    
        }  
        
        /*  find the location to insert */
	/*按超时时间从小到大插入定时器*/
        pre_temp_Ptr = s_pSoftTimerHeadPtr;
        temp_Ptr = pre_temp_Ptr;
        while ( (temp_Ptr != SOFTTIMER_NULL_PTR)
            && (pst_Timer->TimeOutValueInCycle >= temp_Ptr->TimeOutValueInCycle) )
        {
            pst_Timer->TimeOutValueInCycle -= temp_Ptr->TimeOutValueInCycle;
            pre_temp_Ptr = temp_Ptr;
            temp_Ptr = temp_Ptr->next;
        }
		
        /* insert timer < head timer-新定时器超时时间最短插入表头*/
        if ( temp_Ptr == s_pSoftTimerHeadPtr )
        {
            pst_Timer->next = s_pSoftTimerHeadPtr;
            s_pSoftTimerHeadPtr = pst_Timer;
        }
        else
        {
            pst_Timer->next = temp_Ptr;
            pre_temp_Ptr->next = pst_Timer;
            pst_Timer->previous = pre_temp_Ptr;
        }

        if ( temp_Ptr != SOFTTIMER_NULL_PTR )
        {
            temp_Ptr->TimeOutValueInCycle
                = temp_Ptr->TimeOutValueInCycle - pst_Timer->TimeOutValueInCycle;
            temp_Ptr->previous = pst_Timer;
        }
    }


    /* -restart hard timer */
    /*
    s_pSoftTimerHeadPtr->TimeOutValueInCycle-=ul_ElapsedCycles;
    softtimer_start_hard_timer(s_pSoftTimerHeadPtr->TimeOutValueInCycle);
    */

    if ( s_pSoftTimerHeadPtr == pst_Timer)
    {
        /* judge timer value when the new timer at head */
        pst_Timer->TimeOutValueInCycle -= ul_ElapsedCycles;
        softtimer_start_hard_timer(pst_Timer->TimeOutValueInCycle);
    }
    /*printk( "add a new timer to the running list!!!!!!!!!!!\n");*/
    
}

/*****************************************************************************
 Function   : softtimer_check_timer
 Description: check a relative timer whether or not.
 Input          : phTm -- where store the timer to be stopped
                       ulFileID -- file name, for debug;
                       usLineNo -- line number, for debug;
 Output     : ulTimerID -- timer ID
 Return     :  SOFTTIMER_OK on success or errno on failure
 *****************************************************************************/
static UINT32 softtimer_check_timer( HTIMER  *phTm, UINT32 *ulTimerID,
                           char* ulFileID, UINT32 usLineNo )
{
    SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer;
    UINT32               ul_TimerId;
	
    
	
    /*判断phTm指向的地址范围*/
    if ( ((UINT32)*phTm >= (UINT32)s_pSoftTimerCtrlBlkBegin)
        && ((UINT32)*phTm < (UINT32)s_pSoftTimerCtrlBlkEnd) )
    {
        ul_TimerId = *((UINT32 *)(*phTm));
        /*printk("the ul_timerID is : %d   \n",ul_TimerId);*/
      
	  
        if(ul_TimerId <= (s_ulSoftTimerCtrlBlkNumber -1))
        {
            pst_Timer = &s_pSoftTimerCtrlBlk[ul_TimerId];
            /*printk("pst_Timer  is  0x%8x <<<<<<<\n",pst_Timer);*/
	
            if ( phTm == pst_Timer->phTm )
            {
                *ulTimerID = ul_TimerId;
                return SOFTTIMER_OK;
            }
            else
            {  /*phtm 指向不匹配*/
                printk("###the phTm ID is not equal to the struct ID ###\n\n");
	      // BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"###the phTm ID is not equal to the struct ID ###\n\n",0,0,0,0,0,0);
            }
        }
        else
        {  /*ID 范围不对*/
            printk( "###this softtimer_check_timer pst_Timer ID is error.###\r\n");
	 //  BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"###this softtimer_check_timer pst_Timer ID is error.###\r\n",0,0,0,0,0,0);
        }
    }
    else
    {   /*地址范围不对*/
         printk( "### OSA softtimer_check_timer phTm address rang is error.###\r\n");
        // BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"### OSA softtimer_check_timer phTm address rang is error.###\r\n",0,0,0,0,0,0);
    }
	
    return SOFTTIMER_ERR;
}

/*****************************************************************************
 Function     : softtimer_del_timer_from_list
 Description: del a timer from list
 Input           : pst_Timer -- the timer's address
 Return         : void
 Other          :
 *****************************************************************************/
static void softtimer_del_timer_from_list( SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer)
{
    if ( pst_Timer == s_pSoftTimerHeadPtr )
    {
        s_pSoftTimerHeadPtr = pst_Timer->next;
        if ( SOFTTIMER_NULL_PTR != s_pSoftTimerHeadPtr )
        {
            s_pSoftTimerHeadPtr->previous = SOFTTIMER_NULL_PTR;
        }
    }
    else
    {
        (pst_Timer->previous)->next = pst_Timer->next;
        if ( SOFTTIMER_NULL_PTR != pst_Timer->next )
        {
            (pst_Timer->next)->previous = pst_Timer->previous;
        }
    }

    /* adjust the time_val after this timer */
    if ( pst_Timer->next != NULL )
    {
        pst_Timer->next->TimeOutValueInCycle += pst_Timer->TimeOutValueInCycle;
    }
    
    /* Stop hard timer if no timer */
    if ( SOFTTIMER_NULL_PTR == s_pSoftTimerHeadPtr )
    {
        softtimer_stop_hard_timer();
    }
}

/*****************************************************************************
 Function     : softtimer_del_timer
 Description: stop a 32K relative timer which was previously started.
 Input          : phTm -- where store the timer to be stopped
                       ulFileID --- File name, for debug;
                       usLineNo --  line number, for debug;
 Return        : SOFTTIMER_OK on success or errno on failure
 *****************************************************************************/
UINT32 softtimer_del_timer( HTIMER *phTm, char * ulFileID, UINT32 usLineNo )
{

    UINT32  ul_TimerId = 0;
    SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer;
    unsigned long  keylock = 0;

    if (0 == s_ucSoftTimerModuleInit)
    {
        printk( "softtimer uninit!!!!!!!!!!!\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"softtimer uninit!!!\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }
    
    if( SOFTTIMER_NULL_PTR == phTm )
    {  
        printk( "# para phTm  is NULL!\n");       
        return SOFTTIMER_ERR;
    }
    if( SOFTTIMER_NULL_PTR == *phTm)
    {  
        printk( "# softtimer_del_timer Error, timer Id is NULL!\n");       
        return SOFTTIMER_ERRNO_STOP_TIMERINVALID;
    }

    local_irq_save( keylock );

    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
	
    /*检测要删除的定时器是否有效*/
    if ( SOFTTIMER_OK != softtimer_check_timer(phTm, &ul_TimerId, ulFileID, usLineNo ) )
    {
        local_irq_restore( keylock);
        printk( "# softtimer_del_timer Error, phTm is : %x \n", (int)*phTm);
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"# softtimer_del_timer Error, phTm is : %x \n", (int)*phTm,0,0,0,0,0);
        return(SOFTTIMER_ERRNO_STOP_TIMERINVALID);
    }

    pst_Timer = &s_pSoftTimerCtrlBlk[ul_TimerId];

    /*先判断一下是否在使用,add by f00164371*/
    if ( SOFTTIMER_USED == pst_Timer->ulUsedFlag )  
    {
        softtimer_del_timer_from_list( pst_Timer );
        *(pst_Timer->phTm) = SOFTTIMER_NULL_PTR;
    }
    else
    {
        printk( "# softtimer_del_timer Error, the timer is not used!\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"# softtimer_del_timer Error, the timer is not used!\n",0,0,0,0,0,0);
        local_irq_restore( keylock );
        return (SOFTTIMER_ERRNO_STOP_TIMERINVALID);
    }    

    /*softtimer_ctrl_blk_free(pst_Timer, THE_SECOND_SOFTTIMER_TAG);*/
    /*从运行定时器链删除后加入空闲链中*/
    softtimer_ctrl_blk_give_back(pst_Timer, THE_SECOND_SOFTTIMER_TAG);

    local_irq_restore( keylock);
	/*lint +e516*/
	
    return SOFTTIMER_OK;
    
}
//EXPORT_SYMBOL(softtimer_del_timer);

/*************************************************
  Function:        drv_sleep_call_back
  Description:   drv_sleep功能的定时器回调函数
  Calls:
  Called By:
  Data Accessed: NONE
  Data Updated:  NONE
  Input:    arg:  
  Output:         
  Return: 
  Others:         
************************************************/
void drv_sleep_call_back(UINT32  arg )
{
    struct semaphore drvsleep_sem = *((struct semaphore*)(arg));
    /*释放信号量*/
    up( &drvsleep_sem ); 
    /*printk("####exit  the drv_sleep_call_back()!!!\n");*/
    
}


/*************************************************
  Function:      drv_sleep
  Description:   使用always on 定时器实现sleep功能
  Calls:
  Called By:
  Data Accessed: NONE
  Data Updated:  NONE
  Input:    ulLength ：sleep 单位，目前为10ms
            ulFileID:  File name, for debug;
            usLineNo:  Line number, for debug;
  Output:         
  Return: 
  Others:
          
************************************************/
void drv_sleep( UINT32 ulLength, char* ulFileID, UINT32 usLineNo)
{
    struct semaphore drvsleep_sem;
    UINT32      status;
    HTIMER timerId = SOFTTIMER_NULL_PTR;

    /*printk("####enter the drv_sleep()!!!\n");*/
   
    if (0 == s_ucSoftTimerModuleInit)
    {
        printk("##softtimer module  uninit!!!!###\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"##softtimer module  uninit!!!!###\n",0,0,0,0,0,0);
		return;
    }

    if( 0 == ulLength )
    {
        printk("###the sleep time is 0 !!!\n");
        return ;
    }
      /*定时时间   */ 
    ulLength *= 10;  
	  
   /*信号量初始化为不可用状态*/
    sema_init(&drvsleep_sem,  SEM_EMPTY);

    /*创建定时器*/
    status= softtimer_create_timer( &timerId, ulLength, SOFTTIMER_NO_LOOP,drv_sleep_call_back, (UINT32)&drvsleep_sem, ulFileID, usLineNo);
    if( SOFTTIMER_OK == status )
    {
        down(&drvsleep_sem);            
    }   
    /*printk("#####exit the drv_sleep()!!!\n");*/
    //BSP_TRACE(BSP_LOG_LEVEL_INFO, BSP_MODU_PWRCTRL,"##exit the drv_sleep()!!!\n",0,0,0,0,0,0);
}

//EXPORT_SYMBOL(drv_sleep);
/*****************************************************************************
 Function     : softtimer_create_timer
 Description: allocate and start a relative timer using callback function.
 Input           : ulLength       -- expire time. unit is millsecond
                        ulParam        -- additional parameter to be pass to app
                        ucMode         -- timer work mode
                        SOFTTIMER_LOOP  -- start periodically
                        SOFTTIMER_NO_LOOP -- start once time
                        TimeOutRoutine -- Callback function when time out
 Output       : phTm           -- timer pointer which system retuns to app
 Return        : SOFTTIMER_OK on success and errno on failure
 *****************************************************************************/
UINT32 softtimer_create_timer( HTIMER *phTm, UINT32 ulLength, UINT8 ucMode, 
    SOFTTIMER_FUNC TimeOutRoutine, UINT32 ulParam, char* ulFileID, UINT32 ulLineNo )
{
    SOFTTIMER_CONTROL_BLOCK_STRU  *pst_Timer;
    UINT32                       ul_TimerId = 0;
    unsigned long  keylock = 0;
 
    if (0 == s_ucSoftTimerModuleInit)
    {
        printk( "####softtimer module not init####\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"####softtimer module not init####\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }
	
    if(SOFTTIMER_NULL_PTR == phTm)
    {
        printk( "### para phTm is NULL .###\r\n");
	  return SOFTTIMER_ERR;	
    }
	
    if(SOFTTIMER_NULL_PTR == TimeOutRoutine)
    {

        printk("### the TimeOutRoutine  is NULL.###\r\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"### the TimeOutRoutine  is NULL.###\r\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }
    /*定时时间为0 */
    if(0 ==ulLength)
    {
        printk( "### the timer expire value is 0.###\r\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"### the timer expire value is 0.###\r\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }
    /*定时器工作模式判断*/
    if((SOFTTIMER_NO_LOOP != ucMode)&&(SOFTTIMER_LOOP !=ucMode))
    {
        printk( "### the timer mode  is  error.###\r\n");
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"### the timer mode  is  error.###\r\n",0,0,0,0,0,0);
        return SOFTTIMER_ERR;
    }		

    local_irq_save(keylock);
	
    /* stop the timer if exists -用于接收创建定时器ID的指针非空*/
    if( SOFTTIMER_NULL_PTR != *phTm )
    {
        if ( SOFTTIMER_OK == softtimer_check_timer(phTm, &ul_TimerId, ulFileID, ulLineNo) )
        {
            pst_Timer = &s_pSoftTimerCtrlBlk[ul_TimerId];

           /*先判断一下是否在使用是则删除*/
            if ( SOFTTIMER_USED == pst_Timer->ulUsedFlag )  
            {
                softtimer_del_timer_from_list( pst_Timer );
                *(pst_Timer->phTm) = SOFTTIMER_NULL_PTR;
                softtimer_ctrl_blk_give_back(pst_Timer, THE_SECOND_SOFTTIMER_TAG);
            }          
        }
    }   

    /*申请新的定时器结构*/
    if(SOFTTIMER_OK!=softtimer_new_timer(phTm, ulLength, ucMode, TimeOutRoutine, ulParam, ulFileID,ulLineNo))
    {
        printk( "### create new timer  error.###\r\n");  
         // BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"### create new timer  error.###\r\n",0,0,0,0,0,0);  
        return SOFTTIMER_ERR;
    }

    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
    local_irq_restore(keylock);
	/*lint +e516*/

    return(SOFTTIMER_OK);
}

EXPORT_SYMBOL(softtimer_create_timer);
EXPORT_SYMBOL(softtimer_del_timer);


/*****************************************************************************
 Function     : softtimer_show_used_timerinfo
 Description: print the usage info of 32K timer's control block
 Input          : void
 Return        : void
 Other         :
 *****************************************************************************/
void softtimer_show_used_info( void )
{
    SOFTTIMER_CONTROL_BLOCK_STRU     *pstTimer;
    unsigned long  keylock = 0;

    printk("# #softtimer_show_used_info:\n");
    
    local_irq_save(keylock);

    pstTimer = s_pSoftTimerHeadPtr;
    while( SOFTTIMER_NULL_PTR != pstTimer )
    {
        printk("TimerId:%u, ulUsedFlag:%u, Mode:%d, CallBackFunc:%#x, TimeOutValueInMilliSeconds:%u,"\
            "TimeOutValueInCycle:%#x,File:%s,Line:%u \n",
               (UINT32)pstTimer->TimerId,
               (UINT32)pstTimer->ulUsedFlag,
               (UINT32)pstTimer->Mode,
               (UINT32)pstTimer->CallBackFunc,                          
               (UINT32)pstTimer->TimeOutValueInMilliSeconds,
               (UINT32)pstTimer->TimeOutValueInCycle,
               pstTimer->ulFileID,
               (UINT32)pstTimer->ulLineNo);        

        pstTimer = pstTimer->next;
    }
    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
    local_irq_restore(keylock);   
    /*lint +e516*/
	return;
}

void softtimer_show_info( void )
{
    unsigned long  keylock = 0;
    int i=0;

    printk("# #softtimer_show_info:\n");
    
    local_irq_save(keylock);
    for(i=0;i<10;i++)
    {
        printk("call file :%s \n",timer_rec[i]);
    }
    /*解决参数类型不对的问题-- linux内核*/
    /*lint -e516*/
    local_irq_restore(keylock);   
	/*lint +e516*/
    return;
}

/*****************************************************************************
                                                   硬件定时器的操作函数
 *****************************************************************************/

void timer_load_set(UINT32 value,UINT8 timerId)
{
    /*写计数初始值*/
    PWRCTRL_REG_WRITE32(TIMER_BASE(timerId) + CLK_REGOFF_LOAD,value);
}

UINT32 timer_load_get(UINT8 timerId)
{   
    UINT32 result;
    
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_LOAD,result);
    return result;
}

UINT32 timer_value_get(UINT8 timerId)
{
    UINT32 readValueTmp;
    UINT32 i;   

    /*将定时器的当前值锁定*/
    
    PWRCTRL_SET_BIT32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL,CLK_DEF_TC_LOCK);  
    
    for (i = 0; i < 10000; i++)
    {
        
	  PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL,readValueTmp);
        
        /*printk("timer_value_get():CTRL resigter: %08x \n ",readValueTmp);*/
	  if(!(readValueTmp & CLK_DEF_TC_LOCK))
        {
            break;
        }
    }
	
    if (readValueTmp & CLK_DEF_TC_LOCK)
    {        
        printk( "!!lock the timer current value to lock currentvalue reister: %08x  \n ",readValueTmp );
        //BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_PWRCTRL,"lock the timer current value to lock currentvalue resigter: %08x \n ",
        //        readValueTmp,0,0,0,0,0);
        return 0;
    }


    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_VALUE,readValueTmp);
    /*printk("timer_value_get():readValueTmp: %08x \n ",readValueTmp);*/
    
    return readValueTmp;
}

void timer_start(UINT32 mode,UINT8 timerId)
{
    UINT32 result;

    
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_CLEAR,result);

    if (TIMER_ONCE_COUNT == mode) /*Free mode */
    {
        for(;;)  //b00198513 Modified for pclint e716          
        {
            /*modified for lint e665 */
            PWRCTRL_REG_WRITE32((TIMER_BASE(timerId) + CLK_REGOFF_CTRL), (CLK_DEF_ENABLE & ~CLK_DEF_TC_PERIODIC));
            
            PWRCTRL_REG_READ32((TIMER_BASE(timerId) + CLK_REGOFF_CTRL),result);
            if(0x1 == (result & 0x1))
            {
                break;
            }
        }
    }
    else  /*User define mode*/
    {
	
        PWRCTRL_REG_WRITE32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL, CLK_DEF_ENABLE);
    }
}

void timer_stop(UINT8 timerId)
{
    
    PWRCTRL_REG_WRITE32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL, CLK_DEF_DISABLE);
}


void timer_int_mask(UINT8 timerId)
{
    UINT32 result;
   
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL,result);

    /*modified for lint e665 */
    PWRCTRL_REG_WRITE32((TIMER_BASE(timerId) + CLK_REGOFF_CTRL), (result | CLK_DEF_TC_INTDISABLE));
}

void timer_int_unmask(UINT8 timerId)
{
    UINT32 result;
    
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL,result);

    /*modified for lint e665 */
    PWRCTRL_REG_WRITE32((TIMER_BASE(timerId) + CLK_REGOFF_CTRL), (result | CLK_DEF_TC_ENABLE));
}

void timer_int_clear(UINT8 timerId)
{
    UINT32 result;
    
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_CLEAR,result);
}/*lint !e550*/

UINT32 timer_int_status_get(UINT8 timerId)
{
    UINT32 result;
   
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_INTSTATUS,result);
    return (result&CLK_INT_VALID);
}

UINT32 timer_raw_int_status_get(UINT8 timerId)
{
    UINT32 result;
    
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_RAWINTSTATUS,result);
    return (result&CLK_INT_VALID);
}

void timer_32bit_count_set(UINT8 timerId)
{
    UINT32 result;
    
    PWRCTRL_REG_READ32(TIMER_BASE(timerId) + CLK_REGOFF_CTRL,result);

    /*modified for lint e665 */
    PWRCTRL_REG_WRITE32((TIMER_BASE(timerId) + CLK_REGOFF_CTRL), (result |0x2));
}

UINT32  timer_input_clk_get(UINT8 timerId)
{
     switch (timerId)
     {
        case 0:
            return SOFT_TIMER_CLK;
        case 1:
            return SOFT_TIMER_CLK;
        case 2:
        case 3:
            return SOFT_TIMER_CLK;
        default:
            return 0xFFFFFFFF; /*invalid address*/
     }
}


UINT32 TIMER_BASE(UINT8 u32ClkNum)
{
    UINT32 u32Ret;
    u32ClkNum = u32ClkNum%8;
/***
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
    if(ACPU_SOFTTIMER_ID == u32ClkNum)
    {
        u32Ret = PWRCTRL_SOC_ADDR_TIMER0_0 + u32ClkNum*0x14;	
    }   
    else
    {
        u32Ret = PWRCTRL_SOC_ADDR_TIMER0_0 + u32ClkNum*0x14;
    }        
#elif defined (BOARD_SFT) && defined (VERSION_V3R2)
    if(ACPU_SOFTTIMER_ID == u32ClkNum)
    {
        u32Ret = PWRCTRL_SOC_ADDR_TIMER0_0 + u32ClkNum*0x14;
    }      
    else
    {
        u32Ret = PWRCTRL_SOC_ADDR_TIMER1_0 + u32ClkNum*0x14;
    }       
#endif
****/
    u32Ret = PWRCTRL_SOC_ADDR_TIMER0_0 + u32ClkNum*0x14;
    return u32Ret;
}


static struct irqaction acpu_softtimer_irq = {
	.name = "acpu_softtimer",
	.flags  = IRQF_DISABLED | IRQF_TIMER,
	.handler	= softtimer_interrupt_call_back,
};
/*****************************************************************************
 Function     : Acpu_softtimer_init
 Description: Acpu softtimer module init
 Input           : 
 Return        : 
 Other         :
 *****************************************************************************/

static int __init Acpu_softtimer_init(void)
{
//#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) ||(defined(BOARD_SFT) && defined(VERSION_V3R2))
    int error = 0;
    UINT32 returnvalue = 0;
    printk( "enter Acpu-softtimer-modeule-init!!!\n");
    
	
    /*注册硬件定时器中断*/
    setup_irq ( INT_LVL_TIMER_2, &acpu_softtimer_irq);
    /* 软定时器结构初始化*/
    returnvalue = softtimer_module_start();
	
    /*创建线程*/
    printk( "start create the softtimer thread!!!\n");
    
    softtimer_task = kthread_create(softtimer_task_func,(void *)NULL,"acpu_softtimer");
    if ( NULL != softtimer_task )
    {
        wake_up_process( softtimer_task);
    }  

    printk( "end  the  Acpu_softtimer_init()  !!!\n");
    
    return error;
//#endif
}/*lint !e550*/

/*****************************************************************************
 Function     : Acpu_softtimer_exit
 Description: Acpu softtimer module uninit
 Input           : 
 Return        : 
 Other         :
 *****************************************************************************/
static void  __init Acpu_softtimer_exit(void)
{
//#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) ||(defined(BOARD_SFT) && defined(VERSION_V3R2))
    /*modified for lint e529 
    int error = 0;*/
    printk("enter acpu-softtimer-modeule-exit!!!\n");
    
	
    if (softtimer_task)
    {   
         /*删除线程*/
        kthread_stop(softtimer_task);
        softtimer_task = NULL;
    }
    printk("end acpu-softtimer-modeule-exit!!!\n");
    
//#endif
}


subsys_initcall(Acpu_softtimer_init);
module_exit(Acpu_softtimer_exit);
MODULE_LICENSE("GPL v2");

/*****************************************************************************
* 函 数 名  : is_charger_online
* 功能描述  : 读共享内存，判断USB/Charger是否在位
                               此函数只是泛指USB/Charger，不区分二者
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : STATUS_ONLINE--在位；
                           STATUS_ONLINE--不在位；
* 其它说明  : 无
*****************************************************************************/
CHG_ON_OFF_LINE_STATUS is_charger_online(void)
{
    uint32_t is_chg_in_addr = MEMORY_AXI_USB_INOUT_ADDR;
    CHG_ON_OFF_LINE_STATUS is_chg_in_flag = 0;

    is_chg_in_flag = *(volatile uint32_t *)(is_chg_in_addr);
    if(PMU2USB_C_FLAG == (is_chg_in_flag & CHG_FALG_MASK))
    {
        if(CHARGER_ONLINE == (is_chg_in_flag & CHG_ON_OFF_LINE_MASK))
        {
            return STATUS_ONLINE;
        }
        else
        {
            return STATUS_OFFLINE;
        }
    }
}
/*****************************************************************************
* 函 数 名  : special_timer_vote_sleep
* 功能描述  : Special_timer 超时函数中起的1秒定时器超时处理函数
                                投LCD允许休眠票
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
void special_timer_vote_sleep(void)
{
    /*投票允许休眠*/
    BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SPECIALTIMER);
    /*给定时器句柄置空*/
    s_special_timer_vote_sleep = NULL;
}
/*****************************************************************************
* 函 数 名  : special_timer_cb
* 功能描述  : Special_timer 超时处理函数
                                借用LCD投反对票，起1秒定时器，超时后投LCD允许休眠票
                                通过netlink上报上层定时器超时事件
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
void special_timer_cb(void)
{
    int ret, size;
    DEVICE_EVENT event;
    printk( KERN_EMERG "special_timer_cb() entry!!!\n");

    /*借用LCD投票标志，给系统投一票反对休眠票，1秒中之后投允许休眠票*/
    BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_SPECIALTIMER);
    SOFTTIMER_CREATE_TIMER(&s_special_timer_vote_sleep, 
        SPECIAL_TIMER_VOTE_SLEEP_TIME, SOFTTIMER_NO_LOOP, special_timer_vote_sleep, 0);
    if(NULL == s_special_timer_vote_sleep)
    {
        printk( KERN_EMERG "special_timer_cb() creat timer fail!!!\n");
        /*定时器启动失败，需要立刻投允许休眠票，防止单板一直无法休眠*/
        BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_SPECIALTIMER);
        return;
    }

    size =  sizeof(DEVICE_EVENT);

    event.device_id = DEVICE_ID_TIMER;//your_id 设备ID
    event.event_code = DEVICE_TIMEROU_F; //your_code;事件代码
    event.len = sizeof(event);
    
    ret = device_event_report(&event, size);
    printk( KERN_EMERG "event: %d, %d\n",event.device_id,event.event_code);

    if (-1 == ret) 
    {
        printk(KERN_ERR "special_timer_cb device_event_report fail!\n");
    }

    return;
}
/*****************************************************************************
* 函 数 名  : special_timer_start_func
* 功能描述  : 起special timer定时器
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : BSP_OK--创建成功；
                           BSP_ERROR--创建失败；
* 其它说明  : 无
*****************************************************************************/
BSP_S32 special_timer_start_func(unsigned int uTime)
{
    unsigned int uTimeTemp = 0;

    uTimeTemp = uTime * SPECIAL_TIMER_TIME_UNIT_SECOND;

    if(NULL != s_special_softtimer)
    {
        printk(KERN_EMERG "Special timer is creat, not stop, creat again not allowed!\n");
        return -1;
    }

    SOFTTIMER_CREATE_TIMER(&s_special_softtimer, 
       uTimeTemp, SOFTTIMER_LOOP, special_timer_cb, uTime);
    if(NULL == s_special_softtimer)
    {
        printk(KERN_EMERG "Creat special timer fail!!! \n");
        return -1;
    }

    printk(KERN_EMERG "Creat special timer sucess, timerID is %d \n",
        *s_special_softtimer);
    return 0;
}
/*****************************************************************************
* 函 数 名  : special_timer_stop_func
* 功能描述  : 停止special timer定时器
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : BSP_OK--创建成功；
                           BSP_ERROR--创建失败；
* 其它说明  : 无
*****************************************************************************/
BSP_S32 special_timer_stop_func(void)
{
    if (NULL != s_special_softtimer)
    {
        printk(KERN_EMERG "Before delete, timerID  is  %d \n",*s_special_softtimer);
        if(0 != SOFTTIMER_DEL_TIMER(&s_special_softtimer))
        {
            printk(KERN_EMERG "Delete special_timer fail!\n");
            return -1;
        }
        s_special_softtimer = NULL;
    }

    return 0;
}
/*****************************************************************************
* 函 数 名  : balong_special_timer_open
* 功能描述  : Special_timer open处理函数
                               预留，目前没使用
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : 
* 其它说明  : 无
*****************************************************************************/
static BSP_S32 balong_special_timer_open(struct inode *inode, struct file *file)
{
    if (NULL != inode && NULL != file)
    {
        ;
    }
    return 0;
}
/*****************************************************************************
* 函 数 名  : balong_special_timer_release
* 功能描述  : Special_timer release处理函数
                               预留，目前没使用
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : 
* 其它说明  : 无
*****************************************************************************/
static BSP_S32 balong_special_timer_release(struct inode *inode, struct file *file)
{
    if (NULL != inode && NULL != file)
    {
        ;
    }
    return 0;
}
/*****************************************************************************
* 函 数 名  : balong_special_timer_read
* 功能描述  : Special_timer read处理函数
                               预留，目前没使用
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : 
* 其它说明  : 无
*****************************************************************************/
static BSP_S32 balong_special_timer_read(struct file *file, char __user * buffer, 
        size_t count, loff_t *ppos)
{
    CHG_STATUS chg_status;
    if (NULL != file)
    {
        ;
    }

    chg_status.chg_on_off_line_flag = is_charger_online();
    chg_status.chg_type = CHARGE_TYPE_USB;
    printk(KERN_EMERG "balong_special_timer_read, status=%d, type=%d, count=%d!\n",
        chg_status.chg_on_off_line_flag, chg_status.chg_type, count);
    if(0 != copy_to_user(buffer, &chg_status, sizeof(chg_status)))
    {
        return -1;
    }
    return 0;
}
/*****************************************************************************
* 函 数 名  : balong_special_timer_write
* 功能描述  :字符设备写函数，预留
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : 
* 其它说明  : 无
*****************************************************************************/
static void balong_special_timer_write(struct file *file, const char __user *buf, 
        size_t count,loff_t *ppos)
{
    if (NULL == file && NULL == ppos)
    {
        ;
    }

    return;
}
extern void softtimer_print(UINT32 param);
/*****************************************************************************
* 函 数 名  : balong_special_timer_ioctl
* 功能描述  : Special_timer ioctrl处理函数
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
* 其它说明  : 无
*****************************************************************************/
static BSP_S32 balong_special_timer_ioctl(struct inode *inode, 
        struct file *file, bool bStart, unsigned int uTime)
{
    BSP_S32 err_code = 0;

    printk( KERN_EMERG "balong_special_timer_ioctl() entry-------!!!\n");

    if ((NULL == inode) && (NULL == file))
    {
        ;
    }

    if(SPECIAL_TIMER_START == bStart)
    {
        err_code = special_timer_start_func(uTime);
        if(0 != err_code)
        {
            return err_code;
        }
    }
    else if(SPECIAL_TIMER_STOP == bStart)
    {
        err_code = special_timer_stop_func();
        if(0 != err_code)
        {
            return err_code;
        }
    }
    else
    {
        printk(KERN_EMERG "param bStart invalid, bStart=%d!\n",bStart);
        return -1;
    }

    printk(KERN_EMERG "balong_special_timer_ioctll() leave-------!!!\n");

    return 0;
}

/*special timer函数数据结构*/
static const struct file_operations balong_special_timer_fops = {
    .owner = THIS_MODULE,
    .read = balong_special_timer_read,
    .write = balong_special_timer_write,
    .ioctl = balong_special_timer_ioctl,
    .open = balong_special_timer_open,
    .release = balong_special_timer_release,
};


/*balong_special_timer_miscdev作为调用misc_register函数的参数，
用于向linux内核注册special timer(硬timer)misc设备。
*/
static struct miscdevice balong_special_timer_miscdev = {
    .name = "special_timer",
    .minor = MISC_DYNAMIC_MINOR,/*动态分配子设备号（minor）*/
    .fops = &balong_special_timer_fops,
};
/*****************************************************************************
* 函 数 名  : balong_special_timer_init
*
* 功能描述  : Special_timer A核模块初始化
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
static BSP_S32 balong_special_timer_init(void)
{
    int ret = 0;

    ret = misc_register(&balong_special_timer_miscdev);
    if (0 != ret)
    {
        printk("------special_timer misc register fail!\n");
        return ret;
    }

    printk("------special_timer misc register leave!\n");
    return BSP_OK;
}

module_init(balong_special_timer_init);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


