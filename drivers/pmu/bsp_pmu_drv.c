#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/bsp_pmu_drv.h>

#include "BSP.h"
#include <generated/FeatureConfig.h>



static struct semaphore	g_semPmuCtrlSend;
static struct semaphore	g_semPmuCtrlRply;
static BSP_S32 g_s32RplyVal;
static BSP_BOOL s_bPmuCtrlInfo = BSP_FALSE;

BSP_VOID PMU_DbgSet(BSP_BOOL bOn)
{
    s_bPmuCtrlInfo = bOn;
}

BSP_VOID PMU_DbgShow(stPmuCtrl *pstPmuCtrl,BSP_S32 line)
{
    if(s_bPmuCtrlInfo)
    {
        printk("eSrType=%d,eCtrlType=%d,id=%d,value=%d,line=%d\n",
            pstPmuCtrl->eSrType,pstPmuCtrl->eCtrlType,pstPmuCtrl->id,pstPmuCtrl->param.value,line);
    }
}

/*****************************************************************************
* 函 数 名  : BSP_PMU_CtrlCb
*
* 功能描述  : 供A核调用的PMU控制函数
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_CtrlCb( BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    stPmuCtrl *pstPmuCtrl;
    BSP_S32 iRet = BSP_OK;        
    
    if(u32Len < sizeof(stPmuCtrl))
    {
        printk("[BSP_PMU_CtrlCb]:Para ERROR, u32Len:%d\n",u32Len);
        g_s32RplyVal = BSP_ERROR;
        return BSP_ERR_INVALID_PARA;
    }

    pstPmuCtrl = (stPmuCtrl*)pMsgBody;
    g_s32RplyVal = iRet = pstPmuCtrl->param.value;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);
    
    up(&g_semPmuCtrlRply);

    return iRet;
}

/*****************************************************************************
* 函 数 名  : BSP_PMU_DRCtrl
*
* 功能描述  : 调用M核的函数打开DR
*
* 输入参数  : id,DR号，从0开始;
*             bOn,打开或关闭
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DRCtrl(BSP_U32 id, BSP_BOOL bOn)
{
    BSP_S32 iRet;    
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];
    
    down(&g_semPmuCtrlSend);

    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }
    
    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];

    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_DR;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_ONOFF;
    pstPmuCtrl->id = id;
    pstPmuCtrl->param.bOn = bOn;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /* 字节数 */ /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC DRCtrl Send error, iRet:%#x\n",iRet);
		goto exit;
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC DRCtrl NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}

    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC DRCtrl failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}

