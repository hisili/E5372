/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  huawei_info.c
*
*   ��    �� :  ����
*
*   ��    �� :  Ӳ���汾��Ϣ
*
*   �޸ļ�¼ :  2011��11��12��  v1.00  ����  ����
*
*************************************************************************/
/**********************���ⵥ�޸ļ�¼**********************************************************
��    ��              �޸���                     ���ⵥ��                   �޸�����
********************************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/string.h>
#include <mach/common/bsp_version.h>
#include <mach/common/bsp_memory.h>
#include "BSP.h"
#include <linux/slab.h>

#include "huawei_info.h"
#include "ptable_def.h"

/*--------------------------------------------------------------*
 * �궨��                                                       *
 *--------------------------------------------------------------*/

PRODUCT_INFO_T g_huaweiProductInfo = {0};                        /*ȫ�ֱ����������Ʒ��Ϣ*/

const PRODUCT_INFO_T huaweiProduct[] = 
{
    /*��Ʒ��˵��������������������*/
    /*Ӳ���汾����ֵ(��汾��1+��汾��2,���ֲ�ͬ��Ʒ)���Ӱ汾��,
    �ڲ���Ʒ�����ڲ���Ʒ��PLUS��Ӳ���汾����������ʹ�õ����ƣ��ⲿ��Ʒ����
    ��Ʒ֧�ֵ�Ƶ�Σ�
    ��Ʒ֧�ֵķּ�(��δʹ��),
    �����ֶ�
    */
    
    /*1.���԰�*/
    {HW_VER_PRODUCT_UDP, 0,\
    "UDP", "\0", "UDP", "MPWUDP", "UDP",\
    {RF_BAND_ALL, RF_BAND_ALL, RF_BAND_ALL}, \
    {0}, \
    {0}
    },
#if (defined(CHIP_BB_6920CS) && ((FEATURE_EPAD==FEATURE_ON)||(FEATURE_PHONE ==FEATURE_ON)))
    /*ΪEPAD��ʱ�޸ģ�������Ҫ����ʵ�ʵ���Ҫ�����޸ģ�
      ͬʱӲ���汾����Ҫ��DrvInterface.h���ú궨��*/
      
    {HW_VER_PRODUCT_S10_TEMP, 0,\
    "S10-Temp", "\0", "SH2101UM", "H69DUSAVA", "S10",\
    {RF_BAND_ALL, RF_BAND_ALL, RF_BAND_ALL}, \
    {0}, \
    {0}
    },

    {HW_VER_PRODUCT_S10_101U, 0,\
    "S10-101U", "\0", "SH2101UM", "H69DUSAVA", "S10",\
    {RF_WCDMA_2100 | RF_WCDMA_900 | RF_GSM_ALL, \
      RF_BAND_ALL, \
      RF_BAND_ALL \
    }, \
    {0}, \
    {0}
    },

    {HW_VER_PRODUCT_S10_102U, 0,\
    "S10-102U", "\0", "SH2101UM", "H69DUSAVA", "S10",\
    {RF_WCDMA_2100 | RF_WCDMA_850 | RF_WCDMA_1900 |RF_GSM_ALL, \
      RF_BAND_ALL, \
      RF_BAND_ALL \
    }, \
    {0}, \
    {0}
    },

    {HW_VER_PRODUCT_S10_101L, 0,\
    "S10-101L", "\0", "SH2101UM", "H69DUSAVA", "S10",\
    {RF_BAND_ALL, RF_BAND_ALL, RF_BAND_ALL}, \
    {0}, \
    {0}
    },

    {HW_VER_PRODUCT_S10_MODEM, 0,\
    "S10-Modem", "\0", "SH2101UM", "H69DUSAVA", "S10",\
    {RF_BAND_ALL, RF_BAND_ALL, RF_BAND_ALL}, \
    {0}, \
    {0}
    },
	
     /*ΪEPAD��ʱ�޸ģ�������Ҫ����ʵ�ʵ���Ҫ�����޸ģ�
      ͬʱӲ���汾����Ҫ��DrvInterface.h���ú궨��*/
#endif      
#if defined(CHIP_BB_6920ES)
    /* 2.���ݿ� */
    {HW_VER_PRODUCT_E392S_U, 0,\
    "E392s", "\0", "CH1E392DM", "CH1E392DM", "E392",\
    {
        RF_WCDMA_2100 | RF_WCDMA_1800 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B8 | RF_LTE_B20,\
        RF_LTE_B38
    },
    {0}, \
    {0}\
    },
    
    {HW_VER_PRODUCT_E3276S_150, 0,\
    "E3276s-150", "\0", "CH1E3276SM", "CH1E3276SM", "E3276",\
    {
        RF_WCDMA_2100 | RF_WCDMA_1900 | RF_WCDMA_1700 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B20,\
        0
    },
    {0}, \
    {0}\
    },
    
    /* E3276s-200��ʵ��*/
    
    /* 3.E5 */
    {HW_VER_PRODUCT_E5_SBM, 0,\
    "E5776s", "\0", "CL1E5776SF", "CL1E5776SF", "102HW",\
	{
		RF_WCDMA_2100 | RF_WCDMA_J1500,	\
		0,\
		RF_LTE_B64\
	}, \
    {0}, \
    {0}
    },
    {HW_VER_PRODUCT_E5776_EM, 0,\
    "E5776s-71", "\0", "CL1E5776SS", "CL1E5776SS", "GL04P",\
    {
        RF_WCDMA_2100 | RF_WCDMA_J1700,    \
        RF_LTE_B3,\
        0\
    }, \
    {0}, \
    {0}\
    },
#else
    /* 2.���ݿ� */
    {HW_VER_PRODUCT_E392S_U, 0,\
    "E392s", "\0", "CH1E392DM", "B710S0", "E392",\
    {
        RF_WCDMA_2100 | RF_WCDMA_1800 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B8 | RF_LTE_B20,\
        RF_LTE_B38
    },
    {0}, \
    {0}\
    },

    {HW_VER_PRODUCT_E3276S_150, 0,\
    "E3276s-150", "\0", "CH1E3276SM", "B710S0", "E3276",\
    {
        RF_WCDMA_2100 | RF_WCDMA_1900 | RF_WCDMA_1700 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B20,\
        0
    },
    {0}, \
    {0}\
    },
    
    /* E3276s-200��ʵ��*/
    
    /* 3.E5 */
    {HW_VER_PRODUCT_E5_SBM, 0,\
    "E5776s", "\0", "CL1E5776SF", "B710D0", "102HW",\
	{
		RF_WCDMA_2100 | RF_WCDMA_J1500,	\
		0,\
		RF_LTE_B64\
	}, \
    {0}, \
    {0}
    },
	{HW_VER_PRODUCT_E5_CMCC, 0,\
	/* modified for E5776s-860 begin */
	"E5776s-860", "\0", "CL3E5776SM", "B710D0", "E5776",\
	/* modified for E5776s-860 end */
	{
		RF_WCDMA_2100 | RF_WCDMA_J1700,    \
		RF_LTE_B3,\
		0\
	}, \
	{0}, \
	{0}\
	},
    /*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/
    {HW_VER_PRODUCT_E5_CMCC_CY, 0,\
    "E5776Bs-860", "\0", "CL3E5776BSM", "B710D0", "E5776B",\
    {
        RF_WCDMA_2100 | RF_WCDMA_J1700,    \
        RF_LTE_B3,\
        0\
    }, \
    {0}, \
    {0}\
    },
    /*END DST2012092001529 liangshukun 20121025 ADDED*/
    {HW_VER_PRODUCT_E5776_EM, 0,\
    "E5776s-71", "\0", "CL1E5776SS", "B710D0", "GL04P",\
    {
        RF_WCDMA_2100 | RF_WCDMA_J1700,    \
        RF_LTE_B3,\
        0\
    }, \
    {0}, \
    {0}\
    },
	/* Added for E5371_DCM */
    /* E5371��Ʒ������ E5371S-7�޸�ΪHW-02E */
    {HW_VER_PRODUCT_E5371_DCM, 0,\
    "HW-02E", "\0", "CL1E5371SM", "B710D0", "HW-02E",\
	{
		RF_WCDMA_2100 | RF_WCDMA_800 | RF_WCDMA_850 | RF_GSM_ALL, \
		RF_LTE_B1 | RF_LTE_B19 | RF_LTE_B21,\
		0\
	}, \
	{0}, \
	{0}\
	},
	
	{HW_VER_PRODUCT_E5372_32, 0,\
    "E5372s-32", "\0", "CL1E5372SM", "B710D0", "E5372",\
	{
		RF_WCDMA_2100 | RF_WCDMA_1900 | RF_WCDMA_900 | RF_WCDMA_850 |RF_GSM_ALL, \
		RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B8 | RF_LTE_B20,\
		0\
	}, \
	{0}, \
	{0}\
	},

	{HW_VER_PRODUCT_E5375, 0,\
    "E5375", "\0", "CL1E5375SM", "B710D0", "E5375",\
	{
		RF_WCDMA_2100 | RF_WCDMA_1900 | RF_WCDMA_850 |RF_GSM_ALL, \
		RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B17 | RF_LTE_B38 | \
        RF_LTE_B39 | RF_LTE_B40 | RF_LTE_B41, \
		0\
	}, \
	{0}, \
	{0}\
	},

    {HW_VER_PRODUCT_E5375_SEC, 0,\
    "E5375", "\0", "CL1E5375SS", "B710D0", "E5375",\
    {
        RF_WCDMA_2100 | RF_WCDMA_1900 | RF_WCDMA_850 | RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B17 | RF_LTE_B38 | \
        RF_LTE_B39 | RF_LTE_B40 | RF_LTE_B41, \
        0\
    }, \
    {0}, \
    {0}\
    },
    {HW_VER_PRODUCT_E5775S_925, 0,\
    "E5775s-925", "\0", "CL1E5775SM", "B710D0", "E5775",\
    {
        RF_WCDMA_2100 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B38 | \
        RF_LTE_B40 | RF_LTE_B41, \
        0\
    }, \
    {0}, \
    {0}\
    }, 
    {HW_VER_PRODUCT_E5372_601, 0,\
    "E5372s-601", "\0", "CL1E5372SM02", "B710D0", "E5372",\
    {
        RF_WCDMA_2100 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B40, \
        0\
    }, \
    {0}, \
    {0}\
    },
    {HW_VER_PRODUCT_E5372TS_32, 0,\
    "E5372TS-32", "\0", "CL1E5372TSM", "B710D0", "E5372T",\
    {
        RF_WCDMA_2100 | RF_WCDMA_1900 | RF_WCDMA_900 | RF_WCDMA_850 |RF_GSM_ALL, \
        RF_LTE_B1 | RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B8 | RF_LTE_B20 ,\
        0\
    }, \
    {0}, \
    {0}\
    },
    {HW_VER_PRODUCT_E5372S_22, 0,\
    "E5372s-22", "\0", "CL1E5372SM03", "B710D0", "E5372",\
    {
        RF_WCDMA_2100 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B20 | RF_LTE_B38,\
        0\
    }, \
    {0}, \
    {0}\
    },
    {HW_VER_PRODUCT_R215, 0,\
    "R215", "\0", "CL1E5372SM", "B710D0", "R215",\
    {
        RF_WCDMA_2100 | RF_WCDMA_900 | RF_GSM_ALL, \
        RF_LTE_B3 | RF_LTE_B7 | RF_LTE_B40, \
        0\
    }, \
    {0}, \
    {0}\
    },
#endif
    
/* ���������� */
    {HW_VER_PRODUCT_PV500, 0,\
    "PV500", "\0", "PV500", "PV500", "PV500",\
    {RF_BAND_ALL, RF_BAND_ALL, RF_BAND_ALL}, \
    {0}, \
    {0}
    },
            
    {HW_VER_PRODUCT_PORTING, 0,\
    "PORTING", "\0", "PORTING", "PORTING", "PORTING",\
    {RF_BAND_ALL, RF_BAND_ALL, RF_BAND_ALL}, \
    {0}, \
    {0}
    },

    /*NULL product*/
    { 0xFFFF,0,\
    "\0", "\0", "\0", "\0",  "\0", \
    {0}, \
    {0}, \
    {0}}
};

