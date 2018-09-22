#include <linux/kernel.h>
#include <linux/delay.h>
#include "BSP.h"
#include <mach/common/bsp_memory.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <mach/spinLockAmp.h>
#include <mach/hardware.h>




#define IFC_MODULE_LEN    4
#define IFC_MSGBODY_LEN   32
#define IFC_MSGBUFF_LEN   ((IFC_MODULE_LEN) + (IFC_MSGBODY_LEN))
#define IFC_ST_MODULE_NUM 6
#define TASK_STACK        (10000)


BSP_U32 g_u32IfcStInit = FALSE;
BSP_U32 g_u32MsgBuffer[IFC_ST_MODULE_NUM];
BSP_U32 g_u32MsgTotalBuffer = NULL;
BSP_U32 g_u32IfcSendTaskId[IFC_ST_MODULE_NUM];
BSP_U32 g_u32IfcSendTaskTimes[IFC_ST_MODULE_NUM];
BSP_U32 g_u32IfcSendTaskFailTimes[IFC_ST_MODULE_NUM];
BSP_U32 g_u32IfcSendTaskRun[IFC_ST_MODULE_NUM];

BSP_U32 g_u32IfcRcvTaskTimes[IFC_ST_MODULE_NUM];
BSP_U32 g_u32IfcRcvTaskFailTimes[IFC_ST_MODULE_NUM];


BSP_S32 CalcMsgData(const void* pbuffer, BSP_U32 u32Count,int size);
BSP_S32 MsgBodyCmp(const void* pbuffer,BSP_U32 u32Count,int size);
BSP_S32 IFC_ST_UnInit();
BSP_S32 SendIfcMsg(BSP_U32 u32Count, BSP_U32 u32Times);



BSP_S32 PS_IfcCallback(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 u32Count = 0;
    BSP_S32 s32Result = 0;
    u32Count = *(BSP_U32 *)pMsgBody;

    s32Result = MsgBodyCmp((BSP_U8*)((BSP_U32)pMsgBody + sizeof(BSP_U32)),u32Count,(u32Len - sizeof(BSP_U32)));
    if (OK != s32Result)
    {
        g_u32IfcRcvTaskFailTimes[IFC_MODULE_LPS]++;
        printk(">>>line %d FUNC %s fail，s32Result = %d\n",(int)__LINE__,__FUNCTION__,s32Result);	
        return ERROR;
    }

    g_u32IfcRcvTaskTimes[IFC_MODULE_LPS]++;
    return OK;
}

BSP_S32 IFC_ST_Init(void)
{    
    BSP_U32 u32Result = 0;
    IFC_MODULE_E enModuleId = IFC_MODULE_LPS;
    BSP_U8 * pBuffer = NULL;
    BSP_U8 * pMspBody = NULL;
    BSP_U32 i;
    
    if(TRUE == g_u32IfcStInit)
    {
        return OK;
    }

    IFC_Init();
    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  

    memset(g_u32IfcSendTaskId,0x0,sizeof(g_u32IfcSendTaskId));
    memset(g_u32IfcSendTaskTimes,0x0,sizeof(g_u32IfcSendTaskTimes));
    memset(g_u32IfcSendTaskFailTimes,0x0,sizeof(g_u32IfcSendTaskFailTimes));
    memset(g_u32IfcSendTaskRun,0x1,sizeof(g_u32IfcSendTaskRun));

    u32Result = BSP_IFC_RegFunc(enModuleId, PS_IfcCallback);
    if (OK != u32Result)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);	
        return ERROR;
    }

    pBuffer = kmalloc(IFC_MSGBUFF_LEN * IFC_ST_MODULE_NUM,GFP_KERNEL);
    if (NULL == pBuffer)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);	
        IFC_ST_UnInit();
        return ERROR;
    }

    g_u32MsgTotalBuffer = (BSP_U32)pBuffer;
    
    for (i = 0; i < IFC_ST_MODULE_NUM; i++)
    {
        g_u32MsgBuffer[i] = g_u32MsgTotalBuffer + i * IFC_MSGBUFF_LEN;
        pMspBody = (BSP_U8 *)(g_u32MsgBuffer[i] + IFC_MODULE_LEN);
        *(BSP_U32 *)pMspBody = i;
        CalcMsgData((BSP_U8 *)(g_u32MsgBuffer[i] + IFC_MODULE_LEN + sizeof(BSP_U32)), 
            i,(IFC_MSGBODY_LEN - sizeof(BSP_U32)));
    }
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  

    g_u32IfcStInit = TRUE;
    return OK;
}

BSP_S32 IFC_ST_UnInit(void)
{    
    BSP_U32 u32Result = 0;
    IFC_MODULE_E enModuleId = IFC_MODULE_LPS;
        
    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  
    u32Result = BSP_IFC_RegFunc(enModuleId, NULL);
    if (OK != u32Result)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);	
        return ERROR;
    }
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  

    if (g_u32MsgTotalBuffer)
    {
        kfree ((BSP_U8 *)g_u32MsgTotalBuffer);
    }
    g_u32IfcStInit = FALSE;
    return OK;
}


