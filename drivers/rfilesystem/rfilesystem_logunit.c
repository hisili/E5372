/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  rfilesystem_logunit.c
*
*
*   描    述 :  用于rfilesystem 存储log,实现可维可测。
*
*
*************************************************************************/
#include "rfilesystem_logunit.h"
#include "../mntn/excDrv.h"
#include "BSP_IFC_API.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <linux/jiffies.h>
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef FLASHLESS_RFILE_LOG
/****************************************************************************************
宏定义
*****************************************************************************************/
/*保存记录的最大条数，达到该值后环回存储*/
#define     FLASHLESS_LOG_MAX_NUMBER        (FLASHLESS_LOG_DATA_SIZE/sizeof(bsp_flashless_log_data))


#define FLASHLESS_LOG_RECORD_START                   0x2468ace0
#define FLASHLESS_LOG_RECORD_STOP               0x0eca8642

/*用于对添加记录的管理*/
#define FLASHLESS_LOG_RECORDMGR_DATA_ADDR                     (EXCH_RFS_ADDR)
#define FLASHLESS_LOG_RECORDMGR_DATA_SIZE                     (sizeof(bsp_flashless_log_recordmgr))


/*用于存储log数据*/
#define FLASHLESS_LOG_DATA_ADDR                  (FLASHLESS_LOG_RECORDMGR_DATA_ADDR + FLASHLESS_LOG_RECORDMGR_DATA_SIZE)
#define FLASHLESS_LOG_DATA_SIZE                   (EXCH_RFS_SIZE - FLASHLESS_LOG_RECORDMGR_DATA_SIZE)  

/****************************************************************************************
全局变量定义
*****************************************************************************************/
/*用于保存记录总数、当前index和是否允许添加记录的锁信息*/
bsp_flashless_log_recordmgr     *g_flashless_log_recordmgr = (bsp_flashless_log_recordmgr *)FLASHLESS_LOG_RECORDMGR_DATA_ADDR;
/*保存log数据*/
bsp_flashless_log_data   *g_flashless_log_data_ptr = (bsp_flashless_log_data *)FLASHLESS_LOG_DATA_ADDR;

/*用于打印解析，便于log打印更直观
注意要与RFS_EVENT_VALUE顺序/个数保持一致*/
const char* g_RfileLogEvent[]=
{
    "RFS_EVENT_NONE",
     "RFS_EVENT_LOCAL",
    "RFS_EVENT_PACKET",
    "RFS_EVENT_ICC",
    "RFS_EVENT_MODEM",
    "RFS_EVENT_HSIC",
    "RFS_EVENT_RECEIVEAP",
    "RFS_EVENT_ACORE",
    "RFS_EVENT_MAX",
};
/*用于打印解析，便于log打印更直观
注意要与bsp_flashless_fileoper_type 中的顺序/个数保持一致*/
const char* g_RfileLogFileoperate[]=
{
    "open file",
    "close file",
    "to read",
    "to write",
    "to seek",
    "to delete",
     "to tell",
    "to set",
    "to send",
    "to init",
    "none",
};
extern void msleep(unsigned int msec);
/****************************************************************************************
函数定义
*****************************************************************************************/

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
void FlashLessLogInit(void)
{
    if (NULL != g_flashless_log_recordmgr)
    {
        if (FLASHLESS_LOG_RECORD_START != g_flashless_log_recordmgr->uStartStop
            && FLASHLESS_LOG_RECORD_STOP != g_flashless_log_recordmgr->uStartStop)
        {
            printk("FlashLessLogInit, To clear all data in A core!\n");
            /*调用memset太早，会死机*/
          //  memset((void*)EXCH_RFS_ADDR, 0x00, EXCH_RFS_SIZE);
            g_flashless_log_recordmgr->uStartStop = FLASHLESS_LOG_RECORD_START;
            g_flashless_log_recordmgr->uCount = 0;
            g_flashless_log_recordmgr->uIndex = 0;
        }
        else
        {
            printk("FlashLessLogInit, the unit has been initiated by C core!\n");
        }
    } 
    else
    {
        printk("FlashLessLogInit, g_flashless_log_recordmgr = NULL!\n");
    }
}
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
void FlashLessLogClearData(void)
{
    memset((void*)EXCH_RFS_ADDR, 0x00, EXCH_RFS_SIZE);
    g_flashless_log_recordmgr->uStartStop = FLASHLESS_LOG_RECORD_START;
    printk("FlashLessLogClearData, this function has run!\n");
}

