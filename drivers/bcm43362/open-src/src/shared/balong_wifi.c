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
#include <linux/sched.h>
#include <linuxver.h>
#include <balong_wifi.h>
#include <drivers/mmc/host/wifi_printf.h>	

#ifdef BALONG_WIFI_TRACE

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
#define DAEMONIZE(a) daemonize(a); \
	allow_signal(SIGKILL); \
	allow_signal(SIGTERM);
#else /* Linux 2.4 (w/o preemption patch) */
#define RAISE_RX_SOFTIRQ() \
	cpu_raise_softirq(smp_processor_id(), NET_RX_SOFTIRQ)
#define DAEMONIZE(a) daemonize(); \
	do { if (a) \
		strncpy(current->comm, a, MIN(sizeof(current->comm), (strlen(a) + 1))); \
	} while (0);
#endif /* LINUX_VERSION_CODE  */

#define LOCAL_TRACE_PRE_LEN 8
#define LOCAL_TRACE_LEN   (WIFI_TRACE_MAX_LEN - LOCAL_TRACE_PRE_LEN)

struct local_trace_type
{
    unsigned int moduleId;
    unsigned int lev;
    char buf[LOCAL_TRACE_LEN];
};

struct frame_trace_type
{
    DRV_TRACE_IND_STRU  drvIndStru;
};

#define FRAME_TRACE_MAXSIZE    (WIFI_TRACE_MAX_LEN - sizeof(DRV_TRACE_IND_STRU) + 4)

/********************global variables area*******************************/
WIFI_TRACE_CTRL_STRU   *g_pstWifiTraceCtrl = NULL;
static WIFI_TRACE_CTRL_STRU   g_stWifiTraceCtrl;
static int wifi_trace_default_lev = 0;
static int wifi_trace_prio = 100;

/******************** debug functions *******************************/
int wifi_trace_null = 0;

#define WIFI_TRACE_IS_OPEN()     do {    \
                                 if(wifi_trace_null)  \
                                    return (0);\
                                 }  while(0)

void wifi_trace_disable(int ctl)
{
    
    printk("WiFi trace %s ---> %s \n",wifi_trace_null?"disable":"enable",ctl?"disable":"enable");
    wifi_trace_null = ctl;

    return;

}

void wifi_trace_lev(int lev)
{
    
    printk("WiFi lev %d ---> %d \n",wifi_trace_default_lev,lev);
    wifi_trace_null = lev;

    return;

}


void wifi_trace_dump( WIFI_TRACE_STRU *pTrace)
{
    int i;
    struct local_trace_type *pLocalTrace;
    
    switch(pTrace->traceType)
    {
        case WIFI_STR_TRACE:
            pLocalTrace = (struct local_trace_type*)pTrace->traceArry;            
            if(OM_PrintWithModuleIsReady)
            {
                OM_PRINT_WITH_MODULE(pLocalTrace->moduleId, pLocalTrace->lev,"%s",pLocalTrace->buf);            
            }
            else
            {
                printf("%s",pLocalTrace->buf);
            }            
            break;   

        case WIFI_CTL_FRAME:
        case WIFI_TX_FRAME:
        case WIFI_TXGEN_FRAME:
        case WIFI_RXSUP_FRAME:
        case WIFI_RXSUBDATA_FRAME:
        case WIFI_RX_FRAME:            
            {
                BSP_MNTN_OmDrvTraceSend(pTrace->traceType, TRACE_ID_DEBUG, 
                    (unsigned char *)(pTrace->traceArry + sizeof(DRV_TRACE_IND_STRU)- 4),
                     pTrace->traceLen);                                
            }                    
            break;             
        default:
            printk("--- trace type 0x%x ----\n",pTrace->traceType);
            if(pTrace->traceLen <= WIFI_TRACE_MAX_LEN)
            {

                for (i=0; i<pTrace->traceLen; i++)
                {
                    printk("%02x",pTrace->traceArry[i]);
                }            
                printk("\n----------------------------------\n");                
            }
            else
            {
                printk("--- trace len 0x%x ----\n",pTrace->traceLen);                
            }
          break;
    }
    
    return;
}