BSP_S32 IFC_ST_RegCallBack(void)
{
    BSP_U32 u32Result = 0;
    IFC_MODULE_E enModuleId = IFC_MODULE_LPS;
    
    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  
    u32Result = BSP_IFC_RegFunc(enModuleId, PS_IfcCallback);
    if (OK != u32Result)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);	
        return ERROR;
    }
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}

BSP_S32 IFC_ST_SendMsg001(void)
{
    BSP_S32 u32Result = 0;
    
    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  

    u32Result = IFC_ST_Init();
    if (OK != u32Result)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);	
        return ERROR;
    }
    
    u32Result =  SendIfcMsg(0,1);
    if (OK != u32Result)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);	
        return ERROR;
    }
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}

/*输入错误模块枚举*/
BSP_S32 IFC_ST_RegFuncWrong001(void)
{
    /* Pre-condition initialization */
    /* Initializing argument 1 (enModuleId) */ 
    IFC_MODULE_E _enModuleId  = IFC_MODULE_BUTT;
    /* Initializing argument 2 (pFunc) */ 
    BSP_IFC_REG_FUNC _pFunc  = PS_IfcCallback ;
    BSP_U32 _return;
    
    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  
    /* Tested function call */
    _return  = BSP_IFC_RegFunc(_enModuleId, _pFunc);
    if (OK == _return)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,_return);	
        return ERROR;
    }
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}

/*pMspBody参数为NULL*/
BSP_S32 IFC_ST_SendWrong001(void)
{
    /* Pre-condition initialization */
    /* Initializing argument 1 (pMspBody) */ 
    BSP_VOID * _pMspBody  = NULL ;
    /* Initializing argument 2 (u32Len) */ 
    BSP_U32 _u32Len  = 100;
    BSP_U32 _return = 0;
   
    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  

    /* Tested function call */
    _return  = BSP_IFC_Send(_pMspBody, _u32Len);
    if (OK == _return)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,_return);    
        return ERROR;
    }
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}

/*u32Len长度为0*/
BSP_S32 IFC_ST_SendWrong002(void)
{
    /* Pre-condition initialization */
    /* Initializing argument 1 (pMspBody) */ 
    BSP_VOID * _pMspBody  = NULL ;
    /* Initializing argument 2 (u32Len) */ 
    BSP_U32 _u32Len  = 0;
    BSP_U32 _return = 0;
   
   printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  

    _pMspBody = kmalloc(_u32Len,GFP_KERNEL);
    if(NULL == _pMspBody)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,_return);    
        return ERROR;
    }   
    
    *(BSP_U32*)_pMspBody = IFC_MODULE_GUOM;

    /* Tested function call */
    _return  = BSP_IFC_Send(_pMspBody, _u32Len);
    if (OK == _return)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,_return);    
        return ERROR;
    }
    kfree(_pMspBody);
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}

/*输入模块ID非法*/
BSP_S32 IFC_ST_SendWrong003(void)
{
    /* Pre-condition initialization */
    /* Initializing argument 1 (pMspBody) */ 
    BSP_VOID * _pMspBody  = NULL ;
    /* Initializing argument 2 (u32Len) */ 
    BSP_U32 _u32Len  = 100;
    BSP_U32 _return = 0;
   
   printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  

    _pMspBody = kmalloc(_u32Len,GFP_KERNEL);
    if(NULL == _pMspBody)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,_return);    
        return ERROR;
    }   
    
    *(BSP_U32*)_pMspBody = IFC_MODULE_BUTT;
    
    /* Tested function call */
    _return  = BSP_IFC_Send(_pMspBody, _u32Len);    
    if (OK == _return)
    {
        printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,_return);    
        return ERROR;
    }
    
    kfree(_pMspBody);
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}
/* CPPTEST_TEST_CASE_END test_BSP_IFC_Send_5 */

BSP_S32 IFC_ST_PerfSendTest(void)
{
    //BSP_S32 u32Result = 0;
    //IFC_MODULE_E enModuleId = IFC_MODULE_LPS;
    //BSP_U8 * pMspBuff = NULL;
    //BSP_U8 * pMspBody = NULL;
    //BSP_U32 u32Len = IFC_MSGBUFF_LEN;
    BSP_U32 i,j;
    
    char taskName[50] = {0};

    printk(">>>line %d FUNC %s begin\n",(int)__LINE__,__FUNCTION__);  

    for(i = 0; i < IFC_ST_MODULE_NUM; i++)
    {        
        sprintf(taskName,"ifcTest%02d",i);
        //g_u32IfcSendTaskId[i] = taskSpawn(taskName,100,
                                //VX_FP_TASK,20000,(FUNCPTR)SendIfcMsg,
                                //(int)i,0,0,0,0,0,0,0,0,0);
        g_u32IfcSendTaskId[i] = kthread_run(SendIfcMsg, NULL,"ifc_thread");
        if(IS_ERR(g_u32IfcSendTaskId[i]))
        {
            for (j = 0; j < i; j++)
            {
                msleep(g_u32IfcSendTaskId[j]*10);
            }
            printk(">>>line %d FUNC %s taskSpawn[%d] fail\n",(int)__LINE__,__FUNCTION__,i);  
            return ERROR;
        }
    }    
    
    printk(">>>line %d FUNC %s end\n",(int)__LINE__,__FUNCTION__);  
    return OK;
}

