/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IPF.h
*
*   作    者 :  luting 00168886
*
*   描    述 :  IPF模块用户接口文件
*
*   修改记录 :  2011年01月21日  v1.00  luting  创建
*************************************************************************/

#ifndef    _BSP_IPF_H_
#define    _BSP_IPF_H_

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */

/**************************************************************************
  头文件包含                            
**************************************************************************/
#ifndef VERSION_V3R2
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#include "product_config.h"
#else
#include "product_config.h"
#endif
#endif

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#include "DrvInterface.h"
#include "TtfDrvInterface.h"
#else
#include "DrvInterface.h"
#endif
/**************************************************************************
  宏定义 
**************************************************************************/
#ifdef __BSP_DEBUG__
#define __BSP_IPF_DEBUG__
#endif


/*************************IPF BEGIN************************/
/**************************************************************************
  宏定义 
**************************************************************************/

/*错误码定义*/
#define IPF_SUCCESS                         BSP_OK
#define IPF_ERROR                           BSP_ERROR
#define BSP_ERR_IPF_INVALID_PARA          BSP_DEF_ERR(BSP_MODU_IPF, BSP_ERR_INVALID_PARA)
#define BSP_ERR_IPF_BDQ_NOT_ENOUGH        BSP_DEF_ERR(BSP_MODU_IPF, 1)
#define BSP_ERR_IPF_CDQ_NOT_ENOUGH        BSP_DEF_ERR(BSP_MODU_IPF, 2)
#define BSP_ERR_IPF_RDQ_EMPTY              BSP_DEF_ERR(BSP_MODU_IPF, 3)
#define BSP_ERR_IPF_FILTER_NOT_ENOUGH     BSP_DEF_ERR(BSP_MODU_IPF, 4) 
#define BSP_ERR_IPF_NOT_INIT               BSP_DEF_ERR(BSP_MODU_IPF, 5) 

#define IPF_ULBD_DESC_SIZE         64
#define IPF_ULRD_DESC_SIZE         64
#define IPF_DLBD_DESC_SIZE         64        
#define IPF_DLRD_DESC_SIZE         64       
#define IPF_DLCD_DESC_SIZE         1024 //上海需求

/**************************************************************************
  枚举定义
**************************************************************************/

/* 过滤模式 */
typedef enum tagIPF_MODE_E
{
	IPF_MODE_FILTERANDTRANS = 0,   /* 过滤并搬移 */
	IPF_MODE_FILTERONLY,           /* 只过滤 */
    IPF_MODE_MAX                    /* 边界值 */
}IPF_MODE_E;

/* BURST最大长度 */
typedef enum tagIPF_BURST_E
{
	IPF_BURST_16 = 0,           /* BURST16 */
	IPF_BURST_8,                /* BURST8 */
    IPF_BURST_4,                /* BURST4 */
    IPF_BURST_MAX                /* BURST4 跟寄存器一致 */
}IPF_BURST_E;

/* IP类型 */
typedef enum tagIPF_IP_TYPE_E
{
    IPF_IPTYPE_V4 = 0,        /* IPV4 */
    IPF_IPTYPE_V6,            /* IPV6 */
    IPF_IPTYPE_MAX           /* 边界值 */
}IPF_IP_TYPE_E;

/* 通道类型 */
typedef enum tagIPF_CHANNEL_TYPE_E
{
    IPF_CHANNEL_UP	= 0,        /* 上行通道 */
    IPF_CHANNEL_DOWN,         /* 下行通道 */ 
    IPF_CHANNEL_MAX           /* 边界值 */
}IPF_CHANNEL_TYPE_E;


/**************************************************************************
  STRUCT定义
**************************************************************************/

/* BD描述符 */
typedef struct tagIPF_BD_DESC_S
{
    BSP_U16 u16Attribute;
    BSP_U16 u16PktLen;
    BSP_U32 u32InPtr;
    BSP_U32 u32OutPtr;
    BSP_U16 u16Resv;
    BSP_U16 u16UsrField1;
    BSP_U32 u32UsrField2;
    BSP_U32 u32UsrField3;
} IPF_BD_DESC_S;

/* RD描述符 */
typedef struct tagIPF_RD_DESC_S
{
    BSP_U16 u16Attribute;
    BSP_U16 u16PktLen;
    BSP_U32 u32InPtr;
    BSP_U32 u32OutPtr;
    BSP_U16 u16Result;
    BSP_U16 u16UsrField1;
    BSP_U32 u32UsrField2;
    BSP_U32 u32UsrField3;
} IPF_RD_DESC_S;

