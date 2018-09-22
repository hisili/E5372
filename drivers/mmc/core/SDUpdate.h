/*************************************************************************
*   ��Ȩ����(C) 2011-2030, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  SDUpdate.h
*
*
*   ��    �� :  SD����A���ļ�У��
*
*************************************************************************/
#include<linux/netlink.h>
#include "ptable_def.h"

#define LICCRCDATANUM         (256)       /*lic��CRCУ����ȡ�ֽڸ���*/
#define BINFADDRFORCRC        (0x500000)  /*LIC��CRCУ���ӦBIN��ȡֵ��ʼ��ַ*/
#define FILENAME_NUM_MAX      (5)         /*dir�ļ������ֵ*/
#define FILENAME_LEN_MAX      (256)       /*�ļ�������󳤶�*/

#define TFDIRNAME          "/huawei_dload"/*TF�����ļ�Ŀ¼����/sdΪ�豸��*/
#define VEFILEEXTNAME      ".lic"     /*һ��ΪСд*/
#define UPFILEEXTNAME      ".BIN"     /*һ�㶼Ϊ��д*/
#define UPFILEEXTNAME_LOWCASE      ".bin"
#define EXTNAMELEN         (4)
#define LOGFILENAME        "/huawei_dload/LOG.TXT"  /*�����д*/
#define FLASHLOGFILENAME        "/yaffs0/sdUpdateLog.txt"

#define OSAL_ERROR (-1)  /*�����쳣����ֵ*/
#define OSAL_OK    (0)  /*�����ɹ�����ֵ*/

#define MAINVERSION             "00"      /*���߰汾*/
#define UNITHEAD_TYPE_OFF       (112)       /*�ļ���һ��Ԫ��Ԫ����ƫ��*/
#define TF_LOGSIZE_MAX          (0x10000)   /*log�����ֵ*/

typedef struct sd_dev_type_str
{
    unsigned long devNameLen; 
    char devName[16];
}SD_DEV_STR;

 /*�����ļ�����*/
typedef enum
{
    FIRMWARESOFT =155,
    DASHBOARD,
    OTHERSOFT
    
}UPFILETYPE;

 /*�ļ���������*/
typedef struct 
{
    char *pFileName;
    UPFILETYPE upFileType;
}TFFILE;

 #ifndef NULL
#define NULL    (void *)(0)
#endif

 #define UPFILENUM_MAX      2
#define VEFILENUM_MAX      2         /*�������ļ�ƥ���У���ļ�������*/


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define BOOL    int

/*dir�е��ļ�����*/
typedef enum 
 {
     BIN=100,
     LIC,
     LOG,
     ELSE
 }FILETYPE;

 void hi_TFUP_CheckTask(void);/*У������*/


typedef struct {
  /*Ԥ��*/
  unsigned int ulReserved;
}TEMP_PROTECT_TYPE;

typedef struct {
  unsigned int  index;                              /*Ӳ���汾����ֵ(��汾��+��չ�汾��)�����ֲ�ͬ��Ʒ*/
  char* name;                                /*�ڲ���Ʒ��*/
  char* namePlus;                         /*�ڲ���Ʒ��PLUS*/
  char* hwVer;                             /*Ӳ���汾��*/
  char* dloadId;                           /*������ʹ�õ�����*/
  char* productId;                       /*�ⲿ��Ʒ��*/
  unsigned int  band;                            /*��Ʒ֧�ֵ�Ƶ��*/
  unsigned int  bandDiv;                       /*��Ʒ֧�ֵķּ�*/
  TEMP_PROTECT_TYPE tempProtect;    /*�¶ȱ�����ֵ��Ԥ��*/
}PRODUCT_INFO_TYPE;

//PRODUCT_INFO_TYPE g_ProductInfo = {0};  /*ȫ�ֱ����������Ʒ��Ϣ*/


/*������־λ,��λΪ����������LED�ܿ�*/
typedef enum 
{
    TFUP_IDLE = 0,      /* ��δ���� */
    TFUP_RUNNING = 1,   /* ������ */
    TFUP_DONE = 2,      /* ������� */    
}TFUP_FLAG_E;

/*BOOTROM��־λ*/
typedef enum 
{
    VXWORKSFLAG = 0,
    BOOTROMFLAG
}TFUP_FLAG_BOOTROM;

//time_t  localtime;

#define DLOAD_ID "BALONGSPC5H"
#define HARDWARE_VER_INVALID    0xFFFFFFFF    /*��Ч��Ӳ���汾��*/

 /* ��ɫ��״̬���*/
