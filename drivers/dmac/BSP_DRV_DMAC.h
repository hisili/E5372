/******************************************************************
* Copyright (C), 2005-2007, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: dmaDrv.h                                          *
*                                                                 *
* Description:                                                    *
*      DMA controller driver header file                          *
*                                                                 *
* Author:                                                         *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*                                                                 *
* Date:                                                           *
*                                                                 *
* History:                                                        *
*                                                                 *
1.date:2008-07-20
 question number:AT2D04493
 modify by: k60638
 modify reasion:modify pclint warnings
*******************************************************************/


#ifndef    _BSP_DMA_DRV_H_
#define    _BSP_DMA_DRV_H_

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#ifdef __VXWORKS__
#include "Drvinterface.h"
#define DMAC_PRINT          printf
#else
#include "BSP.h"
#define DMAC_PRINT          printk
#endif

#define DMAC_SUCCESS        BSP_OK
/**************************************************************************
  宏定义
**************************************************************************/
#define DMA_VERSION                0        /* version number increase from 0 */
#define DMA_MEM_ALIGN              32   /* For aligining to 4 words. */
#define DMA_MAX_PERIPHERALS        32

#define DMA_MAX_CHANNELS         4
#define DMA_CHANNELS_M           3 

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#ifdef DMAC_TEST_DSP
#define  DMA_SPECIAL_MODE_NUM   1
#else
#define  DMA_SPECIAL_MODE_NUM   0
#endif
#define DMA_MIN_Index         2
#define DMA_MAX_Index         DMA_CHANNELS_M
#else
#define  DMA_SPECIAL_MODE_NUM   0
#define DMA_MIN_Index         DMA_CHANNELS_M
#define DMA_MAX_Index         DMA_MAX_CHANNELS
#endif

#define DMA_INT_CLEAR            0xfF
#define DMA_INT_MASK_WE          0xff00
      

#define DMA_INT_UNMASK           0x0f
#define DMA_ERRINT_UNMASK        0x0f
#define DMA_INT_MASK             0
#define DMA_CHN_EN               0x0f

#define DMA_MAX_SINGLE_BLOCK_TRANS_LENGTH  0xfff
#define DMA_SINGLE_BLOCK_TRANS_LENGTH  4080

#define DMA_NODE_NUM_NORMAL       258

#define DMA_CHANNEL_ENABLE         1
#define DMA_CHANNEL_DISABLE        0

/* 定义模块是否已经初始化 */
#define DMA_NOTINITIALIZE  0
#define DMA_INITIALIZED    1

#define BSP_INT_VEC_DMA (45)

/*define the address for the DMA control and status register*/
#define DMA_CxDISABLE                      0x00
#define DMA_CxENABLE                       0x01