/*--------------------------------------------------------------*
 * �ⲿ����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ڲ�����ԭ��˵��                                             *
 *--------------------------------------------------------------*/


/*--------------------------------------------------------------*
 * �ⲿ��������                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * ȫ�ֱ�������                                                 *
 *--------------------------------------------------------------*/
 
/*****************************************************************************
* �� �� ��  : BSP_Flashless_HwGetVerMain
*
* ��������  : Flashless��Ʒ��̬�л�ȡ����汾��(��汾��1+��汾��2)
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : ���ص���İ汾��
*
* ����˵��  : 
*
*****************************************************************************/
BSP_U32 BSP_Flashless_HwGetVerMain()
{
	ST_PART_TBL_ST * ram_ptable = (ST_PART_TBL_ST *)PTABLE_RAM_TABLE_ADDR;
	BSP_U32 cnt = 0;
	for (; cnt < PTABLE_PARTITION_MAX; cnt++)
	{
		if (0 == strcmp(PTABLE_END_STR, ram_ptable->name))
		{
			printk("HwVersion is %x, MEMORY_AXI_HW_ID_ADDR is %x\n", ram_ptable->offset, MEMORY_AXI_HW_ID_ADDR);
			return ram_ptable->offset;
		}
		//printk("Ptable name is %s, HwVersion is %x\n", ram_ptable->name, ram_ptable->offset);
		ram_ptable++;
	}

	printk("Can not get HwVersion\n");
	return 0;
}


