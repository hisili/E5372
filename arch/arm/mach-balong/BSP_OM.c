
#ifdef __cplusplus
extern "C" 
{
#endif

#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/errno.h>

#include <linux/string.h>
#include <mach/common/bsp_version.h>
#include <mach/common/bsp_memory.h>

#include <mach/BSP_OM.h>
#include "BSP.h"
#include <linux/slab.h>

#if defined BSP_GMAC
#include "synopGMAC_Host.h"
#elif defined BSP_FE
#include "BSP_DRV_END.h"
#endif
#define BSP_BOOT_VER_ADDR MEMORY_RAM_BOOTLOAD_VER_ADDR

#define GPIO_HW_VER_L_PIN 0x10000000
#define GPIO_HW_VER_H_PIN 0x20000000

#define GPIO_PCB_VER_L_PIN 0x40000000
#define GPIO_PCB_VER_H_PIN 0x80000000

#define logMsg printk
#define BSP_PROJECT_INFO project_info

#define BSP_OM_CHECK_RET(ret) \
do{\
    if (OK != ret)\
    {\
        logMsg("OM Check Error, line:%d, return:%d\n", __LINE__, ret,0,0,0,0);\
        return ret;\
    }\
}while(0)

/*2009-06-18 wangxuesong Add for BJ9D01071 begin*/
extern BSP_S32 WDT_IntHandler(BSP_U8 u8WdtId);
extern uint32_t tickGet(void);
extern BSP_S32 BSP_GPIO_GetVersion (BSP_U8 *pu8MainIdx,BSP_U8 *pu8SubIdx);

#if defined(BSP_GMAC)||defined(BSP_FE)
extern VOID BSP_ShowFtpHostAddr(VOID);
extern VOID BSP_ShowLocalIPAddr(VOID);
extern VOID SHOWMAC();
#endif
//clean warning line62
#if (defined (BOARD_FPGA) && defined (VERSION_V3R2))||\
    (defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2))||\
    (defined (BOARD_SFT) && defined (VERSION_V3R2))||\
    defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)

extern void systemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
extern unsigned int exchMemMalloc(UINT32 ulSize);

#endif
/*2009-06-18 wangxuesong Add for BJ9D01071 end*/

//static char* g_OM_shellTaskName = "tShell0";  //clean warning
static UE_SW_BUILD_VER_INFO_STRU g_BspBuildVersion = 
{
    1,      // V 部分
    1,      // R 部分
    1,      // C 部分
    0,      // B 部分
    0,      // SPC 部分
    0,      // 自定义

    // 产品类型编号，即不同外设组合的硬件平台
    (BSP_U32)UE_SW_BUILD_VER_PRODUCT_UNKNOWN,
    __DATE__,
    __TIME__
};

static UE_SW_BUILD_VER_INFO_STRU g_BootLoaderVersion = 
{
    1,      // V 部分
    1,      // R 部分
    1,      // C 部分
    0,      // B 部分
    0,      // SPC 部分
    0,      // 自定义

    // 产品类型编号，即不同外设组合的硬件平台
    (BSP_U32)UE_SW_BUILD_VER_PRODUCT_UNKNOWN,
    __DATE__,
    __TIME__
};

/* 单板主类型命令*/
const BSP_CHAR BoardMainName[8][BOARD_MAINNAME_LEN] = 
{
    "H69DUSA",
    "H69DUSB",
    "H69DUSC",
    "H69DUSD",
    "RSV",
    "RSV",
    "RSV",
    "RSV"
};

/* 单板子类型命令 */
const BSP_CHAR BoardSubName[4][BOARD_SUBNAME_LEN] = 
{
    "VA",
    "VB",
    "VC",
    "VD"
};
/* LTE协议栈MAC地址 */
BSP_U8  g_mac_address_pstable[] =
{
    //0x00,0x1a,0x2b,0x3c,0x4d,0x5f
    0x4c,0x54,0x99,0x45,0xe5,0xd5
};

