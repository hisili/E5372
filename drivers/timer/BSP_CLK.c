#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <asm/cacheflush.h>
#include "BSP.h"
#include "../jusb/port/defines/BSP_GLOBAL.h"
#include "BSP_DRV_CLK.h"

/*lint -e729 */
BSP_U32  g_u32TickCnt;  
/*lint +e729 */

/*用户时钟1*/
FUNCPTR usr1ClkRoutine    = NULL;
int usr1ClkArg        = 0;
int usrClk1Running    = FALSE;
int usrClk1TicksPerSecond = 100;

/*用户时钟*/
USRCLK_CTX_S g_stUsrClkCtx[BSP_USR_CLK_MUX_NUM] = {0};
BSP_U32 g_u32UsrClkInit = FALSE;
int g_UsrIntFirstTime = 0;
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Init
*
* 功能描述  : 用户时钟初始化
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_USRCLK_Init()
{
    BSP_S32 s32Ret = 0;
    BSP_S32 i;
    if (BSP_TRUE == g_u32UsrClkInit)
    {
        return OK;
    }
    memset(g_stUsrClkCtx, 0x0, sizeof(g_stUsrClkCtx));
    for (i = 0; i < BSP_USR_CLK_MUX_NUM; i++)
    {
        g_stUsrClkCtx[i].s32UsrClkTicksPerSecond = CLK_DEFULT_TICKS_PERSECOND;
    }
    
    s32Ret = request_irq(USR_TIMER_INT_LVL, USRClkInt_Handle, 0, "usrclk_irq", NULL);
    if(s32Ret)
    {
        printk("usrclk request_irq error\n");
        return ERROR;
    }

    g_u32UsrClkInit = BSP_TRUE;
    printk("=======BSP_USRCLK_Init OK=========\n");
    return OK;
}
//module_init(BSP_USRCLK_Init);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Alloc
*
* 功能描述  : 申请用户时钟ID
*
* 输入参数  : BSP_S32 * ps32UsrClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_USRCLK_Alloc(BSP_S32 * ps32UsrClkId)
{
    BSP_S32 i;
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }
    
    if (NULL == ps32UsrClkId)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    for (i = 0; i < BSP_USR_CLK_MUX_NUM; i++)
    {
        if (BSP_FALSE == g_stUsrClkCtx[i].s32UsrClkIsUsed)
        {
            *ps32UsrClkId = i;
            g_stUsrClkCtx[i].s32UsrClkIsUsed = BSP_TRUE;
            return OK;
        }
    }
    return BSP_ERR_CLK_NO_FREE_CLK;
}

EXPORT_SYMBOL(BSP_USRCLK_Alloc);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Free
*
* 功能描述  : 释放用户时钟
*
* 输入参数  : BSP_S32 s32UsrClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_USRCLK_Free(BSP_S32 s32UsrClkId)
{    
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }
    
    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    g_stUsrClkCtx[s32UsrClkId].s32UsrClkIsUsed = BSP_FALSE;
    return OK;
}