/*****************************************************************************
* �� �� ��  : BSP_ProductInfoInit
*
* ��������  : ����Ӳ���汾�ų�ʼ����Ʒ��Ϣ
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_ProductInfoInit( BSP_VOID )
{

	BSP_U32 u32HwId;
    BSP_U16 u16IdMain;
    BSP_U8  u16IdSub;
    BSP_U32 i;

    /*����ֲ��ṹ�������ڴ棬����ʼ��Ϊȫ0*/	
    memset((void *)(&g_huaweiProductInfo), 0, sizeof(PRODUCT_INFO_T));	

    /* Ĭ������ */
    //g_huaweiProductInfo.index = HW_VER_INVALID;
    //g_huaweiProductInfo.hwIdSub = 0xFF;
    g_huaweiProductInfo.name = "PRODUCT_NAME";
    g_huaweiProductInfo.namePlus = "PRODUCT_NAME_PLUS";
    g_huaweiProductInfo.hwVer= "HW_VER";
    g_huaweiProductInfo.dloadId = "DLOAD_ID";
    g_huaweiProductInfo.productId = "PRODUCT_ID";
    memset(&g_huaweiProductInfo.band, 0x0, sizeof(BAND_INFO_T));

    memset(&g_huaweiProductInfo.bandDiv, 0x0, sizeof(BAND_INFO_T));  