BSP_S32 OM_GetVersionInfo(OM_VERSION_INFO_S *pstVersionInfo)
{
    if(!pstVersionInfo)
    {
        return ERROR;
    }

#if defined(BSP_ASIC_V7R1)
    memcpy(pstVersionInfo->stAsic,"V7R1",strlen("V7R1"));    
#elif defined(BSP_ASIC_PV500)
    memcpy(pstVersionInfo->stAsic,"PV500",strlen("PV500"));
#elif defined (BSP_BOARD_V3R2_SFT)  /*g00176659*/
    memcpy(pstVersionInfo->stAsic,"V3R2",strlen("V3R2"));
#else
    memcpy(pstVersionInfo->stAsic,"UNKNOWN_ASIC",strlen("UNKNOWN_ASIC"));
#endif

#if defined(BSP_PLATFOROM_SFT)
    memcpy(pstVersionInfo->stPlat,"SFT",strlen("SFT"));
#elif defined(BSP_PLATFOROM_ASIC)
    memcpy(pstVersionInfo->stPlat,"ASIC",strlen("ASIC"));
#elif defined(BSP_PLATFOROM_FPGA)
    memcpy(pstVersionInfo->stPlat,"FPGA",strlen("FPGA"));
#else
    memcpy(pstVersionInfo->stPlat,"UNKNOWN_PLAT",strlen("UNKNOWN_PLAT"));
#endif

#if defined(BSP_PRODUCT_USBSTICK)
    memcpy(pstVersionInfo->stProduct,"USBSTICK",strlen("USBSTICK"));
#elif defined(BSP_PRODUCT_E5)
    memcpy(pstVersionInfo->stProduct,"E5",strlen("E5"));
#elif defined(BSP_PRODUCT_CPE)
    memcpy(pstVersionInfo->stProduct,"CPE",strlen("CPE"));
#elif defined(BSP_PRODUCT_PHONE)
    memcpy(pstVersionInfo->stProduct,"PHONE",strlen("PHONE"));
#else
    memcpy(pstVersionInfo->stProduct,"UNKNOWN_PRODUCT",strlen("UNKNOWN_PRODUCT"));
#endif

#if defined(BSP_BOARD_V7R1_SFT) || defined(BSP_BOARD_V3R2_SFT)
    memcpy(pstVersionInfo->stBoard,"V7R1_SFT",strlen("V7R1_SFT"));
#elif defined(BSP_BOARD_P500_FPGA)
    memcpy(pstVersionInfo->stBoard,"P500_FPGA",strlen("P500_FPGA"));
#elif defined(BSP_BOARD_P500_ASIC)
    memcpy(pstVersionInfo->stBoard,"P500_ASIC",strlen("P500_ASIC"));
#else
    memcpy(pstVersionInfo->stBoard,"UNKNOWN_BOARD",strlen("UNKNOWN_BOARD"));
#endif

#if defined(BSP_CORE_MODEM)
    memcpy(pstVersionInfo->stCore,"MODEM",strlen("MODEM"));
#elif defined(BSP_CORE_APP)
    memcpy(pstVersionInfo->stCore,"APP",strlen("APP"));
#else
    memcpy(pstVersionInfo->stCore,"UNKNOWN_CORE",strlen("UNKNOWN_CORE"));
#endif

#if defined(BSP_COMPILE_SEPARATE)
    memcpy(pstVersionInfo->stCompile,"SEPARATE",strlen("SEPARATE"));
#elif defined(BSP_COMPILE_ALLY)
    memcpy(pstVersionInfo->stCompile,"ALLY",strlen("ALLY"));
#else
    memcpy(pstVersionInfo->stCompile,"UNKNOWN_COMPILE",strlen("UNKNOWN_COMPILE"));
#endif

#if defined(BSP_IMAGE_VXWORKS)
    memcpy(pstVersionInfo->stImageType,"VXWORKS",strlen("VXWORKS"));
#elif defined(BSP_IMAGE_BOOTROM)
    memcpy(pstVersionInfo->stImageType,"BOOTROM",strlen("BOOTROM"));
#elif defined(BSP_IMAGE_DBLVXWORKS)
    memcpy(pstVersionInfo->stImageType,"DBLVXWORKS",strlen("DBLVXWORKS"));
#else
    memcpy(pstVersionInfo->stImageType,"UNKNOWN_IMAGE",strlen("UNKNOWN_IMAGE"));
#endif

    return OK;
}