/* 定义各个寄存器的偏移 */
#define DMA_SAR(i)   (0x0 + i * 0x58)        /* 通道源地址寄存器 */
#define DMA_DAR(i)    (0x8 + i * 0x58)        /* 通道目的地址寄存器 */
#define DMA_LLP(i)   (0x10 + i * 0x58)        /* 通道链表指针寄存器 */
#define DMA_CTL(i)   (0x18 + i * 0x58)        /* 通道控制寄存器 */
#define DMA_SSTAT(i)   (0x20 + i * 0x58)        /* 通道源状态寄存器 */
#define DMA_DSTAT(i)   (0x28 + i * 0x58)        /* 通道目的状态寄存器 */
#define DMA_SSTATAR(i)   (0x30 + i * 0x58)       /* 通道源状态地址寄存器 */
#define DMA_DSTATAR(i)   (0x38 + i * 0x58)       /* 通道目的状态地址寄存器 */
#define DMA_CFG(i)    (0x40 + i * 0x58)        /* 通道配置寄存器 */
#define DMA_SGR(i)    (0x48 + i * 0x58)        /* 通道源聚集寄存器 */
#define DMA_DSR(i)    (0x50 + i * 0x58)        /* 通道目的分散寄存器 */
#define DMA_RAWTFR    0x2c0        /* intTfr中断原始状态寄存器 */
#define DMA_RAWBLOCK    0x2c8        /* intBlock中断原始状态寄存器 */
#define DMA_RAWSRCTRAN   0x2d0        /* intSrcTran中断原始状态寄存器 */
#define DMA_RAWDSTTRAN    0x2d8        /* intDstTran中断原始状态寄存器 */
#define DMA_RAWERR    0x2e0        /* intErr中断原始状态寄存器 */
#define DMA_STATUSTFR    0x2e8        /* intTfr中断状态寄存器 */
#define DMA_STATUSBLOCK    0x2f0        /* intBlock中断状态寄存器 */
#define DMA_STATUSSCRTRAN   0x2f8        /* intSrcTran中断状态寄存器 */
#define DMA_STATUSDSTTRAN    0x300        /* intDstTran中断状态寄存器 */
#define DMA_STATUSERR    0x308        /* intErr中断状态寄存器 */
#define DMA_MASKTFR    0x310        /* intTfr中断屏蔽寄存器 */
#define DMA_MASKBLOCK    0x318        /* intBlock中断屏蔽寄存器 */
#define DMA_MASKSRCTRAN    0x320        /* intSrcTran中断屏蔽寄存器 */
#define DMA_MASKDSTTRAN    0x328        /* intDstTran中断屏蔽寄存器 */
#define DMA_MASKERR    0x330        /* intErr中断屏蔽寄存器 */
#define DMA_CLEARTFR    0x338        /* intTfr中断清除寄存器 */
#define DMA_CLEARBLOCK    0x340        /* intBlock中断清除寄存器 */
#define DMA_CLEARSRCTRAN    0x348        /* intSrcTran中断清除寄存器 */
#define DMA_CLEARDSTTRAN   0x350        /* intDstTran中断清除寄存器 */
#define DMA_CLEARERR    0x358        /* intErr中断清除寄存器 */
#define DMA_STATUSINT    0x360        /* 组合中断状态寄存器 */
#define DMA_REQSRCREG    0x368        /* 源软件传输请求寄存器 */
#define DMA_REQDSTREG    0x370        /* 目的软件传输请求寄存器 */
#define DMA_SGLREQSRCREG    0x378        /* 源软件单次传输请求寄存器 */
#define DMA_SGLREGDSTREG    0x380        /* 目的软件单次传输请求寄存器 */
#define DMA_LSTSRCREG    0x388        /* 最后一次源软件传输请求寄存器 */
#define DMA_LSTDSTREG    0x390        /* 最后一次目的软件传输请求寄存器 */
#define DMA_DMACFGREG    0x398        /* DMA配置寄存器 */
#define DMA_CHENREG    0x3a0        /* DMA通道使能寄存器 */
#define DMA_DMAIDREG    0x3a8        /* DMA  ID 寄存器 */
#define DMA_DMATESTREG    0x3b0        /* DMA  测试寄存器 */
#define DMA_DMAVERREG    0x3f8        /* DMA  元件版本寄存器 */

#define DMA_INT_ADDRESS_SPAN      0x8
#define DMA_RAW_INT_BASE     DMA_RAWTFR
#define DMA_STATUS_INT_BASE    DMA_STATUSTFR
#define DMA_MASK_INT_BASE     DMA_MASKTFR
#define DMA_CLEAR_INT_BASE     DMA_CLEARTFR

#define DMA_ENABLE   1
#define DMA_DISABLE   0

/* 定义电平值 */
#define DMA_HIGH_LEVEL   1
#define DMA_LOW_LEVEL   0

/* 定义默认的CTL及CFG的值 */
#define DMA_LLI_ENABLE   0x18000000
#define DMA_CTL_H_DEFAULT  0x0
#define DMA_CTL_L_DEFAULT  0x1

#define DMA_CFG_H_DEFAULT  0x0
#define DMA_CFG_L_DEFAULT  0x0

/* 单块传输时一次就可以传完*/
#define DMA_SINGLE_TRANS 0
/* 单块传输时如果传输大小大于单块允许的最大长度，则需要分多次传输*/
#define DMA_MUTI_TRANS   1
#define DMA_BURST_LEN16 0x10

/**************************************************************************
  全局变量声明
**************************************************************************/