/*******************************************************************************
  Function:     BSP_PMU_DrCurrentSet
  Description:  用于设置各路电流源的输出电流

  Input:        Vs_ID: PMU电流源编号，取值范围由PMU_ISRC定义：
                          DR1~DR3；
                ImA:      目标电流值（单位:mA），取值范围：
                          3mA,6mA,9mA,12mA,15mA,18mA,21mA,24mA
  Output:
  Return:       BSP_OK:    操作成功
                BSP_ERROR: 操作失败
  Others:
*******************************************************************************/
BSP_S32 BSP_PMU_DrCurrentSet(BSP_U32 id, BSP_U16 value)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];
    
    down(&g_semPmuCtrlSend);

    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }
    
    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_DR;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_VOL_CUR_SET;
    pstPmuCtrl->id = id;
    pstPmuCtrl->param.value = value;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet = (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC DrCurrentSet Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC DrCurrentSet NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC DrCurrentSet failed\n");
    }
    
    up(&g_semPmuCtrlSend);
    return iRet;
}
/*******************************************************************************
  Function:     BSP_PMU_DrCurrentGet
  Description:  用于查询各路电流源的输出电流

  Input:        id: PMU电流源编号，取值范围 DR1~DR3 (0~2)
  Output:
  Return:       查询电流源的输出电流值（单位:mA）
                为负值（ERROR）表示查询失败
  Others:
*******************************************************************************/
BSP_S32 BSP_PMU_DrCurrentGet(BSP_U32 id)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];
    
    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_DR;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_VOL_CUR_GET;
    pstPmuCtrl->id = id;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /*clen lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC DrCurrentGet Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC DrCurrentGet NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}

    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC DrCurrentGet failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}
/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOCtrl
*
* 功能描述  : 调用M核的函数打开LDO
*
* 输入参数  : LdoID,Ldo号;
*             bOn,打开或关闭
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOCtrl(BSP_U32 id, BSP_BOOL bOn)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];

    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_LDO;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_ONOFF;
    pstPmuCtrl->id = id;
    pstPmuCtrl->param.bOn = bOn;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg));  /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC LDOCtrl Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC LDOCtrl NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC LDOCtrl failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltSet
*
* 功能描述  : 调用M核的函数设置LDO电压 
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltSet(BSP_U32 id, BSP_U16 value)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];

    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_LDO;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_VOL_CUR_SET;
    pstPmuCtrl->id = id;
    pstPmuCtrl->param.value = value;

    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC LDOVoltSet Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC LDOVoltSet NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC LDOVoltSet failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltGet
*
* 功能描述  : 获取LDO通道的电压值
*
* 输入参数  : LdoID，LDO通道
*             pusVoltValue，电压值指针，
* 输出参数  : 
*
* 返 回 值  : 电压值,等于实际电压乘以100
*               -1,错误
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltGet(BSP_U8 ucLDO)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];

    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_LDO;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_VOL_CUR_GET;
    pstPmuCtrl->id = ucLDO;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg));  /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC LDOVoltGet Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC LDOVoltGet NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC LDOVoltGet failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}

/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckCtrl
*
* 功能描述  : 调用M核的函数打开Buck
*
* 输入参数  : BuckID,Buck号，从1开始;
*             bOpen,打开或关闭
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckCtrl(BSP_U32 id, BSP_BOOL bOn)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];

    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_BUCK;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_ONOFF;
    pstPmuCtrl->id = id;
    pstPmuCtrl->param.bOn = bOn;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC BuckCtrl Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC BuckCtrl NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC BuckCtrl failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltSet
*
* 功能描述  : 调用M核的函数设置BUCK电压 
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltSet(BSP_U32 id, BSP_U16 value)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];

    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_BUCK;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_VOL_CUR_SET;
    pstPmuCtrl->id = id;
    pstPmuCtrl->param.value = value;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC BuckVoltSet Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC BuckVoltSet NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC BuckVoltSet failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}
/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckVoltGet
*
* 功能描述  : 设置PMU BUCK电压输出值
*
* 输入参数  : buckID，BUCK通道
*
* 输出参数  : 
*
* 返 回 值  : 
* 返 回 值  : 电压值,等于实际电压乘以100
*               -1,错误
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltGet(BSP_U8 buckID)
{
    BSP_S32 iRet;
    stPmuCtrl *pstPmuCtrl;
    BSP_U32 msg[(sizeof(stPmuCtrl)/4 + 1) + 2];

    down(&g_semPmuCtrlSend);
    
    iRet = BSP_SYNC_Wait(SYNC_MODULE_PMU, PMU_SYNC_TIME_OUT);
    if(iRet)
    {
        printk("BSP_PMU_DRCtrl sync lock timeout ret=0x%x, line:%d\n", iRet, __LINE__);
        goto exit;
    }

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_PMU_CTRL;
    pstPmuCtrl = (stPmuCtrl *)&msg[2];
    pstPmuCtrl->eSrType = E_PMU_CTRL_SR_TYPE_BUCK;
    pstPmuCtrl->eCtrlType = E_PMU_CTRL_TYPE_VOL_CUR_GET;
    pstPmuCtrl->id = buckID;
    
    PMU_DbgShow(pstPmuCtrl,__LINE__);

    iRet =  (BSP_S32)BSP_IFC_Send(msg, sizeof(msg)); /*clean lint e713*/
    if(BSP_OK != iRet)
    {
        printk("IFC BuckVoltGet Send error, iRet:%#x\n",iRet);
		goto exit; 
    }
    
	if (BSP_OK != down_timeout(&g_semPmuCtrlRply, msecs_to_jiffies(PMU_CTRL_TIME_OUT)))
	{
        printk("IFC BuckVoltGet NO RPLY\n");
        iRet = BSP_ERROR;
		goto exit; 
	}
    
    iRet = g_s32RplyVal;

exit:
    if(BSP_OK != iRet)
    {
        printk("IFC BuckVoltGet failed\n");
    }

    up(&g_semPmuCtrlSend);
    return iRet;
}


BSP_S32 BSP_PMU_Init(BSP_VOID)
{
    BSP_S32 iRet;
    
	sema_init(&g_semPmuCtrlSend, SEM_FULL);
	sema_init(&g_semPmuCtrlRply, SEM_EMPTY);
    
    iRet = (BSP_S32)BSP_IFCP_RegFunc(IFCP_FUNC_PMU_CTRL, (BSP_IFCP_REG_FUNC)BSP_PMU_CtrlCb);/*clean lint e713*/

    if(BSP_OK != iRet)
    {
        printk("BSP_PMU_Init. Para ERROR. \n");
    }
    
    return iRet;
}
//EXPORT_SYMBOL(BSP_PMU_Init);

