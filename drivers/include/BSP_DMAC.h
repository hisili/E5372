/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DMAC.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_DMAC_H__
#define __BSP_DMAC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/**************************************************************************
  全局变量声明
**************************************************************************/

/**************************************************************************
  枚举定义
**************************************************************************/
/*DMA transport type*/
typedef enum tagDMA_TRANS_TYPE_E
{
    MEM2MEM_DMA = 0,    /* 内存到内存，DMA流控*/
    MEM2PRF_DMA,        /* 内存到外设，DMA流控*/
    PRF2MEM_DMA,        /* 外设到内存，DMA流控*/
    PRF2PRF_DMA,        /* 外设到外设，DMA流控*/
    PRF2MEM_PRF,        /* 外设到内存，外设流控*/
    PRF2PRF_SRCPRF = 5, /* 外设到外设，源外设流控*/
    MEM2PRF_PRF,        /* 内存到外设，外设流控*/
    PRF2PRF_DSTPRF,     /* 外设到外设，目的外设流控*/
    CTL_TT_FC_BUTTOM    /* 最大值，判断用*/
} DMA_TRANS_TYPE_E;

/**************************************************************************
  宏定义 
**************************************************************************/

/*错误码定义*/
#define EDMA_SUCCESS                    BSP_OK
#define DMAC_SUCCESS                    BSP_OK
#define DMAC_FAIL                       BSP_ERROR


#define BSP_ERR_DMA_MODULE_NOT_INITED   BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_DMA_NULL_PTR            BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_NULL_PTR)
#define BSP_ERR_DMA_INVALID_PARA        BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_INVALID_PARA)

#define BSP_ERR_DMA_CHANNEL_BUSY        BSP_DEF_ERR(BSP_MODU_DMAC, 1)
#define BSP_ERR_DMA_ALL_CHANNEL_BUSY    BSP_DEF_ERR(BSP_MODU_DMAC, 2)
#define BSP_ERR_DMA_STOP_FAIL           BSP_DEF_ERR(BSP_MODU_DMAC, 3)

/**************************************************************************
  枚举定义
**************************************************************************/

/* 通道逻辑状态*/
typedef enum tagDMA_CHN_USR_STATUS_E
{
    DMA_CHN_IDLE = 0, /* 通道空闲*/
    DMA_CHN_ALLOCED = 1 /* 通道已经被申请*/
}DMA_CHN_USR_STATUS_E;

/* 通道属性*/
typedef enum tagDMA_CHN_TYPE_E
{
    DMA_FIXED_MODE = 0,   /* 固定通道，使用完成之后不会自动释放，但是可以通过释放通道接口释放*/
    DMA_COMMON_MODE = 1,   /* 普通通道，使用完成之后会自动释放，也可以通过释放通道接口释放*/
    DMA_SPECIAL_MODE = 2   /* 
特殊通道，使用完成之后不会自动释放，通道释放接口也无法释放，且该通道需要某些特殊处理，目前只有协议栈使用*/
}DMA_CHN_TYPE_E;

/* 通道状态 */
typedef enum tagDMA_CHN_STATUS_E
{
    DMA_CHN_FREE = 0,   /* 通道空闲 */
    DMA_CHN_BUSY = 1   /* 通道忙 */
}DMA_CHN_STATUS_E;



/* DESCRIPTION
 * This data type is used for selecting the transfer width for the
 *  source and/or destination on a DMA channel when using the specified
 *  driver API functions. This data type maps directly to the AMBA AHB
 *  HSIZE parameter.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit field(s): (x = channel number)
 *    - CTLx.SRC_TR_WIDTH, CTLx.DST_TR_WIDTH
 */

typedef enum tagDMA_TRANS_WIDTH_E {
    DMA_TRANS_WIDTH_8   = 0x0,/* 8bit位宽*/
    DMA_TRANS_WIDTH_16  = 0x1,/* 16bit位宽*/
    DMA_TRANS_WIDTH_32  = 0x2,/* 32bit位宽*/
    DMA_TRANS_WIDTH_64  = 0x3,/* 64bit位宽*/
    DMA_TRANS_WIDTH_128 = 0x4,/* 128bit位宽*/
    DMA_TRANS_WIDTH_256 = 0x5 /* 256bit位宽*/
}DMA_TRANS_WIDTH_E;

/* DESCRIPTION
 *  This data type is used for selecting the burst transfer length
 *  on the source and/or destination of a DMA channel when using the
 *  specified driver API functions. These transfer length values do
 *  not relate to the AMBA HBURST parameter.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit field(s): (x = channel number)
 *    - CTLx.SRC_MSIZE, CTLx.DEST_MSIZE
 */
