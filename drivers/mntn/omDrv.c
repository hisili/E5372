/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  omdrv.c
*
*   作    者 :  yudongbin
*
*   描    述 :  本文件命名为"omdrv.c"
*
*   修改记录 :  2011年9月19日  v1.00  yudongbin创建
*************************************************************************/
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/delay.h>
#include "BSP.h"
#include "omDrv.h"





/**********全局数据区**********/
OM_RSP_FUNC          *g_pOmRspFuncPtr   = NULL;   /*保存OM注册的回调函数*/
DRV_TRACE_PERMISSION *g_pstDrvTracePerm = NULL;   /*标志是否允许发送*/
MPS_PROC_S           g_stMspRegFunc     = {NULL};
struct task_struct   *g_pstDrvProbeOm   = {NULL};

/***********************************************************************************
 Function:          omDrvTraceReqEntry
 Description:       om查询函数
 Calls:
 Input:             pReqPacket, 
                    pRsqFuncPtr
 Output:            pReqPacket, 
                    pRsqFuncPtr
 Return:            NA
 ************************************************************************************/
void BSP_MNTN_OmDrvTraceReqEntry(OM_REQ_PACKET_STRU *pReqPacket,  OM_RSP_FUNC *pRsqFuncPtr)
{
    BSP_U16                rspLength      = 0x0;
    BSP_U16                usReqLen 	  = 0x0;
    DRV_TRACE_REQ_STRU     *pTraceReq     = NULL;
    DRV_TRACE_IND_STRU     *pTraceRsp     = NULL;
    DRV_TRACE_PERMISSION   *pDrvTracePerm = NULL;

#ifdef MNTN_DBG
    printk("#######goin BSP_MNTN_OmDrvTraceReqEntry head!#######\n");
#endif

    if ((NULL == pReqPacket) |(NULL == pRsqFuncPtr))
    {
#ifdef MNTN_DBG
        printk("BSP_MNTN_OmDrvTraceReqEntry para error!\n");
#endif
        return;
    }

    pTraceReq = (DRV_TRACE_REQ_STRU *)pReqPacket;

    switch (pTraceReq->usPrimId)
    {
     case DRV_PRIM_SEND_CONF_REQ:
#ifdef MNTN_DBG
		printk("###########this is goin DRV_PRIM_SEND_CONF_REQ case############\n");
#endif
        g_pOmRspFuncPtr = pRsqFuncPtr;
			 
	    rspLength = sizeof(DRV_TRACE_IND_STRU);
	    if (NULL == (pTraceRsp = (DRV_TRACE_IND_STRU *)kmalloc(rspLength, GFP_KERNEL)))
	    {
            printk("BSP_MNTN_OmDrvTraceReqEntry kmalloc\n");
		    return;
	    }
		
	    pTraceRsp->usPrimId   = DRV_PRIM_SEND_CONF_RSP;
        pTraceRsp->ulModuleId = DRV_MODULE_ID_MNTN;

	    usReqLen = pTraceReq->usLength - (sizeof(DRV_TRACE_REQ_STRU) - 4) + 4;
	    if (usReqLen % sizeof(DRV_TRACE_PERMISSION) != 0)
	    {
            printk("recieve SDT Conf req ERROR\n");
		    memset(pTraceReq->aucData, 0x00, 0x04 );
			kfree(pTraceRsp);
			pTraceRsp = NULL;
			
			return;
	    }

	    if (NULL != g_pstDrvTracePerm)
        {
            kfree(g_pstDrvTracePerm);
		    g_pstDrvTracePerm = NULL;
	    }

	    if (NULL == (g_pstDrvTracePerm = (DRV_TRACE_PERMISSION *)
			         kmalloc(usReqLen + sizeof(DRV_TRACE_PERMISSION), GFP_KERNEL)))
	    {
            printk("omDRV   malloc   permission  error\n");
		    kfree(pTraceRsp);
		    pTraceRsp = NULL;
		    return;
	    }

        /*modified for lint e665 */
	    memset(g_pstDrvTracePerm, 0x0, (usReqLen + sizeof(DRV_TRACE_PERMISSION)));
	    memcpy(g_pstDrvTracePerm, pTraceReq->aucData, usReqLen);
	    pDrvTracePerm = (DRV_TRACE_PERMISSION *)((DRV_TRACE_PERMISSION *)g_pstDrvTracePerm
			                                    + usReqLen / sizeof(DRV_TRACE_PERMISSION));
	    pDrvTracePerm->usPrimId = INVALID_PRIM_ID;
        pDrvTracePerm->usVal    = 0x0000;
	    pDrvTracePerm           = g_pstDrvTracePerm;
	    while(INVALID_PRIM_ID !=pDrvTracePerm->usPrimId)
	    {
	        printk("/-----OM权限表---------/\n");
            printk("primId:%#x,val:%#x\n",pDrvTracePerm->usPrimId, pDrvTracePerm->usVal);
		    pDrvTracePerm++;
	    }
	    memset(pTraceRsp->aucData, 0xFF, 0x04);

#ifdef MNTN_DBG
            printk("########Iam go out##########\n");
#endif
	    break;
    default:
	        return;
	
     }

    pTraceRsp->usLength   = rspLength - 4;
    pTraceRsp->ucFuncType = DRV_FUNCTION_TRACE;
    pTraceRsp->usReserve  = pTraceReq->usReserve;
    if (NULL == g_stMspRegFunc.OM_AddSNTime)
    {
         printk("g_stMspRegFunc.OM_AddSNTime is error\n");
		 return;
    }
    else
    {
        (*(g_stMspRegFunc.OM_AddSNTime))(&(pTraceRsp->ulSn), &(pTraceRsp->ulTimeStamp));
    }
    if (OSAL_OK != (*pRsqFuncPtr)((OM_RSP_PACKET_STRU *)pTraceRsp, rspLength))
    {
        printk("send rsp error\n");
    }
#ifdef MNTN_DBG
	else
	{
        printk("send is ok\n");
	}
#endif
    kfree(pTraceRsp);
	
#ifdef MNTN_DBG
    printk("########go out  BSP_MNTN_OmDrvTraceReqEntry !########\n");
#endif

    return;
}

