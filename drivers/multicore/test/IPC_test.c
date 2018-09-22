 
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

/*M����A�˷�һ���ж�0��A���Ѿ�ʹ�ܸ��ж�,����A�������У�����M����*/
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
        printk("�����жϴ�������뷢�жϴ��������.\n");
        BSP_IPC_SpinUnLock(g_SingalNum0);
        return -1;
    }

    BSP_IPC_SpinUnLock(g_SingalNum0);

    b_result = OK;
    return BSP_OK;

}

/*M����A�˷�һ���ж�0��A��δʹ�ܸ��ж�,����A�������У�����M����*/
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
/*M����A�˷�һ���ж�0��A��ʹ�ܸ��жϣ���δ�ҽ��жϻص�����,����A�������У�����M����*/
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

/*M����A�˷�����ж�0��1��2��A��ʹ�ܸ��жϣ�����A�������У�����M����*/
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

/*M�˺�A��ͬʱ��һ���ж�0��A�˺�M���Ѿ�ʹ�ܸ��ж�*/
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

/*AM�жϻ�����������һ���ж�*/
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

/*M�˺�A��ͬʱ����ͨ�ж�0��1��2��A�˺�M���Ѿ�ʹ�ܸ��жϣ���������ͬ�ж�*/
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

/*AM�жϻ���*/
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

/*AM�жϻ���,ȥʹ��֮�󲻻����ϱ��жϣ�������BSP_IPC_ST_INT_AM001*/
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
     /*ȥʹ��֮���ٷ��жϣ������ϱ��ж�*/
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

/*AM�жϻ���,disconnect�жϴ�����֮�󲻻��ٽ����жϴ��������������ж��ϱ���������BSP_IPC_ST_INT_AM001*/
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

    /*disconnect�жϴ�����֮���ٷ��жϣ��жϴ������е�ͳ��ֵ����������*/
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


/*���Զ�ʱ�����жϣ�ʱ����Ϊ1sm���ɵ�*/
int IPC_UsrClkSendIsr(void)
{
    up(&g_IpcUsrClkSem);
    return OK;
}


/*****************************************************************************
* �� �� ��  : IPC_UsrClk_Send
*
* ��������  :�ҽӺ����ж�
*
* �������  : SendRate: �жϼ��
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  :
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
       // printk(">>>line %d FUNC %s fail��s32ret = %d\n", (int)__LINE__, (int)__FUNCTION__, s32ret, 0, 0, 0);
       printk(">>>line %d FUNC %s fail��s32ret = %d\n");
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
    printk("�����жϴ������Ĵ���: %d \n", g_IntConnectTimes[g_CoreNum]);
    b_result = BSP_OK;
    return OK;
}

/*A�˺�M��ͬʱ����һ���ź���*/
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

/*A�˺�M��ͬʱ���벻ͬ�ź���*/
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

/*M�˷�������һ���ź��������ź���һֱ��A��ռ��,����A������*/
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

/*M����A�˷�����ж�0��1��2��A��ʹ�ܸ��жϣ�����A�������У�����M����,ѹ������,�ȵ���BSP_IPC_ST_INT_AM002*/
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

    printk("���뵽�жϴ������Ĵ���:%d\n", g_IntConnectTimes[g_CoreNum]);

    for (i = 0; i < INT_NUM; i++)
    {
        BSP_IPC_IntDisable((IPC_INT_LEV_E)i);

        BSP_IPC_IntDisonnect((IPC_INT_LEV_E)i, (VOIDFUNCPTR)IPC_Int_Test, 0);
    }

    printk("���뵽�жϴ������Ĵ���:%d\n", g_IntConnectTimes[g_CoreNum]);
    for (i = 0; i < INT_NUM; i++)
    {
        u32ret += g_stIpc_debug.u32IntSendTimes[i];
    }

    printk("�����жϵĴ���:%d\n", u32ret);

    b_result = OK;
    return BSP_OK;
}

/*A�˺�M��ͬʱ���벻ͬ�ź���*/
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
            printk("SemTake����:%d, SemGive����:%d \n", g_stIpc_debug.u32SemTakeTimes[j],
                   g_stIpc_debug.u32SemGiveTimes[j]);
            printk("BSP_IPC_ST_SEM_STRESS success.\n");
            continue;
        }
        else
        {
            printk("SemTake����:%d, SemGive����:%d \n", g_stIpc_debug.u32SemTakeTimes[j],
                   g_stIpc_debug.u32SemGiveTimes[j]);
            printk( "BSP_IPC_ST_SEM_STRESS failed.\n");
            return BSP_ERROR;
        }
    }

    return BSP_OK;
}

/*spinlockѹ������*/
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

    printk("�����ź����ɹ�����:%d\n", u32takeSuccess);
    return OK;
}

BSP_S32 BSP_IPC_GetResult(void)
{
    return b_result;
}