typedef enum tagDMA_BURST_TRANS_LENGTH_E
{
    DMA_MSIZE_1   = 0x0,/* burst长度，即一次传输的个数为1个*/
    DMA_MSIZE_4   = 0x1,/* burst长度，即一次传输的个数为4个*/
    DMA_MSIZE_8   = 0x2,/* burst长度，即一次传输的个数为8个*/
    DMA_MSIZE_16  = 0x3,/* burst长度，即一次传输的个数为16个*/
    DMA_MSIZE_32  = 0x4,/* burst长度，即一次传输的个数为32个*/
    DMA_MSIZE_64  = 0x5,/* burst长度，即一次传输的个数为64个*/
    DMA_MSIZE_128 = 0x6,/* burst长度，即一次传输的个数为128个*/
    DMA_MSIZE_256 = 0x7 /* burst长度，即一次传输的个数为256个*/
}DMA_BURST_TRANS_LENGTH_E;

/* DMA 硬件握手接口*/
typedef enum tagDMA_HS_HK_INTERFACE_E 
{
    DMA_SPI0_RX = 0x0,
    DMA_SPI0_TX = 0x1,
    DMA_SPI1_RX = 0x2,
    DMA_SPI1_TX = 0x3,
    DMA_SIO_RX = 0x4,
    DMA_SIO_TX = 0x5,
	DMA_SCI0_RX = 0x6,
    DMA_SCI0_TX = 0x7,
    DMA_HSUART_RX = 0x8,
    DMA_HSUART_TX = 0x9,
    DMA_UART0_RX = 0xa,
    DMA_UART0_TX = 0xb,
    DMA_UART1_RX = 0xc,
    DMA_UART1_TX = 0xd,
    DMA_SCI1_RX = 0xe,
    DMA_SCI1_TX = 0xf,
    DMA_HK_BUTT
}DMA_HS_HK_INTERFACE_E;

/**************************************************************************
  STRUCT定义
**************************************************************************/
/*链表传输参数结构体*/
typedef struct tagDMA_LLI_S
{
    BSP_U32 u32Reserved[2]; /* reserved 8 bytes  PS要求格式，使用时保留即可*/
    struct  tagDMA_LLI_S *pNextLLI; /* next node */
    BSP_U32 TransLength;  /* transfer length */
    BSP_U32 SrcAddr; /*source address*/    
    BSP_U32 DstAddr; /*destination address，PS可以不关注这个参数*/
} DMA_LLI_S;

/*底软需要构建的符合dma硬件要求的数据结构*/
typedef struct tagDMA_LLI_PHY_S
{
    BSP_U32 SrcAddr; /*source address*/
    BSP_U32 DstAddr; /*destination address*/
    BSP_U32 NextLLI; /*pointer to next LLI  the pSrcAddr of next node*/
    BSP_U32 ulTransferCtrl_l; /*control word low 32 bits */
    BSP_U32 ulTransferCtrl_h; /*control word high 32 bits */
    BSP_U32 s_stat; /*source state*/
    BSP_U32 d_stat; /*dest state*/
} DMA_LLI_PHY_S;

/*传输通道参数结构*/
typedef struct tagDMA_CHN_PARA_S
{
    BSP_U32 u32Chan; /*通道号*/
    BSP_VOID * pfuncDmaTcIsr;/* 完成中断回调函数*/
    BSP_S32 s32DmaTcPara; /* 完成中断回调函数参数*/
    BSP_VOID * pfuncDmaErrIsr;/* 错误中断回调函数*/
    BSP_S32 s32DmaErrPara; /* 错误中断回调函数参数*/
} DMA_CHN_PARA_S;

/*sturcture for single block*/
typedef struct tagDMA_SINGLE_BLOCK_S
{
    DMA_CHN_PARA_S stChnPara;/*传输通道参数结构*/
    DMA_TRANS_TYPE_E enTransType; /*传输流控类型*/
    DMA_HS_HK_INTERFACE_E enSrcHSHKInterface;/* 硬件握手接口*/
    DMA_HS_HK_INTERFACE_E enDstHSHKInterface;/* 硬件握手接口*/
    DMA_TRANS_WIDTH_E enSrcWidth; /*源位宽*/
    DMA_TRANS_WIDTH_E enDstWidth; /*目的位宽 */
    DMA_BURST_TRANS_LENGTH_E enSrcBurstLength; /*源burst长度 */
    DMA_BURST_TRANS_LENGTH_E enDstBurstLength; /*目的burst长度*/
    BSP_U32 u32SrcAddr;/* 源地址*/
    BSP_U32 u32DstAddr;/* 目的地址*/
    BSP_U32 u32TransLength;/* 传输总长度*/
} DMA_SINGLE_BLOCK_S;

