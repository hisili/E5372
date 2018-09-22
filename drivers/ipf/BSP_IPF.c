/*************************************************************************
*   ��Ȩ����(C) 1987-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IPF.c
*
*   ��    �� :  luting
*
*   ��    �� :  ������Ҫ���IP���˽ӿں�����ʵ�����к����е�IP���˹���
*
*   �޸ļ�¼ :  2011��1��11��  v1.00  luting  ����
*
*************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __VXWORKS__
#include <vxWorks.h>
#include <cacheLib.h>
#include <intLib.h>
#include <logLib.h>
#include <string.h>
#include <taskLib.h>
#include <memLib.h>
#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include "arm_pbxa9.h"
#include "product_config.h"
#include "BSP_GLOBAL.h"
#include "BSP_MEMORY.h"
#include "BSP.h"
#include "BSP_IPF.h"

#else 
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/string.h>

#include "BSP.h"
#endif
#include "BSP_DRV_IPF.h"
#define BSP_TRACE(enLogLevel, enLogModule, fmt, s32Arg1, s32Arg2, \
               s32Arg3, s32Arg4, s32Arg5, s32Arg6)
IPF_UL_S g_stIpfUl = {0};
IPF_DL_S g_stIpfDl = {0};
/* ������Ϣ�ṹ�� */
IPF_DEBUG_INFO_S* g_stIPFDebugInfo = BSP_NULL;
BSP_U32* g_pbIPFInit = BSP_NULL;

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
extern void PWRCTRL_AcpuWakeUpInt(void);
	IPF_REG_INFO_S g_stIpfRegTable[] =
	{
			{IPF_EF_BADDR, 4},
			{IPF_CH_BDQ_SIZE(0), 4},
			{IPF_CH_BDQ_SIZE(1), 4},
			{IPF_CH_RDQ_SIZE(0), 4},
			{IPF_CH_RDQ_SIZE(1), 4},
			{IPF_CH_BDQ_BADDR(0), 4},
			{IPF_CH_BDQ_BADDR(1), 4},
			{IPF_CH_RDQ_BADDR(0), 4},
			{IPF_CH_RDQ_BADDR(1), 4},
			{IPF_INT_MASK0, 4},
			{IPF_INT_MASK1, 4},
			{IPF_TIME_OUT, 4},
			{IPF_CTRL, 4},
			{IPF_CH_CTRL(0), 4},
			{IPF_CH_CTRL(1), 4},
			{IPF_PKT_LEN, 4},
			{IPF_CH_EN, 4},
	};
    IPF_MATCH_INFO_S* g_pstExFilterAddr;
    IPF_ID_S* g_stIPFFreeList = BSP_NULL;
    IPF_FILTER_INFO_S g_stIPFFilterInfo[IPF_CHANNEL_MAX];
	IPF_PWRCTL_FILTER_INFO_S g_stIPFPwrCtlFilterInfo[IPF_CHANNEL_MAX];
	BSP_U32 g_u32IPFPwrCtlStart = 0;
	BSP_U8* g_u8IpfBakAddr = BSP_NULL;
	BSP_U32 g_u32IpfStartRestore = 0;
	BSP_U32 g_u32IpfSwitchInt = 0;
#endif

/*****************************************************************************
* �� �� ��      : IPF_Init
*
* ��������  : IPF��ʼ��     �ڲ�ʹ�ã�����Ϊ�ӿں���
*
* �������  : BSP_VOID
* �������  : ��
* �� �� ֵ     : IPF_SUCCESS    ��ʼ���ɹ�
*                           IPF_ERROR      ��ʼ��ʧ��
*
* �޸ļ�¼  :2011��1��21��   ³��  ����
*****************************************************************************/
#ifdef __KERNEL__
BSP_S32 __init IPF_Init(BSP_VOID)
#else
BSP_S32 IPF_Init(BSP_VOID)
#endif
{
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    BSP_U32 u32IntMask0 = 0;
    BSP_U32 u32IntMask1 = 0;
    BSP_U32 u32ChanCtrl[IPF_CHANNEL_MAX] = {0,0};
    BSP_U32 u32IntStatus = 0;
    BSP_U32 u32Timeout = 0;
    BSP_U32 u32BDSize[IPF_CHANNEL_MAX] = {IPF_ULBD_DESC_SIZE, IPF_DLBD_DESC_SIZE};
    BSP_U32 u32RDSize[IPF_CHANNEL_MAX] = {IPF_ULRD_DESC_SIZE, IPF_DLRD_DESC_SIZE};
    BSP_U32 i = 0;

    /* ��ʼ��ȫ�ֽṹ�� */
    memset((BSP_VOID*)MEMORY_AXI_IPF_ADDR, 0x0, MEMORY_AXI_IPF_SIZE);
    
    g_pbIPFInit = (BSP_U32*)IPF_INIT_ADDR;
    
    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;
    g_stIpfUl.pu32IdleBd = (BSP_U32*)IPF_ULBD_IDLENUM_ADDR;
    *(BSP_U32*)IPF_ULBD_IDLENUM_ADDR = IPF_ULBD_DESC_SIZE;
    
    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;
    #ifdef __BSP_IPF_DEBUG__
    g_stIpfDl.pstIpfDebugCDQ = (IPF_CD_DESC_S*)IPF_DEBUG_DLCD_ADDR;
    #endif
    g_stIpfDl.u32IdleBd = IPF_DLBD_DESC_SIZE;

    g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)IPF_DEBUG_INFO_ADDR;
	g_u8IpfBakAddr = (BSP_U8*)IPF_PWRCTL_INFO_ADDR;

    /* ������չ���������ڴ�, ������չ�������Ļ�ַ */
    g_pstExFilterAddr = (IPF_MATCH_INFO_S*)cacheDmaMalloc(EXFLITER_NUM*sizeof(IPF_MATCH_INFO_S));
    memset((BSP_VOID*)g_pstExFilterAddr, 0x0, EXFLITER_NUM*sizeof(IPF_MATCH_INFO_S));
    
    /* ������չ����������ʼ��ַ */
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_EF_BADDR, (BSP_U32)g_pstExFilterAddr);

    /* ��ʼ������������ */
    if(IPF_FilterList_Init() != IPF_SUCCESS)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF_Init malloc list ERROR! \n");
        goto FREE_ERROR;
    }
    
    /* ���ó�ʱ���üĴ�����ʹ�ܳ�ʱ�жϣ����ó�ʱʱ�� */
    u32Timeout = TIME_OUT_CFG | (1<<16);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_TIME_OUT, u32Timeout);

    for(i = 0; i < IPF_CHANNEL_MAX; i++)
    {
        /* ��������ж��� */
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_CTRL(i), u32ChanCtrl[i]);
        u32ChanCtrl[i] |= 0x30;
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_CTRL(i), u32ChanCtrl[i]);

        /* ����������ͨ����BD��RD��� */
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_BDQ_SIZE(i), u32BDSize[i]-1);
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_RDQ_SIZE(i), u32RDSize[i]-1);
    }

    /* ���к�����ͨ����BD��RD��ʼ��ַ*/    
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_BDQ_BADDR(IPF_CHANNEL_UP), (BSP_U32)g_stIpfUl.pstIpfBDQ);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_RDQ_BADDR(IPF_CHANNEL_UP), (BSP_U32)g_stIpfUl.pstIpfRDQ);
    
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_BDQ_BADDR(IPF_CHANNEL_DOWN), (BSP_U32)g_stIpfDl.pstIpfBDQ);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_RDQ_BADDR(IPF_CHANNEL_DOWN), (BSP_U32)g_stIpfDl.pstIpfRDQ);
    
    /* �����ж�����,ֻ��3���жϣ�����ϱ��������ʱ��RD���� */
    u32IntMask0 = IPF_INT_OPEN0;  /* 0��ΪModem CPU */
    u32IntMask1 = IPF_INT_OPEN1;
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_INT_MASK0, u32IntMask0);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_INT_MASK1, u32IntMask1);

    /* ���ж� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_INT_STATE, u32IntStatus);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_INT_STATE, u32IntStatus);

    IPF_Int_Connect();

    *g_pbIPFInit = BSP_TRUE;
    BSP_SYNC_Give(SYNC_MODULE_IPF);
    
    return IPF_SUCCESS;
    
FREE_ERROR:
    BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_Init malloc ERROR! \n");
    return IPF_ERROR;
#else
	BSP_S32 s32Ret = 0;

    g_pbIPFInit = (BSP_U32*)IPF_INIT_ADDR;

    g_stIpfUl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_ULBD_MEM_ADDR;
    g_stIpfUl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_ULRD_MEM_ADDR;
    g_stIpfUl.pu32IdleBd = (BSP_U32*)IPF_ULBD_IDLENUM_ADDR;

    g_stIpfDl.pstIpfBDQ = (IPF_BD_DESC_S*)IPF_DLBD_MEM_ADDR;
    g_stIpfDl.pstIpfRDQ = (IPF_RD_DESC_S*)IPF_DLRD_MEM_ADDR;
    g_stIpfDl.pstIpfCDQ = (IPF_CD_DESC_S*)IPF_DLCD_MEM_ADDR;
    #ifdef __BSP_IPF_DEBUG__
    g_stIpfDl.pstIpfDebugCDQ = (IPF_CD_DESC_S*)IPF_DEBUG_DLCD_ADDR;
    #endif

    g_stIPFDebugInfo = (IPF_DEBUG_INFO_S*)IPF_DEBUG_INFO_ADDR;

    IPF_Int_Connect();

    s32Ret = BSP_SYNC_Wait(SYNC_MODULE_IPF, 5000);
	if(s32Ret != BSP_OK)
	{

		return IPF_ERROR;
	}

    return IPF_SUCCESS;

#endif
}

/*****************************************************************************
* �� �� ��     : IPF_Int_Connect
*
* ��������  : ��IPF�жϴ�����(���˶��ṩ)
*
* �������  : BSP_VOID
* �������  : ��
* �� �� ֵ      : ��
*
* �޸ļ�¼  :2011��12��2��   ³��  ����
*****************************************************************************/
BSP_VOID IPF_Int_Connect(BSP_VOID)
{
    /* ���ж�ʹ�� */
    (BSP_VOID)BSP_INT_Connect((VOIDFUNCPTR *)(INT_LVL_IPF), (VOIDFUNCPTR)IPF_IntHandler, 0);    
    /* ʹ���жϺ� */
    (BSP_VOID)BSP_INT_Enable(INT_LVL_IPF);
}

/*****************************************************************************
* �� �� ��  : IPF_IntHandler
*
* ��������  : IPF�жϴ�����
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��1��24��   ³��  ����
*****************************************************************************/
#ifdef __KERNEL__
irqreturn_t  IPF_IntHandler (int irq, void* dev)
#else
BSP_VOID IPF_IntHandler(BSP_VOID)
#endif
{
    BSP_U32 u32IntStatus = 0;
    BSP_U32 u32IpfInt = 0;

    /* ��ȡ�ж�״̬ */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM    
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_INT0, u32IpfInt); 
    /* д1�� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_INT_STATE, u32IntStatus); 
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_INT_STATE, u32IntStatus&0xFFFF);

    /* ���н���ϱ��ͽ����ʱ�ϱ� */
    if(u32IpfInt&0x3)
    {
        /* ����ps�������� */
        if(g_stIpfUl.pFnUlIntCb != BSP_NULL)
        {
            (BSP_VOID)g_stIpfUl.pFnUlIntCb();
        }
        else
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                        "\r IPF_IntHandler ULTASK NULL! \n",0,0,0,0,0,0);
        }
    }
    /* ����BDQ����ָʾ */
    if(u32IpfInt&0x80)
    {
        g_stIPFDebugInfo->u32UlBdqOverflow++;
    }
	if(g_u32IpfSwitchInt)
	{
		if((u32IpfInt&0xFFFF0000) != 0)
		{
			PWRCTRL_AcpuWakeUpInt();
		}
	}
