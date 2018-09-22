/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_DRV_IPF.h
*
*   ��    �� :  luting 00168886
*
*   ��    �� :  IPFģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2011��01��21��  v1.00  luting  ����
*************************************************************************/

#ifndef    _BSP_DRV_IPF_H_
#define    _BSP_DRV_IPF_H_

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */

/**************************************************************************
  ͷ�ļ�����                            
**************************************************************************/
#ifdef __VXWORKS__
#include <vxWorks.h>
#include <lstLib.h>
#include "product_config.h"
#include "BSP.h"
#else
#include "BSP.h"
#include <mach/hardware.h>
#include <mach/common/bsp_memory.h>
#include <mach/balong_v100r001.h>
#endif
#include "BSP_IPF.h"
/**************************************************************************
  �궨�� 
**************************************************************************/
#ifdef __BSP_DEBUG__
#define __BSP_IPF_DEBUG__
#endif


#define IPF_ULBD_MEM_ADDR              MEMORY_AXI_IPF_ADDR
#define IPF_ULBD_MEM_SIZE               (IPF_ULBD_DESC_SIZE * sizeof(IPF_BD_DESC_S))

#define IPF_ULRD_MEM_ADDR              (IPF_ULBD_MEM_ADDR + IPF_ULBD_MEM_SIZE)
#define IPF_ULRD_MEM_SIZE               (IPF_ULRD_DESC_SIZE * sizeof(IPF_RD_DESC_S))

#define IPF_DLBD_MEM_ADDR              (IPF_ULRD_MEM_ADDR + IPF_ULRD_MEM_SIZE)
#define IPF_DLBD_MEM_SIZE               (IPF_DLBD_DESC_SIZE * sizeof(IPF_BD_DESC_S))

#define IPF_DLRD_MEM_ADDR              (IPF_DLBD_MEM_ADDR + IPF_DLBD_MEM_SIZE)
#define IPF_DLRD_MEM_SIZE               (IPF_DLRD_DESC_SIZE * sizeof(IPF_RD_DESC_S))

#define IPF_DLCD_MEM_ADDR              (IPF_DLRD_MEM_ADDR + IPF_DLRD_MEM_SIZE)
#define IPF_DLCD_MEM_SIZE               (IPF_DLCD_DESC_SIZE * sizeof(IPF_CD_DESC_S))

#define IPF_INIT_ADDR                        (IPF_DLCD_MEM_ADDR + IPF_DLCD_MEM_SIZE)
#define IPF_INIT_SIZE                          16

#define IPF_ULBD_IDLENUM_ADDR       (IPF_INIT_ADDR + IPF_INIT_SIZE)
#define IPF_ULBD_IDLENUM_SIZE          16

#define IPF_DEBUG_INFO_ADDR           (IPF_ULBD_IDLENUM_ADDR + IPF_ULBD_IDLENUM_SIZE)
#define IPF_DEBUG_INFO_SIZE            sizeof(IPF_DEBUG_INFO_S)

#define IPF_DEBUG_DLCD_ADDR           (IPF_DEBUG_INFO_ADDR + IPF_DEBUG_INFO_SIZE)
#define IPF_DEBUG_DLCD_SIZE            (IPF_DLCD_DESC_SIZE * sizeof(IPF_CD_DESC_S))

#define IPF_PWRCTL_INFO_ADDR		(IPF_DEBUG_DLCD_ADDR + IPF_DEBUG_DLCD_SIZE)
#define IPF_PWRCTL_INFO_SIZE        1024
#define IPF_BF_NUM                                132  /* ������������Ŀ */
#define IPF_TOTAL_FILTER_NUM             256  /* ����������Ŀ */
#define EXFLITER_NUM                            (IPF_TOTAL_FILTER_NUM-IPF_BF_NUM) /* ��չ��������Ŀ */
#define IPF_TAIL_INDEX                          511  /* filter��ʶ���һ��filter */

#define TIME_OUT_CFG                       500           /* RD��ʱʱ�� */

#define IPF_INT_OPEN0                      0x83
#define IPF_INT_OPEN1                      0x830000

