/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  rfilesystem_logunit.h
*
*
*   描    述 :  用于rfilesystem 存储log,实现可维可测功能的头文件。
*
*************************************************************************/
#ifndef _RFILESYSTEM_LOGUNIT_H_
#define _RFILESYSTEM_LOGUNIT_H_

#include "product_config.h"
#include "BSP.h"
//#include "typedefs.h"
//#include "drvinterface.h"
//#include <stdio.h>
#if defined (FEATURE_FLASH_LESS)
#define FLASHLESS_RFILE_LOG
#endif

#ifdef FLASHLESS_RFILE_LOG
/*************************************************************************************************
宏定义、数据结构定义
*****************************************************************************************************/
#define FLASHLESS_LOG_FILENAME_LEN              15          /*只保存所访问文件名字的15个字符*/

#define RFILE_OPER_RESULT_FAIL                      (-1)
#define RFILE_OPER_RESULT_OK                      (0)

/*事件定义，用来区分当前所作的操作*/
typedef enum
{
    /* RFS stands for Remote File System*/ 
   /*接收到命令的消息*/
    RFS_EVENT_CMD,
    RFS_EVENT_LOCAL,

    /* 组包消息 */
    RFS_EVENT_PACKET,   
    /*发送给核间通信消息 */
    RFS_EVENT_ICC, 
    /*收到C Core消息 */
    RFS_EVENT_MODEM, 
    /* 发送给AP消息 */
    RFS_EVENT_HSIC,    
    /*收到AP回应消息 */
    RFS_EVENT_RECEIVEAP,
     /*收到A核回应消息 */
    RFS_EVENT_ACORE, 
    RFS_EVENT_MAX
}RFS_EVENT_VALUE;

/*定义文件的操作类型*/
typedef enum _bsp_flashless_fileoper_type
{
    FLASHLESS_FILEOPER_OPEN,
    FLASHLESS_FILEOPER_CLOSE,
    FLASHLESS_FILEOPER_READ,
    FLASHLESS_FILEOPER_WRITE,
    FLASHLESS_FILEOPER_SEEK,
    FLASHLESS_FILEOPER_DELETE,
    FLASHLESS_FILEOPER_TELL,
    FLASHLESS_FILEOPER_SET,
    FLASHLESS_FILEOPER_SEND,
    FLASHLESS_FILEOPER_INIT,
    FLASHLESS_FILEOPER_MAX
} bsp_flashless_fileoper_type;


/*存储log信息的数据结构*/
typedef struct _bsp_flashless_log_data
{
    RFS_EVENT_VALUE          eEvent;                                /*文件访问方式,HSIC, ICC等*/
    BSP_U32     codeline;                                                                     /*当前所在行*/    

    BSP_U32     utimestamp;                                                             /*时间戳*/
    BSP_S32     sRelt;                                                                          /*操作结果*/
    BSP_S32     param;                                                                          /*其他有用信息*/
 
    BSP_U32         fhandle;                                                                 /*访问的文件的句柄*/
    char            filename[FLASHLESS_LOG_FILENAME_LEN + 1];       /*访问的文件名字信息*/
    char            srcfile[FLASHLESS_LOG_FILENAME_LEN + 1];       /*访问的源文件名字信息*/
    bsp_flashless_fileoper_type     fileopertype;                       /*文件操作方式，如读、写等*/
} bsp_flashless_log_data;

/*用于核间通讯*/
typedef struct _bsp_flashless_log_ifcmsg
{
    BSP_U32 ulModuleId;
    BSP_U32 ulFuncId;
    BSP_U32 ulFlag;
} bsp_flashless_log_ifcmsg;
typedef struct _bsp_flashless_log_recordmgr
{
    BSP_U32     uIndex;
    BSP_U32     uCount;
    BSP_U32     uStartStop;
} bsp_flashless_log_recordmgr;

/*************************************************************************************************
函数声明
*****************************************************************************************************/

