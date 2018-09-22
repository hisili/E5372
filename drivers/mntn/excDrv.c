
/****************************问题单修改记录****************************
    日期      修改人     问题单号         修改内容
                                          改为中断底半部处理
*********************************************************************/

#include <asm/current.h>
#include <asm/cacheflush.h>

#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/thread_info.h>

#include "BSP.h"
#include "excDrv.h"   
#include <linux/syslog.h>
#include "BSP_WDT.h"
#include "DrvInterface.h"

#define PID_PPID_GET(taskTCB)  ((((struct task_struct *)taskTCB)->pid & 0xffff)| \
                                 ((((struct task_struct *)taskTCB)->real_parent->pid & 0xffff)<< 16))


SOCP_LOG_EXC_INFO_STRU * g_pstSocpLogExcInfo = (SOCP_LOG_EXC_INFO_STRU *)EXCH_A_C_SOCP_LOG_ADDR;

EH_ROOT* g_pExchRoot = (EH_ROOT*)EXCH_ROOT_ADDR;
TASK_NAME_STRU * pcvNameAddr = (TASK_NAME_STRU *)EXCH_CPU_VIEW_ADDR;

EH_NV g_exchNV = {0};
unsigned int taskIdList[DRV_LINUX_TASK_NUM_MAX] = {0};
unsigned int taskPidList[DRV_LINUX_TASK_NUM_MAX] = {0};
BSP_U32 g_stMntnInitFlag = 0;
struct task_struct *taskIdCurrent =NULL;
static struct work_struct exch_work;

typedef void (*FUNC_VOID)(void);

extern int log_buf_len;
extern char *log_buf ;
extern unsigned log_end;
extern unsigned int g_ulExcLogStart ;
extern unsigned int g_ulExcLogEnd;

extern    int task_switch_hook_add ( FUNC_VOID switchHook);
extern void int_switch_hook_add(FUNC_VOID pHookFunc);
extern void exc_hook_add(FUNC_VOID pHookFunc);
extern void show_mem(void);
void show_stack(struct task_struct *tsk, unsigned long *sp);

void exchUNDEF(void)
{
    int b = 0;
    FUNC_VOID a = (FUNC_VOID)(&b);
    b = 0x12345678;
    a();
}

void exchSWI(void)
{
    __asm("        SWI   0x1234   ");
}

void exchPABT(void)
{
    FUNC_VOID a = (FUNC_VOID)0xe0000000;
    a();
}

void exchDABT(void)
{
    *(int *)0xe0000000 = 0x12345678;
}
void exchLOOP(void)
{
    for(;;)
    {
        ;
    }
}

int exchDIV(int a)
{
    int b = 4;
    return  b/a;
}

void exchPANIC(void)
{
    panic("exchPANIC!\n");

}

/*****************************************************************************
  Function    : reg_save
  Description : 寄存器备份
  Calls       : 
  Called By   : 
  Input       : 
                  UINT32 ulRamAddr, 
                  UINT32 ulReg,
                  UINT32 ulSize
  Output      : NULL
  Return      : OK              ERROR      
*****************************************************************************/
void reg_save(unsigned int ulRamAddr, unsigned int ulReg, unsigned int ulSize)
{
    volatile unsigned int i = 0;
    volatile unsigned int *pulAddr = (volatile unsigned int *)ulRamAddr;
    ulSize = (ulSize >>2);
    for(i=0; i<ulSize; i++)
    {
        /*lint -e52*/
        *pulAddr = *(volatile unsigned int *)(ulReg + i*4);
        pulAddr++;
         /*lint +e52*/
    }

}

EXPORT_SYMBOL(reg_save);

/*************************************************
 函 数 名       : task_id_current_get
 功能描述   :  task_id_current_get
 输入参数   : 
 输出参数   :
 返 回 值      :  

 调用函数   :
 被调函数   :

 修改历史   :
    日    期       : 2011年10月28日
    作    者       : C54299
    修改内容 : linux modify

*************************************************/
unsigned int task_id_current_get()
{
    taskIdCurrent = current;
    return (unsigned int )taskIdCurrent;
}


