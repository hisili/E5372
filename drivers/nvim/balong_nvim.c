/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_nvim.c
*
*
*   描    述 :  通过核间通信读NV  A核模块
* 
*************************************************************************/
#if defined (FEATURE_FLASH_LESS)
/*Flashless 中不使用此模块*/
#else
#include "BSP.h"
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include "balong_nvim.h"
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/module.h>  //clean warning


typedef struct
{
	BSP_U32 slNvRet;
	BSP_U32 ulDataLength;
	BSP_U16 usNvID;  
    BSP_CHAR scNvData[1];
}NVIM_MSG_GET;

typedef struct
{
    BSP_U16 usNvID;             
    BSP_U32 ulNvLength;
}NVIM_MSG_SET;

typedef struct
{
    BSP_U32 ulModuleId;
    BSP_U32 ulFuncId;
	NVIM_MSG_GET Msg;
}NVIM_IFC_MSG_GET;

typedef struct
{
    BSP_U32 ulModuleId;
    BSP_U32 ulFuncId;
	NVIM_MSG_SET MSG;
}NVIM_IFC_MSG_SET;

BSP_U32 g_ulNvRet = NV_NOT_READY;
BSP_CHAR *g_pNvData = NULL;
BSP_U16 g_usNvID = 0;

struct semaphore g_sem_NV;
struct semaphore g_sem_NV_Read;

/**************************************************************************
  函数声明
**************************************************************************/
BSP_S32 __init balong_nvim_IFCInit(BSP_VOID);/*clean warning*/
BSP_U32 balong_nvim_IFCReceive(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_U32 balong_nvim_IFCSend(BSP_U16 usID,BSP_U32 ulLength);
BSP_U32 BSP_NV_ReadTest(BSP_U16 usID,BSP_U32 ulength);

/*****************************************************************************
 * 函 数 名  :balong_nvim_IFCInit
 *
 * 功能描述  : balong_nvim模块核间通信
 *             A核初始化函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : BSP_OK:    操作成功
 *             BSP_ERROR: 操作失败
 *
 * 其它说明  : 无
 *
 *****************************************************************************/

BSP_S32 __init balong_nvim_IFCInit(BSP_VOID)
{
    BSP_U32 ret = 0; /*clean lint e713*/
	printk("A Core NVIM Init Begin!\n");
    ret = BSP_IFCP_RegFunc(IFCP_FUNC_NVIM_READ_GET,(BSP_IFCP_REG_FUNC)balong_nvim_IFCReceive);
    if(NV_OK != ret)
    {
        printk("NVIM: balong_nvim_IFCInit Fail! ret=%d\n", ret);
        return NV_IFC_ERROR;
    }
	printk("A Core NVIM Init End!\n");
	init_MUTEX_LOCKED(&g_sem_NV);
	init_MUTEX(&g_sem_NV_Read);
	g_pNvData = NULL;
	return NV_OK;
}

/*模块初始化*/




/*****************************************************************************
* 函 数 名  : balong_nvim_IFCReceive
*
* 功能描述  : balong_nvim模块核间通信
*             将C核的NV通过IFC回传
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
BSP_U32 balong_nvim_IFCReceive(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
	//int i  = 0;  //clean warning
    //将IFC传入A核的消息解析
    NVIM_MSG_GET *pMsgGet = (NVIM_MSG_GET *)pMsgBody;

	/* 释放上次NV内存 */
	if(g_pNvData)
	{
		kfree(g_pNvData);
	}
	g_pNvData = NULL;

	/* 分配本次NV内存 */	
	g_usNvID = pMsgGet->usNvID;
    g_ulNvRet = pMsgGet->slNvRet;
	if(NV_OK != g_ulNvRet)
	{
		printk(KERN_ERR"balong_nvim_IFCReceive: Receive from C Core error!usNvID:%d ret:%d\t\n", 
				pMsgGet->usNvID, pMsgGet->slNvRet);
		up(&g_sem_NV);
		return NV_ERROR;
	}

	g_pNvData = kmalloc(pMsgGet->ulDataLength, GFP_KERNEL);
	if(NULL == g_pNvData)
	{
		printk(KERN_ERR"balong_nvim_IFCReceive: kmalloc %d error!usNvID:%d\t\n", 
				pMsgGet->ulDataLength, pMsgGet->usNvID);
		g_ulNvRet = NV_ALLOC_BUFFER_FAIL;
		up(&g_sem_NV);
		return NV_ERROR;
	}

	memcpy(g_pNvData,&(pMsgGet->scNvData[0]),pMsgGet->ulDataLength);

#if 0
	printk(KERN_INFO"balong_nvim_IFCReceive: Receive from C Core OK!\t usNvID:%d ret: %d\n", pMsgGet->usNvID, pMsgGet->slNvRet);
	printk(KERN_INFO"balong_nvim_IFCReceive: data :\n");
	for(i = 0; i < pMsgGet->ulDataLength; i++)
	{
		printk(KERN_INFO"%d\t", pMsgGet->scNvData[i]);
	}
	
	printk(KERN_INFO"\n");
#endif

	up(&g_sem_NV);
	return NV_OK;
}