#else
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_INT1, u32IpfInt); 
    /* д1�� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_INT_STATE, u32IntStatus); 
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_INT_STATE, u32IntStatus&0xFFFF0000);

    /* ���н���ϱ��ͽ����ʱ�ϱ� */
    if(u32IpfInt&0x30000)
    {
        /* ����ps�������� */
        if(g_stIpfDl.pFnDlIntCb != BSP_NULL)
        {
            (BSP_VOID)g_stIpfDl.pFnDlIntCb();  
        }
        else
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                        "\r IPF_IntHandler DLTASK NULL! \n",0,0,0,0,0,0);
        }
    } 
    /* ����BDQ����ָʾ */
    if(u32IpfInt&0x800000)
    {
        g_stIPFDebugInfo->u32DlBdqOverflow++;
    }

    return IRQ_HANDLED;
#endif
}/*lint !e550*/

/*****************************************************************************
* �� �� ��      : BSP_IPF_ConfigTimeout
*
* ��������  : ����ʹ�ã����ó�ʱʱ��ӿ�
*
* �������  : BSP_U32 u32Timeout ���õĳ�ʱʱ��
* �������  : ��
* �� �� ֵ     : IPF_SUCCESS    �ɹ�
*                           BSP_ERR_IPF_INVALID_PARA      ������Ч
*
* ˵��              : 1����256��ʱ������
*
* �޸ļ�¼   : 2011��11��30��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigTimeout(BSP_U32 u32Timeout)
{
    if((u32Timeout == 0) || (u32Timeout > 0xFFFF))
    {

        return BSP_ERR_IPF_INVALID_PARA;
    }

    u32Timeout |= (1<<16);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_TIME_OUT, u32Timeout);
    
    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��      : BSP_IPF_ConfigUpFilter
*
* ��������  : IPF����IP���˺��� 
*
* �������  : BSP_U32 u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara, BSP_BOOL bintFlag
* �������  : ��
* �� �� ֵ      : IPF_SUCCESS    ���óɹ�
*                           IPF_ERROR      ����ʧ��
*                           BSP_ERR_IPF_NOT_INIT         ģ��δ��ʼ��
*                           BSP_ERR_IPF_INVALID_PARA     ��������
*
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigUpFilter(BSP_U32 u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara, BSP_BOOL bintFlag)
{
    BSP_U32 u32BdqWptr = 0;
    IPF_CONFIG_ULPARAM_S* pstUlParam = pstUlPara;
    BSP_U32 u32BD = 0;
    BSP_U32 u32LastBd = 0;
    BSP_U32 i = 0;

    /* ������� */
    if((NULL == pstUlPara))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                         "\r BSP_IPF_ConfigUpFilter pInfoNode NULL! \n",0,0,0,0,0,0);
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* ���ģ���Ƿ��ʼ�� */
    if(!(*g_pbIPFInit))
    {

        return BSP_ERR_IPF_NOT_INIT;
    }

    #ifdef __BSP_IPF_DEBUG__
    if(u32Num > *(g_stIpfUl.pu32IdleBd))
    {
        g_stIPFDebugInfo->u32UlBdNotEnough++;
        return IPF_ERROR;
    }
    #endif

    /* ����BDдָ��,��u32BdqWptr��Ϊ��ʱдָ��ʹ�� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_UP), u32BdqWptr); 

    u32BD = u32BdqWptr;
    for(i = 0; i < u32Num; i++)
    {
        g_stIpfUl.pstIpfBDQ[u32BD].u16Attribute = 0;
        g_stIpfUl.pstIpfBDQ[u32BD].u32InPtr = pstUlParam[i].u32Data;
        g_stIpfUl.pstIpfBDQ[u32BD].u16PktLen = pstUlParam[i].u16Len;
        g_stIpfUl.pstIpfBDQ[u32BD].u16UsrField1 = pstUlParam[i].u16UsrField1;
        g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField2= pstUlParam[i].u32UsrField2;
        g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField3 = pstUlParam[i].u32UsrField3;
        #ifdef __BSP_IPF_DEBUG__
        if(g_stIPFDebugInfo->u32IpfDebug)
        {
            IPF_PRINT(" func: %d  %x, %d\n", u32BD, g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField2,g_stIpfUl.pstIpfBDQ[u32BD].u32UsrField3);
        }
        #endif
        u32BD = ((u32BD + 1) < IPF_ULBD_DESC_SIZE)? (u32BD + 1) : 0;
    }
    
    if((u32BdqWptr+u32Num-1) < IPF_ULBD_DESC_SIZE)
    {
        u32LastBd = u32BdqWptr+u32Num-1;
    }
    else
    {
        u32LastBd = u32BdqWptr+u32Num-1 - IPF_ULBD_DESC_SIZE;
    }
    g_stIpfUl.pstIpfBDQ[u32LastBd].u16Attribute = (BSP_U16)bintFlag;
    
    /* ����BDдָ��*/
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_UP), u32BD);  

    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetUlBDNum
*
* ��������  : �ýӿ����ڻ�ȡ���п���BD ��Ŀ
*                            ��Χ: 0~64
* �������  : ��
*
* �������  : ��
* �� �� ֵ      : ����BD��Ŀ
*  
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_U32 BSP_IPF_GetUlBDNum(BSP_VOID)
{
    BSP_U32 u32UlBdDepth = 0;
    BSP_U32 u32IdleBd = 0;

    /* �������BD���� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_DQ_DEPTH(IPF_CHANNEL_UP), u32UlBdDepth);
    u32IdleBd = IPF_ULBD_DESC_SIZE - (u32UlBdDepth & 0x1FF);
    *(g_stIpfUl.pu32IdleBd) = u32IdleBd;
    
    return u32IdleBd;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_UlStateIdle
*
* ��������  : �ýӿ����ڻ�ȡ����ͨ���Ƿ�Ϊ����
*                            
* �������  : ��
*
* �������  : ��
* �� �� ֵ      : IPF_SUCCESS  ��ʾ���п��У������л�ģʽ
*                            IPF_ERROR      ��ʾ���зǿ��У��������л�ģʽ
*  
* �޸ļ�¼  :2011��12��9��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_UlStateIdle(BSP_VOID)
{
    BSP_U32 u32UlState = 0;
    
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_STATE(IPF_CHANNEL_UP), u32UlState);

    if(u32UlState != 0x14)
    {
        return IPF_ERROR;
    }

    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��      : BSP_IPF_PktLen
*
* ��������  : �ýӿ��������ù�������������С����
*
* �������  : BSP_U32 MaxLen   ������
*                           BSP_U32 MinLen   ��С����
*
* �������   : ��
* �� �� ֵ      : IPF_SUCCESS                �ɹ�
*                           BSP_ERR_IPF_INVALID_PARA   ��������(����������С����С)
*
* �޸ļ�¼  :2011��2��17��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_SetPktLen(BSP_U32 u32MaxLen, BSP_U32 u32MinLen)
{
    BSP_U32 u32PktLen = 0;

    /* ������� */
    if(u32MaxLen < u32MinLen)
    {

        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    u32PktLen = ((u32MaxLen&0x3FFF)<<16) | (u32MinLen&0x3FFF);
    
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_PKT_LEN, u32PktLen);
    return IPF_SUCCESS;
}/*lint !e550*/

/*****************************************************************************
* �� �� ��  : BSP_IPF_GetStat
*
* ��������  : �ýӿڻ�ȡͳ�Ƽ�������ֵ
*
* �������  : IPF_FILTER_STAT_S *pstFilterStat ͳ�Ƽ����ṹ��ָ��
*             
*
* �������  : ��
* �� �� ֵ  : IPF_SUCCESS                �ɹ�
*             BSP_ERR_IPF_INVALID_PARA   ��������
*
* �޸ļ�¼  :2011��2��17��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_GetStat(IPF_FILTER_STAT_S *pstFilterStat)
{
    /* ������� */
    if(BSP_NULL == pstFilterStat)
    {

        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_UL_CNT0, pstFilterStat->u32UlCnt0); 
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_UL_CNT1, pstFilterStat->u32UlCnt1);
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_UL_CNT2, pstFilterStat->u32UlCnt2);
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_DL_CNT0, pstFilterStat->u32DlCnt0);
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_DL_CNT1, pstFilterStat->u32DlCnt1);
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_DL_CNT2, pstFilterStat->u32DlCnt2);
    return IPF_SUCCESS;
}

/**********************************��ά�ɲ�ӿں���************************************/
BSP_VOID BSP_IPF_Help(BSP_VOID)
{
    IPF_PRINT("===============================================\n");
    IPF_PRINT("BSP_IPF_Info    ����1:ͨ������  0Ϊ���У�1Ϊ����\n");
    IPF_PRINT("BSP_IPF_BDInfo  ����1:ͨ������  ����2:BDָ��\n");
    IPF_PRINT("BSP_IPF_RDInfo  ����1:ͨ������  ����2:RDָ��\n"); 
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM    
    IPF_PRINT("===============================================\n");
    IPF_PRINT("BSP_IPF_UseFilterInfo  ����1:ͨ������\n"); 
    IPF_PRINT("BSP_IPF_FreeFilterInfo\n"); 
    IPF_PRINT("BSP_IPF_FilterInfoHWID  ����1:Ӳ��Filter ID\n");
    IPF_PRINT("BSP_IPF_FilterInfoPSID  ����1 :PS Filter ID\n");
#endif
    IPF_PRINT("===============================================\n");
    IPF_PRINT("��������BD,BD�����ô���:            %d\n",g_stIPFDebugInfo->u32UlBdNotEnough);
    IPF_PRINT("��������BD,BD�����ô���:            %d\n",g_stIPFDebugInfo->u32DlBdNotEnough);
    IPF_PRINT("��������CD,CD�����ô���:            %d\n",g_stIPFDebugInfo->u32DlCdNotEnough);
    IPF_PRINT("�ж��ϱ�����BD�����������:         %d\n",g_stIPFDebugInfo->u32UlBdqOverflow);
    IPF_PRINT("�ж��ϱ�����BD�����������:         %d\n",g_stIPFDebugInfo->u32DlBdqOverflow);
    IPF_PRINT("===============================================\n");
}

