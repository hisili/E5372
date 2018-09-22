/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  Drvinterface.c
*
*   作    者 :  c61362
*
*   描    述 :  底软给上层软件封装的接口层
*
*   修改记录 :  2012年3月2日 创建
*************************************************************************/
#include "BSP.h"
#include <linux/module.h>
#include <mach/common/bsp_memory.h>
#include <generated/MemoryMap.h>
#include <generated/FeatureConfig.h> //clean warning



#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#ifndef __BOOTLOADER__
extern int	printf (const char *, ...);
extern void * memcpy(void * destination, const void * source, int size);
#define   DRV_PRINTF     printf
#endif
#else
int printk(const char *fmt, ...);
#define   DRV_PRINTF     printk
#endif


/* 全局的结构体，记录DDR内存段分配信息 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
BSP_DDR_SECT_INFO g_stDdrSectInfo[BSP_DDR_SECT_TYPE_BUTTOM] = {
    {BSP_DDR_SECT_TYPE_TTF,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_TTF_BASE_ADDR ,              ECS_TTF_BASE_ADDR,            ECS_TTF_SIZE},
    {BSP_DDR_SECT_TYPE_ARMDSP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_ARM_DSP_BUFFER_BASE_ADDR,    ECS_ARM_DSP_BUFFER_BASE_ADDR, ECS_ARM_DSP_BUFFER_SIZE},
    {BSP_DDR_SECT_TYPE_UPA,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_UPA_BASE_ADDR,               ECS_UPA_BASE_ADDR,            ECS_UPA_SIZE},
    {BSP_DDR_SECT_TYPE_CQI,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_CQI_BASE_ADDR,               ECS_CQI_BASE_ADDR,            ECS_CQI_SIZE},
    {BSP_DDR_SECT_TYPE_APT,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_APT_BASE_ADDR,               ECS_APT_BASE_ADDR,            ECS_APT_SIZE},
    {BSP_DDR_SECT_TYPE_ET,        BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_ET_BASE_ADDR,                ECS_ET_BASE_ADDR,             ECS_ET_SIZE},
    {BSP_DDR_SECT_TYPE_BBPMASTER, BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_BBP_MASTER_BASE_ADDR,        ECS_BBP_MASTER_BASE_ADDR,     ECS_BBP_MASTER_SIZE},
    {BSP_DDR_SECT_TYPE_NV,        BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_NV_BASE_ADDR_VIRT,           ECS_NV_BASE_ADDR,             ECS_NV_SIZE},
    {BSP_DDR_SECT_TYPE_DICC,      BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_TTF_DICC_ADDR_VIRT,          ECS_TTF_DICC_ADDR,            ECS_TTF_DICC_SIZE},
    {BSP_DDR_SECT_TYPE_WAN,       BSP_DDR_SECT_ATTR_NONCACHEABLE, CORESHARE_MEM_WAN_ADDR,          CORESHARE_MEM_WAN_ADDR,       CORESHARE_MEM_WAN_SIZE},
    {BSP_DDR_SECT_TYPE_SHARE_MEM, BSP_DDR_SECT_ATTR_NONCACHEABLE, GLOBAL_MEM_CORE_SHARE_ADDR,      GLOBAL_MEM_CORE_SHARE_ADDR,   GLOBAL_MEM_CORE_SHARE_SIZE},
    {BSP_DDR_SECT_TYPE_EXCP,      BSP_DDR_SECT_ATTR_NONCACHEABLE, GLOBAL_MEM_MCORE_USER_RESERVED_ADDR, GLOBAL_MEM_MCORE_USER_RESERVED_ADDR, GLOBAL_MEM_MCORE_USER_RESERVED_SIZE},
    {BSP_DDR_SECT_TYPE_HIFI,      BSP_DDR_SECT_ATTR_NONCACHEABLE, GLOBAL_MEM_HIFI_ADDR,            GLOBAL_MEM_HIFI_ADDR,    GLOBAL_MEM_HIFI_SIZE},
    {BSP_DDR_SECT_TYPE_TDS_LH2,   BSP_DDR_SECT_ATTR_NONCACHEABLE, GLOBAL_MEM_TDS_LH2_ADDR,         GLOBAL_MEM_TDS_LH2_ADDR,    GLOBAL_MEM_TDS_LH2_SIZE}
    };
#else
BSP_DDR_SECT_INFO g_stDdrSectInfo[BSP_DDR_SECT_TYPE_BUTTOM] = {
    {BSP_DDR_SECT_TYPE_TTF,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_TTF_BASE_ADDR_VIRT,          ECS_TTF_BASE_ADDR,            ECS_TTF_SIZE},
    {BSP_DDR_SECT_TYPE_ARMDSP,    BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_ARM_DSP_BUFFER_BASE_ADDR,    ECS_ARM_DSP_BUFFER_BASE_ADDR, ECS_ARM_DSP_BUFFER_SIZE},
    {BSP_DDR_SECT_TYPE_UPA,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_UPA_BASE_ADDR,               ECS_UPA_BASE_ADDR,            ECS_UPA_SIZE},
    {BSP_DDR_SECT_TYPE_CQI,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_CQI_BASE_ADDR,               ECS_CQI_BASE_ADDR,            ECS_CQI_SIZE},
    {BSP_DDR_SECT_TYPE_APT,       BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_APT_BASE_ADDR,               ECS_APT_BASE_ADDR,            ECS_APT_SIZE},
    {BSP_DDR_SECT_TYPE_ET,        BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_ET_BASE_ADDR,                ECS_ET_BASE_ADDR,             ECS_ET_SIZE},
    {BSP_DDR_SECT_TYPE_BBPMASTER, BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_BBP_MASTER_BASE_ADDR,        ECS_BBP_MASTER_BASE_ADDR,     ECS_BBP_MASTER_SIZE},
    {BSP_DDR_SECT_TYPE_NV,        BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_NV_BASE_ADDR_VIRT,           ECS_NV_BASE_ADDR,             ECS_NV_SIZE},
    {BSP_DDR_SECT_TYPE_DICC,      BSP_DDR_SECT_ATTR_NONCACHEABLE, ECS_TTF_DICC_ADDR_VIRT,          ECS_TTF_DICC_ADDR,            ECS_TTF_DICC_SIZE},
    {BSP_DDR_SECT_TYPE_WAN,       BSP_DDR_SECT_ATTR_NONCACHEABLE, CORESHARE_MEM_WAN_ADDR,          CORESHARE_MEM_WAN_ADDR,       CORESHARE_MEM_WAN_SIZE},
    {BSP_DDR_SECT_TYPE_SHARE_MEM, BSP_DDR_SECT_ATTR_NONCACHEABLE, IO_ADDRESS(GLOBAL_MEM_CORE_SHARE_ADDR), GLOBAL_MEM_CORE_SHARE_ADDR,   GLOBAL_MEM_CORE_SHARE_SIZE},
    {BSP_DDR_SECT_TYPE_EXCP,      BSP_DDR_SECT_ATTR_NONCACHEABLE, PBXA9_DRAM_EXC_SHARE_VIRT,       PBXA9_DRAM_EXC_SHARE_PHYS,    PBXA9_DRAM_EXC_SHARE_SIZE},
    {BSP_DDR_SECT_TYPE_HIFI,      BSP_DDR_SECT_ATTR_NONCACHEABLE, GLOBAL_MEM_HIFI_ADDR,            GLOBAL_MEM_HIFI_ADDR,    GLOBAL_MEM_HIFI_SIZE}
    };
#endif

/* 全局的结构体，记录AXI内存段分配信息 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
BSP_AXI_SECT_INFO g_stAxiSectInfo[BSP_AXI_SECT_TYPE_BUTTOM] = {
    {BSP_AXI_SECT_TYPE_ACORE_DEEPSLEEP,   0,                                 0,                                 0},
    {BSP_AXI_SECT_TYPE_FLASH_SEM,         MEMORY_AXI_FLASH_SEM_ADDR,         MEMORY_AXI_FLASH_SEM_ADDR,         MEMORY_AXI_FLASH_SEM_SIZE},
    {BSP_AXI_SECT_TYPE_IFC,               MEMORY_AXI_IFC_ADDR,               MEMORY_AXI_IFC_ADDR,               MEMORY_AXI_IFC_SIZE},
    {BSP_AXI_SECT_TYPE_ICC,               MEMORY_AXI_ICC_ADDR,               MEMORY_AXI_ICC_ADDR,               MEMORY_AXI_ICC_SIZE},
    {BSP_AXI_SECT_TYPE_MEMMGR_FLAG,       MEMORY_AXI_MEMMGR_FLAG_ADDR,       MEMORY_AXI_MEMMGR_FLAG_ADDR,       MEMORY_AXI_MEMMGR_FLAG_SIZE},
    {BSP_AXI_SECT_TYPE_DYNAMIC,           MEMORY_AXI_DYNAMIC_SEC_ADDR,       MEMORY_AXI_DYNAMIC_SEC_ADDR,       MEMORY_AXI_DYNAMIC_SEC_SIZE},
    {BSP_AXI_SECT_TYPE_SOFT_FLAG,         0,                                 0,                                 0},
    {BSP_AXI_SECT_TYPE_IPF,               MEMORY_AXI_IPF_ADDR,               MEMORY_AXI_IPF_ADDR,               MEMORY_AXI_IPF_SIZE},
    {BSP_AXI_SECT_TYPE_TEMPERATURE,       MEMORY_AXI_TEMP_PROTECT_ADDR,      MEMORY_AXI_TEMP_PROTECT_ADDR,      MEMORY_AXI_TEMP_PROTECT_SIZE},
    {BSP_AXI_SECT_TYPE_ONOFF,             MEMORY_AXI_ONOFF_ADDR,             MEMORY_AXI_ONOFF_ADDR,             MEMORY_AXI_ONOFF_SIZE},
    {BSP_AXI_SECT_TYPE_DICC,              MEMORY_AXI_DICC_ADDR,              MEMORY_AXI_DICC_ADDR,              MEMORY_AXI_DICC_SIZE},
    {BSP_AXI_SECT_TYPE_HIFI,              MEMORY_AXI_HIFI_ADDR,              MEMORY_AXI_HIFI_ADDR,              MEMORY_AXI_HIFI_SIZE},
    {BSP_AXI_SECT_TYPE_PTABLE,            MEMORY_AXI_PTABLE_ADDR,            MEMORY_AXI_PTABLE_ADDR,            MEMORY_AXI_PTABLE_SIZE},
    {BSP_AXI_SECT_TYPE_RESERVE,           0,                                 0,                                 0},
    {BSP_AXI_SECT_TYPE_DLOAD_AUTOINSTALL, MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR, MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR, MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE},
    {BSP_AXI_SECT_TYPE_DLOAD,             0,             0,             0}
    };
#else
#define AXI_V2P(var) ((var) - AXI_MEM_ADDR + AXI_MEM_BASE_ADDR)
BSP_AXI_SECT_INFO g_stAxiSectInfo[BSP_AXI_SECT_TYPE_BUTTOM] = {
    {BSP_AXI_SECT_TYPE_ACORE_DEEPSLEEP,   0,                                   0,                                 0},
    {BSP_AXI_SECT_TYPE_FLASH_SEM,         (MEMORY_AXI_FLASH_SEM_ADDR),         AXI_V2P(MEMORY_AXI_FLASH_SEM_ADDR),         MEMORY_AXI_FLASH_SEM_SIZE},
    {BSP_AXI_SECT_TYPE_IFC,               (MEMORY_AXI_IFC_ADDR),               AXI_V2P(MEMORY_AXI_IFC_ADDR),               MEMORY_AXI_IFC_SIZE},
    {BSP_AXI_SECT_TYPE_ICC,               (MEMORY_AXI_ICC_ADDR),               AXI_V2P(MEMORY_AXI_ICC_ADDR),               MEMORY_AXI_ICC_SIZE},
    {BSP_AXI_SECT_TYPE_MEMMGR_FLAG,       (MEMORY_AXI_MEMMGR_FLAG_ADDR),       AXI_V2P(MEMORY_AXI_MEMMGR_FLAG_ADDR),       MEMORY_AXI_MEMMGR_FLAG_SIZE},
    {BSP_AXI_SECT_TYPE_DYNAMIC,           (MEMORY_AXI_DYNAMIC_SEC_ADDR),       AXI_V2P(MEMORY_AXI_DYNAMIC_SEC_ADDR),       MEMORY_AXI_DYNAMIC_SEC_SIZE},
    {BSP_AXI_SECT_TYPE_SOFT_FLAG,         0,                                   0,                                          0},
    {BSP_AXI_SECT_TYPE_IPF,               (MEMORY_AXI_IPF_ADDR),               AXI_V2P(MEMORY_AXI_IPF_ADDR),               MEMORY_AXI_IPF_SIZE},
    {BSP_AXI_SECT_TYPE_TEMPERATURE,       (MEMORY_AXI_TEMP_PROTECT_ADDR),      AXI_V2P(MEMORY_AXI_TEMP_PROTECT_ADDR),      MEMORY_AXI_TEMP_PROTECT_SIZE},
    {BSP_AXI_SECT_TYPE_ONOFF,             (MEMORY_AXI_ONOFF_ADDR),             AXI_V2P(MEMORY_AXI_ONOFF_ADDR),             MEMORY_AXI_ONOFF_SIZE},
    {BSP_AXI_SECT_TYPE_DICC,              (MEMORY_AXI_DICC_ADDR),              AXI_V2P(MEMORY_AXI_DICC_ADDR),              MEMORY_AXI_DICC_SIZE},
    {BSP_AXI_SECT_TYPE_HIFI,              (MEMORY_AXI_HIFI_ADDR),              AXI_V2P(MEMORY_AXI_HIFI_ADDR),              MEMORY_AXI_HIFI_SIZE},
    {BSP_AXI_SECT_TYPE_PTABLE,            (MEMORY_AXI_PTABLE_ADDR),            AXI_V2P(MEMORY_AXI_PTABLE_ADDR),            MEMORY_AXI_PTABLE_SIZE},
    {BSP_AXI_SECT_TYPE_RESERVE,           0,                                   0,                                          0},
    {BSP_AXI_SECT_TYPE_DLOAD_AUTOINSTALL, (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR), AXI_V2P(MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR), MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE},
    {BSP_AXI_SECT_TYPE_DLOAD,             (0),                                 0,                                          0}
    };

#endif
/* 全局变量，记录IP基地址 */
BSP_U32 g_ulIPBaseAddr[BSP_IP_TYPE_BUTTOM] = {
    SOCP_REG_BASEADDR,
    CICOM0_REGBASE_ADDR,
    CICOM1_REGBASE_ADDR,
    HDLC_REGBASE_ADDR,
    BBP_MASTER_REGBASE_ADDR,
    ZSP_ITCM_ADDR,
    ZSP_DTCM_ADDR,
    AHB_BASE_ADDR,
    WBBP_BASE_ADDR,
    WBBP_DRX_ADDR,
    GBBP_BASE_ADDR,
    GBBP_DRX_ADDR,
    ZSP_DMAC_BASE,
    INTEGRATOR_SC_BASE,
    TDS_REGBASE_ADR,
    TDS_ZSP_DHI_ADDR
};