/*************************************************
 函 数 名       : task_id_list_get
 功能描述   :  task_id_list_get
 输入参数   : 
 输出参数   :
 返 回 值      :  

 调用函数   :
 被调函数   :

 修改历史   :
    日    期       : 2011年10月28日
    作    者       : C54299
    修改内容 : linux modify

*************************************************/
int task_id_list_get()
{
    struct task_struct *pTid;
    int ulTaskNum = 0x00;
    unsigned int pid_ppid = 0x0;
    (void)memset(taskIdList, 0, sizeof(taskIdList));
    for_each_process(pTid) 
    {    
        if(ulTaskNum >=  DRV_LINUX_TASK_NUM_MAX)
        {
            return -1;
        }
        taskIdList[ulTaskNum] = (unsigned)(pTid);
        taskPidList[ulTaskNum] = ((pTid->pid & 0xffff)|((pTid->real_parent->pid & 0xffff)<< 16));;
        ulTaskNum++;    
    }
    return (int)ulTaskNum; /*clean lint e713*/

}

unsigned int task_tcb_test()
{

#if 0
    task_id_current_get();
    printk(KERN_DEBUG "stack = 0x%x    %08x\n",
		       taskIdCurrent->stack, );
#endif
    return 0;
}

/**********************queue  start*************************/
void QueueInit(Queue *Q, unsigned int elementNum)
{
    Q->maxNum = elementNum;
    Q->front = 0;
    Q->rear = 0;
    Q->num = 0;
    memset((void *)Q->data, 0, (size_t )(elementNum*sizeof(unsigned int)));
}

int QueueIn(Queue *Q, unsigned int element)
{
    if (Q->num == Q->maxNum)
    {
        return -1;
    }

    Q->data[Q->rear] = element;
    Q->rear = (Q->rear+1) % Q->maxNum;
    Q->num++;

    return 0;
}

int QueueOut(Queue *Q, unsigned int *element)
{
    if (0 == Q->num)
    {
        return -1;
    }

    *element = Q->data[Q->front];
    Q->front = (Q->front+1) % Q->maxNum;
    Q->num--;

    return 0;
}

int QueueReadLast(Queue *Q, unsigned int *element)
{
    if (0 == Q->num)
    {
        return -1;
    }

    *element = Q->data[((Q->rear+Q->maxNum)-1) % Q->maxNum];

    return 0;
}


int QueueLoopIn(Queue *Q, unsigned int element)
{
    if (Q->num < Q->maxNum)
    {
        return QueueIn(Q, element);
    }
    else
    {
        Q->data[Q->rear] = element;
        Q->rear = (Q->rear+1) % Q->maxNum;
        Q->front = (Q->front+1) % Q->maxNum;
    }

    return 0;
}
/**********************queue  end*************************/


unsigned int omTimerGet(void)
{
#if (defined(BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
    return 0;
#else
    return *(volatile unsigned int *)(OM_TIMER_CURRENT);
#endif
}
unsigned int omTimerTickGet(void)
{
#if (defined(BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    return 0;
#else
    unsigned int omTimerValue;
    unsigned long long omTick;

    omTimerValue=	omTimerGet();
    omTick=((unsigned long long)(0xFFFFFFFF-omTimerValue)*100)>>(15);
    return (unsigned int)omTick;
#endif
}


/*************************************************
 函 数 名       : usrPlatformReset
 功能描述   :  usrPlatformReset
 输入参数   : 
 输出参数   :
 返 回 值      :  

 调用函数   :
 被调函数   :

 修改历史   :
    日    期       : 2011年10月28日
    作    者       : C54299
    修改内容 : linux modify

*************************************************/
void  usrPlatformReset(void)
{
	#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
    *( volatile unsigned int * )FPGA_SOC_RESET_VA_BASE = SC_SW_RST_VAL;
	#elif defined (BOARD_SFT) && defined (VERSION_V3R2)
	*( volatile unsigned int * )SYSCTRL_SCSYSSTAT0 = SC_SW_RST_VAL;
	/*V3R2 CS f00164371*/
	#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2) && defined(CHIP_BB_6756CS)
	*( volatile unsigned int * )SYSCTRL_SCSYSSTAT0 = SC_SW_RST_VAL;
	#endif
}

