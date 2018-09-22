/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SPI_API.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_SPI_API_H__
#define __BSP_SPI_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/*错误码定义*/
#define BSP_ERR_SPI_MODULE_NOT_INITED BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_SPI_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_INVALID_PARA)
#define BSP_ERR_SPI_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_RETRY_TIMEOUT)

#define BSP_ERR_SPI_SEM_CREATE        BSP_DEF_ERR(BSP_MODU_SPI, 1)
#define BSP_ERR_SPI_SEM_LOCK          BSP_DEF_ERR(BSP_MODU_SPI, 2)
#define BSP_ERR_SPI_SEM_UNLOCK        BSP_DEF_ERR(BSP_MODU_SPI, 3)
#define BSP_ERR_SPI_DATASIZE_INVALID  BSP_DEF_ERR(BSP_MODU_SPI, 4)
#define BSP_ERR_SPI_ATTR_NOTSET       BSP_DEF_ERR(BSP_MODU_SPI, 5)
#define BSP_ERR_SPI_PROT_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 6)
#define BSP_ERR_SPI_TMOD_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 7)
#define BSP_ERR_SPI_RECEIVE_POLL      BSP_DEF_ERR(BSP_MODU_SPI, 8)


/*SPI的ID，ASIC上使用3片SPI*/
typedef enum tagSPI_DEV_ID
{
    SPI_ID0,
    SPI_ID1,
    SPI_ID_MAX
}SPI_DEV_ID_E;


/*每个SPI上的片选号，当前每个SPI有4个片选*/
typedef enum tagSPI_DEV_CS
{
    SPI_DEV_CS0,
    SPI_DEV_CS1,
    SPI_DEV_CS2,
    SPI_DEV_CS3,
    SPI_DEV_CS_MAX,
    NO_OWNER = -1
}SPI_DEV_CS_E;


/*数据传输方式，包括DMA方式和轮循方式*/
typedef enum tagSPI_SENDMOD
{
    SPI_SENDMOD_POLLING,                                                          
    SPI_SENDMOD_DMA,
    SPI_SENDMOD_MAX
}RXTX_MODE_E;


/* 记录SPI的ID和片选号的结构体*/
typedef struct tagSPI_DEV_STRUCTION
{
    SPI_DEV_ID_E enSpiId;
    SPI_DEV_CS_E enSpiCs;
}SPI_DEV_S;

/* 控制命令字长度 */
typedef enum tagspi_CommandLen                                                       
{
    SPI_COMMANDSIZE_1BITS =  0x0,     /*  1 bit command */
    SPI_COMMANDSIZE_2BITS =  0x1,     /*  2 bit command */
    SPI_COMMANDSIZE_3BITS =  0x2,     /*  3 bit command */
    SPI_COMMANDSIZE_4BITS  = 0x3,     /*  4 bit command */                           
    SPI_COMMANDSIZE_5BITS  = 0x4,     /*  5 bit command */                           
    SPI_COMMANDSIZE_6BITS  = 0x5,     /*  6 bit command */                           
    SPI_COMMANDSIZE_7BITS  = 0x6,     /*  7 bit command */                           
    SPI_COMMANDSIZE_8BITS  = 0x7,     /*  8 bit command */                           
    SPI_COMMANDSIZE_9BITS  = 0x8,     /*  9 bit command */                           
    SPI_COMMANDSIZE_10BITS = 0x9,     /* 10 bit command */                           
    SPI_COMMANDSIZE_11BITS = 0xA,     /* 11 bit command */                           
    SPI_COMMANDSIZE_12BITS = 0xB,     /* 12 bit command */                           
    SPI_COMMANDSIZE_13BITS = 0xC,     /* 13 bit command */                           
    SPI_COMMANDSIZE_14BITS = 0xD,     /* 14 bit command */                           
    SPI_COMMANDSIZE_15BITS = 0xE,     /* 15 bit command */                           
    SPI_COMMANDSIZE_16BITS = 0xF,      /* 16 bit command */ 
    SPI_COMMANDSIZE_MAX
} SPI_COMMAND_LEN_E;