/*****************************************************************\
5.2.1 BSP_OM_RegRead
【描述】
  读寄存器值
【原型】
   INT32 BSP_OM_RegRead(UINT32 ulRegAddr, ENADDRTYPE enAddrType, UINT32* pulValue)

参数	含义	输入/输出
ulRegAddr	待查询的寄存器地址	输入参数
enAddrType	读取的寄存器类型(8、16、32位)	输入参数
pulValue	返回的寄存器值	输出参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败
\****************************************************************/
BSP_S32 BSP_OM_RegRead(BSP_U32 u32RegAddr, 
    ENADDRTYPE enAddrType, BSP_U32 *pu32Value)
{
    BSP_S32 s32Ret;
    s32Ret = OK;
    if(OK != s32Ret)
    {
        return ERROR;
    }
    
    switch(enAddrType)
    {
        case ADDRTYPE8BIT:
            *pu32Value = *((volatile BSP_U8 *)u32RegAddr);
            return OK;

        case ADDRTYPE16BIT:
            *pu32Value = *((volatile BSP_U16 *)u32RegAddr);
            return OK;

        case ADDRTYPE32BIT:
            *pu32Value = *((volatile BSP_U32 *)u32RegAddr);
            return OK;
            
        default:
            return ERROR;
    }
}

/*****************************************************************\
5.2.1 BSP_OM_RegWrite
【描述】
  写寄存器值
【原型】
   INT32 BSP_OM_RegWrite(UINT32 u32RegAddr, ENADDRTYPE enAddrType, UINT32 u32Value)

参数	含义	输入/输出
u32RegAddr	待写入的寄存器地址	输入参数
enAddrType	写入的寄存器类型(8、16、32位)	输入参数
u32Value	返回的寄存器值	输出参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败
\****************************************************************/
BSP_S32 BSP_OM_RegWrite(BSP_U32 u32RegAddr, 
    ENADDRTYPE enAddrType, BSP_U32 u32Value)
{
    BSP_S32 s32Ret;
    s32Ret = OK;
    if(OK != s32Ret)
    {
        return ERROR;
    }
    
    switch(enAddrType)
    {
        case ADDRTYPE8BIT:
            *((volatile BSP_U8 *)u32RegAddr) = (BSP_U8)u32Value;
            return OK;

        case ADDRTYPE16BIT:
            *((volatile BSP_U16 *)u32RegAddr) = (BSP_U16)u32Value;
            return OK;

        case ADDRTYPE32BIT:
            *((volatile BSP_U32 *)u32RegAddr) = u32Value;
            return OK;
            
        default:
            return ERROR;
    }
}


/*****************************************************************\
5.2.7 BSP_OM_GetVerTime
【描述】
  获得版本编译链接时间

【原型】
   void BSP_OM_GetVerTime(char* strVerTime)
  
参数	含义	输入/输出
strVerTime	版本链接时间字符串地址，输入的buffer长度是64字节	输入参数

【返回值】
无

\****************************************************************/
BSP_VOID BSP_OM_GetVerTime(BSP_S8* strVerTime)
{
    return;
}

/*****************************************************************\
5.2.8 BSP_OM_SoftReboot
【描述】
 单板复位
【原型】
   void BSP_OM_SoftReboot()
【返回值】
无

\****************************************************************/
BSP_VOID BSP_OM_GetVersion()
{
//    const UE_SW_BUILD_VER_INFO_STRU* pBootVer = BSP_GetBootBuildVersion(); //clean warning
    const UE_SW_BUILD_VER_INFO_STRU* pBspVer = BSP_GetBuildVersion();
    //OM_VERSION_INFO_S stOmVersionInfo = {0};
    OM_VERSION_INFO_S stOmVersionInfo;//clean warning
    BSP_CHAR stVersionInfo[256] = {0};

   memset((void*)&stOmVersionInfo,0,sizeof(OM_VERSION_INFO_S)); //clean warning
   
    (BSP_VOID)OM_GetVersionInfo(&stOmVersionInfo);
    sprintf(stVersionInfo,"%s %s %s %s %s %s",
        stOmVersionInfo.stAsic,
        stOmVersionInfo.stPlat,
        stOmVersionInfo.stProduct,
        stOmVersionInfo.stBoard,
        stOmVersionInfo.stCore,
        stOmVersionInfo.stCompile);

    if (pBspVer->ulSpcNo)
    {
    }
    else
    {
    }

#if defined(BSP_GMAC) || defined(BSP_FE)
    BSP_ShowFtpHostAddr();
    BSP_ShowLocalIPAddr();
	SHOWMAC();
#endif
}

/*****************************************************************\
5.2.8 BSP_OM_SoftReboot
【描述】
 单板复位
【原型】
   void BSP_OM_SoftReboot()
【返回值】
无

\****************************************************************/
BSP_VOID BSP_OM_SoftReboot()
{
    //BSP_WDT_reboot();
    return;
}

/*****************************************************************\
5.2.9 BSP_OM_Shutdown
【描述】
 单板关闭
【原型】
   void BSP_OM_ShutDown()
【返回值】
无

\****************************************************************/

