/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  DrvInterface.h
*
*   ��    �� :  yangzhi
*
*   ��    �� :  ���ļ�����Ϊ"DrvInterface.h", ����V7R1�����Э��ջ֮���API�ӿ�ͳ��
*
*   �޸ļ�¼ :  2011��1��18��  v1.00  yangzhi����
*************************************************************************/

#ifndef __DRV_INTERFACE_H__
#define __DRV_INTERFACE_H__

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)

#include "FeatureConfigDRV.h"
#include "FeatureConfig.h"

#ifndef _lint
#include "MemoryMap.h"
#endif

#else

#ifndef _lint
#include <asm/io.h>
#include <mach/hardware.h>
#include <generated/FeatureConfig.h>
#include <mach/drvLogDef.h>
#include <linux/scatterlist.h>
#include <generated/MemoryMap.h>
#endif
#endif


/*���ļ�Ĭ����4�ֽڶ��룬�ر���������lint�澯*/
/*lint -e958 -e959*/
#pragma pack(4)

/*************************GLOBAL BEGIN*****************************/
/* �����������Ͷ��� */
#ifndef _WIN32_COMPILE
typedef signed long long    BSP_S64;
#else
typedef double              BSP_S64;
#endif
typedef signed int          BSP_S32;
typedef signed short        BSP_S16;
typedef signed char         BSP_S8;
typedef char                BSP_CHAR;

#ifndef _WIN32_COMPILE
typedef unsigned long long  BSP_U64;
#else
typedef double              BSP_U64;
#endif
typedef unsigned int        BSP_U32;
typedef unsigned short      BSP_U16;
typedef unsigned char       BSP_U8;

typedef int                 BSP_BOOL;
typedef void                BSP_VOID;
typedef int                 BSP_STATUS;

#ifndef _WIN32_COMPILE
typedef signed long long*   BSP_PS64;
#else
typedef double*             BSP_PS64;
#endif
typedef signed int*         BSP_PS32;
typedef signed short*       BSP_PS16;
typedef signed char*        BSP_PS8;

#ifndef _WIN32_COMPILE
typedef unsigned long long* BSP_PU64;
#else
typedef double*             BSP_PU64;
#endif
typedef unsigned int*       BSP_PU32;
typedef unsigned short*     BSP_PU16;
typedef unsigned char*      BSP_PU8;

typedef unsigned char       UINT8;
typedef unsigned int        UINT32;
typedef int*                BSP_PBOOL;
typedef void*               BSP_PVOID;
typedef int*                BSP_PSTATUS;

typedef void                VOID;
typedef BSP_S32             STATUS;
typedef BSP_S32             UDI_HANDLE;

#ifndef BSP_CONST
#define BSP_CONST           const
#endif

#ifndef OK
#define OK                  (0)
#endif

#ifndef ERROR
#define ERROR               (-1)
#endif

#ifndef TRUE
#define TRUE                (1)
#endif

#ifndef FALSE
#define FALSE               (0)
#endif

#ifndef BSP_OK
#define BSP_OK              (0)
#endif

#ifndef BSP_ERROR
#define BSP_ERROR           (-1)
#endif

#ifndef BSP_TRUE
#define BSP_TRUE            (1)
#endif

#ifndef BSP_FALSE
#define BSP_FALSE           (0)
#endif

#ifndef BSP_NULL
#define BSP_NULL            (void*)0
#endif

#ifndef _LINUX_KERNEL_STAT_H  /* FUNCPTR defined in kernel_stat.h */
#ifndef FUNCPTR_IS_DEFINED
typedef BSP_S32 (*FUNCPTR)(void);
#define FUNCPTR_IS_DEFINED  1
#endif
#endif

typedef BSP_S32 (*FUNCPTR_1)(int);
typedef int (*PWRCTRLFUNCPTRVOID)(void);

#ifndef PWRCTRLFUNCPTR_D
typedef unsigned int (*PWRCTRLFUNCPTR)(unsigned int arg);     /* ptr to function returning int */
#define PWRCTRLFUNCPTR_D
#endif


#ifndef INLINE
#define INLINE              __inline__
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
typedef BSP_VOID (*VOIDFUNCPTR)();
#else
typedef BSP_VOID (*VOIDFUNCPTR)(BSP_U32);
#define SEM_FULL            (1)
#define SEM_EMPTY           (0)
#define LOCAL                   static
#define IVEC_TO_INUM(intVec)    ((int)(intVec))
#endif


/* ������궨�� */
#define BSP_ERR_MODULE_OFFSET (0x1000)    /* ��ֹ��ϵͳ�Ĵ������ص� */
#define BSP_DEF_ERR( mod, errid) \
    ((((BSP_U32) mod + BSP_ERR_MODULE_OFFSET) << 16) | (errid))

#define BSP_REG(base, reg) (*(volatile BSP_U32 *)((BSP_U32)base + (reg)))

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)
#define BSP_REG_READ(base, reg, result) \
    ((result) = BSP_REG(base, reg))

#define BSP_REG_WRITE(base, reg, data) \
    (BSP_REG(base, reg) = (data))

#else
#define BSP_REG_READ(base, reg, resule) \
    (resule = readl(base + reg))

#define BSP_REG_WRITE(base, reg, data) \
    (writel(data, (base + reg)))
#endif

/*****************************************************************************
  1 GU����������ӿڣ�GUPorting��Ŀ���.
*****************************************************************************/

/*****************************************************************************
  2 �궨��
*****************************************************************************/

#define DRV_OK                          (0)
#define DRV_ERROR                       (-1)
#define DRV_INTERFACE_RSLT_OK           (0)
#define BSP_BBP_OK   0x1
#define BSP_BBP_ERROR    0xffff

/* �����������Ͷ��� */
typedef void (*PVOIDFUNC)(void);

typedef int  (*pFUNCPTR)(void);

typedef unsigned long (*pFUNCPTR2)( unsigned long ulPara1, unsigned long ulPara2);

typedef unsigned int tagUDI_DEVICE_ID_UINT32;

/*��Ҫ�Ƶ����ͷ�ļ��� start*/
/*����Flash��Ϣ*/
typedef struct
{
    BSP_U32 ulblockCount;    /*Block����*/
    BSP_U32 ulpageSize;    /*pageҳ��С*/
    BSP_U32 ulpgCntPerBlk;    /*һ��Block�е�page����*/
}DLOAD_FLASH_STRU;
/*��Ҫ�Ƶ����ͷ�ļ��� end*/

/* ������·�հ�����ָ�� */
typedef BSP_VOID (*UpLinkRxFunc)(BSP_U8 *buf, BSP_U32 len);

/* ����װ�ͷź���ָ�� */
typedef BSP_VOID (*FreePktEncap)(BSP_VOID *PktEncap);

/* GMAC������ö�� */
typedef enum tagGMAC_OWNER_E
{
    GMAC_OWNER_VXWORKS = 0, /* Vxworks����Э��ջ */
    GMAC_OWNER_PS,          /* LTE����Э��ջ */
    GMAC_OWNER_MSP,         /* MSP */
    GMAC_OWNER_MAX          /* �߽�ֵ */
}GMAC_OWNER_E;

typedef enum tagWDT_TIMEOUT_E
{
    WDT_TIMEOUT_1   = 0,        /*0xFFFF000/WDT_CLK_FREQ,  about 3657ms*/      /*WDT_CLK_FREQ = ARM_FREQ/6 = 70M*/
    WDT_TIMEOUT_2,              /*0x1FFFE000/WDT_CLK_FREQ, about 7314ms*/
    WDT_TIMEOUT_3,              /*0x3FFFC000/WDT_CLK_FREQ, about 14628ms*/
    WDT_TIMEOUT_4,              /*0x7FFF8000/WDT_CLK_FREQ, about 29257ms*/
    WDT_TIMEOUT_BUTT
}WDT_TIMEOUT_E;

BSP_U8* BSP_GetMacAddr();
extern int ddmPhaseScoreBoot(const char * phaseName, unsigned int param);

/*************************GLOBAL END****************************/

/*************************VERSION START*****************************/
/* Ӳ����Ʒ��Ϣ���� */
#define HW_VER_INVALID              (BSP_U16)0xFFFF

/* Porting���P500����� */
#define HW_VER_PRODUCT_PORTING      (BSP_U16)0xFFFE
#define HW_VER_PRODUCT_PV500        (BSP_U16)0xFFFD

/* ���԰� */
#define HW_VER_PRODUCT_UDP          (BSP_U16)0xFFFC

/* Stick */
#define HW_VER_PRODUCT_E392S_U      (BSP_U16)HW_VER_INVALID
#define HW_VER_PRODUCT_E3276S_150   (BSP_U16)0x0201

/* E5 */
#define HW_VER_PRODUCT_E5_SBM       (BSP_U16)0x0001

/*E5776*/
#define HW_VER_PRODUCT_E5776_EM     (BSP_U16)0x0000

#define HW_VER_PRODUCT_E5_CMCC   	(BSP_U16)0x0003

/*E5371-DCM*/
#define HW_VER_PRODUCT_E5371_DCM     (BSP_U16)0x0100

/*E5372-32*/
#define HW_VER_PRODUCT_E5372_32      (BSP_U16)0x0103

/*E5375*/
#define HW_VER_PRODUCT_E5375          (BSP_U16)0x0104

/*E5375 �ڶ���*/
#define HW_VER_PRODUCT_E5375_SEC      (BSP_U16)0x0106

#define HW_VER_PRODUCT_E5775S_925     (BSP_U16)0x0107
#define HW_VER_PRODUCT_E5372_601      (BSP_U16)0x0109
#define HW_VER_PRODUCT_E5372TS_32     (BSP_U16)0x0903

#define HW_VER_PRODUCT_E5372S_22     (BSP_U16)0x0900
#define HW_VER_PRODUCT_R215      (BSP_U16)0x0901

/*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/
#define HW_VER_PRODUCT_E5_CMCC_CY   (BSP_U16)0x0007
/*END DST2012092001529 liangshukun 20121025 ADDED*/

/*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/
/* ��LNA�İ壬PCB��ʶ 3~5��ʾΪA~C */
#define HW_CMCC_NON_LNA_PCB_VER_START_VALUE     3

/* CMCC ����BOM��Ӳ����ʶ*/
#define HW_VER_PRODUCT_E5_CMCC_BOM_ERR  (BSP_U16)0x0603
/*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/

/*Epad, S10 101u */
#define HW_VER_PRODUCT_S10_101U     (BSP_U16)0x0401
#define HW_VER_PRODUCT_S10_101U_1   HW_VER_PRODUCT_S10_101U
#define HW_VER_PRODUCT_S10_101U_2   (BSP_U16)0x0402
#define HW_VER_PRODUCT_S10_101U_3   (BSP_U16)0x0403
#define HW_VER_PRODUCT_S10_101U_4   (BSP_U16)0x0404

/*Epad, S10 102u */
#define HW_VER_PRODUCT_S10_102U      (BSP_U16)0x0406
#define HW_VER_PRODUCT_S10_102U_1    HW_VER_PRODUCT_S10_102U
#define HW_VER_PRODUCT_S10_102U_2    (BSP_U16)0x0407
#define HW_VER_PRODUCT_S10_102U_3    (BSP_U16)0x0408
#define HW_VER_PRODUCT_S10_102U_4    (BSP_U16)0x0409

/*Epad, S10 LTE */
#define HW_VER_PRODUCT_S10_101L     (BSP_U16)0x0501
#define HW_VER_PRODUCT_S10_101L_1   HW_VER_PRODUCT_S10_101L
#define HW_VER_PRODUCT_S10_101L_2   (BSP_U16)0x0502
#define HW_VER_PRODUCT_S10_101L_3   (BSP_U16)0x0503
#define HW_VER_PRODUCT_S10_101L_4   (BSP_U16)0x0504

/*Epad, S10 Reserved HW ID*/
#define HW_VER_PRODUCT_S10_RESERVED_1   (BSP_U16)0x0506
#define HW_VER_PRODUCT_S10_RESERVED_2   (BSP_U16)0x0507
#define HW_VER_PRODUCT_S10_RESERVED_3   (BSP_U16)0x0508
#define HW_VER_PRODUCT_S10_RESERVED_4   (BSP_U16)0x0509

/*Epad, S10 Modem */
#define HW_VER_PRODUCT_S10_MODEM     (BSP_U16)0x0000
#define HW_VER_PRODUCT_S10_MODEM_1   (BSP_U16)0x0005
#define HW_VER_PRODUCT_S10_MODEM_2   HW_VER_PRODUCT_S10_MODEM
#define HW_VER_PRODUCT_S10_MODEM_3   (BSP_U16)0x0500

/*Epad, S10 Temp */
#define HW_VER_PRODUCT_S10_TEMP     (BSP_U16)0x0005

/*****************************************************************************
* �� �� ��  : BSP_HwGetVerMain
* ��������  : ��ȡ��Ʒ�汾��
* �������  : ��
* �������  : ��Ʒ�汾��
* �� �� ֵ  : 0����ȷ����0: ʧ��
* �޸ļ�¼  : 2011-3-30 wuzechun creat
*****************************************************************************/
BSP_U16 BSP_HwGetVerMain( BSP_VOID );

/*****************************************************************************
* �� �� ��  : BSP_GetProductName
* ��������  : ��ȡ��Ʒ����
* �������  : char* pProductName,�ַ���ָ�룬��֤��С��32�ֽ�
*             BSP_U32 ulLength,����������
* �������  : ��
* �� �� ֵ  : 0����ȷ����0: ʧ��
* �޸ļ�¼  : 2011-3-30 wuzechun creat
*****************************************************************************/
BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);

/*************************VERSION END  *****************************/


/*************************MMI START*****************************/

/***************************************************************
*  ������        :   BSP_MMI_TestResultSet
*  ��������      :   ���ӿ��ǽ�MMI�Ĳ��Ա�־д��NV
                     ��ulFlag == 1 ��д�� ��ST P��
                     ��ulFlag == 0 ��д�� ��ST F��
*  �������      :   BSP_U32 ulFlag
*  �������      :   ��
*  ����ֵ        :   �ɹ�(0)  ʧ��(-1)
***************************************************************/
int BSP_MMI_TestResultSet(unsigned int ulFlag);
#define DVR_MMI_TEST_RESULT_SET(ulFlag) BSP_MMI_TestResultSet(ulFlag)

/***************************************************************
*  ������        :   BSP_MMI_TestResultGet
*  ��������      :   ���ӿڱȽ�MMI NV�е��Ƿ��ǡ�ST P��
                                  �Ƿ���1   ���Ƿ���0
*  �������      :   ��
*  �������      :   ��
*  ����ֵ        :   �ɹ�(1)  ʧ��(0)
***************************************************************/
unsigned int BSP_MMI_TestResultGet(void);
#define DVR_MMI_TEST_RESULT_GET() BSP_MMI_TestResultGet()

/*************************MMI END*****************************/

/*************************OLED START**************************/

/*****************************************************************************
*  Function:  DRV_OLED_CLEAR_WHOLE_SCREEN
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern BSP_VOID DRV_OLED_CLEAR_WHOLE_SCREEN(BSP_VOID);

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_STATE_DISPLAY
*  Description: oled display right or not right  *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern  void DRV_OLED_UPDATE_STATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 �� �� ��  : DRV_OLED_UPDATE_DISPLAY
 ��������  : SD����Ƭ�汾������OLED��ʾ
 �������  : BOOL UpdateStatus
 �������  : �ޡ�
 ����ֵ��   ��
 ����:  �����ɹ���NV������ɺ���øú�����
        ����ʧ�ܺ���øú������
*****************************************************************************/
extern void DRV_OLED_UPDATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 �� �� ��  : DRV_OLED_STRING_DISPLAY
 ��������  : oled�ַ�����ʾ����
 �������  :
 *           Ҫ��ʾ�ַ�������ʼ����
 *           Ҫ��ʾ���ַ���
 �������  : none
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID DRV_OLED_STRING_DISPLAY(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*****************************************************************************
 �� �� ��  : DRV_OLED_POWER_OFF
 ��������  : oled�µ纯��
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID DRV_OLED_POWER_OFF(BSP_VOID);

/*************************OLED END******************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)

#else
/*************************LCD START*****************************/

typedef enum
{
    LCD_LIGHT = 3,
    LCD_SLEEP,
    LCD_DIM,
    LCD_RESET,
    LCD_UPDATE,
    LCD_CONTRAST,
    LCD_BRIGHTNESS,
    LCD_POWER_ON,
    LCD_POWER_OFF,
    LCD_DEL_TIMER,/*delete the power on animation timer*/
    LCD_SYS_POWER_DOWN,
    LCD_INIT_IMAGE,
    LCD_POWER_OFF_CHARGING_START,/*restart power off charging animation*/
    LCD_POWER_OFF_CHARGING_STOP,/*stop power off charging animation*/
    LCD_GET_FRAME_BUFFER,
    LCD_WRITE_IMAGE,
    LCD_ALL_WHITE,
    LCD_ALL_BLACK,
    LCD_CMD_MAX
}LCD_IOCTL_CMD;


/*****************************************************************************
 �� �� ��  : lcdRefresh
 ��������  : lcd��ָ��λ��ˢ������
 �������  :
 �������  :
*          UINT32 ulXStart, UINT32 ulYStart������ʼ����
*          UINT32 ulXOffset, UINT32 ulYOffset������������ƫ����
*          UINT8 *pucBuffer������ʾ����
 �� �� ֵ  : void
*****************************************************************************/
extern int lcdRefresh(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,const unsigned char * test_buf);

/*****************************************************************************
 �� �� ��  : balong_lcd_ioctl
 ��������  : lcd ioctl
 �������  :
*            int cmd --command ID
*            arg--para
 �������  : none
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 balong_lcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

extern BSP_VOID lcdClearWholeScreen(BSP_VOID);
extern BSP_VOID lcdStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*************************LCD END*******************************/
#endif



/*************************SCI BEGIN*****************************/

/*  SIM  state*/
typedef enum tagSCI_CARD_STATE_E
{
    SCI_CARD_STATE_READY = 0,           	/* Ready */
    SCI_CARD_STATE_NOCARD,                	/* No card */
    SCI_CARD_STATE_BUSY,               		/* In initialization*/
	SCI_CARD_STATE_BUTT,               		/* Butt*/
} SCI_CARD_STATE_E;

/*****************************************************************************
* �� �� ��  : BSP_SCI_Reset
*
* ��������  : ���ӿ����ڸ�λSCI��Smart Card Interface��������USIM��Universal
*           Subscriber Identity Module����
*
* �������  : BSP_VOID
* �������  : ��
*
* �� �� ֵ  : OK  ��λ�ɹ�
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_Reset(BSP_VOID);
#define DRV_USIMMSCI_RST()    BSP_SCI_Reset()

/********************************************************************************************************************
 �� �� ��  : BSP_GUSCI_GetCardStatus
 ��������  : ���ӿ����ڻ�ÿ���ǰ��״̬��ĿǰLTE�ṩ�ӿڲ������Ͳ�һ��
 �������  : �ޡ�
 �������  :
 �� �� ֵ  : 0:  ������Ready��
             -1����δ����Ready��
 ע������  ����Ч����BSP_SCI_Reset()�������ٵ��ô˺�����
********************************************************************************************************************/
extern int BSP_GUSCI_GetCardStatus(BSP_VOID);
#define DRV_USIMMSCI_GET_CARD_STAU()    BSP_GUSCI_GetCardStatus()

/*****************************************************************************
* �� �� ��  : DRV_USIMMSCI_SND_DATA
*
* ��������  : ���ӿ����ڷ���һ�����ݵ�USIM��
*
* �������  : BSP_U32 u32DataLength �������ݵ���Ч���ȡ�ȡֵ��Χ1��256����λΪ�ֽ�
*             BSP_U8 *pu8DataBuffer �������������ڴ���׵�ַ�����Ƕ�̬���䣬���ýӿ�
*                                   ���������ͷţ�������Ӳ����������
* �������  : ��
*
* �� �� ֵ  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_SND_DATA(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer);

/*****************************************************************************
* �� �� ��  : DRV_USIMMSCI_RCV
*
* ��������  : ���ӿ�����USIM Manager��ȡ����SCI Driver�Ŀ���������
*             �ýӿ�Ϊ�����ӿڣ�ֻ��SCI���յ��㹻����������Ż᷵�أ�
*             �ýӿڵĳ�ʱ����Ϊ1s
*
* �������  : BSP_U32 u32DataLength USIM Manager����SCI Driver��ȡ�����ݳ��ȡ�
* �������  : BSP_U8 *pu8DataBuffer USIM Managerָ����Buffer��SCI Driver�����ݿ�������Buffer��
* �� �� ֵ  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_RCV(BSP_U32 u32Length,BSP_U8 *pu8Data);

/*****************************************************************************
* �� �� ��  : DRV_USIMMSCI_GET_ATR
*
* ��������  : ���ӿ����ڽ�Driver�㻺���ATR���ݺ����ݸ������ظ�USIM Manager��
*
* �������  : BSP_VOID
* �������  : unsigned long *u8DataLength  Driver��ȡ��ATR���ݳ��ȣ����ظ�USIM Manager��
*                                   ȡֵ��Χ0��32����λ���ֽ�
*           BSP_U8 *pu8ATR          USIM Managerָ����Buffer��SCI Driver��ATR
*                                   ���ݿ�������Buffer��һ��Ϊ����ϵͳ������̬����
*                                   ���߾�̬����ĵ�ַ
*
*
* �� �� ֵ  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_GET_ATR (unsigned long *u32DataLength, BSP_U8 *pu8ATR);

/*****************************************************************************
* �� �� ��  : DRV_USIMMSCI_CLASS_SWITCH
*
* ��������  : ���ӿ�����֧��PS�Կ��ĵ�ѹ���ͽ����л�����1.8V�л���3V
*
* �������  : BSP_VOID
* �������  : ��
*
* �� �� ֵ  :  OK    ��ǰ��ѹ������ߵģ����е�ѹ�л�����
*           BSP_ERR_SCI_CURRENT_STATE_ERR �л�ʧ�� current SCI driver state is ready/rx/tx
*           BSP_ERR_SCI_VLTG_HIGHEST   ��ǰ��ѹ�Ѿ�����ߵ�ѹ��û�н��е�ѹ�л�
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR ��ǰ��ѹֵ�쳣����class B����C��
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_CLASS_SWITCH(BSP_VOID);

/*****************************************************************************
* �� �� ��  : DRV_USIMMSCI_TM_STOP
*
* ��������  : ���ӿ�����֧��PS�ر�SIM��ʱ��
*
* �������  :
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg ʱ��ֹͣģʽ
*
* �������  : ��
*
* �� �� ֵ  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_VOID DRV_USIMMSCI_TM_STOP(BSP_U32 ulStopType);

/*****************************************************************************
* �� �� ��  : BSP_SCI_Deactive
*
* ��������  : ���ӿ����ڶ�SIM����ȥ�������
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : OK
*             BSP_ERR_SCI_NOTINIT
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_Deactive(BSP_VOID);
#define DRV_USIMMSCI_DEACT()    BSP_SCI_Deactive()

/*****************************************************************************
* �� �� ��  : BSP_SCI_GetClkStatus
*
* ��������  : ���ӿ����ڻ�ȡ��ǰSIM��ʱ��״̬
*
* �������  : BSP_U32 *pu32SciClkStatus   ����ָ�룬���ڷ���SIM��ʱ��״̬��
*                                       0��ʱ���Ѵ򿪣�
*                                       1��ʱ��ֹͣ
* �������  : ��
*
* �� �� ֵ  : OK    �����ɹ�
*          BSP_ERR_SCI_INVALIDPARA
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetClkStatus(BSP_U32 *pu32SciClkStatus);
#define DRV_USIMMSCI_GET_CLK_STAU(pulSciClkStatus)    BSP_SCI_GetClkStatus(pulSciClkStatus)

/*****************************************************************************
* �� �� ��  : BSP_SCI_GetClkFreq
*
* ��������  : ���ӿ����ڻ�ȡ��ǰSIM��ʱ��Ƶ��
*
* �������  : ��
*
* �������  : unsigned long *pLen   ʱ��Ƶ�����ݵĳ���
*             BSP_U8 *pBuf    ʱ��Ƶ������
* �� �� ֵ  : OK    �����ɹ�
*             BSP_ERR_SCI_INVALIDPARA
* �޸ļ�¼  : 2010��8��24��   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetClkFreq(unsigned long *pLen, BSP_U8 *pBuf);
#define DRV_PCSC_GET_CLK_FREQ(pDataLen, Buffer)    BSP_SCI_GetClkFreq(pDataLen, Buffer)

/*****************************************************************************
* �� �� ��  : BSP_SCI_GetBaudRate
*
* ��������  : ���ӿ����ڻ�ȡ��ǰSIM��ʱ��Ƶ��
*
* �������  : ��
*
* �������  : unsigned long *pLen   ���������ݵĳ���
*             BSP_U8 *pBuf    ����������
*
* �� �� ֵ  : OK    �����ɹ�
*             BSP_ERR_SCI_INVALIDPARA
* �޸ļ�¼  : 2010��8��24��   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetBaudRate(unsigned long *pLen, BSP_U8 *pBuf);
#define DRV_PCSC_GET_BAUD_RATE(pDataLen, Buffer)    BSP_SCI_GetBaudRate(pDataLen, Buffer)

/*****************************************************************************
* �� �� ��  : BSP_SCI_GetPCSCParameter
*
* ��������  : ���ӿ����ڻ�ȡ��ǰSIM����PCSC��ز���
*
* �������  : ��
*
* �������  : BSP_U8 *pBuf    PCSC��ز���
*
* �� �� ֵ  : OK    �����ɹ�
*             BSP_ERR_SCI_INVALIDPARA
* �޸ļ�¼  : 2010��8��24��   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetPCSCParameter(BSP_U8 *pBuf);
#define DRV_PCSC_GET_SCI_PARA(Para)    BSP_SCI_GetPCSCParameter(Para)

/*****************************************************************************
 �� �� ��  : sciRecordDataSave
 ��������  : ���ӿ����ڻ�ȡ��ǰSIM���������ݡ������쳣����ģ�飬V7�ȴ�׮
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܣ�ָ�����Ϊ�ա�
 ע������  ��
*****************************************************************************/
/*extern int sciRecordDataSave(void);*/
extern int BSP_USIMMSCI_STUB(void);
#define DRV_USIMMSCI_RECORD_DATA_SAVE()        BSP_USIMMSCI_STUB()

typedef enum
{
    SIM_CARD_OUT = 0,
    SIM_CARD_IN  = 1
} SCI_DETECT_STATE;

typedef void (*OMSCIFUNCPTR)(unsigned int ulVal);

/*****************************************************************************
* �� �� ��  : BSP_SCIFuncRegister
*
* ��������  : ���ӿ�����ע��OAM�Ļص�����
*
* �������  : omSciFuncPtr
* �������  : ��
*
* �� �� ֵ  : NA
*
*
*****************************************************************************/
void BSP_SCIFuncRegister(OMSCIFUNCPTR omSciFuncPtr);
#define DRV_USIMMSCI_FUNC_REGISTER(omSciFuncPtr)    BSP_SCIFuncRegister(omSciFuncPtr)

/*************************SCI END*************************************/

/*************************PMU BEGIN***********************************/

/*****************************************************************************
 �� �� ��  : DRV_GET_PMU_STATE
 ��������  : ��ȡPMUģ�鿪����ʽ�����״̬����ص����������λ״̬��
 �������  : �ޡ�
 �������  : Pmu_State :������ʽ�����״̬����ص����������λ״̬��
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע��������ýӿڽ���PS��PC������ʹ�ã�Ŀǰû��Ӧ�ã��ݱ�����
*****************************************************************************/
extern unsigned int DRV_GET_PMU_STATE(void*  Pmu_State);

/*****************************************************************************
 �� �� ��  : BSP_PMU_UsbEndRegActionFunc
 ��������  : ���ӿ���USB����/�γ��ص�ע�ắ����
 �������  : srcFunc��ע���USB�����γ���������ʱ�Ļص�����ָ�롣
             actionIndex������ָʾ��
                          0������srcFunc��ע���USB���붯���Ļص�������
                          1������srcFunc��ע���USB�γ������Ļص�������

 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע������  �����ô˽ӿڣ�����Ҫִ�еĺ���ָ�봫�룬����USB�����γ�ʱ������Ҫִ�еĺ�����
*****************************************************************************/
extern int BSP_PMU_UsbEndRegActionFunc(pFUNCPTR srcFunc, unsigned char actionIndex);
#define DRV_USB_ENDREGACTION_FUNC(srcFunc,actionIndex) \
                BSP_PMU_UsbEndRegActionFunc(srcFunc,actionIndex)


/*****************************************************************************
��������   BSP_USB_PortTypeValidCheck
��������:  �ṩ���ϲ��ѯ�豸�˿���̬���úϷ��Խӿ�
           1���˿�Ϊ��֧�����ͣ�2������PCUI�ڣ�3�����ظ��˿ڣ�4���˵���������16��
           5����һ���豸��ΪMASS��
��������� pucPortType  �˿���̬����
           ulPortNum    �˿���̬����
����ֵ��   0:    �˿���̬�Ϸ�
           �������˿���̬�Ƿ�
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
��������   BSP_USB_GetAvailabePortType
��������:  �ṩ���ϲ��ѯ��ǰ�豸֧�ֶ˿���̬�б�ӿ�
��������� ulPortMax    Э��ջ֧�����˿���̬����
�������:  pucPortType  ֧�ֵĶ˿���̬�б�
           pulPortNum   ֧�ֵĶ˿���̬����
����ֵ��   0:    ��ȡ�˿���̬�б�ɹ�
           ��������ȡ�˿���̬�б�ʧ��
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)

/*************************PMU END*************************************/

/*************************INT BEGIN***********************************/

/*****************************************************************************
* �� �� ��  : BSP_INT_Enable
*
* ��������  : ʹ��ĳ���ж�
*
* �������  : INT32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��40
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
*****************************************************************************/
BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl);
#define DRV_VICINT_ENABLE(ulLvl)    BSP_INT_Enable(ulLvl)

/*****************************************************************************
 * �� �� ��  : BSP_INT_Disable
 *
 * ��������  : ȥʹ��ĳ���ж�
 *
 * �������  : INT32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��40
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Disable ( BSP_S32 s32Lvl);
#define  DRV_VICINT_DISABLE(ulLvl)    BSP_INT_Disable(ulLvl)

/*****************************************************************************
 * �� �� ��  : BSP_INT_Connect
 *
 * ��������  : ע��ĳ���ж�
 *
 * �������  : VOIDFUNCPTR * vector �ж������ţ�ȡֵ��Χ0��40
 *           VOIDFUNCPTR routine  �жϷ������
 *           INT32 parameter      �жϷ���������
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Connect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter);
#define DRV_VICINT_CONNECT(vector,routine,parameter)    BSP_INT_Connect(vector,routine,parameter)

/*************************INT END*************************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)

/*************************WDT BEGIN***********************************/

/*****************************************************************************
* �� �� ��  : BSP_WDT_Init
*
* ��������  : ��ʼ�����ƿ��Ź������ó�ʱʱ�䣬�ҽ��жϷ�����
*
* �������  : wdtId         ���Ź�ID,оƬ�ṩ1�����Ź���ID����Ϊ0
*             wdtTimeOuts   ���Ź���ʱʱ�䣬��λ��,ʱ������ܳ���51��
*
* �������  : ��
*
* �� �� ֵ  : OK& ERROR
*
* �޸ļ�¼ :  2009��3��5��  v1.00  wangxuesong  ����
*****************************************************************************/
BSP_S32 BSP_WDT_Init(BSP_U8 wdtId , BSP_U32 wdtTimeOuts);
#define DRV_WDT_INIT(wdtId,wdtTimeOutms)      BSP_WDT_Init(wdtId,wdtTimeOutms)

/*****************************************************************************
* �� �� ��  : BSP_WDT_EnableWdt
*
* ��������  : �������Ź�
*
* �������  : wdtId  ���Ź�ID
*
* �������  : ��
*
* �� �� ֵ  : OK& ERROR
*
* �޸ļ�¼ :  2009��3��5��  v1.00  wangxuesong  ����
*****************************************************************************/
BSP_S32 BSP_WDT_Enable(BSP_U8 wdtId);
#define DRV_WDT_ENABLE(wdtId)    BSP_WDT_Enable(wdtId)

/*****************************************************************************
* �� �� ��  : BSP_WDT_Disable
*
* ��������  : �رտ��Ź�
*
* �������  : wdtId  ���Ź�ID
*
* �������  : ��
*
* �� �� ֵ  : OK& ERROR
*
* �޸ļ�¼ :  2009��3��5��  v1.00  wangxuesong  ����
*****************************************************************************/
BSP_S32 BSP_WDT_Disable(BSP_U8 wdtId);
#define DRV_WDT_DISABLE(wdtId)    BSP_WDT_Disable(wdtId)
#endif

/*****************************************************************************
* �� �� ��  : BSP_WDT_HardwareFeed
*
* ��������  : ���ÿ��Ź������Ĵ�����ι����
*
* �������  : wdtId  ���Ź�ID
*
* �������  : ��
*
* �� �� ֵ  : OK& ERROR
*
* �޸ļ�¼ :  2009��3��5��  v1.00  wangxuesong  ����
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed(BSP_U8 wdtId);
#define DRV_WDT_FEED(wdtId)    BSP_WDT_HardwareFeed(wdtId)

/*************************WDT END*************************************/

/*************************SD BEGIN************************************/
#define WRFlAG     1
#define RDFlAG     0

typedef enum
{
    TFUP_FAIL = 0,  /*���*/
    TFUP_OK         /*��*/
}TFUP_DISPLAY_TYPE;

enum SD_MMC_OPRT_ENUM
{
    SD_MMC_OPRT_FORMAT = 0,
    SD_MMC_OPRT_ERASE_ALL,
    SD_MMC_OPRT_ERASE_SPEC_ADDR,
    SD_MMC_OPRT_WRITE,
    SD_MMC_OPRT_READ,
    SD_MMC_OPRT_BUTT
};

typedef unsigned int SD_MMC_OPRT_ENUM_UINT32;


/*SD���豸����*/
typedef struct _sd_dev_type_str
{
    unsigned long   devNameLen;
    char            devName[16];
}SD_DEV_TYPE_STR;

/*****************************************************************************
 �� �� ��  : BSP_SDMMC_ATProcess
 ��������  : at^sd,SD��������д����������ʽ������
 �������  : �������� ulOp:
            0  ��ʽ��SD��
            1  ��������SD�����ݣ�
            2  ����ָ����ַ���ݵĲ���������ָ�����������ݳ���Ϊ512�ֽڡ�������ĵ�ַ��дȫ1
            3  д���ݵ�SD����ָ����ַ�У���Ҫ���ڶ����͵���������
            4  ��ȡulAddrָ���ĵ�ַ(ulAddr*512)��512���ֽڵ����ݵ�pucBuffer��

            ulAddr < address >  ��ַ����512BYTEΪһ����λ��������n��ʾ

            ulData
             < data >            �������ݣ���ʾ512BYTE�����ݣ�ÿ���ֽڵ����ݾ���ͬ��
             0       �ֽ�����Ϊ0x00
             1       �ֽ�����Ϊ0x55
             2       �ֽ�����Ϊ0xAA
             3       �ֽ�����Ϊ0xFF

 �������  : pulErr
 �� �� ֵ  : 0 ��OK  �� 0 ��Error

            ����Ĵ���ֵ�����*pulErr��
            0 ��ʾSD������λ
            1 ��ʾSD����ʼ��ʧ��
            2 ��ʾ<opr>�����Ƿ�����Ӧ������֧��(�ô�����ATʹ��,����Ҫ����ʹ��)
            3 ��ʾ<address>��ַ�Ƿ�������SD����������
            4 ����δ֪����
*****************************************************************************/
extern  unsigned long  BSP_SDMMC_ATProcess(SD_MMC_OPRT_ENUM_UINT32 ulOp,
                unsigned long ulAddr,  unsigned long ulData,unsigned char *pucBuffer,unsigned long *pulErr);
#define DRV_SDMMC_AT_PROCESS(ulOp,ulAddr,ulData,pucBuffer,pulErr)   \
                     BSP_SDMMC_ATProcess (ulOp,ulAddr,ulData,pucBuffer,pulErr)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
/*****************************************************************************
* �� �� ��  : sd_mmc_blk_w
*
* ��������  :д���ӿ�
*
* �������  : u32StartBlk ��ʼ��
				  pu8DataBuf  д���ݻ���
				  u32Len д���ݴ�С������Ϊ512�ֽڵ�������
* �������  : NA
*
* �� �� ֵ  :  �ɹ�����0
*
* ����˵��  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_w(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len);
/*****************************************************************************
* �� �� ��  : sd_mmc_blk_r
*
* ��������  : �����ӿ�
*
* �������  : u32StartBlk ��ʼ��
				  pu8DataBuf  �����ݻ���
				  u32Len  �����ݴ�С������Ϊ512�ֽڵ�������
* �������  : NA
*
* �� �� ֵ  :  �ɹ�����0
*
* ����˵��  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_r(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len);
/*****************************************************************************
* �� �� ��  : sd_mmc_blk_erase
*
* ��������  : ��������ӿ�
*
* �������  : u32StartBlk ��ʼ��
				  u32EndBlk  ��ֹ��
* �������  : NA
*
* �� �� ֵ  :  �ɹ�����0
*
* ����˵��  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_erase(BSP_U32 u32StartBlk,BSP_U32 u32EndBlk);
/*****************************************************************************
* �� �� ��  : sd_mmc_get_status
*
* ��������  : ����λ��ѯ
*
* �������  : void
* �������  : NA
*
* �� �� ֵ  : 0 : ��λ��-1: ����λ
*
* ����˵��  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_get_status(BSP_VOID);
#endif

/*****************************************************************************
 �� �� ��  : BSP_SDMMC_GetOprtStatus
 ��������  : at^sd,SD����ǰ����״̬
 �������  : ��
 �������  : ��
 �� �� ֵ  :
            0: δ����������Ѿ����;
            1: ������
            2: �ϴβ���ʧ��
*****************************************************************************/
extern unsigned long BSP_SDMMC_GetOprtStatus(void);
#define DRV_SDMMC_GET_OPRT_STATUS()    BSP_SDMMC_GetOprtStatus()

/*****************************************************************************
 �� �� ��  : BSP_SDMMC_AddHook
 ��������  : TFģ�鹳�Ӻ���ע�ᡣ
 �������  : hookType���ص��������ͣ�
                       0����������ļ���Ϣ����õĻص�������
                       1��TF������֪ͨOM�Ļص�������
                       2��TF���γ�֪ͨOM�Ļص�������
                       3�� TF����ʼ����֪ͨU�̵Ļص�������
             p���ص�����ָ�롣
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע������  ������ͬһ���ͻص��������ظ�ע��Ḳ����ǰ���á�
*****************************************************************************/
//#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int sdmmcAddHook (int hookType , void * p);/*y00186965 for sd_update*/
#define DRV_SDMMC_ADD_HOOK(hookType,p)    sdmmcAddHook(hookType,p)
//#else
//extern int BSP_SDMMC_AddHook(int hookType, void * p);
//#define DRV_SDMMC_ADD_HOOK(hookType,p)    BSP_SDMMC_AddHook(hookType,p)
//#endif

/*****************************************************************************
 �� �� ��  : BSP_SDMMC_UsbGetStatus
 ��������  : ����SD��״̬
 �������  : �ޡ�
 �������  : �ޡ�
 ����ֵ��   0  Ϊ����
           ��0 ������

*****************************************************************************/
extern unsigned int BSP_SDMMC_UsbGetStatus(void);
#define DRV_SDMMC_USB_STATUS()    BSP_SDMMC_UsbGetStatus();

/*****************************************************************************
* �� �� ��  : sdmmc_ClearWholeScreen
* ��������  : SD������NV�ָ������ʾ������
* �������  : ��
* �������  : ��
* �� �� ֵ  :��
* ����˵��  : ��
*****************************************************************************/
extern void sdmmc_ClearWholeScreen(void);
#define DRV_SDMMC_CLEAR_WHOLE_SCREEN()    sdmmc_ClearWholeScreen()

/*****************************************************************************
* �� �� ��  : sdmmc_UpdateDisplay
* ��������  : SD������NV�ָ������ʾ����ʾ
* �������  : BOOL UpdateStatus
* �������  : ��
* �� �� ֵ  :��
* ����˵��  : �����ɹ���NV������ɺ���øú�����
              ����ʧ�ܺ���øú������
*****************************************************************************/
extern void sdmmc_UpdateDisplay(int UpdateStatus);
#define DRV_SDMMC_UPDATE_DISPLAY(state)    sdmmc_UpdateDisplay(state)

/*****************************************************************************
* �� �� ��  : DRV_SD_GET_STATUS
*
* ��������  : ����λ��ѯ
*
* �������  : void
* �������  : NA
*
* �� �� ֵ  : 0 : ��λ��-1: ����λ
*
* ����˵��  : NA
*
*****************************************************************************/
extern int DRV_SD_GET_STATUS(void);

/*****************************************************************************
* �� �� ��  : DRV_SD_GET_CAPACITY
*
* ��������  : ��������ѯ
*
* �������  : void
* �������  : NA
*
* �� �� ֵ  : 0 : ʧ�ܣ�>0: ������
*
* ����˵��  : NA
*
*****************************************************************************/
extern int DRV_SD_GET_CAPACITY(void);

/*****************************************************************************
* �� �� ��  : DRV_SD_TRANSFER
*
* ��������  : ���ݴ���
*
* �������  : struct scatterlist *sg    ���������ݽṹ��ָ��
                            unsigned dev_addr   ��д���SD block ��ַ
                            unsigned blocks    ��д���block����
                            unsigned blksz      ÿ��block�Ĵ�С����λ�ֽ�
                            int wrflags    ��д��־λ��д:WRFlAG ; ��:RDFlAG
* �������  : NA
*
* �� �� ֵ  : 0 : �ɹ�������: ʧ��
*
* ����˵��  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_TRANSFER(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags);
#endif
/*****************************************************************************
* �� �� ��  : DRV_SD_SG_INIT_TABLE
*
* ��������  : SD������ݴ���sg list��ʼ��
*
* �������  : const void *buf		��������buffer��ַ
				  unsigned int buflen	��������buffer��С��С��32K, ��СΪ512B��������
				  					����32K, ��СΪ32KB�������������bufferΪ128K
* �������  : NA
*
* �� �� ֵ  :  0 : �ɹ�;  ����:ʧ��
* ����˵��  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen);
#endif
/*****************************************************************************
* �� �� ��  : DRV_SD_MULTI_TRANSFER
*
* ��������  : SD������ݴ���
*
* �������  : unsigned dev_addr	��д���SD block��ַ
				  unsigned blocks		��д���block ����
				  unsigned blksz		ÿ��block �Ĵ�С����λ�ֽ�
				  int write			��д��־λ��д:1;	��:0
* �������  : NA
*
* �� �� ֵ  :  0 : �ɹ�;  ����:ʧ��
* ����˵��  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_MULTI_TRANSFER(unsigned dev_addr,unsigned blocks,unsigned blksz,int write);
#endif
/*************************SD END**************************************/


/*************************DLOAD BEGIN*****************************/
/*--------------------------------------------------------------*
 * �궨��                                                       *
 *--------------------------------------------------------------*/
/* ������ */
#define DLOAD_OK                    BSP_OK
#define DLOAD_ERROR                 BSP_ERROR
#define DLOAD_ERR_NOT_INITED        BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_MODULE_NOT_INITED)
#define DLOAD_ERR_NULL_PTR          BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_NULL_PTR)
#define DLOAD_ERR_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_INVALID_PARA)
#define DLOAD_ERR_ALLOC_FAILED      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_ALLOC_FAILED)
#define DLOAD_ERR_FREE_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_FREE_FAILED)
#define DLOAD_ERR_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_RETRY_TIMEOUT)

#define DLOAD_ERR_NO_BACKUP         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 1)  /* �ޱ��ݰ汾 */
#define DLOAD_ERR_OPEN_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 2)  /* ���⴮�ڴ�ʧ�� */
#define DLOAD_ERR_SEM_CREAT         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 3)  /* �ź�������ʧ�� */
#define DLOAD_ERR_ABORT             BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 4)  /* ������ֹ */
#define DLOAD_ERR_MODE_MISMATCH     BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 5)  /* ����ģʽ��ƥ�� */
#define DLOAD_ERR_INVALID_ATSTRING  BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 6)  /* AT�ַ����Ƿ� */


#define MODE_GSM      0x01
#define MODE_CDMA     0x02
#define MODE_WCDMA    0x04


/*--------------------------------------------------------------*
 * ö�ٶ���                                                     *
 *--------------------------------------------------------------*/
enum UPDATE_STATUS_I
{
	DL_UPDATE = 0,
	DL_NOT_UPDATE = 1
};

typedef enum
{
	NORMAL_DLOAD = 0,	/*��������ģʽ*/
	FORCE_DLOAD = 1    /*ǿ������ģʽ*/
}DLOAD_TYPE;

/* ����ģʽö�� */
typedef enum tagDLOAD_MODE_E
{
    DLOAD_MODE_DOWNLOAD = 0,
    DLOAD_MODE_NORMAL,
    DLOAD_MODE_DATA,
    DLOAD_MODE_MAX
}DLOAD_MODE_E;

/*--------------------------------------------------------------*
 * ����ָ�����Ͷ���                                             *
 *--------------------------------------------------------------*/
typedef BSP_U32 (*BSP_DLOAD_AtCallBack)( BSP_VOID* pDataIn, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVGetInfoCb)( BSP_VOID* pDataOut, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVSetInfoCb)( BSP_VOID* pDataIn, BSP_U32 ulLen);

/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetProductId
*
* ��������  : ��ȡ��Ʒ�����ַ���
*
* �������  : BSP_S8 *str   :�ַ���������
*             BSP_S32 len   :�ַ�������
* �������  : BSP_S8 *str   :�ַ���������
*
* �� �� ֵ  : DLOAD_ERR_INVALID_PARA    :��������Ƿ�
*             DLOAD_ERROR               :ʧ��
*             ����                      :�����ַ�������
*
* ����˵��  : ATģ�����
*             ����ģʽ֧��
*             ����ģʽ֧��
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetProductId(BSP_CHAR *str, BSP_U32 len);
/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetDloadType
*
* ��������  : ��ȡ��������
*
* �������  : BSP_VOID
* �������  : ��
*
* �� �� ֵ  : 0 :����ģʽ
*             1 :ǿ��ģʽ
*
* ����˵��  : ATģ�����
*             ����ģʽ֧��
*             ����ģʽ֧��
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadType(BSP_VOID);
/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetCurMode
*
* ��������  : ���õ�ǰ����ģʽ
*
* �������  : BSP_VOID
* �������  : DLOAD_MODE_NORMAL     :����ģʽ
*             DLOAD_MODE_DATA       :����ģʽ
*             DLOAD_MODE_DOWNLOAD   :����ģʽ
*
* �� �� ֵ  : ��
*
* ����˵��  : ATģ�����
*             ����ģʽ֧�ֵ���:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DOWNLOAD)
*             ����ģʽ֧�ֵ���:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DATA)
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCurMode(DLOAD_MODE_E eDloadMode);
/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetCurMode
*
* ��������  : ��ȡ��ǰ����ģʽ
*
* �������  : BSP_VOID
* �������  : ��
*
* �� �� ֵ  : DLOAD_MODE_NORMAL     :����ģʽ
*             DLOAD_MODE_DATA       :����ģʽ
*             DLOAD_MODE_DOWNLOAD   :����ģʽ
*
* ����˵��  : ��
*
*****************************************************************************/
DLOAD_MODE_E BSP_DLOAD_GetCurMode(BSP_VOID);
/*****************************************************************************
* �� �� ��  : BSP_DLOAD_SetSoftLoad
*
* ��������  : ����������־
*
* �������  : BSP_BOOL bSoftLoad  :
*             BSP_FALSE :��vxWorks����
*             BSP_TRUE  :��bootrom����
* �������  : ��
*
* �� �� ֵ  : ��
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetSoftLoad (BSP_BOOL bSoftLoad);


/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetSoftLoad
*
* ��������  : ��ȡ������־
*
* �������  : BSP_VOID
* �������  : ��
*
* �� �� ֵ  : BSP_TRUE  :��bootrom����
*             BSP_FALSE :��vxWorks����
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetSoftLoad (BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_DLOAD_SendData
*
* ��������  : ͨ�����⴮����PC�˷�������
*
* �������  : pBuf      :���ݻ�����
*             u32Len    :���ݻ���������
* �������  : pBuf      :���ݻ�����
*
* �� �� ֵ  : ��
*
* ����˵��  : pBuf���뱣֤cache line(32�ֽ�)����
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_SendData(BSP_CHAR *pBuf, BSP_U32 u32Len);
/*****************************************************************************
* �� �� ��  : BSP_DLOAD_AtProcReg
*
* ��������  : ע��AT������
*
* �������  : pFun
* �������  : ��
*
* �� �� ֵ  : DLOAD_OK:�ɹ�
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_AtProcReg (BSP_DLOAD_AtCallBack pFun);

/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetSoftwareVer
*
* ��������  : ��ȡ����汾��
*
* �������  : BSP_S8 *str   :�ַ���������
*             BSP_S32 len   :�ַ�������
* �������  : BSP_S8 *str   :�ַ���������
*
* �� �� ֵ  : DLOAD_ERROR   :��������Ƿ�
*             ����          :�����ַ�������
*
* ����˵��  : ATģ�����
*             ����ģʽ֧��
*             ����ģʽ֧��
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetSoftwareVer(BSP_CHAR *str, BSP_U32 len);


/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetDloadVer
*
* ��������  : ��ѯ����Э��汾�š�����ϢBSP�̶�дΪ2.0��
*
* �������  : BSP_S8 *str   :�ַ���������
*             BSP_S32 len   :�ַ�������
* �������  : BSP_S8 *str   :�ַ���������
*
* �� �� ֵ  : DLOAD_ERROR   :��������Ƿ�
*             ����          :�����ַ�������
*
* ����˵��  : ATģ�����
*             ����ģʽ֧��
*             ����ģʽ֧��
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadVer(BSP_CHAR *str, BSP_U32 len);

/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetISOVer
*
* ��������  : ��ȡ��̨�汾���ַ���
*
* �������  : BSP_S8 *str   :�ַ���������
*             BSP_S32 len   :�ַ�������
* �������  : BSP_S8 *str   :�ַ���������
*
* �� �� ֵ  : DLOAD_ERROR   :��������Ƿ�
*             ����          :�����ַ�������
*
* ����˵��  : ATģ�����
*             ����ģʽ֧��
*             ����ģʽ֧��
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetISOVer(BSP_CHAR *str, BSP_U32 len);
#define DRV_GET_CDROM_VERSION(pVersionInfo,ulLength)    BSP_DLOAD_GetISOVer(pVersionInfo, ulLength)

/*****************************************************************************
 �� �� ��  : DRV_SET_UPDATA_FLAG
 ��������  : �����������ر�־��
 �������  : flag���������ر�־��
                   0�����������bootrom�������������ء�
                   1�������󲻽���bootrom������������
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int DRV_SET_UPDATA_FLAG(int flag);


/*****************************************************************************
 �� �� ��  : DRV_USB_DISCONNECT
 ��������  : ����ʱ�Ͽ�USB����
 �������  : ��
                  
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
BSP_VOID DRV_USB_DISCONNECT();
	
/*****************************************************************************
* �� �� ��  : BSP_DLOAD_SetCdromMarker
*
* ��������  : �����豸��̬��־
*
* �������  : BSP_BOOL bCdromMarker  :
*             BSP_TRUE  :���������豸��̬
*             BSP_FALSE :����bootrom�豸��̬
* �������  : ��
*
* �� �� ֵ  : ��
*
* ����˵��  : ��������̨�ļ�ʱ������Ϊ�ϱ������豸��̬�����ָ�NV��
*             ������ǰ̨�ļ�ʱ������Ϊ�ϱ�bootrom�豸��̬���ָ�NV��
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCdromMarker(BSP_BOOL bCdromMarker);
#define DRV_SET_CDROM_FLAG(flag)    BSP_DLOAD_SetCdromMarker(flag)

/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetCdromMarker
*
* ��������  : ��ȡ�豸��̬��־
*
* �������  : BSP_VOID
* �������  : ��
*
* �� �� ֵ  : BSP_TRUE  :�ϱ������豸��̬
*             BSP_FALSE :�ϱ�bootrom�豸��̬
*
* ����˵��  : ��������̨�ļ�ʱ������Ϊ�ϱ������豸��̬�����ָ�NV��
*             ������ǰ̨�ļ�ʱ������Ϊ�ϱ�bootrom�豸��̬���ָ�NV��
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetCdromMarker (BSP_VOID);
#define DRV_GET_CDROM_FLAG()    BSP_DLOAD_GetCdromMarker()

/********************************************************************************************************
 �� �� ��  : BSP_DLOAD_NVBackupRead
 ��������  : ��Flash�е�NV�������ȡ���ݣ�ʵ��NV��Ļָ����ܡ�
 �������  : len����NV�������ʼ����ʼ����Ҫ��ȡ��NV��ȣ��ֽ�������������1Block��
 �������  : pRamAddr��Ŀ��RAM��ַ�����ڴ�Ŷ�����NV�����ݡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_DLOAD_NVBackupRead(unsigned char *pRamAddr, unsigned int len);
#define DRV_NVBACKUP_READ(pRamAddr,len)    BSP_DLOAD_NVBackupRead(pRamAddr, len)

/********************************************************************************************************
 �� �� ��  : BSP_DLOAD_NVBackupWrite
 ��������  : ���ض�����д��Flash�е�NV�������ʵ��NV��ı��ݹ��ܡ�
 �������  : pRamAddr��ԴRAM��ַ�����ڴ����Ҫд���NV�����ݡ�
             len����NV�������ʼ����ʼ����Ҫд���NV��ȣ��ֽ�������������1Block��
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_DLOAD_NVBackupWrite(unsigned char *pRamAddr, unsigned int len);
#define DRV_NVBACKUP_WRITE(pRamAddr, len)   BSP_DLOAD_NVBackupWrite(pRamAddr, len)


/********************************************************************************************************
 �� �� ��  : NVBackupFlashDataWrite
 ��������  : ʵ��FLASH ��NV���ݻָ�����ָ��λ�ö����ܡ�
 �������  : pRamAddr:ԴRAM��ַ
*         offset :��NV������0��ַ��ʼ��ƫ��
*         len: ��Ҫд��ĳ���
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int NVBackupFlashDataWrite(unsigned char* pRamAddr, unsigned int offset,unsigned int len);
#define DRV_NV_FLASH_WRITE(pRamAddr, offset,len) NVBackupFlashDataWrite(pRamAddr, offset,len)


/*****************************************************************************
 �� �� ��  : BSP_DLOAD_GetTFUpdateFlag
 ��������  : �ж��Ƿ���TF������
 �������  : None
 �������  : None
 �� �� ֵ  : ����1��TF������NV�ָ��ɹ�����Ҫ������
             ����0����TF������NV�ָ��ɹ�������Ҫ������

*****************************************************************************/
extern int BSP_DLOAD_GetTFUpdateFlag(void);
#define DRV_GET_TFUPDATE_FLAG()    BSP_DLOAD_GetTFUpdateFlag()

/*****************************************************************************
 �� �� ��  : DRV_GET_DLOAD_VERSION
 ��������  : Get dload version
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern BSP_S32 DRV_GET_DLOAD_VERSION(BSP_U8 *str, int len);


/*****************************************************************************
 �� �� ��  : DRV_GET_DLOAD_INFO
 ��������  : Get dload infomation
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/

extern BSP_S32 DRV_GET_DLOAD_INFO(unsigned char atCmdBuf[], unsigned int dloadType);

/*****************************************************************************
 �� �� ��  : DRV_GET_AUTHORITY_VERSION
 ��������  : Get Authority version
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern BSP_S32 DRV_GET_AUTHORITY_VERSION(BSP_U8 *str, BSP_S32 len);

/*****************************************************************************
 �� �� ��  : DRV_GET_AUTHORITY_ID
 ��������  : ��ȡ���ؼ�ȨЭ��Id
 �������  : unsigned char *buf
                          int len
 �������  : ��
 �� �� ֵ  : 0:  �����ɹ���
                      -1������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 DRV_GET_AUTHORITY_ID(unsigned char *buf, BSP_S32 len);

/*****************************************************************************
 �� �� ��  : BSP_DLOAD_GetDloadNetMode
 ��������  : Get net mode
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern BSP_S32  BSP_DLOAD_GetDloadNetMode(BSP_U32 *netMode);
#define DRV_GET_DLOAD_NETMODE(netMode)    BSP_DLOAD_GetDloadNetMode (netMode)

/*****************************************************************************
 �� �� ��  : BSP_DLOAD_GetDloadFlashInfo
 ��������  : Get dload flash infomation
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetDloadFlashInfo(DLOAD_FLASH_STRU* pFlashInfo);
#define DRV_GET_DLOAD_FLASHINFO(pFlashInfo)    BSP_DLOAD_GetDloadFlashInfo(pFlashInfo)


/*****************************************************************************
 �� �� ��  : BSP_DLOAD_GetWebUIVersion
 ��������  : ���WEBUI �汾��Ϣ
 �������  : pVersionInfo: ��ŷ��صİ汾��Ϣ���ڴ��ַ
                           ulLength: ��ŷ��صİ汾��Ϣ���ڴ泤�ȣ�Ŀǰ�̶�
                           128
 �������  : pVersionInfo: ���صİ汾��Ϣ���ڴ��ַ
 �� �� ֵ  :  0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetWebUIVersion(BSP_CHAR *pVersionInfo, BSP_U32 u32Length);
#define DRV_GET_WEBUI_VERSION(pVersionInfo, u32Length) BSP_DLOAD_GetWebUIVersion(pVersionInfo,u32Length)

/*****************************************************************************
 �� �� ��  : BSP_TFUP_CompleteDeal
 ��������  : TF��������ɺ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
extern void BSP_TFUP_CompleteDeal(void);
#define DRV_TFUP_COMPLETEDEAL() BSP_TFUP_CompleteDeal()

/*****************************************************************************
 �� �� ��  : BSP_TFUP_CompleteDeal
 ��������  : ����������ɺ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
void BSP_ONUP_CompleteDeal(void);
#define DRV_ONUP_COMPLETEDEAL() BSP_ONUP_CompleteDeal()

/*****************************************************************************
* �� �� ��  : BSP_DLOAD_GetNVBackupFlag
*
* ��������  : ��ȡ����ǰ�Ƿ����NV���ݱ�־
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : BSP_TRUE  :����NV
*            BSP_FALSE :������NV
*
* ����˵��  : �˽ӿ�ֻ�Է�һ��ʽ������ʽ��SD����/������������Ч��һ��ʽ�����ᷢAT��������
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetNVBackupFlag(BSP_VOID);
#define DRV_DLOAD_GETNVBACKUPFLAG() BSP_DLOAD_GetNVBackupFlag()

/*************************DLOAD END***********************************/

/*************************GPIO START**********************************/

typedef unsigned int GPIO_OPRT_ENUM_UINT32;

/*****************************************************************************
 �� �� ��  : BSP_GPIO_Oprt
 ��������  : at^GPIOPL,���úͲ�ѯGPIO�ĵ�ƽ
 �������  : �������� ulOp:
             0  ���ø�GPIO��PLֵ
             1  ��ѯ��ǰ��GPIO��PLֵ

             pucPL ��Ӧ20Byte������,ÿ��BYTE����һ��8���ܽŵ�ֵ

             ���ò���ʱ,pucPLΪ���õ�20��Byte
             ��ѯ����ʱ,pucPLΪ��ǰ��ȡ����PL��ʵ��ֵ��ɵ�16��������
             �����û�����at^GPIOPL = ABCDEF,���ӦpucPL������ֵΪ{A,B,C,D,E,F,0,0,...}

 �������  : ��
 �� �� ֵ  :  0 ��OK  �� 0 ��Error

*****************************************************************************/
extern unsigned long BSP_GPIO_Oprt(unsigned long ulOp, unsigned char *pucPL);
#define DRV_GPIO_OPRT(ulOp,pucPL)    BSP_GPIO_Oprt(ulOp,pucPL)

/*************************GPIO END************************************/

/*************************IPCM BEGIN**********************************/

/* ����������*/
typedef enum tagIPC_INT_CORE_E
{
    IPC_CORE_ARM11 = 0x0,   /* P500�ϵ�IPC */
    IPC_CORE_A9,
    IPC_CORE_CEVA,
    IPC_CORE_TENS0,
    IPC_CORE_TENS1,
    IPC_CORE_DSP,
    IPC_CORE_APPARM = 0x0,  /* V7R1/V3R2�ϵ�IPC */
    IPC_CORE_COMARM,
    IPC_CORE_LTEDSP,
    IPC_CORE_VDSP,
    IPC_CORE_ZSP,
	IPC_CORE_TDSDSP,
    IPC_CORE_DSP_GU,
    IPC_CORE_BUTTOM
}IPC_INT_CORE_E;

typedef enum tagIPC_INT_LEV_E
{
    IPC_INT_DSP_MODEM = 0,
    IPC_INT_DSP_APP = 1,
    IPC_INT_DSP_MSP = 0,
    IPC_INT_DSP_PS = 1,
    IPC_INT_MODEM_APP = 2,
    IPC_INT_DRX_APP = 3,
    IPC_INT_GUDSP_MODEM = 4,
    IPC_INT_MEDDSP_MODEM = 5,
    IPC_INT_DICC_USRDATA = 6,
    IPC_INT_DICC_RELDATA = 7,
    IPC_INT_DSP_HALT =8,  /*DSP֪ͨARM˯��*/
    IPC_INT_DSP_RESUME,   /*DSP֪ͨARM��ɻ��Ѻ�Ļָ�����*/
    IPC_INT_DSP_WAKE,     /*ARM����DSP*/
    IPC_INT_ARM_SLEEP = 8,
    IPC_INT_WAKE_GU =11,     /*��ģ���Ѵ�ģ�ж�*/
    IPC_INT_SLEEP_GU,     /*��ģ˯���ж�*/
    IPC_INT_CDRX_DSP_HALT,	/*DSP֪ͨMSP����CDRX����*/
	IPC_INT_TDSDSP_HALT =14,
	IPC_INT_TDSDSP_IDLE =15,
	IPC_INT_TDSDSP_RESUME =16,
	IPC_INT_WAKE_TDS = 17,  /*����TDS��ģ�ж�*/
	IPC_INT_WAKE_LTE = 18,  /*����LTE��ģ�ж�*/
	IPC_INT_SLEEP_TDS = 19, /*TDS��ģ˯���ж�*/
	IPC_INT_TDSDSP_MSP = 20, /*����TDS DSP �� MSP ���ж�*/
	IPC_INT_HIFI_FULL = 27,
    IPC_INT_HIFI_ON,
    IPC_INT_HIFI_OFF,
    IPC_INT_ICC_MODEM = 30,
    IPC_INT_ICC_APP,
    IPC_INT_BUTTOM
}IPC_INT_LEV_E;

typedef enum tagIPC_SEM_ID_E
{
    IPC_SEM_ICC,
    IPC_SEM_NAND,
    IPC_SEM_MEM,
    IPC_SEM_DICC,
    IPC_SEM_RFILE_LOG,          /*����flashless��ά�ɲ�*/
    IPC_SEM_SYNC = 25,
    IPC_SEM_SYSCTRL = 30,
    IPC_SEM_ZSP_HALT = 31,
    IPC_SEM_BUTTOM
}IPC_SEM_ID_E;


#define INTSRC_NUM                     32

/*****************************************************************************
* �� �� ��  : BSP_DRV_IPCIntInit
*
* ��������  : IPCģ���ʼ��
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��3��5��   wangjing  creat
*****************************************************************************/
BSP_S32 BSP_DRV_IPCIntInit(void);


/*****************************************************************************
* �� �� ��  : DRV_IPC_SEMCREATE
*
* ��������  : �ź�����������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemCreate(BSP_U32 u32SignalNum);
extern BSP_S32 DRV_IPC_SEMCREATE(BSP_U32 u32SignalNum);

/*****************************************************************************
* �� �� ��  : DRV_IPC_SEMDELETE
*
* ��������  : ɾ���ź���
*
* �������  :   BSP_U32 u32SignalNum Ҫɾ�����ź������

* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemDelete(BSP_U32 u32SignalNum);
extern BSP_S32 DRV_IPC_SEMDELETE(BSP_U32 u32SignalNum);

/*****************************************************************************
* �� �� ��  : BSP_IPC_IntEnable
*
* ��������  : ʹ��ĳ���ж�
*
* �������  :
                BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);
extern BSP_S32 DRV_IPC_INTENABLE(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* �� �� ��  : DRV_IPC_INTDISABLE
*
* ��������  : ȥʹ��ĳ���ж�
*
* �������  :
            BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl);
extern BSP_S32 DRV_IPC_INTDISABLE(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* �� �� ��  : BSP_IPC_IntConnect
*
* ��������  : ע��ĳ���ж�
*
* �������  :
           BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31
           VOIDFUNCPTR routine �жϷ������
*             BSP_U32 parameter      �жϷ���������
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);
extern BSP_S32 DRV_IPC_INTCONNECT(IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* �� �� ��  : BSP_IPC_IntDisonnect
*
* ��������  : ȡ��ע��ĳ���ж�
*
* �������  :
*              BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31
*              VOIDFUNCPTR routine �жϷ������
*             BSP_U32 parameter      �жϷ���������
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* �� �� ��  : DRV_IPC_INTSEND
*
* ��������  : �����ж�
*
* �������  :
                IPC_INT_CORE_E enDstore Ҫ�����жϵ�core
                BSP_U32 ulLvl Ҫ���͵��жϺţ�ȡֵ��Χ0��31
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);
extern BSP_S32 DRV_IPC_INTSEND(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* �� �� ��  : DRV_IPC_SEMTAKE
*
* ��������  : ��ȡ�ź���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout);
extern BSP_S32 DRV_IPC_SEMTAKE(BSP_U32 u32SignalNum, BSP_S32 s32timeout);

/*****************************************************************************
* �� �� ��  : DRV_IPC_SEMGIVE
*
* ��������  : �ͷ��ź���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum);
extern BSP_VOID DRV_IPC_SEMGIVE(BSP_U32 u32SignalNum);

/*****************************************************************************
* �� �� ��  : BSP_IPC_SpinLock
*
* ��������  : ��ȡ�ź���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum);
extern BSP_VOID DRV_SPIN_LOCK (BSP_U32 u32SignalNum);

/*****************************************************************************
* �� �� ��  : DRV_SPIN_UNLOCK
*
* ��������  : �ͷ��ź���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum);
extern BSP_VOID DRV_SPIN_UNLOCK(BSP_U32 u32SignalNum);


BSP_S32 BSP_SGI_Connect(BSP_U32 ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);
BSP_S32 BSP_SGI_IntSend(BSP_U32 ulLvl);

/*************************IPCM END************************************/


/*************************UDI START***********************************/

/**************************************************************************
  �궨��
**************************************************************************/
#define UDI_INVALID_HANDLE     (-1)

#define UDI_CAPA_BLOCK_READ    ((BSP_U32)(1<<0)) /* ���豸��read�ӿ�Ϊ�����ӿ� */
#define UDI_CAPA_BLOCK_WRITE   ((BSP_U32)(1<<1)) /* ���豸��write�ӿ�Ϊ�����ӿ� */
#define UDI_CAPA_READ_CB       ((BSP_U32)(1<<2)) /* ���豸֧��read �ص����� */
#define UDI_CAPA_WRITE_CB      ((BSP_U32)(1<<3)) /* ���豸֧��write �ص����� */
#define UDI_CAPA_BUFFER_LIST   ((BSP_U32)(1<<4)) /* ���豸�Ķ�дbufferΪ�ڴ�����ṹ(Ĭ��Ϊ��ͨ�ڴ��) */
#define UDI_CAPA_CTRL_OPT      ((BSP_U32)(1<<5)) /* ���豸֧�ֿ��Ʋ��� */

#define MIN_UDI_HANDLE     0x5a0000
#define MAX_UDI_HANDLE     0x5a003f

/**************************************************************************
  �ṹ����
**************************************************************************/
#define UDI_BUILD_DEV_ID(dev, type) (((BSP_U32)(dev) << 8) | ((BSP_U32)(type) & 0x00ff))
#define UDI_BUILD_CMD_ID(dev, cmd) (((BSP_U32)(dev) << 8) | ((BSP_U32)(cmd) & 0xffff))


/* ���豸���� */
typedef enum tagUDI_DEVICE_MAIN_ID
{
    UDI_DEV_USB_ACM = 0,
    UDI_DEV_USB_NCM,
    UDI_DEV_ICC,
    UDI_DEV_UART,

    UDI_DEV_MAX                 /* ���������, ���ڱ߽�ֵ */
}UDI_DEVICE_MAIN_ID;

/* ���豸���Ͷ���(Ҫ�� usb ���豸��̬ͳһ����) */
/* ACM */
typedef enum tagUDI_ACM_DEV_TYPE
{
    UDI_USB_ACM_CTRL,
    UDI_USB_ACM_AT,
    UDI_USB_ACM_SHELL,
    UDI_USB_ACM_LTE_DIAG,
    UDI_USB_ACM_OM,
    UDI_USB_ACM_MODEM,
    UDI_USB_ACM_GPS,      /*HISO*/
    UDI_USB_ACM_3G_GPS,   /*ashell*/
    UDI_USB_ACM_3G_PCVOICE, /*Ԥ��*/
    UDI_USB_ACM_PCVOICE,    /*Ԥ��*/
    /*UDI_USB_ACM_3G_AT,    //3G AT*/
    /*UDI_USB_ACM_3G_MODEM, //3G PPP*/
    UDI_USB_HSIC_ACM0,
    UDI_USB_HSIC_ACM1,
    UDI_USB_HSIC_ACM2,
    UDI_USB_HSIC_ACM3,
    UDI_USB_HSIC_ACM4,
    UDI_USB_HSIC_ACM5,
    UDI_USB_HSIC_ACM6,
    UDI_USB_HSIC_ACM7,
    UDI_USB_HSIC_ACM8,
    UDI_USB_HSIC_ACM9,
    UDI_USB_HSIC_ACM10,
    UDI_USB_HSIC_ACM11,
    UDI_USB_HSIC_ACM12,
    UDI_USB_HSIC_ACM13,
	UDI_USB_HSIC_ACM14,
    UDI_USB_ACM_MAX                /* ���������, ���ڱ߽�ֵ */
}UDI_ACM_DEV_TYPE;

/* NCM */
typedef enum tagUDI_NCM_DEV_TYPE
{
    UDI_USB_NCM_NDIS,
    UDI_USB_NCM_CTRL,
    UDI_USB_HSIC_NCM0,
    UDI_USB_HSIC_NCM1,
    UDI_USB_HSIC_NCM2,

    UDI_USB_NCM_MAX                /* ���������, ���ڱ߽�ֵ */
}UDI_NCM_DEV_TYPE;

/* ICC */
typedef enum tagUDI_ICC_DEV_TYPE
{
    UDI_ICC_GUOM0 = 16,
    UDI_ICC_GUOM1,
    UDI_ICC_GUOM2,
    UDI_ICC_GUOM3,
    UDI_ICC_GUOM4,
    UDI_ICC_GUOM5,

    UDI_ICC_MAX                /* ���������, ���ڱ߽�ֵ */
}UDI_ICC_DEV_TYPE;


/* �豸ID�Ŷ��� */
typedef enum tagUDI_DEVICE_ID
{
    /* USB ACM */
    UDI_ACM_CTRL_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_CTRL),
    UDI_ACM_AT_ID =    UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_AT),
    UDI_ACM_SHELL_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_SHELL),
    UDI_ACM_LTE_DIAG_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_LTE_DIAG),
    UDI_ACM_OM_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_OM),
    UDI_ACM_MODEM_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_MODEM),
    UDI_ACM_GPS_ID      = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_GPS),     /*HISO*/
    UDI_ACM_3G_GPS_ID   = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_GPS),  /*ashell*/
    UDI_ACM_3G_PCVOICE_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_PCVOICE), /*Ԥ��*/
    UDI_ACM_PCVOICE_ID    = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_PCVOICE),    /*Ԥ��*/
    /*UDI_ACM_3G_AT_ID      = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_AT),    //3G AT*/
    /*UDI_ACM_3G_MODEM_ID   = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_MODEM), //3G PPP*/
    UDI_ACM_HSIC_ACM0_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM0),
    UDI_ACM_HSIC_ACM1_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM1),
    UDI_ACM_HSIC_ACM2_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM2),
    UDI_ACM_HSIC_ACM3_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM3),
    UDI_ACM_HSIC_ACM4_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM4),
    UDI_ACM_HSIC_ACM5_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM5),
    UDI_ACM_HSIC_ACM6_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM6),
    UDI_ACM_HSIC_ACM7_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM7),
    UDI_ACM_HSIC_ACM8_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM8),
    UDI_ACM_HSIC_ACM9_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM9),
    UDI_ACM_HSIC_ACM10_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM10),
    UDI_ACM_HSIC_ACM11_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM11),
    UDI_ACM_HSIC_ACM12_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM12),
    UDI_ACM_HSIC_ACM13_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM13),
	UDI_ACM_HSIC_ACM14_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM14),

    /* USB NCM */
    UDI_NCM_NDIS_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_NDIS),
    UDI_NCM_CTRL_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_CTRL),
    UDI_NCM_HSIC_NCM0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM0),
    UDI_NCM_HSIC_NCM1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM1),
    UDI_NCM_HSIC_NCM2_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM2),

    /* UART */
    UDI_UART_0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_UART, 0),
    UDI_UART_1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_UART, 1),

    /* ICC */
    UDI_ICC_IFC_ID  =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 0),
    UDI_ICC_IPM_ID  =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 1),
    UDI_ICC_LMSP_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 2),

    UDI_ICC_GUOM0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM0),
    UDI_ICC_GUOM1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM1),
    UDI_ICC_GUOM2_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM2),
    UDI_ICC_GUOM3_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM3),
    UDI_ICC_GUOM4_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM4),
    UDI_ICC_GUOM5_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM5),

    /* MUST BE LAST */
    UDI_INVAL_DEV_ID = 0xFFFF
} UDI_DEVICE_ID;


/* �豸�Ĵ򿪲��� */
typedef struct tagUDI_OPEN_PARAM
{
    UDI_DEVICE_ID devid;                        /* �豸ID */
    void   *pPrivate;                            /* ģ�����е����� */
} UDI_OPEN_PARAM;

/* IOCTL ������,��Ҫ���������ڴ���� */
typedef enum tagUDI_IOCTL_CMD_TYPE
{
    UDI_IOCTL_SET_WRITE_CB = 0xF001,            /* ����һ����ʼ��ֵ��ֹ��ϵͳ�����ͻ */
    UDI_IOCTL_SET_READ_CB,

    UDI_IOCTL_INVAL_CMD = 0xFFFFFFFF
} UDI_IOCTL_CMD_TYPE;

/**************************************************************************
  ��������
**************************************************************************/
/*****************************************************************************
* �� �� ��  : udi_get_capability
*
* ��������  : �����豸ID��ȡ��ǰ�豸֧�ֵ�����
*
* �������  : devId: �豸ID
* �������  : ��
* �� �� ֵ  : ֧�ֵ�����ֵ
*****************************************************************************/
BSP_S32 udi_get_capability(UDI_DEVICE_ID devId);

/*****************************************************************************
* �� �� ��  : udi_open
*
* ��������  : ���豸(����ͨ��)
*
* �������  : pParam: �豸�Ĵ����ò���
* �������  : ��
* �� �� ֵ  : -1:ʧ�� / ����:�ɹ�
*****************************************************************************/
UDI_HANDLE udi_open(UDI_OPEN_PARAM *pParam);
#define DRV_UDI_OPEN(pParam)    udi_open(pParam)

/*****************************************************************************
* �� �� ��  : udi_close
*
* ��������  : �ر��豸(����ͨ��)
*
* �������  : handle: �豸��handle
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 udi_close(UDI_HANDLE handle);
#define DRV_UDI_CLOSE(handle)    udi_close(handle)

/*****************************************************************************
* �� �� ��  : udi_write
*
* ��������  : ����д
*
* �������  : handle:  �豸��handle
*             pMemObj: buffer�ڴ� �� �ڴ��������
*             u32Size: ����д�ߴ� �� �ڴ��������ɲ�����
* �������  :
*
* �� �� ֵ  : ����ֽ��� �� �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 udi_write(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);
#define  DRV_UDI_WRITE(handle,pMemObj,u32Size)     udi_write (handle,pMemObj,u32Size)

/*****************************************************************************
* �� �� ��  : udi_read
*
* ��������  : ���ݶ�
*
* �������  : handle:  �豸��handle
*             pMemObj: buffer�ڴ� �� �ڴ��������
*             u32Size: ���ݶ��ߴ� �� �ڴ��������ɲ�����
* �������  :
*
* �� �� ֵ  : ����ֽ��� �� �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 udi_read(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);
#define DRV_UDI_READ(handle,pMemObj,u32Size)  udi_read(handle,pMemObj,u32Size)

/*****************************************************************************
* �� �� ��  : udi_ioctl
*
* ��������  : ����ͨ����������
*
* �������  : handle: �豸��handle
*             u32Cmd: IOCTL������
*             pParam: ��������
* �������  :
*
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 udi_ioctl(UDI_HANDLE handle, BSP_U32 u32Cmd, BSP_VOID* pParam);
#define DRV_UDI_IOCTL(handle,u32Cmd,pParam)    udi_ioctl(handle,u32Cmd,pParam)

/*************************UDI END*************************************/

/*************************MEMORY START********************************/

/**************************************************************************
  �궨��
**************************************************************************/
/* �ڴ������, Flags����� */
typedef enum tagMEM_POOL_TYPE
{
    MEM_NORM_DDR_POOL = 0,
    MEM_ICC_DDR_POOL,
    MEM_ICC_AXI_POOL,
    MEM_POOL_MAX
}MEM_POOL_TYPE;

/**************************************************************************
  �ӿ�����
**************************************************************************/
BSP_VOID* BSP_Malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);
BSP_VOID* BSP_MallocDbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line);
BSP_VOID  BSP_Free(BSP_VOID* pMem);
BSP_VOID  BSP_FreeDbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line);

/*****************************************************************************
* �� �� ��  : BSP_MALLOC
*
* ��������  : BSP ��̬�ڴ����
*
* �������  : sz: ����Ĵ�С(byte)
*             flags: �ڴ�����(�ݲ�ʹ��,Ԥ��)
* �������  : ��
* �� �� ֵ  : ����������ڴ�ָ��
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_MALLOC(sz, flags) BSP_MallocDbg(sz, flags, __FILE__, __LINE__)
#else
#define BSP_MALLOC(sz, flags) BSP_Malloc(sz, flags)
#endif

/*****************************************************************************
* �� �� ��  : BSP_FREE
*
* ��������  : BSP ��̬�ڴ��ͷ�
*
* �������  : ptr: ��̬�ڴ�ָ��
* �������  : ��
* �� �� ֵ  : ��
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_FREE(ptr) BSP_FreeDbg(ptr, __FILE__, __LINE__)
#else
#define BSP_FREE(ptr) BSP_Free(ptr)
#endif



/*****************************************************************************
* �� �� ��  : BSP_SFree
*
* ��������  : BSP ��̬�ڴ��ͷ�(��spin lock����,��˳���ʹ��)
*
* �������  : pMem: ��̬�ڴ�ָ��
* �������  : ��
* �� �� ֵ  : ��
*****************************************************************************/
BSP_VOID  BSP_SFree(BSP_VOID* pMem);
/*****************************************************************************
* �� �� ��  : BSP_SMalloc
*
* ��������  : BSP ��̬�ڴ����(��spin lock����,��˳���ʹ��)
*
* �������  : u32Size: ����Ĵ�С(byte)
*             enFlags: �ڴ�����(�ݲ�ʹ��,Ԥ��)
* �������  : ��
* �� �� ֵ  : ����������ڴ�ָ��
*****************************************************************************/
BSP_VOID* BSP_SMalloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);


#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_VOID* cacheDmaMalloc(BSP_U32  bytes);
#define DRV_CACHEDMAM_ALLOC(bytes) cacheDmaMalloc(bytes)
#else
#define DRV_CACHEDMAM_ALLOC(bytes) kmalloc(bytes, GFP_KERNEL)
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_S32 cacheDmaFree(BSP_VOID*  pBuf);
#define DRV_CACHEDMAM_FREE(pBuf) cacheDmaFree(pBuf)
#else
#define DRV_CACHEDMAM_FREE(pBuf) kfree(pBuf)
#endif

/************************************************************************
 * FUNCTION
 *       vmEnable
 * DESCRIPTION
 *       �ڴ��д��������ʹ�ܺ���
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int vmEnable(int enable);
#define DRV_VM_ENABLE(flag)   vmEnable(flag)

/*****************************************************************************
 �� �� ��  : vmStateSet
 ��������  : ���õ�ַ�ռ�״̬
 �������  : ��

 �������  : �ޡ�
 �� �� ֵ  : ��
 ע������  ��
*****************************************************************************/
#define DRV_VM_STATESET(context, virtAdrs, len, stateMask, state) vmStateSet(context, virtAdrs, len, stateMask, state)

/*****************************************************************************
 �� �� ��  : BSP_CACHE_DATA_FLUSH
 ��������  :
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern void BSP_CACHE_DATA_FLUSH(void * addr, int size);

/*************************MEMORY END**********************************/

/*************************SOCP START**********************************/

/**************************************************************************
  �궨��
**************************************************************************/
#define SOCP_CODER_SRC_CHAN                 0x00
#define SOCP_CODER_DEST_CHAN                0x01
#define SOCP_DECODER_SRC_CHAN               0x02
#define SOCP_DECODER_DEST_CHAN              0x03

#define SOCP_MAX_ENCSRC_BACKNUM                 (1)
#define SOCP_MAX_ENCDST_BACKNUM                 (1)


#define SOCP_CHAN_DEF(chan_type, chan_id)   ((chan_type<<16)|chan_id)
#define SOCP_REAL_CHAN_ID(unique_chan_id)   (unique_chan_id & 0xFFFF)
#define SOCP_REAL_CHAN_TYPE(unique_chan_id) (unique_chan_id>>16)

#define SOCP_CODER_DEST_CHAN_0      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 0)
#define SOCP_CODER_DEST_CHAN_1      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 1)
#define SOCP_CODER_DEST_CHAN_2      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 2)
#define SOCP_CODER_DEST_CHAN_3      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 3)
#define SOCP_CODER_DEST_CHAN_4      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 4)
#define SOCP_CODER_DEST_CHAN_5      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 5)
#define SOCP_CODER_DEST_CHAN_6      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 6)

#define SOCP_DECODER_SRC_CHAN_0     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 0)
#define SOCP_DECODER_SRC_CHAN_1     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 1)
#define SOCP_DECODER_SRC_CHAN_2     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 2)
#define SOCP_DECODER_SRC_CHAN_3     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 3)

#define SOCP_CODER_SRC_CHAN_BASE            0x00000000
#define SOCP_CODER_DEST_CHAN_BASE           0x00010000
#define SOCP_DECODER_SRC_CHAN_BASE          0x00020000
#define SOCP_DECODER_DEST_CHAN_BASE         0x00030000

/**************************************************************************
  �����붨��
**************************************************************************/
#define BSP_ERR_SOCP_BASE            BSP_DEF_ERR(BSP_MODU_SOCP, 0)
#define BSP_ERR_SOCP_NULL            (BSP_ERR_SOCP_BASE + 0x1)
#define BSP_ERR_SOCP_NOT_INIT        (BSP_ERR_SOCP_BASE + 0x2)
#define BSP_ERR_SOCP_MEM_ALLOC       (BSP_ERR_SOCP_BASE + 0x3)
#define BSP_ERR_SOCP_SEM_CREATE      (BSP_ERR_SOCP_BASE + 0x4)
#define BSP_ERR_SOCP_TSK_CREATE      (BSP_ERR_SOCP_BASE + 0x5)
#define BSP_ERR_SOCP_INVALID_CHAN    (BSP_ERR_SOCP_BASE + 0x6)
#define BSP_ERR_SOCP_INVALID_PARA    (BSP_ERR_SOCP_BASE + 0x7)
#define BSP_ERR_SOCP_NO_CHAN         (BSP_ERR_SOCP_BASE + 0x8)
#define BSP_ERR_SOCP_SET_FAIL        (BSP_ERR_SOCP_BASE + 0x9)
#define BSP_ERR_SOCP_TIMEOUT         (BSP_ERR_SOCP_BASE + 0xa)
#define BSP_ERR_SOCP_NOT_8BYTESALIGN (BSP_ERR_SOCP_BASE + 0xb)
#define BSP_ERR_SOCP_CHAN_RUNNING    (BSP_ERR_SOCP_BASE + 0xc)
#define BSP_ERR_SOCP_CHAN_MODE       (BSP_ERR_SOCP_BASE + 0xd)
#define BSP_ERR_SOCP_DEST_CHAN       (BSP_ERR_SOCP_BASE + 0xe)
#define BSP_ERR_SOCP_DECSRC_SET      (BSP_ERR_SOCP_BASE + 0xf)
/**************************************************************************
  �ṹ����
**************************************************************************/


typedef enum tagSOCP_EVENT_E
{
    SOCP_EVENT_PKT_HEADER_ERROR         = 0x1,    /* ��ͷ������"HISI" */
    SOCP_EVENT_OUTBUFFER_OVERFLOW       = 0x2,    /* Ŀ��buffer���� */
    SOCP_EVENT_RDBUFFER_OVERFLOW        = 0x4,    /* RDbuffer���� */
    SOCP_EVENT_DECODER_UNDERFLOW        = 0x8,    /* ����Դbuffer���� */
    SOCP_EVENT_PKT_LENGTH_ERROR         = 0x10,   /* ��������ȼ����� */
    SOCP_EVENT_CRC_ERROR                = 0x20,   /* ����CRCУ����� */
    SOCP_EVENT_DATA_TYPE_ERROR          = 0x40,   /* �����������ʹ���*/
    SOCP_EVENT_HDLC_HEADER_ERROR        = 0x80,   /* ����HDLC������ */
    SOCP_EVENT_BUTT
}SOCP_EVENT_E;

/* ����Դ�����ݽṹģʽ */
typedef enum tagSOCP_ENCSRC_CHNMODE_E
{
    SOCP_ENCSRC_CHNMODE_CTSPACKET       = 0,    /* �������ݰ����λ����� */
    SOCP_ENCSRC_CHNMODE_FIXPACKET,              /* �̶��������ݰ����λ����� */
    SOCP_ENCSRC_CHNMODE_LIST,                   /* ��ʽ���λ����� */
    SOCP_ENCSRC_CHNMODE_BUTT
}SOCP_ENCSRC_CHNMODE_E;

/* ����Դ�����ݽṹģʽ */
typedef enum tagSOCP_DECSRC_CHNMODE_E
{
    SOCP_DECSRC_CHNMODE_BYTES        = 0,       /* �����ֽڻ��λ����� */
    SOCP_DECSRC_CHNMODE_LIST,                   /* ��ʽ���λ����� */
    SOCP_DECSRC_CHNMODE_BUTT
}SOCP_DECSRC_CHNMODE_E;

/* ��ʱѡ��ʹ�����ݽṹ�� */
typedef enum tagSOCP_TIMEOUT_EN_E
{
    SOCP_TIMEOUT_BUFOVF_DISABLE        = 0,       /* buffer��������ϱ��ж� */
    SOCP_TIMEOUT_BUFOVF_ENABLE,                   /* buffer�������ʱ�����ϱ��ж� */
    SOCP_TIMEOUT_TRF,                             /* �����жϳ�ʱ���� */
    SOCP_TIMEOUT_BUTT
}SOCP_TIMEOUT_EN_E;

/* ͬһ���͵�ͨ����ͬ���ȼ�ö��ֵ*/
typedef enum tagSOCP_CHAN_PRIORITY_E
{
    SOCP_CHAN_PRIORITY_0     = 0,               /* ������ȼ�*/
    SOCP_CHAN_PRIORITY_1,                       /* �ε����ȼ�*/
    SOCP_CHAN_PRIORITY_2,                       /* �θ����ȼ�*/
    SOCP_CHAN_PRIORITY_3,                       /* ������ȼ�*/
    SOCP_CHAN_PRIORITY_BUTT
}SOCP_CHAN_PRIORITY_E;

/* ��������ö��ֵ*/
typedef enum tagSOCP_DATA_TYPE_E
{
    SOCP_DATA_TYPE_0            = 0,            /* LTE OAM���� */
    SOCP_DATA_TYPE_1,                           /* GU OAM���� */
    SOCP_DATA_TYPE_2,                           /* ���� */
    SOCP_DATA_TYPE_3,                           /* ���� */
    SOCP_DATA_TYPE_BUTT
}SOCP_DATA_TYPE_E;


/* BBP ����ģʽ�����ݶ����򸲸� */
typedef enum tagSOCP_BBP_DS_MODE_E
{
    SOCP_BBP_DS_MODE_DROP           = 0,        /* ���ݶ��� */
    SOCP_BBP_DS_MODE_OVERRIDE,                  /* ���ݸ��� */
    SOCP_BBP_DS_MODE_BUTT
}SOCP_BBP_DS_MODE_E;

/* ����Դͨ��data type ʹ��λ */
typedef enum tagSOCP_DATA_TYPE_EN_E
{
    SOCP_DATA_TYPE_EN           = 0,        /* data type ʹ�ܣ�Ĭ��ֵ */
    SOCP_DATA_TYPE_DIS,                     /* data type ��ʹ�� */
    SOCP_DATA_TYPE_EN_BUTT
}SOCP_DATA_TYPE_EN_E;

/* ����Դͨ��debug ʹ��λ */
typedef enum tagSOCP_ENC_DEBUG_EN_E
{
    SOCP_ENC_DEBUG_DIS          = 0,       /* debug ��ʹ�ܣ�Ĭ��ֵ */
    SOCP_ENC_DEBUG_EN,                     /* debug ʹ�� */
    SOCP_ENC_DEBUG_EN_BUTT
}SOCP_ENC_DEBUG_EN_E;

/* ����ͨ·�������ýṹ�� */
typedef struct tagSOCP_DEC_PKTLGTH_S
{
    BSP_U32                 u32PktMax;         /*���������ֵ*/
    BSP_U32                 u32PktMin;         /*��������Сֵ*/
}SOCP_DEC_PKTLGTH_S;


/* ͨ��Դͨ��buffer�ṹ�嶨��*/
typedef struct tagSOCP_SRC_SETBUF_S
{
    BSP_U32                 u32InputStart;      /* ����ͨ����ʼ��ַ*/
    BSP_U32                 u32InputEnd;        /* ����ͨ��������ַ*/
    BSP_U32                 u32RDStart;         /* RD buffer��ʼ��ַ*/
    BSP_U32                 u32RDEnd;           /* RD buffer������ַ*/
    BSP_U32                 u32RDThreshold;     /* RD buffer�����ϱ���ֵ*/
}SOCP_SRC_SETBUF_S;

/* ͨ��Ŀ��ͨ��buffer�ṹ�嶨��*/
typedef struct tagSOCP_DST_SETBUF_S
{
    BSP_U32                 u32OutputStart;     /* ���ͨ����ʼ��ַ*/
    BSP_U32                 u32OutputEnd;       /* ���ͨ��������ַ*/
    BSP_U32                 u32Threshold;       /* ���ͨ������ֵ */
}SOCP_DST_SETBUF_S;

/* ����Դͨ���ṹ�嶨��*/
typedef struct tagSOCP_CODER_SRC_CHAN_S
{
    BSP_U32                 u32DestChanID;      /* Ŀ��ͨ��ID*/
    BSP_U32                 u32BypassEn;        /* ͨ��bypassʹ��*/
    SOCP_DATA_TYPE_E        eDataType;          /* �������ͣ�ָ�����ݷ�װЭ�飬���ڸ��ö�ƽ̨*/
#if defined (CHIP_BB_6920CS)
    SOCP_DATA_TYPE_EN_E     eDataTypeEn;        /* ��������ʹ��λ*/
    SOCP_ENC_DEBUG_EN_E     eDebugEn;           /* ����λʹ��*/
#endif
    SOCP_ENCSRC_CHNMODE_E   eMode;              /* ͨ������ģʽ*/
    SOCP_CHAN_PRIORITY_E    ePriority;          /* ͨ�����ȼ�*/
    SOCP_SRC_SETBUF_S       sCoderSetSrcBuf;
}SOCP_CODER_SRC_CHAN_S;

/* ����Ŀ�����ýṹ�嶨��*/
typedef struct tagSOCP_CODER_DEST_CHAN_S
{
#if defined (CHIP_BB_6920CS)
    BSP_U32                 u32EncDstThrh;     /* ����Ŀ��ͨ����ֵ���ޣ��ٲ�ͨ��ʱʹ��*/
#endif
    SOCP_DST_SETBUF_S       sCoderSetDstBuf;
}SOCP_CODER_DEST_CHAN_S;

/* ����Դͨ���ṹ�嶨��*/
typedef struct tagSOCP_DECODER_SRC_CHAN_S
{
#if defined (CHIP_BB_6920CS)
    SOCP_DATA_TYPE_EN_E     eDataTypeEn;        /* ��������ʹ��λ*/
#endif
    SOCP_DECSRC_CHNMODE_E   eMode;              /* ͨ��ģʽ*/
    SOCP_SRC_SETBUF_S       sDecoderSetSrcBuf;
}SOCP_DECODER_SRC_CHAN_S;

/* ����Ŀ��ͨ���ṹ�嶨��*/
typedef struct tagSOCP_DECODER_DEST_CHAN_S
{
    BSP_U32                 u32SrcChanID;       /* ������Դͨ��ID*/
    SOCP_DATA_TYPE_E        eDataType;          /* �������ͣ�ָ�����ݷ�װЭ�飬���ڸ��ö�ƽ̨*/
    SOCP_DST_SETBUF_S       sDecoderDstSetBuf;
}SOCP_DECODER_DEST_CHAN_S;

/* ͨ��buffer�����ṹ�嶨��*/
typedef struct tagSOCP_BUFFER_RW_S
{
    BSP_CHAR   *pBuffer;                        /* bufferָ��*/
    BSP_U32     u32Size;                        /* ����buffer��С*/
    BSP_CHAR   *pRbBuffer;                      /* �ؾ�bufferָ��*/
    BSP_U32     u32RbSize;                      /* �ؾ�buffer��С*/
}SOCP_BUFFER_RW_S;

/* ����Դͨ����������ṹ�嶨��*/
typedef struct tagSOCP_DECODER_ERROR_CNT_S
{
    BSP_U32     u32PktlengthCnt;                /* �������������*/
    BSP_U32     u32CrcCnt;                      /* CRCУ��������*/
    BSP_U32     u32DataTypeCnt;                 /* �����������ͼ���������*/
    BSP_U32     u32HdlcHeaderCnt;               /* 0x7EУ�����*/
}SOCP_DECODER_ERROR_CNT_S;

/* ����ͨ·�������ýṹ�� */
typedef struct tagSOCP_ENCSRC_RSVCHN_SCOPE_S
{
    BSP_U32                 u32RsvIDMin;        /*����Դ����ͨ��ID��Сֵ*/
    BSP_U32                 u32RsvIDMax;        /*����Դ����ͨ��ID���ֵ*/
}SOCP_ENCSRC_RSVCHN_SCOPE_S;

#define SOCP_OM_CHANNEL_CNT    3
/*******************************************************************************
 �ṹ��    : SOCP_LOG_EXC_INFO_STRU
 Э����  :
 ASN.1���� :
 �ṹ˵��  : LOG2.0 OM���ݸ�λ��Ϣ����
 ����˵��:   ��˫�˹����ڴ�����˸ýṹ���ȵĿռ�
             �Ķ��ýṹʱ��ȷ��ͬ������excdrv.h��EXCH_A_C_SOCP_LOG_SIZE
*******************************************************************************/
typedef struct
{
    UINT32 logOnFlag;    
    UINT32 dataBaseAddr[SOCP_OM_CHANNEL_CNT];
    UINT32 dataLen[SOCP_OM_CHANNEL_CNT];
    UINT32 dataUnreelAddr[SOCP_OM_CHANNEL_CNT];
    UINT32 dataUnreelLen[SOCP_OM_CHANNEL_CNT]; 
} SOCP_LOG_EXC_INFO_STRU;

typedef struct SOCP_ENC_DST_BUF_LOG_CFG
{
    BSP_U32     guWaterMark;    /* SOCP����Ŀ��ͨ�����ݴ���ˮ�ߣ�GUͨ�� */
    BSP_U32     lWaterMark;     /* SOCP����Ŀ��ͨ�����ݴ���ˮ�ߣ�Lͨ�� */
    BSP_U32     overTime;       /* SOCP����Ŀ��ͨ�����ݴ��䳬ʱʱ�� */
    BSP_U32     flushFlag;      /* SOCP����Ŀ��ͨ���������������־ */
    BSP_U32     logOnFlag;      /* LOG2.0����������־ */
} SOCP_ENC_DST_BUF_LOG_CFG_STRU;

typedef BSP_S32 (*socp_event_cb)(BSP_U32 u32ChanID, SOCP_EVENT_E u32Event, BSP_U32 u32Param);
typedef BSP_S32 (*socp_read_cb)(BSP_U32 u32ChanID);
typedef BSP_S32 (*socp_rd_cb)(BSP_U32 u32ChanID);

/**************************************************************************
  ��������
**************************************************************************/
/*****************************************************************************
 �� �� ��  : BSP_SOCP_SleepIn
 ��������  : SOCP����˯��
 �������  : pu32SrcChanID:����Դͨ��ID�б�
             uSrcChanNum:  ����Դͨ������
             pu32DstChanID:����Ŀ��ͨ��ID�б�
             uSrcChanNum:  ����Ŀ��ͨ������
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:����˯�߳ɹ���
             ����:   ����˯��ʧ��
*****************************************************************************/
extern BSP_U32 DRV_SOCP_SLEEPIN(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);

/*****************************************************************************
 �� �� ��  : DRV_SOCP_SLEEPOUT
 ��������  : SOCP�˳�˯��
 �������  : pu32SrcChanID:����Դͨ��ID�б�
             uSrcChanNum:  ����Դͨ������
             pu32DstChanID:����Ŀ��ͨ��ID�б�
             uSrcChanNum:  ����Ŀ��ͨ������
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:����˯�߳ɹ���
             ����:   ����˯��ʧ��
*****************************************************************************/
extern BSP_U32 DRV_SOCP_SLEEPOUT(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);

/*****************************************************************************
 �� �� ��  : BSP_SOCP_CoderAllocSrcChan
 ��������  : �˽ӿ����SOCP������Դͨ���ķ��䣬���ݱ�����Դͨ����������ͨ�����ԣ�����Ŀ��ͨ�������غ���ִ�н����
 �������  : pSrcAttr:������Դͨ�������ṹ��ָ�롣
             pSrcChanID:���뵽��Դͨ��ID��
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:����Դͨ������ɹ���
             SOCP_ERROR:����Դͨ������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_CoderAllocSrcChan(SOCP_CODER_SRC_CHAN_S *pSrcAttr, BSP_U32 *pSrcChanID);
#define DRV_SOCP_CoderAllocSrcChan(pSrcAttr, pSrcChanID)    BSP_SOCP_CoderAllocSrcChan(pSrcAttr, pSrcChanID)

/*****************************************************************************
 �� �� ��  : BSP_SOCP_CoderSetDestChanAttr
 ��������  : �˽ӿ����ĳһ����Ŀ��ͨ�������ã����غ���ִ�еĽ����
 �������  : u32DestChanID:SOCP��������Ŀ��ͨ��ID��
             pDestAttr:SOCP������Ŀ��ͨ�������ṹ��ָ�롣
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:����Ŀ��ͨ�����óɹ���
             SOCP_ERROR:����Ŀ��ͨ������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_CoderSetDestChanAttr(BSP_U32 u32DestChanID, SOCP_CODER_DEST_CHAN_S *pDestAttr);
#define DRV_SOCP_CoderSetDestChanAttr(u32DestChanID, pDestAttr)    BSP_SOCP_CoderSetDestChanAttr(u32DestChanID, pDestAttr)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_DecoderAllocDestChan
 ��������  :�˽ӿ����SOCP������Ŀ��ͨ���ķ��䣬
                ���ݽ���Ŀ��ͨ����������ͨ�����ԣ�
                ������Դͨ�������غ���ִ�н����
 �������  : pAttr:������Ŀ��ͨ�������ṹ��ָ��
                         pDestChanID:���뵽��Ŀ��ͨ��ID
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:����Ŀ��ͨ������ɹ���
                             SOCP_ERROR:����Ŀ��ͨ������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_DecoderAllocDestChan (SOCP_DECODER_DEST_CHAN_S *pAttr, BSP_U32 *pDestChanID);
#define  DRV_SOCP_DecoderAllocDestChan(pAttr ,pDestChanID)  BSP_SOCP_DecoderAllocDestChan(pAttr ,pDestChanID)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_DecoderSetSrcChanAttr
 ��������  :�˽ӿ����ĳһ����Դͨ�������ã����غ���ִ�еĽ����
 �������  : u32SrcChanID:������Դͨ��ID
                         pInputAttr:������Դͨ�������ṹ��ָ��
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:����Դͨ�����óɹ�
                             SOCP_ERROR:����Դͨ������ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_DecoderSetSrcChanAttr ( BSP_U32 u32SrcChanID,SOCP_DECODER_SRC_CHAN_S *pInputAttr);
#define  DRV_SOCP_DecoderSetSrcChanAttr(u32SrcChanID, pInputAttr)   BSP_SOCP_DecoderSetSrcChanAttr(u32SrcChanID, pInputAttr)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_DecoderGetErrCnt
 ��������  :�˽ӿڸ�������ͨ���������쳣����ļ���ֵ��
 �������  : u32ChanID:������ͨ��ID
                         pErrCnt:�������쳣�����ṹ��ָ��
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:�����쳣�����ɹ�
                             SOCP_ERROR:�����쳣����ʧ��
*****************************************************************************/
extern  BSP_S32 BSP_SOCP_DecoderGetErrCnt (BSP_U32 u32ChanID, SOCP_DECODER_ERROR_CNT_S *pErrCnt);
#define  DRV_SOCP_DecoderGetErrCnt(u32ChanID, pErrCnt)   BSP_SOCP_DecoderGetErrCnt ( u32ChanID, pErrCnt)

/*****************************************************************************
 �� �� ��  : BSP_SOCP_FreeChannel
 ��������  : �˽ӿڸ���ͨ��ID�ͷŷ���ı����ͨ����
 �������  : u32ChanID:ͨ��ID��
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:ͨ���ͷųɹ���
             SOCP_ERROR:ͨ���ͷ�ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_FreeChannel(BSP_U32 u32ChanID);
#define  DRV_SOCP_FreeChannel(u32ChanID)  BSP_SOCP_FreeChannel(u32ChanID)

/*****************************************************************************
 �� �� ��  : BSP_SOCP_RegisterEventCB
 ��������  : �˽ӿ�Ϊ����ͨ��ע���¼��ص�������
 �������  : u32ChanID:ͨ��ID��
             EventCB:�¼��ص��������ο�socp_event_cb��������
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:ע���¼��ص������ɹ���
             SOCP_ERROR:ע���¼��ص�����ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_RegisterEventCB(BSP_U32 u32ChanID, socp_event_cb EventCB);
#define  DRV_SOCP_RegisterEventCB( u32ChanID,  EventCB) BSP_SOCP_RegisterEventCB( u32ChanID,  EventCB)

/*****************************************************************************
 �� �� ��  : BSP_SOCP_Start
 ��������  : �˽ӿ�����Դͨ��������������߽��롣
 �������  : u32SrcChanID:Դͨ��ID��
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:�������������ɹ���
             SOCP_ERROR:������������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_Start(BSP_U32 u32SrcChanID);
#define  DRV_SOCP_Start( u32SrcChanID)   BSP_SOCP_Start( u32SrcChanID)

/*****************************************************************************
 �� �� ��  : BSP_SOCP_Stop
 ��������  : �˽ӿ�����Դͨ����ֹͣ������߽��롣
 �������  : u32SrcChanID:Դͨ��ID��
 �������  : �ޡ�
 �� �� ֵ  : SOCP_OK:��������ֹͣ�ɹ���
             SOCP_ERROR:��������ֹͣʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_SOCP_Stop(BSP_U32 u32SrcChanID);
#define DRV_SOCP_Stop( u32SrcChanID)  BSP_SOCP_Stop( u32SrcChanID)


/*****************************************************************************
 �� �� ��      : BSP_SOCP_SetTimeout
 ��������  :�˽ӿ����ó�ʱ��ֵ��
 �������  : u32Timeout:��ʱ��ֵ

 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:���ó�ʱʱ����ֵ�ɹ���
                             SOCP_ERROR:���ó�ʱʱ����ֵʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetTimeout (SOCP_TIMEOUT_EN_E eTmOutEn, BSP_U32 u32Timeout);
#define DRV_SOCP_SetTimeout(eTmOutEn,u32Timeout)   BSP_SOCP_SetTimeout(eTmOutEn,u32Timeout)

/*****************************************************************************
 �� �� ��   : BSP_SOCP_SetDecPktLgth
 ��������  :���ý�������ȼ���ֵ
 �������  : pPktlgth:��������ȼ�ֵ

 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:���óɹ���
                    ����ֵ:����ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetDecPktLgth(SOCP_DEC_PKTLGTH_S *pPktlgth);
#define DRV_SOCP_SetDecPktLgth(pPktlgth)   BSP_SOCP_SetDecPktLgth(SOCP_DEC_PKTLGTH_S *pPktlgth)

/*****************************************************************************
 �� �� ��   : BSP_SOCP_SetDebug
 ��������  :���ý���Դͨ����debugģʽ
 �������  : u32ChanID:ͨ��ID
                  u32DebugEn: debug��ʶ
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:���óɹ���
                     ����ֵ:����ʧ��
*****************************************************************************/
BSP_S32 BSP_SOCP_SetDebug(BSP_U32 u32DestChanID, BSP_U32 u32DebugEn);
#define DRV_SOCP_SetDebug(u32ChanID, u32DebugEn)   BSP_SOCP_SetDebug(BSP_U32 u32DestChanID, BSP_U32 u32DebugEn)

/*****************************************************************************
 �� �� ��   : BSP_SOCP_ChnSoftReset
 ��������  : Դͨ����λ
 �������  : u32ChanID:ͨ��ID
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:�踴λ�ɹ���
                     ����ֵ:��λʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_ChnSoftReset(BSP_U32 u32ChanID);
#define DRV_SOCP_ChnSoftReset(u32ChanID)   BSP_SOCP_ChnSoftReset(BSP_U32 u32ChanID)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_GetWriteBuff
 ��������  :�˽ӿ����ڻ�ȡд����buffer��
 �������  : u32SrcChanID:Դͨ��ID
                  pBuff:           :д����buffer

 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:��ȡд����buffer�ɹ���
                             SOCP_ERROR:��ȡд����bufferʧ��
*****************************************************************************/
extern  BSP_S32 BSP_SOCP_GetWriteBuff( BSP_U32 u32SrcChanID, SOCP_BUFFER_RW_S *pBuff);
#define  DRV_SOCP_GetWriteBuff( u32SrcChanID, pBuff)  BSP_SOCP_GetWriteBuff( u32SrcChanID, pBuff)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_WriteDone
 ��������  :�ýӿ��������ݵ�д�������ṩд�����ݵĳ��ȡ�
 �������  : u32SrcChanID:Դͨ��ID
                  u32WrtSize:   ��д�����ݵĳ���
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:д�����ݳɹ���
                             SOCP_ERROR:д������ʧ��
*****************************************************************************/
extern   BSP_S32  BSP_SOCP_WriteDone(BSP_U32 u32SrcChanID, BSP_U32 u32WrtSize);
#define  DRV_SOCP_WriteDone(u32SrcChanID, u32WrtSize)  BSP_SOCP_WriteDone(u32SrcChanID, u32WrtSize)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_RegisterRdCB
 ��������  :�ýӿ�����ע���RD�������ж�ȡ���ݵĻص�������
 �������  : u32SrcChanID:Դͨ��ID
                  RdCB:  �¼��ص�����
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:ע��RD���λ����������ݻص������ɹ���
                             SOCP_ERROR:ע��RD���λ����������ݻص�����ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_RegisterRdCB(BSP_U32 u32SrcChanID, socp_rd_cb RdCB);
#define DRV_SOCP_RegisterRdCB( u32SrcChanID,  RdCB) BSP_SOCP_RegisterRdCB( u32SrcChanID,  RdCB)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_GetRDBuffer
 ��������  :�ô˽ӿ����ڻ�ȡRD buffer������ָ�롣
 �������  : u32SrcChanID:Դͨ��ID
                  pBuff:  RD buffer
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:��ȡRD���λ������ɹ�
                             SOCP_ERROR:��ȡRD���λ�����ʧ��
*****************************************************************************/
extern  BSP_S32 BSP_SOCP_GetRDBuffer( BSP_U32 u32SrcChanID,SOCP_BUFFER_RW_S *pBuff);
#define DRV_SOCP_GetRDBuffer(  u32SrcChanID,pBuff)  BSP_SOCP_GetRDBuffer(  u32SrcChanID,pBuff)


/*****************************************************************************
 �� �� ��      : BSP_SOCP_ReadRDDone
 ��������  :�˽ӿ������ϲ�֪ͨSOCP��������RD buffer��ʵ�ʶ�ȡ�����ݡ�
 �������  : u32SrcChanID:Դͨ��ID
                  u32RDSize:  ��RD buffer��ʵ�ʶ�ȡ�����ݳ���
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:��ȡRDbuffer�е����ݳɹ�
                             SOCP_ERROR:��ȡRDbuffer�е�����ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_ReadRDDone(BSP_U32 u32SrcChanID, BSP_U32 u32RDSize);
#define DRV_SOCP_ReadRDDone( u32SrcChanID,  u32RDSize)   BSP_SOCP_ReadRDDone( u32SrcChanID,  u32RDSize)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_RegisterReadCB
 ��������  :�ýӿ�����ע������ݵĻص�������
 �������  : u32DestChanID:Ŀ��ͨ��ID
                  ReadCB: �¼��ص�����
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:ע������ݻص������ɹ�
                             SOCP_ERROR:ע������ݻص�����ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_RegisterReadCB( BSP_U32 u32DestChanID, socp_read_cb ReadCB);
#define   DRV_SOCP_RegisterReadCB(  u32DestChanID,  ReadCB)  BSP_SOCP_RegisterReadCB(  u32DestChanID,  ReadCB)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_RegisterReadCB
 ��������  :�ô˽ӿ����ڻ�ȡ�����ݻ�����ָ�롣
 �������  : u32DestChanID:Ŀ��ͨ��ID
                  ReadCB: ������buffer
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:��ȡ�����ݻ������ɹ���
                             SOCP_ERROR:��ȡ�����ݻ������ɹ���
*****************************************************************************/
extern BSP_S32 BSP_SOCP_GetReadBuff( BSP_U32 u32DestChanID,SOCP_BUFFER_RW_S *pBuffer);
#define   DRV_SOCP_GetReadBuff(  u32DestChanID,  pBuffer)  BSP_SOCP_GetReadBuff(  u32DestChanID,  pBuffer)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_ReadDataDone
 ��������  :�ýӿ������ϲ����SOCP��������Ŀ��ͨ���ж��ߵ�ʵ�����ݡ�
 �������  : u32DestChanID:Ŀ��ͨ��ID
                  u32ReadSize: �Ѷ������ݵĳ���
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:�����ݳɹ���
                             SOCP_ERROR:������ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_ReadDataDone(BSP_U32 u32DestChanID,BSP_U32 u32ReadSize);
#define  DRV_SOCP_ReadDataDone( u32DestChanID, u32ReadSize)  BSP_SOCP_ReadDataDone( u32DestChanID, u32ReadSize)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_SetBbpEnable
 ��������  :ʹ�ܻ�ֹͣBBPͨ����
 �������  : bEnable:ͨ��ID
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:���óɹ���
                   ����ֵ:����ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetBbpEnable(BSP_BOOL bEnable);
#define  DRV_SOCP_SetBbpEnable( bEnable)  BSP_SOCP_SetBbpEnable(BSP_BOOL bEnable)

/*****************************************************************************
 �� �� ��      : BSP_SOCP_SetBbpDsMode
 ��������  :����BBP DSͨ�������������ģʽ��
 �������  : eDsMode:DSͨ���������ʱ����ģʽ����
 �������  : �ޡ�
 �� �� ֵ      : SOCP_OK:���óɹ���
                   ����ֵ:����ʧ��
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetBbpDsMode(SOCP_BBP_DS_MODE_E eDsMode);
#define  DRV_SOCP_SetBbpDsMode( eDsMode)  BSP_SOCP_SetBbpDsMode(SOCP_BBP_DS_MODE_E eDsMode)

/*****************************************************************************
* �� �� ��  : BSP_SOCP_GetRsvChnScope
* ��������  : ��ñ���ͨ��ID�ķ�Χ
* �������  : ��
* �������  : pScope,����Դ����ͨ����ID��Χ
* �� �� ֵ  :
*****************************************************************************/
extern BSP_S32 BSP_SOCP_GetRsvChnScope(SOCP_ENCSRC_RSVCHN_SCOPE_S *pScope);
#define  DRV_SOCP_GetRsvChnScope(pScope)  BSP_SOCP_GetRsvChnScope(SOCP_ENCSRC_RSVCHN_SCOPE_S *pScope)

/*****************************************************************************
* �� �� ��  : DRV_SOCP_SET_HIFICHAN
* ��������  : ����̶�ͨ��
* �������  : Ŀ��ͨ��ID
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
extern BSP_S32 DRV_SOCP_SET_HIFICHAN(BSP_U32 u32DestChanID);

/*****************************************************************************
* �� �� ��  : DRV_SOCP_START_HIFICHAN
* ��������  : �����̶�ͨ��
* �������  : ��
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
extern BSP_VOID DRV_SOCP_START_HIFICHAN(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_SOCP_StartDsp
* ��������  :enable DSP channel
* �������  : 
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
BSP_VOID BSP_SOCP_StartDsp(BSP_VOID);


/*****************************************************************************
* �� �� ��  : BSP_SOCP_StopDsp
* ��������  :disable DSP channel
* �������  : 
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
BSP_S32 BSP_SOCP_StopDsp(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_SOCP_StartTdDsp
* ��������  :enable Tds DSP channel
* �������  : 
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
BSP_VOID BSP_SOCP_StartTdDsp(BSP_VOID);


/*****************************************************************************
* �� �� ��  : BSP_SOCP_StopTdDsp
* ��������  :disable Tds DSP channel
* �������  : 
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
BSP_S32 BSP_SOCP_StopTdDsp(BSP_VOID);


/*****************************************************************************
* �� �� ��  : BSP_SOCP_GetLogCfg
*
* ��������  : ��ȡLOG2.0����
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : SOCP_ENC_DST_BUF_LOG_CFG_STRU *
*****************************************************************************/
extern SOCP_ENC_DST_BUF_LOG_CFG_STRU * BSP_SOCP_GetLogCfg();

/*****************************************************************************
* �� �� ��  : BSP_SOCP_EncDstBufFlush
*
* ��������  : SOCP����Ŀ��buffer���������������
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : BSP_S32 BSP_OK:�ɹ� BSP_ERROR:ʧ�� 
*****************************************************************************/
extern BSP_S32 BSP_SOCP_EncDstBufFlush();
#define DRV_SOCP_EncDstBufFlush() BSP_SOCP_EncDstBufFlush()
/*****************************************************************************
* �� �� ��  : BSP_SOCP_SaveEncDstBufInfo
*
* ��������  : ����SOCP����Ŀ��buffer����������Ϣ
*
* �������  : SOCP_LOG_EXC_INFO_STRU * logExcInfo
*
* �������  : ��
*
* �� �� ֵ  : BSP_S32 BSP_OK:�ɹ� BSP_ERROR:ʧ�� 
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SaveEncDstBufInfo(SOCP_LOG_EXC_INFO_STRU * logExcInfo);


/*************************SOCP END************************************/

/*************************CIPHER START**********************************/

/* ������ */
#define CIPHER_SUCCESS                  0

#define CHN_BUSY                    0
#define CHN_FREE                    1

/* CIPHER֧�ֵ��㷨����,ע��ö��ֵ��Ĵ�������ֵһ�� */
typedef enum tagCIPHER_ALGORITHM_E
{
    CIPHER_ALG_NULL = 0,        /* ���㷨 */
    CIPHER_ALG_SNOW3G  = 1,     /* SNOW3G�㷨 */
    CIPHER_ALG_AES_128 = 2,     /* AES128�㷨 */
    CIPHER_ALG_AES_192 = 3,     /* AES192�㷨 */
    CIPHER_ALG_AES_256 = 4,     /* AES256�㷨 */
    CIPHER_ALG_BUTTOM
}CIPHER_ALGORITHM_E;

typedef enum tagKDF_SHA_KEY_SOURCE_E
{
    SHA_KEY_SOURCE_DDR,             //sha_key��Դ��input pointָ���ĵ�ַ�ռ�
    SHA_KEY_SOURCE_KEYRAM,          //sha_key��Դ���ڲ�KeyRam
    SHA_KEY_SOURCE_FOLLOW,          //sha_key��Դ���ϴ�KDF������sha_key
    SHA_KEY_SOURCE_RESULT,          //sha_key��Դ����һ��KDF�����Ľ��
    SHA_KEY_SOURCE_MAX
}KDF_SHA_KEY_SOURCE_E;

typedef enum tagKDF_SHA_S_SOURCE_E
{
    SHA_S_SOURCE_DDR,               //sha_s��Դ��input pointָ���ĵ�ַ�ռ�
    SHA_S_SOURCE_KEYRAM,            //sha_s��Դ���ڲ�KeyRam
    SHA_S_SOURCE_MAX
}KDF_SHA_S_SOURCE_E;

/**************************************************************************
  ö�ٶ���
**************************************************************************/
enum CIPHER_SECURITY_CHANNEL_ENUM
{
    CIPHER_SECURITY_CHANNEL_0            = 0,                /*ͨ��0*/
    CIPHER_SECURITY_CHANNEL_1            = 1,                /*ͨ��1*/
    CIPHER_SECURITY_CHANNEL_2            = 2,                /*ͨ��2*/
    CIPHER_SECURITY_CHANNEL_3            = 3,                /*ͨ��3*/

    CIPHER_SECURITY_CHANNEL_BUTT
};

/* ͨ����*/
#define LTE_SECURITY_CHANNEL_NAS    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_RRC    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_UL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_DL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_UL_DRB CIPHER_SECURITY_CHANNEL_0
#define LTE_SECURITY_CHANNEL_DL_DRB CIPHER_SECURITY_CHANNEL_3

/* CIPHER������������ */
typedef enum tagCIPHER_SINGLE_OPT_E
{
    /* ������������,����ö��ֵ��Ĵ�������ֵһ�� */
    CIPHER_OPT_ENCRYPT = 0x0,   /* ���� */
    CIPHER_OPT_DECRYPT = 0x1,   /* ���� */
    CIPHER_OPT_PROTECT_SMAC = 0x2,   /* �������������MAC����Ϊ2Byte */
    CIPHER_OPT_PROTECT_LMAC = 0x3,   /* �������������MAC����Ϊ4Byte */
    CIPHER_OPT_CHECK_PRE_SMAC  = 0x4,   /* ��������MAC����Ϊ2Byte��λ��������ǰ�� */
    CIPHER_OPT_CHECK_POST_SMAC = 0x5,   /* ��������MAC����Ϊ2Byte��λ�������ݺ��� */
    CIPHER_OPT_CHECK_PRE_LMAC  = 0x6,   /* ��������MAC����Ϊ4Byte��λ��������ǰ�� */
    CIPHER_OPT_CHECK_POST_LMAC = 0x7,   /* ��������MAC����Ϊ4Byte��λ�������ݺ��� */
    CIPHER_SINGLE_OPT_BUTTOM
}CIPHER_SINGLE_OPT_E;


/* CIPHER������������ */     //PDCP ���������������
typedef enum tagCHPHER_RELA_OPT_E
{
    /*  ������������ */
    CIPHER_PDCP_PRTCT_ENCY,     /*PDCP��������(����)*/
    CIPHER_PDCP_DECY_CHCK,      /*PDCP���ܼ��(����)*/

    CIPHER_NAS_ENCY_PRTCT,      /* NAS����ܱ���(����) */
    CIPHER_NAS_CHCK_DECY,       /* NAS�������(����) */

	CIPHER_RELA_OPT_BUTTOM
}CIPHER_RELA_OPT_E;


/* CIPHER��Կ����, ע��ö��ֵ��Ĵ�������ֵһ�� */
typedef enum tagCIPHER_KEY_LEN_E
{
    CIPHER_KEY_L128 = 0,        /* ��Կ����128 bit */
    CIPHER_KEY_L192 = 1,        /* ��Կ����192 bit */
    CIPHER_KEY_L256 = 2,        /* ��Կ����256 bit */
    CIPHER_KEY_LEN_BUTTOM
}CIPHER_KEY_LEN_E;


/* CIPHER�����ύ���� */
typedef enum tagCIPHER_SUBMIT_TYPE_E
{
    CIPHER_SUBM_NONE = 0,           /* �����κ�֪ͨ */
    CIPHER_SUBM_BLK_HOLD = 1,       /* ѭ����ѯ�ȴ���� */
    CIPHER_SUBM_CALLBACK = 2,       /* �ص�����֪ͨ */
    CIPHER_SUBM_BUTTOM
}CIPHER_SUBMIT_TYPE_E;

/* CIPHER֪ͨ���� */
typedef enum tagCIPHER_NOTIFY_STAT_E
{
    CIPHER_STAT_OK = 0,           /* �ɹ���� */
    CIPHER_STAT_CHECK_ERR = 1,    /* �����Լ����� */
    CIPHER_STAT_BUTTOM
}CIPHER_NOTIFY_STAT_E;

typedef enum CIPHER_HDR_BIT_ENUM
{
    CIPHER_HDR_BIT_TYPE_0              = 0,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ0�����������˹���*/
    CIPHER_HDR_BIT_TYPE_5              = 1,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ1���������˹��ܣ���Ӧ����ͷ��������Countֵ�ĵ�5λ�����뱣��*/
    CIPHER_HDR_BIT_TYPE_7              = 2,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ1���������˹��ܣ���Ӧ����ͷ��������Countֵ�ĵ�7λ�����뱣��*/
    CIPHER_HDR_BIT_TYPE_12             = 3,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ2���������˹��ܣ���Ӧ����ͷ��������Countֵ�ĵ�12λ�����뱣��*/

    CIPHER_APPEND_HDR_BIT_TYPE_5       = 4,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ1���������˹��ܣ���Ӧ����ͷ��������Countֵ�ĵ�5λ�������뱣����ֻ����*/
    CIPHER_APPEND_HDR_BIT_TYPE_7       = 5,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ1���������˹��ܣ���Ӧ����ͷ��������Countֵ�ĵ�7λ�������뱣����ֻ����*/
    CIPHER_APPEND_HDR_BIT_TYPE_12      = 6,     /*��Ӧ��ͷ�򸽼�ͷ�ֽڳ���Ϊ2���������˹��ܣ���Ӧ����ͷ��������Countֵ�ĵ�12λ�������뱣����ֻ����*/

    CIPHER_HDR_BIT_TYPE_BUTT
}CIPHER_HDR_E;

/* �㷨������Ϣ */
typedef struct tagCIHPER_ALG_INFO_S
{
    BSP_U32 u32KeyIndexSec;
    BSP_U32 u32KeyIndexInt;
    CIPHER_ALGORITHM_E enAlgSecurity;          /* ��ȫ�����㷨ѡ��AEC�㷨����SNOW3G�㷨 */
    CIPHER_ALGORITHM_E enAlgIntegrity;         /* �����Բ����㷨ѡ��AEC�㷨����SNOW3G�㷨 */
}CIHPER_ALGKEY_INFO_S;


/*�������������Ϣ*/
typedef struct tagACC_SINGLE_CFG_S
{
    BSP_U32 u32BearId;
    BSP_U32 u32AppdHeaderLen;
    BSP_U32 u32HeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* �Ƿ��ǵ��ڴ�� */
    BSP_U32 u32BlockLen;                /* ����ǵ����ڴ棬��Ҫ֪������*/
    BSP_U32 u32Offset;                  /* ʹ�����ݾ������ݰ���ʼ��ַƫ��*/
    BSP_U32 u32OutLen;                  /* ʹ�õ������ݳ���*/
    BSP_U32 u32Aph;                      /*���Ӱ�ͷ��*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* �����������㷨��Կ������Ϣ */
} ACC_SINGLE_CFG_S;

/* CIPHER������Ϣ���� */
typedef struct tagCIHPER_SINGLE_CFG_S
{
    CIPHER_SINGLE_OPT_E enOpt;          /* ������������(��DMA��������������㷨����) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* �Ƿ��ǵ��ڴ�� */
	BSP_U32 u32BlockLen;                /* ����ǵ����ڴ棬��Ҫ֪������*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* �����������㷨��Կ������Ϣ */
} CIHPER_SINGLE_CFG_S;

typedef struct tagCIHPER_RELA_CFG_S
{
    CIPHER_RELA_OPT_E enOpt;             /* ������������(��DMA��������������㷨����) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* �Ƿ��ǵ��ڴ�� */
	BSP_U32 u32BlockLen;                /* ����ǵ����ڴ棬��Ҫ֪������*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* ����������һ�����㷨��Կ������Ϣ */

} CIHPER_RELA_CFG_S;


typedef struct tagS_CONFIG_INFO_S
{
    KDF_SHA_S_SOURCE_E   enShaSSource;      //sha_s��Դ
    BSP_U32              u32ShaSIndex;       //sha_s��Դ��KeyRamʱ������keyRam�е�����
    BSP_U32              u32ShaSLength;     //sha_s�ĳ���
    BSP_VOID             *pSAddr;           //
}S_CONFIG_INFO_S;

typedef enum tagCIPHER_KEY_OUTPUT_E
{
    CIPHER_KEY_NOT_OUTPUT,    //�������Կ
    CIPHER_KEY_OUTPUT,        //�����Կ
    CIPHER_KEY_OUTPUT_BUTTOM
}CIPHER_KEY_OUTPUT_E;

typedef BSP_U32 CIPHER_KEY_LEN_E_U32;
typedef BSP_U32 CIPHER_KEY_OUTPUT_E_U32;

/* ��ȡKey���Ⱥ�Key���ݵĽṹ��*/
typedef struct
{
    CIPHER_KEY_LEN_E_U32    enKeyLen;//Ҫ��ȡ��Key�ĳ��ȣ����ϲ㴫��
    BSP_VOID                *pKeyAddr;//Keyֵ��ŵ�ַ
    CIPHER_KEY_LEN_E_U32    *penOutKeyLen;//�õ�ַ���ڴ��ʵ�ʷ��ص�Key�ĳ���
}KEY_GET_S;

/* KeyMakeʱ����ȡKey�ĳ��Ⱥ�Keyֵ*/
typedef struct
{
    CIPHER_KEY_OUTPUT_E_U32 enKeyOutput; //ָʾ�Ƿ����Key��stKeyGet��
    KEY_GET_S stKeyGet;
}KEY_MAKE_S;


/* ��ɻص��������Ͷ��� */
/*
u32ChNum   :  ͨ����;
u32SourAddr:  Դ��ַ
u32DestAddr:  Ŀ�ĵ�ַ
enStatus   :  ֪ͨ��״̬,����: ��� / ���ݴ������ / ��������Լ�����
u32Private   :  �û�˽������;
*/
typedef BSP_VOID (*CIPHER_NOTIFY_CB_T)(BSP_U32 u32ChNum, BSP_U32 u32SourAddr,
              BSP_U32 u32DestAddr, CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private);

typedef BSP_VOID (*CIPHER_FREEMEM_CB_T)(BSP_VOID* pMemAddr);

/**************************************************************************
  KDF���
**************************************************************************/

typedef struct tagKEY_CONFIG_INFO_S
{
    KDF_SHA_KEY_SOURCE_E enShaKeySource;    //sha_key��Դ
    BSP_U32              u32ShaKeyIndex;     //sha_key��Դ��KeyRamʱ������keyRam�е�����
    BSP_VOID             *pKeySourceAddr;   //
}KEY_CONFIG_INFO_S;


/*****************************************************************************
* �� �� ��  : BSP_Accelerator_DMA
*
* ��������  : Ϊָ��ͨ�����������������ڵ�����DMA����
*
* �������  : u32BdFifoAddr: BDFIFO�׵�ַ
*             pInMemMgr    : ����buffer�׵�ַ
*             pOutMemMgr   : ���buffer�׵�ַ
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_Accelerator_DMA(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,ACC_SINGLE_CFG_S *pstCfg);

/*****************************************************************************
* �� �� ��  : BSP_Accelerator_Cipher
*
* ��������  : Ϊָ��ͨ��������������������ҪCipher���м��ܵĲ���
*
* �������  : u32BdFifoAddr: BDFIFO�׵�ַ
*             pInMemMgr    : ����buffer�׵�ַ
*             pOutMemMgr   : ���buffer�׵�ַ
*             pstCfg       : Cipher������Ϣ
* �������   : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_Accelerator_Cipher(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
               ACC_SINGLE_CFG_S *pstCfg);


/**************************************************************************
 ��������
**************************************************************************/

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_RegistNotifyCB
*
* ��������  : ע��֪ͨ�ص�����
*
* �������  : pFunNotifyCb: ֪ͨ�ص�����
*
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*           : ʧ��:CIPHER_NOT_INIT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistNotifyCB(CIPHER_NOTIFY_CB_T pFunNotifyCb);

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_FreeMemCB
*
* ��������  : ע��Memory�ͷŻص�����
*
* �������  : u32Chn: ͨ����
*             bSrc:   0:����Buffer / 1:���Bufffer
*             pFunFreeMemCb: Memory�ͷŻص�����
*
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*           : ʧ��:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistFreeMemCB(BSP_U32 u32Chn, BSP_BOOL bSrc, CIPHER_FREEMEM_CB_T pFunFreeMemCb);

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_Purge
*
* ��������  : ������е�ǰָ��ͨ����CIPHER����(�����ӿ�,���purge�󷵻�)
*
* �������  : u32Chn: Ҫ�����ͨ����
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*           : ʧ��:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_Purge(BSP_U32 u32Chn);

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_SingleSubmitTask
*
* ��������  : ��ʼCIPHER ָ��ͨ���Ĺ��������ڵ���������
*
* �������  : u32Chn: ͨ����
*             pInMemMgr: ����buffer�ڴ����ṹָ��
*             pOutMemMgr:���buffer�ڴ����ṹָ��
*             pstCfg: Cipher������������
*             pstSubmAttr:�ύ������
*             u32Private:˽������
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*             ʧ��:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_SingleSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                   CIHPER_SINGLE_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_RelaSubmitTask
*
* ��������  : ��ʼCIPHER ָ��ͨ���Ĺ��������ڹ���������
*
* �������  : u32Chn: ͨ����
*             pInMemMgr: ����buffer�ڴ����ṹָ��
*             pOutMemMgr:���buffer�ڴ����ṹָ��
*             pstCfg: Cipher������������
*             enSubmAttr:�ύ������
*             u32Private:˽������
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*             ʧ��:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RelaSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                   CIHPER_RELA_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_SetKeyIndex
*
* ��������  : ������Կֵ
*
* �������  : pKeyAddr   : ��Կ��ַ
*             enKeyLen   : ��Կ����
*             u32KeyIndex: ��Կ����
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*           : ʧ��:CIPHER_NOT_INIT
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_ENUM
*           :      CIPHER_INVALID_KEY
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetKey(const BSP_VOID* pKeyAddr,CIPHER_KEY_LEN_E enKeyLen,BSP_U32 u32KeyIndex);

/*****************************************************************************
* �� �� ��  : BSP_CIPHER_GetKey
*
* ��������  : ����KeyIndex��ȡKey�ĵ�ַ
*
* �������  : u32KeyIndex: ��Կ����
* �������  : ��
* �� �� ֵ  : �ɹ�:����KeyIndex�õ���Key�ĵ�ַ
*           : ʧ��:��ȡKeyֵ���ڵ�ַʧ��,����0
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetKey(BSP_U32 u32KeyIndex, KEY_GET_S *pstKeyGet);

/*****************************************************************************
* �� �� ��  : CIPHER_DataBuffPara
*
* ��������  : ��ȡ�ϲ�������Ϣ��������Ϣ
*
* �������  : u32Chn        : ͨ����
*             bSrc          : TRUE:Դ��ַ���������� / FALSE:Ŀ�ĵ�ַ��������
*             u32BufOft     : ����bufferָ��ƫ��
*             u32LenOft     : Buffer����ƫ��
*             NextOft       : ��һ�ڵ�ƫ��
* �������  : ��
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*           : ʧ��:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetDataBufPara(BSP_U32 u32Chn, BSP_BOOL bSrc, BSP_U32 u32BufOft,
                                 BSP_U32 u32LenOft, BSP_U32 u32NextOft);

/*****************************************************************************
* �� �� ��  : CIPHER_GetCmplSrcBuff
*
* ��������  : ��ȡ�Ѿ�������ɵ�Դ���ݵ�ַ
*
* �������  : u32ChNum     : ͨ����
* �������  : pu32SourAddr : ������ɵ�Դ��ַ;
*             pu32DestAddr : ������ɵ�Ŀ�ĵ�ַ
*             penStatus    : ������ɵ�״̬
*             pu32Private  : ���ص�˽������
* �� �� ֵ  : �ɹ�:CIPHER_SUCCESS
*           : ʧ��:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_RD
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetCmplSrcBuff(BSP_U32 u32Chn, BSP_U32 *pu32SourAddr,
             BSP_U32 *pu32DestAddr, CIPHER_NOTIFY_STAT_E *penStatus, BSP_U32 *pu32Private);

/*****************************************************************************
* �� �� ��  : BSP_Accelerator_GetBdFifoAddr
*
* ��������  : ���ڻ�ȡ��ǰ���õ�BDFIFO�׵�ַ
*
* �������  : ��
*
* �������  : ��
* �� �� ֵ  : BDFIFO�׵�ַ
*****************************************************************************/
BSP_U32 BSP_Accelerator_GetBdFifoAddr(void);

/*****************************************************************************
* �� �� ��  : BSP_Accelerator_GetStatus
*
* ��������  : ��ȡ��ǰͨ��״̬
*
* �������  :  ��
* �������   : ��
* �� �� ֵ  : ͨ��æ/����
*****************************************************************************/
BSP_S32 BSP_Accelerator_GetStatus(void);

/*****************************************************************************
* �� �� ��  : BSP_Accelerator_Enable
*
* ��������  : ʹ���������
*
* �������  : u32BdFifoAddr: BDFIFO�׵�ַ
*
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
BSP_S32 BSP_Accelerator_Enable(BSP_U32 u32BdFifoAddr);

/*****************************************************************************
* �� �� ��  : BSP_KDF_KeyRamRead()
*
* ��������  : ��Key Ram�ж�ȡ����
*
* �������  : enKeyIndex: Key�������Ӹ���������ʼ��ȡ����
*             u32Length : ��ȡ���ݳ��ȣ���ByteΪ��λ
* �������  : pDestAddr : Ŀ�ĵ�ַ������KeyRam�ж�ȡ�����ݴ洢����λ��
* �� �� ֵ  : �ɹ�: BSP_OK
*           : ʧ��: BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyRamRead(BSP_VOID *pDestAddr, BSP_U32 u32KeyIndex ,BSP_U32 u32ReadLength);

/*****************************************************************************
* �� �� ��  : BSP_KDF_KeyRamWrite
*
* ��������  : ��KeyRam��д������
*
* �������  : enKeyIndex: Key�������Ӹ���������ʼд������
*             pSourAddr : Դ��ַ��д�뵽KeyRam�е����ݵ�ַ
*             u32Length : ���ݳ���
*
* �������  : ��
* �� �� ֵ  : �ɹ�: BSP_OK
*           : ʧ��: BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyRamWrite(BSP_U32 u32KeyIndex, BSP_VOID *pSourAddr, BSP_U32 u32Length);


/*****************************************************************************
* �� �� ��  : BSP_KDF_KeyMake
*
* ��������  : KDF���㣬���ڲ����µ�Key
*
* �������  : stKeyCfgInfo: KDF����ʱ����sha_key��������
            : stKeyCfgInfo: KDF����ʱ����sha_s��������
            : enDestIndex : KDF���������Key������KeyRam�е�λ��
* �������  : ��
* �� �� ֵ  : �ɹ�:BSP_OK
*           : ʧ��:BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyMake(KEY_CONFIG_INFO_S *pstKeyCfgInfo, S_CONFIG_INFO_S *pstSCfgInfo,
                        BSP_U32 u32DestIndex, KEY_MAKE_S *pstKeyMake);



/**************************************************************************
  �궨��
**************************************************************************/

/* ��ӡ���� */
#define CIPHER_PRNT_NOT           BSP_LOG_LEVEL_MAX       /* ����ӡ�κ���Ϣ */
#define CIPHER_PRNT_ERR           BSP_LOG_LEVEL_ERROR     /* ֻ��ӡ������Ϣ */
#define CIPHER_PRNT_WAR           BSP_LOG_LEVEL_WARNING   /* ֻ��ӡ����, �澯��Ϣ*/
#define CIPHER_PRNT_LOG           BSP_LOG_LEVEL_DEBUG     /* ��ӡ����, �澯, ������Ϣ */

/* ע��CIPHER������ҪΪ��ֵ */
#define CIPHER_ERROR_BASE               0x80000800

typedef enum tagCIPHER_ERR_CODE_E
{
    CIPHER_ERR_CODE_NULL_PTR = 1,
    CIPHER_ERR_CODE_NO_MEM ,
    CIPHER_ERR_CODE_NOT_INIT ,
    CIPHER_ERR_CODE_FIFO_FULL ,
    CIPHER_ERR_CODE_INVALID_CHN ,
    CIPHER_ERR_CODE_INVALID_OPT ,
    CIPHER_ERR_CODE_ALIGN_ERROR ,
    CIPHER_ERR_CODE_PURGING ,
    CIPHER_ERR_CODE_TIME_OUT,
    CIPHER_ERR_CODE_INVALID_ENUM,
    CIPHER_ERR_CODE_INVALID_RD,
    CIPHER_ERR_CODE_RD_NULL,
    CIPHER_ERR_CODE_INVALID_KEY,
    CIPHER_ERR_CODE_CHECK_ERROR,
    CIPHER_ERR_CODE_BDLEN_ERROR,
    CIPHER_ERR_CODE_INVALID_NUM,
    CIPHER_ERR_CODE_NO_KEY,
    CIPHER_ERR_CODE_KEYLEN_ERROR
}CIPHER_ERR_CODE_E;

#define CIPHER_NULL_PTR      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NULL_PTR))
#define CIPHER_NO_MEM        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_MEM))
#define CIPHER_NOT_INIT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NOT_INIT))
#define CIPHER_FIFO_FULL     ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_FIFO_FULL))
#define CIPHER_INVALID_CHN   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_CHN))
#define CIPHER_INVALID_OPT   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_OPT))
#define CIPHER_ALIGN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_ALIGN_ERROR))
#define CIPHER_PURGING       ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_PURGING))
#define CIPHER_TIME_OUT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_TIME_OUT))
#define CIPHER_INVALID_ENUM  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_ENUM))
#define CIPHER_INVALID_RD    ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_RD))
#define CIPHER_RDQ_NULL      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_RD_NULL))
#define CIPHER_INVALID_KEY   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_KEY))
#define CIPHER_CHECK_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_CHECK_ERROR))
#define CIPHER_BDLEN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_BDLEN_ERROR))
#define CIPHER_INVALID_NUM   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_NUM))
#define CIPHER_NO_KEY        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_KEY))
#define CIPHER_KEYLEN_ERROR  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_KEYLEN_ERROR))
#define CIPHER_UNKNOWN_ERROR ((BSP_S32)(CIPHER_ERROR_BASE|0xff))



/*************************CIPHER END **********************************/

/*************************WIFI START**********************************/

/*****************************************************************************
 �� �� ��  : WifiCalDataLen
 ��������  : WIFI������Ҫ���ݿ��ڴ�
 �������  : usLen - �û��������ݳ���Len
 �������  : ��
 �� �� ֵ  : ��������������ܳ���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��22��
    �޸�����   : �����ɺ���

*****************************************************************************/
extern unsigned short BSP_WifiCalDataLen(unsigned short usLen);
#define DRV_WifiCalDataLen(usLen) BSP_WifiCalDataLen(usLen)

/*****************************************************************************
 �� �� ��  : WIFI_TEST_CMD
 ��������  : ��������
 �������  : cmdStr�������ַ���
 �������  : ��
 ����ֵ    ����
*****************************************************************************/
extern void WIFI_TEST_CMD(char * cmdStr);

/*****************************************************************************
 �� �� ��  : WIFI_GET_TCMD_MODE
 ��������  : ��ȡ���������ģʽ
 �������  : ��
 �������  : 16��У׼����ģʽ
             17����ʾ���ڷ���ģʽ
             18����ʾ����ģʽ
 ����ֵ    ������ִ�е�״̬���ֵ
*****************************************************************************/
extern int WIFI_GET_TCMD_MODE(void);

/*****************************************************************************
 �� �� ��  : WIFI_POWER_START
 ��������  : WIFI�ϵ�
 �������  : ��
 �������  : ��
 ����ֵ    �� 0: execute ok
              1: execute failed
*****************************************************************************/
extern int WIFI_POWER_START(void);

/*****************************************************************************
 �� �� ��  : WIFI_POWER_SHUTDOWN
 ��������  : WIFI�µ�
 �������  : ��
 �������  : ��
 ����ֵ    �� 0: execute ok
              1: execute failed
*****************************************************************************/
extern int WIFI_POWER_SHUTDOWN(void);

/*****************************************************************************
 �� �� ��  : WIFI_GET_STATUS
 ��������  : WIFI״̬��ȡ
 �������  : ��
 �������  : ��
 ����ֵ    �� 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
extern int WIFI_GET_STATUS(void);

/*****************************************************************************
 �� �� ��  : WIFI_GET_RX_DETAIL_REPORT
 ��������  : get result of rx report: totalPkt, GoodPkt, ErrorPkt
 �������  : ��
 �������  : totalPkt��goodPkt��badPkt
 ����ֵ    ����
*****************************************************************************/
extern void WIFI_GET_RX_DETAIL_REPORT(int* totalPkt,int* goodPkt,int* badPkt);

/*****************************************************************************
 �� �� ��  : WIFI_GET_RX_PACKET_REPORT
 ��������  : get result of rx ucast&mcast packets
 �������  : ��
 �������  : ucastPkts��mcastPkts
 ����ֵ    ����
*****************************************************************************/
extern void  WIFI_GET_RX_PACKET_REPORT(unsigned int *ucastPkts, unsigned int *mcastPkts);

/*****************************************************************************
 �� �� ��  : WIFI_GET_PA_CUR_MODE
 ��������  : get the currrent PA mode of the wifi chip
 �������  : ��
 �������  : ��
 ����ֵ    ��0:  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
             -1: wifi chip is in abnormal mode
*****************************************************************************/
extern int WIFI_GET_PA_CUR_MODE(void);

/*****************************************************************************
 �� �� ��  : WIFI_GET_PA_MODE
 ��������  : get the support PA mode of wifi chip
 �������  : ��
 �������  : ��
 ����ֵ    ��0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
             -1: failed
*****************************************************************************/
extern int WIFI_GET_PA_MODE(void);

/*****************************************************************************
 �� �� ��  : WIFI_SET_PA_MODE
 ��������  : set the PA mode of wifi chip
 �������  : 0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
 �������  : ��
 ����ֵ    ��0: success
             -1: failed
*****************************************************************************/
extern int WIFI_SET_PA_MODE(int wifiPaMode);

/*****************************************************************************
 �� �� ��  : DRV_WIFI_DATA_RESERVED_TAIL
 ��������  : WIFI������Ҫ���ݿ��ڴ�
 �������  : usLen - �û��������ݳ���Len
 �������  : ��
 �� �� ֵ  : ��������β��Ԥ���ĳ���
*****************************************************************************/
extern unsigned int DRV_WIFI_DATA_RESERVED_TAIL(unsigned int len);

/*h00106354 20120201 �������ؽӿ� add start */
/*****************************************************************************
 �� �� ��  : DRV_WIFI_SET_RX_FCTL
 ��������  : ����WIFI�������ر�ʶ
 �������  : para1��para2
 �������  : ��
 �� �� ֵ  : BSP_OK/BSP_ERROR
*****************************************************************************/
extern unsigned long DRV_WIFI_SET_RX_FCTL(unsigned long para1, unsigned long para2);

/*****************************************************************************
 �� �� ��  : DRV_WIFI_CLR_RX_FCTL
 ��������  : ���WIFI�������ر�ʶ
 �������  : para1��para2
 �������  : ��
 �� �� ֵ  : 1 : ��
             0 : ��
*****************************************************************************/
extern unsigned long DRV_WIFI_CLR_RX_FCTL(unsigned long para1, unsigned long para2);

/*****************************************************************************
 �� �� ��  : DRV_AT_GET_USER_EXIST_FLAG
 ��������  : ���ص�ǰ�Ƿ���USB���ӻ���WIFI�û�����(C�˵���)
 �������  : ��
 �������  : ��
 �� �� ֵ  : 1 : ��
             0 : ��
*****************************************************************************/
extern unsigned long  DRV_AT_GET_USER_EXIST_FLAG(void);


unsigned long USB_ETH_DrvSetRxFlowCtrl   (unsigned long ulParam1, unsigned long ulParam2);
unsigned long USB_ETH_DrvClearRxFlowCtrl (unsigned long ulParam1, unsigned long ulParam2);
unsigned long USB_ETH_DrvSetHostAssembleParam(unsigned long ulHostOUtTimeout);
unsigned long USB_ETH_DrvSetDeviceAssembleParam(unsigned long ulEthTxMinNum,
											    unsigned long ulEthTxTimeout,
											    unsigned long ulEthRxMinNum,
											    unsigned long ulEthRxTimeout);
/*h00106354 20120201 �������ؽӿ� add end */

/*************************WIFI END************************************/

/*************************AUDIO START*********************************/

/*AUDIO IOCTL�ӿ�������*/
#define AUDIO_IOCTL_BASE                    (0x30000000)
#define IOCTL_AUDIO_SAMPLE_RATE_SET         (AUDIO_IOCTL_BASE+1)
#define IOCTL_AUDIO_PCM_MODE_SET            (AUDIO_IOCTL_BASE+7)
#define IOCTL_AUDIO_IN_DEV_SELECT           (AUDIO_IOCTL_BASE+11)
#define IOCTL_AUDIO_OUT_DEV_SELECT          (AUDIO_IOCTL_BASE+12)
#define IOCTL_AUDIO_OUT_DEV_UNSELECT        (AUDIO_IOCTL_BASE+13)
#define IOCTL_AUDIO_IN_DEV_UNSELECT         (AUDIO_IOCTL_BASE+14)
#define IOCTL_AUDIO_VOICE_OPEN              (AUDIO_IOCTL_BASE+23)
#define IOCTL_AUDIO_VOICE_CLOSE             (AUDIO_IOCTL_BASE+24)

typedef enum
{
    NODEV=0x00,
    HEADSET=0x01,
    HANDSET=0x02,
    HANDFREE=0x04
}AUDIO_DEV_TYPE;
typedef enum
{
    SAMPLE_RATE_8K=0,
    SAMPLE_RATE_11K025=1,
    SAMPLE_RATE_12K=2,
    SAMPLE_RATE_16K=3,
    SAMPLE_RATE_22K05=4,
    SAMPLE_RATE_24K=5,
    SAMPLE_RATE_32K=6,
    SAMPLE_RATE_44K1=7,
    SAMPLE_RATE_48K=8,
    SAMPLE_RATE_INVALID=0xFFFF
}AUDIO_SAMPLE_RATE;


/********************************************************************************************************
 �� �� ��  : BSP_AUDIO_CodecOpen
 ��������  : ��һ��audio�豸������codec���г�ʼ����
 �������  : devname���ò����̶�Ϊ"/dev/codec0"��
                           flags���ò�����ʾ��д��ʽ���̶�Ϊ0��
                           mode���ò�����ʾ�½��ļ���ʽ���̶�Ϊ0����
  �������  : �ޡ�
 �� �� ֵ  : �Ǹ�ֵ:  �豸�����
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_AUDIO_CodecOpen(const char *devname, int flags, int mode);
#define DRV_CODEC_OPEN(devname, flags, mode)    BSP_AUDIO_CodecOpen(devname, flags, mode)

/********************************************************************************************************
 �� �� ��  : BSP_AUDIO_Codec_ioctl
 ��������  : codec�豸�����ֿ��ơ�
 �������  : devid���豸�����
                           cmd�������֡�
                           arg�����������
  �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_AUDIO_Codec_ioctl(int devid, int cmd, int arg);
#define DRV_CODEC_IOCTL(devid, cmd, arg)   BSP_AUDIO_Codec_ioctl(devid, cmd, arg)

/*************************AUDIO END***********************************/

/*************************USB START***********************************/
/* �豸ö�����˿ڸ��� */
#define DYNAMIC_PID_MAX_PORT_NUM        17

typedef  enum
{
    FLOW_CON,     /* ����        */
    PDP_STATUS,   /* PDP������ */
    SIGNAL_LEVEL, /* �ź�ǿ��    */
    PKT_STATICS,  /* ����ͳ��    */
    AT_CMD,       /* AT����      */
    PS_NOTIFY,    /* Notify����  */
    RESERVE       /* ����        */
}NDIS_NOTICE;

typedef unsigned long (*pPCSCFUNC)(unsigned long CmdType, unsigned char *pApdu, unsigned long ApduLen);

/* NV��50091�ṹ�������豸��Ҫö�ٵĶ˿���̬ */
typedef struct
{
    unsigned long ulStatus;
    unsigned char aucFirstPortStyle[DYNAMIC_PID_MAX_PORT_NUM];  /* �豸�л�ǰ�˿���̬ */
    unsigned char aucRewindPortStyle[DYNAMIC_PID_MAX_PORT_NUM]; /* �豸�л���˿���̬ */
    unsigned char reserved[22];
}DRV_DYNAMIC_PID_TYPE_STRU;

typedef enum
{
    WWAN_WCDMA    = 1,/*WCDMAģʽ*/
    WWAN_CDMA    = 2  /*CDMAģʽ*/
}NDIS_WWAN_MODE;


/*�ܽ��źŵĶ���*/
#define IO_CTRL_FC                      (0x02)
#define IO_CTRL_DSR                     (0x20)
#define IO_CTRL_DTR                     (0x01)
#define IO_CTRL_RFR                     (0x04)
#define IO_CTRL_CTS                     (0x10)
#define IO_CTRL_RI                      (0x40)
#define IO_CTRL_DCD                     (0x80)

typedef int  (*pComRecv)(unsigned char  uPortNo, unsigned char* pData, unsigned short uslength);

typedef int  (*pUartRecv)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);

typedef long (*pUsbStatus)(unsigned char ucStatus);

typedef long  (*pUsbState)(unsigned char ucStatus);

typedef int  (*pComStatusNotify)(unsigned char uStatus);

/* MODEM �ܽ��źŽṹ�嶨��*/
typedef struct tagMODEM_MSC_STRU
{
    BSP_U32 OP_Dtr     :    1;          /*DTR CHANGE FLAG*/
    BSP_U32 OP_Dsr     :    1;          /*DSR CHANGE FLAG*/
    BSP_U32 OP_Cts     :    1;          /*CTSCHANGE FLAG*/
    BSP_U32 OP_Rts     :    1;          /*RTS CHANGE FLAG*/
    BSP_U32 OP_Ri      :    1;          /*RI CHANGE FLAG*/
    BSP_U32 OP_Dcd     :    1;          /*DCD CHANGE FLAG*/
    BSP_U32 OP_Fc      :    1;          /*FC CHANGE FLAG*/
    BSP_U32 OP_Brk     :    1;          /*BRK CHANGE FLAG*/
    BSP_U32 OP_Spare   :    24;         /*reserve*/
    BSP_U8   ucDtr;                     /*DTR  VALUE*/
    BSP_U8   ucDsr;                     /*DSR  VALUE*/
    BSP_U8   ucCts;                     /*DTS VALUE*/
    BSP_U8   ucRts;                     /*RTS  VALUE*/
    BSP_U8   ucRi;                      /*RI VALUE*/
    BSP_U8   ucDcd;                     /*DCD  VALUE*/
    BSP_U8   ucFc;                      /*FC  VALUE*/
    BSP_U8   ucBrk;                     /*BRK  VALUE*/
    BSP_U8   ucBrkLen;                  /*BRKLEN VALUE*/
} MODEM_MSC_STRU, *PMODEM_MSC_STRU,AT_DCE_MSC_STRU;

/*Э��ջBULK���ݽ��ջص��������Ͷ���*/
typedef int (*USB_MODEM_RECV_CALL)
(
    unsigned char ucPortType,          /*�˿ں�*/
    unsigned char ucDlci,                 /*��·��*/
    unsigned char *pData,               /*����ָ��*/
    unsigned short usLen                /*���ݳ���*/
);


/*Э��ջ��Ϣͨ��״̬�仯���ݽ��ջص���������*/
typedef int (*USB_MODEM_AT_Sig)
(
    unsigned char ucPortType,          /*�˿ں�*/
    unsigned char ucDlci,                   /*��·��*/
    MODEM_MSC_STRU* data  /*ָ��MODEM �ܽ��źŽṹ���ָ��*/
);


/*Э��ջ�����������ص�����*/
typedef int (*USB_MODEM_IN_OUT)
(
    unsigned char ucPortType,           /*�˿ں�*/
    unsigned char ucDlci                   /*��·��*/
);

/*Э��ջ�����ܸ��ƺ�������*/
typedef void (*USB_MODEM_COPY)
(
    unsigned char   *pDest,
    unsigned char   *pSrc,
    unsigned long   ulLen
);


typedef struct
{
    unsigned int    GateWayIpAddr;  /* PDP�����ȡ����Զ������IP��ַ      */
    unsigned int    LocalIpAddr;    /* PDP�����ȡ���ķ��䵽MODEM��IP��ַ */
    unsigned int    MaskAddr;       /* �������� */
    unsigned int    speed;          /* �����ٶȣ���λbps */
    unsigned char   ActiveSatus;    /* ��������0Ϊ�ɹ�������Ϊʧ�� */
}NDIS_PDP_STRU,*pNDIS_PDP_STRU;


typedef struct
{
    unsigned int    currentTx;    /* CURRENT_TX */
    unsigned int    currentRx;    /* CURRENT_RX */
    unsigned int    maxTx;        /* MAX_TX */
    unsigned int    maxRx;        /* MAX_RX */
}NDIS_RATE_STRU,*pNDIS_RATE_STRU;


typedef struct
{
    unsigned int     TxOKCount;         /*���Ͱ���*/
    unsigned int     RxOKCount;         /*���հ���*/
    unsigned int     TxErrCount;        /*���ʹ���*/
    unsigned int     RxErrCount;        /*���մ���*/
    unsigned int     TxOverFlowCount;   /*�����������*/
    unsigned int     RxOverFlowCount;   /*�����������*/
    unsigned int     MaxTx;         /*��������*/
    unsigned int     MaxRx;         /*��������*/
} NDIS_PKT_STATISTICS, *pNDIS_PKT_STATISTICS;

typedef struct
{
    unsigned char   *pucAtCmd;          /*ָ��AT�����ָ��*/
    unsigned short   usLen;             /*AT����ĳ���*/
}NDIS_AT_CMD_STRU, *pNDIS_AT_CMD_STRU;

/* �ź��ϱ�����ʹ�õĽṹ��*/
typedef struct
{
    NDIS_NOTICE                 type;
    union
    {
        NDIS_PDP_STRU           pdp;
        NDIS_PKT_STATISTICS     pktStatics;
        unsigned char           flowFlag;   /*  1:���أ�0�������� */
        int                     dbm;
        NDIS_AT_CMD_STRU        atCmd;
    }NDIS_STATUS_DATA;

} NDIS_PRO_STRU, *pNDIS_PRO_STRU;

typedef int (*USB_NDIS_RECV_CALL)
(
    unsigned char *pData, /* ���ջ�����ָ��,����ΪIP���� */
    unsigned short usLen  /* ���ݳ��� */
);

typedef int (*USB_NDIS_BRK)
(
    void
);

typedef int (*USB_NDIS_EST)
(
    unsigned char *pData /* ���ջ�����ָ�룬����Ϊ�������ù������ */
);

typedef int (*USB_NDIS_PKT_STATISTICS_PERIOD)
(
    unsigned int period /*��λS */
);

typedef int (*USB_NDIS_GET_RATE)
(
    NDIS_RATE_STRU  *pRate
);

typedef int (*USB_NDIS_AT_CMD_RECV)
(
    unsigned char *pBuff,   /*ָ��AT����ָ��*/
    unsigned short usLen    /*AT�����*/
);

typedef unsigned int (*USB_NDIS_GET_WWAN_MODE)  /*��ȡ��ǰ����ģʽcdma/wcdma*/
(
    void
);

typedef unsigned long (*USB_NET_DEV_SET_MAX_TX_PACKET_NUMBER)
(
    unsigned long ulNumber
);

typedef void (*IPC_RECV_HANDLER)(unsigned char ucCidNo, unsigned char *pstData, unsigned int ulLength );

typedef int (*IPC_COM_RECV_HANDLER)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);

typedef unsigned long (*MNTN_ERRLOGREGFUN)(char * cFileName,unsigned int ulFileId, unsigned int ulLine,
                unsigned int ulErrNo, void * pBuf, unsigned int ulLen);

/*****************************************************************************
 �� �� ��  : BSP_USB_SetPid
 ��������  :
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern int BSP_USB_SetPid(unsigned char u2diagValue);
#define DRV_SET_PID(u2diagValue)    BSP_USB_SetPid(u2diagValue)

/*****************************************************************************
��������   BSP_USB_PortTypeQuery
��������:  ��ѯ��ǰ���豸ö�ٵĶ˿���ֵ̬
��������� stDynamicPidType  �˿���̬
��������� stDynamicPidType  �˿���̬
����ֵ��   0:    ��ѯ�ɹ�
           ��������ѯʧ��
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType);
#define DRV_SET_PORT_QUIRY(a)   BSP_USB_PortTypeQuery(a)

/*****************************************************************************
��������   BSP_USB_PortTypeValidCheck
��������:  �ṩ���ϲ��ѯ�豸�˿���̬���úϷ��Խӿ�
           1���˿�Ϊ��֧�����ͣ�2������PCUI�ڣ�3�����ظ��˿ڣ�4���˵���������16��
           5����һ���豸��ΪMASS��
��������� pucPortType  �˿���̬����
           ulPortNum    �˿���̬����
����ֵ��   0:    �˿���̬�Ϸ�
           �������˿���̬�Ƿ�
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
��������   BSP_USB_GetAvailabePortType
��������:  �ṩ���ϲ��ѯ��ǰ�豸֧�ֶ˿���̬�б�ӿ�
��������� ulPortMax    Э��ջ֧�����˿���̬����
�������:  pucPortType  ֧�ֵĶ˿���̬�б�
           pulPortNum   ֧�ֵĶ˿���̬����
����ֵ��   0:    ��ȡ�˿���̬�б�ɹ�
           ��������ȡ�˿���̬�б�ʧ��
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)

/*****************************************************************************
 �� �� ��  : BSP_USB_ATProcessRewind2Cmd
 ��������  : rewind2 CMD ����
 �������  : pData�����ݡ�
 �������  : ��
 �� �� ֵ  : false(0):����ʧ��
             tool(1):����ɹ�
*****************************************************************************/
extern int BSP_USB_ATProcessRewind2Cmd(unsigned char *pData);
#define DRV_PROCESS_REWIND2_CMD(pData)    BSP_USB_ATProcessRewind2Cmd(pData)

/*****************************************************************************
 �� �� ��  : BSP_USB_GetDiagModeValue
 ��������  : ����豸���͡�
 �������  : �ޡ�
 �������  : ucDialmode:  0 - ʹ��Modem����; 1 - ʹ��NDIS����; 2 - Modem��NDIS����
              ucCdcSpec:   0 - Modem/NDIS������CDC�淶; 1 - Modem����CDC�淶;
                           2 - NDIS����CDC�淶;         3 - Modem/NDIS������CDC�淶
 �� �� ֵ  : VOS_OK/VOS_ERR
*****************************************************************************/
extern BSP_S32 BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec);
#define DRV_GET_DIAG_MODE_VALUE(pucDialmode, pucCdcSpec)    \
                             BSP_USB_GetDiagModeValue(pucDialmode, pucCdcSpec)

/*****************************************************************************
 �� �� ��  : BSP_USB_GetPortMode
 ��������  : ��ȡ�˿���̬ģʽ�����ضԽ����󣬴�׮��
 �������  : ��
 �������  :
 �� �� ֵ  :
*****************************************************************************/
extern unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned long*Length );
#define DRV_GET_PORT_MODE(PsBuffer, Length)    BSP_USB_GetPortMode(PsBuffer,Length)

/*****************************************************************************
 �� �� ��  : BSP_USB_GetU2diagDefaultValue
 ��������  : ��ö˿�Ĭ��u2diagֵ
 �������  : �ޡ�
 �������  : �ޡ�
 ����ֵ��   u2diagֵ

*****************************************************************************/
extern BSP_U32 BSP_USB_GetU2diagDefaultValue(void);
#define DRV_GET_U2DIAG_DEFVALUE()    BSP_USB_GetU2diagDefaultValue()

/*****************************************************************************
 �� �� ��  : ErrlogRegFunc
 ��������  : USB MNTNע���쳣��־�ӿڣ�Porting��Ŀ�д�׮
 �������  : �ޡ�
 �������  : �ޡ�
 ����ֵ��   ��

*****************************************************************************/
//extern void ErrlogRegFunc(MNTN_ERRLOGREGFUN pRegFunc);
extern void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc);

/*****************************************************************************
 �� �� ��  : getSystemResetInfo
 ��������  : ��ȡϵͳ��λ��Ϣ
 �������  : ��

 �������  : �ޡ�
 �� �� ֵ  : ��
 ע������  ��
*****************************************************************************/
extern void * GET_SYSTEM_RESET_INFO(void);

/*****************************************************************************
* �� �� ��  : DRV_DSP_AHB_RESET
* ��������  : ZSP AHB���߸�λ
* �������  : ��
* �������  : ��
* �� �� ֵ  : ��
* ����˵��  : ��
*****************************************************************************/
extern void DRV_DSP_AHB_RESET(void);

/*****************************************************************************
* �� �� ��  : DRV_DSP_AHB_RESET_CANCEL
* ��������  : ZSP AHB���߽⸴λ
* �������  : ��
* �������  : ��
* �� �� ֵ  : ��
* ����˵��  : ��
*****************************************************************************/
extern void DRV_DSP_AHB_RESET_CANCEL(void);


/*****************************************************************************
 �� �� ��  : BSP_USB_UdiagValueCheck
 ��������  : ���ӿ����ڼ��NV����USB��ֵ̬�ĺϷ���.
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0��OK��
            -1��ERROR
*****************************************************************************/
extern int BSP_USB_UdiagValueCheck(unsigned long DiagValue);
#define DRV_UDIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)
#define DRV_U2DIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)

/*****************************************************************************
 �� �� ��  : BSP_USB_GetLinuxSysType
 ��������  : ���ӿ����ڼ��PC���Ƿ�ΪLinux���Թ��Linux��̨���β���ʧ�ܵ�����.
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0��Linux��
            -1����Linux��
*****************************************************************************/
extern int BSP_USB_GetLinuxSysType(void) ;
#define DRV_GET_LINUXSYSTYPE()    BSP_USB_GetLinuxSysType()

/********************************************************
����˵���� ���ص�ǰ�豸�б���֧��(sel=1)���߲�֧��(sel=0)PCSC���豸��ֵ̬
��������:
���������sel
          0: ͨ������dev_type���ص�ǰ����PCSC���豸��ֵ̬
          1��ͨ������dev_type���ص�ǰ��PCSC���豸��ֵ̬
���������dev_type д����Ҫ���豸��ֵ̬�����û����д��ֵ��
          NV�д洢���豸��ֵ̬
���������pulDevType ��ulCurDevType��Ӧ���豸��ֵ̬�����û�з���ֵ1��
����ֵ��
          0����ѯ����Ӧ���豸��ֵ̬��
          1��û�в�ѯ����Ӧ���豸��ֵ̬��
********************************************************/
int BSP_USB_PcscInfoSet(unsigned int  ulSel, unsigned int  ulCurDevType, unsigned int *pulDevType);
#define DRV_USB_PCSC_INFO_SET(ulSel, ulCurDevType, pulDevType)    BSP_USB_PcscInfoSet(ulSel, ulCurDevType, pulDevType)

/*TCP/IPЭ��ջ��ά�ɲⲶ�����Ϣ��ʶ*/
enum IPS_MNTN_TRACE_IP_MSG_TYPE_ENUM
{
    /* IP ���ݰ���ά�ɲ��ϱ� */
    ID_IPS_TRACE_IP_ADS_UL                  = 0xD030,
    ID_IPS_TRACE_IP_ADS_DL                  = 0xD031,
    ID_IPS_TRACE_IP_USB_UL                  = 0xD032,
    ID_IPS_TRACE_IP_USB_DL                  = 0xD033,

    ID_IPS_TRACE_IP_MSG_TYPE_BUTT
};

/********************************************************
����˵����Э��ջע��USB��SKB�ؼ���Ϣ�ϱ���SDT�ص�����
��������:
���������pFunc: SKB�ؼ���Ϣ�ϱ���SDT�ص�����ָ��
�����������
�����������
����ֵ��  0���ɹ�
          1��ʧ��
********************************************************/
typedef void (*USB_IPS_MNTN_TRACE_CB_T)(struct sk_buff *skb,unsigned short usType);
extern unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc);
#define DRV_USB_REG_IPS_TRACECB(x) BSP_USB_RegIpsTraceCB(x)

/********************************************************
����˵����Э��ջע��USBʹ��֪ͨ�ص�����
��������:
���������pFunc: USBʹ�ܻص�����ָ��
�����������
�����������
����ֵ��  0���ɹ�
          1��ʧ��
********************************************************/
typedef void (*USB_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc);
#define DRV_USB_REGUDI_ENABLECB(x) BSP_USB_RegUdiEnableCB(x)

/********************************************************
����˵����Э��ջע��USBȥʹ��֪ͨ�ص�����
��������:
���������pFunc: USBʹ�ܻص�����ָ��
�����������
�����������
����ֵ��  0���ɹ�
          1��ʧ��
********************************************************/
typedef void (*USB_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc);
#define DRV_USB_REGUDI_DISABLECB(x) BSP_USB_RegUdiDisableCB(x)

/********************************************************
����˵����Э��ջע��HSICʹ��֪ͨ�ص�����
��������:
���������pFunc: HSICʹ�ܻص�����ָ��
�����������
�����������
����ֵ��  0���ɹ�
          1��ʧ��
********************************************************/
typedef void (*HSIC_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_ENABLECB(x) BSP_HSIC_RegUdiEnableCB(x)

/********************************************************
����˵����Э��ջע��HSICȥʹ��֪ͨ�ص�����
��������:
���������pFunc: HSICȥʹ�ܻص�����ָ��
�����������
�����������
����ֵ��  0���ɹ�
          1��ʧ��
********************************************************/
typedef void (*HSIC_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_DISABLECB(x) BSP_HSIC_RegUdiDisableCB(x)
/********************************************************
����˵����Э��ջ��ѯHSICö��״̬
��������:
�����������
�����������
�����������
����ֵ��  1��ö�����
          0��ö��δ���
********************************************************/
extern unsigned int BSP_GetHsicEnumStatus(void);
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()

/********************************************************
����˵����Э��ջ��ѯHSIC�Ƿ�֧��NCM
��������:
�����������
�����������
�����������
����ֵ��  1��֧��NCM
          0����֧��NCM
********************************************************/
extern int BSP_USB_HSIC_SupportNcm(void);
#define DRV_USB_HSIC_SUPPORT_NCM() BSP_USB_HSIC_SupportNcm()

/********************************************************
����˵����TTF��ѯԤ����SKB Num
��������:
�����������
�����������
�����������
����ֵ��  SKB Num
********************************************************/
extern BSP_U32 BSP_AcmPremallocSkbNum(void);
#define DRV_GET_PREMALLOC_SKB_NUM() BSP_AcmPremallocSkbNum()

/********************************************************
��������   BSP_UDI_FindVcom
���������� ��ѯ��ǰ�豸��̬��ָ����UDI���⴮�ڶ˿��Ƿ����
��������� UDI_DEVICE_IDö��ֵ��������ѯ�����⴮�ڶ˿�ID
��������� ��
����ֵ��
           0����ǰ�豸��̬��֧�ֲ�ѯ�����⴮�ڶ˿ڣ�
           1����ǰ�豸��̬֧�ֲ�ѯ�����⴮�ڶ˿ڡ�
ע����� ��
********************************************************/
extern int BSP_UDI_FindVcom(UDI_DEVICE_ID enVCOM);

/*****************************************************************************
* �� �� ��  : DRV_USB_RegEnumDoneForMsp
* ��������  : �ṩ�� MSP ע�� USB ö����ɺ�֪ͨ����
* �������  : pFunc: ö����ɻص�����ָ��
* �������  : ��
* �� �� ֵ  : 0: �ɹ�ע��,�ȴ�ö�����֪ͨ;
*             1: USB �Ѿ�ö�����, ����ֱ�ӳ�ʼ�� USB ����;
*             -1: ʧ��, ��USB��̬,û��USB����
*****************************************************************************/
extern signed int BSP_USB_RegEnumDoneForMsp(void *pFunc);
#define DRV_USB_RegEnumDoneForMsp   BSP_USB_RegEnumDoneForMsp

/*****************************************************************************
* �� �� ��  : BSP_USB_RegEnumDoneForPs
* ��������  : �ṩ�� PS ע�� USB ö����ɺ�֪ͨ����
* �������  : pFunc: ö����ɻص�����ָ��
* �������  : ��
* �� �� ֵ  : 0: �ɹ�ע��,�ȴ�ö�����֪ͨ;
*             1: USB �Ѿ�ö�����, ����ֱ�ӳ�ʼ�� USB ����;
*             -1: ʧ��, ��USB��̬,û��USB����
*****************************************************************************/
extern signed int BSP_USB_RegEnumDoneForPs(void *pFunc);
#define DRV_USB_RegEnumDoneForPs    BSP_USB_RegEnumDoneForPs

/********************************************************
����˵���� ���ص�ǰNV���е��豸��ֵ̬���Ƿ�֧��PCSC
��������:
��������� dev_type���豸��ֵ̬��OAM��NV�ж�����

��������� �ޡ�
����ֵ��
          0: dev_type��֧��PCSC�豸��
          1��dev_type֧��PCSC�豸
********************************************************/
/*unsigned int pcsc_info_quiry(unsigned int ulDevType);*/
#define DRV_USB_PCSC_INFO_QUIRY(a)            DRV_OK

typedef unsigned long (*pFunAPDUProcess)(unsigned long CmdType, unsigned char *pApdu, unsigned long ApduLen);
typedef unsigned long (*GetCardStatus)(void);

typedef struct
{
    pFunAPDUProcess   pFuncApdu;
    GetCardStatus  pFuncGetCardStatus;
}BSP_CCID_REGFUNC;

/*****************************************************************************
 �� �� ��  : pcsc_usim_int
 ��������  : ע��PCSC������պ���
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern void pcsc_usim_int(pFunAPDUProcess pFun1, GetCardStatus pFun2);
#define DRV_PCSC_REG_CALLBACK(pFun1, pFun2) pcsc_usim_int((pFun1), (pFun2))


/*****************************************************************************
 �� �� ��  : pcsc_usim_ctrl_cmd
 ��������  : PCSC����ظ�����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern unsigned long pcsc_usim_ctrl_cmd(unsigned long CmdTpye, unsigned long Result,
                                        unsigned char *Buf, unsigned long Length);
#define DRV_PCSC_SEND_DATA(CmdType, Result, Buffer, Length) pcsc_usim_ctrl_cmd(CmdType, Result, Buffer, Length)

/* Porting���࣬��ʱ����Ϊ�� */
#define DRV_COM_RCV_CALLBACK_REGI(uPortNo,pCallback)    DRV_OK


/*****************************************************************************
 �� �� ��  : pcsc_usim_ctrl_cmd
 ��������  : PCSC����ظ�����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/


/*���翨��δ���� ����ʱAPI ��׮begin*/
/*****************************************************************************
 �� �� ��  : BSP_USB_RndisAppEventDispatch
 ��������  : ���ӿ�����֪ͨAPP ��Ӧ��USB����¼�
 �������  : usb�¼���
 �� �� ֵ  :��
*****************************************************************************/
extern void BSP_USB_RndisAppEventDispatch(unsigned ulStatus) ;
#define DRV_RNDIS_APP_ENENT_DISPATCH(ulStatus)    BSP_USB_RndisAppEventDispatch(ulStatus)

/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       ���翨�汾����Ӧ�ý��в��Ż�Ͽ���������
 * INPUTS
 *       ���в��Ż�Ͽ�����ָʾ
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern VOID rndis_app_event_dispatch(unsigned int ulStatus);
extern VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus);


/************************************************************************
 * FUNCTION
 *       PDP_ACT_DRV_CALLBACK
 * DESCRIPTION
 *       PDP�������õ���Ļص�������ԭ�������翨���������ط��ű���Ϣ�����ڴ�׮
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int PDP_ACT_DRV_CALLBACK(void);

typedef void(* USB_NET_DEV_SWITCH_GATEWAY)
(
    void
);

/*****************************************************************************
 �� �� ��  : BSP_USB_NASSwitchGatewayRegExtFunc
 ��������  : ���ӿ�����NASע���л�����֪ͨ�ص�����
 �������  :�ص��ӿڡ�
 �������  : �ޡ�
 �� �� ֵ  : 0���ɹ�
                       ����:ʧ��
*****************************************************************************/
extern int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode) ;
#define DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC(switchGwMode)    BSP_USB_NASSwitchGatewayRegFunc(switchGwMode)
/*���翨��δ���� ����ʱAPI ��׮end*/

/*************************USB END*************************************/


/*************************OM START**********************************/

#define PS_L2_UL_TRACE           (0)
#define PS_L2_DL_TRACE           (1)

#define PROCESS_ENTER(pADDRESS)  do { } while (0)
#define PROCESS_EXIT(pADDRESS)   do { } while (0)

typedef enum{
     ADDRTYPE8BIT,
     ADDRTYPE16BIT,
     ADDRTYPE32BIT
}ENADDRTYPE;

typedef enum{
     BOARD_TYPE_LTE_ONLY    = 0,
     BOARD_TYPE_GUL
}BOARD_TYPE_E;

typedef enum{
     BSP_MODEM_CORE         = 0,
     BSP_APP_CORE
}BSP_CORE_TYPE_E;

typedef enum{
     PV500_CHIP             = 0,
     V7R1_CHIP              = 1,
     PV500_PILOT_CHIP,
     V7R1_PILOT_CHIP
}BSP_CHIP_TYPE_E;

/******************************************************************************
* Function     :   BSP_OM_RegRead
*
* Description  :
*
* Input        :
*
* Output       :   ��
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegRead(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);


/******************************************************************************
* Function     :   BSP_OM_RegWrite
*
* Description  :
*
* Input        :
*
* Output       :   ��
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegWrite(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);

/******************************************************************************
* Function     :   BSP_OM_SoftReboot
*
* Description  :
*
* Input        :
*
* Output       :   ��
*
* return       :
******************************************************************************/
BSP_VOID BSP_OM_SoftReboot(void);

/*****************************************************************************
* �� �� ��  : BSP_OM_GetBoardType
*
* ��������  : ��ȡ��������
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : BSP��������ö��
*
* ����˵��  : ��
*
*****************************************************************************/
BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_GetHostCore
*
* ��������  : ��ѯ��ǰCPU���Ӻ�����
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : CPU���Ӻ�����
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_CORE_TYPE_E BSP_GetHostCore(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_OM_GetChipType
*
* ��������  : ��ȡоƬ����
*
* �������  : BSP_VOID
*
* �������  : ��
*
* �� �� ֵ  : оƬ����
*             PV500_CHIP:PV500оƬ
*             V7R1_CHIP: V7R1оƬ
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_CHIP_TYPE_E BSP_OM_GetChipType(BSP_VOID);

typedef struct tagBSP_OM_NET_S
{
    BSP_U32 u32NetRxStatOverFlow;       /* ����FIFO���ͳ�Ƽ��� */
    BSP_U32 u32NetRxStatPktErr;         /* �����ܴ������ */
    BSP_U32 u32NetRxStatCrcErr;         /* ����CRC������� */
    BSP_U32 u32NetRxStatLenErr;         /* ������Ч���Ȱ����� */
    BSP_U32 u32NetRxNoBufInt;           /* ����û��BUFFER�жϼ��� */
    BSP_U32 u32NetRxStopInt;            /* ����ֹͣ�жϼ��� */
    BSP_U32 u32NetRxDescErr;            /* �������������� */

    BSP_U32 u32NetTxStatUnderFlow;      /* ����FIFO����ͳ�Ƽ��� */
    BSP_U32 u32NetTxUnderFlowInt;       /* ����FIFO�����жϼ��� */
    BSP_U32 u32NetTxStopInt;            /* ����ֹͣ�жϼ��� */
    BSP_U32 u32NetTxDescErrPs;          /* ��������������(Ps) */
    BSP_U32 u32NetTxDescErrOs;          /* ��������������(Os) */
    BSP_U32 u32NetTxDescErrMsp;         /* ��������������(Msp) */

    BSP_U32 u32NetFatalBusErrInt;      /* ���ߴ���*/
}BSP_OM_NET_S;

/******************************************************************************
* Function     :   BSP_OM_NET
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm);

/*************************OM END  **********************************/

/*************************MNTN START**********************************/
/*VERSIONINFO_I���ݽṹ�а汾�ַ��������Ч�ַ�����*/
#define VER_MAX_LENGTH                  30

/*memVersionCtrl�ӿڲ�������*/
#define VERIONREADMODE                  0
#define VERIONWRITEMODE                 1

#define OM_SAVE_EXCHFILE_ING            0
#define OM_SAVE_EXCHFILE_END            1


/*�������*/
typedef enum
{
    VER_BOOTLOAD = 0,
    VER_BOOTROM =1,
    VER_NV =2 ,
    VER_VXWORKS =3,
    VER_DSP =4 ,
    VER_PRODUCT_ID =5 ,
    VER_WBBP =6 ,
    VER_PS =7,
    VER_OAM =8,
    VER_GBBP =9 ,
    VER_SOC =10,
    VER_HARDWARE =11,
    VER_SOFTWARE =12,
    VER_MEDIA =13,
    VER_APP =14,
    VER_ASIC =15,
    VER_RF =16,
    VER_PMU =17,
    VER_PDM = 18,
    VER_PRODUCT_INNER_ID = 19,
    VER_INFO_NUM =20
}COMP_TYPE_I;

/* ��ɫ��״̬���*/
enum
{
    LED_LIGHT_SYSTEM_STARTUP,
    LED_LIGHT_POWER_ON,
    LED_LIGHT_G_REGISTERED,
    LED_LIGHT_W_REGISTERED,
    LED_LIGHT_G_CONNECTED,
    LED_LIGHT_W_CONNNECTED,
    LED_LIGHT_H_CONNNECTED,
    LED_LIGHT_OFFLINE,
    LED_LIGHT_FORCE_UPDATING,
    LED_LIGHT_NORMAL_UPDATING,
    LED_LIGHT_UPDATE_FAIL,
    LED_LIGHT_UPDATE_SUCCESS,
    LED_LIGHT_UPDATE_FILEFAIL,
    LED_LIGHT_UPDATE_NVFAIL,
    LED_LIGHT_SIM_ABSENT,
    LED_LIGHT_SHUTDOWN,
    LED_LIGHT_G_PLMN_SEARCH,
    LED_LIGHT_W_PLMN_SEARCH,
    LED_LIGHT_L_REGISTERED,
    LED_LIGHT_STATE_MAX = 32
};/*the state of LED */

enum SECURE_SUPPORT_STATUS_I
{
    SECURE_NOT_SUPPORT = 0,
    SECURE_SUPPORT = 1
};

enum SECURE_ENABLE_STATUS_I
{
    SECURE_DISABLE = 0,
    SECURE_ENABLE = 1
};

typedef enum                /* CACHE_TYPE */
{
    PS_OSAL_INSTRUCTION_CACHE ,
    PS_OSAL_DATA_CACHE
} PS_OSAL_CACHE_TYPE_I;

typedef int  (*OM_SAVE_FUNC)(int funcType, unsigned char *data, unsigned int* ulLength);

typedef struct
{
    unsigned char CompId;              /* ����ţ��μ�COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* ���汾���� 30 �ַ�+ \0 */
}VERSIONINFO_I;

typedef struct
{
    unsigned char enHSDSCHSupport;          /*Ӳ���Ƿ�֧��HSDPA*/
    unsigned char ucHSDSCHPhyCategory;      /*Ӳ��֧�ֵ����HSDPA�����ȼ�*/
    unsigned char enEDCHSupport;            /*Ӳ���Ƿ�֧��HSUPA*/
    unsigned char ucEDCHPhyCategory;        /*Ӳ��֧�ֵ����HSUPA�����ȼ�*/
    unsigned int  reserved;                 /*Ԥ��*/
}SUPPORT_MAX_HSPA_CAPA_STRU;

/*****************************BSP_MspProcReg  begin******************************/
typedef enum tagMSP_PROC_ID_E
{
    OM_REQUEST_PROC = 0,
    OM_ADDSNTIME_PROC = 1,
    OM_PRINTF_WITH_MODULE = 2,
    OM_PRINTF = 3,

    OM_PRINTF_GET_MODULE_IDLEV = 4,
    OM_READ_NV_PROC = 5,
    OM_WRITE_NV_PROC = 6,
    OM_MNTN_ERRLOG = 7, 

    MSP_PROC_REG_ID_MAX
}MSP_PROC_ID_E;

/*RF��������*/
typedef enum tagRF_VOLTAGE
{
    RF_VOLTAGE_18V,                                                          
    RF_VOLTAGE_22V,
    RF_VOLTAGE_MAX
}RF_VOLTAGE_E;


typedef void (*BSP_MspProc)(void);
/*****************************************************************************
* �� �� ��  : DRV_MSP_PROC_REG
*
* ��������  : DRV�ṩ��OM��ע�ắ��
*
* �������  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* �������  : NA
*
* �� �� ֵ  : NA
*
* ����˵��  : ��ά�ɲ�ӿں���
*
*****************************************************************************/
extern void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);

/********************************************************************************************************
 �� �� ��  : BSP_FS_GetDskspc
 ��������  : �õ��ļ�ϵͳ�ռ䡣
 �������  : path���ļ�ϵͳ·����
 �������  : DskSpc���ܿռ䡣
                           UsdSpc���ù��ռ䡣
                           VldSpc�����пռ䡣
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_FS_GetDskspc(const char *path,unsigned int *DskSpc,unsigned int  *UsdSpc,  unsigned int *VldSpc);
#define DRV_FILE_GET_DISKSPACE(path,DskSpc,UsdSpc,VldSpc)    BSP_FS_GetDskspc(path,DskSpc,UsdSpc,VldSpc)


/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetFlashSpec
 ��������  : Get flash infomation
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern int BSP_MNTN_GetFlashSpec(unsigned char * pFlashInfo, unsigned int ulLength);
#define DRV_GET_FLASH_INFO(pFlashInfo, usLength)    BSP_MNTN_GetFlashSpec (pFlashInfo, usLength )


/************************************************************************
 * FUNCTION
 *       max_freeblock_size_get
 * DESCRIPTION
 *       get memory max free block size
 * INPUTS
 *       ��
 * OUTPUTS
 *       max free block size
 *************************************************************************/
extern int DRV_GET_FREE_BLOCK_SIZE(void);

/*****************************************************************************
 �� �� ��  : BSP_MNTN_UartRecvCallbackRegister
 ��������  : �ṩ�ϲ�Ӧ�ó���ע�����ݽ��ջص�����ָ���API�ӿں�����
 �������  : uPortNo������ʵ���š�
             pCallback���ص�����ָ�롣
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int  BSP_MNTN_UartRecvCallbackRegister(unsigned char  uPortNo, pUartRecv pCallback);
#define DRV_UART_RCV_CALLBACK_REGI(uPortNo,pCallback)\
                BSP_MNTN_UartRecvCallbackRegister(uPortNo,pCallback)

/*************************************************
 �� �� ��   : BSP_MNTN_ProductTypeGet
 ��������   : ���ص�ǰ��Ʒ����
 �������   : ��
 �������   : ��
 �� �� ֵ   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
extern BSP_U32 BSP_MNTN_ProductTypeGet(void);
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()


/*************************************************
 �� �� ��   : DRV_HKADC_BAT_VOLT_GET
 ��������   : ���ص�ǰ��ص�ѹֵ
 �������   : pslData : ��ص�ѹֵ
 �������   : pslData : ��ص�ѹֵ
 �� �� ֵ   :0:��ȡ�ɹ�
            -1:��ȡʧ��
*************************************************/
extern BSP_S32 DRV_HKADC_BAT_VOLT_GET(BSP_S32 *ps32Data);


/*****************************************************************************
* �� �� ��  : BSP_HKADC_PaValueGet
*
* ��������  :
*
* �������  :
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_HKADC_PaValueGet( BSP_U16 *pusValue );


#define BUILD_DATE_LEN  12
#define BUILD_TIME_LEN  12

typedef struct  tagUE_SW_BUILD_VER_INFO_STRU
{
    BSP_U16 ulVVerNo;        // V����
    BSP_U16 ulRVerNo;        // R����
    BSP_U16 ulCVerNo;        // C����
    BSP_U16 ulBVerNo;        // B����
    BSP_U16 ulSpcNo;         // SPC����
    BSP_U16 ulCustomVer;     // �ͻ��趨�Ʋ���, ���Ӳ���ӿڵ�8bit PCB��,��8bitHW��
    BSP_U32 ulProductNo;     // such as porting,CPE, ...
    BSP_S8 acBuildDate[BUILD_DATE_LEN];  // build����,
    BSP_S8 acBuildTime[BUILD_TIME_LEN];  // buildʱ��
} UE_SW_BUILD_VER_INFO_STRU; // �ڲ��汾


typedef struct {
    BSP_U32   bandGU;         	/*֧�ֵ�GUƵ��*/
    BSP_U32   bamdLTELow;         /*֧��LTEƵ�Σ�B1~B32*/
    BSP_U32   bamdLTEHigh;        /*֧��LTEƵ�Σ�B33~B64*/
}BAND_INFO_T;

typedef struct {
    BSP_U16  index;         /*Ӳ���汾����ֵ(��汾��1+��汾��2)�����ֲ�ͬ��Ʒ*/
    BSP_U16   hwIdSub;         /*Ӳ���Ӱ汾�ţ����ֲ�Ʒ�Ĳ�ͬ�İ汾*/
    BSP_CHAR* name;           /*�ڲ���Ʒ��*/
    BSP_CHAR* namePlus;       /*�ڲ���Ʒ��PLUS*/
    BSP_CHAR* hwVer;          /*Ӳ���汾����*/
    BSP_CHAR* dloadId;        /*������ʹ�õ�����*/
    BSP_CHAR* productId;      /*�ⲿ��Ʒ��*/
    BAND_INFO_T  band;            /*��Ʒ֧�ֵ�Ƶ��*/
    BAND_INFO_T  bandDiv;         /*��Ʒ֧�ֵķּ�*/
    BSP_CHAR reserve[4];      /*Ԥ��*/
}PRODUCT_INFO_T;

/*****************************************************************************
* �� �� ��  : BSP_HKADC_PaValueGet
*
* ��������  :
*
* �������  :
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBuildVersion(BSP_VOID);


/*****************************************************************************
* �� �� ��  : BSP_HwGetHwVersion
*
* ��������  : ��ȡӲ���汾����
*
* �������  : BSP_CHAR* pHwVersion,�ַ���ָ�룬��֤��С��32�ֽ�
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_HwGetHwVersion (char* pFullHwVersion, BSP_U32 ulLength);


/*****************************************************************************
* �� �� ��  : BSP_GetSupportBands
*
* ��������  : ��ȡ֧�ֵ�Ƶ��
*
* �������  : BAND_INFO_T *pBandInfo,Ƶ����Ϣ�ṹ��ָ��
* �������  :
*
 �� �� ֵ  : 0����ȷ����0: ʧ��
*
* ����˵��  :
*
*****************************************************************************/
BSP_S32 BSP_GetSupportBands( BAND_INFO_T *pBandInfo);

/*****************************************************************************
* �� �� ��  : BSP_HwIsSupportWifi
*
* ��������  : ��ӡӲ���汾��Ϣ
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : ��
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_BOOL BSP_HwIsSupportWifi(BSP_VOID);

/*****************************************************************************
 �� �� ��  : hkadcBatADCRead
 ��������  : ��ȡ��ص�ѹ����ֵ
 �������  : ��
 �������  : pTemp��        ָ�����¶ȵ�ָ�롣
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 DRV_GET_BATTERY_ADC(BSP_S32 * pslData);


/*****************************************************************************
 �� �� ��  : BSP_PMU_LDOOFF
 ��������  : ���ӿ����ڹر�ĳ·LDO�ĵ�ѹ�����
 �������  : ucLDO��ָʾĳ·LDO��ѹ��ȡֵ��ΧΪ1��12��
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
 ע�������LDO6��Ӳ�����ƣ������֧�ֶ�LDO6�Ĳ����������LDO1�رգ���ϵͳ��ػ���
*****************************************************************************/
extern void BSP_PMU_LDOOFF(BSP_U8 u8LDO);
#define DRV_PM_LDO_OFF(u8LDO)                 BSP_PMU_LDOOFF(u8LDO)

/*****************************************************************************
 �� �� ��  : BSP_PMU_LDOON
 ��������  : ���ӿ��������ô�ĳ·LDO�ĵ�ѹ�����
 �������  : ucLDO��ָʾĳ·LDO��ѹ��ȡֵ��ΧΪ2��12��
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
 ע���������֧�ֶ�LDO1��LDO6�Ĳ�����LDO1����ϵͳ����ʱ�Զ��򿪣�һ���ر�������ϵͳ���µ磻LDO6����Ӳ���ܽſ��ơ�
*****************************************************************************/
extern void BSP_PMU_LDOON(BSP_U8 u8LDO);
#define DRV_PM_LDO_ON(u8LDO)                  BSP_PMU_LDOON(u8LDO)

/************************************
�� �� ��  : BSP_PMU_AptEnable
��������  : ʹ��PMU��APT����
�������  : ��
�������  : ��������״̬
�� �� ֵ  : BSP_OK�����óɹ�
		����ֵ������ʧ��
**************************************/
extern BSP_S32 BSP_PMU_AptEnable(BSP_VOID);
#define DRV_PMU_APT_ENABLE()    BSP_PMU_AptEnable()

/************************************
�� �� ��  : BSP_PMU_AptDisable
��������  : ȥʹ��PMU��APT����
�������  : ��
�������  : ��������״̬
�� �� ֵ  : BSP_OK�����óɹ�
		����ֵ������ʧ��
**************************************/
extern BSP_S32 BSP_PMU_AptDisable(BSP_VOID);
#define DRV_PMU_APT_DISABLE()    BSP_PMU_AptDisable()

/************************************
�� �� ��  : BSP_PMU_AptIsEnable
��������  : ��ѯPMU��APT�����Ƿ�ʹ��
�������  : ��
�������  : ��������״̬
�� �� ֵ  : BSP_TURE��ʹ��APT
		    BSP_FALSE,δʹ��APT
**************************************/
extern BSP_BOOL  BSP_PMU_AptIsEnable(BSP_VOID);
#define DRV_PMU_APT_ISENABLE()    BSP_PMU_AptIsEnable()

extern BSP_U32 DRV_ADJUST_RF_VOLTAGE(RF_VOLTAGE_E enRfVol, BSP_U32 u32Voltage);

/*****************************************************************************
 �� �� ��  : BSP_ONOFF_DrvPowerOff
 ��������  : ����ֱ���µ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
extern void DRV_POWER_OFF(void);


/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetGsmPATemperature
 ��������  : ��ȡGSM PA�¶�
 �������  : Temprature
             hkAdcTalble
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_GetGsmPATemperature(int *temperature, unsigned short *hkAdcTable);
#define DRV_GET_PA_GTEMP(temperature, hkAdcTable)   \
                BSP_MNTN_GetGsmPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 �� �� ��  : getWcdmaPATemperature
 ��������  : ��ȡWCDMA PA�¶�
 �������  : Temprature
             hkAdcTalble
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_GetWcdmaPATemperature (int *temperature, unsigned short *hkAdcTable);
#define  DRV_GET_PA_WTEMP(temperature, hkAdcTable)  \
              BSP_MNTN_GetWcdmaPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetSIMTemperature
 ��������  : ��ȡSIM���¶�
 �������  : pusHkAdcTable����ŵ�ѹ��
 �������  : pTemp��        ָ��SIM���¶ȵ�ָ�롣
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_GetSIMTemperature(int *plTemp, unsigned short *pusHkAdcTable);
#define DRV_GET_SIM_TEMP(plTemp, pusHkAdcTable)   \
               BSP_MNTN_GetSIMTemperature(plTemp, pusHkAdcTable)

/*****************************************************************************
 �� �� ��  : void BSP_PWRCTRL_DfsCpuUp(void)
 ��������  : Set CPU freq to 450M
 �������  : 
 �������  : 
 �� �� ֵ  : 
*****************************************************************************/
extern void BSP_PWRCTRL_DfsCpuUp(void);
#define BSP_PWRCTRL_DFS_CPUUP() BSP_PWRCTRL_DfsCpuUp()

/*****************************************************************************
 �� �� ��  : void BSP_PWRCTRL_DfsThrProOpen(void)/
 			 void BSP_PWRCTRL_DfsThrProClose(void)
 ��������  : open CPU freq to THREE PROFILES MODE
 �������  : 
 �������  : 
 �� �� ֵ  : 
*****************************************************************************/

extern void BSP_PWRCTRL_DfsThrProOpen(void);
#define BSP_PWRCTRL_DFS_THRPRO_OPEN() BSP_PWRCTRL_DfsThrProOpen()
/*****************************************************************************
 �� �� ��  : void BSP_PWRCTRL_DfsThrProClose(void)
 ��������  : CLOSE CPU freq to THREE PROFILES MODE
 �������  : 
 �������  : 
 �� �� ֵ  : 
*****************************************************************************/
extern void BSP_PWRCTRL_DfsThrProClose(void);
#define BSP_PWRCTRL_DFS_THRPRO_CLOSE() BSP_PWRCTRL_DfsThrProClose()

/*****************************************************************************
Function:   BSP_PWRCTRL_GetCpuFreq(void)
Description:Get CPU frequency
Input:      None
Output:     None
Return:     None
Others:
*****************************************************************************/
extern int BSP_PWRCTRL_GetCpuFreq();
#define BSP_PWRCTRL_GET_CPUFREQ() BSP_PWRCTRL_GetCpuFreq()


/*****************************************************************************
 �� �� ��  : BSP_DFS_GetCurCpuLoad
 ��������  : ��ѯ��ǰCPU
 �������  : pu32AcpuLoad ACPUloadָ��
             pu32CcpuLoad CCPUloadָ��
 �������  : pu32AcpuLoad ACPUloadָ��
             pu32CcpuLoad CCPUloadָ��
 �� �� ֵ  : 0:  �����ɹ���
            -1������ʧ�ܡ�
*****************************************************************************/
extern BSP_U32 BSP_DFS_GetCurCpuLoad(BSP_U32 *pu32AcpuLoad,BSP_U32 *pu32CcpuLoad);
#define DRV_GET_CUR_CPU_LOAD(pu32AcpuLoad,pu32CcpuLoad) BSP_DFS_GetCurCpuLoad(pu32AcpuLoad,pu32CcpuLoad)


/*****************************************************************************
 �� �� ��  : BSP_GU_GetVerTime
 ��������  : ��ȡ�汾����ʱ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
extern char *BSP_GU_GetVerTime(void);
#define DRV_GET_VERSION_TIME()    BSP_GU_GetVerTime()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_MemVersionCtrl
 ��������  : ����汾��д�ӿڡ�
 �������  : pcData����ModeΪ����ʱ��Ϊ�����ߴ����淵�ص�����汾��Ϣ���ڴ��ַ��
                     ��ModeΪд��ʱ��Ϊ������׼��д�������汾��Ϣ�ĵ�ַ��
             ucLength����ModeΪ����ʱ��Ϊ�����ߴ����淵�ص�����汾��Ϣ���ڴ��С��
                       ��ModeΪд��ʱ��Ϊ������׼��д�������汾��Ϣ���ַ�����������'\0'����
             ucType���汾��ϢID��
                     0�� BOOTLOAD��
                     1�� BOOTROM��
                     2�� NV��
                     3�� VXWORKS��
                     4�� DSP��
                     5�� CDROMISO��
                     6�� PHY��
                     7�� PS��
                     8�� COM��
                     9�� RF��
                     10��SOC��
                     11��HARDWARE��
                     12��SOFTWARE��
                     13��MEDIA��
                     14��APP��
                     15��INFO_NUM��
             ucMode��0����ȡָ��ID������汾��Ϣ��1��д��ָ��ID������汾��Ϣ��
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_MemVersionCtrl(signed char *pcData, unsigned char ucLength, unsigned char ucType, unsigned char ucMode);
#define DRV_MEM_VERCTRL(pcData,ucLength,ucType,ucMode)  \
                          BSP_MNTN_MemVersionCtrl(pcData,ucLength,ucType,ucMode)

/********************************************************************************************************
 �� �� ��  : getHwVersionIndex
 ��������  : ��ȡ��HKADC��ȡ��Ӳ��ID����ƴװ���Ӳ������ֵ
 �������  : ��
 �������  : Ӳ��ID������ֵ

 �� �� ֵ  : ��-1:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int getHwVersionIndex(void);
#define DRV_GET_HW_VERSION_INDEX()       getHwVersionIndex()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetFullHwVersion
 ��������  : Ӳ�������汾���ӿڡ�
 �������  : pFullHwVersion��Ϊ�����ߴ����淵�ص�Ӳ�������汾��Ϣ���ڴ��׵�ַ��
             ulLength      ��Ϊ�����ߴ����淵�ص�Ӳ�������汾��Ϣ���ڴ��С��
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_GetFullHwVersion(char * pFullHwVersion,unsigned int ulLength);
#define  DRV_GET_FULL_HW_VER(pFullHwVersion,ulLength)  \
               BSP_MNTN_GetFullHwVersion(pFullHwVersion,ulLength)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetProductIdInter
 ��������  : ��Ʒ���������汾���ӿڡ�
 �������  : pProductIdInter��Ϊ�����ߴ����淵�صĲ�Ʒ���������汾���ڴ��׵�ַ��
             ulLength       ��Ϊ�����ߴ����淵�صĲ�Ʒ���������汾���ڴ��С��
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_GetProductIdInter(char * pProductIdInter, unsigned int ulLength);
#define  DRV_GET_PRODUCTID_INTER_VER(pProductIdInter,ulLength)  \
              BSP_MNTN_GetProductIdInter(pProductIdInter,ulLength)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_VersionQueryApi
 ��������  : ��ѯ��������İ汾�š�
 �������  : ppVersionInfo��������İ汾��Ϣ��ַ��
 �������  : ucLength�������ص����ݵ��ֽ�����
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_VersionQueryApi(void ** ppVersionInfo, unsigned int * ulLength);
#define  DRV_VER_QUERY(ppVersionInfo,ulLength)  \
              BSP_MNTN_VersionQueryApi (ppVersionInfo,ulLength)



typedef enum DRV_SHUTDOWN_REASON_tag_s
{
    DRV_SHUTDOWN_LOW_BATTERY,           /* ��ص�����                 */
    DRV_SHUTDOWN_BATTERY_ERROR,         /* ����쳣                   */
    DRV_SHUTDOWN_POWER_KEY,             /* ���� Power ���ػ�          */
    DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* ���±����ػ�               */
    DRV_SHUTDOWN_LOW_TEMP_PROTECT,
    DRV_SHUTDOWN_RESET,                 /* ϵͳ��λ                 */
    DRV_SHUTDOWN_CHARGE_REMOVE,         /* �ػ����ģʽ�£��γ������ */
    DRV_SHUTDOWN_UPDATE,                /* �ػ�����������ģʽ         */
    DRV_SHUTDOWN_BUTT
}DRV_SHUTDOWN_REASON_ENUM;

#ifdef FEATURE_HUAWEI_MBB_CHG
/*BATTERY(���)�����ϱ����¼�*/
typedef enum _GPIO_BAT_EVENT
{
    GPIO_BAT_CHARGING_ON_START = 0,  /* �ڿ���ģʽ����ʼ��� */
    GPIO_BAT_CHARGING_OFF_START,     /* �ڹػ�ģʽ����ʼ��� */   
    GPIO_BAT_LOW_POWER,              /* ��ص�����ʱ���ϱ����¼� */
    GPIO_BAT_CHARGING_UP_STOP,       /* ����ֹͣ���ʱ���ϱ����¼� */
    GPIO_BAT_CHARGING_DOWN_STOP,     /* �ػ�ֹͣ���ʱ���ϱ����¼� */
    GPIO_BAT_LOW_POWEROFF = 9,           /* ��ص����͵��µĹػ����ϱ����¼�*/       /* BEGIN: PN:��һ������¹�� Modified by d00135750, 2012/12/20   */
    GPIO_BAT_EVENT_MAX               /* ����¼�ֵ��С�ڴ�ֵ����Ϊ�Ƿ��¼� */
}GPIO_BATTERY_EVENT;

/*��ظ����ϱ��¼�*/
typedef enum _TEMP_EVENT
{
   GPIO_TEMP_BATT_LOW,   /* ��ص����¼� */      
   GPIO_TEMP_BATT_HIGH,   /* ��ظ����¼� */ 
   GPIO_TEMP_BATT_NORMAL, /* ��ظ��½���¼� */
   GPIO_TEMP_BATT_MAX     /* ����¼�ֵ��С�ڴ�ֵ����Ϊ�Ƿ��¼�*/
}GPIO_TEMP_EVENT;

/*KEY�����ϱ����¼�*/
typedef enum _GPIO_KEY_EVENT
{
    GPIO_KEY_WAKEUP = 0,           /* �̰����ⰴ�����ϱ����¼������ڵ�����Ļ */    
    GPIO_KEY_WLAN,                 /* ���ڴ�/�ر�WLAN�豸����ͬ�İ汾ʹ�õİ�����ͬ */
    GPIO_KEY_WPS,                  /* ����WPS��������5�룩ʱ���ϱ����¼������ڼ���WPS���� */
    GPIO_KEY_FACTORY_RESTORE,      /* ������λ��ʱ���ϱ����¼������ڻָ��������� */
    GPIO_KEY_POWER_OFF,            /* ����POWER��ʱ���ϱ����¼������ڹػ� */
    GPIO_KEY_UPDATE,               /* ��ϵͳ��Ҫ����ʱ���ϱ����¼�����������ϵͳ */
    GPIO_KEY_SSID,                 /* �̰�WPS����С��5�룩ʱ��������ʾSSID��ֻ�в��ְ汾��Ҫ */
    GPIO_KEY_WPS_PROMPT,           /* �̰�WPS��������1�룩ʱ��������ʾWPS��ʾ�ֻ�в��ְ汾��Ҫ */ 
    GPIO_KEY_POWER_SHORT,          /* �̰�Power������500������Ҫ�ɿ����� */
    GPIO_KEY_WPS_SHORT,            /* �̰�WPS������500������Ҫ�ɿ����� */
    GPIO_KEY_RESET_SHORT,          /* �̰�Reset������500 ������Ҫ�ɿ����� */
    GPIO_KEY_WPS_POWER,            /* WPS��POWER ��һ����2�� */
    GPIO_KEY_WPS_POWER_UP,         /* WPS��POWER ��һ����2�����5��ǰ̧�𣬻���5���δ��500������̧����WPS */
    GPIO_KEY_POWER_ONE_SECOND,     /* Power����1�� */
    GPIO_KEY_POWER_ONE_SECOND_UP,  /* Power����1�����2��ǰ̧�� */
    GPIO_KEY_WPS_ONE_SECOND,       /*WPS �̰� 1���¼�����Ҫ���ڿ��� WiFi Offload*/
    GPIO_KEY_WPS_DIAL= 16,         /*WPS �̰��ɿ��ϱ� WPS �����¼�����Ҫ���ڰ�������*/  
#ifdef FEATURE_FUNC_FAST_BOOT
    /*ƽ̨�ṩ5����ٹػ��¼�*/
    GPIO_KEY_SHORT_POWER_OFF = 17,
    /*ƽ̨�ṩ5����ٿ����¼�*/
    GPIO_KEY_SHORT_POWER_ON = 18,
 #endif
    GPIO_KEY_EVENT_MAX             /* ����¼�ֵ��С�ڴ�ֵ����Ϊ�Ƿ��¼� */
}GPIO_KEY_EVENT;
#endif /* FEATURE_HUAWEI_MBB_CHG */

/*****************************************************************************
 �� �� ��  : DRV_SHUT_DOWN
 ��������  : ����ػ�
 �������  : ��
 �������  : eReason��        �ػ�ԭ��
 �� �� ֵ  : ��
*****************************************************************************/
void drvShutdown( DRV_SHUTDOWN_REASON_ENUM eReason );
void DRV_SHUT_DOWN( DRV_SHUTDOWN_REASON_ENUM eReason );

typedef enum tagMNTN_FILE_TYPE_E
{
	MNTN_TYPE_ZSP = 0,
	MNTN_TYPE_HIFI,
    MNTN_TYPE_MAX
}MNTN_FILE_TYPE_E;

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ABBSelfCheck
 ��������  : ����abb�Լ���
 �������  : None
 �������  : None
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_ABBSelfCheck(void);
#define DRV_ABB_SELFCHECK()    BSP_MNTN_ABBSelfCheck()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_PmuSelfCheck
 ��������  : PMU�Լ졣
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_PmuSelfCheck(void);
#define DRV_PMU_SELFCHECK()    BSP_MNTN_PmuSelfCheck()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_I2cSelfCheck
 ��������  : ����I2c�Լ���
 �������  : None
 �������  : None
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע��������ýӿڽ���PS��PC������ʹ�ã�Ŀǰû��Ӧ�ã��ݱ�����
*****************************************************************************/
extern int BSP_MNTN_I2cSelfCheck(void);
#define DRV_I2C_SELFCHECK()    BSP_MNTN_I2cSelfCheck()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_SpiSelfCheck
 ��������  : ����spi�Լ���
 �������  : None
 �������  : None
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע��������ýӿڽ���PS��PC������ʹ�ã�Ŀǰû��Ӧ�ã��ݱ�����
*****************************************************************************/
extern int BSP_MNTN_SpiSelfCheck(void);
#define DRV_SPI_SELFCHECK()    BSP_MNTN_SpiSelfCheck()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetCodecFuncStatus
 ��������  : ��ȡCODEC����״̬
 �������  : ��
 �������  : ��
 �� �� ֵ  : 1:  CODEC����
             0��CODEC ������
*****************************************************************************/
extern  int BSP_MNTN_GetCodecFuncStatus(void);
#define DRV_GET_CODEC_FUNC_STATUS()    BSP_MNTN_GetCodecFuncStatus()

/*****************************************************************************
 �� �� ��  : MNTN_RFGLockStateGet
 ��������  : ��ȡGSM RF����״̬��
 �������  : �ޡ�
 �������  : Status���������״̬�����ֵ��
                        0��TX����RX��������
                        1��TX��RX��û�б�������

 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_RFGLockStateGet(unsigned int *Status);
#define DRV_GET_RF_GLOCKSTATE(Status)    BSP_MNTN_RFGLockStateGet(Status)

/****************************************************************************
 �� �� ��  : BSP_MNTN_LedStateFuncReg
 ��������  : ���ӿ�Ϊ�ṩ��Э��ջ�Ĺ��Ӻ��������ڻ�ȡЭ��ջ�ĵ��״̬��
 �������  : Э��ջ��ȡ���״̬������ָ�롣
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
 ע������  : �ޡ�

******************************************************************************/
extern void BSP_MNTN_LedStateFuncReg (pFUNCPTR p);
#define DRV_LED_STATE_FUNREG(p)    BSP_MNTN_LedStateFuncReg(p)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_LedFlush
 ��������  : ��ɫ�����á�
 �������  : status����ɫ�Ƶ�״̬����LED_STATUS_I
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_LedFlush(unsigned long state);
#define DRV_LED_FLUSH(state)    BSP_MNTN_LedFlush(state)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_LedControlAddressGet
 ��������  : �õ����Ƶĵ������
 �������  : ��
 �������  : �ޡ�
 �� �� ֵ  : ��ƿ������ݵ�ȫ�ֱ�����ַ
*****************************************************************************/
extern unsigned int BSP_MNTN_LedControlAddressGet(void);
#define DRV_LED_GET_ADDRESS()   BSP_MNTN_LedControlAddressGet()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_BootForceloadModeCheck
 ��������  : ǿ�Ƽ���ģʽ��ѯ��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0��ǿ�Ƽ��أ�
             1����ǿ�Ƽ��ء�
*****************************************************************************/
extern unsigned int BSP_MNTN_BootForceloadModeCheck(void);
#define DRV_BOOT_FORCELOAD_MODE_CHECK()    BSP_MNTN_BootForceloadModeCheck()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_BootFlagConfig
 ��������  : ����Bootline�����е�������־Ϊ����������������ʱ7�����������ֶ�������
 �������  : ulBootFlag����δ���塣
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern  int BSP_MNTN_BootFlagConfig(unsigned int ulBootFlag);
#define DRV_BOOTFLAG_CFG(ulBootFlag)    BSP_MNTN_BootFlagConfig(ulBootFlag)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_CheckArmTCM
 ��������  : TCM���
 �������  : �ޡ�
 �������  : �ޡ�
 ����ֵ��   0xffff0000�����ɹ���TCM����
            ��ַ��������ĵ�ַ��0��0x5fff��
            0xffffffff:���ʧ��

*****************************************************************************/
extern unsigned int BSP_MNTN_CheckArmTCM(void);
#define DRV_CHECK_ARM_TCM()    BSP_MNTN_CheckArmTCM()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExchOMSaveRegister
 ��������  : ע��������trace���溯���������ڵ��帴λʱ��
             ���浱ǰ���������־��Ϣ��
 �������  : funcType: ����ID
             pFunc:ע��Ļص�����
 �������  : None
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע���������ͬ��funcTypeֻ��ע��һ���ص��������ظ�ע��Ḳ��ԭ������
*****************************************************************************/
extern int BSP_MNTN_ExchOMSaveRegister(int funcType, OM_SAVE_FUNC *pFunc);
#define DRV_SAVE_REGISTER(funcType, pFunc)    BSP_MNTN_ExchOMSaveRegister(funcType, pFunc)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExchOMRegisterTdsPhy
 ��������  : TDSע���������ڴ��ַ����������ʱ��¼TDS�������Ϣ������BIN�ļ�����ʼ��ʱ����
 �������  : ulLength: �ڴ泤��
            tphyaddr: ��ʼ��ַ
 �������  : None
 �� �� ֵ  : 
 ע�������
*****************************************************************************/
extern void BSP_MNTN_ExchOMRegisterTdsPhy(BSP_U32 ulLength,char * tphyaddr);


/*****************************************************************************
 �� �� ��  : BSP_MNTN_SystemError
 ��������  : ϵͳ��������
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
#define DRV_SYSTEM_ERROR(modId, arg1, arg2, arg3, arg3Length)\
                   BSP_MNTN_SystemError(modId, arg1, arg2, arg3, arg3Length)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExchMemMalloc
 ��������  : ��systemOsaError�ӿ�һ��������¼���ĸ�λ��Ϣ��
             Ŀǰ���֧��128k.
 �������  : ulSize - �ڴ��С
 �������  : �ޡ�
 �� �� ֵ  : �ڴ��ַ
*****************************************************************************/
extern unsigned int BSP_MNTN_ExchMemMalloc(unsigned int ulSize);
#define DRV_EXCH_MEM_MALLOC(ulSize)    BSP_MNTN_ExchMemMalloc(ulSize)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_OmExchFileSave
 ��������  : ������¼ARM��DSP�������쳣�ļ�
 �������  :    address��buffer��ַ
                length���洢����
                IsFileEnd��1��ʾ�ļ������һ�飬0��ʾ�����һ��
 �������  : �ޡ�
 �� �� ֵ  : 0 ΪOK��
*****************************************************************************/
extern int BSP_MNTN_OmExchFileSave(void * address, unsigned long length,unsigned char IsFileEnd,unsigned char type);
#define DRV_EXCH_FILE_SAVE(address, length, IsFileEnd, type) BSP_MNTN_OmExchFileSave(address, length, IsFileEnd, type)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetHwGpioInfo
 ��������  : Get flash infomation
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern int BSP_MNTN_GetHwGpioInfo(unsigned char *pGpioInfo, unsigned long usLength );
#define DRV_GET_GPIO_INFO(pGpioInfo, usLength)    BSP_MNTN_GetHwGpioInfo(pGpioInfo, usLength )

/*****************************************************************************
 �� �� ��  : BSP_MNTN_Int4ToString
 ��������  : ���Ĵ�����ֵת��ΪASCII�ַ�
 �������  : �Ĵ�����ֵ����ת���ַ��Ĵ洢�ռ�,���ռ䲻����30byte
 �������  : ת���ַ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_Int4ToString(unsigned int value, char * string);
#define DRV_MEM_VERCONVERT(value,string)    BSP_MNTN_Int4ToString(value, string)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_HeapInfoGet
 ��������  : ��ȡ�������ڴ���Ϣ��
 �������  : allocSize: �Ѿ�����Ķ��ڴ��С����λbyte��
             totalSize: ���ڴ��ܳߴ磬��λbyte��
 �������  : None
 �� �� ֵ  : 0: �����ɹ���
             -1��  ����ʧ�ܡ�
*****************************************************************************/
extern int BSP_MNTN_HeapInfoGet(unsigned int *allocSize, unsigned int *totalSize);
#define DRV_GET_HEAPINFO(allocSize, totalSize)    BSP_MNTN_HeapInfoGet(allocSize, totalSize)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetSupportBands
 ��������  : �ӵ����õ�ǰ֧�ֵ�Bands.
 �������  : pusWBands - WCDMA��Ƶ��ֵ
             pusGBands - GSM��Ƶ��ֵ

 �������  : pusWBands - WCDMA��Ƶ��ֵ
             pusGBands - GSM��Ƶ��ֵ

 �� �� ֵ  : 0����ȷ����0: ʧ��
             ͨ��Bitλ����ʾ��ЩƵ��֧�֡�
*****************************************************************************/
extern int BSP_MNTN_GetSupportBands(unsigned short *pusWBands, unsigned short *pusGBands);
#define DRV_GET_SUPPORT_BANDS(pusWBands, pusGBands)    BSP_MNTN_GetSupportBands(pusWBands, pusGBands)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetSupportDivBands
 ��������  : �ӵ����õ�ǰ֧�ֵķּ�Ƶ��
 �������  : N/A

 �������  : pDivBands - ���ص�ǰ������֧�ֵķּ�Ƶ�Σ�Ƶ�ΰ�bit
 			      ����ʽ����(ͬW ��G��Ƶ�β�ѯ)

 �� �� ֵ  : 0����ȷ����0: ʧ��
*****************************************************************************/
extern int BSP_MNTN_GetSupportDivBands(unsigned short* pDivBands);
#define DRV_GET_SUPPORT_DIVBANDS(pDivBands)    BSP_MNTN_GetSupportDivBands(pDivBands)

/********************************************************************************************************
 �� �� ��  : BSP_MNTN_WriteSysBackupFile
 ��������  : ��FLASH����ZSP��NV����
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int BSP_MNTN_WriteSysBackupFile(void);
#define DRV_BACKUP_SYS_FILE()    BSP_MNTN_WriteSysBackupFile()

/*****************************************************************************
 �� �� ��  : DRV_MNTN_GetExtAntenLockState
 ��������  : ��ȡ���������ߵ�����״̬
 �������  : None
 �������  : 0 - ��������
             1 - ��������
 �� �� ֵ  : 0 - �ɹ�
             ����Ϊʧ��

*****************************************************************************/
extern int DRV_MNTN_GetExtAntenLockState(unsigned int *Status);
#define DRV_GET_ANTEN_LOCKSTATE(Status)    DRV_MNTN_GetExtAntenLockState(Status)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExtAntenIntInstall
 ��������  : �ж�ע�ắ����������õ�ǰ������״̬
 �������  : routine   - �жϴ�����
             para      - �����ֶ�
 �������  : None
 �� �� ֵ  : void

*****************************************************************************/
extern void BSP_MNTN_ExtAntenIntInstall(void* routine, int para);
#define DRV_ANTEN_INT_INSTALL(routine, para)    BSP_MNTN_ExtAntenIntInstall(routine, para)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExchHookFuncAdd
 ��������  : �����л����жϹ���ע��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
extern void BSP_MNTN_ExchHookFuncAdd(void);
#define DRV_EXCH_HOOK_FUNC_ADD()    BSP_MNTN_ExchHookFuncAdd()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExchHookFuncDelete
 ��������  : �����л����жϹ���ע��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
extern void BSP_MNTN_ExchHookFuncDelete(BSP_CHAR* file, BSP_U32 line);
#define DRV_EXCH_HOOK_FUNC_DELETE()    BSP_MNTN_ExchHookFuncDelete(__FILE__, __LINE__)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_ExcStackPeakRecord
 ��������  : ��¼����ջ�����������Ϣ.
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_ExcStackPeakRecord(void);
#define DRV_STACK_HIGH_RECORD()    BSP_MNTN_ExcStackPeakRecord()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_SubIntHook
 ��������  : �����жϷ����Ӻ���ע�ᡣ��ά�ɲ����
 �������  : p_Func�����Ӻ���ָ�롣
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_SubIntHook(pFUNCPTR p_Func);
#define DRV_VICINT_HOOK_ADD(p_Func)    BSP_MNTN_SubIntHook(p_Func)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_SubIntHookDel
 ��������  : �����жϷ����Ӻ���ע������ά�ɲ����
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_SubIntHookDel(void);
#define DRV_VICINT_HOOK_DEL()    BSP_MNTN_SubIntHookDel()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_IntBreakInHook
 ��������  : һ���ж���ڹ��Ӻ���ע�ᡣ��ά�ɲ����
 �������  : p_Func :���Ӻ�����
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_IntBreakInHook(pFUNCPTR p_Func);
#define DRV_VICINT_IN_HOOK_ADD(p_Func)   BSP_MNTN_IntBreakInHook(p_Func)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_IntBreakInHookDel
 ��������  : һ���ж���ڹ��Ӻ���ע������ά�ɲ����
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_IntBreakInHookDel(void);
#define DRV_VICINT_IN_HOOK_DEL()    BSP_MNTN_IntBreakInHookDel()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_IntBreakOutHook
 ��������  : һ���жϳ��ڹ��Ӻ���ע�ᡣ��ά�ɲ���أ�V7�ȴ�׮
 �������  : p_Func: ���Ӻ�����
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_IntBreakOutHook(pFUNCPTR p_Func);
#define DRV_VICINT_OUT_HOOK_ADD(p_Func)    BSP_MNTN_IntBreakOutHook(p_Func)

/*****************************************************************************
 �� �� ��  : vicIntBreakOutHookDel
 ��������  : һ���жϳ��ڹ��Ӻ���ע������ά�ɲ����
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_IntBreakOutHookDel(void);
#define DRV_VICINT_OUT_HOOK_DEL()    BSP_MNTN_IntBreakOutHookDel()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_IntLvlChgHook
 ��������  : һ���ж�level change���Ӻ���ע�ᡣ��ά�ɲ����
 �������  : p_Func: ���Ӻ�����
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_IntLvlChgHook(pFUNCPTR p_Func);
#define DRV_VICINT_LVLCHG_HOOK_ADD(p_Func)    BSP_MNTN_IntLvlChgHook(p_Func)

/*****************************************************************************
 �� �� ��  : BSP_MNTN_IntLvlChgHookDel
 ��������  : һ���ж�level change���Ӻ���ע������ά�ɲ����
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_MNTN_IntLvlChgHookDel(void);
#define DRV_VICINT_LVLCHG_HOOK_DEL()    BSP_MNTN_IntLvlChgHookDel()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_TFUPIfNeedNvBackup
 ��������  : ʹ��TF������ǰͨ���ýӿ��ж��Ƿ���Ҫ����NV��.
 �������  : None
 �������  : None
 �� �� ֵ  : 1:��Ҫ����
             0:���豸��
*****************************************************************************/
extern int BSP_MNTN_TFUPIfNeedNvBackup(void);
#define DRV_TF_NVBACKUP_FLAG()    BSP_MNTN_TFUPIfNeedNvBackup()

/*****************************************************************************
 �� �� ��  : BSP_MNTN_GetSDDevName
 ��������  : ��ȡ�豸�б��е�SD�豸��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : SD_DEV_TYPE_STR�ṹָ�롣
*****************************************************************************/
extern SD_DEV_TYPE_STR * BSP_MNTN_GetSDDevName(void);
#define DRV_GET_SD_PATH()     BSP_MNTN_GetSDDevName()

/*****************************************************************************
 �� �� ��  : BSP_PROD_GetRecoverNvInfo
 ��������  : �ӵ����õ�ǰ��Ҫ���ǵ�NV��Ϣ
 �������  : N/A

 �������  : ppNvInfo  - ָ��NV��Ϣ�ṹ�������
             pulNvNum  - �������

 �� �� ֵ  : 0����ȷ����0: ʧ��
*****************************************************************************/
extern  int BSP_PROD_GetRecoverNvInfo(void **ppNvInfo, unsigned long *pulNvNum);
#define  DRV_GET_RECOVER_NV_INFO(ppNvInfo, pulNvNum)  \
                BSP_PROD_GetRecoverNvInfo(ppNvInfo, pulNvNum)


/*****************************************************************************
 �� �� ��  : BSP_MNTN_GETSDSTATUS
 ��������  : ����SD����λ״̬
 �������  : �ޡ�
 �������  : �ޡ�
 ����ֵ��   1��λ
            0����λ

*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int BSP_MNTN_GETSDSTATUS(void);
#define DRV_SDMMC_GET_STATUS()    BSP_MNTN_GETSDSTATUS()
#else
extern int BSP_SDMMC_GetSDStatus(void);
#define DRV_SDMMC_GET_STATUS()    BSP_SDMMC_GetSDStatus()
#endif

/*****************************************************************************
 �� �� ��  : SDIO_read_write_blkdata
 ��������  : SD����д�ӿ�
 �������  :    nCardNo     SD����
                dwBlkNo     ����
                nBlkCount   ��д��С
                pbDataBuff  ��д������
                nFlags      ��/д
 �������  : �ޡ�
 ����ֵ��   0:  ��д�ɹ�
            ��0:������
*****************************************************************************/
extern int BSP_MNTN_SDRWBlkData(int nCardNo,unsigned int dwBlkNo,
                   unsigned int nBlkCount, unsigned char *pbDataBuff,int nFlags);
#define DRV_SDMMC_BLK_DATA_RW(nCardNo,dwBlkNo,nBlkCount,pbDataBuff,nFlags)   \
             BSP_MNTN_SDRWBlkData(nCardNo,dwBlkNo,nBlkCount,pbDataBuff, nFlags)


/***********************************�ڴ濽���Ż�*****************************************/
/*****************************************************************************
 �� �� ��  : __rt_memcpy
 ��������  : ���汾��memcpy����
 �������  : Dest :Ŀ�ĵ�ַ
             Src :Դ��ַ
             Count:�������ݵĴ�С
 �������  : �ޡ�
 �� �� ֵ  : Ŀ�ĵ�ַ��
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern void * __rt_memcpy(void * Dest, const void * Src, unsigned long Count);
#define DRV_RT_MEMCPY(Dest,Src,Count)       __rt_memcpy(Dest,Src,Count)
#else
#define DRV_RT_MEMCPY(Dest,Src,Count)       memcpy(Dest,Src,Count)
#endif



/************************************����궨��Ϊ�յĺ���******************************************/
/*****************************************************************************
 �� �� ��  : UsbStatusCallbackRegister
 ��������  : ��¼pCallBack��ȫ�ֱ����С�SD��������ؽӿڣ�V7�ȴ�׮
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
 /*extern int  UsbStatusCallbackRegister(pUsbState pCallBack);*/
#define DRV_USB_STATUS_CALLBACK_REGI(pCallBack)              DRV_OK

/*****************************************************************************
 �� �� ��  : CicomClkEnable
 ��������  : This routine enable CICOM IP clock gating.�͹�����أ�V7��׮
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
/*extern void CicomClkEnable(void);*/
#define DRV_CICOM_CLK_ENABLE()

/*****************************************************************************
 �� �� ��  : CicomClkDisable
 ��������  : This routine disable CICOM IP clock gating.�͹�����أ�V7��׮
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
/*extern void CicomClkDisable(void);*/
#define DRV_CICOM_CLK_DISABLE()

/*****************************************************************************
 �� �� ��  : HdlcClkEnable
 ��������  : This routine enable HDLC IP clock gating.�͹�����أ�V7��׮
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
/*extern void HdlcClkEnable(void);*/
#define DRV_HDLC_CLK_ENABLE()

/*****************************************************************************
 �� �� ��  : HdlcClkDisable
 ��������  : This routine disable HDLC IP clock gating.�͹�����أ�V7��׮
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
/*extern void HdlcClkDisable(void);*/
#define DRV_HDLC_CLK_DISABLE()

/*****************************************************************************
 �� �� ��  : DRV_MEM_READ
 ��������  : ��32λ��Ȳ�ѯ�ڴ棬���4�ֽ����ݡ���ά�ɲ⣬V7�ȴ�׮
 �������  : ulAddress����ѯ��ַ�ռ��ַ����ַ�����ڴ淶Χ�ڣ����򷵻�-1��
 �������  : pulData��ָ���ѯ���ݵ�ָ�롣
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int DRV_MEM_READ(unsigned int ulAddress, unsigned int* pulData);

/*****************************************************************************
 �� �� ��  : DrvLogInstall
 ��������  : ��ӡ����ע�ᡣ��ά�ɲ⣬V7�ȴ�׮
 �������  : fptr ע��ĺ���ָ��
 �������  : None
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
/*extern int DrvLogInstall(PVOIDFUNC fptr, PVOIDFUNC fptr1, PVOIDFUNC fptr2, PVOIDFUNC fptr3, PVOIDFUNC fptr4);*/
#define DRV_LOG_INSTALL(fptr, fptr1, fptr2, fptr3, fptr4)     DRV_OK

/*****************************************************************************
* �� �� ��  : BSP_MNTN_GetBattState
*
* ��������  : ��ȡ�����λ״̬
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : �ɹ���0��1��2
              ʧ�ܣ�����
*
*
*****************************************************************************/
extern BSP_S32 BSP_MNTN_GetBattState(void);
#define DRV_MNTN_GET_BATT_STATE() BSP_MNTN_GetBattState()

/*****************************************************************************
 �� �� ��  : pwrctrl_wpa_pwr_up
 ��������  : RF�µ�
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
extern void pwrctrl_wpa_pwr_up(void);
#define DRV_WPA_POWERUP()

/*****************************************************************
Function: free_mem_size_get
Description:
    get mem size
Input:
    N/A
Output:
    N/A
Return:
    free mem size
*******************************************************************/
extern unsigned long free_mem_size_get(void);
#define FREE_MEM_SIZE_GET() free_mem_size_get()

/*****************************************************************************
 �� �� ��  : BSP_DMR_ATAnalyze
 ��������  : ��NAS��ѯ��̬�ڴ�ռ����Ϣ��ͨ��AT�����ѯ
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ɹ�0��ʧ��-1
*****************************************************************************/
extern int BSP_DMR_ATANALYZE(void * pstMem,unsigned int uiSize, unsigned int * puiModIdSum);

/*************************MNTN END************************************/

/*************************DPM START***********************************/

#define PWRCTRL_GET_SUCCESS         (0x00)
#define PWRCTRL_GET_PARAINVALID     (0x01)
#define PWRCTRL_ONORLOCK            (0x10)
#define PWRCTRL_OFFORUNLOCK         (0x20)

typedef unsigned long (*pULULFUNCPTR)( unsigned long ulPara );

typedef struct
{
    pULULFUNCPTR pRestoreFunction;
    pULULFUNCPTR pEnableFunction;
    pULULFUNCPTR pDisableFunction;
    pFUNCPTR2    pDfsSetFunc;
    pULULFUNCPTR pDfsReleaseFunc;
}DPM_SLEEP_CALLBACK_STRU;

/*wangwei dfs*/
typedef  enum
{
    PWRCTRL_DFS_USB = 0,
    PWRCTRL_DFS_SD  = 1,
    PWRCTRL_DFS_PS1 = 2,
    PWRCTRL_DFS_HIFI = 3,  /*hifi*/
    PWRCTRL_DFS_BUTT
} PWC_DFS_ID_E;

typedef enum
{
    DFS_PROFILE_0 = 0,            /*Ƶ�ʼ���1*/
    DFS_PROFILE_1 = 1,            /*Ƶ�ʼ���2*/
    DFS_PROFILE_2 = 2,            /*Ƶ�ʼ���3*/
    DFS_PROFILE_3 = 3,            /*Ƶ�ʼ���4*/
    DFS_PROFILE_4 = 4,            /*Ƶ�ʼ���5*/
    DFS_PROFILE_5 = 5,
    DFS_PROFILE_6 = 6,
    DFS_PROFILE_7 = 7,
    DFS_PROFILE_8 = 8,
    DFS_PROFILE_BUTT
} PWC_DFS_PROFILE_E;


typedef enum tagPWC_COMM_MODE_E
{
    PWC_COMM_MODE_WCDMA,
    PWC_COMM_MODE_GSM,
    PWC_COMM_MODE_LTE,
    PWC_COMM_MODE_TDS,
    PWC_COMM_MODE_BUTT
}PWC_COMM_MODE_E;

typedef enum tagPWC_COMM_MODULE_E
{
    PWC_COMM_MODULE_PA,
    PWC_COMM_MODULE_RF,
    PWC_COMM_MODULE_BBP_SOC,
    PWC_COMM_MODULE_BBP_PWR,
    PWC_COMM_MODULE_DSP,
    PWC_COMM_MODULE_ABB,
    PWC_COMM_MODULE_HIFI,
    PWC_COMM_MODULE_BUTT
}PWC_COMM_MODULE_E;

/* ˯��ͶƱID */
typedef enum tagPWC_CLIENT_ID_E
{
    PWRCTRL_SLEEP_SCI       = 0x100,
    PWRCTRL_SLEEP_PS        = 0x101,
    PWRCTRL_SLEEP_BBPMASTER = 0x102,
    PWRCTRL_SLEEP_CICOM     = 0x103,
    PWRCTRL_SLEEP_NV        = 0x104,
	PWRCTRL_SLEEP_RNIC      = 0x105,
	PWRCTRL_SLEEP_APP		= 0x106,
	PWRCTRL_SLEEP_TDSPS     = 0x107,
    PWRCTRL_SLEEP_DMA       = 0x108,
    PWRCTRL_SLEEP_FLASH     = 0x109,
    PWRCTRL_SLEEP_SD        = 0x10A,
    PWRCTRL_SLEEP_FTM       = 0x10B,
    PWRCTRL_SLEEP_OAM       = 0x10C,
    PWRCTRL_SLEEP_SOCP      = 0x10D,
    PWRCTRL_SLEEP_SPECIALTIMER        = 0x10E,	
    PWRCTRL_SLEEP_GUDSP_EDMA = 0x10E, 

    PWRCTRL_SLEEP_BOOT      = 0x110,
    PWRCTRL_SLEEP_ACPU      = 0x111,
    PWRCTRL_SLEEP_HSIC      = 0x112,

    PWRCTRL_LIGHTSLEEP_WIFI = 0x113, /* HSIC���ܵ��� */
    PWRCTRL_SLEEP_WIFI      = 0x114,
    PWRCTRL_SLEEP_USB       = 0x115,
    PWRCTRL_SLEEP_LCD       = 0x116,
    PWRCTRL_SLEEP_KEY     	= 0x117,
    PWRCTRL_SLEEP_ACM     	= 0x118,
    PWRCTRL_SLEEP_ICC     	= 0x119,
    PWRCTRL_SLEEP_NAS     	= 0x11a,
    PWRCTRL_SLEEP_LTEPS     = 0x11b,
    PWRCTRL_SLEEP_IFC		= 0x11c,
	PWRCTRL_SLEEP_CHG		= 0x11d,
    /* Test */
    PWRCTRL_TEST_LIGHTSLEEP = 0x11E,
    PWRCTRL_TEST_DEEPSLEEP  = 0x11F,
    PWRCTRL_CLIENT_BUTT     = 0x120
} PWC_CLIENT_ID_E;

typedef struct
{
    PWRCTRLFUNCPTRVOID  pFuncDsIn;
    PWRCTRLFUNCPTRVOID  pFuncDsOut;
}PWC_DS_SOCP_CB_STRU;

typedef enum tagPWC_TEMP_PROTECT_E
{
    PWC_TEMP_OLED,
    PWC_TEMP_BATTERY,
	PWC_TEMP_L_PA,
    PWC_TEMP_G_PA,
    PWC_TEMP_W_PA,
    PWC_TEMP_SIM_CARD,
    PWC_TEMP_DCXO,
    /* BEGIN PN:N/A,Modified by c00191475, 2013/01/08*/
    PWC_TEMP_SURFACE,/*Surfaceö��������*/
    /* END   PN:N/A,Modified by c00191475, 2013/01/08*/
    PWC_TEMP_BUTT
}PWC_TEMP_PROTECT_E;

typedef struct
{
    PWRCTRLFUNCPTR drx_slow;
} PWC_SLEEP_CALLBACK_STRU;


typedef enum tagPWRCTRL_MODULE_E
{
    PWRCTRL_MODU_BOOTROM,       /*0x0*/
    PWRCTRL_MODU_EFUSE,
    PWRCTRL_MODU_SSDMA,
    PWRCTRL_MODU_LBBP,
    PWRCTRL_MODU_LDSP,    	
    PWRCTRL_MODU_HSUART,
    PWRCTRL_MODU_EMI,  
    PWRCTRL_MODU_SDCC,
    PWRCTRL_MODU_USBHSIC,
    PWRCTRL_MODU_SOCP,
    PWRCTRL_MODU_GMAC,          /* 0xa */
    PWRCTRL_MODU_SCI0,
    PWRCTRL_MODU_SCI1,
    PWRCTRL_MODU_UICC,
    PWRCTRL_MODU_USBOTG_BC,
    PWRCTRL_MODU_MMC0,              /*0x10*/
    PWRCTRL_MODU_MMC1,
    PWRCTRL_MODU_MMC2,
    PWRCTRL_MODU_USBOTG,
    PWRCTRL_MODU_WDT,
    PWRCTRL_MODU_TCSSI,
    PWRCTRL_MODU_IPF,
    PWRCTRL_MODU_CIPHER,
    PWRCTRL_MODU_BBPCOMM,
    PWRCTRL_MODU_TENSI,             /*0x1a*/
    PWRCTRL_MODU_CORESIGHT,
    PWRCTRL_MODU_ACP,
    PWRCTRL_MODU_I2C,
    PWRCTRL_MODU_SPI1,
    PWRCTRL_MODU_DMAC,
    PWRCTRL_MODU_LSIO,                  /*0x20*/
    PWRCTRL_MODU_HIFI,
    PWRCTRL_MODU_TSENSOR,
    PWRCTRL_MODU_CICOM,
    PWRCTRL_MODU_EDMA,
    PWRCTRL_MODU_HDLC,
    PWRCTRL_MODU_DM,
    PWRCTRL_MODU_DWSSI0,
    PWRCTRL_MODU_WGBBP,
    PWRCTRL_MODU_ZSP,
    PWRCTRL_MODU_NANDC,
    PWRCTRL_MODU_SSI0,
    PWRCTRL_MODU_SSI1,
    PWRCTRL_MODU_SSI2,
    PWRCTRL_MODU_SSI3,
    PWRCTRL_MODU_SPI0,
    PWRCTRL_MODU_MST,
    PWRCTRL_MODU_ALL,   /* �������е�ģ�� */
    PWRCTRL_MODU_MAX    /* �߽�ֵ */
}PWRCTRL_MODULE_E;
/************************************************************************
 * FUNCTION
 *      PWRCTRL_BSP_GuDsp_StatusInfo
 * DESCRIPTION
 *        ���ڲ�ѯGUDSP_ZSPcore��λ���⸴λʱ��ʹ���
 * INPUTS
 *        GuZspAddr :GU_ZSP���渴λ���⸴λ��Ϣ��ָ���ַ
 * OUTPUTS
           BSP_ERROR:ָ��Ƿ���
 *************************************************************************/
 BSP_U32 PWRCTRL_BSP_GuDsp_StatusInfo(BSP_U32 *GuZspAddr);

/************************************************************************
 * FUNCTION
 *      PWRCTRL_BSP_RTT_StatusInfo
 * DESCRIPTION
 *        BBP ״̬��ѯ
 * INPUTS
 *       NONE
 * OUTPUTS
          BSP_BBP_ERROR: ���ܷ���
          BSP_BBP_OK:���Է���
 *************************************************************************/
BSP_U32  PWRCTRL_BSP_RTT_StatusInfo (BSP_VOID);


/*****************************************************************************
* �� �� ��  : BSP_PWRCTRL_TimerClose
*
* ��������  : SOC����ʱ�ӹرսӿ�
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerClose(PWRCTRL_MODULE_E enModu);

/*****************************************************************************
* �� �� ��  : BSP_PWRCTRL_TimerOpen
*
* ��������  : SOC����ʱ��ʹ�ܽӿ�
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerOpen(PWRCTRL_MODULE_E enModu);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_DEEPSLEEP_FOREVER
 ��������  : AARM CARM �µ�ӿ�
 �������  : None
 �������  : None
 �� �� ֵ  : None

*****************************************************************************/
extern void DRV_PWRCTRL_DEEPSLEEP_FOREVER(void );

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_SLEEP_CALLBACK_REG
 ��������  : ע��PWC����������ʹ��
 �������  : ��ע��ĺ���
 �������  : None
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int DRV_PWRCTRL_SLEEP_CALLBACK_REG(PWC_SLEEP_CALLBACK_STRU pCallback);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_GET_TEMP
 ��������  : ��ȡOLED����ء�GPA��WPA��SIM����DCXO���¶ȡ�
 �������  : enModule:PWC_TEMP_PROTECT_E
             hkAdcTable:��ѹ�¶Ȼ����
 �������  : pslData:�洢�ĵ�ѹת��Ϊ���¶�ֵ
 �� �� ֵ  : HKADC_OK/HKADC_ERROR ��
*****************************************************************************/
/*V3R2 CS f00164371������ӿ���ʱ�����룬��ΪSFTƽ̨�Ƿ���0��ʵ�ʻ�Ƭ��Ҫʵ�֣�
��Ҫʹ��CS_SFT��������֣����ǲ�ͬ�����CS_SFT�겻һ��ͳһ�����Բ���ֱ�����ε���
�����ڽӿڴ����ڲ����и���*/
extern BSP_S32 DRV_PWRCTRL_GET_TEMP(PWC_TEMP_PROTECT_E enModule, BSP_U16 *hkAdcTable,BSP_S32 *pslData);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_AHB_BAK_ADDR_ALLOC
 ��������  : arm���µ繦�ܣ���Ҫ����ahb���ݣ��ýӿ��������ݱ����ַ�ͳ���
 �������  : length     ��������ռ��С��ָ��
 �������  : ��
 �� �� ֵ  : ����ռ�ĵ�ַ ��
*****************************************************************************/
/*V3R2 CS f00164371������ӿ���ʱ������*/
extern void * DRV_PWRCTRL_AHB_BAK_ADDR_ALLOC(unsigned int * length);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_GetCcpuLoadCB
 ��������  : arm �ṩ��TTF�Ļص�����
 �������  : pFunc:TTF����ָ��
 �������  : ��
 �� �� ֵ  : ����ռ�ĵ�ַ ��
*****************************************************************************/
extern void DRV_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc );

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_GetCpuLoad
 ��������  : arm cpuռ���ʲ�ѯ����
 �������  : ��
 �������  : ulAcpuLoad:Acpuռ���ʵ�ַ.
             ulCcpuLoad:Ccpuռ���ʵ�ַ.
 �� �� ֵ  : 0/1 ��
*****************************************************************************/
extern unsigned int DRV_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_PWRUP
 ��������  : Wģ��Gģ��Lģ��PA��RF��BBP��DSP��ABB��HIFI���ϵ���ơ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
/*V3R2 CS f00164371������ӿ���ʱ�����룬��ΪHIFI������Ҫ���ں����ڲ������˸���*/
extern BSP_U32 DRV_PWRCTRL_PWRUP (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_PWRDOWN
 ��������  : Wģ��Gģ��Lģ��PA��RF��BBP��DSP��ABB��HIFI���µ���ơ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
/*V3R2 CS f00164371������ӿ���ʱ�����룬��ΪHIFI������Ҫ���ں����ڲ������˸���*/
extern BSP_U32 DRV_PWRCTRL_PWRDOWN (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PwrStatusGet
 ��������  : Wģ��Gģ��Lģ��PA��RF��BBP��DSP��ABB�����µ�״̬��ѯ��
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
extern  BSP_U32 DRV_PWRCTRL_PWRSTATUSGET(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_PLL_ENABLE
 ��������  : Wģ��Gģ��Lģ��PA��DSP��ABB��PLLʹ�ܡ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_PLL_ENABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_PLL_DISABLE
 ��������  : Wģ��Gģ��Lģ��PA��DSP��ABB��PLLȥʹ�ܡ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_PLL_DISABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_DSP_RESET
 ��������  : Lģ��DSP��λ�ӿڡ�
 �������  : 
 �������  : None
 �� �� ֵ  : 
*****************************************************************************/
extern BSP_VOID DRV_PWRCTRL_DSP_RESET();

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PllStatusGet
 ��������  : Wģ��Gģ��Lģ��PA��DSP��ABB��PLL״̬��ѯ��
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_PllStatusGet (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_ZspSleepFlagStore
 * DESCRIPTION
 *       ZSP˯�߱�־λ����ӿ�
 * INPUTS
 *       
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern BSP_VOID DRV_ZSP_SLEEP_FALG_STORE(BSP_VOID);

/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_ZspSleepFlagRead
 * DESCRIPTION
 *       ZSP˯�߱�־λ��ȡ�ӿ�
 * INPUTS
 *       
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern BSP_U32 DRV_ZSP_SLEEP_FALG_READ(BSP_VOID);


/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_SLEEPVOTE_LOCK
 ��������  : �����ֹ˯��ͶƱ�ӿڡ�
 �������  : enClientId:PWC_CLIENT_ID_E
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_SLEEPVOTE_LOCK(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 ��������  : ��������˯��ͶƱ�ӿڡ�
 �������  : enClientId:PWC_CLIENT_ID_E
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_SET_PRF_CCPU
Description:Set the System Min Profile
Input:      ulClientID: The Vote Module Client
            enProfile:  The Min Profile Value
Output:     None
Return:     The Vote Result
Others:
*****************************************************************************/
extern BSP_U32  DRV_PWRCTRL_DFS_SET_PRF_CCPU(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile);

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_RLS_PRF_CCPU
Description:Release the Vote Result
Input:      ulClientID: The Vote Module Client
Output:     None;
Return:     The Vote Result
Others:
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_DFS_RLS_PRF_CCPU(PWC_DFS_ID_E ulClientID);

/*****************************************************************************
Function:   DRV_PWRCTRL_SLEEP_IN_CB
Description:
Input:
Output:     None;
Return:
Others:
*****************************************************************************/
extern int DRV_PWRCTRL_SLEEP_IN_CB(PWC_DS_SOCP_CB_STRU stFunc);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_UsbLowPowerEnter
 ��������  : USB����͹��Ľӿ�
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
extern void DRV_PWRCTRL_USB_LOWPOWER_ENTER(void);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_USB_LOWPOWER_EXIT
 ��������  : USB�˳��͹��Ľӿ�
 �������  : None
 �������  : None
 �� �� ֵ  : None
*****************************************************************************/
extern void DRV_PWRCTRL_USB_LOWPOWER_EXIT(void);


/*****************************************************************************
 �� �� ��  : BSP_32K_GetTick
 ��������  : 32Kʱ�Ӷ�ӦTick��ѯ�ӿ�
 �������  : None
 �������  : None
 �� �� ֵ  : 32Kʱ�Ӷ�ӦTickֵ
*****************************************************************************/
extern BSP_U32 BSP_32K_GetTick( BSP_VOID );


/*****************************************************************************
Function:   BSP_PWC_GetMeansFlag
Description:
Input:
Output:     the means flag value;
Return:
Others:
*****************************************************************************/
extern BSP_S32 BSP_PWC_GetMeansFlag(PWC_COMM_MODE_E enCommMode);
#define DRV_PWC_GET_MEANSFLAG(enCommMode) BSP_PWC_GetMeansFlag(enCommMode)

extern BSP_VOID BSP_PWC_MspVoteRegister(FUNCPTR routine);
extern BSP_VOID BSP_PWC_SocpVoteRegister(FUNCPTR routine);
extern BSP_VOID BSP_PWC_SocpRestoreRegister(FUNCPTR routine);

/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: ����timer4��Ϊ����Դ
 Input      : 
 Return     : void
 Other      : 
*****************************************************************************/
extern VOID BSP_PWC_SetTimer4WakeSrc(VOID);

/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: ����timer4����Ϊ����Դ 
 Input      :  
            : 
 Return     : void
 Other      : 
*****************************************************************************/
extern VOID BSP_PWC_DelTimer4WakeSrc(VOID);


/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 ��������  : AT^PSTANDBY
 �������  :
 �������  :
 ����ֵ��
*****************************************************************************/
extern unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
extern unsigned int BSP_PWRCTRL_StandbyStateCcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
#define DRV_PWRCTRL_STANDBYSTATEACPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateAcpu(ulStandbyTime, ulSwitchTime)
#define DRV_PWRCTRL_STANDBYSTATECCPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateCcpu(ulStandbyTime, ulSwitchTime)



/*************************DPM END*************************************/

/*************************ABB START***********************************/

/*****************************************************************************
 �� �� ��  : BSP_ABB_RegGet
 ��������  : ��ȡABB�Ĵ���ֵ
 �������  : usInstruction: �����Ĵ�����ַ
             uspData: ��żĴ���ֵ
 �������  :
 �� �� ֵ  :
*****************************************************************************/
extern int BSP_ABB_RegGet(unsigned char usInstruction, unsigned char *uspData);
#define DRV_ABB_REG_VALUE_GET(usInstruction,uspData)    BSP_ABB_RegGet (usInstruction,uspData)

/*************************ABB END*************************************/

/*************************SYSCTRL START*******************************/
/*****************************************************************************
 �� �� ��  : BSP_GUSYS_GuDspCountInfo
 ��������  : ���ӿ�ʵ��ZSPģ��Ľ⸴λ����������zsp BRG������
 �������  : *GuDspAddr ���ڱ��������Ϣ
 �������  : �ޡ�
 �� �� ֵ  : BSP_ERROR:��ַ�Ƿ�
                       BSP_OK:��ַ�Ϸ�
         �쾭��x00221564  2012.11.20
*****************************************************************************/
extern unsigned int BSP_GUSYS_GuDspCountInfo(unsigned int *GuDspAddr);
#define DRV_DSP_RESET_GET_INFO(GuDspAddr)    BSP_GUSYS_GuDspCountInfo (GuDspAddr)

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_DspReset
 ��������  : ���ӿ�ʵ��ZSPģ��ĸ�λ����������zsp core�����ߺ����衣
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_DspReset(void);
#define DRV_DSP_ZONE_RESET()    BSP_GUSYS_DspReset()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_DspResetCancel
 ��������  : ���ӿ�ʵ��ZSPģ��Ľ⸴λ����������zsp core�����ߺ����衣
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_DspResetCancel(void);
#define DRV_DSP_ZONE_RESET_CANCEL()    BSP_GUSYS_DspResetCancel()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_ABBGSMRxCtrl
 ��������  : ���ӿ�ʵ��ABB GSM���տ��ơ�
 �������  : ucStatus��0���رգ�1���򿪡�
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern unsigned int BSP_GUSYS_ABBGSMRxCtrl(unsigned char ucStatus);
#define DRV_ABB_GRX_CTRL(ucStatus)    BSP_GUSYS_ABBGSMRxCtrl(ucStatus)

/*****************************************************************************
 �� �� ��  : ABBWCDMARxCtrl
 ��������  : ���ӿ�ʵ��ABB WCDMA���տ��ơ�
 �������  : ucStatus��0���رգ�1���򿪡�
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern unsigned int BSP_GUSYS_ABBWCDMARxCtrl(unsigned char ucStatus);
#define DRV_ABB_WRX_CTRL(ucStatus)    BSP_GUSYS_ABBWCDMARxCtrl(ucStatus)

/*****************************************************************************
 �� �� ��  : ABBTxCtrl
 ��������  : ���ӿ�ʵ��ABB ���Ϳ��ơ�
 �������  : ucStatus��0���رգ�1���򿪡�
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע������  ��ABB���Ϳ��Ʋ�����W/Gģʽ��
*****************************************************************************/
extern int BSP_GUSYS_ABBTxCtrl(unsigned char ucStatus);
#define DRV_ABB_TX_CTRL(ucStatus)    BSP_GUSYS_ABBTxCtrl(ucStatus)

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_WcdmaBbpPllEnable
 ��������  : ���ӿ�ʵ��ʹ��WCDMA BBP PLL��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_WcdmaBbpPllEnable(void);
#define DRV_WBBP_PLL_ENABLE()    BSP_GUSYS_WcdmaBbpPllEnable()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_WcdmaBbpPllDisable
 ��������  : ���ӿ�ʵ�ֹر�WCDMA BBP PLL��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_WcdmaBbpPllDisable(void);
#define DRV_WBBP_PLL_DSABLE()    BSP_GUSYS_WcdmaBbpPllDisable()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_GsmBbpPllEnable
 ��������  : ���ӿ�ʵ��ʹ��GSM BBP PLL��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_GsmBbpPllEnable(void);
#define DRV_GBBP_PLL_ENABLE()    BSP_GUSYS_GsmBbpPllEnable()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_GsmBbpPllDisable
 ��������  : ���ӿ�ʵ�ֹر�GSM BBP PLL��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_GsmBbpPllDisable(void);
#define DRV_GBBP_PLL_DISABLE()    BSP_GUSYS_GsmBbpPllDisable()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_RFLdoOn
 ��������  : ���ӿ�ʵ��WCDMA��GSM RF LDO�ϵ硣
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_RFLdoOn(void);
#define DRV_RF_LDOUP()    BSP_GUSYS_RFLdoOn()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_RFLdoDown
 ��������  : ���ӿ�ʵ��WCDMA��GSM RF LDO�µ硣
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_RFLdoDown(void);
#define DRV_RF_LDODOWN()    BSP_GUSYS_RFLdoDown()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_WcdmaPllStatusGet
 ��������  : ��ȡWCDMA BBP PLL�ȶ�״̬��
 �������  : �ޡ�
 �������  :�ޡ�
 �� �� ֵ  :
                     0���ȶ�
                     1��δ�ȶ�
*****************************************************************************/
extern unsigned long BSP_GUSYS_WcdmaPllStatusGet(void);
#define DRV_GET_WCDMA_PLL_STATUS()    BSP_GUSYS_WcdmaPllStatusGet()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_GsmPllStatusGet
 ��������  : ��ȡGSM BBP PLL�ȶ�״̬��
 �������  : �ޡ�
 �������  :�ޡ�
 �� �� ֵ  :
                     0���ȶ�
                     1��δ�ȶ�
*****************************************************************************/
extern unsigned long BSP_GUSYS_GsmPllStatusGet(void);
#define DRV_GET_GSM_PLL_STATUS()    BSP_GUSYS_GsmPllStatusGet()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_DspPllStatusGet
 ��������  : ��ȡDSP PLL�ȶ�״̬��
 �������  : �ޡ�
 �������  :�ޡ�
 �� �� ֵ  :
                     0���ȶ�
                     1��δ�ȶ�
*****************************************************************************/
extern unsigned long BSP_GUSYS_DspPllStatusGet(void);
#define DRV_GET_DSP_PLL_STATUS()    BSP_GUSYS_DspPllStatusGet()


/*****************************************************************************
 �� �� ��  : BSP_GUSYS_ArmPllStatusGet
 ��������  : ��ȡARM PLL�ȶ�״̬��
 �������  : �ޡ�
 �������  :�ޡ�
 �� �� ֵ  :
                     0���ȶ�
                     1��δ�ȶ�
*****************************************************************************/
extern unsigned int BSP_GUSYS_ArmPllStatusGet(void);
#define DRV_GET_ARM_PLL_STATUS()    BSP_GUSYS_ArmPllStatusGet()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_BBPAutoAdjust
 ��������  : ��ʼ��ABB����У׼ʹ��
 �������  : uiSysMode��0��WCDMAģʽ��1��GSMģʽ��
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_GUSYS_BBPAutoAdjust(unsigned char ucAdjustMode);
#define DRV_BBP_AUTO_ADJUST(ucAdjustMode)    BSP_GUSYS_BBPAutoAdjust(ucAdjustMode)

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_DspPllEnable
 ��������  : ��DSP PLL
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_DspPllEnable(void);
#define DRV_DSP_PLL_ENABLE()    BSP_GUSYS_DspPllEnable()

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_DspPllDisable
 ��������  : �ر�DSP PLL
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUSYS_DspPllDisable(void);
#define DRV_DSP_PLL_DISABLE()    BSP_GUSYS_DspPllDisable()


#define WCDMA_MODE      0
#define GSM_MODE        1
#define LTE_MODE        2
#define TDS_MODE        3

/*****************************************************************************
 �� �� ��  : BSP_GUSYS_ModeSwitchs
 ��������  : ���ӿ�ʵ��WCDMA/GSMϵͳģʽ�л���
 �������  : uiSysMode��0���л���WCDMAģʽ��1���л���GSMģʽ��
 �������  : �ޡ�
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern int BSP_GUSYS_ModeSwitch(unsigned int uiSysMode);
#define DRV_SYSMODE_SWITCH(uiSysMode)    BSP_GUSYS_ModeSwitch(uiSysMode)

/*************************SYSCTRL START*******************************/

/*************************GUDSP���� START*****************************/

/*****************************************************************************
 �� �� ��  : BSP_GUDSP_LayerInfoGet
 ��������  : ��ȡ����������Ϣ��
 �������  : �ޡ�
 �������  : pulAddr���������������Ϣ�Ļ��档
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern void BSP_GUDSP_LayerInfoGet(unsigned long* pulAddr, unsigned long* pulLength);
#define DRV_PHY_SLOAD_INFO_GET(pulAddr,pulLength)    BSP_GUDSP_LayerInfoGet(pulAddr,pulLength)

/*****************************************************************************
 �� �� ��  : BSP_GUDSP_CommonInfoGet
 ��������  : ��ȡ�����BSS COMMON����Ϣ��
 �������  : �ޡ�
 �������  : pulAddr���������������Ϣ�Ļ��档
 �� �� ֵ  : 0:�ɹ���-1:ʧ�ܡ�
*****************************************************************************/
extern int BSP_GUDSP_CommonInfoGet(unsigned long* pulAddr, unsigned long* pulLength);
#define DRV_DSP_COMMON_INFO_GET(pulAddr,pulLength)    BSP_GUDSP_CommonInfoGet(pulAddr,pulLength)

/*****************************************************************************
 �� �� ��  : BSP_GUDSP_Load
 ��������  : ���GU ZSP���ع��ܣ���ZSP��̬�δ�DDR���ص�ZSP TCM��
 �������  : �ޡ�
 �������  : �ޡ�
 �� �� ֵ  : 0:�ɹ���-1:ʧ�ܡ�
*****************************************************************************/
extern int BSP_GUDSP_Load(void);
#define DRV_GUDSP_LOAD()    BSP_GUDSP_Load()

/*************************GUDSP���� END*******************************/

/*************************EDMA START**********************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
typedef void (*channel_isr)(BSP_U32 channel_arg, BSP_U32 int_status);

/*�������������*/
typedef enum _BALONG_DMA_REQ
{
    EDMA_DWSSI0_RX = 0,
    EDMA_DWSSI0_TX,
    EDMA_DWSSI1_RX,
    EDMA_DWSSI1_TX,
    EDMA_HIFISIO_RX,
    EDMA_HIFISIO_TX,
    EDMA_HSSPI_RX,
    EDMA_HSSPI_TX,
    EDMA_HSUART_RX,
    EDMA_HSUART_TX,
    EDMA_UART0_RX,
    EDMA_UART0_TX,
    EDMA_UART1_RX,
    EDMA_UART1_TX,
    EDMA_UART2_RX,
    EDMA_UART2_TX,
    EDMA_SCI_RX,
    EDMA_SCI_TX,
    EDMA_UART3_RX,
    EDMA_UART3_TX,
    EDMA_MMC0,     /*20*/
    EDMA_MMC1,
    EDMA_MMC2,
    EDMA_MEMORY,    /*memory to memory ,��������������*/
    EDMA_REQ_MAX    /*����豸����С�ڴ�ֵ����Ϊ�Ƿ�����*/
} BALONG_DMA_REQ;

/* ����void (*channel_isr)(BSP_U32 channel_arg, BSP_U32 int_status)�Ĳ���int_status��
    ����int balong_dma_channel_init (BALONG_DMA_REQ req,  channel_isr pFunc,
                     UINT32 channel_arg, UINT32 int_flag)�Ĳ���int_flag
    Ϊ���¼����ж����ͣ������       */
#define BALONG_DMA_INT_DONE           1          /*DMA��������ж�*/
#define BALONG_DMA_INT_LLT_DONE       2          /*��ʽDMA�ڵ㴫������ж�*/
#define BALONG_DMA_INT_CONFIG_ERR     4          /*DMA���ô����µ��ж�*/
#define BALONG_DMA_INT_TRANSFER_ERR   8          /*DMA��������µ��ж�*/
#define BALONG_DMA_INT_READ_ERR       16         /*DMA����������µ��ж�*/

/* EDMAC���䷽����*/
#define BALONG_DMA_P2M      1
#define BALONG_DMA_M2P      2
#define BALONG_DMA_M2M      3

/* EDMAC�������봫������*/
typedef enum tagEDMA_TRANS_TYPE
{
    MEM_MEM_DMA = 0x00,    /* �ڴ浽�ڴ棬DMA����*/
    MEM_PRF_DMA = 0x01,        /* �ڴ������裬DMA����*/
    MEM_PRF_PRF = 0x10        /* �ڴ������裬��������*/
} EDMA_TRANS_TYPE;

/* ͨ��״̬ */

#define   EDMA_CHN_FREE          1   /* ͨ������ */
#define   EDMA_CHN_BUSY          0   /* ͨ��æ */


/* EDMA����λ��Դ��Ŀ�ĵ�ַԼ��Ϊһ�µ�ֵ */
#define   EDMA_TRANS_WIDTH_8       0x0   /* 8bitλ��*/
#define   EDMA_TRANS_WIDTH_16      0x1   /* 16bitλ��*/
#define   EDMA_TRANS_WIDTH_32      0x2   /* 32bitλ��*/
#define   EDMA_TRANS_WIDTH_64      0x3   /* 64bitλ��*/

/*  EDMA burst length, ȡֵ��Χ0~15����ʾ��burst����Ϊ1~16*/
#define   EDMA_BUR_LEN_1    0x0    /* burst���ȣ���һ�δ���ĸ���Ϊ1��*/
#define   EDMA_BUR_LEN_2    0x1    /* burst���ȣ���һ�δ���ĸ���Ϊ2��*/
#define   EDMA_BUR_LEN_3    0x2   /* burst���ȣ���һ�δ���ĸ���Ϊ3��*/
#define   EDMA_BUR_LEN_4    0x3   /* burst���ȣ���һ�δ���ĸ���Ϊ4��*/
#define   EDMA_BUR_LEN_5    0x4   /* burst���ȣ���һ�δ���ĸ���Ϊ5��*/
#define   EDMA_BUR_LEN_6    0x5   /* burst���ȣ���һ�δ���ĸ���Ϊ6��*/
#define   EDMA_BUR_LEN_7    0x6   /* burst���ȣ���һ�δ���ĸ���Ϊ7��*/
#define   EDMA_BUR_LEN_8    0x7   /* burst���ȣ���һ�δ���ĸ���Ϊ8��*/
#define   EDMA_BUR_LEN_9    0x8   /* burst���ȣ���һ�δ���ĸ���Ϊ9��*/
#define   EDMA_BUR_LEN_10   0x9   /* burst���ȣ���һ�δ���ĸ���Ϊ10��*/
#define   EDMA_BUR_LEN_11   0xa   /* burst���ȣ���һ�δ���ĸ���Ϊ11��*/
#define   EDMA_BUR_LEN_12   0xb   /* burst���ȣ���һ�δ���ĸ���Ϊ12��*/
#define   EDMA_BUR_LEN_13   0xc   /* burst���ȣ���һ�δ���ĸ���Ϊ13��*/
#define   EDMA_BUR_LEN_14   0xd   /* burst���ȣ���һ�δ���ĸ���Ϊ14��*/
#define   EDMA_BUR_LEN_15   0xe   /* burst���ȣ���һ�δ���ĸ���Ϊ15��*/
#define   EDMA_BUR_LEN_16   0xf   /* burst���ȣ���һ�δ���ĸ���Ϊ16��*/


/* EDMA ��Ӧ�ľ���λ����EDMA  �Ĵ������ú�
       EDMAC_BASIC_CONFIG��BALONG_DMA_SET_LLI��BALONG_DMA_SET_CONFIG ʹ��*/
#define EDMAC_TRANSFER_CONFIG_SOUR_INC      (0X80000000)
#define EDMAC_TRANSFER_CONFIG_DEST_INC      (0X40000000)

#define EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH( _len )  ((BSP_U32)((_len)<<24))
#define EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH( _len )  ((BSP_U32)((_len)<<20))

#define EDMAC_TRANSFER_CONFIG_SOUR_WIDTH( _len )  ((BSP_U32)((_len)<<16))
#define EDMAC_TRANSFER_CONFIG_DEST_WIDTH( _len )  ((BSP_U32)((_len)<<12))

#define EDMAC_TRANSFER_CONFIG_REQUEST( _ulReg )    ( (_ulReg ) << 4)
#define EDMAC_TRANSFER_CONFIG_FLOW_DMAC( _len )    ((BSP_U32)((_len)<<2))

#define EDMAC_TRANSFER_CONFIG_INT_TC_ENABLE            ( 0x2 )
#define EDMAC_TRANSFER_CONFIG_INT_TC_DISABLE           ( 0x0 )

#define EDMAC_TRANSFER_CONFIG_CHANNEL_ENABLE           ( 0x1 )
#define EDMAC_TRANSFER_CONFIG_CHANNEL_DISABLE          ( 0x0 )

#define EDMAC_TRANSFER_CONFIG_EXIT_ADD_MODE_A_SYNC     ( 0UL )
#define EDMAC_TRANSFER_CONFIG_EXIT_ADD_MODE_AB_SYNC    ( 0x00008000 )

#define EDMAC_NEXT_LLI_ENABLE       0x2           /* Bit 1 */

/*��ʽ����ʱ�Ľڵ���Ϣ*/
typedef struct _BALONG_DMA_CB
{
    volatile BSP_U32 lli;     /*ָ���¸�LLI*/
    volatile BSP_U32 bindx;
    volatile BSP_U32 cindx;
    volatile BSP_U32 cnt1;
    volatile BSP_U32 cnt0;   /*�鴫�����LLI�����ÿ���ڵ����ݳ��� <= 65535�ֽ�*/
    volatile BSP_U32 src_addr; /*�����ַ*/
    volatile BSP_U32 des_addr; /*�����ַ*/
    volatile BSP_U32 config;
} BALONG_DMA_CB;

#define P2M_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_PRF_DMA) | EDMAC_TRANSFER_CONFIG_DEST_INC)
#define M2P_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_PRF_DMA) | EDMAC_TRANSFER_CONFIG_SOUR_INC)
#define M2M_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_MEM_DMA) | EDMAC_TRANSFER_CONFIG_SOUR_INC | EDMAC_TRANSFER_CONFIG_DEST_INC)

#define EDMAC_BASIC_CONFIG(burst_width, burst_len) \
               ( EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH(burst_len) | EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH(burst_len) \
               | EDMAC_TRANSFER_CONFIG_SOUR_WIDTH(burst_width) | EDMAC_TRANSFER_CONFIG_DEST_WIDTH(burst_width) )

/*addr:�����ַ*/
#define BALONG_DMA_SET_LLI(addr, last)   ((last)?0:(EDMAC_MAKE_LLI_ADDR(addr) | EDMAC_NEXT_LLI_ENABLE))

#define BALONG_DMA_SET_CONFIG(req, direction, burst_width, burst_len) \
                 ( EDMAC_BASIC_CONFIG(burst_width, burst_len) | EDMAC_TRANSFER_CONFIG_REQUEST(req) \
                 | EDMAC_TRANSFER_CONFIG_INT_TC_ENABLE | EDMAC_TRANSFER_CONFIG_CHANNEL_ENABLE \
                 | ((direction == BALONG_DMA_M2M)?M2M_CONFIG:((direction == BALONG_DMA_P2M)?P2M_CONFIG:M2P_CONFIG)))

/**************************************************************************
  �궨��
**************************************************************************/

/*�����붨��*/
#define DMAC_SUCCESS                       BSP_OK
#define DMA_FAIL                          BSP_ERROR

#define DMA_ERROR_BASE                          -100
#define DMA_CHANNEL_INVALID                     (DMA_ERROR_BASE-1)
#define DMA_TRXFERSIZE_INVALID                  (DMA_ERROR_BASE-2)
#define DMA_SOURCE_ADDRESS_INVALID              (DMA_ERROR_BASE-3)
#define DMA_DESTINATION_ADDRESS_INVALID         (DMA_ERROR_BASE-4)
#define DMA_MEMORY_ADDRESS_INVALID              (DMA_ERROR_BASE-5)
#define DMA_PERIPHERAL_ID_INVALID               (DMA_ERROR_BASE-6)
#define DMA_DIRECTION_ERROR                     (DMA_ERROR_BASE-7)
#define DMA_TRXFER_ERROR                        (DMA_ERROR_BASE-8)
#define DMA_LLIHEAD_ERROR                       (DMA_ERROR_BASE-9)
#define DMA_SWIDTH_ERROR                        (DMA_ERROR_BASE-0xa)
#define DMA_LLI_ADDRESS_INVALID                 (DMA_ERROR_BASE-0xb)
#define DMA_TRANS_CONTROL_INVALID               (DMA_ERROR_BASE-0xc)
#define DMA_MEMORY_ALLOCATE_ERROR               (DMA_ERROR_BASE-0xd)
#define DMA_NOT_FINISHED                        (DMA_ERROR_BASE-0xe)
#define DMA_CONFIG_ERROR                        (DMA_ERROR_BASE-0xf)


/*******************************************************************************
  ������:      BSP_S32 balong_dma_init(void)
  ��������:    DMA��ʼ�����򣬹ҽ��ж�
  �������:    ��
  �������:    ��
  ����ֵ:      0
*******************************************************************************/
extern BSP_S32 balong_dma_init(void);

/*******************************************************************************
  ������:       int balong_dma_current_transfer_address(UINT32 channel_id)
  ��������:     ���ĳͨ����ǰ������ڴ��ַ
  �������:     channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
  �������:     ��
  ����ֵ:       �ɹ���ͨ����ǰ������ڴ��ַ
                ʧ�ܣ�����
*******************************************************************************/
extern int balong_dma_current_transfer_address(BSP_U32 channel_id);
#define DRV_EDMA_CURR_TRANS_ADDR(channel_id)  balong_dma_current_transfer_address(channel_id)

/*******************************************************************************
  ������:       int balong_dma_channel_stop(UINT32 channel_id)
  ��������:     ָֹͣ����DMAͨ��
  �������:     channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
  �������:     ��
  ����ֵ:       �ɹ���ͨ����ǰ������ڴ��ַ
                ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_stop(BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_STOP(channel_id)  balong_dma_channel_stop(channel_id)


/*******************************************************************************
  ������:      BALONG_DMA_CB *balong_dma_channel_get_lli_addr(UINT32 channel_id)
  ��������:    ��ȡָ��DMAͨ����������ƿ����ʼ��ַ
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
extern BALONG_DMA_CB *balong_dma_channel_get_lli_addr (BSP_U32 channel_id);
#define DRV_EDMA_CHAN_GET_LLI_ADDR(channel_id)  balong_dma_channel_get_lli_addr(channel_id)


/******************************************************************************
  ������:      int balong_dma_channel_init (BALONG_DMA_REQ req,
                  channel_isr pFunc, UINT32 channel_arg, UINT32 int_flag)
  ��������:    ��������ŷ���ͨ����ע��ͨ���жϻص���������ʼ����������ź�����
               �������д��config�Ĵ���
  �������:    req : ���������
               pFunc : �ϲ�ģ��ע���DMAͨ���жϴ�������NULLʱ������ע��
               channel_arg : pFunc�����1��
                             pFuncΪNULL������Ҫ�����������
               int_flag : pFunc�����2, �������ж����ͣ�ȡֵ��ΧΪ
                        BALONG_DMA_INT_DONE��BALONG_DMA_INT_LLT_DONE��
                      �BALONG_DMA_INT_CONFIG_ERR��BALONG_DMA_INT_TRANSFER_ERR��
                        BALONG_DMA_INT_READ_ERR֮һ��������ϡ�
                        pFuncΪNULL������Ҫ�����������
  �������:    ��
  ����ֵ:      �ɹ���ͨ����
               ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_init (BALONG_DMA_REQ req, channel_isr pFunc, BSP_U32 channel_arg, BSP_U32 int_flag);
#define DRV_EDMA_CHANNEL_INIT(req, pFunc,channel_arg,int_flag) balong_dma_channel_init(req, pFunc,channel_arg,int_flag)

/*******************************************************************************
  ������:      int balong_dma_channel_set_config (UINT32 channel_id,
                       UINT32 direction, UINT32 burst_width, UINT32 burst_len)
  ��������:    ����ʽDMA����ʱ�����ñ���������ͨ������
               ��ʽDMA����ʱ������Ҫʹ�ñ�������
  �������:    channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
               direction : DMA���䷽��, ȡֵΪBALONG_DMA_P2M��BALONG_DMA_M2P��
                           BALONG_DMA_M2M֮һ
               burst_width��ȡֵΪ0��1��2��3����ʾ��burstλ��Ϊ8��16��32��64bit
               burst_len��ȡֵ��Χ0~15����ʾ��burst����Ϊ1~16
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_set_config (BSP_U32 channel_id, BSP_U32 direction,BSP_U32 burst_width, BSP_U32 burst_len);
#define DRV_EDMA_CHANNEL_CONFIG(channel_id, direction,burst_width,burst_len)  balong_dma_channel_set_config(channel_id, direction,burst_width,burst_len)

/*******************************************************************************
  ������:      int balong_dma_channel_start (UINT32 channel_id, UINT32 src_addr,
                       UINT32 des_addr, UINT32 len)
  ��������:    ����һ��ͬ��DMA����, DMA������ɺ󣬲ŷ���
               ʹ�ñ�����ʱ������Ҫע���жϴ�����
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
               src_addr�����ݴ���Դ��ַ�������������ַ
               des_addr�����ݴ���Ŀ�ĵ�ַ�������������ַ
               len�����ݴ��䳤�ȣ���λ���ֽڣ�һ�δ������ݵ���󳤶���65535�ֽ�
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_start (BSP_U32 channel_id, BSP_U32 src_addr, BSP_U32 des_addr, BSP_U32 len);
#define DRV_EDMA_CHANNEL_START(channel_id,src_addr,des_addr,len)  balong_dma_channel_start(channel_id,src_addr,des_addr,len)


/*******************************************************************************
  ������:      int balong_dma_channel_async_start (UINT32 channel_id,
                unsigned int src_addr, unsigned int des_addr, unsigned int len)
  ��������:    ����һ���첽DMA���䡣����DMA����󣬾ͷ��ء����ȴ�DMA������ɡ�
               ʹ�ñ�����ʱ��ע���жϴ��������жϴ������д���DMA
��������¼�
               ���ߣ���ע���жϴ�������ʹ��balong_dma_channel_is_idle������ѯ
               DMA�����Ƿ����
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
               src_addr�����ݴ���Դ��ַ�������������ַ
               des_addr�����ݴ���Ŀ�ĵ�ַ�������������ַ
               len�����ݴ��䳤�ȣ���λ���ֽڣ�һ�δ������ݵ���󳤶���65535�ֽ�
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_async_start (BSP_U32 channel_id, BSP_U32 src_addr, BSP_U32 des_addr, BSP_U32 len);
#define DRV_EDMA_CHANNEL_ASYNC_START(channel_id,src_addr,des_addr,len) balong_dma_channel_async_start(channel_id,src_addr,des_addr,len)


/*******************************************************************************
  ������:      int balong_dma_channel_lli_start (UINT32 channel_id)
  ��������:    ������ʽDMA���䡣����ʽDMA�����нڵ㴫�䶼ȫ����ɺ�ŷ��ء�
               ��ʽDMA��ÿ���ڵ����������䳤��Ϊ65535�ֽڡ�
               ע�⣺���ô˺���ǰ���������ú�������ƿ顣
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_lli_start (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_lli_START(channel_id)   balong_dma_channel_lli_start(channel_id)

/*******************************************************************************
  ������:      int balong_dma_channel_lli_start (UINT32 channel_id)
  ��������:    ������ʽDMA���䣬Ȼ���������أ����ȴ�DMA������ɡ�
               ��ʽDMA��ÿ���ڵ����������䳤��Ϊ65535�ֽڡ�
               ע�⣺���ô˺���ǰ���������ú�������ƿ顣
  �������:    channel_id��ͨ��ID,����balong_dma_channel_init�����ķ���ֵ
  �������:    ��
  ����ֵ:      �ɹ���0
               ʧ�ܣ�����
*******************************************************************************/
extern BSP_S32 balong_dma_channel_lli_async_start (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_lli_ASYNC_START(channel_id)   balong_dma_channel_lli_async_start(channel_id)


/******************************************************************************
*
  ������:       int balong_dma_channel_is_idle (UINT32 channel_id)
  ��������:     ��ѯDMAͨ���Ƿ����
  �������:     channel_id : ͨ��ID������balong_dma_channel_init�����ķ���ֵ
  �������:     ��
  ����ֵ:       0 : ͨ��æµ
                1 : ͨ������
                ���� : ʧ��
*******************************************************************************/
extern BSP_S32 balong_dma_channel_is_idle (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_IS_IDLE(chanel_id) balong_dma_channel_is_idle(chanel_id)
#endif

/*************************EDMA END************************************/

/*************************SEC START***********************************/

/*************************************************
 �� �� ��       : DRV_SECURE_SUPPORT
 ��������   : ��ǰ�汾�Ƿ�֧�ְ�ȫ����
 �������   : unsigned char *pData
 �������   : unsigned char *pData
 �� �� ֵ      : OK/ERROR
*************************************************/
extern BSP_S32 DRV_SECURE_SUPPORT(BSP_U8 *pu8Data);

/*************************************************
 �� �� ��       : DRV_SECURE_ALREADY_USE
 ��������   : ��ѯ��ǰ�汾�Ƿ��Ѿ����ð�ȫ����
 �������   : unsigned char *pData
 �������   : unsigned char *pData
 �� �� ֵ      : OK/ERROR
*************************************************/
extern BSP_S32 DRV_SECURE_ALREADY_USE(BSP_U8 *pu8Data);

/*************************************************
 �� �� ��       : DRV_START_SECURE
 ��������   : ���ð�ȫ����
 �������   :
 �������   :
 �� �� ֵ      : OK/ERROR
*************************************************/
extern BSP_S32 DRV_START_SECURE(void);


/*************************************************
 �� �� ��       : DRV_SEC_CHECK
 ��������   : ��ȫ�����ж�
 �������   :
 �������   :
 �� �� ֵ      : 0:���ǰ�ȫ����
                           1:�ǰ�ȫ����
                           ����:����
 ���ú���   :
 ��������   :

 �޸���ʷ   :
    ��    ��       : 2011��03��07��
   �޸����� : �����ɺ���

*************************************************/
extern BSP_U32 DRV_SEC_CHECK(void);

/*************************SEC END*************************************/

/*************************MD5��� START*******************************/

/*****************************************************************************
 �� �� ��  : VerifySIMLock
 ��������  : �жϵ�ǰ�������Ƿ���ȷ .
 �������  : imei       - ����IMEI��
             unlockcode - ������
 �������  : �ޡ�
 �� �� ֵ  : 1��   �����ɹ�
             0:    ����ʧ��
*****************************************************************************/
extern int VerifySIMLock(char* UnlockCode, char* Key);
#define DRV_CARDLOCK_MD5_VERIFY(unlockcode, imei)  VerifySIMLock(unlockcode, imei)

/*******************************************************
  �������ƣ�GetAuthVer
  ���ù�ϵ��Produce Tool
  ���룺
  ����ֵ��
    ������SIMLock managerģ��汾��
    ���ͣ�����ֵ
    ���Ϊ1����ʾ�������ڵİ汾���������ȡ����1.0�汾��
                    ���Ϊ2�����ʾ2.0�汾����ȡ�����Ż�֮��İ汾��ȡ���롣
                    ����Ϊ����ֵ��
 �޸���ʷ      :
  1.��    ��   : 2011��4��23��
    �޸�����   : SIMLock����

********************************************************/
extern int GetAuthVer(void);
#define DRV_GET_AUTH_VER()  GetAuthVer()

/*************************MD5��� END*********************************/

/*************************SYNC START**********************************/

/**************************************************************************
  �����붨��
**************************************************************************/
#define BSP_ERR_SYNC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_SYNC, 0)))
#define BSP_ERR_SYNC_TIMEOUT             (BSP_ERR_SYNC_BASE + 0x0)

/**************************************************************************
  �ṹ����
**************************************************************************/
typedef enum tagSYNC_MODULE_E
{
    SYNC_MODULE_MEM     = 0,
    SYNC_MODULE_NANDC,
    SYNC_MODULE_PMU,
    SYNC_MODULE_SOCP,
    SYNC_MODULE_DMA,
    SYNC_MODULE_IPF,
    SYNC_MODULE_NV,
    SYNC_MODULE_YAFFS,
    SYNC_MODULE_MSP,
    SYNC_MODULE_GPIO,
    SYNC_MODULE_CSHELL,
    SYNC_MODULE_MNTN,
	SYNC_MODULE_MSPNV,
	SYNC_MODULE_ONLINE,
	SYNC_MODULE_CHG,
	SYNC_MODULE_TFUPDATE,
    SYNC_MODULE_BUTT
}SYNC_MODULE_E;

/**************************************************************************
  ��������
**************************************************************************/
BSP_S32 BSP_SYNC_Lock(SYNC_MODULE_E u32Module, BSP_U32 *pState, BSP_U32 u32TimeOut);
BSP_S32 BSP_SYNC_UnLock(SYNC_MODULE_E u32Module, BSP_U32 u32State);
BSP_S32 BSP_SYNC_Wait(SYNC_MODULE_E u32Module, BSP_U32 u32TimeOut);
BSP_S32 BSP_SYNC_Give(SYNC_MODULE_E u32Module);

/*************************SYNC END************************************/

/*************************CHGģ��START********************************/

/*****************************************************************************
 �� �� ��  : BSP_CHG_GetCbcState
 ��������  : ���ص��״̬�͵���
 �������  :pusBcs 0:����ڹ��� 1:�������ӵ��δ���� 2:û����������
                          pucBcl  0:���������û����������
 �������  : pusBcs 0:����ڹ��� 1:�������ӵ��δ���� 2:û����������
                          pucBcl  0:���������û����������
 ����ֵ��   0 �����ɹ�
                         -1����ʧ��

*****************************************************************************/
extern int BSP_CHG_GetCbcState(unsigned char *pusBcs,unsigned char *pucBcl);
extern int DRV_CHG_GET_CBC_STATE(unsigned char *pusBcs,unsigned char *pucBcl);

typedef enum CHARGING_STATE_ENUM_tag
{
        CHARGING_INIT = -1,
        NO_CHARGING_UP = 0,  /*����δ���*/
        CHARGING_UP ,              /*���������*/
        NO_CHARGING_DOWN ,  /*�ػ�δ���*/
        CHARGING_DOWN         /*�ػ�δ���*/
}CHARGING_STATE_ENUM;

typedef enum BATT_LEVEL_ENUM_tag
{
        BATT_INIT = -2,
        BATT_LOW_POWER =-1,    /*��ص͵�*/
        BATT_LEVEL_0,                 /*0���ص���*/
        BATT_LEVEL_1,                  /*1���ص���*/
        BATT_LEVEL_2,                   /*2���ص���*/
        BATT_LEVEL_3,                    /*3���ص���*/
        BATT_LEVEL_4,                    /*4���ص���*/
        BATT_LEVEL_MAX
}BATT_LEVEL_ENUM;

typedef struct BATT_STATE_tag
{
    CHARGING_STATE_ENUM  charging_state;
    BATT_LEVEL_ENUM      battery_level;
    #ifdef FEATURE_HUAWEI_MBB_CHG
    GPIO_TEMP_EVENT      batt_temp_state;
	#endif
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
    int                       batt_percent;
#endif
}BATT_STATE_T;

/*AT ����ö������*/
typedef enum
{
    CHG_AT_BATTERY_CHECK,        /* BatteryУ׼*/
    CHG_AT_BATTERY_LEVEL,        /* Battery���� */
    CHG_AT_BATTERY_ADC,          /* Battery Temperature����*/
    CHG_AT_BATTERY_INVALID
} ENUM_CHG_ATCM_TYPE;

typedef enum ENUM_POWER_ON_MODE_tag
{
    POWER_ON_INVALID = 0,          	/* ��Ч����ģʽ*/
    POWER_ON_NORMAL,       	 	/* ��������ģʽ*/
    POWER_ON_CHARGING,       	/* �ػ����ģʽ*/
    POWER_ON_MAX
} ENUM_POWER_ON_MODE;
/*****************************************************************************
 �� �� ��  : BSP_CHG_GetBatteryState
 ��������  :��ȡ�ײ���״̬��Ϣ
 �������  :battery_state ������Ϣ
 �������  :battery_state ������Ϣ
 ����ֵ��   0 �����ɹ�
                         -1����ʧ��

*****************************************************************************/
extern int BSP_CHG_GetBatteryState(BATT_STATE_T *battery_state);
extern int DRV_CHG_GET_BATTERY_STATE(BATT_STATE_T *battery_state);

/*****************************************************************************
 �� �� ��  : BSP_CHG_ChargingStatus
 ��������  :��ѯĿǰ�Ƿ����ڳ����
 �������  :��
 �������  :��
 ����ֵ��   0 δ���
                          1 �����
*****************************************************************************/
extern int BSP_CHG_ChargingStatus(void);
extern int DRV_CHG_GET_CHARGING_STATUS(void);

/*****************************************************************************
 �� �� ��  : BSP_CHG_StateSet
 ��������  :ʹ�ܻ��߽�ֹ���
 �������  :ulState      0:��ֹ���
 						1:ʹ�ܳ��
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
extern void DRV_CHG_STATE_SET(unsigned long ulState);

/*****************************************************************************
 �� �� ��  : BSP_CHG_StateGet
 ��������  :��ѯ
 �������  :
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
extern BSP_S32 BSP_CHG_StateGet(void);
#define DRV_CHG_STATE_GET()    BSP_CHG_StateGet()

/*****************************************************************************
 �� �� ��  : BSP_CHG_Sply
 ��������  :��ѯ
 �������  :
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
extern BSP_S32 BSP_CHG_Sply(void);
#define DRV_CHG_BATT_SPLY()    BSP_CHG_Sply()

#ifdef FEATURE_HUAWEI_MBB_CHG
/* 	AT���ӿ� for chg normalizaton*/
extern int BSP_TBAT_Read(ENUM_CHG_ATCM_TYPE atID, void *pItem);
extern int BSP_TBAT_Write(ENUM_CHG_ATCM_TYPE atID, void *pItem);
#else
/* 	AT���ӿ�*/
extern BSP_S32 BSP_TBAT_Read(ENUM_CHG_ATCM_TYPE atID, void *pItem);
extern BSP_S32 BSP_TBAT_Write(ENUM_CHG_ATCM_TYPE atID, void *pItem);
#endif /* FEATURE_HUAWEI_MBB_CHG */

/**********************************************************************
�� �� ��      : BSP_TBAT_CHRStGet
��������  :  TBAT AT^TCHRENABLE?�Ƿ���Ҫ����

�������  : ��
�������  : ��
�� �� ֵ      : 1:��Ҫ����
			      0:����Ҫ����
ע������  : ��
***********************************************************************/
extern BSP_S32 BSP_TBAT_CHRStGet(void);

/**********************************************************************
�� �� ��      : BSP_TBAT_CHRSuply
��������  :  TBAT AT^TCHRENABLE=4����ʵ��
            			��Ҫ�ṩ����ɹ�LCD��ʾͼ��
�������  : ��
�������  : ��
�� �� ֵ      :
ע������  : ��
***********************************************************************/
extern BSP_S32 BSP_TBAT_CHRSply(void);

/**********************************************************************
�� �� ��      : BSP_ONOFF_StartupModeGet
��������  :  A���жϿ��ػ�ģʽ
            			
�������  : ��
�������  : ��
�� �� ֵ      :
ע������  : ��
***********************************************************************/
ENUM_POWER_ON_MODE BSP_ONOFF_StartupModeGet( void );
/*****************************************************************************
* �� �� ��  : BSP_PMU_BattCali
*
* ��������  : ���У׼ǰ��С��������ӿڣ���AT���� 
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
extern void BSP_PMU_BattCali(void);


/*************************CHGģ�� END*********************************/

/*************************ICC BEGIN***********************************/
/**************************************************************************
  �궨��
**************************************************************************/
#define ICC_CHAN_NUM_MAX            (32)

#define ICC_CHAN_PRIORITY_HIGH      (0)
#define ICC_CHAN_PRIORITY_LOW       (255)
#define ICC_CHAN_PRIORITY_DEFAULT   ICC_CHAN_PRIORITY_LOW

#define ICC_CHAN_FIFO_MIN           (32)
#define ICC_CHAN_FIFO_MAX           (65536)
#define ICC_CHAN_FIFO_SIZE1         (1024)
#define ICC_CHAN_FIFO_SIZE2         (2048)
#define ICC_CHAN_FIFO_SIZE3         (4096)

/**************************************************************************
  �ṹ����
**************************************************************************/
typedef enum tagICC_CHAN_STATE_E
{
    ICC_CHAN_STATE_CLOSED   = 0,
    ICC_CHAN_STATE_OPENED,
    ICC_CHAN_STATE_CLOSEING,
    ICC_CHAN_STATE_BUTT
}ICC_CHAN_STATE_E;

typedef enum tagICC_CHAN_MODE_E{
    ICC_CHAN_MODE_STREAM    = 0,
    ICC_CHAN_MODE_PACKET,
    ICC_CHAN_MODE_BUTT
}ICC_CHAN_MODE_E;

typedef enum tagICC_IOCTL_CMD_E{
    ICC_IOCTL_SET_WRITE_CB  = 0,
    ICC_IOCTL_SET_READ_CB,
    ICC_IOCTL_SET_EVENT_CB,
    ICC_IOCTL_GET_STATE,
    ICC_IOCTL_SET_BUTT
}ICC_IOCTL_CMD_E;


typedef enum tagICC_EVENT_E{
    ICC_EVENT_CLOSE         = 0,
    ICC_EVENT_RESET,
    ICC_EVENT_BUTT
}ICC_EVENT_E;


typedef BSP_U32 (*icc_event_cb)(BSP_U32 u32ChanID, BSP_U32 u32Event, BSP_VOID* Param);
typedef BSP_U32 (*icc_write_cb)(BSP_U32 u32ChanID);
typedef BSP_U32 (*icc_read_cb)(BSP_U32 u32ChanID, BSP_S32 u32Size);

typedef struct tagICC_CHAN_ATTR_S{
    BSP_U32             u32FIFOInSize;
    BSP_U32             u32FIFOOutSize;
    BSP_U32             u32Priority;
    ICC_CHAN_MODE_E     enChanMode;
    BSP_U32             u32TimeOut;
    icc_event_cb        event_cb;
    icc_write_cb        write_cb;
    icc_read_cb         read_cb;
}ICC_CHAN_ATTR_S;

/**************************************************************************
  ��������
**************************************************************************/
BSP_S32 BSP_ICC_Open(BSP_U32 u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);
BSP_S32 BSP_ICC_Close(BSP_U32 u32ChanId);
BSP_S32 BSP_ICC_Write(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Read(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Ioctl(BSP_U32 u32ChanId, BSP_U32 cmd, BSP_VOID *param);
BSP_U32 BSP_ICC_CanSleep(BSP_U32 u32Flag);

/**************************************************************************
  �����붨��
**************************************************************************/
#define BSP_ERR_ICC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_ICC, 0)))
#define BSP_ERR_ICC_NOT_INIT            (BSP_ERR_ICC_BASE + 0x1)
#define BSP_ERR_ICC_NOT_CONNECT         (BSP_ERR_ICC_BASE + 0x2)
#define BSP_ERR_ICC_NULL                (BSP_ERR_ICC_BASE + 0x3)
#define BSP_ERR_ICC_NO_MEM              (BSP_ERR_ICC_BASE + 0x4)
#define BSP_ERR_ICC_INVALID_CHAN        (BSP_ERR_ICC_BASE + 0x5)
#define BSP_ERR_ICC_INVALID_PARAM       (BSP_ERR_ICC_BASE + 0x6)
#define BSP_ERR_ICC_EXIST               (BSP_ERR_ICC_BASE + 0x7)
#define BSP_ERR_ICC_NOT_EXIST           (BSP_ERR_ICC_BASE + 0x8)
#define BSP_ERR_ICC_OPENED              (BSP_ERR_ICC_BASE + 0x9)
#define BSP_ERR_ICC_NOT_OPEN            (BSP_ERR_ICC_BASE + 0xa)
#define BSP_ERR_ICC_NO_SEM              (BSP_ERR_ICC_BASE + 0xb)
#define BSP_ERR_ICC_NEGOTIATE_FAIL      (BSP_ERR_ICC_BASE + 0xc)
#define BSP_ERR_ICC_BUFFER_FULL         (BSP_ERR_ICC_BASE + 0xd)
#define BSP_ERR_ICC_BUFFER_EMPTY        (BSP_ERR_ICC_BASE + 0xe)
#define BSP_ERR_ICC_INVALID_PACKET      (BSP_ERR_ICC_BASE + 0xf)
#define BSP_ERR_ICC_PACKET_SIZE         (BSP_ERR_ICC_BASE + 0x10)
#define BSP_ERR_ICC_USER_BUFFER_SIZE    (BSP_ERR_ICC_BASE + 0x11)

/*************************ICC END*************************************/



/*************************IFC START*************************************/
/* ģ��IDö��*/
typedef enum tagIFC_MODULE_E
{
    IFC_MODULE_BSP=  0,
    IFC_MODULE_LPS=  1,
    IFC_MODULE_GUPS= 2,
    IFC_MODULE_LMSP = 3,
    IFC_MODULE_GUOM = 4,
    IFC_MODULE_BUTT
} IFC_MODULE_E;


/* IFC�ص������ṹ*/
/* pMsgBody:�ú���ID��Ӧ�����Ĳ���,ǰ���ֽ��豣֤ΪmoduleId*/
/* u32Len:pMsgBody����*/
typedef BSP_S32 (*BSP_IFC_REG_FUNC)(BSP_VOID *pMsgBody,BSP_U32 u32Len);

/*****************************************************************************
* �� �� ��  : BSP_IFC_RegFunc
*
* ��������  : ע��IFC�ص������ӿ�
*
* �������  : IFC_MODULE_E enModuleId    ģ��ID
*             BSP_IFC_REG_FUNC pFunc     �ص�����ָ��
* �������  : ��
*
* �� �� ֵ  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_U32 BSP_IFC_RegFunc(IFC_MODULE_E enModuleId, BSP_IFC_REG_FUNC pFunc);


/*****************************************************************************
* �� �� ��  : BSP_IFC_Send
*
* ��������  : IFC������Ϣ�ӿ�
*
* �������  : BSP_VOID * pMspBody           ������Ϣ����
*             BSP_U32 u32Len               pMspBody�ĳ���
* �������  : ��
*
* �� �� ֵ  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_U32 BSP_IFC_Send(BSP_VOID * pMspBody, BSP_U32 u32Len);

/*************************IFC END*************************************/

/*************************Build�Ż� START*****************************/
/*************************�ڴ���� START******************************/

/* DDR�и����ڴ�ε����� */
typedef enum tagBSP_DDR_SECT_TYPE_E
{
    BSP_DDR_SECT_TYPE_TTF = 0x0,
    BSP_DDR_SECT_TYPE_ARMDSP,
    BSP_DDR_SECT_TYPE_UPA,
    BSP_DDR_SECT_TYPE_CQI,
    BSP_DDR_SECT_TYPE_APT,
    BSP_DDR_SECT_TYPE_ET,
    BSP_DDR_SECT_TYPE_BBPMASTER,
    BSP_DDR_SECT_TYPE_NV,
    BSP_DDR_SECT_TYPE_DICC,
    BSP_DDR_SECT_TYPE_WAN,
    BSP_DDR_SECT_TYPE_SHARE_MEM,
    BSP_DDR_SECT_TYPE_EXCP,
    BSP_DDR_SECT_TYPE_HIFI,
    BSP_DDR_SECT_TYPE_TDS_LH2,
    BSP_DDR_SECT_TYPE_BUTTOM
}BSP_DDR_SECT_TYPE_E;


/* AXI�и����ڴ�ε����� */
typedef enum tagBSP_AXI_SECT_TYPE_E
{
    BSP_AXI_SECT_TYPE_ACORE_DEEPSLEEP = 0x0,
    BSP_AXI_SECT_TYPE_FLASH_SEM,
    BSP_AXI_SECT_TYPE_IFC,
    BSP_AXI_SECT_TYPE_ICC,
    BSP_AXI_SECT_TYPE_MEMMGR_FLAG,
    BSP_AXI_SECT_TYPE_DYNAMIC,
    BSP_AXI_SECT_TYPE_SOFT_FLAG,
    BSP_AXI_SECT_TYPE_IPF,
    BSP_AXI_SECT_TYPE_TEMPERATURE,
    BSP_AXI_SECT_TYPE_ONOFF,
    BSP_AXI_SECT_TYPE_DICC,
    BSP_AXI_SECT_TYPE_HIFI,
    BSP_AXI_SECT_TYPE_PTABLE,
    BSP_AXI_SECT_TYPE_RESERVE,
    BSP_AXI_SECT_TYPE_DLOAD_AUTOINSTALL,
    BSP_AXI_SECT_TYPE_DLOAD,
    BSP_AXI_SECT_TYPE_GUDSP_LOG,
    BSP_AXI_SECT_TYPE_BUTTOM
}BSP_AXI_SECT_TYPE_E;


/* �ڴ������ */
typedef enum tagBSP_DDR_SECT_ATTR_E
{
    BSP_DDR_SECT_ATTR_CACHEABLE = 0x0,
    BSP_DDR_SECT_ATTR_NONCACHEABLE,
    BSP_DDR_SECT_ATTR_BUTTOM
}BSP_DDR_SECT_ATTR_E;


/* ������ʵ��ַ�Ƿ���ͬ��ö�� */
typedef enum tagBSP_DDR_SECT_PVADDR_E
{
    BSP_DDR_SECT_PVADDR_EQU = 0x0,
    BSP_DDR_SECT_PVADDR_NOT_EQU,
    BSP_DDR_SECT_PVADDR_BUTTOM
}BSP_DDR_SECT_PVADDR_E;


/* DDR�ڴ�εĲ�ѯ�ṹ */
typedef struct tagBSP_DDR_SECT_QUERY
{
    BSP_DDR_SECT_TYPE_E     enSectType;
    BSP_DDR_SECT_ATTR_E     enSectAttr;
    BSP_DDR_SECT_PVADDR_E   enPVAddr;
    BSP_U32                 ulSectSize;
}BSP_DDR_SECT_QUERY;


/* DDR�ڴ�ε���ϸ��Ϣ */
typedef struct tagBSP_DDR_SECT_INFO
{
    BSP_DDR_SECT_TYPE_E    enSectType;
    BSP_DDR_SECT_ATTR_E    enSectAttr;
    BSP_U32                ulSectVirtAddr;
    BSP_U32                ulSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_DDR_SECT_INFO;


/* AXI�ڴ�ε���ϸ��Ϣ */
typedef struct tagBSP_AXI_SECT_INFO
{
    BSP_AXI_SECT_TYPE_E    enSectType;
    BSP_U32                ulSectVirtAddr;
    BSP_U32                ulSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_AXI_SECT_INFO;

/*****************************************************************************
 �� �� ��  : BSP_DDR_GetSectInfo
 ��������  : DDR�ڴ�β�ѯ�ӿ�
 �������  : pstSectQuery: ��Ҫ��ѯ���ڴ�����͡�����
 �������  : pstSectInfo:  ��ѯ�����ڴ����Ϣ
 ����ֵ    ��BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_DDR_ADDR(pstSectQuery, pstSectInfo) BSP_DDR_GetSectInfo(pstSectQuery, pstSectInfo)

/*****************************************************************************
 �� �� ��  : BSP_AXI_GetSectInfo
 ��������  : AXI�ڴ�β�ѯ�ӿ�
 �������  : enSectType: ��Ҫ��ѯ���ڴ������
 �������  : pstSectInfo:  ��ѯ�����ڴ����Ϣ
 ����ֵ    ��BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_AXI_ADDR(enSectType, pstSectInfo) BSP_AXI_GetSectInfo(enSectType, pstSectInfo)

/*************************�ڴ���� END********************************/

/*************************IP����ַ���жϺŲ�ѯ START******************/

/* ����������Ҫ��ѯ��IP���� */
typedef enum tagBSP_IP_TYPE_E
{
    BSP_IP_TYPE_SOCP = 0x0,
    BSP_IP_TYPE_CICOM0,
    BSP_IP_TYPE_CICOM1,
    BSP_IP_TYPE_HDLC,
    BSP_IP_TYPE_BBPMASTER,
    BSP_IP_TYPE_ZSP_ITCM,
    BSP_IP_TYPE_ZSP_DTCM,
    BSP_IP_TYPE_AHB,
    BSP_IP_TYPE_WBBP,
    BSP_IP_TYPE_WBBP_DRX,
    BSP_IP_TYPE_GBBP,
    BSP_IP_TYPE_GBBP_DRX,
    BSP_IP_TYPE_ZSPDMA,
    BSP_IP_TYPE_SYSCTRL,
    BSP_IP_TYPE_TDSSYS,
    BSP_IP_TYPE_ZSPDHI,
    BSP_IP_TYPE_BUTTOM
}BSP_IP_TYPE_E;

/*****************************************************************************
 �� �� ��  : BSP_GetIPBaseAddr
 ��������  : IP����ַ��ѯ
 �������  : enIPType: ��Ҫ��ѯ��IP����
 �������  : ��
 ����ֵ    ����ѯ����IP����ַ
*****************************************************************************/
BSP_U32 BSP_GetIPBaseAddr(BSP_IP_TYPE_E enIPType);
#define DRV_GET_IP_BASE_ADDR(enIPType)  BSP_GetIPBaseAddr(enIPType)


/* ��Ҫ��ѯ���ж����� */
typedef enum tagBSP_INT_TYPE_E
{
    BSP_INT_TYPE_RTC = 0,
    BSP_INT_TYPE_WDT,
    BSP_INT_TYPE_USBOTG,
    BSP_INT_TYPE_UICC,
    BSP_INT_TYPE_IPF_C,
    BSP_INT_TYPE_IPF_A,
    BSP_INT_TYPE_SOCP_A,
    BSP_INT_TYPE_SOCP_C,
    BSP_INT_TYPE_CICOM0,
    BSP_INT_TYPE_CICOM1,
    BSP_INT_TYPE_HDLC,
    BSP_INT_TYPE_BBPMASTER,
    BSP_INT_TYPE_GBBP,          /*G_BBP_INT_LEVEL*/
    BSP_INT_TYPE_GBBP_AWAKE,    /*G_BBP_AWAKE_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_0MS,      /*W_BBP_0MS_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_AWAKE,    /*W_BBP_AWAKE_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_SWITCH,   /*W_BBP_CLOCK_SWITCH_INT_LEVEL*/
    BSP_INT_TYPE_INT_OSRTC,     /*RTC_DUAL_TIMER_INT_LEVEL*/
    BSP_INT_TYPE_INT_SIMI,      /*SOC_SMIM_INT_LEVEL*/
    BSP_INT_TYPE_INT_ZSP_DOG,   /*ZSP_WDG_INT_LEVEL*/
    BSP_INT_TYPE_INT_HIFI_DOG,  /*HIFI_WDG_INT_LEVEL*/
    BSP_INT_TYPE_INT_DRX_TIMER, /**/
    BSP_INT_TYPE_BUTTOM
}BSP_INT_TYPE_E;


/*****************************************************************************
 �� �� ��  : BSP_GetIntNO
 ��������  : �жϺŲ�ѯ
 �������  : enIntType: ��Ҫ��ѯ���ж�����
 �������  : ��
 ����ֵ    ����ѯ�����жϺ�
*****************************************************************************/
BSP_S32 BSP_GetIntNO(BSP_INT_TYPE_E enIntType);
#define DRV_GET_INT_NO(enIntType)    BSP_GetIntNO(enIntType)


/*************************IP����ַ���жϺŲ�ѯ END********************/

/*************************��ѯģ���Ƿ�֧�� START**********************/

/* ��ǰ�汾�Ƿ�֧��ĳ��ģ�� */
typedef enum tagBSP_MODULE_SUPPORT_E
{
    BSP_MODULE_SUPPORT     = 0,
    BSP_MODULE_UNSUPPORT   = 1,
    BSP_MODULE_SUPPORT_BUTTOM
}BSP_MODULE_SUPPORT_E;

/* ��ѯ��ģ������ */
typedef enum tagBSP_MODULE_TYPE_E
{
    BSP_MODULE_TYPE_SD = 0x0,
    BSP_MODULE_TYPE_CHARGE,
    BSP_MODULE_TYPE_WIFI,
    BSP_MODULE_TYPE_OLED,
    BSP_MODULE_TYPE_HIFI,
    BSP_MODULE_TYPE_POWER_ON_OFF,
    BSP_MODULE_TYPE_HSIC,
    BSP_MODULE_TYPE_LOCALFLASH,    
    BSP_MODULE_TYPE_BUTTOM
}BSP_MODULE_TYPE_E;

/*****************************************************************************
 �� �� ��  : BSP_CheckModuleSupport
 ��������  : ��ѯģ���Ƿ�֧��
 �������  : enModuleType: ��Ҫ��ѯ��ģ������
 �������  : ��
 ����ֵ    ��BSP_MODULE_SUPPORT��BSP_MODULE_UNSUPPORT
*****************************************************************************/
BSP_MODULE_SUPPORT_E BSP_CheckModuleSupport(BSP_MODULE_TYPE_E enModuleType);

#define DRV_GET_BATTERY_SUPPORT()   BSP_CheckModuleSupport(BSP_MODULE_TYPE_CHARGE)

#define DRV_GET_WIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_WIFI)

#define DRV_GET_SD_SUPPORT()        BSP_CheckModuleSupport(BSP_MODULE_TYPE_SD)

#define DRV_GET_OLED_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_OLED)

#define DRV_GET_HIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HIFI)

#define DRV_GET_HSIC_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HSIC)

#define DRV_GET_LOCAL_FLASH_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_LOCALFLASH)
/*************************��ѯģ���Ƿ�֧�� END************************/

/*************************��ʵ��ַת�� START**************************/

/*****************************************************************************
 �� �� ��  : DRV_DDR_VIRT_TO_PHY
 ��������  : DDR�ڴ����ַ��ʵ��ַת��
 �������  : ulVAddr�����ַ
 �������  : ��
 ����ֵ    ��ʵ��ַ
*****************************************************************************/
unsigned int DRV_DDR_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 �� �� ��  : DRV_DDR_PHY_TO_VIRT
 ��������  : DDR�ڴ����ַ��ʵ��ַת��
 �������  : ulPAddr��ʵ��ַ
 �������  : ��
 ����ֵ    �����ַ
*****************************************************************************/
unsigned int DRV_DDR_PHY_TO_VIRT(unsigned int ulPAddr);

/*****************************************************************************
 �� �� ��  : TTF_VIRT_TO_PHY
 ��������  : TTF�ڴ����ַ��ʵ��ַת��
 �������  : ulVAddr�����ַ
 �������  : ��
 ����ֵ    ��ʵ��ַ
*****************************************************************************/
extern unsigned int TTF_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 �� �� ��  : TTF_PHY_TO_VIRT
 ��������  : TTF�ڴ����ַ��ʵ��ַת��
 �������  : ulPAddr��ʵ��ַ
 �������  : ��
 ����ֵ    �����ַ
*****************************************************************************/
extern unsigned int TTF_PHY_TO_VIRT(unsigned int ulPAddr);

/*****************************************************************************
 �� �� ��  : IPF_VIRT_TO_PHY
 ��������  : IPF�Ĵ������ַ��ʵ��ַת��
 �������  : ulVAddr�����ַ
 �������  : ��
 ����ֵ    ��ʵ��ַ
*****************************************************************************/
extern unsigned int IPF_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 �� �� ��  : IPF_PHY_TO_VIRT
 ��������  : IPF�Ĵ������ַ��ʵ��ַת��
 �������  : ulPAddr��ʵ��ַ
 �������  : ��
 ����ֵ    �����ַ
*****************************************************************************/
extern unsigned int IPF_PHY_TO_VIRT(unsigned int ulPAddr);

/*************************��ʵ��ַת�� END****************************/

/*************************OAM��������ӿ� START***********************/

/*****************************************************************************
 �� �� ��  : BSP_GUDSP_ShareAddrGet
 ��������  : ��ȡ����㹲���ַ�ε���Ϣ��
 �������  : �ޡ�
 �������  : pulAddr���������㹲���ַ����Ϣ�Ļ��档
 �� �� ֵ  : �ޡ�
*****************************************************************************/
extern int BSP_GUDSP_ShareAddrGet(unsigned int * pulAddrInTcm, unsigned int * pulAddrInDdr, unsigned int * pulLength);
#define DRV_DSP_SHARE_ADDR_GET(pulAddrInTcm,pulAddrInDdr,pulLength)    BSP_GUDSP_ShareAddrGet(pulAddrInTcm,pulAddrInDdr,pulLength)

/*****************************************************************************
 �� �� ��  : BSP_UpateDSPShareInfo
 ��������  : ���������BSS COMMON����Ϣ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : 0:�ɹ���-1:ʧ�ܡ�
*****************************************************************************/
extern int BSP_GUDSP_UpateShareInfo(void);
#define DRV_BSP_UPDATE_DSP_SHAREINFO()  BSP_GUDSP_UpateShareInfo()

/* ����ҵ������DMA��������ж�,���ж�ֻ��ӦModem��*/
typedef BSP_VOID (*BSPBBPIntDlTbFunc)(BSP_VOID);
/* BBP��֡�жϴ�����,���ж�ֻ��ӦModem��*/
typedef BSP_VOID (*BSPBBPIntTimerFunc)(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_BBPIntTimerRegCb
*
* ��������  : ��PS���ã����������ע��1ms��ʱ�жϵĻص�
*
* �������  : 
* �������  :��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2011��3��7��  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntTimerRegCb(BSPBBPIntTimerFunc pFunc);

/*****************************************************************************
* �� �� ��  : BSP_BBPIntTimerClear
*
* ��������  : ��PS���ã��������1ms��ʱ�ж�
*
* �������  : ��
* �������  :��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2011��5��31��  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntTimerClear(void);

/*****************************************************************************
* �� �� ��  : BSP_BBPIntTimerEnable
*
* ��������  : ��PS���ã�������1ms��ʱ�ж�
*
* �������  : ��
* �������  :��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2011��3��7��  wangjing  creat
*****************************************************************************/
extern BSP_S32 BSP_BBPIntTimerEnable(void);


/*****************************************************************************
* �� �� ��  : BSP_BBPIntTimerDisable
*
* ��������  : ��PS���ã������ر�1ms��ʱ�ж�
*
* �������  : ��
* �������  :��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2011��3��7��  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntTimerDisable(void);

/*****************************************************************************
* �� �� ��  : BSP_BBPGetCurTime
*
* ��������  : ��PS���ã�������ȡϵͳ��ȷʱ��
*
* �������  : ��
* �������  : BSP_U32 u32CurTime:��ǰʱ��
*
* �� �� ֵ  : BSP_OK : ��ȡ�ɹ�
*                        BSP_ERR_INVALID_PARA  :��ȡʧ��
*
* �޸ļ�¼  : 2011��3��7��  wangjing  creat
*****************************************************************************/
extern BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime);

/*****************************************************************************
* �� �� ��  : BSP_BBPIntDlTbRegCb
*
* ��������  : ��PS���ã����������ע����������DMA��������жϵĻص�
*
* �������  : 
* �������  :��
*
* �� �� ֵ  : VOID
*
* �޸ļ�¼  : 2011��3��7��  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntDlTbRegCb(BSPBBPIntDlTbFunc pFunc);

/*****************************************************************************
* �� �� ��  : BSP_GetSysFrame
*
* ��������  : get system frame num 
*
* �������  : ��
* �������  :
*
* �� �� ֵ  : frame num/0xffff
*
* �޸ļ�¼  : 2012��4��18��  wangjing  creat
*****************************************************************************/
extern BSP_U32 BSP_GetSysFrame(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_GetSysSubFrame
*
* ��������  : get sub system frame num 
*
* �������  : ��
* �������  :
*
* �� �� ֵ  : frame num/0xffff
*
* �޸ļ�¼  : 2012��4��18��  wangjing  creat
*****************************************************************************/
extern BSP_U32 BSP_GetSysSubFrame(BSP_VOID);

/*****************************************************************************
 �� �� ��  : BSP_GetSliceValue
 ��������  : ��ȡSlice��ʱ����ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ֵ��
*****************************************************************************/
extern unsigned int BSP_GetSliceValue(BSP_VOID);
#define DRV_GET_SLICE()   BSP_GetSliceValue()

/*****************************************************************************
 �� �� ��  : BSP_StartHardTimer
 ��������  : ����һ����ʱ����ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ֵ��
*****************************************************************************/
extern BSP_VOID BSP_StartHardTimer( BSP_U32 value );
#define DRV_STATR_HARD_TIMER(value)   BSP_StartHardTimer(value)

/*****************************************************************************
 �� �� ��  : BSP_StartHardTimer
 ��������  : ����һ����ʱ����ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ֵ��
*****************************************************************************/
extern BSP_VOID BSP_StopHardTimer(BSP_VOID);
#define DRV_STOP_HARD_TIMER()   BSP_StopHardTimer()

/*****************************************************************************
 �� �� ��  : BSP_GetHardTimerCurTime
 ��������  : ��ȡһ����ʱ����ʣ��ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ʣ��ֵ��
*****************************************************************************/
extern BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID);
#define DRV_GET_TIMER_CUR_TIME()   BSP_GetHardTimerCurTime()

/*****************************************************************************
 �� �� ��  : BSP_ClearTimerINT
 ��������  : ���һ����ʱ�����ж�
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ʣ��ֵ��
*****************************************************************************/
extern BSP_VOID BSP_ClearTimerINT(BSP_VOID);
#define DRV_CLEAR_TIMER_INT()   BSP_ClearTimerINT()

/*****************************************************************************
 �� �� ��  : BSP_StartHardTimer
 ��������  : ����һ����ʱ����ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ֵ��
*****************************************************************************/
extern BSP_VOID BSP_StartDrxTimer( BSP_U32 value );
#define DRV_STATR_DRX_TIMER(value)   BSP_StartDrxTimer(value)

/*****************************************************************************
 �� �� ��  : BSP_StartHardTimer
 ��������  : ����һ����ʱ����ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ֵ��
*****************************************************************************/
extern BSP_VOID BSP_StopDrxTimer(BSP_VOID);
#define DRV_STOP_DRX_TIMER()   BSP_StopDrxTimer()

/*****************************************************************************
 �� �� ��  : BSP_GetHardTimerCurTime
 ��������  : ��ȡһ����ʱ����ʣ��ֵ��
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ʣ��ֵ��
*****************************************************************************/
extern BSP_U32 BSP_GetDrxTimerCurTime(BSP_VOID);
#define DRV_GET_DRX_TIMER_CUR_TIME()   BSP_GetDrxTimerCurTime()

/*****************************************************************************
 �� �� ��  : BSP_ClearTimerINT
 ��������  : ���һ����ʱ�����ж�
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : ��ʱ����ʣ��ֵ��
*****************************************************************************/
extern BSP_VOID BSP_ClearDrxTimerINT(BSP_VOID);
#define DRV_CLEAR_DRX_TIMER_INT()   BSP_ClearDrxTimerINT()

/*****************************************************************************
* �� �� ��  : BSP_PWC_SetDrxTimerWakeSrc
* ��������  : ����DRX timer ��Ϊ����Դ
* �������  : 
* �������  : 
* �� �� ֵ   :
* �޸ļ�¼  : 
*****************************************************************************/
extern BSP_VOID BSP_PWC_SetDrxTimerWakeSrc(VOID);
#define DRV_SET_DRX_TIMER_WAKE_SRC() BSP_PWC_SetDrxTimerWakeSrc()
/*****************************************************************************
* �� �� ��  : BSP_PWC_DelDrxTimerWakeSrc
* ��������  : ����DRX timer ����Ϊ����Դ
* �������  : 
* �������  : 
* �� �� ֵ  :
* �޸ļ�¼  : 
*****************************************************************************/
extern BSP_VOID BSP_PWC_DelDrxTimerWakeSrc(VOID);
#define DRV_DEL_DRX_TIMER_WAKE_SRC() BSP_PWC_DelDrxTimerWakeSrc()
/**************DRX���� Timer20***************/

/*****************************************************************************
 �� �� ��  : BSP_ClearZSPWatchDogInt
 ��������  : ���һ��ZSP WatchDog���ж�
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : �ж��Ƿ���Ҫ����
*****************************************************************************/
extern BSP_BOOL BSP_ClearZSPWatchDogInt(BSP_VOID);
#define DRV_CLEAR_ZSPDOG_INT()   BSP_ClearZSPWatchDogInt()

/*****************************************************************************
 �� �� ��  : BSP_ClearHIFIWatchDogInt
 ��������  : ���һ��HIFI WatchDog���ж�
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : �ж��Ƿ���Ҫ����
*****************************************************************************/
extern BSP_BOOL BSP_ClearHIFIWatchDogInt(BSP_VOID);
#define DRV_CLEAR_HIFIDOG_INT()   BSP_ClearHIFIWatchDogInt()

/*****************************************************************************
 �� �� ��  : BSP_ZspWatchDogIntEnalbe
 ��������  : ʹ��һ��ZSP WatchDog���ж�
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : �ж��Ƿ���Ҫ����
*****************************************************************************/
extern BSP_VOID BSP_ZSPWatchDogIntEnalbe(int level);
#define DRV_ZSPDOG_INT_ENABLE(level) BSP_ZSPWatchDogIntEnalbe(level)

/*****************************************************************************
 �� �� ��  : BSP_ZspWatchDogIntEnalbe
 ��������  : ʹ��һ��ZSP WatchDog���ж�
 �������  : �ޡ�
 �������  : ��
 �� �� ֵ  : �ж��Ƿ���Ҫ����
*****************************************************************************/
extern BSP_VOID BSP_HIFIWatchDogIntEnalbe(int level);
#define DRV_HIFIDOG_INT_ENABLE(level) BSP_HIFIWatchDogIntEnalbe(level)

/*****************************************************************************
 �� �� ��  : BSP_SendNMIInterrupt
 ��������  : ���� NMI ���ж�
 �������  : NMI�жϵ�bitλ
 �������  : ��
 �� �� ֵ  : �ж��Ƿ���Ҫ����
*****************************************************************************/
extern BSP_VOID BSP_SendNMIInterrupt(unsigned int SocBitNO, unsigned int ZspBitNO);
#define DRV_SEND_NMI_INT(SocBitNO,ZspBitNO) BSP_SendNMIInterrupt(SocBitNO,ZspBitNO)

/*****************************************************************************
 �� �� ��  : BSP_InitPlatformVerInfo
 ��������  : ��ʼ��оƬ�İ汾��
 �������  : ��
 �������  : none
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID BSP_InitPlatformVerInfo(BSP_VOID);
#define DRV_INIT_PLATFORM_VER() BSP_InitPlatformVerInfo()

/******************************************************************************
*  Function:  DRV_START_MODEGET
*  Description:
*  History:
********************************************************************************/
extern BSP_S32  DRV_START_MODEGET(void);

/*****************************************************************************
 �� �� ��  : BSP_GetPlatformInfo
 ��������  : ��ȡоƬ�İ汾��
 �������  : ��
 �������  : u32PlatformInfo:оƬ�İ汾��
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID BSP_GetPlatformInfo(unsigned long *u32PlatformInfo);
#define DRV_GET_PLATFORM_INFO(u32PlatformInfo) BSP_GetPlatformInfo(u32PlatformInfo)

/*���߰汾дSDT��Ϣ���ļ�ϵͳ�ķ�����*/
#define MANUFACTURE_ROOT_PATH "/manufacture"

/*****************************************************************************
 �� �� ��  : drvOnLineUpdateResult
 ��������  : WebUIģ�麯��ע�ᡣ
 �������  : ��
 �������  : �ޡ�
 �� �� ֵ  : ��
 ע������  ��
*****************************************************************************/
extern void drvOnLineUpdateResult(void);
#define DRV_ONLINE_UPDATE_RESULT()    drvOnLineUpdateResult()

/******************************************************************************
*
  ������:       BSP_S32 nand_get_bad_block (BSP_U32 *len, BSP_U32 **ppBadBlock)
  ��������:     ��ѯ����NAND������FLASH ����
  �������:     ��
  �������:     pNum       : ���ػ������
                ppBadBlock ������ָ�룬�������л����index����
  ����ֵ:       0    : ��ѯ�ɹ�
                ���� : ��ѯʧ��
*******************************************************************************/
extern BSP_S32 nand_get_bad_block(BSP_U32 *pNum, BSP_U32 **ppBadBlock);
#define NAND_GET_BAD_BLOCK(pNum, ppBadBlock) nand_get_bad_block(pNum, ppBadBlock)

/******************************************************************************
*
  ������:       BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock)
  ��������:     ͨ��nand_get_bad_block�ӿ�����Ļ����ڴ��ɵ������룬��Э��ջ����
                �ýӿ��ͷš�
  �������:     pBadBlock
  �������:     ��
  ����ֵ:       BSP_VOID
*******************************************************************************/
extern BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock);
#define NAND_FREE_BAD_BLOCK_MEM(pBadBlock)  nand_free_bad_block_mem(pBadBlock)

#define NAND_MFU_NAME_MAX_LEN    16
#define NAND_DEV_SPEC_MAX_LEN    32

typedef struct
{
    BSP_U32           MufId;                                         /* ����ID */
    BSP_U8      aucMufName[NAND_MFU_NAME_MAX_LEN];             /* ���������ַ��� */
    BSP_U32           DevId;                                         /* �豸ID */
    BSP_U8      aucDevSpec[NAND_DEV_SPEC_MAX_LEN];             /* �豸����ַ��� */
}NAND_DEV_INFO_S;

/******************************************************************************
*
  ������:       BSP_S32 nand_get_dev_info (NAND_DEV_INFO_S *pNandDevInfo)
  ��������:     ��ѯNAND�豸�����Ϣ����������ID���������ơ��豸ID���豸���
  �������:     ��
  �������:     pNandDevInfo    �洢NAND�豸�����Ϣ�Ľṹ��
  ����ֵ:       0    : ��ѯ�ɹ�
                ���� : ��ѯʧ��
*******************************************************************************/
extern BSP_S32 nand_get_dev_info(NAND_DEV_INFO_S *pNandDevInfo);
#define NAND_GET_DEV_INFO(pNandDevInfo) nand_get_dev_info(pNandDevInfo)


/******************************************************************************
* Function     :   BSP_NANDF_Read
*
* Description  :   ��ȡNand Flash��ָ����ַ�ͳ������ݵ�ָ���ռ���
*
* Input        :   u32FlashAddr  ��ȡ���ݵ�Դ��ַ
*              :   u32NumByte    ��ȡ���ݳ��ȣ���λΪ�ֽ�
*
* Output       :   pRamAddr      ��ȡ�����ݴ�ŵĵ�ַ
*
* return       :   �������ɹ����
******************************************************************************/
BSP_S32 BSP_NANDF_Read(BSP_VOID* pRamAddr, BSP_U32 u32FlashAddr, BSP_U32 u32NumByte);

/******************************************************************************
* Function     :   BSP_NANDF_Write
*
* Description  :   ��ָ����ַ�ͳ������ݵ�д��ָ��FLASH��ַ��
*
* Input        :   pRamAddr     д����Դ��ַ
*              :   u32NumByte   ���ݳ��ȣ���λΪ�ֽ�
*
* Output       :   u32FlashAddr д����Ŀ�ĵ�ַ
*
* return       :   д�����ɹ����
******************************************************************************/
BSP_S32 BSP_NANDF_Write(BSP_U32 u32FlashAddr, BSP_VOID *pRamAddr,  BSP_U32 u32NumByte);

/******************************************************************************
* Function     :   BSP_NANDF_Erase
*
* Description  :   ����ָ��Flash��ַ���ڿ�
*
* Input        :   u32address   Ҫ�������ID
*
* Output       :   ��
*
* return       :   ���������ɹ����
******************************************************************************/
BSP_S32 BSP_NANDF_Erase(BSP_U32 u32BlockID);

BSP_U32 nand_isbad(BSP_U32 blockID, BSP_U32 *flag);

/********************************************************************************************************
 �� �� ��  : USB_otg_switch_signal_set
 ��������  : ����BALONG��K3�Խ�ʱUSBͨ���л�GPIO���ſ���
 �������  : ��š����źź�ֵ
 �������  : ��

 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int USB_otg_switch_signal_set(UINT8 ucGroup, UINT8 ucPin, UINT8 ucValue);
#define DRV_GPIO_SET(group, pin, value) USB_otg_switch_signal_set(group, pin, value)

#define DRV_GPIO_USB_SWITCH     200
#define DRV_GPIO_HIGH           1
#define DRV_GPIO_LOW            0

/********************************************************************************************************
 �� �� ��  : USB_otg_switch_set
 ��������  : ����BALONG��K3�Խ�ʱMODEM�࿪����ر�USB PHY
 �������  : ������ر�
 �������  : ��

 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int USB_otg_switch_set(UINT8 ucValue);
#define DRV_USB_PHY_SWITCH_SET(value) USB_otg_switch_set(value)

#define AT_USB_SWITCH_SET_VBUS_VALID            1
#define AT_USB_SWITCH_SET_VBUS_INVALID          2

#define USB_SWITCH_ON       1
#define USB_SWITCH_OFF      0

/********************************************************************************************************
 �� �� ��  : USB_otg_switch_get
 ��������  : ����BALONG��K3�Խ�ʱMODEM��USB PHY״̬��ѯ
 �������  : ��
 �������  : ����USB PHY����״̬

 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
********************************************************************************************************/
extern int USB_otg_switch_get(UINT8 *pucValue);
#define DRV_USB_PHY_SWITCH_GET(value) USB_otg_switch_get(value)

/*************************OAM��������ӿ� END*************************/

/************************ Build�Ż� END*******************************/

enum GPIO_OPRT_ENUM
{
    GPIO_OPRT_SET = 0,
    GPIO_OPRT_GET,
    GPIO_OPRT_BUTT
};


/* IOCTL CMD ���� */
#define ACM_IOCTL_SET_WRITE_CB      0x7F001000
#define ACM_IOCTL_SET_READ_CB       0x7F001001
#define ACM_IOCTL_SET_EVT_CB        0x7F001002
#define ACM_IOCTL_SET_FREE_CB       0x7F001003

#define ACM_IOCTL_WRITE_ASYNC       0x7F001010
#define ACM_IOCTL_GET_RD_BUFF       0x7F001011
#define ACM_IOCTL_RETURN_BUFF       0x7F001012
#define ACM_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define ACM_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014

#define ACM_IOCTL_IS_IMPORT_DONE    0x7F001020
#define ACM_IOCTL_WRITE_DO_COPY     0x7F001021

/* Modem ���������� */
#define ACM_MODEM_IOCTL_SET_MSC_READ_CB 0x7F001030
#define ACM_MODEM_IOCTL_MSC_WRITE_CMD   0x7F001031
#define ACM_MODEM_IOCTL_SET_REL_IND_CB  0x7F001032
#define ACM_IOCTL_FLOW_CONTROL  		0x7F001035

/* UDI IOCTL ����ID */
#define UDI_ACM_IOCTL_SET_READ_CB           ACM_IOCTL_SET_READ_CB
#define UDI_ACM_IOCTL_GET_READ_BUFFER_CB    ACM_IOCTL_GET_RD_BUFF
#define UDI_ACM_IOCTL_RETUR_BUFFER_CB       ACM_IOCTL_RETURN_BUFF

#define UART_IOCTL_SET_WRITE_CB      0x7F001000
#define UART_IOCTL_SET_READ_CB       0x7F001001
#define UART_IOCTL_SET_EVT_CB        0x7F001002
#define UART_IOCTL_SET_FREE_CB       0x7F001003
#define UART_IOCTL_WRITE_ASYNC       0x7F001010
#define UART_IOCTL_GET_RD_BUFF       0x7F001011
#define UART_IOCTL_RETURN_BUFF       0x7F001012
#define UART_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define UART_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014
#define UART_IOCTL_SET_WRITE_TIMEOUT 0x7F001015
#define UART_IOCTL_IS_IMPORT_DONE    0x7F001020
#define UDI_UART_IOCTL_SET_READ_CB           UART_IOCTL_SET_READ_CB
#define UDI_UART_IOCTL_GET_READ_BUFFER_CB    UART_IOCTL_GET_RD_BUFF
#define UDI_UART_IOCTL_RETUR_BUFFER_CB       UART_IOCTL_RETURN_BUFF

/* �첽�����շ��ṹ */
typedef struct tagACM_WR_ASYNC_INFO
{
    char* pBuffer;
    unsigned int u32Size;
    void* pDrvPriv;
}ACM_WR_ASYNC_INFO;

/* ACM�豸�¼����� */
typedef enum tagACM_EVT_E
{
    ACM_EVT_DEV_SUSPEND = 0,        /* �豸�����Խ��ж�д(��Ҫ�����¼��ص�������״̬) */
    ACM_EVT_DEV_READY = 1,          /* �豸���Խ��ж�д(��Ҫ�����¼��ص�������״̬) */
    ACM_EVT_DEV_BOTTOM
}ACM_EVT_E;

typedef enum tagACM_IOCTL_FLOW_CONTROL_E
{
    ACM_IOCTL_FLOW_CONTROL_DISABLE = 0,      /* resume receiving data from ACM port */
    ACM_IOCTL_FLOW_CONTROL_ENABLE      /* stop receiving data from ACM port */
}ACM_IOCTL_FLOW_CONTROL_E;

/* ��buffer��Ϣ */
typedef struct tagACM_READ_BUFF_INFO
{
    unsigned int u32BuffSize;
    unsigned int u32BuffNum;
}ACM_READ_BUFF_INFO;

/*************************NCM START***********************************/

typedef struct tagNCM_PKT_S
{
    BSP_U8 *pBuffer;       /* bufferָ��*/
    BSP_U32  u32BufLen;      /* buffer���� */
}NCM_PKT_S;

/* NCM�豸����ö��*/
typedef enum tagNCM_DEV_TYPE_E
{
    NCM_DEV_DATA_TYPE,      /* ����ͨ�����ͣ�PSʹ��*/
    NCM_DEV_CTRL_TYPE       /* ����ͨ����MSP����AT����ʹ��*/
}NCM_DEV_TYPE_E;

/* ������·�հ�����ָ�� */
typedef BSP_VOID (*USBUpLinkRxFunc)(UDI_HANDLE handle, BSP_VOID * pPktNode);

/* ����װ�ͷź���ָ�� */
typedef BSP_VOID (*USBFreePktEncap)(BSP_VOID *PktEncap);

/* USB IOCTLö�� */
typedef enum tagNCM_IOCTL_CMD_TYPE_E
{
    NCM_IOCTL_NETWORK_CONNECTION_NOTIF,      /* 0x0,NCM�����Ƿ�������*/
    NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, /* 0x1,NCM�豸Э�̵������ٶ�*/
    NCM_IOCTL_SET_PKT_ENCAP_INFO,            /* 0x2,���ð���װ��ʽ*/
    NCM_IOCTL_REG_UPLINK_RX_FUNC,            /* 0x3,ע�������հ��ص�����*/
    NCM_IOCTL_REG_FREE_PKT_FUNC,             /* 0x4,ע���ͷŰ���װ�ص�����*/
    NCM_IOCTL_FREE_BUFF,                     /* 0x5,�ͷŵ���buffer*/
    NCM_IOCTL_GET_USED_MAX_BUFF_NUM,         /* 0x6,��ȡ�ϲ�������ռ�õ�ncm buffer����*/
    NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM,        /* 0x7,��ȡĬ�Ϸ���������ֵ����������ֵ������һ��NCM����*/
    NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT,        /* 0x8,��ȡĬ�Ϸ�����ʱʱ�䣬������ʱ�������һ��NCM����*/
    NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE,       /* 0x9,��ȡĬ�Ϸ����ֽ���ֵ����������ֵ������һ��NCM����*/
    NCM_IOCTL_SET_TX_MIN_NUM,                /* 0xa,���÷���������ֵ����������ֵ������һ��NCM����*/
    NCM_IOCTL_SET_TX_TIMEOUT,                /* 0xb,���÷�����ʱʱ�䣬������ʱ�������һ��NCM����*/
    NCM_IOCTL_SET_TX_MAX_SIZE,               /* 0xc,�������ֲ���ʹ�á����÷����ֽ���ֵ����������ֵ������һ��NCM����*/
    NCM_IOCTL_GET_RX_BUF_SIZE,               /* 0xd,��ȡ�հ�buffer��С*/
    NCM_IOCTL_FLOW_CTRL_NOTIF,               /* 0xe,���ؿ��ƿ���*/
    NCM_IOCTL_REG_AT_PROCESS_FUNC,           /* 0xf,ע��AT�����ص�����*/
    NCM_IOCTL_AT_RESPONSE,                   /* 0x10,AT�����Ӧ*/
    NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC,     /* 0x11,ע������״̬�ı�֪ͨ�ص�����*/
    NCM_IOCTL_SET_PKT_STATICS,               /* 0x12,����ͳ����Ϣ*/
       /*��BSP ������ USB NCM��PS��Ҫ�����ṩNCM����״̬��ѯ�ӿ�*/
    NCM_IOCTL_GET_FLOWCTRL_STATUS,           /* 0x13 ��ѯNCM����״̬*/
    /* END:   Modified by liumengcun, 2011-4-21 */

       /*PS��MSP�������ṩ��ѯ��ǰ�������޸����ӿں�NCM ����ʱ�ص���������NCM �豸ID����*/
    NCM_IOCTL_GET_CUR_TX_MIN_NUM,              /* 0x14 ��ȡ��ǰ����������ֵ*/
    NCM_IOCTL_GET_CUR_TX_TIMEOUT,               /* 0x15 ��ȡ��ǰ������ʱʱ��*/
    NCM_IOCTL_IPV6_DNS_NOTIF,              /*0x16 IPV6 DNS�����ϱ�*/
    /* END:   Modified by liumengcun, 2011-6-23 */
    /* BEGIN: Modified by liumengcun, 2011-7-20 ֧��IPV6 DNS����*/
    NCM_IOCTL_SET_IPV6_DNS,                     /* 0x16 ����IPV6 DNS*/
    /* END:   Modified by liumengcun, 2011-7-20 */
    /* BEGIN: Modified by liumengcun, 2011-8-10 MSP������*/
    NCM_IOCTL_CLEAR_IPV6_DNS,                     /* 0x17 ���IPV6 DNS�ڰ�˵Ļ���,param�ڴ�������û�����壬�����ָ�뼴��*/
    NCM_IOCTL_GET_NCM_STATUS,                     /* 0x18 ��ȡNCM����״̬ enable:TRUE(1);disable:FALSE(0) */
    /* END:   Modified by liumengcun, 2011-8-10 */

    NCM_IOCTL_SET_ACCUMULATION_TIME,

    /* BEGIN: Modified by baoxianchun, 2012-5-17 GU PS ������*/
	NCM_IOCTL_SET_RX_MIN_NUM,		/*�����հ�������ֵ*/
	NCM_IOCTL_SET_RX_TIMEOUT,			/*�����հ���ʱʱ��*/
    /* END: Modified by baoxianchun, 2012-5-17 GU PS ������*/

    NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC,   /* NDISͨ��AT����״̬����ص����� */
}NCM_IOCTL_CMD_TYPE_E;

/* NCM����״̬ö��,NCM_IOCTL_NETWORK_CONNECTION_NOTIF�����ֶ�Ӧ����ö��*/
typedef enum tagNCM_IOCTL_CONNECTION_STATUS_E
{
    NCM_IOCTL_CONNECTION_LINKDOWN,      /* NCM����Ͽ�����*/
    NCM_IOCTL_CONNECTION_LINKUP         /* NCM��������*/
}NCM_IOCTL_CONNECTION_STATUS_E;

/* NCM�����ٶȽṹ,NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF�����ֶ�Ӧ�����ṹ��*/
typedef struct tagNCM_IOCTL_CONNECTION_SPEED_S
{
    BSP_U32 u32DownBitRate;
    BSP_U32 u32UpBitRate;
}NCM_IOCTL_CONNECTION_SPEED_S;

/* ����װ�ṹ��,NCM_IOCTL_SET_PKT_ENCAP_INFO�����ֶ�Ӧ�����ṹ��*/
typedef struct tagNCM_PKT_ENCAP_INFO_S
{
    BSP_S32 s32BufOft;      /* bufƫ���� */
    BSP_S32 s32LenOft;      /* lenƫ���� */
    BSP_S32 s32NextOft;     /* nextƫ���� */
}NCM_PKT_ENCAP_INFO_S;

/* AT����ظ�����ָ�뼰���� NCM_IOCTL_AT_RESPONSE*/
typedef struct tagNCM_AT_RSP_S
{
    BSP_U8* pu8AtAnswer;
    BSP_U32 u32Length;
} NCM_AT_RSP_S;
typedef struct tagNCM_IPV6_DNS_NTF_S
{
    BSP_U8* pu8Ipv6DnsNtf;
    BSP_U32 u32Length;
} NCM_AT_IPV6_DNS_NTF_S;

/* AT������պ���ָ�룬�ú���Ϊͬ���ӿڣ���ӦNCM_IOCTL_REG_AT_PROCESS_FUNC������*/
typedef BSP_VOID (*USBNdisAtRecvFunc)(BSP_U8 * pu8Buf, BSP_U32 u32Len);

/* NCM���ؿ���ö��,NCM_IOCTL_NETWORK_CONNECTION_NOTIF�����ֶ�Ӧ����ö��*/
typedef enum tagNCM_IOCTL_FLOW_CTRL_E
{
    NCM_IOCTL_FLOW_CTRL_ENABLE,      /* ������*/
    NCM_IOCTL_FLOW_CTRL_DISABLE      /* �ر�����*/
}NCM_IOCTL_FLOW_CTRL_E;

/* NCM����״̬�ı�֪ͨö��,NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC�����ֶ�Ӧ����ö��*/
typedef enum tagNCM_IOCTL_CONNECT_STUS_E
{
    NCM_IOCTL_STUS_CONNECT,      /* ����*/
    NCM_IOCTL_STUS_BREAK         /* �����Ͽ�,����*/
}NCM_IOCTL_CONNECT_STUS_E;

/* ����״̬�л�֪ͨ��������ӦNCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC������*/
typedef BSP_VOID (*USBNdisStusChgFunc)(NCM_IOCTL_CONNECT_STUS_E enStatus, BSP_VOID * pBuffer);

/* NDISͨ��AT����״̬����ص����� */
typedef BSP_VOID (*USBNdisRespStatusCB)(BSP_VOID *pBuffer, BSP_U32 status);

/*  �´��ϴ������ذ��ĸ���ͳ����Ϣ��
    �����͸�PC�������ȴ�PC�Ķ�ȡ,NCM_IOCTL_SET_PKT_STATICS�����ֶ�Ӧ�����ṹ*/
typedef struct tagNCM_IOCTL_PKT_STATISTICS_S
{
    BSP_U32     u32TxOKCount;         /*���Ͱ���*/
    BSP_U32     u32RxOKCount;         /*���հ���*/
    BSP_U32     u32TxErrCount;        /*���ʹ���*/
    BSP_U32     u32RxErrCount;        /*���մ���*/
    BSP_U32     u32TxOverFlowCount;   /*�����������*/
    BSP_U32     u32RxOverFlowCount;   /*�����������*/
    BSP_U32     u32CurrentTx;         /*��������*/
    BSP_U32     u32CurrentRx;         /*��������*/
} NCM_IOCTL_PKT_STATISTICS_S;

/* BEGIN: Modified by liumengcun, 2011-7-20 IPV6 DNS���ýṹ,NCM_IOCTL_SET_IPV6_DNS��Ӧ�����ṹ*/
#define BSP_NCM_IPV6_DNS_LEN     32
 typedef struct tagNCM_IPV6DNS_S  /* 0x16 ����IPV6 DNS*/
 {
     BSP_U8 * pu8Ipv6DnsInfo;/* 32�ֽڣ���16�ֽڱ�ʾprimaryDNS����16�ֽڱ�ʾSecondaryDNS��*/
     BSP_U32 u32Length;
 } NCM_IPV6DNS_S;
/* END:   Modified by liumengcun, 2011-7-20 */
typedef struct tagNCM_PKT_INFO_S
{
    BSP_U32 u32PsRcvPktNum;              /* �հ��͵�PS�İ�����*/
    BSP_U32 u32RcvUnusedNum;             /* �հ�������PSҪ����������*/
    BSP_U32 u32NcmSendPktNum;            /* ��������*/
}NCM_PKT_INFO_S;


/*************************TIMER BEGIN*****************************/

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Connect
*
* ��������  : This routine specifies the interrupt service routine to be called
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* �������  : FUNCPTR routine   routine to be called at each clock interrupt
              BSP_S32 arg           argument with which to call routine
              BSP_S32 s32UsrClkid      which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Disable
*
* ��������  : This routine disables user clock interrupts.
*
* �������  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_Enable
*
* ��������  : This routine enables user clock interrupts.
*
* �������  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_Enable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* �� �� ��  : BSP_USRCLK_RateSet
*
* ��������  : This routine sets the interrupt rate of the usr clock.
*
* �������  : BSP_S32 ticksPerSecond   number of clock interrupts per second
              BSP_S32 s32UsrClkid         which user clock the interrup routine belongs to
* �������  : ��
* �� �� ֵ  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* �޸ļ�¼  : 2009��1��20��   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid);

/*****************************************************************************
 �� �� ��  : DRV_AXI_VIRT_TO_PHY
 ��������  : AXI�����ַ��ʵ��ַת��
 �������  : ulVAddr�����ַ
 �������  : ��
 ����ֵ    �����ַ
*****************************************************************************/
extern unsigned int DRV_AXI_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 �� �� ��  : DRV_AXI_PHY_TO_VIRT
 ��������  : AXI��ʵ��ַ�����ַת��
 �������  : ulVAddr��ʵ��ַ
 �������  : ��
 ����ֵ    �����ַ
*****************************************************************************/
extern unsigned int DRV_AXI_PHY_TO_VIRT(unsigned int ulPAddr);

/*****************************************************************************
* �� �� ��  : BSP_IPM_FreeBspBuf
*
* ��������  : �ͷ��ڴ�ӿ�
*
* �������  : BSP_U8 *pBuf ��Ҫ�ͷŵ�ָ��
*
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPM_FreeBspBuf(BSP_U8 *pBuf);

/*for create_crypto_key,hash algorithm enum*/
typedef enum 
{
    CREATE_CRYPTO_KEY_ALGORITHM_MD5 = 0x0,
    CREATE_CRYPTO_KEY_ALGORITHM_SHA1,
    CREATE_CRYPTO_KEY_ALGORITHM_MAX
}CREATE_CRYPTO_KEY_ALGORITHM;

/*for crypto_hash,hash algorithm enum*/
typedef enum 
{
    CRYPTO_ALGORITHM_MD5 = 0x0,
    CRYPTO_ALGORITHM_SHA1,
    CRYPTO_ALGORITHM_MAX
}CRYPTO_HASH_ALGORITHM;

/*for crypto_encrypt,aes algorithm enum*/
typedef enum 
{
    CRYPTO_ENCRYPT_ALGORITHM_AES_ECB = 0x0,
    CRYPTO_ENCRYPT_ALGORITHM_MAX
}CRYPTO_ENCRYPT_ALGORITHM;



/*****************************************************************************
* �� �� ��  : create_crypto_key
*
* ��������  : ʹ����������ݺ�HUK��������Կ��
*��ǰ֧��MD5����SHA-1�㷨��������Կ�ķ�������HUK������
*����������������ΪMD5��SHA-1�㷨�����룬������HASHֵ
*
* �������  : data������������������������Կ�����ݡ�
*                           len���������������������ݳ���(�ֽ�)
*                           algorithm��������������ڲ�����Կ���㷨��
*                           key�����������������ɵ���Կ��
*            ע�⣺�䳤�ȱ��벻С��16�ֽڡ���Ϊ��ԿΪ16�ֽڡ�
*                           klen�����������������Ϊ������������key��
*            �������ĳ��ȡ���Ϊ���������������ɵ���Կ�ĳ��ȡ�(�ֽ�)
* �������  : 
*
* �� �� ֵ  :  BSP_OK--���ܳɹ�;BSP_ERROR--����ʧ��
*
* ����˵��  :�ڴ��ɵ���������
*
*****************************************************************************/
int create_crypto_key(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen);
#define CREATE_CRYPTO_KEY(data,len,algorithm,key,klen)  create_crypto_key(data,len,algorithm,key,klen)

/*****************************************************************************
* �� �� ��  : crypto_hash
*
* ��������  : ������������ݵ�HASHֵ��
*                           ��ǰ֧��MD5����SHA-1�㷨�����HASHֵ������16�ֽ�
*
* �������  : data��������������������Ҫ����HASHֵ�����ݡ�
*                           len���������������������ݳ���(�ֽ�)
*                           algorithm�����������HASH�㷨��
*                           hash�����������������ɵ�HASHֵ��
*                           hlen�����������������Ϊ������������HASHֵ�Ļ������ĳ��ȡ�(�ֽ�)
*                           ��Ϊ���������������ɵ�HASHֵ�ĳ��ȡ�
* �������  : 
*
* �� �� ֵ  : BSP_OK--���ܳɹ�;BSP_ERROR--����ʧ��
*
* ����˵��  : �ڴ��ɵ���������
*
*****************************************************************************/
int crypto_hash(char *data, int len, CRYPTO_HASH_ALGORITHM algorithm, char *hash, int *hlen);
#define CRYPTO_HASH(data,len,algorithm,hash,hlen)  crypto_hash(data,len,algorithm,hash,hlen)

/*****************************************************************************
* �� �� ��  : crypto_encrypt
*
* ��������  : ʹ��ָ������Կ��ָ�����㷨����������ݼ��ܣ�������ܺ�����ݡ�
                             ��ǰ֧��AES-ECB�㷨��
*
* �������  : data����������������Ҫ���ܵ����ݡ�
*                           len���������������������ݳ���(�ֽ�)
*                           algorithm�����������HASH�㷨��
*                           key�������������ż�����Կ��
*                           klen�����������key�ĳ��ȡ�(�ֽ�)
*                           cipher_data�������������ż��ܺ�����ݡ�
*                           cipher_len�����������������Ϊ���������������ĵĻ������ĳ��ȡ�(�ֽ�)
*               ��Ϊ���������������ɵ����ĵĳ��ȡ�
* �������  : 
*
* �� �� ֵ  :  BSP_OK--���ܳɹ�;BSP_ERROR--����ʧ��
*
* ����˵��  : �ڴ��ɵ���������
*
*****************************************************************************/
int crypto_encrypt (char *data, int len, CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *cipher_data, int *cipher_len);
#define CRYPTO_ENCRYPT(data,len,algorithm,key,klen,cipher_data,cipher_len)  \
crypto_encrypt(data,len,algorithm,key,klen,cipher_data,cipher_len)

/*****************************************************************************
* �� �� ��  : crypto_decrypt
*
* ��������  : ʹ��ָ������Կ��ָ�����㷨����������ݽ��ܣ�������ܺ�����ݡ�
*             ��ǰ֧��AES-ECB�㷨��
*
* �������  : 
*             cipher_data: ���ܵ����ݵĴ��buffer��
*             cipher_len:  �����ܵ����ݵ�ʵ�ʳ��ȡ�(byte)
*             algorithm:   ���ý����㷨����ֻ�ṩAES-ECB��
*             key:         ��Կbuffer��
*             klen:        ��Կbuffer���ȡ�(byte)
*             len:  ���ܺ�����ݵĴ��buffer��buffer size��(byte)(û�м��)
*
* �������  : 
*             data:        ���ܺ�����ݡ�
*             len:         ���ܺ�����ݳ��ȡ�(byte)
*
* �� �� ֵ  : BSP_OK:      ���ܳɹ���
*             BSP_ERROR:   ����ʧ�ܡ�
*
* ����˵��  : lenΪ����/��������������len���������ڴ�����д�ء�
*             ���Ա���ֱ�Ӵ�������sizeof()�ĺ������ý����
*
*****************************************************************************/
extern int crypto_decrypt (char *cipher_data,int cipher_len,CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *data, int *len);
#define CRYPTO_DECRYPT(cipher_data,cipher_len,algorithm, key, klen, data, len)  \
crypto_decrypt(cipher_data,cipher_len,algorithm, key, klen, data, len)


/*****************************************************************************
* �� �� ��  : crypto_rsa_encrypt
*
* ��������  : ʹ�ñ�����NV�еĸ�����RSA��Կ����ȡʱ��Ҫͬ������ǩ����֤��
*           ����������ݼ��ܣ�������ܺ�����ݡ�
* �������  : data����������������Ҫ���ܵ����ݡ�
*                           len���������������������ݳ���(�ֽ�)
*                           rsa_key:RSA��Կ
*                           rsa_len:RSA��Կ����(�ֽ�)
*                           cipher_data�������������ż��ܺ�����ݡ�
*                           cipher_len�����������������Ϊ���������������ĵĻ������ĳ��ȡ�(�ֽ�)
*               ��Ϊ���������������ɵ����ĵĳ��ȡ�
* �������  : 
*
* �� �� ֵ  :  BSP_OK--���ܳɹ�;BSP_ERROR--����ʧ��
*
* ����˵��  : �ڴ��ɵ���������
*
*****************************************************************************/
int crypto_rsa_encrypt (char *data, int len, char *rsa_key, int rsa_klen, char *cipher_data, int *cipher_len);
#define CRYPTO_RSA_ENCRYT(data,len,rsa_key,rsa_klen,cipher_data,cihper_len) \
crypto_rsa_encrypt(data,len,rsa_key,rsa_klen,cipher_data,cihper_len)

/*****************************************************************************
* �� �� ��  : crypto_rsa_decrypt
*
* ��������  : ʹ�ñ�����NV�еĸ�����RSA��Կ������������ݽ���
*               ������ܺ�����ݡ�
* �������  : cipher_data�������������ż������ݡ�
*                           cipher_len�����������������ĵĻ������ĳ��ȡ�(�ֽ�)
*                           rsa_key:RSA��Կ
*                           rsa_len:RSA��Կ����(�ֽ�)
*                           data������������������ܺ�����ݡ�
*                           len�����������������Ϊ�����������Ž��ܺ�Ļ������ĳ���(�ֽ�)
*               ��Ϊ���������������ɵ����ĵĳ���
* �������  : 
*
* �� �� ֵ  :  BSP_OK--���ܳɹ�;BSP_ERROR--����ʧ��
*
* ����˵��  : �ڴ��ɵ���������
*
*****************************************************************************/
int crypto_rsa_decrypt (char *cipher_data, int cipher_len, char *rsa_key, int rsa_klen, char *data, int *len);
#define CRYPTO_RSA_DECRYPT(cipher_data,cihper_len,rsa_key,rsa_klen,data,len) \
crypto_rsa_decrypt(cipher_data,cihper_len,rsa_key,rsa_klen,data,len)

/*****************************************************************************
* �� �� ��  : crypto_rand
*
* ��������  : ��������ɽӿ�
* �������  : rand_data:��������buffer
*                           len:�����õ���������ֽ���
*
* �������  : 
*
* �� �� ֵ  :  BSP_OK--��ȡ������ɹ�;BSP_ERROR--��ȡʧ��
*
* ����˵��  : �ڴ��ɵ���������
*
*****************************************************************************/
int crypto_rand (char *rand_data, int len);
#define CRYPTO_RAND(rand_data,len)  crypto_rand(rand_data,len)

/*************************************************
 �� �� ��       : efuseWriteHUK
 ��������   : HUKдefuse�ӿ�
 �������   : pBuf:��д����;len:��д����(�ֽ�)
 �������   : 
 �� �� ֵ      : OK/ERROR
 ���ú���   :
 ��������   :

 �޸���ʷ   :
    ��    ��       : 2012��3��27��
   �޸����� : �����ɺ���

*************************************************/
int efuseWriteHUK(char *pBuf,unsigned int len);
#define EFUSE_WRITE_HUK(pBuf,len) efuseWriteHUK(pBuf,len)

/*************************************************
 �� �� ��   : CheckHukIsValid
 ��������   : �ж�HUK�Ƿ���Ч����Ϊȫ0����Ч����ȫ0����Ч
 �������   : ��
 �������   :
 �� �� ֵ   : BSP_FALSE:HUK��Ч��BSP_TRUE:HUK��Ч
 ���ú���   :
 ��������   :

 �޸���ʷ   :
   ��    �� : 2012��3��27��
   �޸����� : �����ɺ���

*************************************************/
int CheckHukIsValid(void);
/*****************************************************************************
 �� �� ��  : DRV_SMS_AWAKE_OLED_ANTISLEEP
 ��������  : A���յ����Ż���OLEDʱ����ϵͳ���ߵ�API�ӿڣ�������ģ�����
 �������  : None
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_SMS_AWAKE_OLED_ANTISLEEP(void);
#define DRV_CHECK_HUK_IS_VALID() CheckHukIsValid()

extern long bsp_sys_creat(const char  *pathname, int mode);
#define DRV_SYS_CREAT(pathname,mode) bsp_sys_creat(pathname,mode)

extern long bsp_sys_open(const char  *filename,int flags, int mode);
#define DRV_SYS_OPEN(filename,flags,mode) bsp_sys_open(filename,flags,mode)

extern long bsp_sys_close(unsigned int fd);
#define DRV_SYS_CLOSE(fd) bsp_sys_close(fd)

extern long bsp_sys_lseek(unsigned int fd, unsigned int offset,unsigned int origin);
#define DRV_SYS_LSEEK(fd,offset,origin) bsp_sys_lseek(fd,offset,origin)

extern long bsp_sys_read(unsigned int fd, char  *buf, unsigned int count);
#define DRV_SYS_READ(fd, buf, count) bsp_sys_read(fd, buf, count)

extern long bsp_sys_write(unsigned int fd, const char  *buf,unsigned int count);
#define DRV_SYS_WRITE(fd, buf, count) bsp_sys_write(fd, buf, count)

extern long bsp_sys_mkdir(const char  *pathname, int mode);
#define DRV_SYS_MKDIR(pathname, mode) bsp_sys_mkdir(pathname, mode)

extern long bsp_sys_rmdir(const char  *pathname);
#define DRV_SYS_RMDIR(pathname) bsp_sys_rmdir(pathname)

extern long bsp_sys_unlink(const char  *pathname);
#define DRV_SYS_UNLINK(pathname) bsp_sys_unlink(pathname)

extern long bsp_sys_getdents(unsigned int fd,
          void  *dirent,
          unsigned int count);
#define DRV_SYS_GETDENTS(fd,dirent,count) bsp_sys_getdents(fd,dirent,count)

extern long bsp_sys_sync(void);
#define DRV_SYS_SYNC() bsp_sys_sync()

extern long bsp_sys_sync_file(unsigned int fd);
#define DRV_SYS_SYNC_FILE(fd) bsp_sys_sync_file(fd)



#if (FEATURE_UE_MODE_TDS == FEATURE_ON)

typedef enum tagTDS_IPC_INT_CORE_E
{
    TDS_IPC_CORE_ARM_ZSP1 = 0x1,
    TDS_IPC_CORE_ARM_ZSP2 = 0x2,
    TDS_IPC_CORE_BUTTOM   
}TDS_IPC_INT_CORE_E;


/*****************************************************************************
 * �� �� ��  : BSP_TDS_GetDynTableAddr
 *
 * ��������  : TDS ��ȡ��̬���ر��׵�ַ
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : Addr:��̬���׵�ַ(DDR) / NULL: Fail
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_U32 BSP_TDS_GetDynTableAddr(BSP_VOID);

/*****************************************************************************
 * �� �� ��  : BSP_TDS_TF_IntConnect
 *
 * ��������  : TDS ֡�жϹҽӺ���
 *
 * �������  : routine:   �ҽӺ���ָ��
 *             parameter: ����
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_S32 BSP_TDS_TF_IntConnect(VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
 * �� �� ��  : BSP_TDS_TF_IntEnable
 *
 * ��������  : TDS ֡�ж�ʹ��
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_S32 BSP_TDS_TF_IntEnable(BSP_VOID);

/*****************************************************************************
 * �� �� ��  : BSP_TDS_TF_IntDisable
 *
 * ��������  : TDS ֡�ж�ȥʹ��
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_S32 BSP_TDS_TF_IntDisable(BSP_VOID);

/*****************************************************************************
 * �� �� ��  : BSP_RunTdsDsp
 *
 * ��������  : ���� ZSP (��ǰ��ȡ��·��Ϊ: "/yaffs0/tds_zsp.bin")
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_VOID ZSP_Init(BSP_VOID);


/*****************************************************************************
 * �� �� ��  : BSP_RunTdsDsp
 *
 * ��������  : ���� ZSP (��ǰ��ȡ��·��Ϊ: "/yaffs0/tds_zsp.bin")
 *
 * �������  : ��
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_S32 BSP_RunTdsDsp(BSP_VOID);

/*****************************************************************************
 * �� �� ��  : BSP_TDS_IPC_IntConnect
 *
 * ��������  : ע��TDS MailBox�ж�
 *
 * �������  :
               BSP_U32 ulLvl :     ����Ҫʹ��(Ϊ���ֽӿ���ʽһ��)
               VOIDFUNCPTR routine �жϷ������
 *             BSP_U32 parameter      �жϷ���������
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* �� �� ��  : BSP_IPC_IntSend
*
* ��������  : �����ж�
*
* �������  :
                IPC_INT_CORE_E enDstore ����Ҫʹ��(Ϊ���ֽӿ���ʽһ��)
                BSP_U32 u32Msg Ҫ���͵���Ϣ
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  :
*****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntSend(TDS_IPC_INT_CORE_E enDstCore, BSP_U32 u32Msg);

/*****************************************************************************
* �� �� ��  : BSP_TDS_IPC_IntEnable
*
* ��������  : ʹ��MailBox�ж�
*
* �������  :
                BSP_U32 ulLvl ����Ҫʹ��(Ϊ���ֽӿ���ʽһ��)
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  :
*****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntEnable(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
 * �� �� ��  : BSP_TDS_IPC_IntDisable
 *
 * ��������  : ȥʹ��MailBox�ж�
 *
 * �������  :
                BSP_U32 ulLvl ����Ҫʹ��(Ϊ���ֽӿ���ʽһ��)
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  :
 *****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntDisable (IPC_INT_LEV_E ulLvl);




/*************************TDS IPC END************************/
#endif 


/*****************************************************************************
* �� �� ��  : BSP_LDSP_EDMA_MemRestore
*
* ��������  : Restore LDSP mem
*
* �������  : 
* �������  : 
* �� �� ֵ   :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 BSP_LDSP_EDMA_MemRestore();


/*****************************************************************************
* �� �� ��  : BSP_LDSP_EDMA_MemStore
*
* ��������  : Store LDSP mem
*
* �������  : 
* �������  : 
* �� �� ֵ  :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_S32 BSP_LDSP_EDMA_MemStore();

/*****************************************************************************
* �� �� ��  : BSP_LDSP_GetEdmaTaskState
*
* ��������  : ��ѯLDSP����TCM���õ�edmaͨ��ʱ�����
*
* �������  : 
* �������  : 
* �� �� ֵ  :
*
* �޸ļ�¼  : 

*****************************************************************************/
BSP_VOID BSP_LDSP_GetEdmaTaskState();

BSP_VOID BSP_GetEdmaTaskState();


/*****************************************************************************
* �� �� ��  : DRV_Get_DspPsAddr
* ��������  : ������AXI��ΪDSP_PS�����4�ֽڿռ��׵�ַ
* �������  : ��
* �������  : ��
* �� �� ֵ  : MEMORY_AXI_DSP_PS_ADDR
* ����˵��  : ��
*****************************************************************************/
BSP_U32 DRV_Get_DspPsAddr();


/*****************************************************************************
* �� �� ��  : DRV_Get_DspMspAddr
* ��������  : ������AXI��ΪDSP_MSP�����4�ֽڿռ��׵�ַ
* �������  : ��
* �������  : ��
* �� �� ֵ  : MEMORY_AXI_DSP_MSP_ADDR
* ����˵��  : ��
*****************************************************************************/
BSP_U32 DRV_Get_DspMspAddr();


/*******************************************************************************
  Function:     BSP_Modem_OS_Status_Swtich
  Description:  ָʾ Modem Ready ���� ��Ready״̬

  Input:        int enable
                ��0:����Modem����Ready״̬ 
                0:����Modem���ڷ�Ready״̬

  Output:
  Return:       0:    �����ɹ�
                -1:   ����ʧ��
*******************************************************************************/
extern int BSP_Modem_OS_Status_Switch(int enable);

/*****************************************************************************
* �� �� ��  : CHG_CHIP_HARD_READ 
* ��������  : ��ȡ������оƬ�Ƿ�OK
* �������  : ��
* �������  : ��
* �� �� ֵ  : 0����ȷ��-1������ȷ
* �޸ļ�¼  : 2013-3-30 wanghaijie
*****************************************************************************/
extern BSP_S32 CHG_CHIP_HARD_READ (void);

#define DRV_OS_STATUS_SWITCH(enable) BSP_Modem_OS_Status_Switch(enable)
#pragma pack(0)


#endif /* end of __DRV_INTERFACE_H__ */