BSP_S32 BSP_IPF_BDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32BdqPtr)
{
#ifdef __BSP_IPF_DEBUG__
    BSP_U32 u32CdPtr;
#endif
      
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
                if(u32BdqPtr >= IPF_ULBD_DESC_SIZE)
                {
                    return IPF_ERROR;
                }
                IPF_PRINT("==========BD Info=========\n");
                IPF_PRINT("BDλ��:         %d\n",u32BdqPtr);
                IPF_PRINT("u16Attribute:   %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16Attribute);
                IPF_PRINT("u16PktLen:      %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16PktLen);
                IPF_PRINT("u32InPtr:       0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32InPtr);
                IPF_PRINT("u32OutPtr:      0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32OutPtr);
                IPF_PRINT("u16Resv:        %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16Resv);
                IPF_PRINT("u16UsrField1:   %d\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u16UsrField1);
                IPF_PRINT("u32UsrField2:   0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32UsrField2);
                IPF_PRINT("u32UsrField3:   0x%x\n",g_stIpfUl.pstIpfBDQ[u32BdqPtr].u32UsrField3);
            break;
       case IPF_CHANNEL_DOWN:
                if(u32BdqPtr >= IPF_DLBD_DESC_SIZE)
                {
                    return IPF_ERROR;
                }
                IPF_PRINT("==========BD Info=========\n");
                IPF_PRINT("BDλ��:         %d\n",u32BdqPtr);
                IPF_PRINT("u16Attribute:   %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16Attribute);
                IPF_PRINT("u16PktLen:      %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16PktLen);
                IPF_PRINT("u32InPtr:       0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr);
                IPF_PRINT("u32OutPtr:      0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32OutPtr);
                IPF_PRINT("u16Resv:        %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16Resv);
                IPF_PRINT("u16UsrField1:   %d\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u16UsrField1);
                IPF_PRINT("u32UsrField2:   0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32UsrField2);
                IPF_PRINT("u32UsrField3:   0x%x\n",g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32UsrField3);
                #ifdef __BSP_IPF_DEBUG__
                #ifdef __VXWORKS__
                u32CdPtr = g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr-(BSP_U32)g_stIpfDl.pstIpfCDQ;
                #else
                u32CdPtr = IO_ADDRESS(g_stIpfDl.pstIpfBDQ[u32BdqPtr].u32InPtr)-(BSP_U32)g_stIpfDl.pstIpfCDQ;
                #endif
                u32CdPtr = u32CdPtr/sizeof(IPF_CD_DESC_S);
                while(g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute != 1)
                {
                    IPF_PRINT("==========CD Info=========\n");
                    IPF_PRINT("CDλ��:             %d\n",u32CdPtr);
                    IPF_PRINT("u16Attribute:       %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute);
                    IPF_PRINT("u16PktLen:          %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16PktLen);
                    IPF_PRINT("u32Ptr:             0x%x\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u32Ptr);                
                    u32CdPtr = ((u32CdPtr+1) < IPF_DLCD_DESC_SIZE)?(u32CdPtr+1):0;
                };
                IPF_PRINT("==========CD Info=========\n");
                IPF_PRINT("CDλ��:             %d\n",u32CdPtr);
                IPF_PRINT("u16Attribute:       %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16Attribute);
                IPF_PRINT("u16PktLen:          %d\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u16PktLen);
                IPF_PRINT("u32Ptr:             0x%x\n",g_stIpfDl.pstIpfDebugCDQ[u32CdPtr].u32Ptr);  
                #endif
            break; 
        default:
            break;
    }
    IPF_PRINT("************************\n");
    return 0;
}

BSP_S32 BSP_IPF_RDInfo(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32RdqPtr)
{     
    switch(eChnType)
    {
        case IPF_CHANNEL_UP:
            if(u32RdqPtr >= IPF_ULRD_DESC_SIZE)
            {
                return IPF_ERROR;
            }
            IPF_PRINT("===========RD Info==========\n");
            IPF_PRINT("RDλ��:             %d\n",u32RdqPtr);
            IPF_PRINT("u16Attribute:       %d\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16Attribute);
            IPF_PRINT("u16PktLen:          %d\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16PktLen);
            IPF_PRINT("u32InPtr:           0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32InPtr);
            IPF_PRINT("u32OutPtr:          0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32OutPtr);
            IPF_PRINT("u16Result:          0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16Result);
            IPF_PRINT("u16UsrField1:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u16UsrField1);
            IPF_PRINT("u32UsrField2:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32UsrField2);
            IPF_PRINT("u32UsrField3:       0x%x\n",g_stIpfUl.pstIpfRDQ[u32RdqPtr].u32UsrField3);
            break;
       case IPF_CHANNEL_DOWN:
            if(u32RdqPtr >= IPF_DLRD_DESC_SIZE)
            {
                return IPF_ERROR;
            }
            IPF_PRINT("============RD Info===========\n");
            IPF_PRINT("RDλ��:             %d\n",u32RdqPtr);
            IPF_PRINT("u16Attribute:       %d\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16Attribute);
            IPF_PRINT("u16PktLen:          %d\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16PktLen);
            IPF_PRINT("u32InPtr:           0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32InPtr);
            IPF_PRINT("u32OutPtr:          0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32OutPtr);
            IPF_PRINT("u16Result:          0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16Result);
            IPF_PRINT("u16UsrField1:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u16UsrField1);
            IPF_PRINT("u32UsrField2:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32UsrField2);
            IPF_PRINT("u32UsrField3:       0x%x\n",g_stIpfDl.pstIpfRDQ[u32RdqPtr].u32UsrField3);
            break; 
        default:
            break;
    }
    IPF_PRINT("************************\n");
    return 0;
}
BSP_S32 BSP_IPF_Info(IPF_CHANNEL_TYPE_E eChnType)
{
    BSP_U32 u32BdqDepth = 0;
    BSP_U32 u32BdqWptr = 0;
    BSP_U32 u32BdqRptr = 0;
    BSP_U32 u32BdqWaddr = 0;
    BSP_U32 u32BdqRaddr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqWptr = 0;
    BSP_U32 u32RdqWaddr = 0;
    BSP_U32 u32RdqRaddr = 0;
    BSP_U32 u32Depth = 0;

    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_DQ_DEPTH(eChnType), u32Depth);
    u32RdqDepth = (u32Depth>>16)&0x1FF;
    u32BdqDepth = u32Depth&0x1FF;
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(eChnType), u32BdqWptr); 
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_RPTR(eChnType), u32BdqRptr); 
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WADDR(eChnType), u32BdqWaddr); 
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_RADDR(eChnType), u32BdqRaddr); 

    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_RDQ_RPTR(eChnType), u32RdqRptr);
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_RDQ_WPTR(eChnType), u32RdqWptr);
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_RDQ_WADDR(eChnType), u32RdqWaddr); 
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_RDQ_RADDR(eChnType), u32RdqRaddr); 

    IPF_PRINT("============================\n");
    IPF_PRINT("BD ���:            %d\n", u32BdqDepth);
    IPF_PRINT("BD дָ��:          %d\n", u32BdqWptr);
    IPF_PRINT("BD ��ָ��:          %d\n", u32BdqRptr);
    IPF_PRINT("BD д��ַ:          0x%x\n", u32BdqWaddr);
    IPF_PRINT("BD ����ַ:          0x%x\n", u32BdqRaddr);
    IPF_PRINT("RD ���:            %d\n", u32RdqDepth);
    IPF_PRINT("RD ��ָ��:          %d\n", u32RdqRptr);
    IPF_PRINT("RD дָ��:          %d\n", u32RdqWptr);
    IPF_PRINT("RD ����ַ:          0x%x\n", u32RdqRaddr);
    IPF_PRINT("RD д��ַ:          0x%x\n", u32RdqWaddr);
    IPF_PRINT("============================\n");   
    return 0;
}

BSP_VOID BSP_IPF_MEM(BSP_VOID)
{
    IPF_PRINT("=======================================\n");
    IPF_PRINT("   BSP_IPF_MEM          ADDR            SIZE\n");
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_ULBD_MEM_ADDR    ", IPF_ULBD_MEM_ADDR, IPF_ULBD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_ULRD_MEM_ADDR    ", IPF_ULRD_MEM_ADDR, IPF_ULRD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_DLBD_MEM_ADDR    ", IPF_DLBD_MEM_ADDR, IPF_DLBD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_DLRD_MEM_ADDR    ", IPF_DLRD_MEM_ADDR, IPF_DLRD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_DLCD_MEM_ADDR    ", IPF_DLCD_MEM_ADDR, IPF_DLCD_MEM_SIZE);
    IPF_PRINT("%s%#x\t\t%#x\n", "IPF_INIT_ADDR        ", IPF_INIT_ADDR, IPF_INIT_SIZE);
    IPF_PRINT("%20s%#x\t\t%#x\n", "IPF_DEBUG_INFO_ADDR  ", IPF_DEBUG_INFO_ADDR, IPF_DEBUG_INFO_SIZE);
}

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
/*****************************************************************************
* �� �� ��  : IPF_FilterList_Init
*
* ��������  : IPF�����������ʼ��     �ڲ�ʹ�ã�����Ϊ�ӿں���
*
* �������  : BSP_VOID
* �������  : ��
* �� �� ֵ  : IPF_SUCCESS    �����������ʼ���ɹ�
*                       IPF_ERROR      �����������ʼ��ʧ��
*
* �޸ļ�¼  :2011��11��17��   ³��  ����
*****************************************************************************/
BSP_S32 IPF_FilterList_Init(BSP_VOID)
{
    BSP_U32 i = 0;
    IPF_ID_S* current = BSP_NULL;
    IPF_ID_S* prev = BSP_NULL;
	IPF_ID_S* tmp = BSP_NULL;
    
    /* ��ʼ��������uselist */
    for(i = 0; i < IPF_CHANNEL_MAX; i++)
    {
        g_stIPFFilterInfo[i].u32FilterNum = 0;
        g_stIPFFilterInfo[i].pstUseList = BSP_NULL;
    }
    
    /* ��ʼ��freelist */
    for(i = 0; i < IPF_TOTAL_FILTER_NUM; i++)
    {
        current = (IPF_ID_S*)malloc(sizeof(IPF_ID_S));
        if(BSP_NULL == current)
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF_FilterList_Init malloc ERROR! \n");
            goto FREE_ALL;
        }
        if(g_stIPFFreeList != BSP_NULL)
        {
            prev->pstNext = current;/*lint !e613*/
        }
        else /* ͷ��� */
        {
            g_stIPFFreeList = current;
        }
        current->u32FilterID = i;
        current->u32PsID = 0;
        current->pstNext = BSP_NULL;
        prev = current;
    }
    return IPF_SUCCESS;
    
FREE_ALL:
    while(g_stIPFFreeList!= BSP_NULL)
    {
    	tmp = g_stIPFFreeList->pstNext;
        free(g_stIPFFreeList);
        g_stIPFFreeList = tmp;
    }
    return IPF_ERROR;
}

/*****************************************************************************
* �� �� ��  : IPF_AddToFreeList
*
* ��������  : �����ӵ�freelist�ĺ���λ��
*
* �������  : IPF_ID_S* stNode             ������Ľ��ָ��
*             
* �������  : ��

* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��3��30��   ³��  ����
*****************************************************************************/
BSP_VOID IPF_AddToFreeList(IPF_ID_S* stNode)
{
    IPF_ID_S* current = BSP_NULL;    
    IPF_ID_S* prev = BSP_NULL;

    current = g_stIPFFreeList;

    /* ���freelistΪ�� */
    if(BSP_NULL == current)
    {
        g_stIPFFreeList = stNode;
        g_stIPFFreeList->pstNext = BSP_NULL;
        return;
    }

    /* ����ȵ�һ������ID��С */
    if(stNode->u32FilterID < current->u32FilterID)
    {
        stNode->pstNext = current;
        g_stIPFFreeList = stNode;       
        return;
    }

    prev = g_stIPFFreeList;
    current = g_stIPFFreeList->pstNext;

    while(current != BSP_NULL)
    {
        if(stNode->u32FilterID < current->u32FilterID)
        {
            prev->pstNext = stNode;
            stNode->pstNext = current;
            return;
        }
        prev = current;
        current = current->pstNext;
    }
    /* ������� */
    prev->pstNext = stNode;
    stNode->pstNext = BSP_NULL;

}


/*****************************************************************************
* �� �� ��  : IPF_DeleteAll
*
* ��������  : ɾ�������е����н��
*
* �������  : IPF_CHANNEL_TYPE_E eChnType  ͨ������           
*             
* �������  : ��

* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��3��30��   ³��  ����
*****************************************************************************/
BSP_VOID IPF_DeleteAll(IPF_CHANNEL_TYPE_E eChnType)
{
    IPF_ID_S* pstMove = BSP_NULL;
    BSP_U32 u32temp = 0;
    BSP_U32 u32Mask[IPF_CHANNEL_MAX] = {0x1FF,0x1FF0000};
    
    while(g_stIPFFilterInfo[eChnType].u32FilterNum)
    {
        pstMove = g_stIPFFilterInfo[eChnType].pstUseList;
        /* ������uselistɾ�� */
        g_stIPFFilterInfo[eChnType].pstUseList = g_stIPFFilterInfo[eChnType].pstUseList->pstNext;

        /* ��ɾ���Ľ����ӵ�freelist�� */ 
        IPF_AddToFreeList(pstMove); 
        g_stIPFFilterInfo[eChnType].u32FilterNum--;
    }
    g_stIPFFilterInfo[eChnType].pstUseList = BSP_NULL;

    /* ����������ʼ��������Ϊ511 */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_FLT_ZERO_INDEX, u32temp);
    u32temp |= u32Mask[eChnType];
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_FLT_ZERO_INDEX, u32temp);
}

/*****************************************************************************
* �� �� ��  : IPF_MallocOneFilter
*
* ��������  : ��freelist�з���һ�����(ȡ����һ�����)
*
* �������  : ��       
*             
* �������  : ��

* �� �� ֵ  : ���ָ��
*
* �޸ļ�¼  : 2011��3��30��   ³��  ����
*
* ˵��      : �ɵ��ú�������֤һ���ܷ��䵽���
*****************************************************************************/
IPF_ID_S* IPF_MallocOneFilter(BSP_VOID)
{
    IPF_ID_S* current = BSP_NULL;
    current = g_stIPFFreeList;

    if(BSP_NULL == current)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "IPF_MallocOneFilter error\n");
        return current;
    }
    g_stIPFFreeList = g_stIPFFreeList->pstNext;  

    return current;
}

/*****************************************************************************
* �� �� ��  : IPF_AddTailUsedFilter
*
* ��������  : �����ӵ�uselist�Ľ�β
*
* �������  : IPF_CHANNEL_TYPE_E eChnType  ͨ������   
*             IPF_ID_S* stNode             ������Ľ��ָ��
*             
* �������  : ��

* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��3��30��   ³��  ����
*****************************************************************************/
BSP_VOID IPF_AddTailUsedFilter(IPF_CHANNEL_TYPE_E eChnType, IPF_ID_S* stNode)
{
    IPF_ID_S* current = BSP_NULL;

    current = g_stIPFFilterInfo[eChnType].pstUseList;

    if(BSP_NULL == current)
    {
        g_stIPFFilterInfo[eChnType].pstUseList = stNode;
        g_stIPFFilterInfo[eChnType].pstUseList->pstNext = BSP_NULL;
        g_stIPFFilterInfo[eChnType].u32FilterNum = 1;
        return;
    }

    /* �����ӵ������β */
    while(current->pstNext != BSP_NULL)
    {
        current = current->pstNext;
    }
    current->pstNext = stNode;
    stNode->pstNext = BSP_NULL;
    
    g_stIPFFilterInfo[eChnType].u32FilterNum++;

}

/*****************************************************************************
* �� �� ��  : IPF_AddTailFilterChain
*
* ��������  :�����������õ�Ӳ����
*
* �������  : BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo
*             
* �������  : ��

* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��3��30��   ³��  ����
*****************************************************************************/
BSP_VOID IPF_AddTailFilterChain(BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo)
{
    BSP_U32 u32Value = 0;
    IPF_MATCH_INFO_S* pstLastMatchInfo = BSP_NULL;
    
    /* ����������--���µĹ������������õ������� */
    if(u32FilterID < IPF_BF_NUM)
    {
        BSP_REG_WRITE(IPF_REGBASE_ADR, BFLT_INDEX, u32FilterID); 
        memcpy((BSP_VOID*)(IPF_REGBASE_ADR+FLT_LOCAL_ADDR0), pstMatchInfo, sizeof(IPF_MATCH_INFO_S));
    }
    /* ��չ������ */
    else
    {
        memcpy((BSP_VOID*)(g_pstExFilterAddr + u32FilterID - IPF_BF_NUM), pstMatchInfo, sizeof(IPF_MATCH_INFO_S));
    }

    /* ���ǵ�һ��filter����Ҫ������һ��filter��nextindex�� */
    if(u32LastFilterID != IPF_TAIL_INDEX)
    {
        /* ���ϴ��������һ����������nextIndex���������� */
        if(u32LastFilterID < IPF_BF_NUM)
        {
            BSP_REG_WRITE(IPF_REGBASE_ADR, BFLT_INDEX, u32LastFilterID); 
            BSP_REG_READ(IPF_REGBASE_ADR, FTL_CHAIN, u32Value);
            u32Value = (u32Value&0x1FF0000) + u32FilterID;
            BSP_REG_WRITE(IPF_REGBASE_ADR, FTL_CHAIN, u32Value);
        }
        else
        {
            pstLastMatchInfo = g_pstExFilterAddr + u32LastFilterID - IPF_BF_NUM;
            pstLastMatchInfo->unFltChain.Bits.u16NextIndex = u32FilterID;        
        }
    }
    
}

/*****************************************************************************
* �� �� ��     : IPF_FindFilterID
*
* ��������  : ��������������Ѱ����PS ID ƥ���Filter ID
*
* �������  : IPF_CHANNEL_TYPE_E eChnType    ͨ������
*                           BSP_U32 u32PsID                PS ID            
*             
* �������  : BSP_U32* u32FilterID   ��ѯ����Filter ID

* �� �� ֵ     : IPF_SUCCESS                ��ѯ�ɹ�
*                          IPF_ERROR                  ��ѯʧ��
*
* �޸ļ�¼  :2011��1��11��   ³��  ����
*****************************************************************************/
BSP_S32 IPF_FindFilterID(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32PsID, BSP_U32* u32FilterID)
{
    IPF_ID_S* current = BSP_NULL;

    current = g_stIPFFilterInfo[eChnType].pstUseList;
    while(current != BSP_NULL)
    {
        if(current->u32PsID != u32PsID)
        {
            current = current->pstNext;
        }
        else
        {
            *u32FilterID = current->u32FilterID;
            return IPF_SUCCESS;
        }
    }

    return IPF_ERROR;        
}

/*****************************************************************************
* �� �� ��      : BSP_IPF_Init
*
* ��������  : IPF����������ʼ��
*
* �������  : IPF_COMMON_PARA_S *pstCommPara
* �������  : ��
* �� �� ֵ      : IPF_SUCCESS    ���óɹ�
*                           BSP_ERR_IPF_NOT_INIT     δ��ʼ��
*                           BSP_ERR_IPF_INVALID_PARA  ��������
*
* ˵��             : ����IPFȫ�ֿ������üĴ��� ����ѡ�����ȼ���������
*
* �޸ļ�¼  : 2011��11��29��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_Init(IPF_COMMON_PARA_S *pstCommPara)
{
    BSP_U32 u32IPFCtrl = 0;
    BSP_U32 i = 0;

    /* ������� */
    if(NULL == pstCommPara)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_Init pstCommPara NULL! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    if(!(*g_pbIPFInit))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r IPF NOT Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }

    /* ����IPFȫ�ֿ������üĴ��� */
    u32IPFCtrl = pstCommPara->bFilterSeq | (pstCommPara->bFltAddrReverse<<1) | (pstCommPara->bSpPriSel<<2)/*lint !e701*/ 
                 | (pstCommPara->bSpWrrModeSel<<3) | (pstCommPara->eMaxBurst<<4)/*lint !e701*/
                 | (pstCommPara->bIpv6NextHdSel<<6) | (pstCommPara->bEspAhSel<<7)/*lint !e701*/
                 | (pstCommPara->bAhSpiDisable<<8) | (pstCommPara->bEspSpiDisable<<9);/*lint !e701*/
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_IPF, "\r u32IPFCtrl = 0x%x \n", u32IPFCtrl);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CTRL, u32IPFCtrl); 

    return  IPF_SUCCESS; 
}

/*****************************************************************************
* �� �� ��      : BSP_IPF_ConfigUlChannel
*
* ��������  : ��������ͨ�����Ʋ���
*
* �������  : IPF_CHL_CTRL_S *pstCtrl 
* �������  : ��
* �� �� ֵ      : ��
*
* ˵��             : ��������IPFͨ�����ƼĴ���
*
* �޸ļ�¼  : 2011��11��29��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigUlChannel(IPF_CHL_CTRL_S *pstCtrl)
{
    BSP_U32 u32ChanCtrl = 0; 

    /* ������� */
    if(NULL == pstCtrl)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigUlChannel pstCtrl NULL! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    /*��������ͨ�����ƼĴ��� */
    u32ChanCtrl = pstCtrl->eIpfMode | (pstCtrl->bEndian<<2) | 
                            (pstCtrl->bDataChain<<3) | (pstCtrl->u32WrrValue<<16);
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_IPF,"\r u32ChanCtrl = 0x%x \n", u32ChanCtrl);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_CTRL(IPF_CHANNEL_UP), u32ChanCtrl);  

    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��      : BSP_IPF_ConfigDlChannel
*
* ��������  : ��������ͨ�����Ʋ���
*
* �������  : IPF_CHL_CTRL_S *pstCtrl 
* �������  : ��
* �� �� ֵ      : ��
*
* ˵��             : ��������IPFͨ�����ƼĴ���
*
* �޸ļ�¼  : 2011��11��29��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_ConfigDlChannel(IPF_CHL_CTRL_S *pstCtrl)
{
    BSP_U32 u32ChanCtrl = 0; 

    /* ������� */
    if(NULL == pstCtrl)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDlChannel pstCtrl NULL! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    /*��������ͨ�����ƼĴ��� */
    u32ChanCtrl = pstCtrl->eIpfMode | (pstCtrl->bEndian<<2) | 
                            (pstCtrl->bDataChain<<3) | (pstCtrl->u32WrrValue<<16);
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_IPF,"\r u32ChanCtrl = 0x%x \n", u32ChanCtrl);
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_CTRL(IPF_CHANNEL_DOWN), u32ChanCtrl);

    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_SetFilter
*
* ��������  : �����������е�Filter
*
* �������  : IPF_CHANNEL_TYPE_E eChnType ͨ������
*                           IPF_FILTER_CONFIG_S *pFilterInfo  Filter���ýṹ��ָ��
*                           BSP_U32 u32FilterNum   Filter����
*
* �������  : ��
* �� �� ֵ      : IPF_SUCCESS                      ���óɹ�
*                            BSP_ERR_IPF_NOT_INIT             ģ��δ��ʼ��
*                            BSP_ERR_IPF_FILTER_NOT_ENOUGH    Filter��������
*                            BSP_ERR_IPF_INVALID_PARA         ��������
*
* �޸ļ�¼  :2011��1��11��   ³��  ����
                            2011��3��30��   ³��  �޸�
                            2011��11��30��   ³��  �޸�
*****************************************************************************/
BSP_S32 BSP_IPF_SetFilter(IPF_CHANNEL_TYPE_E eChnType, IPF_FILTER_CONFIG_S *pstFilterInfo, BSP_U32 u32FilterNum)
{
    BSP_U32 i = 0;
    IPF_ID_S* current = BSP_NULL;
    BSP_U32 u32temp = 0;
    IPF_CHANNEL_TYPE_E eType = IPF_CHANNEL_MAX;
    BSP_U32 u32LastFilterID = 0;
	IPF_FILTER_CONFIG_S* IPFPwrCtlTemp = BSP_NULL;

    /* ������� */
    if((eChnType >= IPF_CHANNEL_MAX) || (NULL == pstFilterInfo) || (eChnType < IPF_CHANNEL_UP))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_SetFilter input para ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* ���ģ���Ƿ��ʼ�� */
    if(!(*g_pbIPFInit))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }

    eType = (eChnType == IPF_CHANNEL_UP)?IPF_CHANNEL_DOWN:IPF_CHANNEL_UP;

    if(u32FilterNum > (IPF_TOTAL_FILTER_NUM - g_stIPFFilterInfo[eType].u32FilterNum))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_SetFilter Num is not enough! \n");
        return BSP_ERR_IPF_FILTER_NOT_ENOUGH;
    }

    /* ����÷�������н�� */
    IPF_DeleteAll(eChnType);

	if(!g_u32IpfStartRestore) /* �͹��Ļָ����̲���Ҫ����*/
	{
		if(u32FilterNum > 0)
		{		
			if(g_stIPFPwrCtlFilterInfo[eChnType].pstFilterInfo != BSP_NULL)
			{
				free(g_stIPFPwrCtlFilterInfo[eChnType].pstFilterInfo);
			}
			IPFPwrCtlTemp = malloc(u32FilterNum * sizeof(IPF_FILTER_CONFIG_S));
			if(IPFPwrCtlTemp == BSP_NULL)
			{
				BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_SetFilter malloc! \n");
				return IPF_ERROR;
			}
			memcpy(IPFPwrCtlTemp, pstFilterInfo, u32FilterNum * sizeof(IPF_FILTER_CONFIG_S));
			g_stIPFPwrCtlFilterInfo[eChnType].pstFilterInfo = IPFPwrCtlTemp;
			g_stIPFPwrCtlFilterInfo[eChnType].eChnType = eChnType;
			g_stIPFPwrCtlFilterInfo[eChnType].u32FilterNum = u32FilterNum;
			g_stIPFPwrCtlFilterInfo[eChnType].u32Flag = BSP_TRUE;
		}
		else
		{
			if(g_stIPFPwrCtlFilterInfo[eChnType].pstFilterInfo != BSP_NULL)
			{
				free(g_stIPFPwrCtlFilterInfo[eChnType].pstFilterInfo);
			}
			g_stIPFPwrCtlFilterInfo[eChnType].pstFilterInfo = BSP_NULL;
			g_stIPFPwrCtlFilterInfo[eChnType].u32FilterNum = 0;
			g_stIPFPwrCtlFilterInfo[eChnType].u32Flag = BSP_FALSE;
			return IPF_SUCCESS;
		}
	}
    for(i = 0; i < u32FilterNum; i++)
    {
        /* ��freelist��ȡ��δ���õĹ����� */
        current = IPF_MallocOneFilter();        
		if(current == BSP_NULL)
		{		
        	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF_MallocOneFilter error! \n");
			return IPF_ERROR;
		}
        current->u32PsID = pstFilterInfo->u32FilterID;
        
        /* �Ƚ�NextIndex����Ϊ511 */
        pstFilterInfo->stMatchInfo.unFltChain.Bits.u16NextIndex = IPF_TAIL_INDEX;

        if(0 == i)
        {
            /* ����ʼ�����䵽�Ĵ��� */
            BSP_REG_READ(IPF_REGBASE_ADR, IPF_FLT_ZERO_INDEX, u32temp);
            if(eChnType == IPF_CHANNEL_UP)
            {
                u32temp &= 0xFFFF0000;
                u32temp += (current->u32FilterID);
            }
            else
            {
                u32temp &= 0x0000FFFF;
                u32temp += (current->u32FilterID<<16);
            }
            BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_FLT_ZERO_INDEX, u32temp);  
            IPF_AddTailFilterChain(IPF_TAIL_INDEX, current->u32FilterID, &pstFilterInfo->stMatchInfo);
        } 
        else
        {
            IPF_AddTailFilterChain(u32LastFilterID, current->u32FilterID, &pstFilterInfo->stMatchInfo);
        }
        
        /* ���¸÷����uselist */
        IPF_AddTailUsedFilter(eChnType, current);
        u32LastFilterID = current->u32FilterID;
        
        pstFilterInfo++; 
    }
    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��      : BSP_IPF_GetFilter
*
* ��������  : �ṩ��PS��ѯĳ��Filter������Ϣ
*
* �������  : BSP_U32 u32FilterID   Filter ID��
*             
* �������  : IPF_FILTER_CONFIG_S * pFilterInfo  ��ѯ����Filter��Ϣ

* �� �� ֵ     : IPF_SUCCESS                ��ѯ�ɹ�
*                           IPF_ERROR                  ��ѯʧ��
*                           BSP_ERR_IPF_NOT_INIT       ģ��δ��ʼ��
*                           BSP_ERR_IPF_INVALID_PARA   ��������
*
* �޸ļ�¼  : 2011��1��11��   ³��  ����
                             2011��3��30��   ³��  �޸�
                             2011    11��30��   ³��  �޸�
*****************************************************************************/
BSP_S32 BSP_IPF_GetFilter(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32FilterID, IPF_FILTER_CONFIG_S *pstFilterInfo)
{
    BSP_U32 u32FindID = 0;
    
    /* ������� */
    if(NULL == pstFilterInfo)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r BSP_IPF_GetFilter input para ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    
    /* ���ģ���Ƿ��ʼ�� */
    if(!(*g_pbIPFInit))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }

    /* ����PS ID ��Ӧ��FilterID*/
    if(IPF_FindFilterID(eChnType, u32FilterID, &u32FindID) != IPF_SUCCESS)
    {
        return IPF_ERROR;
    }
    pstFilterInfo->u32FilterID = u32FilterID;    
    
    /* ����ǻ��������� */
    if(u32FindID < IPF_BF_NUM)
    {
        /* д���˱������ַ */    
        BSP_REG_WRITE(IPF_REGBASE_ADR, BFLT_INDEX, u32FindID); 
         
        /* ���������������� */
        memcpy(&pstFilterInfo->stMatchInfo, (BSP_VOID*)(IPF_REGBASE_ADR+FLT_LOCAL_ADDR0), sizeof(IPF_MATCH_INFO_S));
    }
    /* ��չ������ */
    else
    {
        /* ���������������� */
        memcpy(&pstFilterInfo->stMatchInfo, (BSP_VOID*)(g_pstExFilterAddr + u32FindID - IPF_BF_NUM), sizeof(IPF_MATCH_INFO_S));
    }
    
    return IPF_SUCCESS;

}

/*****************************************************************************
* �� �� ��      : IPF_ConfigCD
*
* ��������  : ����CD
*
* �������  : TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr
* �������  : ��
* �� �� ֵ      :  ��
*
* �޸ļ�¼  :2011��1��24��   ³��  ����
*****************************************************************************/
BSP_S32 IPF_ConfigCD(TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr)
{
    BSP_U32 u32DlCDWptr = 0;
    BSP_U32 u32DlCDRptr = 0;
    BSP_U16 u16TotalDataLen = 0;
    TTF_MEM_ST* p = BSP_NULL;

    /* Ϊ��ֹBD �����ߺ�������Ŀ�ĵ�ַ���ɹ���������ʱcd��λ�� */
    u32DlCDWptr = g_stIpfDl.u32IpfCdWptr;
    u32DlCDRptr = g_stIpfDl.u32IpfCdRptr;
    *pu32BdInPtr = (BSP_U32)(g_stIpfDl.pstIpfCDQ + (u32DlCDWptr));

    /* ����CD */
    p = pstTtf;
    do
    {
        /* �ж��Ƿ��п���CD */
        if(u32DlCDRptr != (u32DlCDWptr+1)%IPF_DLCD_DESC_SIZE)
        {
            g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u32Ptr = (BSP_U32)p->pData;
            g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u16PktLen = p->usUsed;
            #ifdef __BSP_IPF_DEBUG__
            g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u32Ptr = (BSP_U32)p->pData;
            g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u16PktLen = p->usUsed;
            if(g_stIPFDebugInfo->u32IpfDebug)
            {
                IPF_PRINT("usUsed = %d  pData = 0x%x\n", p->usUsed, p->pData);
            }
            #endif
            u16TotalDataLen += p->usUsed;
            
            /* ��ʶ�Ƿ���� */
            if(p->pNext != BSP_NULL)
            {
                g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u16Attribute = 0;
                #ifdef __BSP_IPF_DEBUG__
                g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u16Attribute = 0;
                #endif
                if(u32DlCDWptr+1 < IPF_DLCD_DESC_SIZE)
                {
                    u32DlCDWptr += 1;
                    p = p->pNext;
                }
                /* δ����ʱ��CD���ܷ�ת�����Ǽ�¼CD��β����ͬʱ����0��ʼ����CD */
                else
                {
                    u32DlCDWptr = 0;
                    u16TotalDataLen = 0;
                    p = pstTtf;
                    *pu32BdInPtr = (BSP_U32)g_stIpfDl.pstIpfCDQ;
                    #ifdef __BSP_IPF_DEBUG__
                    if(g_stIPFDebugInfo->u32IpfDebug)
                    {
                        IPF_PRINT("CD ��ת \n");
                    }
                    #endif
                }
            }
            else
            {
                g_stIpfDl.pstIpfCDQ[u32DlCDWptr].u16Attribute = 1;
                #ifdef __BSP_IPF_DEBUG__
                g_stIpfDl.pstIpfDebugCDQ[u32DlCDWptr].u16Attribute = 1;
                #endif
                
                /* ����ʱCD���Է�ת */
                u32DlCDWptr = (u32DlCDWptr+1 < IPF_DLCD_DESC_SIZE)?(u32DlCDWptr+1):0;
                #ifdef __BSP_IPF_DEBUG__
                if(g_stIPFDebugInfo->u32IpfDebug)
                {
                    IPF_PRINT("u32DlCDWptr  = %d\n", u32DlCDWptr);
                }
                #endif
                break;
            }
        }
        else
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                    "\r DownFilter CD FULL ! u32DlCDRptr=%d u32DlCDWptr=%d\n",u32DlCDRptr,u32DlCDWptr);
            return BSP_ERR_IPF_CDQ_NOT_ENOUGH;
        }        
    }while(p != BSP_NULL);
    
    g_stIpfDl.u32IpfCdWptr = u32DlCDWptr;
    *pu16TotalDataLen = u16TotalDataLen;
    
    return IPF_SUCCESS;

}

BSP_S32 BSP_IPF_ConfigDownFilter(BSP_U32 u32Num, IPF_CONFIG_DLPARAM_S* pstDlPara, BSP_BOOL bintFlag)
{
    BSP_U32 u32BdqWptr = 0;
    TTF_MEM_ST* p = BSP_NULL;
    BSP_U32 i;
    BSP_U32 u32BdInPtr = 0;
    BSP_U32 u32BD = 0;
    BSP_U32 u32LastBd = 0;
    BSP_U16 u16TotalDataLen = 0;

    if(BSP_NULL == pstDlPara)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_ConfigDownFilter input para ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* ���ģ���Ƿ��ʼ�� */
    if(!(*g_pbIPFInit))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r IPF Not Init! \n");
        return BSP_ERR_IPF_NOT_INIT;
    }

    if(u32Num > g_stIpfDl.u32IdleBd)
    {
        g_stIPFDebugInfo->u32DlBdNotEnough++;
        return IPF_ERROR;
    }

    /* ����BDдָ�� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_DOWN), u32BdqWptr);
    u32BD = u32BdqWptr;
    for(i = 0; i < u32Num; i++)
    {
        /* ����CD */
        if(IPF_ConfigCD(pstDlPara[i].pstSrcTtf, &u16TotalDataLen, &u32BdInPtr) != IPF_SUCCESS)
        {
            g_stIPFDebugInfo->u32DlCdNotEnough++;
            return BSP_ERR_IPF_CDQ_NOT_ENOUGH;
        }
        g_stIpfDl.pstIpfBDQ[u32BD].u16Attribute = 0; 
        g_stIpfDl.pstIpfBDQ[u32BD].u32InPtr =  u32BdInPtr;
        g_stIpfDl.pstIpfBDQ[u32BD].u16PktLen = u16TotalDataLen;
        g_stIpfDl.pstIpfBDQ[u32BD].u32OutPtr = (BSP_U32)pstDlPara[i].pstDstTtf->pData;
        g_stIpfDl.pstIpfBDQ[u32BD].u16UsrField1 = pstDlPara[i].u16UsrField;
        g_stIpfDl.pstIpfBDQ[u32BD].u32UsrField2 = (BSP_U32)pstDlPara[i].pstDstTtf;
        g_stIpfDl.pstIpfBDQ[u32BD].u32UsrField3 = (BSP_U32)pstDlPara[i].pstSrcTtf;
        u32BD = ((u32BD + 1) < IPF_DLBD_DESC_SIZE)? (u32BD + 1) : 0;
    }

    if((u32BdqWptr+u32Num-1) < IPF_DLBD_DESC_SIZE)
    {
        u32LastBd = u32BdqWptr+u32Num-1;
    }
    else
    {
        u32LastBd = u32BdqWptr+u32Num-1 - IPF_DLBD_DESC_SIZE;
    }
    g_stIpfDl.pstIpfBDQ[u32LastBd].u16Attribute = bintFlag; 

    /* ����BDдָ�� */
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_DOWN), u32BD);  

    return IPF_SUCCESS;
}
/*****************************************************************************
* �� �� ��     : BSP_IPF_ChannelEnable
*
* ��������  : �ýӿ�����ʹ�ܻ�λIP������������ͨ��
*
* �������  : IPF_CHANNEL_TYPE_E eChanType      ������ͨ����ʶ
*                           BSP_BOOL bFlag   ʹ�ܸ�λ��ʶ 
*
* �������  : ��
* �� �� ֵ      : IPF_SUCCESS                ʹ�ܸ�λ�ɹ�
*                            IPF_ERROR                  ʹ�ܸ�λʧ��
*                            BSP_ERR_IPF_INVALID_PARA   ��������
*
* �޸ļ�¼  :2011��1��11��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_ChannelEnable(IPF_CHANNEL_TYPE_E eChanType, BSP_BOOL bFlag)
{
    BSP_U32 u32ChanEnable = 0;
    BSP_U32 u32ChanState = 0;
    BSP_U32 u32Times = 0;
    BSP_U32 u32ChCtrl = 0;
    
    /* ������� */
    if((eChanType >= IPF_CHANNEL_MAX) || (eChanType < IPF_CHANNEL_UP))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                     "\r BSP_IPF_ChannelEnable eChanType = %d ! \n",eChanType,0,0,0,0,0);
        return BSP_ERR_IPF_INVALID_PARA;
    }

    /* ͨ��ʹ�� */
    if(bFlag)
    {
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_EN, u32ChanEnable);
        u32ChanEnable |= 0x1<<eChanType;
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_EN, u32ChanEnable);                        
    }
    /* ͨ��ȥʹ�� */
    else
    {
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_EN, u32ChanEnable);
        u32ChanEnable &= 0xFFFFFFFF ^ (0x1<<eChanType);
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_EN, u32ChanEnable); 
        
        /* ����ֱ��ͨ��״̬Ϊ0 */      
        do
        {
            (BSP_VOID)taskDelay(1);
            BSP_REG_READ(IPF_REGBASE_ADR, IPF_EN_STATE, u32ChanState);
            u32ChanState &= 0x1<<eChanType;
            u32Times++;
        }while((u32ChanState) && (u32Times < 100)); 
        
        if(100 == u32Times)
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                       "\r BSP_IPF_ChannelEnable disable error u32Times = %d ! \n",u32Times,0,0,0,0,0);
            return IPF_ERROR;
        }

        /* ��λ������к�������еĶ�дָ�� */
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_CTRL(eChanType), u32ChCtrl);
        u32ChCtrl |= 0x30; /* ��4��5bit����Ϊ1 */
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_CTRL(eChanType), u32ChCtrl); 

        if(eChanType == IPF_CHANNEL_DOWN)
        {
            g_stIpfDl.u32IpfBDFptr = 0;
        }
    }

    
    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetDlBDNum
