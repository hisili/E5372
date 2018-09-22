/*************************************************************************
*   版权所有(C) 1987-2010, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SYNC.c
*
*   作    者 :  wangxuesong
*
*   描    述 :  本文件基于IFC模块实现，用于管理模块主从核同步操作。
*
*   修改记录 :  2010年4月2日  v1.00  wangxuesong  创建
*************************************************************************/
#include <mach/common/bsp_version.h>
#include <mach/DrvInterface.h>
#include <mach/BSP_SYNC_DRV.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/delay.h>

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 内部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部变量引用                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 全局变量申明                                                 *
 *--------------------------------------------------------------*/

BSP_U32 g_u32syncStatus = 0;

BSP_S32 BSP_SYNC_CallBack(BSP_VOID *pMsgBody,BSP_U32 u32Len);

 /*****************************************************************************
 * 函 数 名  : BSP_SYNC_Init
 *
 * 功能描述  : SYNC初始化函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 其它说明  : 无
 *
 *****************************************************************************/
BSP_S32 BSP_SYNC_Init()
{
    BSP_S32 s32Ret = 0;
    
    s32Ret = BSP_IFCP_RegFunc(IFCP_FUNC_SYNC, (BSP_IFCP_REG_FUNC)BSP_SYNC_CallBack);
    if(BSP_OK != s32Ret)
    {
        printk( "SYNC init, BSP_IFC_RegFunc failed.\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SYNC_Wait
*
* 功能描述  : 从核等待主核操作完成函数
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
*****************************************************************************/
EXPORT_SYMBOL(BSP_SYNC_Wait);
EXPORT_SYMBOL(BSP_GetBuildVersion);
BSP_S32 BSP_SYNC_Wait(SYNC_MODULE_E u32Module)
{
    BSP_U32 module = 1 << (BSP_U32)u32Module;
    BSP_U32 flag = 1;
    BSP_U32 i;

    /* 等待，直到对面CPU完成初始化 */
    for(i=0; i<BSP_SYNC_TIMEOUT; i++)
    {
        if(g_u32syncStatus & module)
        {
            break;
        }
        
        if(flag)
        {
            printk("###### module %d waiting", u32Module);
            flag = 0;
        }

        if(0 == (i%20))
        {
            printk(".", 0,0,0,0,0,0);
        }

        mdelay(10);
    }

    printk("\n", 0,0,0,0,0,0);

    if(BSP_SYNC_TIMEOUT == i)
    {
        return BSP_ERROR;
    }
    else
    {
        return BSP_OK;
    }
}

/*****************************************************************************
* 函 数 名  : BSP_SYNC_CallBack
*
* 功能描述  : SYNC模块的CallBack函数
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_SYNC_CallBack(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
    BSP_U32 module = *(BSP_U32*)pMsgBody;

    /* 将主核操作完成状态置1*/
    g_u32syncStatus |= (1 << module);

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_SYNC_Give
*
* 功能描述  : 主核通知从核主核操作已经完成
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_SYNC_Give(SYNC_MODULE_E module)
{
    BSP_S32 ret;
    BSP_U32 msg[3];

    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_SYNC;
    msg[2] = module;
    ret = BSP_IFC_Send((BSP_VOID*)msg, sizeof(msg));
    if(BSP_OK != ret)
    {
        printk(BSP_LOG_LEVEL_ERROR, BSP_MODU_SYNC, 
            "module %d sync give,  send ifc failed. ret=0x%x\n",module,ret,0,0,0,0);
        return BSP_ERROR;
    }

    return BSP_OK;
}