/* 全局变量，记录IP的中断号 */
BSP_S32 g_slIntNO[BSP_INT_TYPE_BUTTOM] = {
    INT_LVL_RTC,
    INT_LVL_WDT,
    INT_LVL_USB,
    INT_LVL_ICUSB,
    INT_LVL_IPF_C,
    INT_LVL_IPF_A,
    INT_LVL_SOCP_A,
    INT_LVL_SOCP_C,
    INT_LVL_CICOM0_ARM,
    INT_LVL_CICOM1_ARM,
    INT_LVL_GU_HDLC,
    INT_LVL_BBP_MASTER,
    INT_LVL_GBBP,
    INT_LVL_GBBP_AWAKE,
    INT_LVL_WBBP_0MS,
    INT_LVL_WBBP_AWAKE,
    INT_LVL_WBBP_CLOCK_SWITCH,
    INT_LVL_RTC_TIMER,
    INT_LVL_SMIM,
    INT_LVL_ZSP_WDT,
    INT_LVL_HIFI_WDT
};

#define SOC_ARM_VERSION_ADDR            ((INTEGRATOR_SC_BASE)+(0x0f00))
#define SOC_SC_ZSP_NMI_INT_REG          ((INTEGRATOR_SC_BASE)+(0x04d8))
#define SOC_SC_ZSP_NMI_VIC_REG          ((AHB_BASE_ADDR)+(0xC0C0))
#define W_BBP_VERSION_ADDR              ((WBBP_BASE_ADDR) + (0x0088))
#define G_BBP_VERSION_ADDR              ((GBBP_BASE_ADDR) + (0x5760))


