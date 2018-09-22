/*************************************************************************
*   版权所有(C) 2011-2030, 深圳华为技术有限公司.
*
*   文 件 名 :  SDUpdate.h
*
*
*   描    述 :  SD升级A核文件校验
*
*************************************************************************/
#include<linux/netlink.h>
#include "ptable_def.h"

#define LICCRCDATANUM         (256)       /*lic的CRC校验所取字节个数*/
#define BINFADDRFORCRC        (0x500000)  /*LIC的CRC校验对应BIN的取值起始地址*/
#define FILENAME_NUM_MAX      (5)         /*dir文件的最大值*/
#define FILENAME_LEN_MAX      (256)       /*文件名的最大长度*/

#define TFDIRNAME          "/huawei_dload"/*TF卡的文件目录名，/sd为设备名*/
#define VEFILEEXTNAME      ".lic"     /*一般为小写*/
#define UPFILEEXTNAME      ".BIN"     /*一般都为大写*/
#define UPFILEEXTNAME_LOWCASE      ".bin"
#define EXTNAMELEN         (4)
#define LOGFILENAME        "/huawei_dload/LOG.TXT"  /*必须大写*/
#define FLASHLOGFILENAME        "/yaffs0/sdUpdateLog.txt"

#define OSAL_ERROR (-1)  /*函数异常返回值*/
#define OSAL_OK    (0)  /*函数成功返回值*/

#define MAINVERSION             "00"      /*主线版本*/
#define UNITHEAD_TYPE_OFF       (112)       /*文件第一单元单元类型偏移*/
#define TF_LOGSIZE_MAX          (0x10000)   /*log的最大值*/

typedef struct sd_dev_type_str
{
    unsigned long devNameLen; 
    char devName[16];
}SD_DEV_STR;

 /*升级文件类型*/
typedef enum
{
    FIRMWARESOFT =155,
    DASHBOARD,
    OTHERSOFT
    
}UPFILETYPE;

 /*文件名及类型*/
typedef struct 
{
    char *pFileName;
    UPFILETYPE upFileType;
}TFFILE;

 #ifndef NULL
#define NULL    (void *)(0)
#endif

 #define UPFILENUM_MAX      2
#define VEFILENUM_MAX      2         /*和升级文件匹配的校验文件最大个数*/


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define BOOL    int

/*dir中的文件类型*/
typedef enum 
 {
     BIN=100,
     LIC,
     LOG,
     ELSE
 }FILETYPE;

 void hi_TFUP_CheckTask(void);/*校验任务*/


typedef struct {
  /*预留*/
  unsigned int ulReserved;
}TEMP_PROTECT_TYPE;

typedef struct {
  unsigned int  index;                              /*硬件版本号数值(大版本号+扩展版本号)，区分不同产品*/
  char* name;                                /*内部产品名*/
  char* namePlus;                         /*内部产品名PLUS*/
  char* hwVer;                             /*硬件版本号*/
  char* dloadId;                           /*升级中使用的名称*/
  char* productId;                       /*外部产品名*/
  unsigned int  band;                            /*产品支持的频段*/
  unsigned int  bandDiv;                       /*产品支持的分集*/
  TEMP_PROTECT_TYPE tempProtect;    /*温度保护数值，预留*/
}PRODUCT_INFO_TYPE;

//PRODUCT_INFO_TYPE g_ProductInfo = {0};  /*全局变量，保存产品信息*/


/*升级标志位,置位为正常升级及LED受控*/
typedef enum 
{
    TFUP_IDLE = 0,      /* 尚未升级 */
    TFUP_RUNNING = 1,   /* 升级中 */
    TFUP_DONE = 2,      /* 升级完成 */    
}TFUP_FLAG_E;

/*BOOTROM标志位*/
typedef enum 
{
    VXWORKSFLAG = 0,
    BOOTROMFLAG
}TFUP_FLAG_BOOTROM;

//time_t  localtime;

#define DLOAD_ID "BALONGSPC5H"
#define HARDWARE_VER_INVALID    0xFFFFFFFF    /*无效的硬件版本号*/

 /* 三色灯状态编号*/