BSP_VOID BSP_OM_ShutDown(BSP_VOID) //clean warning
{
}

/*****************************************************************\
5.2.10 BSP_OM_SetBootFlag
【描述】
 设置BootLoader启动标志
【原型】
   INT32 BSP_OM_SetBootFlag(ENBOOTFLAG enBootFlag)
参数	含义	输入/输出
ulFlag	BOOT_FAST 快速启动
BOOT_SLOW 慢速启动	输入参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败

\****************************************************************/
/*BSP_S32 BSP_OM_SetBootFlag(ENBOOTFLAG enBootFlag)
{
    if((enBootFlag == BOOT_FAST) || (enBootFlag == BOOT_SLOW))
    {
        g_enBootFlag = enBootFlag;
        return OK;
    }
    return ERROR;
}
*/
/*****************************************************************\
5.2.11 BSP_OM_ComVersionCtrl
【描述】
 组件版本读写接口
【原型】
INT32 BSP_OM_ComVersionCtrl(INT32* pulValue, UINT32 ulLen,
    UINT32 ulType,UINT32 ulMode)

参数	含义	输入/输出
pulValue	读模式时：为调用者待保存返回的组件版本信息的内存地址。
写模式时：为调用者准备写入的组件版本信息的地址。	输入/输出参数
ulLen	读模式时：为调用者待保存返回的组件版本信息的内存大小。
写模式时：为调用者准备写入的组件版本信息的字符数，不包括"\0"时最大30个字符。	输入参数
ulType	版本信息ID	
ulMode	0：读取指定ID的组件版本信息。
1：写入指定ID的组件版本信息。	输入参数

【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败

\****************************************************************/
/*BSP_S32 BSP_OM_ComVersionCtrl(BSP_S32 *ps32Value, 
    BSP_U32 u32Len, BSP_U32 u32Type, BSP_U32 u32Mode)
{
    return ERROR;
}*/

/*****************************************************************\
5.2.12 BSP_OM_ComVersionQuery
【描述】
查询所有组件的版本号。
【原型】
INT32 BSP_OM_ComVersionQuer(void ** ppVersionInfo,UINT32 * ulLen)
参数	含义	输入/输出
ppVersionInfo	待保存的版本信息地址	输出参数
ulLen	待返回数据的字节数。	输出参数
【返回值】
返回值	含义
OK	处理成功
ERROR	处理失败

\****************************************************************/
/*BSP_S32 BSP_OM_ComVersionQuery(BSP_VOID **ppVersionInfo, 
    BSP_U32 u32Len)
{
    return ERROR;
}*/

/*****************************************************************\
5.2.13 BSP_OM_GetPaPower
【描述】
读取PA发射功率。
【原型】
int BSP_OM_GetPaPower(INT32 *pulData, INT32 *pulTable)
参数	含义	输入/输出
pulData	存放发射功率的输出值。	输出参数
pulTable	电压功率转换表	输入参数

【返回值】
返回值	含义
OK	处理成功。
ERROR	处理失败。

\****************************************************************/
/*BSP_S32 BSP_OM_GetPaPower(BSP_S32 *pu32Data, BSP_U32 *ps32Table)
{
    return ERROR;
}
*/
/*****************************************************************\
5.2.14 BSP_OM_GetPaTemperature
【描述】
读取PA当前温度。
【原型】
int BSP_OM_GetPaTemperature (INT32 *pulData, INT32 *pulTable)
参数	含义	输入/输出
pulData	存放发射功率的输出值。	输出参数
pulTable	电压温度转换表	输入参数

【返回值】
返回值	含义
OK	处理成功。
ERROR	处理失败。

\****************************************************************/
/*BSP_S32 BSP_OM_GetPaTemperature(BSP_S32 *ps32Data, BSP_S32 *ps32Table)
{
    return ERROR;
}*/

/*2009-09-17 wangxuesong added for BJ9D01877 begin*/
/*
*函数名: BSP_OM_GetFPGAVer
*
*输入:  无
*
*输出:  无
*/
BSP_S32 BSP_OM_GetFPGAVer()
{
#if ((defined (CHIP_BB_6920CS) || defined (CHIP_BB_6920ES)) \
        && defined (BOARD_FPGA_P500))
    logMsg("\nCURRENT LOGIC VERSION: \nFPGA1 = %x\nFPGA2 = %x\nFPGA3 = %x\n",\
            (*(int *)0x5016f000), (*(int *)0x52400fec), (*(int *)0x52000070),0,0,0);