/*****************************************************************************
* 函 数 名  : balong_nvim_IFCSend
*
* 功能描述  : balong_nvim模块核间通信
*             将A核NV访问通过IFC传到C核
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
BSP_U32 balong_nvim_IFCSend(BSP_U16 usID,BSP_U32 ulLength)
{
    NVIM_IFC_MSG_SET stNvimIFCMsg = {0};
    
    /*消息体变量赋值*/
    stNvimIFCMsg.ulModuleId= IFC_MODULE_BSP;
    stNvimIFCMsg.ulFuncId= IFCP_FUNC_NVIM_READ_SET;
    stNvimIFCMsg.MSG.usNvID= usID;
    stNvimIFCMsg.MSG.ulNvLength= ulLength;

    /*通过IFC将消息发出*/
    if(NV_OK != BSP_IFC_Send((BSP_VOID*)&stNvimIFCMsg, sizeof(NVIM_IFC_MSG_SET)))
    {
        printk("NVIM: IFC Send Fail!\n");
        return NV_IFC_ERROR;
    }
    
	printk("NVIM: IFC Send Success!\n");
    return NV_OK;
}


/*****************************************************************************
* 函 数 名  : DR_NV_Read
*
* 功能描述  : A核读NV接口函数
*
* 输入参数  : usID -> NV ID
              *pItem -> Save NV data buffer
              ulLength -> buffer length
* 输出参数  : 无
*
* 返 回 值  : BSP_OK:    操作成功
*             BSP_ERROR: 操作失败
*
* 其它说明  : 无
*
*****************************************************************************/
int DR_NV_Read(unsigned short usID, void *pItem, unsigned int ulLength)
{       
    int ret = NV_ERROR;
	//int i = 0; //clean warning

	printk(KERN_INFO "NV: ID is %d, pItem is 0x%p, Length is %d\n", usID, pItem, ulLength);//clean warning

    if((NULL == pItem)||(0 == ulLength))
    {
        /* Buffer is Null */
        printk(KERN_ERR "DR_NV_Read: The Buffer is Null!\n");
        return NV_BUFFER_NOT_EXIST;
    }

    ret = BSP_SYNC_Wait(SYNC_MODULE_NV, NV_READ_ACORE_TIMEOUT);
    if (ret)
    {
        printk(KERN_ERR "NV_IFCRead sync wait timeout ret=0x%x, line:%d\n", ret, __LINE__);
        return NV_NOT_READY;
    }
    
	/*wait for 2s*/
    ret = down_timeout(&g_sem_NV_Read, NV_READ_READY_TIMEOUT);
	if(0 != ret)
	{
		printk(KERN_ERR "DR_NV_Read: Last GU NV reading procedure unfinished!\n");
		return NV_READ_TIMEOUT;
	}

   	ret = balong_nvim_IFCSend(usID, ulLength);
   	if(NV_OK != ret)
   	{
   	    printk(KERN_ERR "DR_NV_Read: IFC Send Fail! ret=%d\n", ret);
		up(&g_sem_NV_Read);
   	    return NV_IFC_ERROR;
   	}
	
	/*wait for at most 5s*/
	ret = down_timeout(&g_sem_NV, NV_READ_TRANS_TIMEOUT);
	if(0 != ret)
	{
		printk(KERN_ERR"DR_NV_Read: Read NV On A CPU Time Out!\n");
        up(&g_sem_NV_Read);
		return NV_READ_TIMEOUT;
	}
    
	if(NV_OK == g_ulNvRet)
	{
		memcpy(pItem, g_pNvData, ulLength);
        
#if 0
		printk(KERN_INFO"DR_NV_Read: NV Send to modules ID is %d, pItem is 0x%x, Length is %d\n", usID, pItem, ulLength);
		printk(KERN_INFO"DR_NV_Read: NV Send to modules Value is :\n");
	
		for(i = 0; i < ulLength; i++)
		{
			printk(KERN_INFO"%d\t", *(char *)((char *)pItem+i));
		}
		printk(KERN_INFO"\n");
#endif

		up(&g_sem_NV_Read);
		return NV_OK;
	}
	else
	{
    	printk(KERN_ERR "DR_NV_Read: NV Read Error! error id is %d \n", g_ulNvRet);
		up(&g_sem_NV_Read);
    	return g_ulNvRet;
    }
}

BSP_U32 BSP_NV_ReadTest(BSP_U16 usID,BSP_U32 ulength)
{
    BSP_U32 ret;
	BSP_U32 i = 0;
	char *pnv_data = NULL;
	if(0 == ulength)
	{
		printk("NV_Read_Test: Length is illegal!\n");
		return ~0x0;
	}
	pnv_data = kmalloc(ulength, GFP_KERNEL);
    ret = DR_NV_Read(usID, pnv_data, ulength);
    if (NV_OK != ret)
    {
        printk("NV_Read_Test: NV Read Fail! ret=%d\n", ret);
    }
    else
    {
        printk("NV_Read_Test: NV Read OK!\nNV value is");
		for(i = 0; i < ulength; i++)
		{
			printk("%x\t", *(pnv_data+i));
		}
		printk("\n");
    }
	return 0;
}

EXPORT_SYMBOL( DR_NV_Read);
module_init(balong_nvim_IFCInit);
/*end added*/   

#endif