#if defined (FEATURE_FLASH_LESS)
	u32HwId = BSP_Flashless_HwGetVerMain();
	u16IdMain = (u32HwId>>16) & 0xFFFF;
    u16IdSub = u32HwId & 0xFFFF;
	if ((HW_VER_INVALID ==  u16IdMain) || (0x0000 == u16IdMain))
	{
		printk("%s: Flashless get wrong hardware version, use hkadc hardware version.\n", __FUNCTION__);
		/* VxWorksֱ�Ӵ�BootLoader�����AXI Mem�л�ȡ */
	    u32HwId = *(BSP_U32*)MEMORY_AXI_HW_ID_ADDR;
	    u16IdMain = (u32HwId>>16) & 0xFFFF;
	    u16IdSub = u32HwId & 0xFFFF;
	}
	else
	{
		*(BSP_U32*)MEMORY_AXI_HW_ID_ADDR = u32HwId;
		printk("%s: Flashless get right hardware version.\n", __FUNCTION__);
	}
#else
    /* VxWorksֱ�Ӵ�BootLoader�����AXI Mem�л�ȡ */
    u32HwId = *(BSP_U32*)MEMORY_AXI_HW_ID_ADDR;
    u16IdMain = (u32HwId>>16) & 0xFFFF;
    u16IdSub = u32HwId & 0xFFFF;