/***********************************************************************************
 Function:          BSP_MNTN_DrvPrimCanSendOrNot
 Description:       usb_mntn模块查询函数
 Calls:
 Input:             usPrimId
 Output:            NA
 Return:            NA
 ************************************************************************************/
int BSP_MNTN_DrvPrimCanSendOrNot(BSP_U16 usPrimId)
{
    DRV_TRACE_PERMISSION *pDrvTracePerm = NULL;
#ifdef MNTN_DBG
    printk("#######goin BSP_MNTN_DrvPrimCanSendOrNot!########\n");
#endif

    if ((NULL == g_pOmRspFuncPtr)||(NULL == g_pstDrvTracePerm))
    {
#ifdef MNTN_DBG
        printk("#######it is INVALID######\n");
#endif
        return OSAL_ERROR;
    }
    else
    {
        pDrvTracePerm = g_pstDrvTracePerm;
	    while (INVALID_PRIM_ID != pDrvTracePerm->usPrimId)
	    {
            if (pDrvTracePerm->usPrimId == usPrimId)
            {
                if(PRIM_SEND_PERMIT == pDrvTracePerm->usVal)
                {
                    return OSAL_OK;
		        }
		        else
		        {
                    return OSAL_ERROR;
		        }
	        }
	        else
	        {
                pDrvTracePerm++;
	        }
	    }
#ifdef MNTN_DBG
       printk("#######i am goout######\n");
#endif
	    return OSAL_ERROR;
    }
}