/******************************************************************************
*  Function:  void  FlashLessLogGetNameInfo(char *ptrname)
*  Description:
*  摘取文件名字的FLASHLESS_LOG_FILENAME_LEN个字符，用于存储
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
void  FlashLessLogGetNameInfo(char *ptrname, char *outptrname)
{
    BSP_U32     uPos = 0;
    int     iLen = 0;
    
    if (NULL != ptrname && NULL != outptrname)
    {
        iLen = strlen((const char*)ptrname);   

        /*文件名信息长度不能大于FLASHLESS_LOG_FILENAME_LEN个字符，否则只记录后FLASHLESS_LOG_FILENAME_LEN个字符*/
        uPos = (iLen > FLASHLESS_LOG_FILENAME_LEN)?(iLen - FLASHLESS_LOG_FILENAME_LEN):0;
        iLen = iLen - uPos;
        strncpy(outptrname, (const char*)(ptrname + uPos), iLen);
    }
    else
    {
        printk("FlashLessLogGetNameInfo, ptr is NULL!\n");
    }
}


/******************************************************************************
*  Function:  void  FlashLessLogAddInfoCurCodeFile(UINT32 uline)
*  Description:
*  将log信息存储到环回空间,这里存储当前源文件的名字和所在行信息
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
void  FlashLessLogAddInfoCurCodeFile(char codefile, UINT32 uline)
{
     char        shortfilename[FLASHLESS_LOG_FILENAME_LEN + 1] = {0}; 
 
    if (NULL != g_flashless_log_data_ptr)
    {
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].codeline = uline;
        //todo, add time stamp bellow
        {
         //   FlashLessLogGetNameInfo("A core file", shortfilename);
            strcpy(g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].srcfile, "Acore");        
        }
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].utimestamp = jiffies;
    }        
    else
    {
        printk("FlashLessLogAddInfoCurCodeFile, s_flashless_log_data_ptr is NULL!\n");
    }
}

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
void  FlashLessLogAddInfoOperation(RFS_EVENT_VALUE eEvent, char *ptrname, BSP_U32 fhandle, bsp_flashless_fileoper_type opertype)
{
    char        shortfilename[FLASHLESS_LOG_FILENAME_LEN + 1] = {0}; 
    
    if (NULL != g_flashless_log_data_ptr)
    {
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].eEvent = eEvent;
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].fileopertype = opertype;
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].fhandle = fhandle;
        if (NULL != ptrname)
        {
            FlashLessLogGetNameInfo(ptrname, shortfilename);
            strcpy(g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].filename, (const char*)shortfilename);        
        }
        else
        {
            printk("FlashLessLogAddInfoOperation, ptrname is NULL!\n");
        }
    }        
    else
    {
        printk("FlashLessLogAddInfoOperation,  g_flashless_log_data_ptr is NULL!\n");
    }
}
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
void  FlashLessLogAddInfoOthers(int param1, int param2)
{
    if (NULL != g_flashless_log_data_ptr)
    {
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].sRelt = param1;
        g_flashless_log_data_ptr[g_flashless_log_recordmgr->uIndex].param = param2;
    }        
    else
    {
        printk("FlashLessLogAddInfoOthers, g_flashless_log_data_ptr is NULL!\n");
    }
}

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
void  FlashLessLogResetDatabuf(void)
{
//    printk("FlashLessLogResetDatabuf, index is %d!\n", g_flashless_log_recordmgr->uIndex);
    if (NULL != g_flashless_log_data_ptr
        && g_flashless_log_recordmgr->uIndex < FLASHLESS_LOG_MAX_NUMBER)
    {        
        memset((void*)(g_flashless_log_data_ptr + (g_flashless_log_recordmgr->uIndex)), 0, sizeof(bsp_flashless_log_data));
    }
}

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
void  FlashLessLogAddIndex(void)
{

    /*只有之前做过有效增加log信息，这里才会对index进行加一处理*/
    if (NULL != g_flashless_log_data_ptr)
    {        
        (g_flashless_log_recordmgr->uIndex)++;
        (g_flashless_log_recordmgr->uCount)++;
        /*采用环回方式存储,即达到最大条数后,从头开始进行覆盖存储*/
        if (g_flashless_log_recordmgr->uIndex >= FLASHLESS_LOG_MAX_NUMBER)
        {
            g_flashless_log_recordmgr->uIndex = 0;
            g_flashless_log_recordmgr->uCount = FLASHLESS_LOG_MAX_NUMBER;
//            printk("FlashLessLogAddIndex, the number of total logs is MAX!\n");
        }
//        printk("FlashLessLogAddIndex, index is %d!\n", g_flashless_log_recordmgr->uIndex);
    }        
    else
    {
        printk("FlashLessLogAddIndex, g_flashless_log_data_ptr is NULL!\n");
    }
}
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
void FlashLessLogPrint(void)
{
    bsp_flashless_log_data  *ptr_log = (bsp_flashless_log_data *)FLASHLESS_LOG_DATA_ADDR;
    BSP_U16     logindex = 0;

    printk("===============================flashless log starts====================================\n");
    /*确保当前没有输出log文件*/
    if (NULL != g_flashless_log_recordmgr
        && FLASHLESS_LOG_RECORD_START == g_flashless_log_recordmgr->uStartStop)
    {
        g_flashless_log_recordmgr->uCount = (g_flashless_log_recordmgr->uCount > FLASHLESS_LOG_MAX_NUMBER)?FLASHLESS_LOG_MAX_NUMBER: g_flashless_log_recordmgr->uCount;
        printk("===============================total records is %d====================================\n", g_flashless_log_recordmgr->uCount);
        while (logindex < (g_flashless_log_recordmgr->uCount))
        {
            /*数组角标判断*/
            (ptr_log + logindex)->eEvent = ((ptr_log + logindex)->eEvent > RFS_EVENT_MAX)? RFS_EVENT_MAX: (ptr_log + logindex)->eEvent;
            (ptr_log + logindex)->fileopertype = ((ptr_log + logindex)->fileopertype > FLASHLESS_FILEOPER_MAX)?FLASHLESS_FILEOPER_MAX : (ptr_log + logindex)->fileopertype;

            printk("%s  line %d  curtime = %d\n%s (%s)  fhandle = 0x%x  %s result = %d param = %d\n", 
                (ptr_log + logindex)->srcfile, (ptr_log + logindex)->codeline, (ptr_log + logindex)->utimestamp, g_RfileLogEvent[(ptr_log + logindex)->eEvent], (ptr_log + logindex)->filename, (ptr_log + logindex)->fhandle, g_RfileLogFileoperate[(ptr_log + logindex)->fileopertype],
                            (ptr_log + logindex)->sRelt, (ptr_log + logindex)->param);
            logindex++;
            msleep(1);
        }
    }
    else
    {
        printk("g_flashless_log_recordmgr = NULL or creating log file! \n");
    }
    printk("===============================flashless log ends====================================\n");
     
}
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
void FlashLessLogSave(void)
{
    bsp_flashless_log_ifcmsg pMsg = {0};
    pMsg.ulModuleId = IFC_MODULE_BSP;
    pMsg.ulFuncId  = IFCP_FUNC_RFILE_LOG_SAVE;
    pMsg.ulFlag    = 0xAAAA;
    printk("RFILE: IFC Send!\n");
    if(0 != BSP_IFC_Send((BSP_VOID*)&pMsg, sizeof(bsp_flashless_log_ifcmsg)))
    {
        printk("FRILE: IFC Send Fail!\n");
    }
}

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
int FlashLessLogCanAdd(void)
{
    int     iRelt = 0;
    
    if (NULL != g_flashless_log_recordmgr
        && FLASHLESS_LOG_RECORD_START == g_flashless_log_recordmgr->uStartStop)
    {
        iRelt = 1;
    //    printk("FlashLessLogCanAdd, to add, index = %d!\n", g_flashless_log_recordmgr->uIndex);
    }
    else
    {
    //    printk("FlashLessLogCanAdd, can not be added!\n");
    }
    return iRelt;
}
void FlashLessLogAddLogs(void)
{
    bsp_flashless_log_ifcmsg pMsg = {0};
    pMsg.ulModuleId = IFC_MODULE_BSP;
//    pMsg.ulFuncId  = IFCP_FUNC_RFILE_LOG_TEST;
    pMsg.ulFlag    = 0xAAAA;
    printk("RFILE: IFC Send!\n");
    if(0 != BSP_IFC_Send((BSP_VOID*)&pMsg, sizeof(bsp_flashless_log_ifcmsg)))
    {
        printk("FRILE: IFC Send Fail!\n");
    }
}
void rfile_log_test0(void)
{
   FlashLessLogInit();
}
void rfile_log_test1(void)
{
   FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "adb/bsp_perimgr_test", 0x12213213, 3, 4,5);
}

void rfile_log_test2(void)
{
   FLASHLESS_CMD_ADD_LOG(RFS_EVENT_HSIC, "adb/bsp_perimgr_test2", 0x12213213, 3, 4,5);
}

void rfile_log_test3(void)
{
    int i = 0;
    for (i = 0;i < FLASHLESS_LOG_MAX_NUMBER;i++)
    {
        rfile_log_test1();
        if (FLASHLESS_LOG_MAX_NUMBER/100 == i )
        {
            FlashLessLogAddLogs();
        }
        if (0 == g_flashless_log_recordmgr->uIndex)
        {
            break;
        }
    }
}
#endif

#ifdef __cplusplus
}
#endif