*
* ��������  : �ýӿ����ڻ�ȡ���п���BD��CD ��Ŀ
*                           BD ��Χ: 0~64      CD��Χ: 0~64*4
* �������  : ��
*
* �������  : ����CD��Ŀ
* �� �� ֵ      : ����BD��Ŀ
*  
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_U32 BSP_IPF_GetDlBDNum(BSP_U32* pu32CDNum)
{
    BSP_U32 u32BdqWptr = 0;
    BSP_U32 u32IdleBd = 0;
    BSP_U32 u32IdleCd = 0;
    BSP_U32 u32IdleCdDown = 0;
    BSP_U32 u32IdleCdUp = 0;
    BSP_U32 u32Tmp = 0;

    /* �������BD���� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_DOWN), u32BdqWptr);
    if(g_stIpfDl.u32IpfBDFptr > u32BdqWptr)
    {
        u32IdleBd = g_stIpfDl.u32IpfBDFptr - u32BdqWptr;
    }
    else if(g_stIpfDl.u32IpfBDFptr == u32BdqWptr)
    {
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_STATE(IPF_CHANNEL_DOWN), u32Tmp);
        u32IdleBd = ((u32Tmp&0x10) > 0)? IPF_DLBD_DESC_SIZE: 0;
    }
    else
    {
        u32IdleBd = g_stIpfDl.u32IpfBDFptr + IPF_DLBD_DESC_SIZE - u32BdqWptr;
    }

    /* �������CD ���� */
    if(g_stIpfDl.u32IpfCdRptr > g_stIpfDl.u32IpfCdWptr)
    {
        u32IdleCd = g_stIpfDl.u32IpfCdRptr - g_stIpfDl.u32IpfCdWptr - 1;
    }
    else
    {
        u32IdleCdUp = (g_stIpfDl.u32IpfCdRptr > 1)?(g_stIpfDl.u32IpfCdRptr - 1):0;
        u32IdleCdDown = IPF_DLCD_DESC_SIZE -  g_stIpfDl.u32IpfCdWptr - 1;
        u32IdleCd = (u32IdleCdUp > u32IdleCdDown)? u32IdleCdUp:u32IdleCdDown;
    }
	u32IdleBd = (u32IdleBd > 1)? (u32IdleBd - 1):0;
    g_stIpfDl.u32IdleBd = u32IdleBd;
    *pu32CDNum = u32IdleCd;
    
    return u32IdleBd;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_DlStateIdle
*
* ��������  : �ýӿ����ڻ�ȡ����ͨ���Ƿ�Ϊ����
*                            
* �������  : ��
*
* �������  : ��
* �� �� ֵ      : IPF_SUCCESS  ��ʾ���п��У������л�ģʽ
*                            IPF_ERROR      ��ʾ���зǿ��У��������л�ģʽ
*  
* �޸ļ�¼  :2011��12��9��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_DlStateIdle(BSP_VOID)
{
    BSP_U32 u32DlState = 0;
    BSP_U32 u32BdqWptr = 0;
    
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_STATE(IPF_CHANNEL_DOWN), u32DlState);

    if(u32DlState != 0x14)
    {
        return IPF_ERROR;
    }

    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_DOWN), u32BdqWptr);
    if(g_stIpfDl.u32IpfBDFptr != u32BdqWptr)
    {
        return IPF_ERROR;
    }

    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetDlBDNum
*
* ��������  : �ýӿ������ͷ�����
*                            �Ѿ�������ɵ�BD �е�Դ�����ڴ�
*                           
* �������  : ��
*
* �������  : ��
* �� �� ֵ      : ��
*  
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPF_ReleaseDlSrcMem (BSP_VOID)
{
    BSP_U32 u32BdqRptr;
    BSP_U32 u32CdqRptr;
    BSP_U32 u32Tmp = 0;
    TTF_MEM_ST* pSrcTtf[IPF_DLBD_DESC_SIZE] = {0};
    BSP_U32 u32Num = 0;
	BSP_U32 u32MoveNum = 0;

    /* ����BD��ָ�� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_RPTR(IPF_CHANNEL_DOWN), u32BdqRptr);
    
    u32Tmp = g_stIpfDl.u32IpfBDFptr;
    while(u32Tmp != u32BdqRptr)
    {       
        /* ����CD��ָ�� */
        u32CdqRptr = (g_stIpfDl.pstIpfBDQ[u32Tmp].u32InPtr - (BSP_U32)g_stIpfDl.pstIpfCDQ)/sizeof(IPF_CD_DESC_S);
        while(g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute != 1)
        {
            /* ���ͷŵ�CD  ��0 */
            g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute = 0;
            g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16PktLen = 0;
            g_stIpfDl.pstIpfCDQ[u32CdqRptr].u32Ptr = 0;
            u32CdqRptr = ((u32CdqRptr+1) < IPF_DLCD_DESC_SIZE)?(u32CdqRptr+1):0;
        }
        g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16Attribute = 0;
        g_stIpfDl.pstIpfCDQ[u32CdqRptr].u16PktLen = 0;
        g_stIpfDl.pstIpfCDQ[u32CdqRptr].u32Ptr = 0;
        u32CdqRptr = ((u32CdqRptr+1) < IPF_DLCD_DESC_SIZE)?(u32CdqRptr+1):0;
        g_stIpfDl.u32IpfCdRptr = u32CdqRptr;

         /* ��ȡ���ͷŵ�ttfָ�� */
        if(g_stIpfDl.pstIpfBDQ[u32Tmp].u32UsrField2 != g_stIpfDl.pstIpfBDQ[u32Tmp].u32UsrField3)
        {
            pSrcTtf[u32Num] = (TTF_MEM_ST*)g_stIpfDl.pstIpfBDQ[u32Tmp].u32UsrField3;
            u32Num++;
        }
		u32MoveNum++;
        u32Tmp = ((u32Tmp+1) < IPF_DLBD_DESC_SIZE)?(u32Tmp+1):0;       
    }

	if(u32MoveNum > 0)
	{
	    /* �ͷ�ԴTTFָ�� */
	    if(( g_stIpfDl.pFnDlFreeMemCb != BSP_NULL) && (u32Num != 0))
	    {
	        if(g_stIpfDl.pFnDlFreeMemCb(u32Num, pSrcTtf) != OK)
	        {
	            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "BSP_IPF_ReleaseDlSrcMem error\n");
	        }
	    }
	    if((g_stIpfDl.u32IpfBDFptr+u32MoveNum)<IPF_DLBD_DESC_SIZE)
	    {
	        g_stIpfDl.u32IpfBDFptr +=  u32MoveNum;
	    }
	    else
	    {
	        g_stIpfDl.u32IpfBDFptr = g_stIpfDl.u32IpfBDFptr + u32MoveNum - IPF_DLBD_DESC_SIZE;
	    }

	    #ifdef __BSP_IPF_DEBUG__
	    if(g_stIPFDebugInfo->u32IpfDebug)
	    {
	        IPF_PRINT("BSP_IPF_ReleaseDlSrcMem u32BdqRptr = %x, u32IpfBDFptr = %x \n", u32BdqRptr, g_stIpfDl.u32IpfBDFptr);
	    }
	    #endif
	}
    
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_RegisterDlFreeMemCb
*
* ��������  : �ýӿ�����ע�������ͷ��ڴ�ص�����
*                           
* �������  : BSP_IPF_DlFreeMemCb pFnDlFreeMem
*
* �������  : ��
* �� �� ֵ      : ��
*  
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPF_RegisterDlFreeMemCb(BSP_IPF_DlFreeMemCb pFnDlFreeMem)
{
    g_stIpfDl.pFnDlFreeMemCb = pFnDlFreeMem;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_RegisterWakeupUlCb
*
* ��������  : �ýӿ�����ע������PS����ص�����
*                           
* �������  : BSP_IPF_WakeupUlCb *pFnWakeupUl
*
* �������  : ��
* �� �� ֵ      : ��
*  
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_RegisterWakeupUlCb(BSP_IPF_WakeupUlCb pFnWakeupUl)
{
    /* ������� */
    if(BSP_NULL == pFnWakeupUl)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_RegisterWakeupUlCb inputPara ERROR! \n");
        return BSP_ERR_IPF_INVALID_PARA;
    }
    g_stIpfUl.pFnUlIntCb = pFnWakeupUl;
    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetUlRd
*
* ��������  : �ýӿ����ڶ�ȡ����BD, ֧��һ�ζ�ȡ���BD
*
* �������  : BSP_U32* pu32Num    
*                           IPF_RD_DESC_S *pstRd
*   
* �������  : BSP_U32* pu32Num    ʵ�ʶ�ȡ��RD��Ŀ
*
* �� �� ֵ     : IPF_SUCCESS               �����ɹ�
*                           IPF_ERROR                   ����ʧ��
*
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPF_GetUlRd(BSP_U32* pu32Num, IPF_RD_DESC_S *pstRd)
{
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32Num = 0;
    BSP_U32 i = 0;
  
    /* ��ȡRD��� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_DQ_DEPTH(IPF_CHANNEL_UP), u32RdqDepth);
    u32RdqDepth = (u32RdqDepth>>16)&0x1FF;
    
    u32Num = (u32RdqDepth < *pu32Num)?u32RdqDepth:*pu32Num;

    if(u32Num > 0)
    {
        /* ��ȡRD��ָ�� */
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_RDQ_RPTR(IPF_CHANNEL_UP), u32RdqRptr);
        #ifdef __BSP_IPF_DEBUG__
        if(g_stIPFDebugInfo->u32IpfDebug)
        {
            IPF_PRINT("in u32RdqRptr = %d\n", u32RdqRptr);
        }
        #endif
        for(i = 0; i < u32Num; i++)
        {        
            /* ��ȡRD */
            pstRd[i].u16Attribute = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16Attribute;
            pstRd[i].u16PktLen = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16PktLen;
            pstRd[i].u16Result = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16Result;
            pstRd[i].u32InPtr = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32InPtr;
            pstRd[i].u32OutPtr = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32OutPtr;
            pstRd[i].u16UsrField1 = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u16UsrField1;
            pstRd[i].u32UsrField2 = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32UsrField2;
            pstRd[i].u32UsrField3 = g_stIpfUl.pstIpfRDQ[u32RdqRptr].u32UsrField3;
            
            u32RdqRptr = ((u32RdqRptr+1) < IPF_ULRD_DESC_SIZE)?(u32RdqRptr+1):0;        
        }
        /* ����RD��ָ�� */
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_RDQ_RPTR(IPF_CHANNEL_UP), u32RdqRptr);
        #ifdef __BSP_IPF_DEBUG__
        if(g_stIPFDebugInfo->u32IpfDebug)
        {
            IPF_PRINT("out u32RdqRptr = %d\n", u32RdqRptr);
        }
        #endif
    }
    *pu32Num = u32Num;
}