/***********************************************************************************
 Function:          BSP_MNTN_OmDrvTraceSend
 Description:       OM发送函数
 Calls:
 Input:             usPrimId
                    ulModuleId
                    buffer
                    ulLength
 Output:            NA
 Return:            NA
 ************************************************************************************/
int BSP_MNTN_OmDrvTraceSend(BSP_U16 usPrimId, BSP_U32 ulModuleId, BSP_U8 *buffer, BSP_U32 ulLength)
{
    DRV_TRACE_IND_STRU *pTraceRsp = NULL;
    static int i                  = 0;
	
#ifdef MNTN_DBG
    printk("#####goin BSP_MNTN_OmDrvTraceSend########!\n");
#endif

    if ((NULL == buffer) || (0 == ulLength))
    {
        printk("usb laction info error\n");
	    return OSAL_ERROR;
    }

    pTraceRsp = (DRV_TRACE_IND_STRU *)(buffer - sizeof(DRV_TRACE_IND_STRU) + 4);
    pTraceRsp->usPrimId    = usPrimId;
    pTraceRsp->ulModuleId  = ulModuleId; 
    pTraceRsp->usLength    = (BSP_U16)(ulLength + sizeof(DRV_TRACE_IND_STRU) - 8);
    pTraceRsp->ucFuncType  = DRV_FUNCTION_TRACE;
    
    if(NULL == g_stMspRegFunc.OM_AddSNTime)
    {
         printk("g_stMspRegFunc.OM_AddSNTime is error\n");
         return OSAL_ERROR;
    }
    else
    {
#ifdef MNTN_DBG
        printk("####@#$(g_stMspRegFunc.OM_AddSNTime) IS ok#!$@@!\n");
#endif
        (*(g_stMspRegFunc.OM_AddSNTime))(&(pTraceRsp->ulSn), &(pTraceRsp->ulTimeStamp));
    }
    
    if(NULL == g_pOmRspFuncPtr)
    {
        printk("g_pOmRspFuncPtr is NULL\n");
		return OSAL_ERROR;
	}
	
    if (OSAL_OK != (*g_pOmRspFuncPtr)((OM_RSP_PACKET_STRU *)pTraceRsp, pTraceRsp->usLength + 4))
    {
        if(i < 20)
        {
            i++;
            printk("BSP_MNTN_OmDrvTraceSend rsp error\n");
        }
	    return OSAL_ERROR;
    }
	
#ifdef MNTN_DBG
    printk("########goout BSP_MNTN_OmDrvTraceSend tail#######!\n");
#endif

    return OSAL_OK;
}
static void BSP_MNTN_DrvProbeOmThread(void)
{
    unsigned int ret_reg = OSAL_OK; /*clean lint e713*/
	
    while(!g_stMspRegFunc.OM_RegisterRequestProcedure)
    {
         msleep(500);	
    }
#ifdef MNTN_DBG
        printk("#######I OUT sleep######\n");
#endif
    ret_reg |= (unsigned int)(*(g_stMspRegFunc.OM_RegisterRequestProcedure))(DRV_FUNCTION_TRACE, BSP_MNTN_OmDrvTraceReqEntry
                                                                                    );
    if (OSAL_OK !=ret_reg)
    {
        printk("OM_RegisterRequestProcedure reg!\n");
        return;
    }
	
#ifdef MNTN_DBG
    printk("#####omdrv init finish!#######\n");
    printk("#####i am go out!#######\n");
#endif

	do_exit(0);
}
/***********************************************************************************
 Function:          BSP_MNTN_OmDrvInit
 Description:     omdrv初始化函数
 Calls:
 Input:               NA
 Output:            NA
 Return:            NA
 ************************************************************************************/
