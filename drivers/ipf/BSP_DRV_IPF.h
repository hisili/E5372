/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DRV_IPF.h
*
*   作    者 :  luting 00168886
*
*   描    述 :  IPF模块用户接口文件
*
*   修改记录 :  2011年01月21日  v1.00  luting  创建
*************************************************************************/

#ifndef    _BSP_DRV_IPF_H_
#define    _BSP_DRV_IPF_H_

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */

/**************************************************************************
  头文件包含                            
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
  宏定义 
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
#define IPF_BF_NUM                                132  /* 基本过滤器数目 */
#define IPF_TOTAL_FILTER_NUM             256  /* 过滤器总数目 */
#define EXFLITER_NUM                            (IPF_TOTAL_FILTER_NUM-IPF_BF_NUM) /* 扩展过滤器数目 */
#define IPF_TAIL_INDEX                          511  /* filter标识最后一个filter */

#define TIME_OUT_CFG                       500           /* RD超时时间 */

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
  枚举定义
**************************************************************************/

/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct tagIPF_UL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;    
    IPF_RD_DESC_S* pstIpfRDQ;
    BSP_IPF_WakeupUlCb pFnUlIntCb; /* 中断中唤醒的PS任务 */
    BSP_U32* pu32IdleBd; /* 记录上一次获取的空闲BD 数 */
}IPF_UL_S;

typedef struct tagIPF_DL_S
{
    IPF_BD_DESC_S* pstIpfBDQ;    
    IPF_RD_DESC_S* pstIpfRDQ;
    IPF_CD_DESC_S* pstIpfCDQ;
    IPF_CD_DESC_S* pstIpfDebugCDQ;
#ifndef PRODUCT_CFG_CORE_TYPE_MODEM	
    BSP_IPF_WakeupDlCb pFnDlIntCb; /* 中断中唤醒的PS任务 */
#else
    BSP_IPF_DlFreeMemCb pFnDlFreeMemCb;/* 下行释放源TTF内存回调函数 */
#endif
    BSP_U32 u32IpfCdRptr; /* 当前可以读出的位置 */
    BSP_U32 u32IpfCdWptr;
    BSP_U32 u32IpfBDFptr; /* 记录BD释放指针位置 */
    BSP_U32 u32IdleBd; /* 记录上一次获取的空闲BD 数 */
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
    BSP_U32 u32UlBdqOverflow; /* 中断上报上行BD队列溢出次数 */
    BSP_U32 u32DlBdqOverflow; /* 中断上报下行BD队列溢出次数 */
    BSP_U32 u32UlBdNotEnough;       /* 上行配置BD,BD不够用次数 */
    BSP_U32 u32DlBdNotEnough;       /* 下行配置BD,BD不够用次数 */
    BSP_U32 u32DlCdNotEnough;       /* 下行配置CD,CD不够用次数 */
	BSP_U32 u32UlIPFBusyNum;       /* 上行IPF忙次数 */
	BSP_U32 u32DlIPFBusyNum;       /* 下行IPF忙次数 */
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
	BSP_U32 u32StartAddr; /* 需要备份的寄存器地址 */
	BSP_U32 u32Length;		/* 需要备份的长度 */
}IPF_REG_INFO_S;
#endif

/**************************************************************************
  UNION定义
**************************************************************************/

/**************************************************************************
  OTHERS定义
**************************************************************************/