#define IPF_SRST                            0x000
#define IPF_SRST_STATE                      0x004
#define IPF_CH_EN                         0x0008
#define IPF_EN_STATE                   0x000C
#define IPF_CTRL                            0x0014
#define IPF_INT0                            0x0030
#define IPF_INT1                            0x0034
#define IPF_INT_MASK0                 0x0038
#define IPF_INT_MASK1                 0x003C
#define IPF_INT_STATE                 0x0040
#define IPF_TIME_OUT                   0x0044
#define IPF_PKT_LEN                     0x0048
#define IPF_FLT_ZERO_INDEX        0x0070
#define IPF_EF_BADDR                   0x0074
#define BFLT_INDEX                       0x0078
#define FLT_LOCAL_ADDR0             0x0080
#define FTL_CHAIN                         0x00DC
#define IPF_CH_CTRL(i)                 (0x0100 + i * 0x50)
#define IPF_CH_STATE(i)               (IPF_CH_CTRL(i) + 0x04)
#define IPF_CH_BDQ_BADDR(i)      (IPF_CH_CTRL(i) + 0x08)
#define IPF_CH_BDQ_SIZE(i)         (IPF_CH_CTRL(i) + 0x0C)
#define IPF_CH_BDQ_WPTR(i)        (IPF_CH_CTRL(i) + 0x10)
#define IPF_CH_BDQ_RPTR(i)         (IPF_CH_CTRL(i) + 0x14)
#define IPF_CH_BDQ_WADDR(i)      (IPF_CH_CTRL(i) + 0x18)
#define IPF_CH_BDQ_RADDR(i)       (IPF_CH_CTRL(i) + 0x1C)
#define IPF_CH_RDQ_BADDR(i)      (IPF_CH_CTRL(i) + 0x20)
#define IPF_CH_RDQ_SIZE(i)         (IPF_CH_CTRL(i) + 0x24)
#define IPF_CH_RDQ_WPTR(i)         (IPF_CH_CTRL(i) + 0x28)
#define IPF_CH_RDQ_RPTR(i)         (IPF_CH_CTRL(i) + 0x2C)
#define IPF_CH_RDQ_WADDR(i)      (IPF_CH_CTRL(i) + 0x30)
#define IPF_CH_RDQ_RADDR(i)       (IPF_CH_CTRL(i) + 0x34)
#define IPF_CH_DQ_DEPTH(i)         (IPF_CH_CTRL(i) + 0x38)
#define IPF_UL_CNT0                     0x0200
#define IPF_UL_CNT1                     0x0204
#define IPF_UL_CNT2                     0x0208
#define IPF_DL_CNT0                     0x0210
#define IPF_DL_CNT1                     0x0214
#define IPF_DL_CNT2                     0x0218

#ifdef __VXWORKS__
#define IPF_PRINT                         printf
#elif defined(__KERNEL__)

#if (defined (BOARD_FPGA) && defined (VERSION_V3R2))
#define IPF_REGBASE_ADR           IO_ADDRESS(0x5f0b0000)
#elif	((defined (BOARD_FPGA_P500) && defined (CHIP_BB_6920CS)) || \
	    (defined (BOARD_FPGA_P500) && defined (CHIP_BB_6920ES)))
#define IPF_REGBASE_ADR           IO_ADDRESS(0x500ac000)
#else
#define IPF_REGBASE_ADR           IO_ADDRESS(0x900ac000)
#endif
#define IPF_PRINT                         printk
#endif

    
/**************************************************************************
  ö�ٶ���
**************************************************************************/

/**************************************************************************
  STRUCT����
**************************************************************************/
typedef struct tagIPF_UL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;    
    IPF_RD_DESC_S* pstIpfRDQ;
    BSP_IPF_WakeupUlCb pFnUlIntCb; /* �ж��л��ѵ�PS���� */
    BSP_U32* pu32IdleBd; /* ��¼��һ�λ�ȡ�Ŀ���BD �� */
}IPF_UL_S;

typedef struct tagIPF_DL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;    
    IPF_RD_DESC_S* pstIpfRDQ;
    IPF_CD_DESC_S* pstIpfCDQ;
    IPF_CD_DESC_S* pstIpfDebugCDQ;
#ifndef PRODUCT_CFG_CORE_TYPE_MODEM	
    BSP_IPF_WakeupDlCb pFnDlIntCb; /* �ж��л��ѵ�PS���� */
#else
    BSP_IPF_DlFreeMemCb pFnDlFreeMemCb;/* �����ͷ�ԴTTF�ڴ�ص����� */
#endif
    BSP_U32 u32IpfCdRptr; /* ��ǰ���Զ�����λ�� */
    BSP_U32 u32IpfCdWptr;
    BSP_U32 u32IpfBDFptr; /* ��¼BD�ͷ�ָ��λ�� */
    BSP_U32 u32IdleBd; /* ��¼��һ�λ�ȡ�Ŀ���BD �� */
}IPF_DL_S;

typedef struct tagIPF_ID_S
{
    BSP_U32 u32PsID;
    BSP_U32 u32FilterID;
    struct tagIPF_ID_S* pstNext;
}IPF_ID_S;