/*****************************************************************************
* 函 数 名  : BSP_WDT_reboot
*
* 功能描述  : 看门狗复位系统
*
* 输入参数  : BSP_VOID  
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 使用看门狗进行系统复位
*
*****************************************************************************/
void wdtReboot(void)
{
    unsigned u32Val;
    BSP_U32 u32Key;
    #if (defined (BOARD_ASIC)||defined (BOARD_SFT))

    local_irq_save(u32Key);
    
	BSP_PWRCTRL_TimerOpen(PWRCTRL_MODU_WDT);

    /*重启之前刷cache*/
    flush_kernel_vmap_range(EXCH_BASE_ADDR, EXCH_C_CORE_SIZE+EXCH_A_CORE_SIZE);
    
    /* 寄存器解锁 */
    BSP_REG_WRITE(IO_ADDRESS(0x90001000), 0xC00, 0x1ACCE551);
	
    /* 使能复位和中断 */    
    BSP_REG_WRITE(IO_ADDRESS(0x90001000), 0x8, 0x3);
    
    /* 设置计数器初值 */
    BSP_REG_WRITE(IO_ADDRESS(0x90001000), 0x0, 1); 
    
    /* 锁寄存器 */
    BSP_REG_WRITE(IO_ADDRESS(0x90001000), 0xC00, 0x0); 

    for(;;)  //b00198513 Modified for pclint e716          
   	{}
    
    #elif (defined (BOARD_FPGA_P500))
   
    /*设置系统控制器:WDG时钟使能*/
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE),INTEGRATOR_SC_WDTCTRL_OFFSET,0,1,1);    /*lint !e778*/

    /*设置看门狗:设置超时间隔寄存器*/
	EXC_WRITE_REG(WDT_REG_TORR,0x0);

    /*设置看门狗工作模式:第一次立刻复位*/
	EXC_READ_REG(WDT_REG_CR, u32Val);
    u32Val &= ~(1<<1);
    EXC_WRITE_REG(WDT_REG_CR, u32Val);

    /*设置系统控制器:WDG使能*/
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE),INTEGRATOR_SC_WDTCTRL_OFFSET,3,1,1);

	/*设置看门狗:WDG使能*/
    EXC_READ_REG(WDT_REG_CR, u32Val);
    u32Val |= 1<<0;
    EXC_WRITE_REG(WDT_REG_CR, u32Val);

    /* 喂狗 */
    EXC_WRITE_REG(WDT_REG_CRR, 0x76);
    #else
	#endif
}/*lint !e529*/
/*************************************************
 函 数 名       : systemError
 功能描述   :  systemError
 输入参数   : 
 输出参数   :
 返 回 值      :  

 调用函数   :
 被调函数   :

 修改历史   :
    日    期       : 2011年10月28日
    作    者       : C54299
    修改内容 : linux modify

*************************************************/
void systemError(int modId, int arg1, int arg2, char * arg3, int arg3Length)
{

    unsigned int len = 0;
    int taskNum = 0;
    int i = 0;
    struct task_struct  * ptcb = NULL;
    unsigned int * taskAllName=(unsigned int *)EXCH_CPU_VIEW_ADDR;
    /*unsigned int * taskAllTCB=(unsigned int *)EXCH_ALL_TASK_TCB_ADDR; */
    unsigned int ulSize = 0x00;
    unsigned int ulOffsetSize = 0x00;  
    unsigned int log_end_index = 0;
    unsigned int pid_ppid = 0;
    struct task_struct  *currentTaskTcb = 0;
    int ret;
	/* wuzechun - if OM is not inited, reboot */
	if (g_stMntnInitFlag != 0x5aaaaa) {

		wdtReboot();
		return;
	}

#if((FEATURE_ON == FEATURE_LTE) && (FEATURE_ON == FEATURE_EPAD || FEATURE_ON == FEATURE_PHONE))
    /* 设置LOG2.0开关状态 */
    exchSetSocpLogOnFlag();
    /* 保存SOCP编码目的buffer信息 */
    if(TRUE == g_pstSocpLogExcInfo->logOnFlag)
    {
        ret = exchSetSocpLogInfo();
        /* 保存失败，清除使能标志，复位后不保存文件 */
        if(BSP_OK != ret)
        {
            g_pstSocpLogExcInfo->logOnFlag = FALSE;
        }      
    }
#endif    
    
    preempt_disable();

    //如果是上层复位，标志复位状态
    if(EXCH_S_UNDEF == g_pExchRoot->whyReboot)
    {
        g_pExchRoot->whyReboot = EXCH_S_NORMAL;
    }

    if (!((EXCH_S_ARM == arg1) && ( BSP_MODU_MNTN == modId)))
    {
        
        /*保存异常时mem 及task stack信息*/
        printk("#####################show mem and current task stack start################################!\n");
        show_mem();
        if(modId&(1<<24))
        {
            show_stack(find_task_by_vpid(arg1),NULL);
        }
        else
        {
            show_stack(NULL,NULL);
        }
        printk("#########################show mem and current task stack end##############################!\n");    

        log_end_index = ((log_end)&(__LOG_BUF_LEN-1));       
                
        if (log_buf_len <= EXCH_LINUX_PRINTK_INFO_SIZE)
        {
            memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR), (const void * )(log_buf), log_buf_len);
        }
        else if ( log_end_index >= EXCH_LINUX_PRINTK_INFO_SIZE)
        {
             memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR), (const void * )(log_buf + log_end_index - EXCH_LINUX_PRINTK_INFO_SIZE), EXCH_LINUX_PRINTK_INFO_SIZE);
        } 
        else
        {
            ulSize = log_end_index;
            ulOffsetSize = EXCH_LINUX_PRINTK_INFO_SIZE - ulSize;            
            memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR), (const void * )(log_buf), ulSize);
            memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR+ulSize), (const void * )(log_buf+log_buf_len-ulOffsetSize), ulOffsetSize);
            
        }  
        
    } 
 
    ptcb = (struct task_struct  *)g_pExchRoot->taskId;
    
    /*保存定制信息*/
    if (NULL != arg3)
    {
    	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
        len = (arg3Length > EXCH_CUSTOM_SIZE) ? EXCH_CUSTOM_SIZE : (unsigned int)arg3Length;
		/*end*/
        memcpy((void *)g_pExchRoot->customAddr, (const void * )arg3, (size_t)len);
    }

    /*保存A核的GIC的状态寄存器*/
    reg_save(EXCH_A_GIC_STATUS_ADDR, GIC_ICSPISR ,8);  
    
    /* 保存任务TCB、堆栈、异常堆栈到异常信息区 */
    
    memcpy((void * )g_pExchRoot->taskTcbAddr , (const void * )ptcb, (size_t )EXCH_TASK_TCB_SIZE);
    memcpy((void * )g_pExchRoot->taskName, (const void * )ptcb->comm, (size_t )0x10);
    /*保存中断栈数据*/
    /*memcpy((void * )g_pExchRoot->intStackAddr, (const void * )vxIntStackEnd, (size_t )(EXCH_INT_STACK_SIZE));*/
    
    /*len = ((ptcb->pStackBase - ptcb->pStackEnd) > EXCH_TASK_STACK_SIZE) ? EXCH_TASK_STACK_SIZE : (ptcb->pStackBase - ptcb->pStackEnd);*/
    
    memcpy((void * )g_pExchRoot->taskStackAddr, (const void * )(ptcb->stack), (size_t )THREAD_SIZE);
            
    if((taskNum = task_id_list_get()) != -1)
    {
        /* 保存所有任务名及TCB*/
        for (i=0; i<(int)taskNum; i++)
        {
            *(unsigned int *)(taskAllName)         = (unsigned int)taskPidList[i];
           /* *(unsigned int *)(taskAllTCB)          = (unsigned int)taskIdList[i];*/
            taskAllName++;
            /*taskAllTCB++;*/
            
            /*拷贝任务名*/
            strncpy((void * )taskAllName, ((struct task_struct * )taskIdList[i])->comm, 11);
            taskAllName += ((sizeof(TASK_NAME_STRU) - sizeof(unsigned int))/(sizeof(unsigned int)));
          
            #if 0
            /*拷贝TCB*/ 
            memcpy((void * )taskAllTCB, (const void * )taskIdList[i],  (size_t )(EXCH_TASK_TCB_SIZE-sizeof(unsigned int)));
            taskAllTCB += ((EXCH_TASK_TCB_SIZE - sizeof(unsigned int))/sizeof(unsigned int));    
            #endif
          
        
        }
    
    }
           
    
    /*设置异常文件记录标志*/
    
    *(volatile unsigned int *) EXCH_A_CORE_FLAG_ADDR = EXCH_A_CORE_FLAG;
    
    currentTaskTcb = (struct task_struct *)task_id_current_get();    
    pid_ppid = PID_PPID_GET((int)currentTaskTcb);
    g_pExchRoot->modId = (unsigned int)modId;
    g_pExchRoot->arg1 = arg1;
    g_pExchRoot->arg2 = arg2;
    g_pExchRoot->rebootTask = pid_ppid;
    g_pExchRoot->rebootTicks = omTimerTickGet();
    g_pExchRoot->omTimer = omTimerGet();    
    
    /*OSAL_CacheFlush(OSAL_DATA_CACHE, g_pExchRoot, EXCH_EXC_BIN_SIZE);*/     

    
    /*ddrRamRefresh();*/
    usrPlatformReset();
	//先通过APP_TIMER_DOG方式，通知mcore中断并记录文件，失败则直接调用开门狗重启