EXPORT_SYMBOL(BSP_USRCLK_Free);
/*****************************************************************************
* 函 数 名  : USRClkInt_Handle
*
* 功能描述  : This routine handles the system clock interrupt.  It is attached 
*             to the clock interrupt vector by the routine sysClkConnect().
*
* 输入参数  : BSP_S32 s32UsrClkNum  用户时钟组号
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
irqreturn_t USRClkInt_Handle (int irq, void* dev_id)
{
    /* 读取寄存器输出值*/
    BSP_U32 readValue = 0;
    BSP_U32 u32ClkId = 0;

    /* USR CLK*/
    /*读取中断状态*/
    g_u32TickCnt++;
    BSP_REG_READ(TIMER_BASE(USR_CLK_NUM), CLK_REGOFF_INTSTATUS, readValue);

    if(readValue != 0)
    {
        USRCLK_GetClkId(USR_CLK_NUM, &u32ClkId);
        /*清中断*/        
        BSP_REG_READ(TIMER_BASE(USR_CLK_NUM), CLK_REGOFF_CLEAR, readValue);

        /* 执行中断处理函数 目前只有一个用户时钟，数组下标为0*/
        if (g_stUsrClkCtx[u32ClkId].pUsrClkRoutine)
        (BSP_VOID)(* g_stUsrClkCtx[u32ClkId].pUsrClkRoutine) (g_stUsrClkCtx[u32ClkId].s32UsrClkArg);  
    }  

    return OK;/*lint !e64*/

}/*lint !e715*/

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Connect
*
* 功能描述  : This routine specifies the interrupt service routine to be called 
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* 输入参数  : FUNCPTR routine     routine to be called at each clock interrupt
              BSP_S32 arg	      argument with which to call routine
              BSP_S32 s32UsrClkId which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_Connect(USRCLK_FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkId)
{
    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {    
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: s32UsrClkId = %d invalid.\n",(int)__LINE__, (int)__FUNCTION__,s32UsrClkId,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    g_stUsrClkCtx[s32UsrClkId].pUsrClkRoutine = routine;
    g_stUsrClkCtx[s32UsrClkId].s32UsrClkArg = arg;

    return OK;
}/*lint !e715*/

EXPORT_SYMBOL(BSP_USRCLK_Connect);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Disable
*
* 功能描述  : This routine disables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkId  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_Disable (BSP_S32 s32UsrClkId)
{
    BSP_U32 u32ClkNum;

    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }

    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);
    if (g_stUsrClkCtx[s32UsrClkId].s32UsrClkRunning)
    {	
        BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_DISABLE);
        g_stUsrClkCtx[s32UsrClkId].s32UsrClkRunning = BSP_FALSE;
    }

    return OK;

}

EXPORT_SYMBOL(BSP_USRCLK_Disable);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Enable
*
* 功能描述  : This routine enables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkId  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_Enable (BSP_S32 s32UsrClkId)
{
    BSP_U32 tc;
    BSP_U32 u32ClkNum;
    BSP_U32 readValueTmp;
    BSP_U32 u32Times = 100;
    BSP_U32 i = 0;

    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }

    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);

    if (!g_stUsrClkCtx[s32UsrClkId].s32UsrClkRunning)
    {    	
        /* Set up in periodic mode */
        BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_DISABLE);

        /* Calculate the timer value */
        tc = (UINT32)((USR_TIMER_CLK / g_stUsrClkCtx[s32UsrClkId].s32UsrClkTicksPerSecond) - AMBA_RELOAD_TICKS);/*lint !e571*/

        /* Load Timer Reload value into Timer registers */
        BSP_REG_WRITE (TIMER_BASE(u32ClkNum),CLK_REGOFF_LOAD, tc);/*lint !e571*/

        /* 查询计数器使能是否生效 */
        do
        {
            BSP_REG_READ(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, readValueTmp);
            readValueTmp = readValueTmp&0x10;
            i++;
        }while((!readValueTmp)&&(i<u32Times));

        if(i == u32Times)
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"write LoadValue error\n");
            return ERROR;
        }

        BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_ENABLE);

        g_stUsrClkCtx[s32UsrClkId].s32UsrClkRunning = BSP_TRUE;
    }
    return OK;	
}/*lint !e550*/

EXPORT_SYMBOL(BSP_USRCLK_Enable);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateGet
*
* 功能描述  : This routine returns the interrupt rate of the system aux clock.
*
* 输入参数  : BSP_S32 s32UsrClkId  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : The number of ticks per second of the system clock.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_RateGet (BSP_S32 s32UsrClkId, BSP_S32 * pu32ClkRate)
{
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }

    if ((s32UsrClkId >= BSP_USR_CLK_MUX_NUM) || (NULL == pu32ClkRate))
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    *pu32ClkRate = g_stUsrClkCtx[s32UsrClkId].s32UsrClkTicksPerSecond;

    return OK;
}

