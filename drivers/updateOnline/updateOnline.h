#ifndef  _UPDATE_UPDATEONLINE_H_
#define  _UPDATE_UPDATEONLINE_H_

#ifdef WIN32
#include "BSP_define.h"
#endif

#if (FEATURE_UPDATEONLINE == FEATURE_ON)

#include <linux/fs.h>
#include <linux/syscalls.h>

#include "os_platform.h"

#include <generated/FeatureConfig.h>
#include <mach/platform.h>

#include "SDUpdate.h"
#include "update_common.h"

#define FS_FILE_HEAD_LEN        92        /*�ļ�ͷ���ֽ���*/
#define FS_MAX_HEAD_NUM         16
#define UPDATE_UNIT_HEAD_FLAG   0xA55AAA55


#ifdef WIN32
#define UPDATEPATH                           ("/online")
#define FIRMWARE1_FILE_PATH_NAME             ("firmware1.bin")
#define FIRMWARE2_FILE_PATH_NAME             ("firmware2.bin")
#define UPDATE_ISO_FILE_PATH_NAME            ("iso.bin")
#define WEB_UI_FILE_PATH_NAME                ("web_ui.bin")

#define UPDATE_INFO_FILE_PATH_NAME           ("update_info.bin")
#else
#define UPDATEPATH                           ("/online")
#define FIRMWARE1_FILE_PATH_NAME             ("/online/firmware1.bin")
#define FIRMWARE2_FILE_PATH_NAME             ("/online/firmware2.bin")
#define UPDATE_ISO_FILE_PATH_NAME            ("/online/iso.bin")
#define WEB_UI_FILE_PATH_NAME                ("/online/web_ui.bin")

#define UPDATE_INFO_FILE_PATH_NAME           ("/data/userdata/update_info.bin")
#endif

typedef enum fun_return 
{
    RETURN_ERROR = -1,
    RETURN_OK =0,
} RETURN_BOOL_TYPE;

typedef enum _WEBUI_FLAG_STATUS
{
    WEBUI_UPDATE = 0,
    WEBUI_NOT_UPDATE = 1
    
}WEBUI_FLAG_STATUS;

typedef struct tagONLLINEUPDATE_VER_IFC_MSG_STRU
{
    unsigned int u32ModuleId;
    unsigned int u32FuncId;
    SDMMC_HOOK_TYPE   nHookNum;
}ONLLINEUPDATE_VER_IFC_MSG_STRU;

typedef enum _COMPONENT_TYPE
{
    FIRMWARE1,
    FIRMWARE2,
    ISO,
    WEB_UI,
    UPDATE_INFO,
    COMPONENT_COUNT
}COMPONENT_TYPE;

typedef enum _UPDATE_TYPE
{
    HUAWEI_PRIVATE,
    OMA_FOTA,      
    UPDATE_TYPE_COUNT
}UPDATE_TYPE;

typedef enum _COMPRESS_TYPE
{
    NO_COMPRESS,
    LZMA,      /*����LZMA�㷨ѹ��*/
    ZIP,        /*Ŀǰ��֧��*/
    COMPRESS_TYPE_COUNT
}COMPRESS_TYPE;

typedef enum _UPDATE_MESSAGE_TYPE
{
    IDLEUP = 10,
    QUERYING = 11,
    NEWVERSIONFOUND = 12,
    DOWNLOAD_FAILED = 20,
    DOWNLOAD_PROGRESSING = 30,
    DOWNLOADPENDING = 31,
    DOWNLOAD_COMPLETE = 40,
    READY_TO_UPDATE = 50,
    UPDATE_PROGRESSING = 60,
    UPDATE_FAILED_HAVE_DATA = 70,
    UPDATE_FAILED_NO_DATA = 80,
    UPDATE_SUCCESSFUL_HAVE_DATA = 90,
    UPDATE_SUCCESSFUL_NO_DATA = 100,
    UPDATE_MESSAGE_TYPE_COUNT
}UPDATE_MESSAGE_TYPE, UPDATE_STATUS;

typedef struct _UPDATE_INFOMATION
{    
    UINT32   m_update_info_length;     
    UPDATE_TYPE m_update_type;               /*������������*/
    COMPONENT_TYPE m_component_type; /*�������*/
    COMPRESS_TYPE m_compress_type;     /*ѹ������*/
    UINT8 m_total_components;                 /*���ΰ汾��⹲��⵽�����°汾���������*/
    UINT8 m_current_component_index;    /*��ǰ����������ڱ��������е����*/
    UPDATE_STATUS m_current_component_status; /*��ǰ�����������״̬*/
    UINT8 m_full;                       /*webuiɾ����־λ��1:ɾ�� 0:��ɾ*/
    UINT8 m_reserved2;
    UINT8 m_reserved3;
    UINT8 m_reserved4;
    void * m_p_reserved5;
}UPDATE_INFOMATION;

#pragma pack(1)
typedef struct SAppFileUnitHeader
{
    UINT32 dwMagicNum;	//ȡ0xA5 5A AA 55
    UINT32 dwHeaderLen;	//���ļ�ͷ���ȣ���dwMagicNum��ʼ�ĳ���,����CRC 
    UINT32 dwHeaderVersion; //��Ա��ṹ��1���Ժ�����ʱ����
    UINT8  UnlockCode[8]; //���ִ��������ݵ�code��8�ֽڵ��������ݣ���ͬƽ̨ʹ�ò�ͬ��ֵ
    UINT32 dwDataType;//������������
    UINT32 dwDataLen;//�������ݵ�ʵ����Ч���ȣ���������ֽ�
    UINT8  szDate[16];//���������ַ�����ʾ	
    UINT8  szTime[16];//����ʱ���ַ�����ʾ		
    UINT8  szDispName[32];	//��UNIT���ݲ�Ʒ�汾�š�Ŀ�굥���ͺź͵�������汾��
    UINT16  wHeadCRC;		//��dwMagicNumУ�鵽dwBlockSize
    UINT32 dwBlockSize;	//֡��С��Ŀǰ����Ʒ��Ϊ4096,ÿһdwBlockSize���ݼ���CRCУ��
} FileUnitHeader;
#pragma pack()

typedef struct
{
    BSP_U32 u32ModuleId;
    BSP_U32 u32FuncId;
	BSP_U32 u32SetStatusRet;
}ONUP_SET_STATUS_MSG_STRU;

extern void  usrPlatformReset(void);
extern void wdtReboot(void);
#endif

#endif /* _UPDATE_UPDATEONLINE_H_ */