//	if(BSP_WDT_TimerReboot() != OK)
	    wdtReboot();

}

void exchTaskSwitchHook(void *pOldTcb, void *pNewTcb)
{
    /* 将要切换的任务ID和时间入队*/

    unsigned int pid_ppid = 0;   

    pid_ppid = PID_PPID_GET((int)pNewTcb);
          
    QueueLoopIn((Queue *)EXCH_TASK_SWITCH_ADDR, (unsigned int)pid_ppid);
    QueueLoopIn((Queue *)EXCH_TASK_SWITCH_ADDR, omTimerGet());

    QueueLoopIn( (Queue *)EXCH_A_TASK_SWITCH_PID_PPID_ADDR, (unsigned int)pid_ppid);    
    QueueLoopIn((Queue *)EXCH_A_TASK_SWITCH_PID_PPID_ADDR, *((int *)(((struct task_struct *)(pNewTcb))->comm)));
    QueueLoopIn((Queue *)EXCH_A_TASK_SWITCH_PID_PPID_ADDR, *((int *)((((struct task_struct *)pNewTcb)->comm)+4)));
    QueueLoopIn((Queue *)EXCH_A_TASK_SWITCH_PID_PPID_ADDR, *((int *)((((struct task_struct *)pNewTcb)->comm)+8)));
    
    g_pExchRoot->taskId = (unsigned int)pNewTcb;
    g_pExchRoot->taskTicks = omTimerTickGet();
    
}