/* CD描述符 */
typedef struct tagIPF_CD_DESC_S
{
    BSP_U16 u16Attribute;
    BSP_U16 u16PktLen;
    BSP_U32 u32Ptr;
}IPF_CD_DESC_S;

/* 通道控制结构体 */
typedef struct tagIPF_CHL_CTRL_S
{
	BSP_BOOL bDataChain;          /* 数据是否为链表 */
	BSP_BOOL bEndian;             /* 大小端指示 */
	IPF_MODE_E eIpfMode;          /* 模式控制 */
    BSP_U32 u32WrrValue;           /* WRR值 */
} IPF_CHL_CTRL_S;

typedef struct tagIPF_CONFIG_ULPARAM_S
{
    BSP_U32 u32Data;
    BSP_U16 u16Len;
    BSP_U16 u16UsrField1;
    BSP_U32 u32UsrField2;
    BSP_U32 u32UsrField3;
}IPF_CONFIG_ULPARAM_S;

/* 统计计数信息结构体 */
typedef struct tagIPF_FILTER_STAT_S
{
    BSP_U32 u32UlCnt0;
    BSP_U32 u32UlCnt1;
    BSP_U32 u32UlCnt2;
    BSP_U32 u32DlCnt0;
    BSP_U32 u32DlCnt1;
    BSP_U32 u32DlCnt2;
}IPF_FILTER_STAT_S;

typedef BSP_S32 (*BSP_IPF_WakeupUlCb)(BSP_VOID);