typedef struct tagIPF_FILTER_INFO_S
{
    IPF_ID_S* pstUseList;
    BSP_U32 u32FilterNum;
}IPF_FILTER_INFO_S;

typedef struct tagIPF_DEBUG_INFO_S
{
#ifdef __BSP_IPF_DEBUG__
    BSP_U32 u32IpfDebug;
#endif
    BSP_U32 u32UlBdqOverflow; /* �ж��ϱ�����BD����������� */
    BSP_U32 u32DlBdqOverflow; /* �ж��ϱ�����BD����������� */
    BSP_U32 u32UlBdNotEnough;       /* ��������BD,BD�����ô��� */
    BSP_U32 u32DlBdNotEnough;       /* ��������BD,BD�����ô��� */
    BSP_U32 u32DlCdNotEnough;       /* ��������CD,CD�����ô��� */
	BSP_U32 u32UlIPFBusyNum;       /* ����IPFæ���� */
	BSP_U32 u32DlIPFBusyNum;       /* ����IPFæ���� */
}IPF_DEBUG_INFO_S;

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM	
typedef struct tagIPF_PWRCTL_FILTER_INFO_S
{
	BSP_U32 u32Flag;	 
	IPF_CHANNEL_TYPE_E eChnType;
	BSP_U32 u32FilterNum;
	IPF_FILTER_CONFIG_S *pstFilterInfo; 	
}IPF_PWRCTL_FILTER_INFO_S;
typedef struct tagIPF_REG_INFO_S
{
	BSP_U32 u32StartAddr; /* ��Ҫ���ݵļĴ�����ַ */
	BSP_U32 u32Length;		/* ��Ҫ���ݵĳ��� */
}IPF_REG_INFO_S;
#endif

/**************************************************************************
  UNION����
**************************************************************************/

/**************************************************************************
  OTHERS����
**************************************************************************/



/**************************************************************************
  ��������
**************************************************************************/

/*****************************************************************************
* �� �� ��  : IPF_Init
*
* ��������  : IPF��ʼ��     �ڲ�ʹ�ã�����Ϊ�ӿں���
*
* �������  : BSP_VOID
* �������  : ��
* �� �� ֵ  : IPF_SUCCESS    ��ʼ���ɹ�
*             IPF_ERROR      ��ʼ��ʧ��
*
* �޸ļ�¼  :2011��1��21��   ³��  ����
*****************************************************************************/
BSP_S32 IPF_Init(BSP_VOID);

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
BSP_VOID IPF_Int_Connect(BSP_VOID);

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
irqreturn_t  IPF_IntHandler (int irq, void* dev);
#else
BSP_VOID IPF_IntHandler(BSP_VOID);
#endif

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
/*****************************************************************************
* �� �� ��  : IPF_FilterList_Init
*
* ��������  : IPF�����������ʼ��     �ڲ�ʹ�ã�����Ϊ�ӿں���
*
* �������  : BSP_VOID
* �������  : ��
* �� �� ֵ  : IPF_SUCCESS    �����������ʼ���ɹ�
*             IPF_ERROR      �����������ʼ��ʧ��
*
* �޸ļ�¼  :2011��3��30��   ³��  ����
*****************************************************************************/
BSP_S32 IPF_FilterList_Init(BSP_VOID);

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
BSP_VOID IPF_DeleteAll(IPF_CHANNEL_TYPE_E eChnType);

/*****************************************************************************
* �� �� ��  : IPF_MallocOneFilter
*
* ��������  : ��freelist�з���һ�����
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
IPF_ID_S* IPF_MallocOneFilter(BSP_VOID);

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
BSP_VOID IPF_AddTailUsedFilter(IPF_CHANNEL_TYPE_E eChnType, IPF_ID_S* stNode);

/*****************************************************************************
* �� �� ��  : IPF_AddTailFilterChain
*
* ��������  : ���µĹ������������õ��Ĵ��������޸���һ����������nextindex
*
* �������  : BSP_U32 u32LastFilterID  ��һ����������ID
*             BSP_U32 u32FilterID  ��ǰ�����õĹ�����ID
*             IPF_MATCH_INFO_S* pstMatchInfo  ��ǰ�����õĹ����������ṹ��ָ��
*
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��5��12��   ³��  ����
*****************************************************************************/
BSP_VOID IPF_AddTailFilterChain(BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo);

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
BSP_S32 IPF_FindFilterID(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32PsID, BSP_U32* u32FilterID);

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
BSP_S32 IPF_ConfigCD(TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr);
#endif


#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _BSP_DMAC_H_ */