#elif (defined (CHIP_BB_NULL) && defined (BOARD_FPGA_P500)) 
    #error "p500 asic can't get FPGA ver"
#endif
    return OK;
}
/*2009-09-17 wangxuesong added for BJ9D01877 end*/

BSP_S32 BSP_OM_ShellLock(BSP_BOOL bRequest)
{
    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_OM_GetFlashSpec
*
* 功能描述  : 获取FLASH规格
*
* 输入参数  : 无
*
* 输出参数  : BSP_U32 *pu32FlashTotalSize  : FLASH总大小
*             BSP_U32 *pu32FlashBlkSize     : FLASH块大小
*             BSP_U32 *pu32FlashPageSize    : FLASH页大小
*
* 返 回 值  : BSP_ERROR:获取失败
*             BSP_OK:获取成功
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_OM_GetFlashSpec(BSP_U32 *pu32FlashTotalSize, BSP_U32 *pu32FlashBlkSize, BSP_U32 *pu32FlashPageSize)
{
    if(!pu32FlashTotalSize)
    {
        logMsg("null pointer pu32FlashTotalSize\n");
        return BSP_ERROR;
    }
    
    if(!pu32FlashBlkSize)
    {
        logMsg("null pointer pu32FlashBlkSize\n");
        return BSP_ERROR;
    }
    
    if(!pu32FlashPageSize)
    {
        logMsg("null pointer pu32FlashPageSize\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_OM_NET
*
* 功能描述  : 网口可维可测接口
*
* 输入参数  : BSP_OM_NET_S *pstNetOm  
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm)
{
    #if defined BSP_GMAC
    BSP_GMAC_OM(pstNetOm);
    #elif defined BSP_FE
    BSP_DRV_EndOM(pstNetOm);
    #endif
}/*lint !e715*/

/*****************************************************************************
* 函 数 名  : BSP_InitBootVersion
*
* 功能描述  : 初始化BOOT版本全局变量
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_InitBootVersion(VOID)
{
    UE_SW_BUILD_VER_INFO_STRU* pstVer = (UE_SW_BUILD_VER_INFO_STRU*)BSP_BOOT_VER_ADDR;
    memcpy((void*)&g_BootLoaderVersion, (void*)pstVer, sizeof(UE_SW_BUILD_VER_INFO_STRU));

    return;
}

/*****************************************************************************
* 函 数 名  : BSP_AtoX
*
* 功能描述  : 将字符串转换为整数(16进制)
*
* 输入参数  : pStr:待转换字符串
*
* 输出参数  : 转换后的整数
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_AtoX(BSP_U8* pStr)
{
	BSP_U8* p = pStr;
    BSP_U32 n;

	n = 0;
	for(;;p++)
    {
		switch(*p)
        {
		case ' ':
		case '\t':
		case '0':
        case 'x':
        case 'X':
			p++;
            continue;
        default:
            break;
		}
		break;
	}
    
	while( (*p >= '0' && *p <= '9') ||
           (*p >= 'a' && *p <= 'f') ||
           (*p >= 'A' && *p <= 'F') )
    {
        if (*p >= '0' && *p <= '9')
        {
            n = n*16 + *p++ - '0';
        }
        else if (*p >= 'a' && *p <= 'f')
        {
            n = n*16 + (*p++ - 'a' + 10);
        }
        else /* A ~ F */
        {
            n = n*16 + (*p++ - 'A' + 10);
        }
    }   
	return (n);
}

/*****************************************************************************
* 函 数 名  : BSP_ParseVersionInfo
*
* 功能描述  : 解析版本字符串到全局变量
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
LOCAL BSP_VOID BSP_ParseVersionInfo(BSP_VOID)
{
#define BSP_VER_IS_NUM(num) ((num) >= '0' && (num) <= '9')
#define MAX_VER_SECTION 8
#define VER_PART_LEN 3

    BSP_CHAR * pCur, * pFind;
    BSP_CHAR au8Args[MAX_VER_SECTION] = {0};

    pCur = BSP_SET_VERSION;
    if (NULL == pCur)/*lint !e774*/
    {
        return;
    }

    pFind = pCur;
    
    while (NULL != (pCur = strchr(pCur, 'V')) )
    {
        pCur++;

        if (BSP_VER_IS_NUM(*(pCur)))
        {
            strncpy(au8Args, pCur, VER_PART_LEN);
//            g_BspBuildVersion.ulVVerNo = (BSP_U16)atoi(au8Args);
            //printf("V:%s (%d)\n", au8Args, g_BspBuildVersion.ulVVerNo);
            break;
        }
    }
    pCur = pFind;
    while (NULL != (pCur = strchr(pFind, 'R')) )
    {
        pCur++;

        if (BSP_VER_IS_NUM(*(pCur-2)) && BSP_VER_IS_NUM(*(pCur)))
        {
            memset(au8Args, 0, MAX_VER_SECTION);
            strncpy(au8Args, pCur, VER_PART_LEN);
//            g_BspBuildVersion.ulRVerNo = (BSP_U16)atoi(au8Args);
            //printf("R:%s(%d)\n", au8Args, g_BspBuildVersion.ulRVerNo);
            break;
        }
    }
    pCur = pFind;
    while (NULL != (pCur = strchr(pFind, 'C')) )
    {
        pCur++;

        if (BSP_VER_IS_NUM(*(pCur-2)) && BSP_VER_IS_NUM(*(pCur)))
        {
            memset(au8Args, 0, MAX_VER_SECTION);
            strncpy(au8Args, pCur, VER_PART_LEN);
            pCur += VER_PART_LEN;
//            g_BspBuildVersion.ulCVerNo = (BSP_U16)atoi(au8Args);
            //printf("C:%s(%d)\n", au8Args, g_BspBuildVersion.ulCVerNo);
            break;
        }
    }
    pCur = pFind;
    while (NULL != (pCur = strchr(pFind, 'B')) )
    {
        pCur++;

        if (BSP_VER_IS_NUM(*(pCur-2)) && BSP_VER_IS_NUM(*(pCur)))
        {
            memset(au8Args, 0, MAX_VER_SECTION);
            strncpy(au8Args, pCur, VER_PART_LEN);
//            g_BspBuildVersion.ulBVerNo = (BSP_U16)atoi(au8Args);
            //printf("B:%s(%d)\n", au8Args, g_BspBuildVersion.ulBVerNo);
            break;
        }
    }
    pCur = pFind;
    while (NULL != (pCur = strstr(pFind, "SPC")) )
    {
        pCur+=3;

        if (BSP_VER_IS_NUM(*(pCur-4)) && BSP_VER_IS_NUM(*(pCur)))
        {
            memset(au8Args, 0, MAX_VER_SECTION);
            strncpy(au8Args, pCur, VER_PART_LEN);
//            g_BspBuildVersion.ulSpcNo = (BSP_U16)atoi(au8Args);
            //printf("SPC:%s(%d)\n", au8Args, g_BspBuildVersion.ulSpcNo);
            break;
        }
    }

    return;
}