void exchIntHook(unsigned int dir, unsigned int oldVec, unsigned int newVec)
{

    /* 将触发的中断ID和时间入队*/
    if (0 == dir)/*中断进入*/
    {
  
        /*QueueLoopIn((Queue *)(g_pExchRoot->intAddr), newVec);*/
        QueueLoopIn((Queue *)EXCH_INT_ADDR, newVec);
    }
    else/*中断退出*/
    {
        /*QueueLoopIn((Queue *)(g_pExchRoot->intAddr), EXCH_INT_EXIT);*/
        QueueLoopIn((Queue *)EXCH_INT_ADDR, EXCH_INT_EXIT);
    }

    /*QueueLoopIn((Queue *)(g_pExchRoot->intAddr), sysClkTicksGet());*/

    /*QueueLoopIn((Queue *)(g_pExchRoot->intAddr), omTimerGet());*/
    QueueLoopIn((Queue *)EXCH_INT_ADDR, omTimerGet());
    g_pExchRoot->intId = newVec;
    g_pExchRoot->intTicks = omTimerTickGet();
}


void exchExcHook(void * currentTaskId, int vec, void * pReg)
{   
    unsigned int ulSize = 0x00;
    unsigned int ulOffsetSize = 0x00;
    
    g_pExchRoot->vec = (unsigned int)vec;
    g_pExchRoot->cpsr = *((unsigned int *)&pReg[16]);/*lint !e124*/
    g_pExchRoot->taskId = (unsigned int)currentTaskId;
    g_pExchRoot->taskTicks = omTimerTickGet();
    g_pExchRoot->whyReboot = EXCH_S_ARM;

	preempt_disable();/*lint !e26 !e515*/

    /*拷贝寄存器信息*/
    memcpy((void * )(g_pExchRoot->regSet), (const void * )(pReg), (size_t )(18*4));

    /*拷贝复位前信息至异常信息区*/
    if(g_ulExcLogEnd >= g_ulExcLogStart)
    {
        /*log_buf未翻转log连续直接拷贝即可*/
        ulSize = ((g_ulExcLogEnd -g_ulExcLogStart) > EXCH_LINUX_PRINTK_INFO_SIZE ) ? 
        (EXCH_LINUX_PRINTK_INFO_SIZE) : (g_ulExcLogEnd -g_ulExcLogStart);
        memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR), (const void * )(log_buf+g_ulExcLogStart), ulSize);
      
    }
    else
    {
        /*log_buf 已翻转log不连续需分段拷贝*/
        ulSize = ((log_buf_len -(volatile int)g_ulExcLogStart +(volatile int)g_ulExcLogEnd) > EXCH_LINUX_PRINTK_INFO_SIZE ) ?
            (EXCH_LINUX_PRINTK_INFO_SIZE) : ((log_buf_len -(volatile int)g_ulExcLogStart +(volatile int)g_ulExcLogEnd));     /*lint !e732*/
        if(ulSize < EXCH_LINUX_PRINTK_INFO_SIZE)
        {
            memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR), (const void * )(log_buf+g_ulExcLogStart), (log_buf_len -(volatile int)g_ulExcLogStart));   /*lint !e732*/
            memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR+(unsigned int)log_buf_len - g_ulExcLogStart), (const void * )(log_buf), (g_ulExcLogEnd));
        }   
        else
        {
            ulOffsetSize = ((log_buf_len -(volatile int)g_ulExcLogStart) >= EXCH_LINUX_PRINTK_INFO_SIZE) 
                ? (EXCH_LINUX_PRINTK_INFO_SIZE) : ((log_buf_len -(volatile int)g_ulExcLogStart));  /*lint !e732*/
            memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR), (const void * )(log_buf+g_ulExcLogStart), ulOffsetSize);  
            if(ulOffsetSize < EXCH_LINUX_PRINTK_INFO_SIZE )
            {
                memcpy((void * )(EXCH_LINUX_PRINTK_INFO_ADDR+ulOffsetSize), (const void * )(log_buf), (EXCH_LINUX_PRINTK_INFO_SIZE -ulOffsetSize));
            }
        }

    }

    /*进行全局复位*/
    systemError(BSP_MODU_MNTN, EXCH_S_ARM, 0, 0, 0);;

}

