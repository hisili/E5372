 
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include <mach/spinLockAmp.h>
#include "BSP.h"
#include <mach/common/bsp_memory.h>
#include <mach/hardware.h>
#include "bsp_drv_ipc.h"

BSP_U32 INT_NUM = 2;
BSP_U32 SINGAL_NUM = 10;

BSP_U32 g_IntNum0 = 0;
BSP_U32 g_IntNum1 = 1;
BSP_U32 g_IntNum2 = 2;

BSP_U32 g_SingalNum0 = 0;

BSP_U32 g_IntConnectTimes[IPC_CORE_BUTTOM]={0};
extern BSP_U32 g_CoreNum;
extern IPC_DEBUG_E g_stIpc_debug;

//SEM_ID g_IpcUsrClkSem = NULL;

struct semaphore g_IpcUsrClkSem;

BSP_S32 g_IpcUsrClkID = -1;
BSP_BOOL b_IntStress  = TRUE;
BSP_BOOL b_SemStress  = TRUE;
BSP_BOOL b_SpinStress = TRUE;
BSP_BOOL b_IntTimer = TRUE;
BSP_S32 b_result = ERROR;

BSP_VOID IPC_Int_Test(void)
{
	g_IntConnectTimes[g_CoreNum]++;
}

/*M核向A核发一个中断0，A核已经使能该中断,先在A核上运行，再在M核上*/
BSP_S32 BSP_IPC_ST_INT_M001(void)
{
    BSP_U32 u32ret;
	g_IntConnectTimes[g_CoreNum] = 0;
   u32ret = BSP_DRV_IPCIntInit();
   if (u32ret!= BSP_OK)
   	{
		printk("BSP_IPC_ST_INT_001 failed.\n");
		return -1;
	}
/*   if (u32ret != BSP_OK)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPC, "BSP_IPC_ST_INT_001 failed.\n", 0, 0, 0, 0, 0, 0);
        b_result = ERROR;
        return BSP_ERROR;
    }
*/

    BSP_IPC_SpinLock(g_SingalNum0);
    u32ret = BSP_IPC_IntConnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test,0);
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_IntConnect failed.\n");
        return -1;
    }

    u32ret = BSP_IPC_IntEnable(g_IntNum0);
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_IntEnable failed.\n");
        return -1;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);
    msleep(20000);



    BSP_IPC_SpinLock(g_SingalNum0);
    BSP_IPC_IntDisable(g_IntNum0);

    BSP_IPC_IntDisonnect((IPC_INT_LEV_E)g_IntNum0,(VOIDFUNCPTR)IPC_Int_Test, 0);
    if (g_IntConnectTimes[g_CoreNum] != 50)
    {
        printk("进入中断处理次数与发中断次数不相等.\n");
        BSP_IPC_SpinUnLock(g_SingalNum0);
        return -1;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);

    b_result = OK;
    return BSP_OK;

}

/*M核向A核发一个中断0，A核未使能该中断,先在A核上运行，再在M核上*/
BSP_S32 BSP_IPC_ST_INT_M002(void)
{
    BSP_U32 u32ret;

    g_IntConnectTimes[g_CoreNum] = 0;
    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_ST_INT_002 failed.\n");
        return -1;
    }


    BSP_IPC_SpinLock(g_SingalNum0);
    u32ret = BSP_IPC_IntConnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test, 0);
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_IntConnect failed.\n");
        return -1;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);
    msleep(20000);



    BSP_IPC_SpinLock(g_SingalNum0);

    //  BSP_IPC_IntDisable(g_IntNum0);

    BSP_IPC_IntDisonnect((IPC_INT_LEV_E)g_IntNum0,(VOIDFUNCPTR)IPC_Int_Test, 0);
    if (g_IntConnectTimes[g_CoreNum] != 0)
    {
        BSP_IPC_SpinUnLock(g_SingalNum0);
        return -1;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);
    b_result = BSP_OK;
    return BSP_OK;

}
/*M核向A核发一个中断0，A核使能该中断，但未挂接中断回调函数,先在A核上运行，再在M核上*/
BSP_S32 BSP_IPC_ST_INT_M003(void)
{
    BSP_U32 u32ret,u32Intimes;

    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_ST_INT_003 failed.\n");
        return -1;
    }

    u32Intimes = g_stIpc_debug.u32IntHandleTimes[g_IntNum0];

    BSP_IPC_SpinLock(g_SingalNum0);
    u32ret = BSP_IPC_IntEnable(g_IntNum0);
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_IntEnable failed.\n");
        return -1;
    }

    g_IntConnectTimes[g_CoreNum] = 0;
    BSP_IPC_SpinUnLock(g_SingalNum0);
    msleep(20000);



    BSP_IPC_SpinLock(g_SingalNum0);
    BSP_IPC_IntDisable(g_IntNum0);

    if (((g_stIpc_debug.u32IntHandleTimes[g_IntNum0] - u32Intimes) != 51) || (g_IntConnectTimes[g_CoreNum] != 0))
    {
        BSP_IPC_SpinUnLock(g_SingalNum0);
        return BSP_ERROR;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);
    b_result = BSP_OK;
    return BSP_OK;

}