/*****************************************************************************
* �� �� ��       : BSP_IPF_SwitchInt
*
* ��������  : �ṩ�ĵ͹��Ľӿڣ� ���ϱ���A�˵��ж�ͬʱ�ϱ���C ��
*
* �������  : BSP_TRUE   �ϱ���C��
*                           BSP_FALSE  ���ϱ���C��
*             
* �������  : ��
* �� �� ֵ     : ��
* �޸ļ�¼  : 2011��2��14��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPF_SwitchInt(BSP_BOOL bFlag)
{
	BSP_U32 u32IntMask0 = 0;

	if(bFlag == BSP_TRUE)
	{
    	u32IntMask0 = IPF_INT_OPEN0 | IPF_INT_OPEN1;
		g_u32IpfSwitchInt = 1;
	}
	else
	{
		u32IntMask0 = IPF_INT_OPEN0;
		g_u32IpfSwitchInt = 0;
	}
    BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_INT_MASK0, u32IntMask0);
}
/*****************************************************************************
* �� �� ��       : BSP_IPF_DrxBakReg
*
* ��������  : �ṩ�ĵ͹��Ľӿڣ�����IPF�����Ϣ
*
* �������  : ��
*             
* �������  : ��
* �� �� ֵ     : IPF_SUCCESS     IPF�����µ磬�����Ϣ�Ѿ�����
*                           IPF_ERROR        IPF�������µ�
*
* �޸ļ�¼  : 2011��2��14��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_DrxBakReg()
{
    BSP_U8* u8addr = BSP_NULL;
    BSP_U32 i = 0;
    BSP_U32 j = 0;
    BSP_U32 u32Total = 0;
	BSP_U32 u32UlState = 0;
	BSP_U32 u32DlState = 0;
    BSP_U32 u32BdqWptr = 0;

	/* �ж�����IPF�Ƿ���� */    
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_STATE(IPF_CHANNEL_UP), u32UlState);

    if(u32UlState != 0x14)
    {
    	g_stIPFDebugInfo->u32UlIPFBusyNum++;
        return IPF_ERROR;
    }

	/* �ж�����IPF�Ƿ���� */  	    
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_STATE(IPF_CHANNEL_DOWN), u32DlState);

    if(u32DlState != 0x14)
    {
    	g_stIPFDebugInfo->u32DlIPFBusyNum++;
        return IPF_ERROR;
    }

    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_BDQ_WPTR(IPF_CHANNEL_DOWN), u32BdqWptr);
    if(g_stIpfDl.u32IpfBDFptr != u32BdqWptr)
    {
    	g_stIPFDebugInfo->u32DlIPFBusyNum++;
        return IPF_ERROR;
    }

	/* �����Ҫ�ļĴ��� */
    u32Total = sizeof(g_stIpfRegTable)/sizeof(IPF_REG_INFO_S);
    u8addr = g_u8IpfBakAddr;
    
    for(i=0; i<u32Total; i++)
    {
        memcpy((BSP_VOID*)u8addr, 
        (BSP_VOID*)(g_stIpfRegTable[i].u32StartAddr+IPF_REGBASE_ADR), g_stIpfRegTable[i].u32Length);
        u8addr += g_stIpfRegTable[i].u32Length;
    }
    
    /* ɾ�����ά���Ĺ��˱���Ϣ */
	for(i = 0; i < IPF_CHANNEL_MAX; i++)
    {
    	IPF_DeleteAll(i);		
	}

	/* ��λIPF */
	BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_SRST, 1); 

    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��       : BSP_IPF_DrxRestoreReg