/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo)
{
    if((BSP_NULL == pstSectQuery) || (BSP_NULL == pstSectInfo))
    {
        return BSP_ERROR;
    }

    memcpy((void *)pstSectInfo, (const void *)(&g_stDdrSectInfo[pstSectQuery->enSectType]), sizeof(BSP_DDR_SECT_INFO));

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : BSP_DDR_ShowSectInfo
 功能描述  : 打印DDR内存段信息
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_DDR_ShowSectInfo(BSP_VOID)
{
    BSP_DDR_SECT_TYPE_E     enSectTypeIndex = (BSP_DDR_SECT_TYPE_E)0;

    DRV_PRINTF("\ntype       paddr      vaddr      size       attr\n");
    for(; enSectTypeIndex < BSP_DDR_SECT_TYPE_BUTTOM; enSectTypeIndex++)
    {
        DRV_PRINTF("0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x\n\n", \
          g_stDdrSectInfo[enSectTypeIndex].enSectType, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectPhysAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectVirtAddr, \
          g_stDdrSectInfo[enSectTypeIndex].ulSectSize, \
          g_stDdrSectInfo[enSectTypeIndex].enSectAttr);
    }
}


/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo)
{
    if(BSP_NULL == pstSectInfo)
    {
        return BSP_ERROR;
    }

    memcpy((void *)pstSectInfo, (const void *)(&g_stAxiSectInfo[enSectType]), sizeof(BSP_AXI_SECT_INFO));

    return BSP_OK;
}

/*****************************************************************************
 函 数 名  : BSP_AXI_ShowSectInfo
 功能描述  : 打印AXI内存段信息
 输入参数  : 无
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
BSP_VOID BSP_AXI_ShowSectInfo(BSP_VOID)
{
    BSP_AXI_SECT_TYPE_E     enSectTypeIndex = (BSP_AXI_SECT_TYPE_E)0;

    DRV_PRINTF("\ntype       paddr      vaddr      sizer\n");
    for(; enSectTypeIndex < BSP_AXI_SECT_TYPE_BUTTOM; enSectTypeIndex++)
    {
        DRV_PRINTF("0x%-8.8x 0x%-8.8x 0x%-8.8x 0x%-8.8x\n\n", \
          g_stAxiSectInfo[enSectTypeIndex].enSectType, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectPhysAddr, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectVirtAddr, \
          g_stAxiSectInfo[enSectTypeIndex].ulSectSize);
    }
}

/*****************************************************************************
 函 数 名  : BSP_GetIPBaseAddr
 功能描述  : IP基地址查询
 输入参数  : enIPType: 需要查询的IP类型
 输出参数  : 无
 返回值    ：查询到的IP基地址，如查不到返回NULL
*****************************************************************************/
BSP_U32 BSP_GetIPBaseAddr(BSP_IP_TYPE_E enIPType)
{
    if(enIPType >= BSP_IP_TYPE_BUTTOM)
    {
        return 0;
    }

    return g_ulIPBaseAddr[enIPType];
}

/*****************************************************************************
 函 数 名  : BSP_GetIntNO
 功能描述  : 中断号查询
 输入参数  : enIntType: 需要查询的中断类型
 输出参数  : 无
 返回值    ：查询到的中断号，如查不到返回BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_GetIntNO(BSP_INT_TYPE_E enIntType)
{
    if(enIntType >= BSP_INT_TYPE_BUTTOM)
    {
        return BSP_ERROR;
    }

    return g_slIntNO[enIntType];
}

/*****************************************************************************
 函 数 名  : BSP_CheckModuleSupport
 功能描述  : 查询模块是否支持
 输入参数  : enModuleType: 需要查询的模块类型
 输出参数  : 无
 返回值    ：BSP_MODULE_SUPPORT或BSP_MODULE_UNSUPPORT
*****************************************************************************/
BSP_MODULE_SUPPORT_E BSP_CheckModuleSupport(BSP_MODULE_TYPE_E enModuleType)
{
    BSP_MODULE_SUPPORT_E enRet = BSP_MODULE_SUPPORT;

    if(enModuleType >= BSP_MODULE_TYPE_BUTTOM)
    {
        return BSP_MODULE_UNSUPPORT;
    }

    switch(enModuleType)
    {
        case BSP_MODULE_TYPE_SD:
#if defined(BOARD_FPGA) || (FEATURE_SDMMC == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;
        case BSP_MODULE_TYPE_CHARGE:
#if (FEATURE_CHARGE == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;
        case BSP_MODULE_TYPE_WIFI:
#if (FEATURE_WIFI == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;
        case BSP_MODULE_TYPE_OLED:
#if (FEATURE_OLED == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;
        case BSP_MODULE_TYPE_HIFI:
#if (FEATURE_HIFI == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;
        case BSP_MODULE_TYPE_POWER_ON_OFF:
#if (FEATURE_POWER_ON_OFF == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;

        case BSP_MODULE_TYPE_HSIC:
#if (FEATURE_HSIC_SLAVE == FEATURE_OFF)
            enRet = BSP_MODULE_UNSUPPORT;
#endif
            break;

        case BSP_MODULE_TYPE_BUTTOM:
	default:
	    break;
    }

    return enRet;
}

/************************************************************************
 * FUNCTION
 *       BSP_StartHardTimer
 * DESCRIPTION
 *       start hard timer
 * INPUTS
 *       value -- timer's value.uint is 32K cycle
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID BSP_StartHardTimer(BSP_U32 value)
{
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    BSP_REG_WRITE((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, CLK_DEF_TC_DISABLE);
    BSP_REG_WRITE((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_LOAD, value);
    BSP_REG_WRITE((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, CLK_DEF_TC_ENABLE);
#else
    BSP_REG_WRITE(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, CLK_DEF_TC_DISABLE);
    BSP_REG_WRITE(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_LOAD, value);
    BSP_REG_WRITE(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, CLK_DEF_TC_ENABLE);
#endif

    return;
}

/************************************************************************
 * FUNCTION
 *       BSP_StopHardTimer
 * DESCRIPTION
 *       Stop hard timer
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID BSP_StopHardTimer(BSP_VOID)
{
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    BSP_REG_WRITE((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, CLK_DEF_TC_DISABLE);
#else
    BSP_REG_WRITE(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, CLK_DEF_TC_DISABLE);
#endif

    return;
}

/************************************************************************
 * FUNCTION
 *       BSP_GetHardTimerCurTime
 * DESCRIPTION
 *       Get hard timer Current Value
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID)
{
    BSP_U32 readValue;

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    BSP_REG_READ((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, readValue);

    if(CLK_DEF_TC_DISABLE == readValue)
    {
        return 0;
    }

    BSP_REG_READ((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_VALUE, readValue);
#else
    BSP_REG_READ(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CTRL, readValue);

    if(CLK_DEF_TC_DISABLE == readValue)
    {
        return 0;
    }

    BSP_REG_READ(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_VALUE, readValue);
#endif

    return readValue;
}

/************************************************************************
 * FUNCTION
 *       BSP_GetHardTimerCurTime
 * DESCRIPTION
 *       Get hard timer Current Value
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID BSP_ClearTimerINT(BSP_VOID)
{
    BSP_U32 ulValue;

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    BSP_REG_READ((RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CLEAR, ulValue);
#else
    BSP_REG_READ(IO_ADDRESS(RTC_TIMER_REGBASE_ADDR), CLK_REGOFF_CLEAR, ulValue);
#endif
    /*for pclint*/
    ulValue = ulValue;

    return ;
}