/*ADD by w00169995*/
typedef struct tagAXI_DMA_TASK_HANDLE_S
{
	BSP_U32 u32SrcAddr;
	BSP_U32 u32DstAddr;
	BSP_U32 ulLength;
	BSP_U32 ulChIdx;
}AXI_DMA_TASK_HANDLE_S;


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
* 函 数 名  : BSP_DMA_Init
*
* 功能描述  : DMA初始化
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2009年1月20日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_Init(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DMA_GetIdleChannel
*
* 功能描述  : 获取一个当前处于空闲状态的DMA通道，当前块或者链表传输完数据后会
*             自动释放，下次再使用时需再获取当前空闲通道,目前DMA_SPECIAL_MODE
*             仅供协议栈使用，其他用户申请不到此种通道
*
* 输入参数  : DMA_CHN_TYPE_E enChnType 通道类型
*           
* 输出参数  : BSP_U32 *pulChan      存放申请到的通道号
* 返 回 值  :  DMAC_SUCCESS       安装成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*             BSP_ERR_DMA_ALL_CHANNEL_BUSY 所有通道忙
*
* 修改记录  :2009年10月15日   liumengcun  创建

*****************************************************************************/
BSP_S32  BSP_DMA_GetIdleChannel(DMA_CHN_TYPE_E enChnType, BSP_U32 *pulChan);

/*****************************************************************************
* 函 数 名  : BSP_DMA_ChannelRelease
*
* 功能描述  : DMA传输通道释放,正常情况下不需要调用此接口释放通道，传送完成后
*             底软会自动释放通道，本接口为了处理某些异常情况下，取得了通道但
*             是却没有开始数据传输，无法自动释放通道的时候调用此接口手动释放通道
*             通道正在传输时无法释放通道，通道0给协议栈固定使用无法释放。
*
* 输入参数  : BSP_U32 ulChan  通道号
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       释放成功
*             DMA_NOT_INIT      DMA未初始化
*             DMA_PARA_INVALID  参数错误
*             DMA_CHANNEL_BUSY  通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_U32  BSP_DMA_ChannelRelease(BSP_U32 u32Chan);

/*****************************************************************************
* 函 数 名  : BSP_DMA_AllChannelCheck
*
* 功能描述  : DMA全部通道是否空闲检测函数
*
* 输入参数  : 无
* 输出参数  : pChannelInfo   所有通道使用状态，用bit掩码表示，正在传输数据的通道bit位置1，
*               bit0对应0通道，bit1对应1通道，以此类推
* 返 回 值  : DMA_ERROR
*             DMAC_SUCCESS
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32  BSP_DMA_AllChannelCheck(BSP_U32 *pChannelInfo);

/*****************************************************************************
* 函 数 名  : BSP_DMA_OneChannelCheck
*
* 功能描述  : DMA单个通道具体状态检查
*
* 输入参数  : BSP_U32 ulChan   待检查的通道号
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS    该通道传输完成
*          DMA_TRXFER_ERROR   该通道传输错误
*          DMA_NOT_FINISHED   该通道传输未完成
*          DMA_ERROR    未知状态
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_OneChannelCheck(BSP_U32 ulChan, BSP_U32 *pChannelStatus);

/*****************************************************************************
* 函 数 名  : BSP_DMA_SingleBlockStart
*
* 功能描述  : 单块数据传输启动
*
* 输入参数  : DMA_SINGLE_BLOCK_S stDMASingleBlock   单块传输使用的参数结构体
*
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS    传输启动完成
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_TRXFERSIZE_INVALID   传入传输长度非法
*          DMA_CHANNEL_BUSY         通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock);

/*****************************************************************************
* 函 数 名  : BSP_DMA_SetDstAddr
*
* 功能描述  : 底层提供给协议栈的配置目的地址函数，协议栈目的地址固定不会变化
*
* 输入参数  : BSP_U32 u32Addr  待传输的目的地址
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS  设置成功
*
* 修改记录  :2009年7月24日   吴振  创建

*****************************************************************************/
BSP_U32 BSP_DMA_SetDstAddr(BSP_U32 u32Addr);

/*****************************************************************************
* 函 数 名  : BSP_DMA_LliBuild
*
* 功能描述  : 链表构建函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             DMA_LLI_S *pFirstLLI  资源链表的第一个节点指针
* 输出参数  : 无
* 返 回 值  : BSP_VOID *  指向底层建立好的传输链表的头节点的指针
*
* 修改记录  :2009年7月24日   吴振  创建

*****************************************************************************/
BSP_VOID * BSP_DMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI);

