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

#define FS_FILE_HEAD_LEN        92        /*文件头空字节数*/
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
    LZMA,      /*采用LZMA算法压缩*/
    ZIP,        /*目前不支持*/
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
    UPDATE_TYPE m_update_type;               /*升级方案类型*/
    COMPONENT_TYPE m_component_type; /*组件类型*/
    COMPRESS_TYPE m_compress_type;     /*压缩类型*/
    UINT8 m_total_components;                 /*本次版本检测共检测到的有新版本的组件数量*/
    UINT8 m_current_component_index;    /*当前待升级组件在本次升级中的序号*/
    UPDATE_STATUS m_current_component_status; /*当前待升级组件的状态*/
    UINT8 m_full;                       /*webui删除标志位，1:删除 0:不删*/
    UINT8 m_reserved2;
    UINT8 m_reserved3;
    UINT8 m_reserved4;
    void * m_p_reserved5;
}UPDATE_INFOMATION;

#pragma pack(1)
typedef struct SAppFileUnitHeader
{
    UINT32 dwMagicNum;	//取0xA5 5A AA 55
    UINT32 dwHeaderLen;	//本文件头长度，从dwMagicNum开始的长度,包括CRC 
    UINT32 dwHeaderVersion; //针对本结构填1，以后升级时增加
    UINT8  UnlockCode[8]; //区分待下载数据的code，8字节的任意数据，不同平台使用不同的值
    UINT32 dwDataType;//本段数据类型
    UINT32 dwDataLen;//本段数据的实际有效长度，不含填充字节
    UINT8  szDate[16];//制作日期字符串表示	
    UINT8  szTime[16];//制作时间字符串表示		
    UINT8  szDispName[32];	//本UNIT数据产品版本号、目标单板型号和单板软件版本号
    UINT16  wHeadCRC;		//从dwMagicNum校验到dwBlockSize
    UINT32 dwBlockSize;	//帧大小，目前各产品均为4096,每一dwBlockSize数据计算CRC校验
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