/*M核向A核发多个中断0，1，2，A核使能该中断，先在A核上运行，再在M核上*/
BSP_S32 BSP_IPC_ST_INT_M004(void)
{
    BSP_U32 u32ret, i;

    g_IntConnectTimes[g_CoreNum] = 0;
    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_ST_INT_004 failed.\n");
        return -1;
    }

    BSP_IPC_SpinLock(g_SingalNum0);
    for (i = 0; i < INT_NUM; i++)
    {
        u32ret = BSP_IPC_IntConnect((IPC_INT_LEV_E)i, (VOIDFUNCPTR)IPC_Int_Test, 0);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntConnect failed.\n");
            return BSP_ERROR;
        }

        u32ret = BSP_IPC_IntEnable(i);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntEnable failed.\n");
            return BSP_ERROR;
        }
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);
    msleep(20000);



    BSP_IPC_SpinLock(g_SingalNum0);
    for (i = 0; i < INT_NUM; i++)
    {
        BSP_IPC_IntDisonnect((IPC_INT_LEV_E)i,(VOIDFUNCPTR)IPC_Int_Test, 0);
        BSP_IPC_IntDisable(i);
    }

    if (g_IntConnectTimes[g_CoreNum] != 10 * INT_NUM)
    {
        BSP_IPC_SpinUnLock(g_SingalNum0);
        return BSP_ERROR;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);
    b_result = BSP_OK;
    return BSP_OK;

}

/*M核和A核同时发一个中断0，A核和M核已经使能该中断*/
BSP_S32 BSP_IPC_ST_INT_AM001(void)
{
    BSP_U32 u32ret;

    g_IntConnectTimes[g_CoreNum] = 0;
    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    if (u32ret != BSP_OK)
    {
         printk("BSP_IPC_ST_INT_001 failed.\n");
        return BSP_ERROR;
    }

    u32ret = BSP_IPC_IntConnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test, 0);
    if (u32ret != BSP_OK)
    {
         printk("BSP_IPC_IntConnect failed.\n");
        return BSP_ERROR;
    }

    u32ret = BSP_IPC_IntEnable(g_IntNum0);
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_IntEnable failed.\n");
        return BSP_ERROR;
    }
    msleep(200);
    g_IntConnectTimes[g_CoreNum] = 0;
    return OK;
}

/*AM中断互发，反复发一个中断*/
BSP_S32 BSP_IPC_ST_INT_AMSend1(void)
{
    BSP_U32 i, u32ret;

    for (i = 0; i < 100; i++)
    {
    #ifdef PRODUCT_CFG_CORE_TYPE_APP
    u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, g_IntNum0);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntSend failed.\n");
            return BSP_ERROR;
        }
#endif
  msleep(50);
    }
    msleep(10000);
    BSP_IPC_IntDisable(g_IntNum0);

    BSP_IPC_IntDisonnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test, 0);
    if (g_IntConnectTimes[g_CoreNum] != 100)
    {
        return BSP_ERROR;
    }

    b_result = BSP_OK;
    return BSP_OK;
}

