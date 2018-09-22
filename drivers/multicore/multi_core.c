#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include "BSP.h"
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>

#include <generated/FeatureConfig.h>
#include <mach/common/mem/bsp_mem.h>
#include <linux/module.h>
#include "product_config.h"

#if defined(CHIP_BB_6756CS)
#define BALONG_BOARD_TYPE_DETECT    BALONG_GPIO_0(7)
#else
#define BALONG_BOARD_TYPE_DETECT    BALONG_GPIO_0(11)
#endif
uint32_t g_cshell_on = 0;
EXPORT_SYMBOL(g_cshell_on);
UDI_HANDLE g_cshell_udi_handle = -1;
extern uint32_t cshell_event_cb(uint32_t id, uint32_t event, void *Param);
extern uint32_t cshell_read_cb(uint32_t id, uint32_t size);
extern uint32_t cshell_write_cb(uint32_t id);
extern int cdc_ashell_acm_init(void);
extern BSP_S32 BSP_UdiAdpUartInit(void);
extern BSP_U32 SCI_Init();

typedef struct tagNETIF_INIT_PARAM_T
{
	BSP_S32 s32CalcTime;
	BSP_S32 s32CalcPktNum;
	BSP_S32 s32NetifFlg;
}NETIF_INIT_PARAM_T;

extern unsigned int NVM_Init(unsigned int ulOption);
/*modified for lint e 752
extern int __init jusb_entry(void *args);*/
extern BSP_S32 BSP_PMU_Init(BSP_VOID);
extern BSP_S32 BSP_ProductInfoInit( BSP_VOID );
extern BSP_S32 test_thread_init(void);
extern BSP_S32 BSP_UDI_Init(VOID);
extern uint32_t NVM_Read(uint32_t ulId, void *pData, uint16_t usDataLen);
extern void netif_init_balong_param(int time, int pktnum,int switchflg);

#if(FEATURE_CHARGE == FEATURE_ON )
#include <linux/BSP_CHGC_DRV.h>
#endif
#if ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)) \
	&& defined (BOARD_FPGA_P500))
void arch_idle(void)
{
}
#endif

static int cshell_icc_open(void)
{
	ICC_CHAN_ATTR_S attr;
	UDI_OPEN_PARAM cshell_icc_param;
    UDI_HANDLE cshell_udi_handle = UDI_INVALID_HANDLE;
	
	attr.enChanMode  = ICC_CHAN_MODE_STREAM;
    attr.u32Priority = 255;
    attr.u32TimeOut  = 1000;

    attr.u32FIFOInSize  = 8192;
    attr.u32FIFOOutSize = 8192;

	cshell_icc_param.devid = UDI_BUILD_DEV_ID(UDI_DEV_ICC, 31);/*lint !e64*/
    cshell_icc_param.pPrivate = &attr;

    cshell_udi_handle = udi_open(&cshell_icc_param);
	if(cshell_udi_handle <= 0)
	{
		printk("cshell_icc_open fail,cshell_udi_handle is %d \n",cshell_udi_handle);
		g_cshell_udi_handle = -1;
	}
	else
	{
		printk("cshell_icc_open success,cshell_udi_handle is %d \n",cshell_udi_handle);
		g_cshell_udi_handle = cshell_udi_handle;

        if ((*(BSP_U32 *)MEMORY_AXI_PRT_FLAG_ADDR != PRT_FLAG_EN_MAGIC_M)
            && (*(BSP_U32 *)MEMORY_AXI_PRT_FLAG_ADDR != PRT_FLAG_EN_MAGIC_A))
        {
            (void)BSP_UdiAdpUartInit();
        }
	}
	return 0;
}

#define NETIF_INIT_PARAM_ID 0xD110
static void netif_init_by_nv(void)
{
	NETIF_INIT_PARAM_T stInitParam;
	BSP_U32 ret;
	
	ret = NVM_Read(NETIF_INIT_PARAM_ID, &stInitParam, sizeof(NETIF_INIT_PARAM_T));
	if (BSP_OK != ret)
	{
		printk("netif_init_by_nv NVM_Read ID Fail:0x%x\n", ret);
		return;
	}
	netif_init_balong_param(stInitParam.s32CalcTime, stInitParam.s32CalcPktNum,stInitParam.s32NetifFlg);
	return;
}

UDI_HANDLE cshell_get_handle(void)
{
	return g_cshell_udi_handle;
}
EXPORT_SYMBOL(cshell_get_handle);