/*数据帧长度*/
typedef enum tagspi_DataLen                                                       
{                                                                              
    SPI_DATASIZE_4BITS  = 0x3,     /*  4 bit data */                           
    SPI_DATASIZE_5BITS  = 0x4,     /*  5 bit data */                           
    SPI_DATASIZE_6BITS  = 0x5,     /*  6 bit data */                           
    SPI_DATASIZE_7BITS  = 0x6,     /*  7 bit data */                           
    SPI_DATASIZE_8BITS  = 0x7,     /*  8 bit data */                           
    SPI_DATASIZE_9BITS  = 0x8,     /*  9 bit data */                           
    SPI_DATASIZE_10BITS = 0x9,     /* 10 bit data */                           
    SPI_DATASIZE_11BITS = 0xA,     /* 11 bit data */                           
    SPI_DATASIZE_12BITS = 0xB,     /* 12 bit data */                           
    SPI_DATASIZE_13BITS = 0xC,     /* 13 bit data */                           
    SPI_DATASIZE_14BITS = 0xD,     /* 14 bit data */                           
    SPI_DATASIZE_15BITS = 0xE,     /* 15 bit data */                           
    SPI_DATASIZE_16BITS = 0xF,     /* 16 bit data */
    SPI_DATASIZE_MAX
} SPI_DATA_LEN_E;    

/* SPI支持的协议*/
typedef enum tagSPI_PROT
{
    SPI_PROT_SPI,
    SPI_PROT_SSP,
    SPI_PROT_NSM,
    SPI_PROT_MAX
}SPI_PROT_E;

typedef enum tagSPI_SCPOL
{
	SPI_SCPOL_LOW, //下降沿触发
	SPI_SCPOL_HIGH,//上升沿触发
	SPI_SCPOL_MAX
}SPI_SCPOL_E;

/*SPI外接设备特性结构体，包括设备需要的命令字长度，数据帧长度，使用协议等 */
typedef struct tagSPI_DEV_ATTR
{
    SPI_COMMAND_LEN_E enCommandLen;
    SPI_DATA_LEN_E enDataLen;
    SPI_PROT_E enSpiProt;
	SPI_SCPOL_E enSpiScpol;
    BSP_U16 u16SpiBaud;
}SPI_DEV_ATTR_S;

/*记录与SPI数据传输相关信息的结构体，成员包括要读写的SPI号，片选号，传输模式，
  数据地址，数据长度等*/                                     
typedef struct tagSPI_DATA_HANDLE
{
    SPI_DEV_ID_E enSpiID;
    SPI_DEV_CS_E enCsID;
    RXTX_MODE_E enMode;
    void *pvCmdData;
    BSP_U32 u32length;
}SPI_DATA_HANDLE_S;


/*****************************************************************************
* 函 数 名  : BSP_SPI_SetAttr
*
* 功能描述  : 配置SPI的寄存器，设置控制命令字长度、数据帧长度等。
*
* 输入参数  : enSpiID        需要设置的SPI号，以及根据哪片片选进行配置的片选号。
*             pstSpiDevAttr  记录SPI外接设备特性的结构体指针，结构体成员包括设备
                             需要的命令字长度，数据帧长度，使用协议，波特率等。
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_SetAttr(SPI_DEV_S *enSpiID,SPI_DEV_ATTR_S *pstSpiDevAttr);

/*****************************************************************************
* 函 数 名  : BSP_SPI_Write
*
* 功能描述  : 通过SPI向设备写入数据
*
* 输入参数  : pstWriteData 记录与SPI数据传输有关信息的结构体指针，成员包括
                           要读写的SPI号，片选号，传输模式等
*             pSendData    存储接收的数据缓冲区指针
*             u32Length    待接收的数据长度
*
* 输出参数  : 无
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_Write(SPI_DATA_HANDLE_S *pstWriteData,BSP_VOID *pSendData, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SPI_Read
*
* 功能描述  : 通过SPI读取设备数据
*
* 输入参数  : pstReadData  记录与SPI数据传输有关信息的结构体指针，成员包括
                           要读写的SPI号，片选号，传输模式等。 
*             u32Length    待接收的数据长度
*
* 输出参数  : pRecData     存储接收的数据缓冲区指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_Read(SPI_DATA_HANDLE_S *pstReadData,BSP_VOID *pRecData, BSP_U32 u32Length);

/*****************************************************************************
* 函 数 名  : BSP_SPI_GetAttr
*
* 功能描述  : 获取当前SPI设置的属性
*
* 输入参数  : enSpiId    要查询的SPI号。                        
*            
* 输出参数  : pstDevAttr 存放SPI属性的结构体指针。
*
* 返 回 值  : OK    接收成功
*             ERROR 接收失败
*****************************************************************************/
BSP_S32 BSP_SPI_GetAttr(SPI_DEV_ID_E enSpiId, SPI_DEV_ATTR_S *pstDevAttr);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SPI_API_H__ */