/*M核和A核同时发不通中断0，1，2，A核和M核已经使能该中断，反复发不同中断*/
BSP_S32 BSP_IPC_ST_INT_AM002(void)
{
    BSP_U32 u32ret, i;

    g_IntConnectTimes[g_CoreNum] = 0;
    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    if (u32ret != BSP_OK)
    {
        printk("BSP_IPC_ST_INT_001 failed.\n");
        return BSP_ERROR;
    }

    for (i = 0; i < INT_NUM; i++)
    {
        u32ret = BSP_IPC_IntConnect((IPC_INT_LEV_E)i, (VOIDFUNCPTR)IPC_Int_Test, 0);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntConnect failed.\n");
            return BSP_ERROR;
        }

        u32ret = BSP_IPC_IntEnable(i);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntEnable failed.\n");
            return BSP_ERROR;
        }
    }

    return OK;
}

/*AM中断互发*/
BSP_S32 BSP_IPC_ST_INT_AMSend2(void)
{
    BSP_U32 i, j, u32ret;

    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < INT_NUM; j++)
        {
#ifdef PRODUCT_CFG_CORE_TYPE_APP
 u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, j);
            if (u32ret != BSP_OK)
            {
                 printk( "BSP_IPC_IntSend failed.\n");
                return BSP_ERROR;
            }
#endif

            msleep(50);
        }

        msleep(10);
    }

    msleep(10000);
    for (i = 0; i < INT_NUM; i++)
    {
        BSP_IPC_IntDisable(i);

        BSP_IPC_IntDisonnect((IPC_INT_LEV_E)i, (VOIDFUNCPTR)IPC_Int_Test, 0);
    }

    if (g_IntConnectTimes[g_CoreNum] != 100 * INT_NUM)
    {
        return BSP_ERROR;
    }

    b_result = BSP_OK;
    return BSP_OK;
}

/*AM中断互发,去使能之后不会再上报中断，先运行BSP_IPC_ST_INT_AM001*/
BSP_S32 BSP_IPC_ST_INT_AMSend3(void)
{
    BSP_U32 i, u32ret;

    for (i = 0; i < 10; i++)
    {
   #ifdef PRODUCT_CFG_CORE_TYPE_APP
     u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, g_IntNum0);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntSend failed.\n");
            return BSP_ERROR;
        }
#endif

        msleep(50);
    }

    msleep(5000);
    BSP_IPC_IntDisable(g_IntNum0);
    msleep(15000);
     /*去使能之后再发中断，不会上报中断*/
    for (i = 0; i < 10; i++)
    {
#ifdef PRODUCT_CFG_CORE_TYPE_APP
u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, g_IntNum0);
        if (u32ret != BSP_OK)
        {
             printk("BSP_IPC_IntSend failed.\n");
            return BSP_ERROR;
        }
#endif
msleep(50);
    }

    BSP_IPC_IntDisonnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test, 0);
    if (g_IntConnectTimes[g_CoreNum] != 10)
    {
        return BSP_ERROR;
    }

    b_result = BSP_OK;
    return BSP_OK;
}

/*AM中断互发,disconnect中断处理函数之后不会再进入中断处理函数，但仍有中断上报，先运行BSP_IPC_ST_INT_AM001*/
BSP_S32 BSP_IPC_ST_INT_AMSend4_Start(void)
{
    BSP_U32 i, u32ret;

    for (i = 0; i < 10; i++)
    {
#ifdef PRODUCT_CFG_CORE_TYPE_APP
 u32ret= BSP_IPC_IntSend(IPC_CORE_COMARM, g_IntNum0);
        if (u32ret != BSP_OK)
        {
             printk( "BSP_IPC_IntSend failed.\n");
            return BSP_ERROR;
        }
#endif

        msleep(50);
    }

    return OK;
}

BSP_S32 BSP_IPC_ST_INT_AMSend4_Stop(void)
{
    BSP_U32 i, u32ret;

    BSP_IPC_IntDisonnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test, 0);
    msleep(20000);

    /*disconnect中断处理函数之后再发中断，中断处理函数中的统计值不会再增加*/
    for (i = 0; i < 10; i++)
    {
#ifdef PRODUCT_CFG_CORE_TYPE_APP
 u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, g_IntNum0);
        if (u32ret != BSP_OK)
        {
             printk( "BSP_IPC_IntSend failed.\n");
            return BSP_ERROR;
        }
#endif

        msleep(50);
    }

    if (g_IntConnectTimes[g_CoreNum] != 10)
    {
        return BSP_ERROR;
    }
    BSP_IPC_IntDisable(g_IntNum0);

    b_result = BSP_OK;
    return BSP_OK;
}