/******************************************************************************
*  Function:   exchShortDoWork
*  Description:定时器处理中断底半部处理函数，由exchInit调用
*  Called By:  exchInit
*  Input: struct work_struct *w
*  Output:NONE 
*  Return:NONE
********************************************************************************/
void exchShortDoWork(struct work_struct *w)
{
	BSP_U32 iRet;
	
	iRet = (BSP_U32)(BSP_SYNC_Wait(SYNC_MODULE_MNTN, 100000));
	if(iRet)
	{
		printk("exchInit sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
		return;
	}

	/*检查C核标志位查看C核是否完成初始化*/
	if ( *(volatile unsigned int *)(EXCH_C_CORE_INIT_END_FLAG_ADDR)== EXCH_C_CORE_INIT_END_FLAG )
	{
		*(volatile unsigned int *)(EXCH_C_CORE_INIT_END_FLAG_ADDR)= 0x0;
	}
	else
	{
		printk("exchInit is error!######################\r\n");
		return ;
	}

    /*保留产品和版本信息*/
    g_pExchRoot->product = 3;
    memcpy((void * )g_pExchRoot->version, (const void * )LINUX_VERSION, (size_t )strlen(LINUX_VERSION));

    /*清空异常信息区*/
    //memset((void *)EXCH_AREA_ADDR, 0, (size_t )EXCH_A_CORE_SIZE);

    /*分配异常信息区*/
    /*g_pExchRoot->osMsgAddr           = EXCH_OS_MSG_ADDR;*/
    g_pExchRoot->taskTcbAddr         = EXCH_TASK_TCB_ADDR;
    g_pExchRoot->customAddr          = EXCH_CUSTOM_ADDR;
    g_pExchRoot->intAddr                 = EXCH_INT_ADDR;
    /*g_pExchRoot->subintAddr            = EXCH_SUBINT_ADDR;*/
    g_pExchRoot->taskSwitchAddr     = EXCH_TASK_SWITCH_ADDR;
    g_pExchRoot->taskStackAddr      = EXCH_TASK_STACK_ADDR;
    g_pExchRoot->intStackAddr         =EXCH_INT_STACK_ADDR;
    /*g_pExchRoot->taskExcStackAddr = EXCH_TASK_EXCSTACK_ADDR;*/
    g_pExchRoot->cpuViewAddr        = EXCH_CPU_VIEW_ADDR;
    g_pExchRoot->omLogAddr          = EXCH_A_OM_LOG_ADDR;
    g_pExchRoot->omMsgAddr          = EXCH_A_OM_MSG_ADDR;
    /*g_pExchRoot->sdExcAddr            = EXCH_SD_ADDR;*/
   /* g_pExchRoot->sciExcAddr            = EXCH_A_SCI_ADDR;*/
    g_pExchRoot->usbExcAddr           = EXCH_A_USB_ADDR;
    /*g_pExchRoot->taskAllTcbAddr       = EXCH_ALL_TASK_TCB_ADDR;*/
    g_pExchRoot->excSize = EXCH_A_CORE_SIZE;

    /*生成循环队列*/
    QueueInit((Queue *)(g_pExchRoot->intAddr), (EXCH_INT_SIZE-0x10)/0x4);
    QueueInit((Queue *)(g_pExchRoot->taskSwitchAddr), (EXCH_TASK_SWITCH_SIZE-0x10)/0x4);
    QueueInit((Queue *)(EXCH_A_TASK_SWITCH_PID_PPID_ADDR), (EXCH_A_TASK_SWITCH_PID_PPID_SIZE-0x10)/0x4);

    /*向操作系统注册钩子函数*/
    task_switch_hook_add((FUNC_VOID)exchTaskSwitchHook);
    int_switch_hook_add((FUNC_VOID)exchIntHook);
    exc_hook_add((FUNC_VOID) exchExcHook);
	g_stMntnInitFlag = 0x5aaaaa;
}

void exchInit(unsigned long data)
{
	/*延时操作交给中断底半部处理*/
	schedule_work(&exch_work);
}

unsigned int exchMemMalloc(UINT32 ulSize)
{
    return (ulSize > EXCH_OSA_SIZE) ? ((unsigned int)NULL) : ((unsigned int)EXCH_OSA_ADDR);
}


struct timer_list g_time;  
int exchInitTimer(void)
{

    INIT_WORK(&exch_work,exchShortDoWork);
	
    init_timer(&g_time);
    g_time.data = 0;
    g_time.function = exchInit;
	g_time.expires = jiffies+ 1*HZ; //40 ->1
    add_timer(&g_time);
    return 0;
}
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) || \
	(defined (BOARD_FPGA) && defined (VERSION_V3R2)) ||\
	(defined (BOARD_SFT) && defined (VERSION_V3R2))||\
	((defined (BOARD_ASIC)||defined (BOARD_FPGA_P500)) \
	  && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))

 //V7R1 B070 暂不使能