/*****************************************************************************
* 函 数 名  : BSP_InitBspVersion
*
* 功能描述  : 初始化BSP版本全局变量
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_InitBspVersion(VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U32 u32ReadData = 0;

    /* HW版本号 */
    g_BspBuildVersion.ulCustomVer = ((u32ReadData >> 30) << 8);

    /* PCB版本号 */
    g_BspBuildVersion.ulCustomVer |= ((u32ReadData >> 28) & 0x3);

    /* 拷贝编译时间 */
    strncpy((char *)g_BspBuildVersion.acBuildDate, __DATE__, BUILD_DATE_LEN);
    strncpy((char *)g_BspBuildVersion.acBuildTime, __TIME__, BUILD_TIME_LEN);

    /* 解析版本字符串 */
    BSP_ParseVersionInfo();

    return s32Ret;
}

/*****************************************************************************
* 函 数 名  : BSP_GetBootBuildVersion
*
* 功能描述  : 获取Boot版本
*
* 输入参数  : 无
*
* 输出参数  : BOOT版本结构体指针
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBootBuildVersion(VOID)
{
    return &g_BootLoaderVersion;
}

/*****************************************************************************
* 函 数 名  : BSP_GetBuildVersion
*
* 功能描述  : 获取BSP版本
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP版本结构体指针
*
* 其它说明  : 无
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBuildVersion(VOID)
{
    return &g_BspBuildVersion;
}

/*****************************************************************************
* 函 数 名  : BSP_GetBuildVersion
*
* 功能描述  : 获取BSP版本
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP版本结构体指针
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 BSP_OM_GetHwVersion(BSP_VOID)
{
    BSP_S32 s32Ret = 0;
    BSP_U8 u8MainIdx = 0,u8SubIdx = 0;    

#ifndef BSP_CORE_APP   
    s32Ret = OK;
    if(OK != s32Ret)
    {
        return ERROR;
    }
    
    printk("hardware version:%s %s\n",BoardMainName[u8MainIdx],BoardSubName[u8SubIdx]);
#endif

    return OK;
}


/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardType
*
* 功能描述  : 获取单板类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
BOARD_TYPE_E BSP_OM_GetBoardType()
{ 
#if ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)) \
   && (defined(BOARD_ASIC) || defined(BOARD_ASIC_BIGPACK) || defined(BOARD_SFT)))
    #ifdef PRODUCT_CFG_MULTIMODE_GUL
        return BOARD_TYPE_GUL;
    #else
        return BOARD_TYPE_LTE_ONLY;
    #endif
#elif (defined(BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
    return (BOARD_TYPE_E)(BSP_REG(IO_ADDRESS(BOARD_TYPE_REG_ADDR), 0) & 0x1);
#elif defined(VERSION_V3R2)
       return BOARD_TYPE_GUL;
#else
    #error "unknown product macro"
#endif  
}

#if defined (BOARD_FPGA) && defined (VERSION_V3R2)\
    ||(defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))

#else
BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl)
{
    return 0;
}
#endif

BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime)
{
#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))

    BSP_U64 u64TempTime[4];

    if (NULL == pCurTime)
    {
        printk("BSP_BBPGetCurTime Wrong Para. line:%d\n", __LINE__);
        return BSP_ERR_INVALID_PARA;
    }

    BSP_REG_READ(BSP_BBP_SYSTIME_ADDR, 0, u64TempTime[0]);
    BSP_REG_READ(BSP_BBP_SYSTIME_ADDR, 4, u64TempTime[1]);
    BSP_REG_READ(BSP_BBP_SYSTIME_ADDR, 0, u64TempTime[2]);
    BSP_REG_READ(BSP_BBP_SYSTIME_ADDR, 4, u64TempTime[3]);

    if (u64TempTime[2] < u64TempTime[0])
    {
        *pCurTime = ((u64TempTime[3] - 1) << 32) | u64TempTime[0];
    }
    else
    {
        *pCurTime = (u64TempTime[1] << 32) | u64TempTime[0];
    }

#endif
    return 0;
}

#if defined (BOARD_FPGA) && defined (VERSION_V3R2)\
    ||(defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))

#else
BSP_S32 BSP_INT_Connect(VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    int result = 0;
    result = request_irq(IVEC_TO_INUM(vector), (irq_handler_t)routine, 0, "om", (void *)parameter); //clean warning
    if (result) {
                printk(KERN_ERR "BSP_INT_Connect: can't get assigned irq %i\n", IVEC_TO_INUM(vector));
    }
    return result;
}
#endif

#if defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)
/* 已经实现，不需要打桩 */