/*****************************************************************************
* 函 数 名  : BSP_DMA_LliStart
*
* 功能描述  : 链表数据传输启动专用函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             BSP_VOID *pu32Head：   调用BSP_DMA_LliBuild获得的待传输链表头指针
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS    传输启动完成
*          DMA_LLIHEAD_ERROR   传入的链表头错误
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_ERROR    通道不空闲或者DMA未正确初始化
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_U32 BSP_DMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head);

/*****************************************************************************
* 函 数 名  : BSP_DMA_GetDar
*
* 功能描述  : 外设使用该函数获取DMA的DAR寄存器的值
*
* 输入参数  : BSP_U32 u32Chan   通道号
*
* 输出参数  : BSP_U32 *u32Dar  该通道对应的目的地址寄存器的值
* 返 回 值  : DMAC_SUCCESS       获取成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2010年12月1日   鲁婷  创建

*****************************************************************************/
BSP_S32 BSP_DMA_GetDar(BSP_U32 u32Chan, BSP_U32 *u32Dar);

/*****************************************************************************
* 函 数 名  : BSP_DMA_DisableChannel
*
* 功能描述  : 外设使用该函数去使能DMAC通道
*
* 输入参数  : BSP_U32 u32Chan   通道号
*
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       获取成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2010年12月1日   鲁婷  创建

*****************************************************************************/
BSP_S32 BSP_DMA_DisableChannel(BSP_U32 u32Chan);

BSP_S32  BSP_AXIDMAC_DrxBakReg(BSP_VOID);
BSP_VOID BSP_AXIDMAC_DrxRestoreReg(BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_EDMA_CheckChannelBusy
*
* 功能描述  : 判断通道是否空闲
*
* 输入参数  : u32Channel：待判断的通道号
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       通道空闲
*             BSP_ERR_DMA_CHANNEL_BUSY  通道忙
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2009年06月20日   吴振  创建

*****************************************************************************/
extern BSP_S32 BSP_EDMA_CheckChannelBusy(BSP_U32 u32Channel);
#define DRV_EDMA_CHECK_CHANNEL_BUSY(u32Channel) BSP_EDMA_CheckChannelBusy(u32Channel)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_SingleBlockStart
*
* 功能描述  : 单块数据传输启动
*
* 输入参数  : DMA_SINGLE_BLOCK_S stDMASingleBlock   单块传输使用的参数结构体
*
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS    传输启动完成
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_TRXFERSIZE_INVALID   传入传输长度非法
*          DMA_CHANNEL_BUSY         通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
extern BSP_S32 BSP_EDMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock);
#define DRV_EDMA_SINGLE_BLOCK_START(stDMASingleBlock) BSP_EDMA_SingleBlockStart(stDMASingleBlock)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_LliBuild
*
* 功能描述  : 链表构建函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             DMA_LLI_S *pFirstLLI  资源链表的第一个节点指针
* 输出参数  : 无
* 返 回 值  : BSP_VOID *  指向底层建立好的传输链表的头节点的指针
*
* 修改记录  :2009年7月24日   吴振  创建

*****************************************************************************/
extern BSP_VOID * BSP_EDMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI);
#define DRV_EDMA_LLI_BUILD(ulChan,pFirstLLI) BSP_EDMA_LliBuild(ulChan,pFirstLLI)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_LliStart
*
* 功能描述  : 链表数据传输启动专用函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             BSP_VOID *pu32Head：   调用BSP_DMA_LliBuild获得的待传输链表头指针
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS    传输启动完成
*          DMA_LLIHEAD_ERROR   传入的链表头错误
*          DMA_CHANNEL_INVALID   传入通道号错误
*          DMA_ERROR    通道不空闲或者DMA未正确初始化
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
extern BSP_U32 BSP_EDMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head);
#define DRV_EDMA_LLI_START(stChnPara,pu32Head) BSP_EDMA_LliStart(stChnPara,pu32Head)

/*****************************************************************************
* 函 数 名  : BSP_EDMA_GetRegistValue
*
* 功能描述  : 获取异常时候的EDMAC模块寄存器的值
*
* 输入参数  : BSP_VOID
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
extern BSP_VOID BSP_EDMA_GetRegistValue(BSP_VOID);
#define DRV_EDMA_GET_REGIST_VALUE() BSP_EDMA_GetRegistValue()


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_DMAC_H__ */