BSP_S32 BSP_IPF_ConfigTimeout(BSP_U32 u32Timeout);
BSP_S32 BSP_IPF_ConfigUpFilter(BSP_U32 u32Num, IPF_CONFIG_ULPARAM_S* pstUlPara, BSP_BOOL bintFlag);
BSP_U32 BSP_IPF_GetUlBDNum(BSP_VOID);
BSP_S32 BSP_IPF_UlStateIdle(BSP_VOID);
BSP_S32 BSP_IPF_SetPktLen(BSP_U32 u32MaxLen, BSP_U32 u32MinLen);
BSP_S32 BSP_IPF_GetStat(IPF_FILTER_STAT_S *pstFilterStat);
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
	/* 公共参数结构体 */
	typedef struct tagIPF_COMMON_PARA_S
	{
		BSP_BOOL bEspSpiDisable; 	 /* ESP SPI是否参与匹配控制信号 */
		BSP_BOOL bAhSpiDisable;		 /* AH SPI是否参与匹配控制信号 */
		BSP_BOOL bEspAhSel; 		 /* 0选ESP，1选AH */
		BSP_BOOL bIpv6NextHdSel;	
		IPF_BURST_E eMaxBurst;		 /* BURST最大长度 */
		BSP_BOOL bSpWrrModeSel; 	
		BSP_BOOL bSpPriSel; 		 /* SP优先级选择 */
		BSP_BOOL bFltAddrReverse;  /* 扩滤器中地址寄存器大小端排列顺序 0表示大端 */
		BSP_BOOL bFilterSeq;		/* 过滤器配置顺序指示 */
	} IPF_COMMON_PARA_S; 

	/* 匹配项信息 */
	typedef struct tagIPF_MATCH_INFO_S
	{
		BSP_U8 u8SrcAddr[16];
		BSP_U8 u8SrcMsk[16];
		BSP_U8 u8DstAddr[16];
		BSP_U8 u8DstMsk[16];
		union
		{
			struct
			{
				BSP_U32 u16SrcPortLo:16;
				BSP_U32 u16SrcPortHi:16;
			}Bits;
			BSP_U32 u32SrcPort;
		} unSrcPort;
		union
		{
			struct
			{
				BSP_U32 u16DstPortLo:16;
				BSP_U32 u16DstPortHi:16;
			}Bits;
			BSP_U32 u32DstPort;
		} unDstPort;
		union
		{
			BSP_U32 u32TrafficClass;
			BSP_U32 u32ServiceType;
		}unTrafficClass;
		union
		{
			BSP_U32 u32TrafficClassMsk;
			BSP_U32 u32ServiceTypeMsk;
		} unTrafficClassMsk;
		union
		{
			BSP_U32 u32NextHeader;
			BSP_U32 u32Protocol;
		}unNextHeader;
		BSP_U32 u32FlowLable;
		union
		{
			struct
			{
				BSP_U32 u16Type:16;
				BSP_U32 u16Code:16;
			}Bits;
			BSP_U32 u32CodeType;
		} unFltCodeType;
		union
		{
			struct
			{
				BSP_U32 u16NextIndex:16;
				BSP_U32 u16FltPri:16;
			}Bits;
			BSP_U32 u32FltChain;
		} unFltChain;
		BSP_U32 u32FltSpi;
		union
		{
			struct
			{
				BSP_U32 FltEn:1;
				BSP_U32 FltType:1;
				BSP_U32 Resv1:2;
				BSP_U32 FltSpiEn:1;
				BSP_U32 FltCodeEn:1;
				BSP_U32 FltTypeEn:1;
				BSP_U32 FltFlEn:1;			 
				BSP_U32 FltNhEn:1;
				BSP_U32 FltTosEn:1;
				BSP_U32 FltRPortEn:1;
				BSP_U32 FltLPortEn:1;
				BSP_U32 FltRAddrEn:1;
				BSP_U32 FltLAddrEn:1;
				BSP_U32 Resv2:2;			
				BSP_U32 FltBid:6;
				BSP_U32 Resv3:10;			 
			}Bits;
			BSP_U32 u32FltRuleCtrl;
		}unFltRuleCtrl;
	}IPF_MATCH_INFO_S;

	/* Filter配置信息结构体 */
	typedef struct tagIPF_FILTER_CONFIG_S
	{
		BSP_U32 u32FilterID;
		IPF_MATCH_INFO_S stMatchInfo; 
	} IPF_FILTER_CONFIG_S;

    typedef struct tagIPF_CONFIG_DLPARAM_S
	{
		TTF_MEM_ST* pstSrcTtf;
		TTF_MEM_ST* pstDstTtf;
		BSP_U16 u16UsrField; /* 2Byte透传 */
	}IPF_CONFIG_DLPARAM_S;

	typedef BSP_S32 (*BSP_IPF_DlFreeMemCb)(BSP_U32 u32Num, TTF_MEM_ST** pstMem);
    BSP_S32 BSP_IPF_Init(IPF_COMMON_PARA_S *pstCommPara);
    BSP_S32 BSP_IPF_ConfigUlChannel(IPF_CHL_CTRL_S *pstCtrl);
    BSP_S32 BSP_IPF_ConfigDlChannel(IPF_CHL_CTRL_S *pstCtrl);
    BSP_S32 BSP_IPF_SetFilter(IPF_CHANNEL_TYPE_E eChnType, IPF_FILTER_CONFIG_S *pstFilterInfo, BSP_U32 u32FilterNum);
    BSP_S32 BSP_IPF_GetFilter(IPF_CHANNEL_TYPE_E eChnType, BSP_U32 u32FilterID, IPF_FILTER_CONFIG_S *pstFilterInfo);
    BSP_S32 BSP_IPF_ConfigDownFilter(BSP_U32 u32Num, IPF_CONFIG_DLPARAM_S* pstDlPara, BSP_BOOL bintFlag);
    BSP_S32 BSP_IPF_ChannelEnable(IPF_CHANNEL_TYPE_E eChanType, BSP_BOOL bFlag);
    BSP_U32 BSP_IPF_GetDlBDNum(BSP_U32* pu32CDNum);
    BSP_S32 BSP_IPF_DlStateIdle(BSP_VOID);
    BSP_VOID BSP_IPF_ReleaseDlSrcMem (BSP_VOID);
    BSP_VOID BSP_IPF_RegisterDlFreeMemCb(BSP_IPF_DlFreeMemCb pFnDlFreeMem);
    BSP_S32 BSP_IPF_RegisterWakeupUlCb(BSP_IPF_WakeupUlCb pFnWakeupUl);
    BSP_VOID BSP_IPF_GetUlRd(BSP_U32* pu32Num, IPF_RD_DESC_S *pstRd);
	BSP_VOID BSP_IPF_SwitchInt(BSP_BOOL bFlag);
	BSP_S32 BSP_IPF_DrxBakReg();
	BSP_VOID BSP_IPF_DrxRestoreReg();
#else
	typedef BSP_S32 (*BSP_IPF_WakeupDlCb)(BSP_VOID);
    BSP_S32 BSP_IPF_RegisterWakeupDlCb(BSP_IPF_WakeupDlCb pFnWakeupDl);
    BSP_VOID BSP_IPF_GetDlRd(BSP_U32* pu32Num, IPF_RD_DESC_S *pstRd);
	BSP_U32 BSP_IPF_GetUlRdNum(BSP_VOID);
	BSP_U32 BSP_IPF_GetDlRdNum(BSP_VOID);
#endif

/*************************IPF END************************/

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End #define _BSP_IPF_H_ */