/**************************************************************************
  枚举定义
**************************************************************************/
/* 中断类型 */
typedef enum tagDMA_INT_TYPE_NUM_E
{
    INT_TFR = 0,
    INT_BLOCK,
    INT_SRC_TRAN,
    INT_DST_TRAN,
    INT_ERR,
    INT_BUTTOM_BIT
}DMA_INT_TYPE_NUM_E;

 /* DESCRIPTION
 *  This data type is used for selecting the address increment
 *  type for the source and/or destination on a DMA channel when using
 *  the specified driver API functions.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit-field(s): (x = channel number)
 *    - CTLx.SINC, CTLx.DINC
 * SEE ALSO
 *  dw_dmac_setAddressInc(), dw_dmac_getAddressInc()
 * SOURCE
 */
typedef enum tagDMA_ADDR_INCREMENT_E
{
    DMA_ADDR_INCREMENT = 0x0, /* 地址递增*/
    DMA_ADDR_DECREMENT = 0x1, /* 地址递减*/
    DMA_ADDR_NOCHANGE  = 0x2  /* 地址固定不变*/
}DMA_ADDR_INCREMENT_E;

/* 协议栈乒乓地址使用情况枚举*/
typedef enum tagDMA_ADDR_USE_STATUS_E
{
    DMA_NO_ADDR_INUSE = 0x0,    /* 没有地址在用*/
    DMA_FIRST_ADDR_INUSE = 0x1, /* 第一个地址在用*/
    DMA_SECOND_ADDR_INUSE  = 0x2/* 第二个地址再用*/
}DMA_ADDR_USE_STATUS_E;

/* DMA master选择*/
typedef enum tagDMA_MASTER_E
{
    DMA_MASTER1 = 0x0,
    DMA_MASTER2 = 0x1,
    DMA_MASTER3 = 0x2,
    DMA_MASTER4 = 0x3
}DMA_MASTER_E;

/* DMA 握手方式选择*/
typedef enum tagDMA_HS_SEL_E
{
    DMA_HW_HANDSHAKE = 0x0,
    DMA_SW_HANDSHAKE = 0x1
}DMA_HS_SEL_E;

/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef BSP_S32 (*DMAC_FUNCPTR)(BSP_S32);

/* 通道信息结构体*/
typedef struct tagDMA_CHN_INFO_S
{
    DMAC_FUNCPTR pfuncDmaTcIsr; /* 完成中断注册函数*/
    BSP_S32 s32DmaTcPara; /* 完成中断注册函数参数*/
    DMAC_FUNCPTR pfuncDmaErrIsr; /* 错误中断注册函数*/
    BSP_S32 s32DmaErrPara; /* 错误中断注册函数参数*/
    BSP_U32 u32ChnUsrStatus; /* 自己维护的channel逻辑状态 */
    BSP_U32 u32ChnAttribute;/* 通道属性*/
    BSP_U32 *pBlockToLLIHead; /* 超大单块拆分成链表时使用的头节点*/
} DMA_CHN_INFO_S;

/* 通道信息结构体*/
typedef struct tagDMA_BUILD_LLI_FOR_BLOCK_S
{
    BSP_U32 u32NodeNum;
    BSP_U32 u32OneTimeTransLength;
    BSP_U32 u32CtlSINC; /* 源地址操作模式，即递增递减还是不变*/
    BSP_U32 u32CtlDINC; /* 目的地址操作模式，即递增递减还是不变*/
    BSP_U32 u32CtlSMS; /* 源设备Master 选择位*/
    BSP_U32 u32CtlDMS; /* 目的设备Master 选择位*/
    BSP_U32 u32SrcBurstLength; /*源burst长度 */
    BSP_U32 u32DstBurstLength; /*目的burst长度*/
} DMA_BUILD_LLI_FOR_BLOCK_S;

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
* 函 数 名  : DMAC_GetRegistValue
*
* 功能描述  : 获取DMAC模块寄存器的值
*
* 输入参数  : BSP_VOID
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
BSP_VOID DMAC_GetRegistValue(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DMAC_GetChannelInfo
*
* 功能描述  : 获取DMAC模块通道信息
*
* 输入参数  : BSP_U32
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
BSP_VOID DMAC_GetChannelInfo(BSP_U32 u32Chan);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of _BSP_DMA_DRV_H_ */

