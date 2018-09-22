/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_wifi.c
*
*   作    者 :  yuanqinshun
*
*   描    述 :  本文件命名为"balong_wifi.c"
*
*   修改记录 :  2011年12月19日  v1.00  yuanqinshun创建
*************************************************************************/

#ifndef _balong_wifi_h_
#define _balong_wifi_h_

#include <typedefs.h>
#include <bcmendian.h>
#include <linuxver.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>

#define BALONG_WIFI_TRACE
#ifdef BALONG_WIFI_TRACE
#define BALONG_WIFI_TRACE_THREAD

/*the max length of one trace*/
#define WIFI_TRACE_MAX_LEN  256   

/*the max number of trace*/
#define WIFI_TRACE_MAX_NUM  256   


/*module ID*/
#define	DHD_ID_DEBUG	   0x8126  /*33062*/
#define	BCMSDH_ID_DEBUG	   0x8127  //SD_ID_DEBUG/*33063*/  
#define	TRACE_ID_DEBUG	   0x8128  //AT_ID_DEBUG	       0x8128  /*33064*/
#define	WL_ID_DEBUG		   0x8129  /*33065*/

/*trace level*/
typedef enum
{
	OM_DEBUG_ERROR = 1,
	OM_DEBUG_DEBUG,
	OM_DEBUG_WARNING,
	OM_DEBUG_INFO
}om_debug_level;


typedef enum _wifi_trace_type
{
    WIFI_STR_TRACE          = 1,
    WIFI_CTL_FRAME          = 0x9103,    //wifi ctrl trace   
    WIFI_TX_FRAME           = 0x9104,   //frame sent to wifi chip
    WIFI_TXGEN_FRAME        = 0x9105,	 //TX gen frame sent to wifi chip
    WIFI_RXSUP_FRAME        = 0x9106,	 //RX supper frame 
    WIFI_RXSUBDATA_FRAME    = 0x9107,	 //RX sub frame data
    WIFI_RX_FRAME           = 0x9108,	 //RX  frame 
    NULL_TYPE               = 0x9109
}   WIFI_TRACE_ENUM;

typedef struct _wifi_trace_stru
{
    WIFI_TRACE_ENUM traceType; // tace type
    unsigned int traceLen; // tace len
    char traceArry[WIFI_TRACE_MAX_LEN];
} WIFI_TRACE_STRU;

typedef struct _wifi_trace_ctrl_stru
{
    long 	thr_pid;
    int 	prio; 
    struct	semaphore sema;
    bool	terminated;
    struct	completion completed;

    WIFI_TRACE_STRU *pTrace;
    int traceHead;
    int traceTail;
    unsigned int dropTraceCnt;
    spinlock_t wifiTraceLock;        
    
} WIFI_TRACE_CTRL_STRU;

#define WIFI_TRACE_CTRL_INIT(task_ctl)     memset(task_ctl,0x0, sizeof(struct _wifi_trace_ctrl_stru))  

#define WIFI_SPIN_LOCK(task_ctl)             spin_lock_bh(&(task_ctl->wifiTraceLock))
#define WIFI_SPIN_UNLOCK(task_ctl)           spin_unlock_bh(&(task_ctl->wifiTraceLock))


#define WIFI_TRACE_PROC_START(thread_func, tsk_ctl, flags) \
{ \
    spin_lock_init(&((tsk_ctl)->wifiTraceLock)) ; \
	sema_init(&((tsk_ctl)->sema), 0); \
	init_completion(&((tsk_ctl)->completed)); \
	(tsk_ctl)->terminated = FALSE; \
	(tsk_ctl)->thr_pid = kernel_thread(thread_func, tsk_ctl, flags); \
	if ((tsk_ctl)->thr_pid > 0) \
		wait_for_completion_timeout(&((tsk_ctl)->completed), 2*HZ); \
	DBG_THR(("%s thr:%lx started\n", __FUNCTION__, (tsk_ctl)->thr_pid)); \
}

#define WIFI_TRACE_PROC_STOP(tsk_ctl) \
{ \
	(tsk_ctl)->terminated = TRUE; \
	smp_wmb(); \
	up(&((tsk_ctl)->sema));	\
	wait_for_completion_timeout(&((tsk_ctl)->completed), 2*HZ); \
	DBG_THR(("%s thr:%lx terminated OK\n", __FUNCTION__, (tsk_ctl)->thr_pid)); \
	(tsk_ctl)->thr_pid = -1; \
}


extern WIFI_TRACE_CTRL_STRU   *g_pstWifiTraceCtrl;
int wifi_trace_init (void) ;
int wifi_trace_add( WIFI_TRACE_ENUM traceType, char *pBuf, unsigned int bufLen);
int wifi_trace_add_exp(unsigned long ulModuleId, unsigned long ulLevel, char * pcformat,...);
int wifi_trace_uninit(void);
int wifi_trace_pri_check(WIFI_TRACE_ENUM traceType);
unsigned int wifi_trace_check_lev (unsigned long ulModuleId,unsigned long ulLevel);

#endif


/*************************************user to extern wl function **************************************/
#define WL_EXT_CMD_PRE_STR    "ext_"
#define WL_EXT_CMD_MIN_LEN    strlen(WL_EXT_CMD_PRE_STR)
#define WL_EXT_START_TIMER    "ext_timer"
#define WL_EXT_CHECK_POWER    "ext_power"

extern int balong_wl_ext_process(char * buf, int len);

extern int g_wifiRxFlowCtrlFlag;
#define WIFI_RX_FLOW_IS_SET()     (g_wifiRxFlowCtrlFlag==1)


#endif