/***********************************************************************************
 Function:          wifi_trace_add_exp
 Description:       add one trace to wifi trace pool
 Calls:
 Input:             ulModuleId:
                    ulLevel:
 Output:            NA
 Return:            1: this trace can be output;
                    0: this trace can't be output;
 ************************************************************************************/

unsigned int wifi_trace_check_lev (unsigned long ulModuleId,unsigned long ulLevel)
{
    if(OM_PrintGetIdLevIsReady)
    {
        return((ulLevel>OM_PRINT_GET_MODULE_IDLEV(ulModuleId))?0:1);
    }
    else
    {
        return 0;
    }
}



/***********************************************************************************
 Function:          wifi_trace_add_exp
 Description:       add one trace to wifi trace pool
 Calls:
 Input:             ulModuleId:
                    ulLevel:
                    pcformat:
 Output:            NA
 Return:            NA
 ************************************************************************************/
int wifi_trace_add_exp(unsigned long ulModuleId,unsigned long ulLevel, char *pcformat,...)
{
#ifdef BALONG_WIFI_TRACE_THREAD
    int head,tail,nextTail;
    WIFI_TRACE_STRU *pTrace;
    struct local_trace_type loacalTrace;
    int localTraceLen;
    va_list argument;
    
    WIFI_TRACE_CTRL_STRU *pWifiTraceCtrl = g_pstWifiTraceCtrl;

    WIFI_TRACE_IS_OPEN();
   
    if (NULL == pWifiTraceCtrl)
    {
        printf("%s g_pstWifiTraceCtrl is null!\n", __FUNCTION__);
        return -1;
    }
    /*判断入参*/

    loacalTrace.moduleId = ulModuleId;
    loacalTrace.lev = ulLevel;
    /* 将格式化字符串和可变参数转换为字符串 */
    va_start( argument, pcformat);    
    localTraceLen = vsnprintf(loacalTrace.buf, LOCAL_TRACE_LEN-1, pcformat, argument);
    localTraceLen = localTraceLen + LOCAL_TRACE_PRE_LEN + 1 ;
    va_end( argument );
    
	/* get the tail and head of the ring */
    WIFI_SPIN_LOCK(pWifiTraceCtrl);   
    
    head = pWifiTraceCtrl->traceHead;
    tail = pWifiTraceCtrl->traceTail;   
    
    nextTail = tail + 1;

    if (WIFI_TRACE_MAX_NUM == nextTail) 
    {
        nextTail = 0;
    }

    if(nextTail == head)
    {
        WIFI_SPIN_UNLOCK(pWifiTraceCtrl);            
        printf("%s no space, so drop pkt!\n", __FUNCTION__);            
        pWifiTraceCtrl->dropTraceCnt++;
        return -1;
    }

    pTrace = pWifiTraceCtrl->pTrace + tail;
    pTrace->traceType = WIFI_STR_TRACE;
    pTrace->traceLen = (localTraceLen > WIFI_TRACE_MAX_LEN)? WIFI_TRACE_MAX_LEN:localTraceLen;
    memcpy((char*)pTrace->traceArry,&(loacalTrace.moduleId),pTrace->traceLen);
    pWifiTraceCtrl->traceTail = nextTail;   
    
    WIFI_SPIN_UNLOCK(pWifiTraceCtrl);    
    
    up(&pWifiTraceCtrl->sema);

    return -1;
#else
    struct local_trace_type loacalTrace;
    int localTraceLen;
    va_list argument;

    /* 将格式化字符串和可变参数转换为字符串 */
    va_start( argument, pcformat);    
    localTraceLen = vsnprintf(loacalTrace.buf, LOCAL_TRACE_LEN-1, pcformat, argument);
    localTraceLen = localTraceLen + LOCAL_TRACE_PRE_LEN + 1 ;
    va_end( argument );

    if(ulLevel <= wifi_trace_default_lev)
    {
        printf("id=0x%x:%s\n",ulModuleId,loacalTrace.buf);
    }

    if(OM_PrintWithModuleIsReady)
    {
        if(!in_atomic())
        {
            OM_PRINT_WITH_MODULE(ulModuleId, ulLevel,loacalTrace.buf);            
        }
    }
                
    return 0;
#endif


}