EXPORT_SYMBOL(BSP_USRCLK_RateGet);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateSet
*
* 功能描述  : This routine sets the interrupt rate of the usr clock.
*
* 输入参数  : BSP_S32 s32TicksPerSecond   number of clock interrupts per second 
              BSP_S32 s32UsrClkId         which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_RateSet(BSP_S32 s32TicksPerSecond,BSP_S32 s32UsrClkId)
{
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }
    
    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    if ((s32TicksPerSecond < USR_CLK_RATE_MIN) || (s32TicksPerSecond > USR_CLK_RATE_MAX))
    {      
        return BSP_ERR_INVALID_PARA;
    }

    g_stUsrClkCtx[s32UsrClkId].s32UsrClkTicksPerSecond = s32TicksPerSecond;
    
    if (g_stUsrClkCtx[s32UsrClkId].s32UsrClkRunning)
    {
        (BSP_VOID)BSP_USRCLK_Disable (s32UsrClkId);
        (BSP_VOID)BSP_USRCLK_Enable (s32UsrClkId);
    }
    return OK;
	
}

EXPORT_SYMBOL(BSP_USRCLK_RateSet);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStart
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkId   Timer Id
*             u32Cycles   Timer寄存器的计数值
* 输出参数  : 无
* 返 回 值  : OK&ERROR.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_S32  BSP_USRCLK_TimerStart(BSP_S32 s32UsrClkId, BSP_U32 u32Cycles)
{    
    BSP_U32 u32ClkNum;
    BSP_U32 readValueTmp;
    BSP_U32 u32Times = 100;
    BSP_U32 i = 0;

    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }

    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    if (0 == u32Cycles)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: u32Cycles = 0.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);

    /* Set up in periodic mode */
    BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_DISABLE);

    /* Load Timer Reload value into Timer registers */
    BSP_REG_WRITE (TIMER_BASE(u32ClkNum),CLK_REGOFF_LOAD, u32Cycles);

    /* 查询计数器使能是否生效 */
    do
    {
        BSP_REG_READ(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, readValueTmp);
        readValueTmp = readValueTmp&0x10;
        i++;
    }while((!readValueTmp)&&(i<u32Times));

    if(i == u32Times)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"write LoadValue error\n");
        return ERROR;
    }

    BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_ENABLE);      

    return OK;

}

EXPORT_SYMBOL(BSP_USRCLK_TimerStart);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerStop
*
* 功能描述  : 关闭单次Timer定时器.
*
* 输入参数  : s32UsrClkId   Timer Id
* 输出参数  : 无
* 返 回 值  : OK&ERROR.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_TimerStop(BSP_S32 s32UsrClkId)
{
    BSP_U32 u32ClkNum;
    
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }
    
    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }
    
    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);
    
    BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_DISABLE);

    return OK;
}

EXPORT_SYMBOL(BSP_USRCLK_TimerStop);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerValue
*
* 功能描述  : 开启单次Timer定时器.
*
* 输入参数  : s32UsrClkId   Timer Id
* 输出参数  : Value      当前Timer value寄存器的计数值
* 返 回 值  : 当前Timer value寄存器的计数值.
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_TimerValue(BSP_S32 s32UsrClkId, BSP_U32 *pu32Value)
{
    BSP_U32 u32ClkNum;
    
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return 0;        
    }
    
    if ((s32UsrClkId >= BSP_USR_CLK_MUX_NUM) || (NULL == pu32Value))
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return 0;
    }
    
    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);
    *pu32Value = USRCLK_GetCurTimerValue(u32ClkNum);

    return OK;
}

EXPORT_SYMBOL(BSP_USRCLK_TimerValue);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecStart
*
* 功能描述  : 以毫秒为单位开启单次Timer定时器.
*
* 输入参数  : s32UsrClkId        Timer Id
*             u32MilliSecond   设置定时器超时的毫秒值
* 输出参数  : 无
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_S32  BSP_USRCLK_TimerMilliSecStart(BSP_S32 s32UsrClkId ,BSP_U32 u32MilliSecond)
{   
    BSP_U32 u32Cycles;
    BSP_U32 u32ClkNum;

    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_MODULE_NOT_INITED;        
    }

    if (s32UsrClkId >= BSP_USR_CLK_MUX_NUM)
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    if ((CLK_DEF_MAX_MILLSECOND < u32MilliSecond) || (u32MilliSecond < CLK_DEF_MIN_MILLSECOND))
    {	    
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: u32MilliSecond = %d invalid.\n",(int)__LINE__, (int)__FUNCTION__,u32MilliSecond,4,5,6);
        return BSP_ERR_INVALID_PARA;
    }

    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);

    u32Cycles = (USR_TIMER_CLK/1000) * u32MilliSecond;

    /* Set up in periodic mode */
    BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_DISABLE);

    /* Load Timer Reload value into Timer registers */
    BSP_REG_WRITE (TIMER_BASE(u32ClkNum),CLK_REGOFF_LOAD, u32Cycles);

    BSP_REG_WRITE(TIMER_BASE(u32ClkNum),CLK_REGOFF_CTRL, CLK_DEF_ENABLE);      

    return OK;
}/*lint !e550*/