/******************************************************************************
*  Function:  FlashLessLogInit
*  Description:
*  用于对所需要的变量和存储空间初始化
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void FlashLessLogInit(void);
/******************************************************************************
*  Function:  FlashLessLogClearData
*  Description:
*  用于清除存储空间的数据，并初始化index
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void FlashLessLogClearData(void);

/******************************************************************************
*  Function:  void  FlashLessLogGetNameInfo(char *ptrname)
*  Description:
*  摘取文件名字的１５个字符，用于存储
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void  FlashLessLogGetNameInfo(char *ptrname, char *outptrname);

/******************************************************************************
*  Function:  void FlashLessLogPrint(void)
*  Description:
*  将存储空间的log内容打印到shell
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void FlashLessLogPrint(void);

/******************************************************************************
*  Function:  void FlashLessLogSave(void)
*  Description:
*  将存储空间的log内容保存到ap侧的文件中
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void FlashLessLogSave(void);

/******************************************************************************
*  Function:  int  FlashLessLogCanAdd(void)
*  Description:
* 通过判断互斥锁来判断当前是否可以增加新的log信息。
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern int FlashLessLogCanAdd(void);
/******************************************************************************
*  Function:  void  FlashLessLogResetDatabuf()
*  Description:
*  清除当前记录，为新记录存储做准备
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void  FlashLessLogResetDatabuf(void);

/******************************************************************************
*  Function:  void  FlashLessLogAddIndex()
*  Description:
*  对log index进行加一处理
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void  FlashLessLogAddIndex(void);

/******************************************************************************
*  Function:  void  FlashLessLogAddInfoCurCodeFile(UINT32 uline)
*  Description:
*  将log信息存储到环回空间,这里存储当前源文件的行信息
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void  FlashLessLogAddInfoCurCodeFile(char codefile, UINT32 uline);

/******************************************************************************
*  Function:  void  FlashLessLogAddInfoOperation(RFS_EVENT_VALUE eEvent, char *ptrname, BSP_U32 fhandle, bsp_flashless_fileoper_type opertype)
*  Description:
*  将log信息存储到环回空间,这里存储操作的event信息、文件名和句柄，以及文件读写或其他操作信息
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void  FlashLessLogAddInfoOperation(RFS_EVENT_VALUE eEvent, char *ptrname, BSP_U32 fhandle, bsp_flashless_fileoper_type opertype);
/******************************************************************************
*  Function:  void  FlashLessLogAddInfoOthers(int param1, int param2)
*  Description:
*  将log信息存储到环回空间,这里存储其他需要记录的信息
*  Calls:
*
*  Called By:
*
*  Input:
*
*  Output:
*
*  Return:
*
********************************************************************************/
extern void  FlashLessLogAddInfoOthers(int param1, int param2);

/*********************************************************************************
对外接口宏,只有以下部分可以被其他模块调用!!!!
********************************************************************************/
/*初始化*/
#define     FLASHLESS_CMD_LOG_INIT()        FlashLessLogInit()
/*清除数据*/
#define     FLASHLESS_CMD_LOG_CLEARDATA()        FlashLessLogClearData()
/*记录log信息*/
#define     FLASHLESS_CMD_ADD_LOG(event, ptrname, fhandle, opertype, param1, param2)       \
do{                   \
	unsigned long flag = 0;     \
	local_irq_save(flag);       \
    DRV_SPIN_LOCK(IPC_SEM_RFILE_LOG);             \
    if (1 == FlashLessLogCanAdd())              \
    {                   \
        FlashLessLogResetDatabuf();             \
        FlashLessLogAddInfoCurCodeFile(__FILE__, __LINE__);                     \
        FlashLessLogAddInfoOperation(event, ptrname, fhandle, opertype);                 \
        FlashLessLogAddInfoOthers(param1, param2);                \
        FlashLessLogAddIndex();                                         \
    }               \
    DRV_SPIN_UNLOCK(IPC_SEM_RFILE_LOG);         \
	local_irq_restore(flag);        \
} while(0)      

/*把记录的log打印到shell*/
#define     FLASHLESS_CMD_LOG_PRINT()        FlashLessLogPrint()
/*把记录的log保存成文件*/
#define     FLASHLESS_CMD_LOG_SAVE()        FlashLessLogSave()
#else
/*********************************************************************************
对外接口宏,只有以下部分可以被其他模块调用!!!!
********************************************************************************/
/*初始化*/
#define     FLASHLESS_CMD_LOG_INIT()
/*清除数据*/
#define     FLASHLESS_CMD_LOG_CLEARDATA()
/*记录log信息*/
#define     FLASHLESS_CMD_ADD_LOG(event, ptrname, fhandle, opertype, param1, param2)
/*把记录的log打印到shell*/
#define     FLASHLESS_CMD_LOG_PRINT()
/*把记录的log保存成文件*/
#define     FLASHLESS_CMD_LOG_SAVE()
#endif

#endif