int  __init BSP_MNTN_OmDrvInit (void) 
{
    int rc;
#ifdef MNTN_DBG
    printk("this is in BSP_MNTN_OmDrvInit head!\n");
#endif
	if (!g_pstDrvProbeOm) 
	{
		g_pstDrvProbeOm = kthread_run(BSP_MNTN_DrvProbeOmThread, NULL, "drv_probe_om");
		rc = IS_ERR(g_pstDrvProbeOm) ? PTR_ERR(g_pstDrvProbeOm) : 0;
		if (rc)
		{
		    printk("kthread_run is failed!\n");
			g_pstDrvProbeOm = NULL;
			return -1;
		}
#ifdef MNTN_DBG
        printk("####!@#^$^creat kthread is ok#^&^*\n");
#endif
	}

#ifdef MNTN_DBG
    printk("this is in BSP_MNTN_OmDrvInit tail!\n");
#endif

    return 0x0; 
}
/***********************************************************************************
 Function:          BSP_MspProcReg
 Description:       DRV提供给MSP的注册函数
 Calls:
 Input:             MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
 Output:            NA
 Return:            NA
 ************************************************************************************/
void BSP_MspProcReg(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc)
{
    if(NULL == pFunc)
    {
        printk("pFunc is error!\n");
		return;
	}
	else if(eFuncID < 0 || eFuncID >= MSP_PROC_REG_ID_MAX)
    {
        printk("eFuncID is error\n");
		return;
	}
    else
    {
#ifdef MNTN_DBG
        printk("######(&g_stMspRegFunc))[%d]=%x######\n",eFuncID,((BSP_U32*)(&g_stMspRegFunc))[eFuncID]);
#endif
        /*lint -e661*/
        ((BSP_U32*)(&g_stMspRegFunc))[eFuncID] = (BSP_U32)pFunc;
        /*lint +e661*/
#ifdef MNTN_DBG
        printk("######(&g_stMspRegFunc))[%d]=%x######\n",eFuncID,((BSP_U32*)(&g_stMspRegFunc))[eFuncID]);
#endif
    }
}
module_init(BSP_MNTN_OmDrvInit);
EXPORT_SYMBOL(BSP_MspProcReg);
EXPORT_SYMBOL(g_stMspRegFunc);
EXPORT_SYMBOL(BSP_MNTN_OmDrvTraceSend);
EXPORT_SYMBOL(BSP_MNTN_DrvPrimCanSendOrNot);

/*just for test*/
int test_func(int id)
{
     int testId = id;
     printk("testId = %d\n",testId);
	 return 0x0;
}
void testomDrv(int caseswitch)
{
    printk("#####this is in testomDRV######\n");
	
    switch(caseswitch)
    {
    case 1:
		{
	    
		break;
    	}
	case 2:
	{
         printk("########DrvPermissionTlb ====== 0X%d\n",&g_pstDrvTracePerm);
		 int i = 0;
		 for(i = 0;i < 12;i++)
		 {
             printk("g_pstDrvTracePerm[%d]:%u :0x%u\n ",i ,g_pstDrvTracePerm[i].usVal,g_pstDrvTracePerm[i].usPrimId  );
		 }
		 break;
	}
	case 3:
	{
		  
        
		  break;
	}
	case 4:
	{
		   printk("&g_pOmRspFuncPtr= 0x%x\n",g_pOmRspFuncPtr);
           printk("g_pOmRspFuncPtr = 0x%x\n",*g_pOmRspFuncPtr);
		  break;
	}
	case 5:
	{
          printk("######testomDrv#######\n");
		  printk("test_func:=0x%x\n",(BSP_MspProc)test_func);
		  BSP_MspProcReg(OM_REQUEST_PROC,(BSP_MspProc)test_func);
		  printk("g_stMspRegFunc=0X%x\n",g_stMspRegFunc.OM_RegisterRequestProcedure);
		  printk("ok\n");
		  break;
    }
	case 6:
	{
          BSP_MspProcReg(OM_REQUEST_PROC, NULL);
		  break;
	}
	case 7:
	{
          BSP_MspProcReg(55, (BSP_MspProc)test_func);/*lint !e64*/
		  break;
	}
	default:
		break;
    }
}