EXPORT_SYMBOL(BSP_USRCLK_TimerMilliSecStart);
/*****************************************************************************
* 函 数 名  : BSP_USRCLK_TimerMilliSecValue
*
* 功能描述  : 获得当前Timer的毫秒值.
*
* 输入参数  : s32UsrClkId        Timer Id
* 输出参数  : 无
* 返 回 值  : 当前Timer value寄存器的毫秒值
*
* 修改记录  : 2009年2月24日   liumengcun  creat

*****************************************************************************/
BSP_S32 BSP_USRCLK_TimerMilliSecValue(BSP_S32 s32UsrClkId ,BSP_U32 * pu32Value)
{
    BSP_U32 readValueTmp;
    BSP_U32 u32ClkNum;
    
    if (BSP_FALSE == g_u32UsrClkInit)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: USRCLK is not inited.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return 0;        
    }
    
    if ((s32UsrClkId >= BSP_USR_CLK_MUX_NUM) || (NULL == pu32Value))
    {        
        BSP_TRACE(BSP_LOG_LEVEL_ERROR,BSP_MODU_TIMER,"line %d FUNC %s: NULL == pu32ClkNum.\n",(int)__LINE__, (int)__FUNCTION__,3,4,5,6);
        return 0;
    }
    
    USRCLK_GetClkNum((BSP_U32)s32UsrClkId, &u32ClkNum);    

    readValueTmp = USRCLK_GetCurTimerValue(u32ClkNum);
    /* (Value * (1/SYS_TIMER_CLK))/1000  = MilliSec*/        
    readValueTmp = readValueTmp/((BSP_U32)(USR_TIMER_CLK/1000));

    *pu32Value = readValueTmp;
    return OK;
}

EXPORT_SYMBOL(BSP_USRCLK_TimerMilliSecValue);
/*****************************************************************************
* 函 数 名  : USRCLK_GetClkId
*
* 功能描述  : 根据实际Timer编号获取用户时钟ID
*
* 输入参数  : BSP_U32 u32ClkNum  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*            实际Timer编号  用户时钟ID
*            3              0
*****************************************************************************/
BSP_VOID USRCLK_GetClkId(BSP_U32 u32ClkNum, BSP_U32 *pu32ClkId)
{
    switch (u32ClkNum)
    {
        case USR_CLK_NUM:
        {
            *pu32ClkId = 0;
            break;
        }
        default:
            break;
    }
    return;
}

/*****************************************************************************
* 函 数 名  : USRCLK_GetClkNum
*
* 功能描述  : 根据用户时钟ID获取实际Timer编号
*
* 输入参数  : BSP_U32 u32ClkId  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*            实际Timer编号  用户时钟ID
*            3              0
*****************************************************************************/
BSP_VOID USRCLK_GetClkNum(BSP_U32 u32ClkId, BSP_U32 *pu32ClkNum)
{
    switch (u32ClkId)
    {
         case 0:
        {
            *pu32ClkNum = (BSP_U32)USR_CLK_NUM;
            break;
        }

        default:
            break;
    }
    return;
}

/*****************************************************************************
* 函 数 名  : USRCLK_GetCurTimerValue
*
* 功能描述  : 获取当前timer的计数值
*
* 输入参数  : BSP_U32 u32ClkNum  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_U32 USRCLK_GetCurTimerValue(BSP_U32 u32ClkNum)
{    
    BSP_U32 readValueTmp;
    
    /* 步骤 1	读寄存器CURRENTVALUE的值。*/    
    BSP_REG_READ(TIMER_BASE(u32ClkNum), CLK_REGOFF_VALUE, readValueTmp);
    return readValueTmp;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