enum LED_STATUS
{
    DRV_LED_LIGHT_SYSTEM_STARTUP,        /*绿灯双闪*/
    DRV_LED_LIGHT_POWER_ON,                   /*绿灯双闪*/
    DRV_LED_LIGHT_G_REGISTERED,             /*绿灯单闪*/
    DRV_LED_LIGHT_W_REGISTERED,            /*蓝灯单闪*/
    DRV_LED_LIGHT_G_CONNECTED,              /*绿灯常亮*/
    DRV_LED_LIGHT_W_CONNNECTED,           /*蓝灯常亮*/
    DRV_LED_LIGHT_H_CONNNECTED,            /*青灯常亮*/
    DRV_LED_LIGHT_OFFLINE,                        /*绿灯双闪*/
    DRV_LED_LIGHT_FORCE_UPDATING,        /*升级中*//*绿灯频闪*/
    DRV_LED_LIGHT_NORMAL_UPDATING,     /*升级中*//*绿灯频闪*/
    DRV_LED_LIGHT_UPDATE_FAIL,               /*升级错误*//*绿灯常亮*/
    DRV_LED_LIGHT_UPDATE_SUCCESS,        /*TF卡加载:升级完成*//*蓝灯常亮*/  
    DRV_LED_LIGHT_UPDATE_FILEERROR,    /*TF卡加载:升级文件错误*//*蓝绿交替双闪*/ 
    DRV_LED_LIGHT_UPDATE_NVFAIL,          /*NV恢复失败*//*蓝灯频闪*/    
    DRV_LED_LIGHT_SIM_ABSENT,                        /*检测SIM卡错（包括SIM lock）*//*红灯常亮*/
    DRV_LED_LIGHT_SHUTDOWN,                  /*灯灭*/
    DRV_LED_LIGHT_STATE_MAX = 16
}; /*the state of LED */

//#define ISO_FILE_ADDR 0x5    /*乒乓Bootrom合入，增加了BOOTROM_BACK枚举，为兼容之前的打包工具定义的ZSP文件头地址*/
#define MOBILE_CONNECT_HD_ADDR 0xA0000    /*深圳终端软件部后台软件升级工具定义的ISO文件头的地址*/
#define MOBILE_CONNECT_ADDR    0xB0000    /*深圳终端软件部后台软件升级工具定义的ISO文件的地址*/
#define WEBUI_HD_ADDR 0x550000    /*深圳终端软件部WEBUI软件升级工具定义的WEBUI文件版本的地址*/
#define WEBUI_ADDR    0x560000    /*深圳终端软件部WEBUI软件升级工具定义的WEBUI文件的地址*/
#define IMAGE_MCORE_ADDR            0x550103    /*vxworks ID使用这个ID可以，实现下载过程复位功能*/

typedef enum sdmmc_hook_type 
{
    SDMMC_HOOK_UPDATE_NOTIFY = 0,
    SDMMC_HOOK_INSERT_NOTIFY = 1,
    SDMMC_HOOK_REMOVE_NOTIFY = 2,
    SDMMC_HOOK_USB_IN_NOTIFY = 3,
    SDMMC_HOOK_UPVERIFY_NOTIFY =4, 
    SDMMC_HOOK_BUFF
} SDMMC_HOOK_TYPE;


 #define  CDROMISOVER    (0x10c)
 #define  CDROMISO       (1)


/*双核通信数据结构*/
typedef struct tagHISDUPDATE_IFC_MSG_STRU
{
    unsigned int u32ModuleId;
    unsigned int u32FuncId;
    SDMMC_HOOK_TYPE   nHookNum;
    	
}HISDUPDATE_IFC_MSG_STRU;
 

 typedef struct tagHISDUPDATE_VER_REG_MSG_STRU
{
    unsigned int u32ModuleId;
    unsigned int u32FuncId;
    unsigned  char  dloadID[128];
    unsigned char  CompVer[31];         /* 最大版本长度 30 字符+ \0 */
}HISDUPDATE_VER_REG_MSG_STRU;

 typedef struct tagHISDUPDATE_VER_REV_STRU
{
    unsigned  char  dloadID[128];
    unsigned char  CompVer[31];         /* 最大版本长度 30 字符+ \0 */
}HISDUPDATE_VER_REV_STRU;

 
typedef struct tagHISDUPDATE_VER_IFC_MSG_STRU
{
    unsigned int u32ModuleId;
    unsigned int u32FuncId;
}HISDUPDATE_VER_IFC_MSG_STRU;


typedef struct device_event_st
{
    int device_id;   /*设备ID*/
    int value;         /*消息value*/ 
    char * desc;     /*描述*/
}device_event_t;

/*升级成功标志位*/
typedef enum 
{
    TFUPUNDOFLAG = 0,
    SUCCESSFLAG
}TFUP_FLAG_RESULT;

/*VERSIONINFO_I数据结构中版本字符串最大有效字符长度*/
#define VER_MAX_LENGTH                  30

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH + 1];         /* 最大版本长度 30 字符+ \0 */
} VERSIONINFO;

#define READ_PATH "/mnt/sdcard"

int report_sd_state( DEVICE_ID dev, int nvalue, char *chr);
int balong_Ver_IFCInit(void);
int app_sd_get_state(void);
int hi_sdio_send_to_SDDriver(void *pstSDLDO10Arg, int nSize);
int hi_TFUP_SetSuccessFlag(int flag);/*设置TF升级成功标志位，A核调用*/
int hi_TFUP_GetSuccessFlag(void);/*获得TF升级成功标志位，A核调用*/

int  imageTypeChange(int* pType);
void hi_TFUP_setTFUpdateFlag(TFUP_FLAG_E flag);
int creat_NvResume(void);