/*测试定时发送中断，时间间隔为1sm，可调*/
int IPC_UsrClkSendIsr(void)
{
    up(&g_IpcUsrClkSem);
    return OK;
}


/*****************************************************************************
* 函 数 名  : IPC_UsrClk_Send
*
* 功能描述  :挂接毫秒中断
*
* 输入参数  : SendRate: 中断间隔
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  :
*
*****************************************************************************/
/*
int IPC_UsrClk_Send(int SendRate)
{
    BSP_S32 * ps32UsrClkId;

    BSP_USRCLK_Init();
    ps32UsrClkId = (BSP_S32 *)kmalloc(sizeof(BSP_S32),GFP_KERNEL);
    BSP_USRCLK_Alloc(ps32UsrClkId);
    g_IpcUsrClkID = *ps32UsrClkId;

    if (OK != BSP_USRCLK_RateSet(SendRate, g_IpcUsrClkID))
    {
        //printk(">>>line %d FUNC %s fail!\n", (int)__LINE__, __FUNCTION__, 0, 0, 0, 0);
        printk(">>>line %d FUNC %s fail!\n");
        return ERROR;
    }

    if (OK != BSP_USRCLK_Connect((FUNCPTR)IPC_UsrClkSendIsr, 0, g_IpcUsrClkID))
    {
        //printk(">>>line %d FUNC %s fail!\n", (int)__LINE__, (int)__FUNCTION__, 0, 0, 0, 0);
        printk(">>>line %d FUNC %s fail!\n");
        return ERROR;
    }

    if (OK != BSP_USRCLK_Enable(g_IpcUsrClkID))
    {
        printk(">>>line %d FUNC %s fail!\n");
        return ERROR;
    }

    return OK;
}

BSP_S32 IPC_SendFunc(void)
{
    BSP_U32 u32ret;

    while (b_IntTimer)
    {
		if (sema_init(&g_IpcUsrClkSem, SEM_EMPTY) == ERROR)
        {
            printk(">>>line %d FUNC %s fail\n");
            return ERROR;
        }


        u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, g_IntNum0);
        if (u32ret != BSP_OK)
        {
            printk("BSP_IPC_IntSend failed.\n");
            return BSP_ERROR;
        }


    }

    return BSP_OK;
}
*/
/*
BSP_S32 BSP_IPC_ST_INT_AMSend_Timer_Start(void)
{
    BSP_U32 s32ret;

    b_IntTimer = TRUE;
    b_result = ERROR;
    BSP_IPC_ST_INT_AM001();
	
	down(&g_IpcUsrClkSem);
	//g_IpcUsrClkSem = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    if (OK != IPC_UsrClk_Send(1000))
    {
        printk(">>>line %d FUNC %s fail\n");
        return ERROR;
    }

    //s32ret = taskSpawn ("tmIpcSendFunc", 132, 0, 20000, (FUNCPTR)IPC_SendFunc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	s32ret = kthread_run(IPC_SendFunc,NULL,"tmIpcSendFunc");
    if (IS_ERR(s32ret))
    {
       // printk(">>>line %d FUNC %s fail，s32ret = %d\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
       printk(">>>line %d FUNC %s fail，s32ret = %d\n");
        return ERROR;
    }

    return BSP_OK;
}
*/

BSP_S32 BSP_IPC_ST_INT_AMSend_Timer_Stop(void)
{
    b_IntTimer = FALSE;
    msleep(10000);
    BSP_IPC_IntDisable(g_IntNum0);

    BSP_IPC_IntDisonnect((IPC_INT_LEV_E)g_IntNum0, (VOIDFUNCPTR)IPC_Int_Test, 0);
    printk("进入中断处理函数的次数: %d \n", g_IntConnectTimes[g_CoreNum]);
    b_result = BSP_OK;
    return OK;
}