#endif

    /* ����ʼ����Ʒ��Ϣ */
    for(i=0; i<ARRAY_SIZE(huaweiProduct); i++)
    {
        if(huaweiProduct[i].index == u16IdMain)
        {
            g_huaweiProductInfo = huaweiProduct[i];
            g_huaweiProductInfo.hwIdSub = u16IdSub;
            break;
        }
    }

    /* δ���ҵ�������ʧ�� */
    if(i == ARRAY_SIZE(huaweiProduct))
    {
        printk("%s: product info is not defined, pls check huaweiProduct!\n", __FUNCTION__);
        return BSP_ERROR;
    }

    printk("hw id: main,%#x, sub,%#x\n", u16IdMain, u16IdSub);

    return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : BSP_HwGetVerMain
*
* ��������  : ��ȡ����汾��(��汾��1+��汾��2)
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_U16 BSP_HwGetVerMain( BSP_VOID )
{
    /* VxWorksֱ�Ӵ�BootLoader�����AXI Mem�л�ȡ */
    return  ((*(BSP_U32*)MEMORY_AXI_HW_ID_ADDR)>>16) & 0xFFFF;
}

/*****************************************************************************
* �� �� ��  : BSP_HwGetVerSub
*
* ��������  : ��ȡ������Ӱ汾��,��ӦGPIO4/5
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_U16 BSP_HwGetVerSub( BSP_VOID )
{
    /* VxWorksֱ�Ӵ�BootLoader�����AXI Mem�л�ȡ */
    return  (*(BSP_U32*)MEMORY_AXI_HW_ID_ADDR) & 0xFFFF;
}

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
BSP_S32 BSP_HwGetHwVersion (char* pFullHwVersion, BSP_U32 ulLength) 
{
    unsigned char hwVerLit = 0;
    unsigned int len = 0;

    /*����ΪӲ�ˣ���ˣ�����UDP*/
    if((HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pFullHwVersion))    
    {
        return BSP_ERROR;
    }    
    
    len = strlen(g_huaweiProductInfo.hwVer)+strlen(" Ver.X");
    
    if( len >= ulLength )
    {
        return BSP_ERROR;
    }
    
    hwVerLit = (BSP_S8)g_huaweiProductInfo.hwIdSub+'A';
    
    (void)memset((void *)pFullHwVersion, 0, ulLength);
    strcat(strcat(pFullHwVersion, g_huaweiProductInfo.hwVer), " Ver.");    
    *((pFullHwVersion + len) - 1) = (char)hwVerLit;
    *(pFullHwVersion + len) = 0;
    
    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_HwGetPCBVersion
*
* ��������  : ��ȡPCB�汾����
*
* �������  : BSP_CHAR* pFullPcbVersion,�ַ���ָ�룬��֤��С��32�ֽ� 
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2012-3-16 wzs creat
*
*****************************************************************************/
BSP_S32 BSP_HwGetPCBVersion (char* pFullPcbVersion, BSP_U32 ulLength) 
{
    unsigned int len = 0;

    /*����ΪӲ�ˣ���ˣ�����UDP*/
    if((HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pFullPcbVersion))    
    {
        return BSP_ERROR;
    }    
    
    len = strlen(g_huaweiProductInfo.hwVer);
    
    if( len >= ulLength )
    {
        return BSP_ERROR;
    }
       
    (void)memset((void *)pFullPcbVersion, 0, ulLength);
    (void)memcpy((void *)pFullPcbVersion,g_huaweiProductInfo.hwVer,len);
    
    return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : BSP_GetProductName 
*
* ��������  : ��ȡ��Ʒ����
*
* �������  : char* pProductName,�ַ���ָ�룬��֤��С��32�ֽ� 
*                             BSP_U32 ulLength,����������  
* �������  : ��
*
 �� �� ֵ  : 0����ȷ����0: ʧ��
*
* �޸ļ�¼  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength)
{
    unsigned int len;

    if( (HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pProductName) )
    {
        return BSP_ERROR;
    }    

    len = strlen(g_huaweiProductInfo.productId);  
    
    if( len >= ulLength )
    {
        return BSP_ERROR;
    }
    
    (void)memset((void*)pProductName, 0, ulLength);
    strcat(pProductName, g_huaweiProductInfo.productId);
    *( pProductName + strlen(g_huaweiProductInfo.productId)) = 0;
    

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_GetProductDloadName 
*
* ��������  : ��ȡ��Ʒ���ذ汾���ƣ�һ�������ݴ��ж��Ƿ���������
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��Ʒ���ذ汾�����ַ���ָ��
*
* �޸ļ�¼  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_GetProductDloadName(char * pProductDloadName, unsigned int ulLength)
{
    unsigned int len;
    
    if( (HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pProductDloadName) )
    {
        return BSP_ERROR;
    }    

    len = strlen(g_huaweiProductInfo.dloadId);  
    
    if( len >= ulLength )
    {
        return BSP_ERROR;
    }
    
    (void)memset((void*)pProductDloadName, 0, ulLength);
    strcat(pProductDloadName, g_huaweiProductInfo.dloadId);
    *( pProductDloadName + strlen(g_huaweiProductInfo.dloadId)) = 0;
    
    return  BSP_OK;  
}

/*****************************************************************************
* �� �� ��  : BSP_GetProductInnerName 
*
* ��������  : ��ȡ�ڲ���Ʒ����
*
* �������  : char* pProductIdInter,�ַ���ָ��
*                             BSP_U32 ulLength,���������� 
* �������  : ��
*
 �� �� ֵ  : 0����ȷ����0: ʧ��
*
* �޸ļ�¼  : 2011-3-29 wuzechun creat
*
*****************************************************************************/

BSP_S32 BSP_GetProductInnerName (char * pProductIdInter, unsigned int ulLength)
{
    unsigned int len;
    
    if( (HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pProductIdInter) )
    {
        return BSP_ERROR;
    }
    
    len = strlen(g_huaweiProductInfo.name)+strlen(g_huaweiProductInfo.namePlus);  
    
    if( len >= ulLength )
    {
        return BSP_ERROR;
    }
    
    (void)memset((void*)pProductIdInter, 0, ulLength);
    strcat(strcat(pProductIdInter, g_huaweiProductInfo.name), g_huaweiProductInfo.namePlus);
    *( pProductIdInter + strlen(g_huaweiProductInfo.name) + strlen(g_huaweiProductInfo.namePlus) ) = 0;
    
    return  BSP_OK;  
}

/*****************************************************************************
* �� �� ��  : BSP_GetSupportBands
*
* ��������  : ��ȡ֧�ֵ�Ƶ��
*
* �������  : BAND_INFO_T *pBandInfo,Ƶ����Ϣ�ṹ��ָ��
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_GetSupportBands( BAND_INFO_T *pBandInfo)
{  
    if( (HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pBandInfo) )
    {
        return BSP_ERROR;
    }
    
    *pBandInfo = g_huaweiProductInfo.band;

    return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : BSP_GetSupportBandsForGu
*
* ��������  : ��ȡGUģ֧�ֵ�Ƶ��
*
* �������  : pusWBands - WCDMA��Ƶ��ֵ
             pusGBands - GSM��Ƶ��ֵ

 �������  : pusWBands - WCDMA��Ƶ��ֵ
             pusGBands - GSM��Ƶ��ֵ

 �� �� ֵ  : 0����ȷ����0: ʧ��
             ͨ��Bitλ����ʾ��ЩƵ��֧�֡�
*
*****************************************************************************/

int BSP_GetSupportBandsForGu(unsigned short *pusWBands, unsigned short *pusGBands)
{
    
    if( (HW_VER_INVALID == g_huaweiProductInfo.index) || (NULL == pusWBands) || (NULL == pusGBands) )
    {
        return BSP_ERROR;
    }

    *pusWBands = g_huaweiProductInfo.band.bandGU & 0xFFFF;
    
    *pusGBands = (g_huaweiProductInfo.band.bandGU >> 16) & 0xFFFF;

    return BSP_OK;
}

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
BSP_BOOL BSP_HwIsSupportWifi(BSP_VOID)
{
    /* E5֧��wifi��UDPĬ��Ҳ֧��wifi ,��������֧��wifi����������Ҫ����*/
#if (FEATURE_E5 == FEATURE_ON) || #defined(FEATURE_BOARD_STUB_BJ_UDP)
    return BSP_TRUE;
#else
    return BSP_FALSE;
#endif
}

/*****************************************************************************
* �� �� ��  : BSP_PrintHwVersion
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
BSP_S32 BSP_PrintHwVersion(BSP_VOID)
{
#define MAX_HARD_VERSION_NUM 32

    BSP_S32 s32Ret = BSP_OK;
    BAND_INFO_T bandInfo;
    
    BSP_CHAR hwVer[MAX_HARD_VERSION_NUM];
   
    printk("main ID:                 %#x\n", BSP_HwGetVerMain());
    printk("sub ID:                  %#x\n", BSP_HwGetVerSub());

    s32Ret = BSP_GetSupportBands(&bandInfo);
    
    printk("support bands:\n");
    printk("   GU:%#x\n", bandInfo.bandGU);
    printk("   LTELow:%#x\n", bandInfo.bamdLTELow);
    printk("   LTEHigh:%#x\n", bandInfo.bamdLTEHigh);

    (BSP_VOID)BSP_GetProductName(hwVer, MAX_HARD_VERSION_NUM);
    printk("product name:            %s\n", hwVer);

    (BSP_VOID)(BSP_VOID)BSP_GetProductInnerName(hwVer, MAX_HARD_VERSION_NUM);
    printk("product inner name:      %s\n", hwVer);

    (BSP_VOID)BSP_GetProductDloadName(hwVer, MAX_HARD_VERSION_NUM);
    printk("product download name:   %s\n", hwVer);

    (BSP_VOID)BSP_HwGetHwVersion(hwVer, MAX_HARD_VERSION_NUM);
    printk("hardware version:        %s\n", hwVer);    
    (BSP_VOID)BSP_HwGetPCBVersion(hwVer, MAX_HARD_VERSION_NUM);
    printk("pcb version:        %s\n", hwVer);    
   
    return s32Ret;
}

EXPORT_SYMBOL(BSP_HwGetVerMain);
EXPORT_SYMBOL(BSP_HwGetVerSub);
EXPORT_SYMBOL(BSP_HwGetHwVersion);
EXPORT_SYMBOL(BSP_GetProductName);
EXPORT_SYMBOL(BSP_GetProductDloadName);
EXPORT_SYMBOL(BSP_GetProductInnerName);
EXPORT_SYMBOL(BSP_GetSupportBands);
EXPORT_SYMBOL(BSP_HwIsSupportWifi);