static int __init p500_multicore_init(void)
{
	int ret = 0;
	int32_t gpio_value = 0;
    uint16_t hwId;
    uint32_t cshell_addr = MEMORY_AXI_USB_CSHELL_ADDR; /*lint !e10 !e522*/
    
	printk("***************************************************************\n");
	printk("begin to init mutilcore: 0000\n");
#if defined (BOARD_SFT) && defined (VERSION_V3R2)
    BSP_DRV_IPCIntInit();
#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
    BSP_DRV_IPCIntInit();
//#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#elif ((defined (BOARD_ASIC) || defined (BOARD_SFT)) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    BSP_ProductInfoInit();
    BSP_MEM_Init();
	BSP_DRV_IPCIntInit();
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
    BSP_DRV_IPCIntInit();
#elif defined (BOARD_FPGA) && defined (VERSION_V3R2)
    BSP_DRV_IPCIntInit();
    /*BSP_MEM_Init();*/
#elif (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    BSP_ProductInfoInit();
    BSP_MEM_Init();
#else
    #error "unknown platform type"
#endif

	printk("begin to init mutilcore: 222 \n");
	ret = BSP_UDI_Init();
	if (ret < 0)
	{
		printk("BSP_UDI_Init failed!\n");
		//return -1;
	}

	/* mutilcore init */
	printk("start BSP_ICC_Init\n");
	ret = BSP_ICC_Init();
	if (ret < 0)
	{
		printk("BSP_ICC_Init failed ret=%d!\n", ret);
		//return -1;
	}
	printk("end BSP_ICC_Init\n");

	printk("begin to init mutilcore: 333\n");
	ret = IFC_Init();
	if (ret < 0)
	{
		printk("IFC_Init failed!\n");
		//return -1;
	}

	printk("begin to init mutilcore: 444\n");

	ret = IFCP_Init();
	if (ret < 0)
	{
		printk("IFCP_Init failed!\n");
		//return -1;
	}

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
    hwId = BSP_HwGetVerMain();
    if((HW_VER_PRODUCT_UDP !=  hwId
        && HW_VER_PRODUCT_PORTING !=  hwId
        && HW_VER_PRODUCT_PV500 !=  hwId))
    {
	    g_cshell_on = 1;
        /* Selective to run cshell */
        if (*(BSP_U32 *)MEMORY_AXI_PRT_FLAG_ADDR != PRT_FLAG_EN_MAGIC_M)
        {
    	    printk("A:start icc cshell... \n");
            *(volatile uint32_t *)(cshell_addr) = 0; /*lint !e26 !e10 !e63*/
    	    cshell_icc_open();
        }
        /* Selective to run ashell */
        if (*(BSP_U32 *)MEMORY_AXI_PRT_FLAG_ADDR != PRT_FLAG_EN_MAGIC_A)
        {
    	    printk("A:start ashell init... \n");
    		cdc_ashell_acm_init();
        }
	}
#elif defined (BOARD_ASIC) && defined (VERSION_V3R2)
        if(gpio_request(BALONG_BOARD_TYPE_DETECT, "Board_Type_detect"))
        {
            printk("Gpio is busy!!! \n");
        }
        else
        {
            gpio_value = gpio_get_value(BALONG_BOARD_TYPE_DETECT);
            if (gpio_value)
            {
                /*GPIO为高电平*/
                printk("Gpio is high!!! \n");
                g_cshell_on = 0;
            }
            else
            {        
                /*GPIO为低电平*/
                printk("Gpio is low!!! \n");
                g_cshell_on = 1;
                /* 产品板，需要初始化cshell */
                printk("A:start icc cshell... \n");
                cshell_icc_open();
				cdc_ashell_acm_init();
            }
            gpio_free(BALONG_BOARD_TYPE_DETECT);
        }
#endif

#if defined(BOARD_ASIC) \
     && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
    ret = BSP_PMU_Init();
	if (ret < 0)
	{
		printk("BSP_PMU_Init failed!\n");
		//return -1;
	}
#endif

#if(FEATURE_CHARGE == FEATURE_ON )
    BSP_CHG_Init();
#endif

#if (defined (BOARD_ASIC)||defined (BOARD_SFT)||defined (BOARD_FPGA_P500)) \
     && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
	ret = WDT_Init();/*lint !e746 */
	if (ret < 0)
	{
		printk("WDT_Init failed!\n");
		//return -1;
	}

#endif

#if defined (FEATURE_FLASH_LESS)
#else

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
 #ifndef PRODUCT_CFG_BUILD_SEPARATE
	printk("start NVM_Init\n");
    ret = NVM_Init(0);
    if (0 != ret)
    {
        printk("\r NVM_Init fail, result = 0x%x \n", ret);
    }
	printk("end NVM_Init\n");
 #endif
#endif
#endif

#if defined (VERSION_V3R2) 
    ret = NVM_Init(0); /*lint !e713*/
    if (0 != ret)
    {
        printk("\r NVM_Init fail, result = 0x%x \n", ret);
    }
#endif
#ifndef BOOT_OPTI_TASKDELAY
	msleep(200);
#endif
	printk("begin to init mutilcore: 555\n");

#if (defined (BOARD_ASIC)||defined (BOARD_SFT)) \
     && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
	BSP_PWRCTRL_SetWakeSrc(PWC_ARM_SLEEP);
#endif

#if defined(FEATURE_HSIC_ON) && !defined(BOARD_FPGA_P500) && \
    (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
    BCM43239_WIFI_Release();
    DRV_HSIC_Release();
#endif

#if (defined (BOARD_FPGA_P500) \
     && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))|| \
     ((defined(BOARD_SFT)||defined (BOARD_FPGA))  \
     && defined (VERSION_V3R2))
#else
	#if defined(CHIP_BB_6756CS)
	#else
	test_thread_init();
	#endif
#endif

// sci detect
#ifdef FEATURE_SIM_NOTIFY
	ret = SCI_Init();
	if (ret < 0)
	{
		printk("SCI_Init failed!\n");
	}
#endif

#if (FEATURE_ON == FEATURE_ECM_RNDIS)
	printk("start netif_init_by_nv\n");
	netif_init_by_nv();
	printk("end netif_init_by_nv\n");
#endif

	printk("begin to init mutilcore: 666\n");

	printk("***************************************************************\n");
	return 0;
}/*lint !e529*/

subsys_initcall(p500_multicore_init);
//module_init(p500_multicore_init);