/***********************************************************************************
 Function:          wifi_trace_pri_check
 Description:      check the trace type can be send or nor
 Calls:
 Input:             traceType:
 Output:            NA
 Return:            1: the trace can be sent
                       0: can't be sent
 ************************************************************************************/

int wifi_trace_pri_check(WIFI_TRACE_ENUM traceType)
{
    return((0 == BSP_MNTN_DrvPrimCanSendOrNot((unsigned short)traceType)) ? 1 : 0);
}

/***********************************************************************************
 Function:          wifi_trace_add
 Description:       add one trace to wifi trace pool
 Calls:
 Input:             traceType:
                    pBuf:
                    bufLen:
 Output:            NA
 Return:            0: success
                   -1: failed
 ************************************************************************************/
int wifi_trace_add( WIFI_TRACE_ENUM traceType, char *pBuf, unsigned int bufLen)
{
#ifdef BALONG_WIFI_TRACE_THREAD
    int head,tail,nextTail;
    WIFI_TRACE_STRU *pTrace;
    WIFI_TRACE_CTRL_STRU *pWifiTraceCtrl = g_pstWifiTraceCtrl;

    WIFI_TRACE_IS_OPEN();

    if ((NULL == pWifiTraceCtrl) || (NULL == pBuf))
    {
        printf("%s g_pstWifiTraceCtrl is 0x%x,pBuf = 0x%x!\n", __FUNCTION__,(unsigned int)pWifiTraceCtrl,(unsigned int)pBuf);
        return -1;
    }
        
	/* get the tail and head of the ring */
    WIFI_SPIN_LOCK(pWifiTraceCtrl);   
    
    head = pWifiTraceCtrl->traceHead;
    tail = pWifiTraceCtrl->traceTail;   
    
    nextTail = tail + 1;

    if (WIFI_TRACE_MAX_NUM == nextTail) 
    {
        nextTail = 0;
    }

    if(nextTail == head)
    {
        WIFI_SPIN_UNLOCK(pWifiTraceCtrl);            
        printf("%s no space, so drop pkt!\n", __FUNCTION__);            
        pWifiTraceCtrl->dropTraceCnt++;
        return -1;
    }

    pTrace = pWifiTraceCtrl->pTrace + tail;

    if (traceType < NULL_TYPE)
    {
        pTrace->traceType = traceType;
        pTrace->traceLen = (bufLen > FRAME_TRACE_MAXSIZE)?FRAME_TRACE_MAXSIZE:bufLen;        
        memcpy((char*)(pTrace->traceArry + sizeof(DRV_TRACE_IND_STRU)- 4),pBuf,pTrace->traceLen);
    }
    else
    {
        WIFI_SPIN_UNLOCK(pWifiTraceCtrl);            
        printf("%s unsupport traceType = %d!\n", __FUNCTION__,traceType);
        return -1;        
    }
        
    pWifiTraceCtrl->traceTail = nextTail;   
    
    WIFI_SPIN_UNLOCK(pWifiTraceCtrl);    
    
    up(&pWifiTraceCtrl->sema);

    return 0;

#else

    int i;

    switch(traceType)
    {
        default:
            printf("%s unknow traceType is %d!\n", __FUNCTION__,traceType);            
            printk("--- trace type %d ----\n",traceType);
            for (i=0; i<bufLen; i++)
            {
                printk("%02x",pBuf[i]);
            }            
            printk("\n----------------------------------\n");

            break;
    }
    
    return;
    

#endif
}


