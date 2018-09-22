/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  rfilesystem_logunit.h
*
*
*   ��    �� :  ����rfilesystem �洢log,ʵ�ֿ�ά�ɲ⹦�ܵ�ͷ�ļ���
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
�궨�塢���ݽṹ����
*****************************************************************************************************/
#define FLASHLESS_LOG_FILENAME_LEN              15          /*ֻ�����������ļ����ֵ�15���ַ�*/

#define RFILE_OPER_RESULT_FAIL                      (-1)
#define RFILE_OPER_RESULT_OK                      (0)

/*�¼����壬�������ֵ�ǰ�����Ĳ���*/
typedef enum
{
    /* RFS stands for Remote File System*/ 
   /*���յ��������Ϣ*/
    RFS_EVENT_CMD,
    RFS_EVENT_LOCAL,

    /* �����Ϣ */
    RFS_EVENT_PACKET,   
    /*���͸��˼�ͨ����Ϣ */
    RFS_EVENT_ICC, 
    /*�յ�C Core��Ϣ */
    RFS_EVENT_MODEM, 
    /* ���͸�AP��Ϣ */
    RFS_EVENT_HSIC,    
    /*�յ�AP��Ӧ��Ϣ */
    RFS_EVENT_RECEIVEAP,
     /*�յ�A�˻�Ӧ��Ϣ */
    RFS_EVENT_ACORE, 
    RFS_EVENT_MAX
}RFS_EVENT_VALUE;

/*�����ļ��Ĳ�������*/
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


/*�洢log��Ϣ�����ݽṹ*/
typedef struct _bsp_flashless_log_data
{
    RFS_EVENT_VALUE          eEvent;                                /*�ļ����ʷ�ʽ,HSIC, ICC��*/
    BSP_U32     codeline;                                                                     /*��ǰ������*/    

    BSP_U32     utimestamp;                                                             /*ʱ���*/
    BSP_S32     sRelt;                                                                          /*�������*/
    BSP_S32     param;                                                                          /*����������Ϣ*/
 
    BSP_U32         fhandle;                                                                 /*���ʵ��ļ��ľ��*/
    char            filename[FLASHLESS_LOG_FILENAME_LEN + 1];       /*���ʵ��ļ�������Ϣ*/
    char            srcfile[FLASHLESS_LOG_FILENAME_LEN + 1];       /*���ʵ�Դ�ļ�������Ϣ*/
    bsp_flashless_fileoper_type     fileopertype;                       /*�ļ�������ʽ�������д��*/
} bsp_flashless_log_data;

/*���ں˼�ͨѶ*/
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
��������
*****************************************************************************************************/

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
extern void FlashLessLogInit(void);
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
extern void FlashLessLogClearData(void);

/******************************************************************************
*  Function:  void  FlashLessLogGetNameInfo(char *ptrname)
*  Description:
*  ժȡ�ļ����ֵģ������ַ������ڴ洢
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
extern void FlashLessLogPrint(void);

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
extern void FlashLessLogSave(void);

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
extern int FlashLessLogCanAdd(void);
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
extern void  FlashLessLogResetDatabuf(void);

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
extern void  FlashLessLogAddIndex(void);

/******************************************************************************
*  Function:  void  FlashLessLogAddInfoCurCodeFile(UINT32 uline)
*  Description:
*  ��log��Ϣ�洢�����ؿռ�,����洢��ǰԴ�ļ�������Ϣ
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
extern void  FlashLessLogAddInfoOperation(RFS_EVENT_VALUE eEvent, char *ptrname, BSP_U32 fhandle, bsp_flashless_fileoper_type opertype);
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
extern void  FlashLessLogAddInfoOthers(int param1, int param2);

/*********************************************************************************
����ӿں�,ֻ�����²��ֿ��Ա�����ģ�����!!!!
********************************************************************************/
/*��ʼ��*/
#define     FLASHLESS_CMD_LOG_INIT()        FlashLessLogInit()
/*�������*/
#define     FLASHLESS_CMD_LOG_CLEARDATA()        FlashLessLogClearData()
/*��¼log��Ϣ*/
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

/*�Ѽ�¼��log��ӡ��shell*/
#define     FLASHLESS_CMD_LOG_PRINT()        FlashLessLogPrint()
/*�Ѽ�¼��log������ļ�*/
#define     FLASHLESS_CMD_LOG_SAVE()        FlashLessLogSave()
#else
/*********************************************************************************
����ӿں�,ֻ�����²��ֿ��Ա�����ģ�����!!!!
********************************************************************************/
/*��ʼ��*/
#define     FLASHLESS_CMD_LOG_INIT()
/*�������*/
#define     FLASHLESS_CMD_LOG_CLEARDATA()
/*��¼log��Ϣ*/
#define     FLASHLESS_CMD_ADD_LOG(event, ptrname, fhandle, opertype, param1, param2)
/*�Ѽ�¼��log��ӡ��shell*/
#define     FLASHLESS_CMD_LOG_PRINT()
/*�Ѽ�¼��log������ļ�*/
#define     FLASHLESS_CMD_LOG_SAVE()
#endif

#endif