enum LED_STATUS
{
    DRV_LED_LIGHT_SYSTEM_STARTUP,        /*�̵�˫��*/
    DRV_LED_LIGHT_POWER_ON,                   /*�̵�˫��*/
    DRV_LED_LIGHT_G_REGISTERED,             /*�̵Ƶ���*/
    DRV_LED_LIGHT_W_REGISTERED,            /*���Ƶ���*/
    DRV_LED_LIGHT_G_CONNECTED,              /*�̵Ƴ���*/
    DRV_LED_LIGHT_W_CONNNECTED,           /*���Ƴ���*/
    DRV_LED_LIGHT_H_CONNNECTED,            /*��Ƴ���*/
    DRV_LED_LIGHT_OFFLINE,                        /*�̵�˫��*/
    DRV_LED_LIGHT_FORCE_UPDATING,        /*������*//*�̵�Ƶ��*/
    DRV_LED_LIGHT_NORMAL_UPDATING,     /*������*//*�̵�Ƶ��*/
    DRV_LED_LIGHT_UPDATE_FAIL,               /*��������*//*�̵Ƴ���*/
    DRV_LED_LIGHT_UPDATE_SUCCESS,        /*TF������:�������*//*���Ƴ���*/  
    DRV_LED_LIGHT_UPDATE_FILEERROR,    /*TF������:�����ļ�����*//*���̽���˫��*/ 
    DRV_LED_LIGHT_UPDATE_NVFAIL,          /*NV�ָ�ʧ��*//*����Ƶ��*/    
    DRV_LED_LIGHT_SIM_ABSENT,                        /*���SIM��������SIM lock��*//*��Ƴ���*/
    DRV_LED_LIGHT_SHUTDOWN,                  /*����*/
    DRV_LED_LIGHT_STATE_MAX = 16
}; /*the state of LED */

//#define ISO_FILE_ADDR 0x5    /*ƹ��Bootrom���룬������BOOTROM_BACKö�٣�Ϊ����֮ǰ�Ĵ�����߶����ZSP�ļ�ͷ��ַ*/
#define MOBILE_CONNECT_HD_ADDR 0xA0000    /*�����ն��������̨����������߶����ISO�ļ�ͷ�ĵ�ַ*/
#define MOBILE_CONNECT_ADDR    0xB0000    /*�����ն��������̨����������߶����ISO�ļ��ĵ�ַ*/
#define WEBUI_HD_ADDR 0x550000    /*�����ն������WEBUI����������߶����WEBUI�ļ��汾�ĵ�ַ*/
#define WEBUI_ADDR    0x560000    /*�����ն������WEBUI����������߶����WEBUI�ļ��ĵ�ַ*/
#define IMAGE_MCORE_ADDR            0x550103    /*vxworks IDʹ�����ID���ԣ�ʵ�����ع��̸�λ����*/

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


/*˫��ͨ�����ݽṹ*/
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
    unsigned char  CompVer[31];         /* ���汾���� 30 �ַ�+ \0 */
}HISDUPDATE_VER_REG_MSG_STRU;

 typedef struct tagHISDUPDATE_VER_REV_STRU
{
    unsigned  char  dloadID[128];
    unsigned char  CompVer[31];         /* ���汾���� 30 �ַ�+ \0 */
}HISDUPDATE_VER_REV_STRU;

 
typedef struct tagHISDUPDATE_VER_IFC_MSG_STRU
{
    unsigned int u32ModuleId;
    unsigned int u32FuncId;
}HISDUPDATE_VER_IFC_MSG_STRU;


typedef struct device_event_st
{
    int device_id;   /*�豸ID*/
    int value;         /*��Ϣvalue*/ 
    char * desc;     /*����*/
}device_event_t;

/*�����ɹ���־λ*/
typedef enum 
{
    TFUPUNDOFLAG = 0,
    SUCCESSFLAG
}TFUP_FLAG_RESULT;

/*VERSIONINFO_I���ݽṹ�а汾�ַ��������Ч�ַ�����*/
#define VER_MAX_LENGTH                  30

typedef struct
{
    unsigned char CompId;              /* ����ţ��μ�COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH + 1];         /* ���汾���� 30 �ַ�+ \0 */
} VERSIONINFO;

#define READ_PATH "/mnt/sdcard"

int report_sd_state( DEVICE_ID dev, int nvalue, char *chr);
int balong_Ver_IFCInit(void);
int app_sd_get_state(void);
int hi_sdio_send_to_SDDriver(void *pstSDLDO10Arg, int nSize);
int hi_TFUP_SetSuccessFlag(int flag);/*����TF�����ɹ���־λ��A�˵���*/
int hi_TFUP_GetSuccessFlag(void);/*���TF�����ɹ���־λ��A�˵���*/

int  imageTypeChange(int* pType);
void hi_TFUP_setTFUpdateFlag(TFUP_FLAG_E flag);
int creat_NvResume(void);