/***********************************************************************************
 Function:          wifi_trace_dump
 Description:       put the wifi trace to SDT
 Calls:
 Input:             pWifiTraceCtrl : wifi trace control block
 Output:            NA
 Return:            NA
 ************************************************************************************/

static void
wifi_trace_process(WIFI_TRACE_CTRL_STRU *pWifiTraceCtrl)
{
    int head,tail;
    WIFI_TRACE_STRU *pTrace;

	/* get the tail and head of the ring */
    WIFI_SPIN_LOCK(pWifiTraceCtrl);

    head = pWifiTraceCtrl->traceHead;
    tail = pWifiTraceCtrl->traceTail;
    
    WIFI_SPIN_UNLOCK(pWifiTraceCtrl);
        
    while(head != tail)
    {
        pTrace = pWifiTraceCtrl->pTrace + head;

        /*dump the trace accord trace type*/
        wifi_trace_dump(pTrace);

        head++;

        if(WIFI_TRACE_MAX_NUM == head)
        {
            head = 0;
        }

        WIFI_SPIN_LOCK(pWifiTraceCtrl);
        
        pWifiTraceCtrl->traceHead = head;
        tail = pWifiTraceCtrl->traceTail;
        
        WIFI_SPIN_UNLOCK(pWifiTraceCtrl);
        
    }

    return;
}



/***********************************************************************************
 Function:          wifi_trace_thread
 Description:       kernel thread to put the wifi trace to SDT
 Calls:
 Input:             pWifiTraceCtrl : wifi trace control block 
 Output:            NA
 Return:            0: success
                   -1: failed
 ************************************************************************************/

static int
wifi_trace_thread(void *data)
{

    WIFI_TRACE_CTRL_STRU *pWifiTraceCtrl = (WIFI_TRACE_CTRL_STRU *)data;

	if (wifi_trace_prio > 0)
	{
		struct sched_param param;
		param.sched_priority = (wifi_trace_prio < MAX_RT_PRIO)?wifi_trace_prio:(MAX_RT_PRIO-1);
		sched_setscheduler(current, SCHED_FIFO, &param);
	}
     
	DAEMONIZE("wifi_trace_thread");

	/*  signal: thread has started */
	complete(&pWifiTraceCtrl->completed);

	/* Run until signal received */
	while (1) {
		if (down_interruptible(&pWifiTraceCtrl->sema) == 0) {

			SMP_RD_BARRIER_DEPENDS();
			if (pWifiTraceCtrl->terminated) {
				break;
			}

            wifi_trace_process(pWifiTraceCtrl);

		}
		else
		{
	        printf("%s loop break\n", __FUNCTION__);            
			break;
		}
	}

	complete_and_exit(&pWifiTraceCtrl->completed, 0);
}

/***********************************************************************************
 Function:          wifi_trace_init
 Description:      wifi debug trace initlization function
 Calls:
 Input:               NA
 Output:            NA
 Return:            0: success
                   -1: failed
 ************************************************************************************/