#else   //V3R2

BSP_S32   BSP_USRCLK_Enable (BSP_S32 s32UsrClkid)
{
    return 0;
}

BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid)
{
    return 0;
}

BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid)
{
    return 0;
}

BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid)
{
    return 0;
}

#endif

unsigned int BSP_MNTN_ExchMemMalloc(unsigned int ulSize)
{
	#if (defined (BOARD_FPGA) && defined (VERSION_V3R2))||\
    (defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2))|| \
    (defined (BOARD_SFT) && defined (VERSION_V3R2))|| \
    (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
    
	return exchMemMalloc(ulSize);
	#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
    && (defined (BOARD_FPGA_P500)||defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
    return exchMemMalloc(ulSize);
	#endif
    return (unsigned int)kmalloc(ulSize, GFP_KERNEL); //clean warning
}

void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length)
{
    systemError( modId, arg1, arg2, arg3, arg3Length);
}
/*****************************************************************************
* 函 数 名  : BSP_GetMacAddr
*
* 功能描述  : 获取ps mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
BSP_U8* BSP_GetMacAddr(void)  //clean warning
{
    return g_mac_address_pstable;
}

/*****************************************************************************
* 函 数 名  : BSP_SetPsMacAddr
*
* 功能描述  : 修改PS mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   liumengcun  creat
*****************************************************************************/
BSP_S32 BSP_SetPsMacAddr(BSP_U8 u8MacAddr0,BSP_U8 u8MacAddr1,BSP_U8 u8MacAddr2,
		BSP_U8 u8MacAddr3,BSP_U8 u8MacAddr4,BSP_U8 u8MacAddr5)
{
	g_mac_address_pstable[0] = u8MacAddr0;
    g_mac_address_pstable[1] = u8MacAddr1;
    g_mac_address_pstable[2] = u8MacAddr2;
    g_mac_address_pstable[3] = u8MacAddr3;
    g_mac_address_pstable[4] = u8MacAddr4;
    g_mac_address_pstable[5] = u8MacAddr5;
 
	return OK;
}

