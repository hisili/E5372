/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SYSCLK.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_SYSCLK_H__
#define __BSP_SYSCLK_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/*****************************************************************************
* �� �� ��  : sysClkTicksGet
*
* ��������  : ���ϵͳʱ������������tick��.
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : ϵͳʱ������������tick��.
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

*****************************************************************************/
extern BSP_U32 sysClkTicksGet (BSP_VOID);

/*****************************************************************************
* �� �� ��  : sysAuxClkTicksGet
*
* ��������  : ���ϵͳ����ʱ������������tick��.
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : ϵͳ����ʱ������������tick��.
*
* �޸ļ�¼  : 2009��2��24��   liumengcun  creat

*****************************************************************************/
BSP_U32 sysAuxClkTicksGet (BSP_VOID);

/*****************************************************************************
* �� �� ��  : sysTimestampRateSet
*
* ��������  : This routine sets the interrupt rate of the timestamp clock.  It does 
*             not enable system clock interrupts unilaterally, but if the timestamp is currently enabled, the clock is disabled and then 
*             re-enabled with the new rate.  Normally it is called by usrRoot() 
*             in usrConfig.c.
*
* �������  : int ticksPerSecond   number of clock interrupts per second 
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateSet(BSP_S32 ticksPerSecond);

/*****************************************************************************
* �� �� ��  : sysTimestampRateGet
*
* ��������  : This routine returns the interrupt rate of the timestamp clock.
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : The number of ticks per second of the system clock.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat

*****************************************************************************/
BSP_S32 sysTimestampRateGet (BSP_VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SYSCLK_H__ */