int wifi_trace_init (void) 
{

#ifdef BALONG_WIFI_TRACE_THREAD
    
    WIFI_TRACE_CTRL_STRU *pWifiTraceCtrl;

	printf("%s enter\n", __FUNCTION__);

    if(NULL != g_pstWifiTraceCtrl)
    {
	    printf("%s g_pstWifiTraceCtrl isn't null \n", __FUNCTION__); 
        return -1;        
    }

    pWifiTraceCtrl =(WIFI_TRACE_CTRL_STRU*) &g_stWifiTraceCtrl;

    /*initlization control block*/
    WIFI_TRACE_CTRL_INIT(pWifiTraceCtrl);
               
    /*start the wifi trace thread*/
    WIFI_TRACE_PROC_START(wifi_trace_thread, pWifiTraceCtrl, 0);

    if(unlikely(pWifiTraceCtrl->thr_pid <= 0))
    {
	    printf("%s WIFI_TRACE_PROC_START failed \n", __FUNCTION__); 
        return -1;
    }
        
    /*init trace space*/
    pWifiTraceCtrl->pTrace = (WIFI_TRACE_STRU *)MALLOC(NULL, sizeof(WIFI_TRACE_STRU)*WIFI_TRACE_MAX_NUM);

    if(NULL == pWifiTraceCtrl->pTrace)
    {
        WIFI_TRACE_PROC_STOP(pWifiTraceCtrl);            
	    printf("MALLOC failed\n"); 
        return -1;
        
    }

    pWifiTraceCtrl->traceHead = 0;
    pWifiTraceCtrl->traceTail = 0;

    g_pstWifiTraceCtrl = pWifiTraceCtrl;

#else

	printf("%s enter,no thread\n", __FUNCTION__);

#endif

    return 0;
}


/***********************************************************************************
 Function:          wifi_trace_uninit
 Description:      wifi debug trace uninitialization
 Calls:
 Input:               NA
 Output:            NA
 Return:            0: success
                   -1: failed
 ************************************************************************************/
int wifi_trace_uninit (void) 
{

#ifdef BALONG_WIFI_TRACE_THREAD    
    WIFI_TRACE_CTRL_STRU *pWifiTraceCtrl;


	printf("%s enter\n", __FUNCTION__);

    if(NULL == g_pstWifiTraceCtrl)
    {
	    printf("%s g_pstWifiTraceCtrl should not be null \n", __FUNCTION__); 
        return -1;        
    }

    pWifiTraceCtrl =(WIFI_TRACE_CTRL_STRU*) &g_stWifiTraceCtrl;

    /*start the wifi trace thread*/
    WIFI_TRACE_PROC_STOP(pWifiTraceCtrl);

    MFREE(NULL, pWifiTraceCtrl->pTrace, sizeof(WIFI_TRACE_STRU)*WIFI_TRACE_MAX_NUM);

    g_pstWifiTraceCtrl = NULL;

#else
	printf("%s enter,no thread\n", __FUNCTION__);
#endif

    return 0;
    
}

#endif


/******************user to extern wl function *******************************/


/***********************************************************************************
 Function:          balong_wl_ext_process
 Description:       extern wl funciton for balong platform
 Calls:
 Input:             buf : extern buf
                    len : buffer length
 Output:            NA
 Return:            0: wl extern function
                   -1: isn't wl extern function
 ************************************************************************************/
int balong_wl_ext_process(char * buf, int len)
{
    int ret = 0;
    
    if ((NULL == buf)||(len <= WL_EXT_CMD_MIN_LEN) || (0 != strncmp(buf,WL_EXT_CMD_PRE_STR,WL_EXT_CMD_MIN_LEN)))
    {
        return -1;    
    }

    //(buf, RWL_WIFI_ACTION_CMD,strlen(RWL_WIFI_ACTION_CMD))

    if(0 == strncmp(buf, WL_EXT_START_TIMER, strlen(WL_EXT_START_TIMER)))
    {
        unsigned int timeVal = 0;

        if(len >= (strlen(WL_EXT_START_TIMER) + sizeof(timeVal) + 1))
        {
            memcpy(&timeVal, (buf + strlen(WL_EXT_START_TIMER) + 1), sizeof(timeVal));
        }
        
        printf("wl ext: %s, and timeVal =%d \n", WL_EXT_START_TIMER, timeVal);        
    }
    else if (0 == strncmp(buf, WL_EXT_CHECK_POWER, strlen(WL_EXT_CHECK_POWER)))
    {
        printf("wl ext: %s\n", WL_EXT_CHECK_POWER);
        strcpy(buf,"ext_power high");
    }
    else
    {
        ret  = -1;
    }

    return ret;

}