/*************************内存虚实转换 start*********************************/

/*****************************************************************************
 函 数 名  : DRV_AXI_VIRT_TO_PHY
 功能描述  : AXI内虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int DRV_AXI_VIRT_TO_PHY(unsigned int ulVAddr)
{
    return ((ulVAddr - AXI_MEM_ADDR) + AXI_MEM_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : DRV_AXI_PHY_TO_VIRT
 功能描述  : AXI内实地址往虚地址转换
 输入参数  : ulVAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int DRV_AXI_PHY_TO_VIRT(unsigned int ulPAddr)
{
    return ((ulPAddr - AXI_MEM_BASE_ADDR) + AXI_MEM_ADDR);
}

/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
unsigned int DRV_DDR_VIRT_TO_PHY(unsigned int ulVAddr)
{
#ifdef PRODUCT_CFG_CORE_TYPE_APP
    if((ulVAddr >= ECS_MDDR_RESERVE_BASE_ADDR_VIRT)
        && (ulVAddr < ECS_MDDR_RESERVE_BASE_ADDR_VIRT + GLOBAL_MEM_GU_RESERVED_SIZE))
    {
        return (ulVAddr - ECS_MDDR_RESERVE_BASE_ADDR_VIRT + ECS_MDDR_RESERVE_BASE_ADDR);
    }

    if((ulVAddr >= IO_ADDRESS(GLOBAL_MEM_CORE_SHARE_ADDR))
        && (ulVAddr < IO_ADDRESS(GLOBAL_MEM_CORE_SHARE_ADDR) + GLOBAL_MEM_CORE_SHARE_SIZE))
    {
        return (ulVAddr - IO_ADDRESS(GLOBAL_MEM_CORE_SHARE_ADDR) + GLOBAL_MEM_CORE_SHARE_ADDR);
    }

    if((ulVAddr >= PBXA9_DRAM_EXC_SHARE_VIRT)
        && (ulVAddr < PBXA9_DRAM_EXC_SHARE_VIRT + PBXA9_DRAM_EXC_SHARE_SIZE))
    {
        return (ulVAddr - PBXA9_DRAM_EXC_SHARE_VIRT + PBXA9_DRAM_EXC_SHARE_PHYS);
    }

    DRV_PRINTF("DRV_DDR_VIRT_TO_PHY: ulVAddr is invalid!\n");
#endif
    return 0;
}

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int DRV_DDR_PHY_TO_VIRT(unsigned int ulPAddr)
{
#ifdef PRODUCT_CFG_CORE_TYPE_APP
    if((ulPAddr >= ECS_MDDR_RESERVE_BASE_ADDR)
        && (ulPAddr < ECS_MDDR_RESERVE_BASE_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE))
    {
        return (ulPAddr - ECS_MDDR_RESERVE_BASE_ADDR + ECS_MDDR_RESERVE_BASE_ADDR_VIRT);
    }

    if((ulPAddr >= GLOBAL_MEM_CORE_SHARE_ADDR)
        && (ulPAddr < GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE))
    {
        return (ulPAddr - GLOBAL_MEM_CORE_SHARE_ADDR + IO_ADDRESS(GLOBAL_MEM_CORE_SHARE_ADDR));
    }

    if((ulPAddr >= PBXA9_DRAM_EXC_SHARE_PHYS)
        && (ulPAddr < PBXA9_DRAM_EXC_SHARE_PHYS + PBXA9_DRAM_EXC_SHARE_SIZE))
    {
        return (ulPAddr - PBXA9_DRAM_EXC_SHARE_PHYS + PBXA9_DRAM_EXC_SHARE_VIRT);
    }

    DRV_PRINTF("DRV_DDR_VIRT_TO_PHY: ulVAddr is invalid!\n");
#endif
    return 0;
}

/*****************************************************************************
 函 数 名  : TTF_VIRT_TO_PHY
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
unsigned int TTF_VIRT_TO_PHY(unsigned int ulVAddr)
{
    return (ulVAddr - ECS_TTF_BASE_ADDR_VIRT + ECS_TTF_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : TTF_PHY_TO_VIRT
 功能描述  : TTF内存实地址往虚地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int TTF_PHY_TO_VIRT(unsigned int ulPAddr)
{
    return (ulPAddr - ECS_TTF_BASE_ADDR + ECS_TTF_BASE_ADDR_VIRT);
}

/*****************************************************************************
 函 数 名  : IPF_VIRT_TO_PHY
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
unsigned int IPF_VIRT_TO_PHY(unsigned int ulVAddr)
{
    return (ulVAddr - IPF_VIRT_BASE_ADDR + IPF_PHY_BASE_ADDR);
}

/*****************************************************************************
 函 数 名  : IPF_PHY_TO_VIRT
 功能描述  : IPF寄存器实地址往虚地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int IPF_PHY_TO_VIRT(unsigned int ulPAddr)
{
    return (ulPAddr - IPF_PHY_BASE_ADDR + IPF_VIRT_BASE_ADDR);
}

/*************************IPC BEGIN*****************************/

