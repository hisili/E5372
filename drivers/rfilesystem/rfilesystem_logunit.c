/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  rfilesystem_logunit.c
*
*
*   ��    �� :  ����rfilesystem �洢log,ʵ�ֿ�ά�ɲ⡣
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
�궨��
*****************************************************************************************/
/*�����¼������������ﵽ��ֵ�󻷻ش洢*/
#define     FLASHLESS_LOG_MAX_NUMBER        (FLASHLESS_LOG_DATA_SIZE/sizeof(bsp_flashless_log_data))


#define FLASHLESS_LOG_RECORD_START                   0x2468ace0
#define FLASHLESS_LOG_RECORD_STOP               0x0eca8642

/*���ڶ���Ӽ�¼�Ĺ���*/
#define FLASHLESS_LOG_RECORDMGR_DATA_ADDR                     (EXCH_RFS_ADDR)
#define FLASHLESS_LOG_RECORDMGR_DATA_SIZE                     (sizeof(bsp_flashless_log_recordmgr))


/*���ڴ洢log����*/
#define FLASHLESS_LOG_DATA_ADDR                  (FLASHLESS_LOG_RECORDMGR_DATA_ADDR + FLASHLESS_LOG_RECORDMGR_DATA_SIZE)
#define FLASHLESS_LOG_DATA_SIZE                   (EXCH_RFS_SIZE - FLASHLESS_LOG_RECORDMGR_DATA_SIZE)  

/****************************************************************************************
ȫ�ֱ�������
*****************************************************************************************/
/*���ڱ����¼��������ǰindex���Ƿ�������Ӽ�¼������Ϣ*/
bsp_flashless_log_recordmgr     *g_flashless_log_recordmgr = (bsp_flashless_log_recordmgr *)FLASHLESS_LOG_RECORDMGR_DATA_ADDR;
/*����log����*/
bsp_flashless_log_data   *g_flashless_log_data_ptr = (bsp_flashless_log_data *)FLASHLESS_LOG_DATA_ADDR;

/*���ڴ�ӡ����������log��ӡ��ֱ��
ע��Ҫ��RFS_EVENT_VALUE˳��/��������һ��*/
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
/*���ڴ�ӡ����������log��ӡ��ֱ��
ע��Ҫ��bsp_flashless_fileoper_type �е�˳��/��������һ��*/
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
��������
*****************************************************************************************/

/******************************************************************************
*  Function:  FlashLessLogInit
*  Description:
*  ���ڶ�����Ҫ�ı����ʹ洢�ռ��ʼ��
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
            /*����memset̫�磬������*/
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
*  ��������洢�ռ�����ݣ�����ʼ��index
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
*  ժȡ�ļ����ֵ�FLASHLESS_LOG_FILENAME_LEN���ַ������ڴ洢
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

        /*�ļ�����Ϣ���Ȳ��ܴ���FLASHLESS_LOG_FILENAME_LEN���ַ�������ֻ��¼��FLASHLESS_LOG_FILENAME_LEN���ַ�*/
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
*  ��log��Ϣ�洢�����ؿռ�,����洢��ǰԴ�ļ������ֺ���������Ϣ
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
*  ��log��Ϣ�洢�����ؿռ�,����洢������event��Ϣ���ļ����;�����Լ��ļ���д������������Ϣ
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
*  ��log��Ϣ�洢�����ؿռ�,����洢������Ҫ��¼����Ϣ
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
*  �����ǰ��¼��Ϊ�¼�¼�洢��׼��
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
*  ��log index���м�һ����
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

    /*ֻ��֮ǰ������Ч����log��Ϣ������Ż��index���м�һ����*/
    if (NULL != g_flashless_log_data_ptr)
    {        
        (g_flashless_log_recordmgr->uIndex)++;
        (g_flashless_log_recordmgr->uCount)++;
        /*���û��ط�ʽ�洢,���ﵽ���������,��ͷ��ʼ���и��Ǵ洢*/
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
*  ���洢�ռ��log���ݴ�ӡ��shell
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
    /*ȷ����ǰû�����log�ļ�*/
    if (NULL != g_flashless_log_recordmgr
        && FLASHLESS_LOG_RECORD_START == g_flashless_log_recordmgr->uStartStop)
    {
        g_flashless_log_recordmgr->uCount = (g_flashless_log_recordmgr->uCount > FLASHLESS_LOG_MAX_NUMBER)?FLASHLESS_LOG_MAX_NUMBER: g_flashless_log_recordmgr->uCount;
        printk("===============================total records is %d====================================\n", g_flashless_log_recordmgr->uCount);
        while (logindex < (g_flashless_log_recordmgr->uCount))
        {
            /*����Ǳ��ж�*/
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
*  ���洢�ռ��log���ݱ��浽ap����ļ���
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
* ͨ���жϻ��������жϵ�ǰ�Ƿ���������µ�log��Ϣ��
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