BSP_S32 IFC_ST_PerfSendTestStop(void)
{
    memset(g_u32IfcSendTaskRun,0x0,sizeof(g_u32IfcSendTaskRun));
    return OK;
}

BSP_S32 CalcMsgData(const void* pbuffer, BSP_U32 u32Count,int size)
{
    BSP_U32 i;
    BSP_U32 u32Buffer = (BSP_U32)pbuffer;
    
    for(i = 0; i < size; i++)
    {
        *(char *)(u32Buffer + i) = (i + u32Count + 1) % 0xff;
    }
	return OK;
}

BSP_S32 MsgBodyCmp(const void* pbuffer,BSP_U32 u32Count,int size)
{
	int i = 0;
	BSP_U32 u32Buffer = (BSP_U32)pbuffer;
	BSP_U32 u32OrigData = g_u32MsgBuffer[u32Count] + IFC_MODULE_LEN + sizeof(BSP_U32);
	
	for(i = 0; i < size; i++)
	{
		if(*((char*)(u32Buffer+i)) != *((char*)(u32OrigData+i)))
		{
			printk("diff_pos:%d,addr1(0x%x):%x,addr2(0x%x):%x\n",i,u32Buffer+i,*(char*)(u32Buffer+i),u32OrigData+i,*(char*)(u32OrigData+i));
			return ERROR;
		}
	}
	return OK;
}

BSP_S32 SendIfcMsg(BSP_U32 u32Count, BSP_U32 u32Times)
{
    BSP_S32 u32Result = 0;
    //IFC_MODULE_E enModuleId = IFC_MODULE_LPS;
    BSP_U8 * pMspBuff = NULL;
    //BSP_U8 * pMspBody = NULL;
    BSP_U32 u32Len = IFC_MSGBUFF_LEN;
    BSP_U32 i;

    pMspBuff = (BSP_U8 *)g_u32MsgBuffer[u32Count]; 

    *(BSP_U32*)pMspBuff = IFC_MODULE_LPS;
    
    if (0 == u32Times)
    {
        while(1 && g_u32IfcSendTaskRun[u32Count])
        {     
            msleep(10);
            g_u32IfcSendTaskTimes[u32Count]++;
            u32Result =  BSP_IFC_Send(pMspBuff, u32Len);
            if (OK != u32Result)
            {
                g_u32IfcSendTaskFailTimes[u32Count]++;
                printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);  
                return ERROR;
            }
        }
    }
    else
    {
        for(i = 0; i < u32Times; i++)
        {                  
            msleep(10);
            if (!g_u32IfcSendTaskRun[u32Count])
            {                
                printk(">>>line %d FUNC %s IFC Send Perf Test Stop!\n",(int)__LINE__,__FUNCTION__);  
                return OK;
            }
            g_u32IfcSendTaskTimes[u32Count]++;
            u32Result =  BSP_IFC_Send(pMspBuff, u32Len);
            if (OK != u32Result)
            {
                g_u32IfcSendTaskFailTimes[u32Count]++;
                printk(">>>line %d FUNC %s fail，u32Result = 0x%x\n",(int)__LINE__,__FUNCTION__,u32Result);  
                return ERROR;
            }
        }
        
    }

    printk(">>>line %d FUNC %s IFC Send Perf Test Stop!\n",(int)__LINE__,__FUNCTION__);  
    
    return OK;
}

BSP_S32 IFC_ST_ShowPerfTestResult(void)
{
    BSP_U32 i;
    for (i = 0; i < IFC_ST_MODULE_NUM; i++)
    {
        printk("\r IFC压力测试任务发送[%d]进入次数:%d\n",i,g_u32IfcSendTaskTimes[i]);
        printk("\r IFC压力测试任务发送[%d]失败次数:%d\n",i,g_u32IfcSendTaskFailTimes[i]);
        
        printk("\r IFC压力测试任务接收[%d]进入次数:%d\n",i,g_u32IfcRcvTaskTimes[i]);
        printk("\r IFC压力测试任务接收[%d]失败次数:%d\n",i,g_u32IfcRcvTaskFailTimes[i]);
    }
    return 0;
}


