/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SCI.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_SCI_H__
#define __BSP_SCI_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


typedef void (*USIMMSCIInfo)(void);
typedef BSP_U32 (*Sci_Event_Func)(BSP_U32 u32Event, BSP_VOID* para);

/*错误码*/
#define BSP_ERR_SCI_NOTINIT              951
#define BSP_ERR_SCI_INVALIDFREQ          952
#define BSP_ERR_SCI_USEDEFAULT           953
#define BSP_ERR_SCI_INVALIDPARA          954
#define BSP_ERR_SCI_NOCARD               955
#define BSP_ERR_SCI_NODATA               956
#define BSP_ERR_SCI_NOTREADY             957
#define BSP_ERR_SCI_DISABLED             958
#define BSP_ERR_SCI_UNSUPPORTED          959
#define BSP_ERR_SCI_INSERROR             960 
#define BSP_ERR_SCI_GETAPDU_ERROR        961
#define BSP_ERR_SCI_VLTG_HIGHEST         962
#define BSP_ERR_SCI_CURRENT_STATE_ERR    963
#define BSP_ERR_SCI_CURRENT_VLTG_ERR     964
#define BSP_ERR_SCI_TIMESTOP_TIMEOUT     965


/*回调函数注册事件类型*/
#define SCI_EVENT_CARD_IN                0x1
#define SCI_EVENT_CARD_OUT               0x2
#define SCI_EVENT_CARD_DEACTIVE_SUCCESS  0x4
#define SCI_EVENT_CARD_RESET_SUCCESS     0x8
#define SCI_EVENT_CARD_READ_DATA         0x10
#define SCI_EVENT_CARD_TX_ERR            0x20

#define SCI_DEF_CARD_OK     0x0 /* 卡正常，可以使用*/
#define SCI_DEF_CARD_ERROR  0x1   /* 无卡或者卡异常*/


typedef enum tagSCI_CLK_STOP_TYPE_E
{
    SCI_CLK_STATE_LOW = 0,        /* 协议侧指示停低*/
    SCI_CLK_STATE_HIGH = 1,       /*协议侧指示停高*/
    SCI_CLK_STATE_UNSUPPORTED = 2,/*协议侧指示不支持*/
    SCI_CLK_STATE_BUTT
}SCI_CLK_STOP_TYPE_E;


/*****************************************************************************
* 函 数 名  : BSP_SCI_SendData
*
* 功能描述  : 本接口用于发送一段数据到USIM卡
*
* 输入参数  : BSP_U32 u32DataLength 发送数据的有效长度。取值范围1～256，单位为字节  
*             BSP_U8 *pu8DataBuffer 发送数据所在内存的首地址，如是动态分配，调用接口
*                                   后不能立即释放，依赖于硬件发完数据
* 输出参数  : 无
*
* 返 回 值  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_SendData(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : BSP_SCI_ReceiveSync
*
* 功能描述  : 本接口用于USIM Manager读取来自SCI Driver的卡返回数据
*             该接口为阻塞接口，只有SCI接收到足够的数据量后才会返回；
*             该接口的超时门限为1s
*
* 输入参数  : BSP_U32 u32DataLength USIM Manager欲从SCI Driver读取的数据长度。
* 输出参数  : BSP_U8 *pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ReceiveSync(BSP_U32 u32Length,BSP_U8 *pu8Data);

/*****************************************************************************
* 函 数 名  : BSP_SCI_ReceiveAll
*
* 功能描述  : 本接口用于USIM Manager在读数超时的时候，调用本函数，读取接收数据缓冲中的所有数据
*
* 输入参数  : 无  
* 输出参数  : BSP_U32 *u32DataLength Driver读取的数据长度，返回给USIM Manager。取值范围1～256，单位是字节
*           BSP_U8 * pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ReceiveAll(BSP_U32 *u32DataLength,BSP_U8 * pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetATR
*
* 功能描述  : 本接口用于将Driver层缓存的ATR数据和数据个数返回给USIM Manager层
*
* 输入参数  : BSP_VOID  
* 输出参数  : BSP_U8 *u8DataLength  Driver读取的ATR数据长度，返回给USIM Manager。
*                                   取值范围0～32，单位是字节
*           BSP_U8 *pu8ATR          USIM Manager指定的Buffer，SCI Driver将ATR
*                                   数据拷贝到本Buffer。一般为操作系统函数动态分配
*                                   或者静态分配的地址
* 
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetATR (BSP_U8 *u8DataLength, BSP_U8 *pu8ATR);

/*****************************************************************************
* 函 数 名  : BSP_SCI_ClassSwitch
*
* 功能描述  : 本接口用于支持PS对卡的电压类型进行切换，从1.8V切换到3V
*
* 输入参数  : BSP_VOID  
* 输出参数  : 无
*
* 返 回 值  :  OK    当前电压不是最高的，进行电压切换操作
*           BSP_ERR_SCI_CURRENT_STATE_ERR 切换失败 current SCI driver state is ready/rx/tx 
*           BSP_ERR_SCI_VLTG_HIGHEST   当前电压已经是最高电压，没有进行电压切换
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR 当前电压值异常（非class B或者C）
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ClassSwitch(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_SCI_TimeStop
*
* 功能描述  : 本接口用于支持PS关闭SIM卡时钟
*
* 输入参数  : 
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg 时钟停止模式
*   
* 输出参数  : 无
*
* 返 回 值  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 BSP_SCI_TimeStop(SCI_CLK_STOP_TYPE_E enTimeStopCfg);


/*****************************************************************************
* 函 数 名  : BSP_SCI_RegEvent
*
* 功能描述  : 本接口用于注册回调函数
*
* 输入参数  : BSP_U32 u32Event      事件类型，用掩码区分：
*                                    0x1：插卡；
*                                    0x2：拔卡；
*                                    0x4：卡下电；
*                                    0x8：卡上电；
*                                    0x10：收数据；
*                                    0x20：发送数据错误
*              Sci_Event_Func * pFunc   回调函数
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*          BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_RegEvent(BSP_U32 u32Event, Sci_Event_Func pFunc);


/*****************************************************************************
* 函 数 名  : BSP_SCI_RecordDataSave
*
* 功能描述  : 本接口用于保存可维可测信息到yaffs中
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*             其他   操作失败
*
* 修改记录  : 2011年5月21日   yangzhi  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_RecordDataSave(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SCI_H__ */