/*****************************************************************************
 * 函 数 名  : DRV_IPC_SEMCREATE
 *
 * 功能描述  : 信号量创建函数
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 DRV_IPC_SEMCREATE(BSP_U32 u32SignalNum)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_SemCreate(u32SignalNum);
#else
    return BSP_IPC_SemCreate(u32SignalNum);
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMDELETE
*
* 功能描述  : 删除信号量
*
* 输入参数  :   BSP_U32 u32SignalNum 要删除的信号量编号

* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 DRV_IPC_SEMDELETE(BSP_U32 u32SignalNum)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_SemDelete(u32SignalNum);
#else
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    return BSP_IPC_SemDelete(u32SignalNum);
#else
    return 0;
#endif
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTENABLE
*
* 功能描述  : 使能某个中断
*
* 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 DRV_IPC_INTENABLE (IPC_INT_LEV_E ulLvl)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_IntEnable(ulLvl);
#else
    return BSP_IPC_IntEnable(ulLvl);
#endif
}

/*****************************************************************************
 * 函 数 名  : DRV_IPC_INTDISABLE
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 DRV_IPC_INTDISABLE (IPC_INT_LEV_E ulLvl)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_IntDisable(ulLvl);
#else
    return BSP_IPC_IntDisable(ulLvl);
#endif
}

/*****************************************************************************
 * 函 数 名  : DRV_IPC_INTCONNECT
 *
 * 功能描述  : 注册某个中断
 *
 * 输入参数  :
               BSP_U32 ulLvl 要使能的中断号，取值范围0～31
               VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 DRV_IPC_INTCONNECT  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_IntConnect( ulLvl, routine, parameter);
#else
    return BSP_IPC_IntConnect(ulLvl, routine, parameter);
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTSEND
*
* 功能描述  : 发送中断
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 DRV_IPC_INTSEND(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_IntSend(enDstCore, ulLvl);
#else
    return BSP_IPC_IntSend(enDstCore, ulLvl);
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMTAKE
*
* 功能描述  : 获取信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32  DRV_IPC_SEMTAKE(BSP_U32 u32SignalNum, BSP_S32 s32timeout)
{
#ifdef BOARD_FPGA_P500
    return BSP_GUIPC_SemTake(u32SignalNum, s32timeout);
#else
    return BSP_IPC_SemTake(u32SignalNum, s32timeout);
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMGIVE
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID DRV_IPC_SEMGIVE(BSP_U32 u32SignalNum)
{
#ifdef BOARD_FPGA_P500
    BSP_GUIPC_SemGive(u32SignalNum);
#else
    BSP_IPC_SemGive(u32SignalNum);
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_SPIN_LOCK
*
* 功能描述  : 获取信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID DRV_SPIN_LOCK (BSP_U32 u32SignalNum)
{
#ifdef BOARD_FPGA_P500
    BSP_GUIPC_SpinLock(u32SignalNum);
#else
    BSP_IPC_SpinLock (u32SignalNum);
#endif
}

/*****************************************************************************
* 函 数 名  : DRV_SPIN_UNLOCK
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID DRV_SPIN_UNLOCK(BSP_U32 u32SignalNum)
{
#ifdef BOARD_FPGA_P500
    BSP_GUIPC_SpinUnLock(u32SignalNum);
#else
    BSP_IPC_SpinUnLock(u32SignalNum);
#endif
}


/****************************内存虚实装好 start******************************/

/*****************************************************************************
 函 数 名  : DRV_SOCP_SLEEPIN
 功能描述  : SOCP进入睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
BSP_U32 DRV_SOCP_SLEEPIN(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum)
{
#if defined (VERSION_V3R2)
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#ifdef BSP_COMPILE_ALLY
    return BSP_SOCP_SleepIn(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
#else
    return 0;
#endif
#else
#if defined (BALONG_COMPILE_SEPATARE)
    return 0;
#else
    return BSP_SOCP_SleepIn(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
#endif
#endif
#else
    return 0;
#endif
}

/*****************************************************************************
 函 数 名  : DRV_SOCP_SLEEPOUT
 功能描述  : SOCP退出睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
BSP_U32 DRV_SOCP_SLEEPOUT(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum)
{
#if defined (VERSION_V3R2)
#if defined (BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#ifdef BSP_COMPILE_ALLY
    return BSP_SOCP_SleepOut(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
#else
    return 0;
#endif
#else
#if defined (BALONG_COMPILE_SEPATARE)
    return 0;
#else
    return BSP_SOCP_SleepOut(pu32SrcChanID, uSrcChanNum, pu32DstChanID, uDstChanNum);
#endif
#endif
#else
    return 0;
#endif
}

/*****************************************************************************
Function:   DRV_PWRCTRL_SLEEP_IN_CB
Description:
Input:
Output:     None;
Return:
Others:
*****************************************************************************/
int DRV_PWRCTRL_SLEEP_IN_CB(PWC_DS_SOCP_CB_STRU stFunc)
{
#if defined (VERSION_V3R2)
    return BSP_PWRCTRL_SleepInSocCB(stFunc);
#else
    return 0;
#endif
}

#if defined(FEATURE_WIFI_ON)
extern int wifi_get_sta_num(void); //clean warning
#endif

/*****************************************************************************
 函 数 名  : DRV_AT_GET_USER_EXIST_FLAG
 功能描述  : 返回当前是否有USB连接或者WIFI用户连接
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
unsigned long DRV_AT_GET_USER_EXIST_FLAG(void)
{
#if defined(FEATURE_WIFI_ON)
    int cnt = 0;
    cnt = wifi_get_sta_num();
    if(cnt >= 1)
    {
        return 1;
    }
#endif

#if (FEATURE_OFF == FEATURE_ECM_RNDIS)
    /* E5形态按照状态返回 */
    if (USB_ETH_LinkStatGet())
    {
        return 1;
    }
#else
    /* USB STICK 形态总返回1 */
    return 1;
#endif

    return 0;
}

