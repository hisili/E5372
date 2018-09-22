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

/*�û�ʱ��1*/
FUNCPTR usr1ClkRoutine    = NULL;
int usr1ClkArg        = 0;
int usrClk1Running    = FALSE;
int usrClk1TicksPerSecond = 100;

/*�û�ʱ��*/
USRCLK_CTX_S g_stUsrClkCtx[BSP_USR_CLK_MUX_NUM] = {0};
BSP_U32 g_u32UsrClkInit = FALSE;
int g_UsrIntFirstTime = 0;
/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Init
*
* ��������  : �û�ʱ�ӳ�ʼ��
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
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
* �� �� ��  : BSP_USRCLK_Alloc
*
* ��������  : �����û�ʱ��ID
*
* �������  : BSP_S32 * ps32UsrClkId  
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
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
* �� �� ��  : BSP_USRCLK_Free
*
* ��������  : �ͷ��û�ʱ��
*
* �������  : BSP_S32 s32UsrClkId  
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
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
* �� �� ��  : USRClkInt_Handle
*
* ��������  : This routine handles the system clock interrupt.  It is attached 
*             to the clock interrupt vector by the routine sysClkConnect().
*
* �������  : BSP_S32 s32UsrClkNum  �û�ʱ�����
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

*****************************************************************************/
irqreturn_t USRClkInt_Handle (int irq, void* dev_id)
{
    /* ��ȡ�Ĵ������ֵ*/
    BSP_U32 readValue = 0;
    BSP_U32 u32ClkId = 0;

    /* USR CLK*/
    /*��ȡ�ж�״̬*/
    g_u32TickCnt++;
    BSP_REG_READ(TIMER_BASE(USR_CLK_NUM), CLK_REGOFF_INTSTATUS, readValue);

    if(readValue != 0)
    {
        USRCLK_GetClkId(USR_CLK_NUM, &u32ClkId);
        /*���ж�*/        
        BSP_REG_READ(TIMER_BASE(USR_CLK_NUM), CLK_REGOFF_CLEAR, readValue);

        /* ִ���жϴ����� Ŀǰֻ��һ���û�ʱ�ӣ������±�Ϊ0*/
        if (g_stUsrClkCtx[u32ClkId].pUsrClkRoutine)
        (BSP_VOID)(* g_stUsrClkCtx[u32ClkId].pUsrClkRoutine) (g_stUsrClkCtx[u32ClkId].s32UsrClkArg);  
    }  

    return OK;/*lint !e64*/

}/*lint !e715*/

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Connect
*
* ��������  : This routine specifies the interrupt service routine to be called 
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* �������  : FUNCPTR routine     routine to be called at each clock interrupt
              BSP_S32 arg	      argument with which to call routine
              BSP_S32 s32UsrClkId which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_Disable
*
* ��������  : This routine disables user clock interrupts.
*
* �������  : BSP_S32 s32UsrClkId  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_Enable
*
* ��������  : This routine enables user clock interrupts.
*
* �������  : BSP_S32 s32UsrClkId  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

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

        /* ��ѯ������ʹ���Ƿ���Ч */
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
* �� �� ��  : BSP_USRCLK_RateGet
*
* ��������  : This routine returns the interrupt rate of the system aux clock.
*
* �������  : BSP_S32 s32UsrClkId  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : The number of ticks per second of the system clock.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_RateSet
*
* ��������  : This routine sets the interrupt rate of the usr clock.
*
* �������  : BSP_S32 s32TicksPerSecond   number of clock interrupts per second 
              BSP_S32 s32UsrClkId         which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_TimerStart
*
* ��������  : ��������Timer��ʱ��.
*
* �������  : s32UsrClkId   Timer Id
*             u32Cycles   Timer�Ĵ����ļ���ֵ
* �������  : ��
* �� �� ֵ  : OK&ERROR.
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

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

    /* ��ѯ������ʹ���Ƿ���Ч */
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
* �� �� ��  : BSP_USRCLK_TimerStop
*
* ��������  : �رյ���Timer��ʱ��.
*
* �������  : s32UsrClkId   Timer Id
* �������  : ��
* �� �� ֵ  : OK&ERROR.
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_TimerValue
*
* ��������  : ��������Timer��ʱ��.
*
* �������  : s32UsrClkId   Timer Id
* �������  : Value      ��ǰTimer value�Ĵ����ļ���ֵ
* �� �� ֵ  : ��ǰTimer value�Ĵ����ļ���ֵ.
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_TimerMilliSecStart
*
* ��������  : �Ժ���Ϊ��λ��������Timer��ʱ��.
*
* �������  : s32UsrClkId        Timer Id
*             u32MilliSecond   ���ö�ʱ����ʱ�ĺ���ֵ
* �������  : ��
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

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
* �� �� ��  : BSP_USRCLK_TimerMilliSecValue
*
* ��������  : ��õ�ǰTimer�ĺ���ֵ.
*
* �������  : s32UsrClkId        Timer Id
* �������  : ��
* �� �� ֵ  : ��ǰTimer value�Ĵ����ĺ���ֵ
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

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
* �� �� ��  : USRCLK_GetClkId
*
* ��������  : ����ʵ��Timer��Ż�ȡ�û�ʱ��ID
*
* �������  : BSP_U32 u32ClkNum  
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*            ʵ��Timer���  �û�ʱ��ID
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
* �� �� ��  : USRCLK_GetClkNum
*
* ��������  : �����û�ʱ��ID��ȡʵ��Timer���
*
* �������  : BSP_U32 u32ClkId  
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*            ʵ��Timer���  �û�ʱ��ID
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
* �� �� ��  : USRCLK_GetCurTimerValue
*
* ��������  : ��ȡ��ǰtimer�ļ���ֵ
*
* �������  : BSP_U32 u32ClkNum  
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_U32 USRCLK_GetCurTimerValue(BSP_U32 u32ClkNum)
{    
    BSP_U32 readValueTmp;
    
    /* ���� 1	���Ĵ���CURRENTVALUE��ֵ��*/    
    BSP_REG_READ(TIMER_BASE(u32ClkNum), CLK_REGOFF_VALUE, readValueTmp);
    return readValueTmp;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