module_init(exchInitTimer);
#endif

#if defined (VERSION_V3R2)
int getCdromFlag (void)
{

    UINT32 ucTmp = 0;
    ucTmp = *(volatile UINT32 *)(TCM_FOR_ISO_LOAD_FLAG_ADDR);
    printk("getCdromFlag: TCM_FOR_ISO_LOAD_FLAG_ADDR=%d\n", ucTmp);
    if( ucTmp == TCM_FOR_ISO_LOAD_FLAG_VAL )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
#else
int getCdromFlag (void)
{    
    return 1;
}
#endif

#if((FEATURE_ON == FEATURE_LTE) && (FEATURE_ON == FEATURE_EPAD || FEATURE_ON == FEATURE_PHONE))
/*****************************************************************************
  Function    : exchSetSocpLogOnFlag
  Description : 保存LOG2.0启用状态
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL
*****************************************************************************/
void exchSetSocpLogOnFlag()
{
    SOCP_ENC_DST_BUF_LOG_CFG_STRU * logCfg;
    
    logCfg = BSP_SOCP_GetLogCfg();
    g_pstSocpLogExcInfo->logOnFlag = logCfg->logOnFlag;
}

/*****************************************************************************
  Function    : exchSetSocpLogInfo
  Description : 保存LOG2.0复位信息，SOCP编码目的buffer数据信息
  Calls       : 
  Called By   : 
  Input       : SOCP_LOG_EXC_INFO_STRU * logInfo
  Output      : NULL
  Return      : int     BSP_OK:成功,BSP_ERROR:失败
*****************************************************************************/
int exchSetSocpLogInfo()
{
    int ret;

    ret = BSP_SOCP_SaveEncDstBufInfo(g_pstSocpLogExcInfo);
    if(BSP_OK != ret)
    {
        printk("exchSetSocpLogInfo: save socp info fail\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}
#endif

EXPORT_SYMBOL(systemError);