/**************************************************************************
  函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : IPF_Init
*
* 功能描述  : IPF初始化     内部使用，不作为接口函数
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS    初始化成功
*             IPF_ERROR      初始化失败
*
* 修改记录  :2011年1月21日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_Init(BSP_VOID);

/*****************************************************************************
* 函 数 名     : IPF_Int_Connect
*
* 功能描述  : 挂IPF中断处理函数(两核都提供)
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值      : 无
*
* 修改记录  :2011年12月2日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_Int_Connect(BSP_VOID);

/*****************************************************************************
* 函 数 名  : IPF_IntHandler
*
* 功能描述  : IPF中断处理函数
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月24日   鲁婷  创建
*****************************************************************************/
#ifdef __KERNEL__
irqreturn_t  IPF_IntHandler (int irq, void* dev);
#else
BSP_VOID IPF_IntHandler(BSP_VOID);
#endif

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
/*****************************************************************************
* 函 数 名  : IPF_FilterList_Init
*
* 功能描述  : IPF过滤器链表初始化     内部使用，不作为接口函数
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
* 返 回 值  : IPF_SUCCESS    过滤器链表初始化成功
*             IPF_ERROR      过滤器链表初始化失败
*
* 修改记录  :2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_FilterList_Init(BSP_VOID);

/*****************************************************************************
* 函 数 名  : IPF_DeleteAll
*
* 功能描述  : 删除链表中的所有结点
*
* 输入参数  : IPF_CHANNEL_TYPE_E eChnType  通道类型           
*             
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  :2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_DeleteAll(IPF_CHANNEL_TYPE_E eChnType);

/*****************************************************************************
* 函 数 名  : IPF_MallocOneFilter
*
* 功能描述  : 从freelist中分配一个结点
*
* 输入参数  : 无       
*             
* 输出参数  : 无

* 返 回 值  : 结点指针
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*
* 说明      : 由调用函数来保证一定能分配到结点
*****************************************************************************/
IPF_ID_S* IPF_MallocOneFilter(BSP_VOID);

/*****************************************************************************
* 函 数 名  : IPF_AddTailUsedFilter
*
* 功能描述  : 将结点加到uselist的结尾
*
* 输入参数  : IPF_CHANNEL_TYPE_E eChnType  通道类型   
*             IPF_ID_S* stNode             待插入的结点指针
*             
* 输出参数  : 无

* 返 回 值  : 无
*
* 修改记录  : 2011年3月30日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_AddTailUsedFilter(IPF_CHANNEL_TYPE_E eChnType, IPF_ID_S* stNode);

/*****************************************************************************
* 函 数 名  : IPF_AddTailFilterChain
*
* 功能描述  : 将新的过滤器配置配置到寄存器，并修改上一个过滤器的nextindex
*
* 输入参数  : BSP_U32 u32LastFilterID  上一个过滤器的ID
*             BSP_U32 u32FilterID  当前需配置的过滤器ID
*             IPF_MATCH_INFO_S* pstMatchInfo  当前需配置的过滤器参数结构体指针
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年5月12日   鲁婷  创建
*****************************************************************************/
BSP_VOID IPF_AddTailFilterChain(BSP_U32 u32LastFilterID, BSP_U32 u32FilterID, IPF_MATCH_INFO_S* pstMatchInfo);

/*****************************************************************************
* 函 数 名     : IPF_FindFilterID
*
* 功能描述  : 在上下行链表中寻找与PS ID 匹配的Filter ID
*
* 输入参数  : IPF_CHANNEL_TYPE_E eChnType    通道类型
*                           BSP_U32 u32PsID                PS ID            
*             
* 输出参数  : BSP_U32* u32FilterID   查询到的Filter ID

* 返 回 值     : IPF_SUCCESS                查询成功
*                          IPF_ERROR                  查询失败
*
* 修改记录  :2011年1月11日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_FindFilterID(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32PsID, BSP_U32* u32FilterID);

/*****************************************************************************
* 函 数 名      : IPF_ConfigCD
*
* 功能描述  : 配置CD
*
* 输入参数  : TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr
* 输出参数  : 无
* 返 回 值      :  无
*
* 修改记录  :2011年1月24日   鲁婷  创建
*****************************************************************************/
BSP_S32 IPF_ConfigCD(TTF_MEM_ST *pstTtf, BSP_U16* pu16TotalDataLen, BSP_U32* pu32BdInPtr);
#endif


#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _BSP_DMAC_H_ */