*
* ��������  : �ṩ�ĵ͹��Ľӿڣ�����IPF�ϵ�ָ�
*
* �������  : ��
*             
* �������  : ��
* �� �� ֵ     : ��
*
* ע��:�ú�����Ҫ�����жϵ�����µ���
* �޸ļ�¼  : 2011��2��14��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPF_DrxRestoreReg()
{
    BSP_U8* u8addr = BSP_NULL;
    BSP_U32 i = 0;
    BSP_U32 j = 0;
    BSP_U32 u32Total = 0;
    BSP_U8* u8FilterIndexAddr = BSP_NULL;
	IPF_CHANNEL_TYPE_E eChnType;
	IPF_FILTER_CONFIG_S *pstFilterInfo;
	BSP_U32 u32FilterNum;
	BSP_S32 s32Ret = 0;

	/* �����λ */
	BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_SRST, 0); 

    u32Total = sizeof(g_stIpfRegTable)/sizeof(IPF_REG_INFO_S);
    u8addr = g_u8IpfBakAddr;

    for(i=0; i<u32Total; i++)
    {
        memcpy((BSP_VOID*)(g_stIpfRegTable[i].u32StartAddr+IPF_REGBASE_ADR), (BSP_VOID*)u8addr, g_stIpfRegTable[i].u32Length);
        u8addr += g_stIpfRegTable[i].u32Length;
    }

	g_u32IpfStartRestore = 1;

	for(i = 0; i < IPF_CHANNEL_MAX; i++)
	{
		if(g_stIPFPwrCtlFilterInfo[i].u32Flag)
		{			
			u32FilterNum = g_stIPFPwrCtlFilterInfo[i].u32FilterNum;
			eChnType = g_stIPFPwrCtlFilterInfo[i].eChnType;
			pstFilterInfo = g_stIPFPwrCtlFilterInfo[i].pstFilterInfo;
			s32Ret = BSP_IPF_SetFilter(eChnType, pstFilterInfo, u32FilterNum);
			if(s32Ret != IPF_SUCCESS)
			{				
				BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,"\r BSP_IPF_DrxRestoreReg restore error! \n");
			}
		}
	}

	g_u32IpfStartRestore = 0;
	g_stIpfDl.u32IpfCdWptr = 0;
	g_stIpfDl.u32IpfCdRptr = 0;
	g_stIpfDl.u32IpfBDFptr = 0;
}
BSP_VOID BSP_IPF_IDLEReg()
{
    BSP_U8* u8addr = BSP_NULL;
    BSP_U32 i = 0;
    BSP_U32 u32Total = 0;
	IPF_FILTER_CONFIG_S stFilterInfo;
	BSP_U32 u32FilterNum;
	BSP_S32 s32Ret = 0;

    u32Total = sizeof(g_stIpfRegTable)/sizeof(IPF_REG_INFO_S);
    u8addr = g_u8IpfBakAddr;

    for(i=0; i<u32Total; i++)
    {
        memset((BSP_VOID*)(g_stIpfRegTable[i].u32StartAddr+IPF_REGBASE_ADR), 0x0, g_stIpfRegTable[i].u32Length);
        u8addr += g_stIpfRegTable[i].u32Length;
    }
	g_u32IpfStartRestore = 1;
	for(i = 0; i < IPF_CHANNEL_MAX; i++)
	{
		BSP_IPF_SetFilter(i, &stFilterInfo, 0);
	}

	g_u32IpfStartRestore = 0;
}
/*****************************************************************************
* �� �� ��  : BSP_IPF_UseFilterInfo
*
* ��������  : �ṩ��debug�ӿڣ���ȡ�Ѿ�ʹ�õ�filter��
*
* �������  : IPF_CHANNEL_TYPE_E eChnType  ͨ������
*             
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��1��11��   ³��  ����
                             2011��3��30��   ³��  �޸�
*****************************************************************************/
BSP_VOID BSP_IPF_UseFilterInfo(IPF_CHANNEL_TYPE_E eChnType)
{
    IPF_ID_S* current = BSP_NULL;
    BSP_U32 u32FilterNum = 0;
    BSP_S32 s32GetChar = 0;
    BSP_U32 u32cyc = 10;
    
    u32FilterNum = g_stIPFFilterInfo[eChnType].u32FilterNum;
    current = g_stIPFFilterInfo[eChnType].pstUseList;

    IPF_PRINT("=============================\n");

    while(u32FilterNum)
    { 
        while(u32cyc&&u32FilterNum)
        {
            IPF_PRINT("Filter ID = %d,    PS ID = %d\n",current->u32FilterID, current->u32PsID);            
            current = current->pstNext;
            u32FilterNum--;
            u32cyc--;
        }
        if(u32FilterNum > 0)
        {
            IPF_PRINT("\npress \'Enter\' to continue, press \'q\' to stop\n");
            s32GetChar = getchar();/*lint !e666*/
            if ('q' == s32GetChar)
            {
                break;
            }
            u32cyc = 10;
        }
        else
        {
            break;
        }
    }
    IPF_PRINT("*****************************\n");

}

/*****************************************************************************
* �� �� ��  : BSP_IPF_FreeFilterInfo
*
* ��������  : �ṩ��debug�ӿڣ���ȡδʹ�õ�filter��
*
* �������  : IPF_CHANNEL_TYPE_E eChnType  ͨ������
*             
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��1��11��   ³��  ����
*
*****************************************************************************/
BSP_VOID BSP_IPF_FreeFilterInfo()
{
    IPF_ID_S* current = BSP_NULL;
    BSP_S32 s32GetChar = 0;
    BSP_U32 u32cyc = 10;

    current = g_stIPFFreeList;

    IPF_PRINT("============================\n");

    while(current != BSP_NULL)
    {
        while(u32cyc&&(current != BSP_NULL))
        {
            IPF_PRINT("Filter ID:      %d\n",current->u32FilterID);
            current = current->pstNext; 
            u32cyc--;
        }
        IPF_PRINT("\npress \'Enter\' to continue, press \'q\' to stop\n");
        s32GetChar = getchar();/*lint !e666*/
        if ('q' == s32GetChar)
        {
            break;
        }
        u32cyc = 10;
    }
    IPF_PRINT("*****************************\n");    
}