#if 0
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBootBuildVersion(VOID)
{
    return NULL;
}
#endif


unsigned int   * MemAddr32 = 0x00000000;

/*******************************************************************************
*
* writeM - write a unsigned int value to perihical address
*
*
* RETURNS: N/A
*/
void writeM(unsigned int pAddr, unsigned int value)
{
    *(volatile unsigned int *)(pAddr)=value;
}


/***********************************************************
 Function: memRead32--read memory and display the value
 Input:    the start address
 return:  void
 see also:memRead16/memRead8
 History:
 1.    2004.9.30   Creat
 2.    2007.3.20   Code stardand
************************************************************/
void memRead32 (unsigned int * uiMemAddr32)
{
    if (0 != uiMemAddr32)
    {
         MemAddr32 = (unsigned int *)((unsigned int)uiMemAddr32 & 0xFFFFFFFC);
    }
     
    printk("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (unsigned int)(MemAddr32+0x0), *(MemAddr32+0x0), *(MemAddr32+0x1), *(MemAddr32+0x2), *(MemAddr32+0x3));
    printk("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (unsigned int)(MemAddr32+0x4), *(MemAddr32+0x4), *(MemAddr32+0x5), *(MemAddr32+0x6), *(MemAddr32+0x7));
    printk("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (unsigned int)(MemAddr32+0x8), *(MemAddr32+0x8), *(MemAddr32+0x9), *(MemAddr32+0xa), *(MemAddr32+0xb));
    printk("0x%.8X: 0x%.8X   0x%.8X   0x%.8X   0x%.8X\n",
            (unsigned int)(MemAddr32+0xc), *(MemAddr32+0xc), *(MemAddr32+0xd), *(MemAddr32+0xe), *(MemAddr32+0xf));
    MemAddr32 += 0x10;
}

/***********************************************************
 Function: r--simple read memory command
 Input:    the start address
 return:  void
 see also:memRead16/memRead8/memRead32
 History:
 1.    2004.9.30   Creat
 2.    2007.3.20   Code stardand
************************************************************/
void r(unsigned int * newMemAddr32)
{
    memRead32(newMemAddr32);
}
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
int BSP_PWRCTRL_SleepInSocCB(PWC_DS_SOCP_CB_STRU stFunc)
{
	return 0;
}
#endif
EXPORT_SYMBOL(BSP_GetBootBuildVersion);
EXPORT_SYMBOL(BSP_OM_RegRead);
EXPORT_SYMBOL(BSP_INT_Connect);
EXPORT_SYMBOL(BSP_BBPGetCurTime);
EXPORT_SYMBOL(BSP_OM_ShellLock);
EXPORT_SYMBOL(BSP_IFC_RegFunc);
//EXPORT_SYMBOL(BSP_UDI_FindVcom);
EXPORT_SYMBOL(udi_close);
EXPORT_SYMBOL(BSP_INT_Enable);
//EXPORT_SYMBOL(BSP_SYNC_Wait);
EXPORT_SYMBOL(udi_open);
EXPORT_SYMBOL(BSP_OM_GetBoardType);
EXPORT_SYMBOL(BSP_GetBuildVersion);
EXPORT_SYMBOL(BSP_OM_RegWrite);
EXPORT_SYMBOL(BSP_IFC_Send);
EXPORT_SYMBOL(udi_read);
EXPORT_SYMBOL(udi_ioctl);
EXPORT_SYMBOL(udi_write);
EXPORT_SYMBOL(BSP_OM_SoftReboot);
EXPORT_SYMBOL(BSP_MNTN_ExchMemMalloc);
EXPORT_SYMBOL(BSP_MNTN_SystemError);
EXPORT_SYMBOL(BSP_GetMacAddr);
EXPORT_SYMBOL(BSP_SetPsMacAddr);
#if defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)
#else
EXPORT_SYMBOL(BSP_USRCLK_Enable);
EXPORT_SYMBOL(BSP_USRCLK_Connect);
EXPORT_SYMBOL(BSP_USRCLK_RateSet);
EXPORT_SYMBOL(BSP_USRCLK_Disable);
#endif
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
EXPORT_SYMBOL(BSP_PWRCTRL_SleepInSocCB);
#endif
#ifdef __cplusplus
}
#endif

