/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_USRCLK.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP �û�ʱ�Ӷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_USRCLK_H__
#define __BSP_USRCLK_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */

/* ��ʱ��ʹ��λ*/
#define CLK_DEF_TC_ENABLE	1     /* 1��Timerʹ��*/
#define	CLK_DEF_TC_DISABLE	0     /* 0��Timer��ֹ*/

#define BSP_ERR_CLK_NO_FREE_CLK         BSP_DEF_ERR(BSP_MODU_TIMER,(BSP_ERR_SPECIAL + 1)) 
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
BSP_U32 BSP_USRCLK_Alloc(BSP_S32 * ps32UsrClkId);

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
BSP_U32 BSP_USRCLK_Free(BSP_S32 s32UsrClkId);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Connect
*
* ��������  : This routine specifies the interrupt service routine to be called 
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* �������  : FUNCPTR routine   routine to be called at each clock interrupt
              BSP_S32 arg	        argument with which to call routine
              BSP_S32 s32UsrClkid      which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Disable
*
* ��������  : This routine disables user clock interrupts.
*
* �������  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Enable
*
* ��������  : This routine enables user clock interrupts.
*
* �������  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_Enable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_RateGet
*
* ��������  : This routine returns the interrupt rate of the system aux clock.
*
* �������  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* �������  : BSP_S32 * pu32ClkRate  clk rate
* �� �� ֵ  : OK&������
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32 BSP_USRCLK_RateGet (BSP_S32 s32UsrClkId, BSP_S32 * pu32ClkRate);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_RateSet
*
* ��������  : This routine sets the interrupt rate of the usr clock.
*
* �������  : BSP_S32 ticksPerSecond   number of clock interrupts per second 
              BSP_S32 s32UsrClkid         which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_TimerStart
*
* ��������  : ��������Timer��ʱ��.
*
* �������  : s32UsrClkid   Timer Id
*             u32Cycles   Timer�Ĵ����ļ���ֵ
* �������  : ��
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStart(BSP_S32 s32UsrClkid,BSP_U32 u32Cycles);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_TimerStop
*
* ��������  : �رյ���Timer��ʱ��.
*
* �������  : s32UsrClkid   Timer Id
* �������  : ��
* �� �� ֵ  : ��.
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_TimerStop(BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_TimerValue
*
* ��������  : ��������Timer��ʱ��.
*
* �������  : s32UsrClkid   Timer Id
* �������  : Value      ��ǰTimer value�Ĵ����ļ���ֵ
* �� �� ֵ  : OK&������
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat
*****************************************************************************/
extern BSP_U32  BSP_USRCLK_TimerValue(BSP_S32 s32UsrClkId, BSP_U32 *pu32Value);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_TimerMilliSecStart
*
* ��������  : �Ժ���Ϊ��λ��������Timer��ʱ��.
*
* �������  : s32UsrClkid        Timer Id
*             u32MilliSecond   ���ö�ʱ����ʱ�ĺ���ֵ
* �������  : ��
* �� �� ֵ  : OK&����������
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_TimerMilliSecStart(BSP_S32 s32UsrClkid ,BSP_U32 u32MilliSecond);


/*****************************************************************************
* �� �� ��  : BSP_USRCLK_TimerMilliSecValue
*
* ��������  : ��õ�ǰTimer�ĺ���ֵ.
*
* �������  : s32UsrClkid        Timer Id
* �������  : BSP_U32 * pu32Value ��ǰTimer value�Ĵ����ĺ���ֵ
* �� �� ֵ  : OK&����������
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat
*****************************************************************************/
extern BSP_U32 BSP_USRCLK_TimerMilliSecValue(BSP_S32 s32UsrClkId ,BSP_U32 * pu32Value);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_USRCLK_H__ */