BSP_VOID BSP_IPF_FilterInfoHWID(BSP_U32 u32Num)
{
    BSP_U32* p = BSP_NULL;
    BSP_U32 i = 0;
    IPF_MATCH_INFO_S stMatchInfo = {0}; 
    
    if(u32Num < IPF_BF_NUM)
    {
         /* д���˱������ַ */    
        BSP_REG_WRITE(IPF_REGBASE_ADR, BFLT_INDEX, u32Num); 
         
        /* ���������������� */
        memcpy(&stMatchInfo, (BSP_VOID*)(IPF_REGBASE_ADR+FLT_LOCAL_ADDR0), sizeof(IPF_MATCH_INFO_S));
    }
    else
    {
        memcpy(&stMatchInfo, (BSP_VOID*)(g_pstExFilterAddr + u32Num - IPF_BF_NUM), sizeof(IPF_MATCH_INFO_S));
    }

    IPF_PRINT("============================\n");
    IPF_PRINT("src ADDR0 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[0], stMatchInfo.u8SrcAddr[1], stMatchInfo.u8SrcAddr[2], stMatchInfo.u8SrcAddr[3]);
    IPF_PRINT("src ADDR1 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[4], stMatchInfo.u8SrcAddr[5], stMatchInfo.u8SrcAddr[6], stMatchInfo.u8SrcAddr[7]);
    IPF_PRINT("src ADDR2 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[8], stMatchInfo.u8SrcAddr[9], stMatchInfo.u8SrcAddr[10], stMatchInfo.u8SrcAddr[11]);
    IPF_PRINT("src ADDR3 : %x.%x.%x.%x\n", stMatchInfo.u8SrcAddr[12], stMatchInfo.u8SrcAddr[13], stMatchInfo.u8SrcAddr[14], stMatchInfo.u8SrcAddr[15]);
    IPF_PRINT("src MASK0 : %x.%x.%x.%x\n", stMatchInfo.u8SrcMsk[0], stMatchInfo.u8SrcMsk[1], stMatchInfo.u8SrcMsk[2], stMatchInfo.u8SrcMsk[3]);
    IPF_PRINT("src MASK1 : %x.%x.%x.%x\n", stMatchInfo.u8SrcMsk[4], stMatchInfo.u8SrcMsk[5], stMatchInfo.u8SrcMsk[6], stMatchInfo.u8SrcMsk[7]);
    IPF_PRINT("src MASK2 : %x.%x.%x.%x\n", stMatchInfo.u8SrcMsk[8], stMatchInfo.u8SrcMsk[9], stMatchInfo.u8SrcMsk[10], stMatchInfo.u8SrcMsk[11]);
    IPF_PRINT("src MASK3 : %x.%x.%x.%x\n", stMatchInfo.u8SrcMsk[12], stMatchInfo.u8SrcMsk[13], stMatchInfo.u8SrcMsk[14], stMatchInfo.u8SrcMsk[15]);
    IPF_PRINT("dst ADDR0 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[0], stMatchInfo.u8DstAddr[1], stMatchInfo.u8DstAddr[2], stMatchInfo.u8DstAddr[3]);
    IPF_PRINT("dst ADDR1 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[4], stMatchInfo.u8DstAddr[5], stMatchInfo.u8DstAddr[6], stMatchInfo.u8DstAddr[7]);
    IPF_PRINT("dst ADDR2 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[8], stMatchInfo.u8DstAddr[9], stMatchInfo.u8DstAddr[10], stMatchInfo.u8DstAddr[11]);
    IPF_PRINT("dst ADDR3 : %x.%x.%x.%x\n", stMatchInfo.u8DstAddr[12], stMatchInfo.u8DstAddr[13], stMatchInfo.u8DstAddr[14], stMatchInfo.u8DstAddr[15]);
    IPF_PRINT("dst MASK0 : %x.%x.%x.%x\n", stMatchInfo.u8DstMsk[0], stMatchInfo.u8DstMsk[1], stMatchInfo.u8DstMsk[2], stMatchInfo.u8DstMsk[3]);
    IPF_PRINT("dst MASK1 : %x.%x.%x.%x\n", stMatchInfo.u8DstMsk[4], stMatchInfo.u8DstMsk[5], stMatchInfo.u8DstMsk[6], stMatchInfo.u8DstMsk[7]);
    IPF_PRINT("dst MASK2 : %x.%x.%x.%x\n", stMatchInfo.u8DstMsk[8], stMatchInfo.u8DstMsk[9], stMatchInfo.u8DstMsk[10], stMatchInfo.u8DstMsk[11]);
    IPF_PRINT("dst MASK3 : %x.%x.%x.%x\n", stMatchInfo.u8DstMsk[12], stMatchInfo.u8DstMsk[13], stMatchInfo.u8DstMsk[14], stMatchInfo.u8DstMsk[15]);
    IPF_PRINT("SrcPortLo : %d  SrcPortHi: %d\n", stMatchInfo.unSrcPort.Bits.u16SrcPortLo, stMatchInfo.unSrcPort.Bits.u16SrcPortHi);
    IPF_PRINT("DstPortLo : %d  DstPortHi: %d\n", stMatchInfo.unDstPort.Bits.u16DstPortLo, stMatchInfo.unDstPort.Bits.u16DstPortHi);
    IPF_PRINT("TrafficClass :      %d\n", stMatchInfo.unTrafficClass.u32TrafficClass);
    IPF_PRINT("TrafficClassMsk :  %d\n", stMatchInfo.unTrafficClassMsk.u32TrafficClassMsk);
    IPF_PRINT("Protocol :          %d\n", stMatchInfo.unNextHeader.u32Protocol);
    IPF_PRINT("FlowLable :         %d\n", stMatchInfo.u32FlowLable);
    IPF_PRINT("Type :        %d  Code: %d\n", stMatchInfo.unFltCodeType.Bits.u16Type, stMatchInfo.unFltCodeType.Bits.u16Code);
    IPF_PRINT("NextIndex : %d  FltPri: %d\n", stMatchInfo.unFltChain.Bits.u16NextIndex, stMatchInfo.unFltChain.Bits.u16FltPri);
    IPF_PRINT("FltSpi :             %d\n", stMatchInfo.u32FltSpi);
    IPF_PRINT("FltRuleCtrl :        %x\n", stMatchInfo.unFltRuleCtrl.u32FltRuleCtrl);    
    IPF_PRINT("============================\n");
}

BSP_VOID BSP_IPF_FilterInfoPSID(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32Num)
{
    BSP_U32 u32FindID = 0;
    
    /* ����PS ID ��Ӧ��FilterID*/
    if(IPF_FindFilterID(eChnType, u32Num, &u32FindID) != IPF_SUCCESS)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF, "\r cannot find filter ID! \n");
        return ;
    }

    BSP_IPF_FilterInfoHWID(u32FindID);
}
#else
/*****************************************************************************
* �� �� ��     : BSP_IPF_RegisterWakeupDlCb
*
* ��������  : �ýӿ�����ע������PS����ص�����
*                           
* �������  : BSP_IPF_WakeupDlkCb *pFnWakeupDl
*
* �������  : ��
* �� �� ֵ      : ��
*  
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_S32 BSP_IPF_RegisterWakeupDlCb(BSP_IPF_WakeupDlCb pFnWakeupDl)
{
    /* ������� */
    if(BSP_NULL == pFnWakeupDl)
    {

        return BSP_ERR_IPF_INVALID_PARA;
    }
    g_stIpfDl.pFnDlIntCb = pFnWakeupDl;
    return IPF_SUCCESS;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetDlRd
*
* ��������  : �ýӿ����ڶ�ȡ����BD, ֧��һ�ζ�ȡ���BD
*
* �������  : BSP_U32* pu32Num    
*                           IPF_RD_DESC_S *pstRd
*   
* �������  : BSP_U32* pu32Num    ʵ�ʶ�ȡ��RD��Ŀ
*
* �� �� ֵ     : IPF_SUCCESS               �����ɹ�
*                           IPF_ERROR                   ����ʧ��
*
* �޸ļ�¼  :2011��11��30��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPF_GetDlRd(BSP_U32* pu32Num, IPF_RD_DESC_S *pstRd)
{
    BSP_U32 u32RdqRptr = 0;
    BSP_U32 u32RdqDepth = 0;
    BSP_U32 u32Num = 0;
    BSP_U32 i = 0;
  
    /* ��ȡRD��� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_DQ_DEPTH(IPF_CHANNEL_DOWN), u32RdqDepth);
    u32RdqDepth = (u32RdqDepth>>16)&0x1FF;
    
    u32Num = (u32RdqDepth < *pu32Num)?u32RdqDepth:*pu32Num;
    for(i = 0; i < u32Num; i++)
    {
        /* ��ȡRD��ָ�� */
        BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_RDQ_RPTR(IPF_CHANNEL_DOWN), u32RdqRptr);
        
        /* ��ȡRD */
        pstRd[i].u16Attribute = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16Attribute;
        pstRd[i].u16PktLen = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16PktLen;
        pstRd[i].u16Result = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16Result;
        pstRd[i].u32InPtr = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32InPtr;
        pstRd[i].u32OutPtr = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32OutPtr;
        pstRd[i].u16UsrField1 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u16UsrField1;
        pstRd[i].u32UsrField2 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField2;
        pstRd[i].u32UsrField3 = g_stIpfDl.pstIpfRDQ[u32RdqRptr].u32UsrField3;
                
        /* ����RD��ָ�� */
        u32RdqRptr = ((u32RdqRptr+1) < IPF_DLRD_DESC_SIZE)?(u32RdqRptr+1):0;        
        BSP_REG_WRITE(IPF_REGBASE_ADR, IPF_CH_RDQ_RPTR(IPF_CHANNEL_DOWN), u32RdqRptr);         
    }
    
    *pu32Num = u32Num;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetUlRdNum
*
* ��������  : �ýӿ����ڶ�ȡ����RD��Ŀ
*
* �������  : ��
*   
* �������  : ��
*
* �� �� ֵ     : ����RD��Ŀ
*
* �޸ļ�¼  :2012��2��16��   ³��  ����
*****************************************************************************/
BSP_U32 BSP_IPF_GetUlRdNum(BSP_VOID)
{
    BSP_U32 u32RdqDepth = 0;
  
    /* ��ȡRD��� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_DQ_DEPTH(IPF_CHANNEL_UP), u32RdqDepth);
    u32RdqDepth = (u32RdqDepth>>16)&0x1FF;
    return u32RdqDepth;
}

/*****************************************************************************
* �� �� ��     : BSP_IPF_GetDlRdNum
*
* ��������  : �ýӿ����ڶ�ȡ����RD��Ŀ
*
* �������  : ��
*   
* �������  : ��
*
* �� �� ֵ     : ����RD��Ŀ
*
* �޸ļ�¼  :2012��7��16��   ³��  ����
*****************************************************************************/
BSP_U32 BSP_IPF_GetDlRdNum(BSP_VOID)
{
    BSP_U32 u32RdqDepth = 0;
  
    /* ��ȡRD��� */
    BSP_REG_READ(IPF_REGBASE_ADR, IPF_CH_DQ_DEPTH(IPF_CHANNEL_DOWN), u32RdqDepth);
    u32RdqDepth = (u32RdqDepth>>16)&0x1FF;
    return u32RdqDepth;
}


#endif

#ifdef __KERNEL__
module_init(IPF_Init);/*lint !e528*/
EXPORT_SYMBOL(BSP_IPF_ConfigTimeout);
EXPORT_SYMBOL(BSP_IPF_ConfigUpFilter);
EXPORT_SYMBOL(BSP_IPF_GetUlBDNum);
EXPORT_SYMBOL(BSP_IPF_UlStateIdle);
EXPORT_SYMBOL(BSP_IPF_SetPktLen);
EXPORT_SYMBOL(BSP_IPF_GetStat);
EXPORT_SYMBOL(BSP_IPF_RegisterWakeupDlCb);
EXPORT_SYMBOL(BSP_IPF_GetDlRd);
EXPORT_SYMBOL(BSP_IPF_GetUlRdNum);
EXPORT_SYMBOL(BSP_IPF_GetDlRdNum);
EXPORT_SYMBOL(BSP_IPF_Help);
EXPORT_SYMBOL(BSP_IPF_BDInfo);
EXPORT_SYMBOL(BSP_IPF_RDInfo);
EXPORT_SYMBOL(BSP_IPF_Info);
EXPORT_SYMBOL(BSP_IPF_MEM);
MODULE_LICENSE("GPL");

#endif

#ifdef __cplusplus
}
#endif