/*A核和M核同时申请一个信号量*/
BSP_S32 BSP_IPC_ST_SEM_001(void)
{
    BSP_U32 i, u32ret;

    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    memset(&g_stIpc_debug, 0, sizeof(g_stIpc_debug));
    if (u32ret != BSP_OK)
    {
        printk("BSP_DRV_IPCIntInit failed.\n");
        return BSP_ERROR;
    }

    BSP_IPC_SemCreate(g_SingalNum0);
    msleep(3000);
    for (i = 0; i < 100; i++)
    {

        BSP_IPC_SemTake(g_SingalNum0, 10000);
        msleep(80);
        BSP_IPC_SemGive(g_SingalNum0);

    }

    //BSP_IPC_SemDelete(g_SingalNum0);
    if ((g_stIpc_debug.u32SemTakeTimes[g_SingalNum0] == 100) && (g_stIpc_debug.u32SemGiveTimes[g_SingalNum0] == 100))
    {
        printk("BSP_IPC_ST_SEM_001 success.\n");
        b_result = BSP_OK;
        return BSP_OK;
    }
    else
    {
        printk("BSP_IPC_ST_SEM_001 failed.\n");
        return BSP_ERROR;
    }
}

/*A核和M核同时申请不同信号量*/
BSP_S32 BSP_IPC_ST_SEM_002(void)
{
    BSP_U32 i, j, u32ret;

    b_result = ERROR;
    u32ret = BSP_DRV_IPCIntInit();
    memset(&g_stIpc_debug, 0, sizeof(g_stIpc_debug));
    if (u32ret != BSP_OK)
    {
        printk("BSP_DRV_IPCIntInit failed.\n");
        return BSP_ERROR;
    }

    for (i = 0; i < INT_NUM; i++)
    {
        BSP_IPC_SemCreate(i);
    }

    msleep(2000);
    for (i = 0; i < 100; i++)
    {
        for (j = 0; j < INT_NUM; j++)
        {

            BSP_IPC_SemTake(j, 10000);
            msleep(50);
            BSP_IPC_SemGive(j);

        }
    }
/*
    for (i = 0; i < INT_NUM; i++)
    {
        BSP_IPC_SemDelete(i);
    }
*/
    for (j = 0; j < INT_NUM; j++)
    {
        if ((g_stIpc_debug.u32SemTakeTimes[j] == 100) && (g_stIpc_debug.u32SemGiveTimes[j] == 100))
        {
            continue;
        }
        else
        {
            printk("BSP_IPC_ST_SEM_002 failed.\n");
            return BSP_ERROR;
        }
    }

    printk( "BSP_IPC_ST_SEM_002 success.\n");
    b_result = OK;
    return BSP_OK;
}

BSP_VOID IPC_SemTake1(struct semaphore g_IpcUsrClkSem)
{
    printk(">>>line %d FUNC %s BSP_IPC_SemTake\n", (int)__LINE__, __FUNCTION__);
    down(&g_IpcUsrClkSem);
    BSP_IPC_SemTake(g_SingalNum0, 10000);
    printk(">>>line %d FUNC %s BSP_IPC_SemGive\n", (int)__LINE__, __FUNCTION__);
	BSP_IPC_SemGive(g_SingalNum0);
    up(&g_IpcUsrClkSem);
}

BSP_VOID IPC_SemTake2(struct semaphore g_IpcUsrClkSem)
{
    printk(">>>line %d FUNC %s BSP_IPC_SemTake\n",(int)__LINE__, __FUNCTION__);
    down(&g_IpcUsrClkSem);
    BSP_IPC_SemTake(g_SingalNum0,10000);
    printk(">>>line %d FUNC %s BSP_IPC_SemGive\n",(int)__LINE__, __FUNCTION__);
    BSP_IPC_SemGive(g_SingalNum0);
    up(&g_IpcUsrClkSem);
}

BSP_VOID IPC_SemTake3(struct semaphore g_IpcUsrClkSem)
{
    printk(">>>line %d FUNC %s BSP_IPC_SemTake\n",(int)__LINE__, __FUNCTION__);
    down(&g_IpcUsrClkSem);
    BSP_IPC_SemTake(g_SingalNum0, 10000);
    printk(">>>line %d FUNC %s BSP_IPC_SemGive\n",(int)__LINE__, __FUNCTION__);
    BSP_IPC_SemGive(g_SingalNum0);
    up(&g_IpcUsrClkSem);
}

/*M核反复申请一个信号量，此信号量一直被A核占用,先在A核运行*/
BSP_S32 BSP_IPC_ST_SEM_003(void)
{
    
    b_result = ERROR;
    BSP_IPC_SemCreate(g_SingalNum0);
    BSP_IPC_SemTake(g_SingalNum0, 10000);
   	msleep(20000);
    BSP_IPC_SemGive(g_SingalNum0);
    b_result = OK;
    return OK;
}