/***************************OLED start*******************/
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
BSP_VOID DRV_OLED_CLEAR_WHOLE_SCREEN(BSP_VOID)
{
#if( FEATURE_OLED == FEATURE_ON )
    oledClearWholeScreen();
#else
    DRV_PRINTF("Function DRV_OLED_CLEAR_WHOLE_SCREEN unsupport!\n");
#endif
}

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
void DRV_OLED_UPDATE_STATE_DISPLAY(int UpdateStatus)
{
#if( FEATURE_OLED == FEATURE_ON )
    oledUpdateDisplay(UpdateStatus);
#else
    DRV_PRINTF("Function DRV_OLED_UPDATE_STATE_DISPLAY unsupport!\n");
#endif
}

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_DISPLAY
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
void DRV_OLED_UPDATE_DISPLAY(int UpdateStatus)
{
#if( FEATURE_OLED == FEATURE_ON )
    oledUpdateDisplay(UpdateStatus);
#else
    DRV_PRINTF("Function DRV_OLED_UPDATE_DISPLAY unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : DRV_OLED_STRING_DISPLAY
 功能描述  : oled字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
BSP_VOID DRV_OLED_STRING_DISPLAY(UINT8 ucX, UINT8 ucY, UINT8 *pucStr)
{
#if( FEATURE_OLED == FEATURE_ON )
    oledStringDisplay(ucX,ucY,pucStr);
#else
    DRV_PRINTF("Function DRV_OLED_STRING_DISPLAYs unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : DRV_OLED_POWER_OFF
 功能描述  : oled下电函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
BSP_VOID DRV_OLED_POWER_OFF(BSP_VOID)
{
#if( FEATURE_OLED == FEATURE_ON )
    oledPwrOff();
#else
    DRV_PRINTF("Function DRV_OLED_POWER_OFF unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_LOCK
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
BSP_U32 DRV_PWRCTRL_SLEEPVOTE_LOCK(PWC_CLIENT_ID_E enClientId)
{
#if (defined(BOARD_ASIC) || defined(BOARD_ASIC_BIGPACK) || defined(BOARD_SFT))
    return BSP_PWRCTRL_SleepVoteLock(enClientId);
#else
    return 0;
#endif
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
BSP_U32 DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWC_CLIENT_ID_E enClientId)
{
#if (defined(BOARD_ASIC) || defined(BOARD_ASIC_BIGPACK) || defined(BOARD_SFT))
    return BSP_PWRCTRL_SleepVoteUnLock(enClientId);
#else
    return 0;
#endif
}

/* A核独有的模块 */

/*****************************************************************************
 函 数 名  : WIFI_TEST_CMD
 功能描述  : 测试命令
 输入参数  : cmdStr；命令字符串
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
void WIFI_TEST_CMD(char * cmdStr)
{
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_tcmd(cmdStr);
#else
    DRV_PRINTF("Function WIFI_TEST_CMD unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_TCMD_MODE
 功能描述  : 获取测试命令的模式
 输入参数  : 无
 输出参数  : 16：校准测试模式
             17：表示处于发射模式
             18：表示接收模式
 返回值    ：函数执行的状态结果值
*****************************************************************************/
int WIFI_GET_TCMD_MODE(void)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_get_tcmd_mode();
#else
    DRV_PRINTF("Function WIFI_GET_TCMD_MODE unsupport!\n");
    return 0;
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_POWER_START
 功能描述  : WIFI上电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
int WIFI_POWER_START(void)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_power_on();
#else
    DRV_PRINTF("Function WIFI_POWER_START unsupport!\n");
    return 1;
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_POWER_SHUTDOWN
 功能描述  : WIFI下电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
int WIFI_POWER_SHUTDOWN(void)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_power_off();
#else
    DRV_PRINTF("Function WIFI_POWER_SHUTDOWN unsupport!\n");
    return 1;
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_STATUS
 功能描述  : WIFI状态获取
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
int WIFI_GET_STATUS(void)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_get_status();
#else
    DRV_PRINTF("Function WIFI_GET_STATUS unsupport!\n");
    return 0;
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_DETAIL_REPORT
 功能描述  : get result of rx report: totalPkt, GoodPkt, ErrorPkt
 输入参数  : 无
 输出参数  : totalPkt、goodPkt、badPkt
 返回值    ：无
*****************************************************************************/
void WIFI_GET_RX_DETAIL_REPORT(int* totalPkt,int* goodPkt,int* badPkt)
{
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_get_rx_detail_report(totalPkt, goodPkt, badPkt);
#else
    DRV_PRINTF("Function WIFI_GET_RX_DATAIL_REPORT unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_PACKET_REPORT
 功能描述  : get result of rx ucast&mcast packets
 输入参数  : 无
 输出参数  : ucastPkts、mcastPkts
 返回值    ：无
*****************************************************************************/
void WIFI_GET_RX_PACKET_REPORT(unsigned int *ucastPkts, unsigned int *mcastPkts)
{
#if (FEATURE_WIFI == FEATURE_ON)
    wifi_get_rx_packet_report(ucastPkts, mcastPkts);
#else
    DRV_PRINTF("Function WIFI_GET_RX_PACKET_REPORT unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_CUR_MODE
 功能描述  : get the currrent PA mode of the wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
             -1: wifi chip is in abnormal mode
*****************************************************************************/
int WIFI_GET_PA_CUR_MODE(void)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return wifi_get_pa_cur_mode();
#else
    DRV_PRINTF("Function WIFI_GET_PA_CUR_MODE unsupport!\n");
    return -1;
#endif
}


/*****************************************************************************
 函 数 名  : WIFI_GET_PA_MODE
 功能描述  : get the support PA mode of wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
             -1: failed
*****************************************************************************/
int WIFI_GET_PA_MODE(void)
{
#if (FEATURE_WIFI == FEATURE_ON)
//    wifi_get_pa_mode();
    DRV_PRINTF("Function WIFI_GET_PA_MODE unsupport!\n");
    return -1;
#else
    DRV_PRINTF("Function WIFI_GET_PA_MODE unsupport!\n");
    return -1;
#endif
}

/*****************************************************************************
 函 数 名  : WIFI_SET_PA_MODE
 功能描述  : set the PA mode of wifi chip
 输入参数  : 0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
 输出参数  : 无
 返回值    ：0: success
             -1: failed
*****************************************************************************/
int WIFI_SET_PA_MODE(int wifiPaMode)
{
#if (FEATURE_WIFI == FEATURE_ON)
    
    return wifi_set_pa_mode(wifiPaMode);
#else
    DRV_PRINTF("Function WIFI_SET_PA_MODE unsupport!\n");
    return -1;
#endif
}

/*****************************************************************************
 函 数 名  : DRV_WIFI_DATA_RESERVED_TAIL
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 数据区的尾部预留的长度
*****************************************************************************/
unsigned int DRV_WIFI_DATA_RESERVED_TAIL(unsigned int len)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return BSP_WifiDataReservedTail(len);
#else
    DRV_PRINTF("Function DRV_WIFI_DATA_RESERVED_TAIL unsupport!\n");
    return 0;
#endif
}


/*****************************************************************************
 函 数 名  : WiFi_DrvSetRxFlowCtrl
 功能描述  : 设置WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : BSP_OK/BSP_ERROR
*****************************************************************************/
unsigned long DRV_WIFI_SET_RX_FCTL(unsigned long para1, unsigned long para2)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return WiFi_DrvSetRxFlowCtrl(para1, para2);
#else
    DRV_PRINTF("Function DRV_WIFI_SET_RX_FCTL unsupport!\n");
    return (unsigned long)BSP_ERROR;
#endif
}

/*****************************************************************************
 函 数 名  : WiFi_DrvSetRxFlowCtrl
 功能描述  : 清除WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
unsigned long DRV_WIFI_CLR_RX_FCTL(unsigned long para1, unsigned long para2)
{
#if (FEATURE_WIFI == FEATURE_ON)
    return WiFi_DrvClearRxFlowCtrl(para1, para2);
#else
    DRV_PRINTF("Function DRV_WIFI_CLR_RX_FCL unsupport!\n");
    return (unsigned long)BSP_ERROR;
#endif
}

/********************************WIFI end***********************************/

/********************************SD start***********************************/
/*****************************************************************************
* 函 数 名  : DRV_SD_GET_STATUS
* 功能描述  : 卡在位查询
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0 : 在位；-1: 不在位
*****************************************************************************/
int DRV_SD_GET_STATUS(void)
{
#if defined(BOARD_FPGA)
    DRV_PRINTF("Function DRV_SD_GET_STATUS unsupport!\n");
    return -1;
#else
    return sd_get_status();
#endif

}

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_CAPACITY
* 功能描述  : 卡容量查询
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0 : 失败；>0: 卡容量
*****************************************************************************/
int DRV_SD_GET_CAPACITY(void)
{
#if defined(BOARD_FPGA)
    DRV_PRINTF("Function DRV_SD_GET_CAPACITY unsupport!\n");
    return 0;
#else
    return sd_get_capacity();
#endif

}

/*****************************************************************************
* 函 数 名  : DRV_SD_TRANSFER
* 功能描述  : 数据传输
* 输入参数  : struct scatterlist *sg    待传输数据结构体指针
                            unsigned dev_addr   待写入的SD block 地址
                            unsigned blocks    待写入的block个数
                            unsigned blksz      每个block的大小，单位字节
                            int wrflags    读写标志位，写:WRFlAG ; 读:RDFlAG
* 输出参数  : 无
*
* 返 回 值  : 0 : 成功；其它: 失败
*****************************************************************************/
int DRV_SD_TRANSFER(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags)
{
#if defined(BOARD_FPGA)
    DRV_PRINTF("Function DRV_SD_TRANSFER unsupport!\n");
    return 1;
#else
    return sd_transfer(sg, dev_addr, blocks, blksz, wrflags);
#endif

}

/********************************SD end*************************************/

/*****************************CHARGE START**********************************/

/*****************************************************************************
 函 数 名  : BSP_CHG_ChargingStatus
 功能描述  :查询目前是否正在充电中
 输入参数  :无
 输出参数  :无
 返回值：   0 未充电
                          1 充电中
*****************************************************************************/
int DRV_CHG_GET_CHARGING_STATUS(void)
{
#if( FEATURE_CHARGE == FEATURE_ON )
    return BSP_CHG_ChargingStatus();
#else
    DRV_PRINTF("Function DRV_CHG_GET_CHARGING_STATUS unsupport!\n");
    return 0;
#endif
}
/*****************************************************************************
* 函 数 名  : DRV_SD_SG_INIT_TABLE
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  : const void *buf		待操作的buffer地址
				  unsigned int buflen	待操作的buffer大小，小于32K, 大小为512B的整数倍
				  					大于32K, 大小为32KB的整数倍，最大buffer为128K
* 输出参数  : NA
*
* 返 回 值  :  0 : 成功;  其它:失败
* 其它说明  : NA
*
*****************************************************************************/
int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen)
{
 #if (FEATURE_SDMMC == FEATURE_OFF)
	return -1;
 #else 
   return sd_sg_init_table(buf,buflen);
 #endif
}
/*****************************************************************************
* 函 数 名  : DRV_SD_MULTI_TRANSFER
*
* 功能描述  : SD多块数据传输
*
* 输入参数  : unsigned dev_addr	待写入的SD block地址
				  unsigned blocks		待写入的block 个数
				  unsigned blksz		每个block 的大小，单位字节
				  int write			读写标志位，写:1;	读:0
* 输出参数  : NA
*
* 返 回 值  :  0 : 成功;  其它:失败
* 其它说明  : NA
*
*****************************************************************************/
int DRV_SD_MULTI_TRANSFER(unsigned dev_addr,unsigned blocks,unsigned blksz,int write)
{
   #if (FEATURE_SDMMC == FEATURE_OFF)
	return -1;
  #else
      return sd_multi_transfer(dev_addr,blocks,blksz,write); 
  #endif
    
}

/*****************************************************************************
 函 数 名  : DRV_CHG_STATE_SET
 功能描述  : 使能或者禁止充电
 输入参数  : ulState    0:禁止充电
 						1:使能充电
 输出参数  : 无
 返回值：    无
*****************************************************************************/
BSP_VOID  DRV_CHG_STATE_SET(unsigned long ulState)
{
#if( FEATURE_CHARGE == FEATURE_ON )
    BSP_CHG_StateSet(ulState);
#else
    DRV_PRINTF("Function DRV_CHG_STATE_SET unsupport!\n");
#endif
}

/*****************************************************************************
 函 数 名  : DRV_CHG_GET_BATTERY_STATE
 功能描述  : 获取底层电池状态信息
 输入参数  : battery_state 电量信息
 输出参数  : battery_state 电量信息
 返回值：    0 操作成功
             -1操作失败
*****************************************************************************/
int DRV_CHG_GET_BATTERY_STATE(BATT_STATE_T *bttery_state)
{
#if( FEATURE_CHARGE == FEATURE_ON )
    return BSP_CHG_GetBatteryState(bttery_state); // clean warning
#else
    DRV_PRINTF("Function DRV_CHG_GET_BATTERY_STATE unsupport!\n");
    return -1;
#endif
}

/*****************************************************************************
 函 数 名  : BSP_CHG_GetCbcState
 功能描述  : 返回电池状态和电量
 输入参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
             pucBcl  0:电量不足或没有与电池连接
 输出参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
             pucBcl  0:电量不足或没有与电池连接
 返回值：    0 操作成功
             -1操作失败
*****************************************************************************/
int DRV_CHG_GET_CBC_STATE(unsigned char *pusBcs,unsigned char *pucBcl)
{
#if( FEATURE_CHARGE == FEATURE_ON )
  //  BSP_CHG_GetCbcState(pusBcs, pucBcl); // clean warning
    return BSP_CHG_GetCbcState(pusBcs, pucBcl);  // clean warning
#else
    DRV_PRINTF("Function DRV_CHG_GET_CBC_STATE unsupport!\n");
    return -1;
#endif
}


/*****************************CHARGE END************************************/

/*****************************闪电卡相关START*******************************/
/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       闪电卡版本控制应用进行拨号或断开拨号连接
 * INPUTS
 *       进行拨号或断开拨号指示
 * OUTPUTS
 *       NONE
 *************************************************************************/
VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus)
{
#if( FEATURE_HILINK == FEATURE_ON )
    rndis_app_event_dispatch(ulStatus);
#else
    DRV_PRINTF("Function DRV_AT_SETAPPDAILMODE unsupport!\n");
#endif
}

/*****************************闪电卡相关END**********************************/

/********************************************************************************************************************
 函 数 名  : BSP_GetSliceValue
 功能描述  : 获取时间戳Timer的当前值
 输出参数  :
 返 回 值  :
********************************************************************************************************************/
unsigned int BSP_GetSliceValue(void)
{
    unsigned int ulSliceValue;

#if (defined (BOARD_ASIC)||defined (BOARD_SFT))
    ulSliceValue = *(volatile unsigned int *)(IO_ADDRESS(0x90003028) + 0x4);
#elif (defined (BOARD_FPGA_P500))
    ulSliceValue = *(volatile unsigned int *)(IO_ADDRESS(0x5F06B000) + 0x20 + 0x4);
#else
    #error "unknown product macro"
#endif

    return ulSliceValue;
}

/*****************************************************************************
* 函 数 名  : DRV_MSP_PROC_REG
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc)
{
#if defined (VERSION_V3R2)
    BSP_MspProcReg(eFuncID, pFunc);
#else

#endif
}

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_USB_LOWPOWER_ENTER
 功能描述  : USB进入低功耗接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
void DRV_PWRCTRL_USB_LOWPOWER_ENTER(void)
{
#if defined (VERSION_V3R2)
    BSP_PWRCTRL_UsbLowPowerEnter();
#else

#endif
    return;
}


/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_USB_LOWPOWER_EXIT
 功能描述  : USB退出低功耗接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
void DRV_PWRCTRL_USB_LOWPOWER_EXIT(void)
{
#if defined (VERSION_V3R2)
    BSP_PWRCTRL_UsbLowPowerExit();
#else

#endif
    return;
}


/*****************************************************************************
* 函 数 名  : getHwVersionIndex
* 功能描述  : 获取硬件版本号索引
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 
*****************************************************************************/
int getHwVersionIndex(void)
{
    return (int)BSP_HwGetVerMain();
}

#if (FEATURE_SDMMC == FEATURE_OFF)
/*add SD Card stub inteface, they are used when SD module is canceled*/
#ifndef CONFIG_MMC
/*****************************************************************************
 函 数 名  : BSP_SDMMC_GetSDStatus
 功能描述  : 返回SD卡在位状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   1在位
            0不在位

*****************************************************************************/
int BSP_SDMMC_GetSDStatus(void)
{
	return 0;
}
EXPORT_SYMBOL(BSP_SDMMC_GetSDStatus);
/*****************************************************************************                                           
 函 数 名  : BSP_SDMMC_GetOprtStatus                                                                                          
 功能描述  : at^sd,SD卡当前操作状态                                                                                      
 输入参数  : 无                                                                                                          
 输出参数  : 无                                                                                                          
 返 回 值  :                                                                                                             
            0: 未操作或操作已经完成;                                                                                     
            1: 操作中 
            2: 上次操作失败
*****************************************************************************/ 
unsigned long BSP_SDMMC_GetOprtStatus(void)
{
	return 0;
}
EXPORT_SYMBOL(BSP_SDMMC_GetOprtStatus);
/*****************************************************************************                                           
 函 数 名  : BSP_SDMMC_ATProcess                                                                                             
 功能描述  : at^sd,SD卡操作，写，擦除，格式化操作                                                                        
             0:                                                                                                          
 输入参数  : 操作类型 ulOp:                                                                                              
            0  格式化SD卡内                                                                                              
            1  擦除整个SD卡内容；                                                                                        
            2  用于指定地址内容的擦除操作，指定擦除的内容长度为512字节。擦除后的地址中写全1                              
            3  写数据到SD卡的指定地址中，需要带第二个和第三个参数                                                        
            4  读数据到SD卡的指定地址中
            
            ulAddr < address >  地址，以512BYTE为一个单位，用数字n表示                                                   
                                                                                                                         
            ulData                                                                                                       
             < data >            数据内容，表示512BYTE的内容，每个字节的内容均相同。                                     
             0       字节内容为0x00                                                                                      
             1       字节内容为0x55                                                                                      
             2       字节内容为0xAA                                                                                      
             3       字节内容为0xFF                                                                                      
                                                                                                                         
 输出参数  : pulErr                                                                                                      
 返 回 值  : 0 ：OK  非 0 ：Error                                                                                        
                                                                                                                         
            具体的错误值填充在*pulErr中                                                                                  
            0 表示SD卡不在位                                                                                             
            1 表示SD卡初始化失败                                                                                         
            2 表示<opr>参数非法，对应操作不支持(该错误由AT使用,不需要底软使用)                                           
            3 表示<address>地址非法，超过SD卡本身容量                                                                    
            4 其他未知错误                                                                                               
*****************************************************************************/   
unsigned long BSP_SDMMC_ATProcess(unsigned int ulOp,unsigned long ulAddr, unsigned long ulData,u8 *pucBuffer,unsigned long *pulErr)
{
	*pulErr = 0;
	return -1;
}
EXPORT_SYMBOL(BSP_SDMMC_ATProcess);
/*****************************************************************************
* 函 数 名  : sd_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_status()
{
    return -1;
}
EXPORT_SYMBOL(sd_get_status);
/*****************************************************************************
* 函 数 名  : sd_get_capacity
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_capacity(void)
{
    return 0;
}
EXPORT_SYMBOL(sd_get_capacity);
/*****************************************************************************
* 函 数 名  : sd_transfer
*
* 功能描述  : 数据传输
*
* 输入参数  : struct scatterlist *sg    待传输数据结构体指针
                            unsigned dev_addr   待写入的SD block 地址
                            unsigned blocks    待写入的block个数
                            unsigned blksz      每个block的大小，单位字节
                            int wrflags    读写标志位，写:WRFlAG ; 读:RDFlAG
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_transfer(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags)
{
    return -1;
}
EXPORT_SYMBOL(sd_transfer);
/*****************************************************************************
* 函 数 名  : sd_get_status_for_usb(void)
*
* 功能描述  : 查询卡是否在升级
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
int sd_get_status_for_usb(void)
{
    return -1;
}
EXPORT_SYMBOL(sd_get_status_for_usb);
#endif

#ifndef CONFIG_MMC_BLOCK
int mmc_get_disk_devidx(void)
{
    return 0;
}
EXPORT_SYMBOL(mmc_get_disk_devidx);

#endif
#endif
EXPORT_SYMBOL(BSP_DDR_GetSectInfo);
EXPORT_SYMBOL(BSP_DDR_ShowSectInfo);
EXPORT_SYMBOL(BSP_AXI_GetSectInfo);
EXPORT_SYMBOL(BSP_AXI_ShowSectInfo);
EXPORT_SYMBOL(BSP_GetIPBaseAddr);
EXPORT_SYMBOL(BSP_GetIntNO);
EXPORT_SYMBOL(BSP_CheckModuleSupport);
EXPORT_SYMBOL(BSP_StartHardTimer);
EXPORT_SYMBOL(BSP_StopHardTimer);
EXPORT_SYMBOL(BSP_GetHardTimerCurTime);
EXPORT_SYMBOL(BSP_ClearTimerINT);
EXPORT_SYMBOL(DRV_AXI_VIRT_TO_PHY);
EXPORT_SYMBOL(DRV_AXI_PHY_TO_VIRT);
EXPORT_SYMBOL(DRV_DDR_VIRT_TO_PHY);
EXPORT_SYMBOL(DRV_DDR_PHY_TO_VIRT);
EXPORT_SYMBOL(TTF_VIRT_TO_PHY);
EXPORT_SYMBOL(TTF_PHY_TO_VIRT);
EXPORT_SYMBOL(IPF_VIRT_TO_PHY);
EXPORT_SYMBOL(IPF_PHY_TO_VIRT);
EXPORT_SYMBOL(DRV_IPC_SEMCREATE);
EXPORT_SYMBOL(DRV_IPC_SEMDELETE);
EXPORT_SYMBOL(DRV_IPC_INTENABLE);
EXPORT_SYMBOL(DRV_IPC_INTDISABLE);
EXPORT_SYMBOL(DRV_IPC_INTCONNECT);
EXPORT_SYMBOL(DRV_IPC_INTSEND);
EXPORT_SYMBOL(DRV_IPC_SEMTAKE);
EXPORT_SYMBOL(DRV_IPC_SEMGIVE);
EXPORT_SYMBOL(DRV_SPIN_LOCK);
EXPORT_SYMBOL(DRV_SPIN_UNLOCK);
EXPORT_SYMBOL(DRV_SOCP_SLEEPIN);
EXPORT_SYMBOL(DRV_SOCP_SLEEPOUT);
EXPORT_SYMBOL(DRV_PWRCTRL_SLEEP_IN_CB);
EXPORT_SYMBOL(DRV_AT_GET_USER_EXIST_FLAG);
EXPORT_SYMBOL(DRV_OLED_CLEAR_WHOLE_SCREEN);
EXPORT_SYMBOL(DRV_OLED_UPDATE_STATE_DISPLAY);
EXPORT_SYMBOL(DRV_OLED_UPDATE_DISPLAY);
EXPORT_SYMBOL(DRV_OLED_STRING_DISPLAY);
EXPORT_SYMBOL(DRV_OLED_POWER_OFF);
EXPORT_SYMBOL(DRV_PWRCTRL_SLEEPVOTE_LOCK);
EXPORT_SYMBOL(DRV_PWRCTRL_SLEEPVOTE_UNLOCK);
EXPORT_SYMBOL(WIFI_TEST_CMD);
EXPORT_SYMBOL(WIFI_GET_TCMD_MODE);
EXPORT_SYMBOL(WIFI_POWER_START);
EXPORT_SYMBOL(WIFI_POWER_SHUTDOWN);
EXPORT_SYMBOL(WIFI_GET_STATUS);
EXPORT_SYMBOL(WIFI_GET_RX_DETAIL_REPORT);
EXPORT_SYMBOL(WIFI_GET_RX_PACKET_REPORT);
EXPORT_SYMBOL(WIFI_GET_PA_CUR_MODE);
EXPORT_SYMBOL(WIFI_GET_PA_MODE);
EXPORT_SYMBOL(WIFI_SET_PA_MODE);
EXPORT_SYMBOL(DRV_WIFI_DATA_RESERVED_TAIL);
EXPORT_SYMBOL(DRV_WIFI_SET_RX_FCTL);
EXPORT_SYMBOL(DRV_WIFI_CLR_RX_FCTL);
EXPORT_SYMBOL(DRV_SD_GET_STATUS);
EXPORT_SYMBOL(DRV_SD_GET_CAPACITY);
EXPORT_SYMBOL(DRV_SD_TRANSFER);
EXPORT_SYMBOL(DRV_SD_SG_INIT_TABLE);
EXPORT_SYMBOL(DRV_SD_MULTI_TRANSFER);
EXPORT_SYMBOL(DRV_CHG_GET_CHARGING_STATUS);
EXPORT_SYMBOL(DRV_CHG_STATE_SET);
EXPORT_SYMBOL(DRV_CHG_GET_BATTERY_STATE);
EXPORT_SYMBOL(DRV_CHG_GET_CBC_STATE);
EXPORT_SYMBOL(DRV_AT_SETAPPDAILMODE);
EXPORT_SYMBOL(BSP_GetSliceValue);
EXPORT_SYMBOL(DRV_MSP_PROC_REG);
EXPORT_SYMBOL(DRV_PWRCTRL_USB_LOWPOWER_ENTER);
EXPORT_SYMBOL(DRV_PWRCTRL_USB_LOWPOWER_EXIT);
EXPORT_SYMBOL(getHwVersionIndex);
#if (FEATURE_SDMMC == FEATURE_OFF)
static int __init hi_hw_adapt_init(void)
{
	/*Just for EXPORT_SYMBOLS*/
	return 0;
}
static void __exit hi_hw_adapt_exit(void)
{
	/*Just for EXPORT_SYMBOLS*/
}
module_init(hi_hw_adapt_init);
module_exit(hi_hw_adapt_exit);
MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("Just for bsp hardware adapter export symbol");
MODULE_LICENSE("GPL");
#endif