/*M核向A核发多个中断0，1，2，A核使能该中断，先在A核上运行，再在M核上,压力测试,先调用BSP_IPC_ST_INT_AM002*/
BSP_S32 BSP_IPC_ST_INT_STRESS(void)
{
    BSP_U32 i, j, u32ret=0;

    while (b_IntStress)
    {
        for (j = 0; j < INT_NUM; j++)
        {
            u32ret = BSP_IPC_IntSend(IPC_CORE_COMARM, (IPC_INT_LEV_E)j);
            if (u32ret != BSP_OK)
            {
                printk("BSP_IPC_IntSend failed.\n");
                return BSP_ERROR;
            }

            msleep(50);
        }
    }

    msleep(10000);

    printk("进入到中断处理函数的次数:%d\n", g_IntConnectTimes[g_CoreNum]);

    for (i = 0; i < INT_NUM; i++)
    {
        BSP_IPC_IntDisable((IPC_INT_LEV_E)i);

        BSP_IPC_IntDisonnect((IPC_INT_LEV_E)i, (VOIDFUNCPTR)IPC_Int_Test, 0);
    }

    printk("进入到中断处理函数的次数:%d\n", g_IntConnectTimes[g_CoreNum]);
    for (i = 0; i < INT_NUM; i++)
    {
        u32ret += g_stIpc_debug.u32IntSendTimes[i];
    }

    printk("发送中断的次数:%d\n", u32ret);

    b_result = OK;
    return BSP_OK;
}

/*A核和M核同时申请不同信号量*/
BSP_S32 BSP_IPC_ST_SEM_STRESS(void)
{
    BSP_U32 i, j, u32ret;

    u32ret = BSP_DRV_IPCIntInit();
//	u32ret = kthread_run(BSP_DRV_IPCIntInit,NULL,"tipc_IntInit"); 	
    if (u32ret != BSP_OK)
    {
        printk("BSP_DRV_IPCIntInit failed.\n");
        return BSP_ERROR;
    }

    for (i = 0; i < SINGAL_NUM; i++)
    {
        BSP_IPC_SemCreate(i);
    }

    while (b_SemStress)
    {
        for (j = 0; j < SINGAL_NUM; j++)
        {

            BSP_IPC_SemTake(j, 10000);
            msleep(50);
            BSP_IPC_SemGive(j);
            msleep(50);
        }
    }
/*
    for (i = 0; i < 10; i++)
    {
        BSP_IPC_SemDelete(i);
    }
*/
    for (j = 0; j < SINGAL_NUM; j++)
    {
        if (g_stIpc_debug.u32SemTakeTimes[j] == g_stIpc_debug.u32SemGiveTimes[j])
        {
            printk("SemTake次数:%d, SemGive次数:%d \n", g_stIpc_debug.u32SemTakeTimes[j],
                   g_stIpc_debug.u32SemGiveTimes[j]);
            printk("BSP_IPC_ST_SEM_STRESS success.\n");
            continue;
        }
        else
        {
            printk("SemTake次数:%d, SemGive次数:%d \n", g_stIpc_debug.u32SemTakeTimes[j],
                   g_stIpc_debug.u32SemGiveTimes[j]);
            printk( "BSP_IPC_ST_SEM_STRESS failed.\n");
            return BSP_ERROR;
        }
    }

    return BSP_OK;
}

/*spinlock压力测试*/
BSP_S32 BSP_IPC_ST_SPINLOCK_STRESS(void)
{
    BSP_U32 j, u32ret;
    BSP_U32 u32takeSuccess = 0;

    u32ret = BSP_DRV_IPCIntInit();
    if (u32ret != BSP_OK)
    {
        printk( "BSP_DRV_IPCIntInit failed.\n");
        return BSP_ERROR;
    }

    while (b_SpinStress)
    {
        for (j = 0; j < SINGAL_NUM; j++)
        {

            BSP_IPC_SpinLock(j);
            msleep(50);
            u32takeSuccess++;
            BSP_IPC_SpinUnLock(j);

        }
    }

    printk("申请信号量成功次数:%d\n", u32takeSuccess);
    return OK;
}

BSP_S32 BSP_IPC_GetResult(void)
{
    return b_result;
}
