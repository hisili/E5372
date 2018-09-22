
//#include <stdio.h>
//#include <logLib.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include "BSP.h"
#include "BSP_I2C.h"
#include <mach/balong_v100r001.h>
#include <generated/FeatureConfig.h>   /*syb*/
#include <linux/module.h>  //clean warning

extern void uw_charger_set_soft_mode(void);
extern void uw_charger_set_bypass_mode(void);
extern void ecsUsbClkOff(void);

I2C_CLKCFG_S CY22393_CFG[CY22393_CFG_NUM] = 
{
        {0x0f,0x54},
        {0x10,0x55},
        {0x1a,0xe9},
        {0x1b,0x08}
};

BSP_S32 I2C_StatusCheck(BSP_VOID)
{
    BSP_U32 u32RegValue = 0;
    BSP_U32 u32LoopNum = 100000;

    /*��ѯ���ߴ����Ƿ����*/
    do
    {
        BSP_REG_READ(IIC_REGBASE_ADDR, 0x0c, u32RegValue);
    }while((!(u32RegValue & 0x1)) && (u32LoopNum-- > 0));

	BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x8, 0x7f);
	
    if(0 == u32LoopNum)
    {
    	printk("I2C: Timeout! \n");
        return ERROR;
    }

    return OK;
}

BSP_S32 BSP_I2C_DataTx(BSP_U8 u8Addr, BSP_U8 u8Value)
{    
    /*����SCL�źŸߵ�ƽ����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x10, 0x77); //��ֵӦ������Ϊ����
    
    /*����SCL�źŵ͵�ƽ����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x14, 0x77); //��ֵӦ������Ϊ����

    /*��������ж�*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x08, 0x7f);
    
    /*����I2C�ж����Σ�ʹ��IP������*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x00, 0x187);//Ϊ������3��4��5��6Bit��Ӧ���ж�
	
    /*��Slave��ַд�뷢�����ݼĴ���,����bit[7]����Ϊ0*/
//    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, u8Addr);//��ַλ��Ϊ9ʱ?
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, (0x69<<1));//��ַλ��Ϊ9ʱ?
//    BSP_REG_CLRBITS(IIC_REGBASE_ADDR, 0x18, 7, 1);

    /*��������Ĵ���bit[3],bit[1]Ϊ1��������ʼ��������������д����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0xa);

    /*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

	/*��ʼ���ͼĴ�����ַ,���Ĵ�����ַ�������ݷ��ͼĴ���*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, u8Addr);    

    /*��������Ĵ�����bit[1]��1������д����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0x2);

    /*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

    /*��ʼ��������,�����ݷ������ݷ��ͼĴ���*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, u8Value);    

    /*��������Ĵ�����bit[1]��1������д����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0x2);

    /*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

    /*���ݷ������,��������Ĵ���bit[0],����ֹͣ����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0x1);

    /*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }
	
    return OK;
}

/*
//i2c ioshare
0x90000988 bit[14:13]=2'b10

//usbphy reset, release after clk_26m stability
0x90000490 bit[0] = 1'b0
*/
BSP_S32 BSP_CY22393_Init(BSP_VOID)
{
    BSP_S32 i;
    BSP_S32 s32Status;
    BSP_U32 u32RegValue = 0;

    /* 0. ����USB ���ģ��Ϊ Bypass */
    BSP_REG_READ(IO_ADDRESS(V3R2_SC_BASE), 0xC000, u32RegValue);
    u32RegValue |= (0x1 << 1); /* set 1bit -> 1 */
    BSP_REG_WRITE(IO_ADDRESS(V3R2_SC_BASE), 0xC000, u32RegValue);

    /* 1. ����i2c�ܽŸ��� */
    BSP_REG_READ(IO_ADDRESS(V3R2_SC_BASE), 0x988, u32RegValue);
    /*Porting�汾--��0x90000988��bit13�л���bit10��ԭ��֮ǰ��uart3�ܽŸ��ã�������uart1�ܽŸ���zhaotao 20110715*/
    //u32RegValue &= ~(0x1 << 13); /* set 13bit -> 0 */
    u32RegValue &= ~(0x1 << 10); /* set 10bit -> 0 */
    u32RegValue |= (0x1 << 14); /* set 14bit -> 1 */
    BSP_REG_WRITE(IO_ADDRESS(V3R2_SC_BASE), 0x988, u32RegValue);

    /* 2. USB PHY reset ������ */
    BSP_REG_READ(IO_ADDRESS(V3R2_SC_BASE), 0x490, u32RegValue);
    u32RegValue |= (0x1); /* set 0bit -> 1 */
    //u32RegValue &= ~(0x1); /* set 0bit -> 0 */
    BSP_REG_WRITE(IO_ADDRESS(V3R2_SC_BASE), 0x490, u32RegValue);

    /* 3. ����22393�Ĵ���, ����I2C��USB PHY �ṩʱ�� */
    for(i = 0; i < CY22393_CFG_NUM; i++)
    {
        s32Status = BSP_I2C_DataTx(CY22393_CFG[i].u8regAddr, CY22393_CFG[i].u8regValue);
        if(OK != s32Status)
        {
            printk("Data Send ERROR! line is %d \r\n",__LINE__);
            return s32Status;
        }
    }

    msleep(10);

    /* 4. USB PHY ʱ���ȶ������߽���reset */
    BSP_REG_READ(IO_ADDRESS(V3R2_SC_BASE), 0x490, u32RegValue);
    u32RegValue &= ~(0x1); /* set 0bit -> 0 */
    //u32RegValue |= (0x1); /* set 0bit -> 1 */
    BSP_REG_WRITE(IO_ADDRESS(V3R2_SC_BASE), 0x490, u32RegValue);

    return OK;
}

BSP_S32 BSP_I2C_DataRx(BSP_U8 u8Addr)
{
    BSP_U32 u32RegValue;

     /*����SCL�źŸߵ�ƽ����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x10, 0x77); //��ֵӦ������Ϊ����
    
    /*����SCL�źŵ͵�ƽ����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x14, 0x77); //��ֵӦ������Ϊ����

    /*��������ж�*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x08, 0x7f);

    /*����I2C�ж����Σ�ʹ��IP������*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x00, 0x187);//Ϊ������3��4��5��6Bit��Ӧ���ж�

    /*��Slave��ַд�뷢�����ݼĴ���,����bit[7]����Ϊ0*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, (0x69<<1));//��ַλ��Ϊ9ʱ?

	/*��������Ĵ���bit[3],bit[1]Ϊ1��������ʼ��������������д����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0xa);

	/*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

	/*��ʼ���ͼĴ�����ַ,���Ĵ�����ַ�������ݷ��ͼĴ���*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, u8Addr);    

    /*��������Ĵ�����bit[1]��1������д����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0x2);

    /*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

	 /*��Slave��ַд�뷢�����ݼĴ���,����bit[7]����Ϊ0*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x18, ((0x69<<1)|0x1));//��ַλ��Ϊ9ʱ?

	/*��������Ĵ���bit[3],bit[1]Ϊ1��������ʼ��������������������*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0xa);

	/*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0x14);	

    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }
	
	BSP_REG_READ(IIC_REGBASE_ADDR, 0x1c, u32RegValue);

	/*���ݽ������,��������Ĵ���bit[0],����ֹͣ����*/
    BSP_REG_WRITE(IIC_REGBASE_ADDR, 0x04, 0x1);

    /*��ѯ���ߴ����Ƿ����*/
    if(OK != I2C_StatusCheck())
    {
        printk("Status Get ERROR! line is %d \r\n",__LINE__);
        return ERROR;
    }

    return (BSP_S32)u32RegValue;
}

BSP_VOID USB_UnReset(BSP_VOID)
{
    BSP_U32 u32Value;

    printk("Enter USB_UnReset\r\n");

    //uw_charger_set_bypass_mode();

    /* otgdisable */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x494, 25, 1, 1);
    /* vbusvldext and vbusvldextsel */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x494, 27, 2, 3);

#if ((FEATURE_EPAD == FEATURE_ON) || (FEATURE_PHONE == FEATURE_ON))
    /* 
    PAD �汾������ͼ
    31:30 [01] ��ͼԤ����,1 ������
    29:28 [00] ���ٷ���������/�½���ʱ����� +10%
    */
    BSP_REG_READ(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x490, u32Value);
    u32Value &= ~0xF0000000;
    u32Value |=  0x40000000;
    BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x490, u32Value);
#endif

    /* Release otg phy */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 12, 1);
    mdelay(2);

#if defined(CHIP_BB_6920ES)  /* for hi6920 CS_Debug */
    printk("Disable OTG functions\n");
    BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x494, 0x3a040000);
    msleep(10);
#endif

#if defined (BOARD_SFT) && (defined (VERSION_V7R1) || defined (CHIP_BB_6920CS))
    if(OK == BSP_CY22393_Init())
    {
        printk("USB_UnReset OK!\r\n");
    }
    else
    {
        printk("USB_UnReset ERROR!\r\n");
    }
#endif
}
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
void DRV_HSIC_Release(void)
{
    BSP_U32 regval;
    
    printk("DRV_HSIC_Release: Entering ...\r\n");

    /* Adjust HSIC signals */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 7, 8, 0xFF);

#if defined(CHIP_BB_6920ES)
    /* Enable HSIC Clock 480MHz */
    BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x94, 0);
    mdelay(100);
    BSP_REG_READ(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x94, regval);
    printk("DRV_HSIC_Release: 0x90000094 = 0x%08x\r\n", regval);
#endif

    /* Force Host Mode */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 31, 1);

#if defined(CHIP_BB_6920CS)
    /* dppulldown, dmpulldown set */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 20, 3, 7);
#endif
    
    udelay(100);
    /* Reset HSIC PHY Port */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 19, 1, 1);
    /* Reset HSIC PHY Por */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 14, 1, 1);
    /* Reset HSIC Controller */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 20, 1, 1);
    /* Activate HSIC Controller */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 20, 1);
    udelay(100);
    /* Release HSIC PHY Port */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 19, 1);
    /* Release HSIC PHY Por */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 14, 1);
    udelay(100);
}

void DRV_HSIC_ControllerPhyCutOff(void)
{
    /* Reset HSIC PHY Port */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 19, 1, 1);
    /* Reset HSIC PHY Por */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 14, 1, 1);
    /* Reset HSIC Controller */
    BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 20, 1, 1);
}

void DRV_HSIC_ControllerPhyRelease(void)
{
    /* Activate HSIC Controller */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 20, 1);
    udelay(100);
    /* Release HSIC PHY Port */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x488, 19, 1);
    /* Release HSIC PHY Por */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x38, 14, 1);
    udelay(100);
}

#if defined(CHIP_BB_6920ES)
#define BCM43239_WIFI_GPIO_BASE         GPIO_1_BASE
#define BCM43239_WIFI_GPIO_RST          5
#define BCM43239_WIFI_GPIO_CTRL         0x848
#define BCM43239_WIFI_GPIO_PU           24
#define BCM43239_WIFI_GPIO_PD           25
#define BCM43239_WIFI_GPIO_SUS          26
#elif defined(CHIP_BB_6920CS)
#define BCM43239_WIFI_GPIO_BASE         GPIO_0_BASE
#define BCM43239_WIFI_GPIO_RST          12
#define BCM43239_WIFI_GPIO_CTRL         0x818
#define BCM43239_WIFI_GPIO_PU           8
#define BCM43239_WIFI_GPIO_PD           9
#define BCM43239_WIFI_GPIO_SUS          10
#endif

void BCM43239_WIFI_Release(void)
{
    printk("BCM43239_WIFI_Release: Entering...\n");

#if defined(CHIP_BB_6920ES)
    /* Configure gpio function */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x988, 27, 1);
#endif

    /* Configure BCM43239_RST_N wake pull keeper */
    /* Shutdown the pullup & pulldown function */
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), BCM43239_WIFI_GPIO_CTRL, BCM43239_WIFI_GPIO_PU, 1);
    BSP_REG_CLRBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), BCM43239_WIFI_GPIO_CTRL, BCM43239_WIFI_GPIO_PD, 1);
    /* Enable wake pull keeper function */
    /* BSP_REG_SETBITS(IO_ADDRESS(INTEGRATOR_SC_BASE), BCM43239_WIFI_GPIO_CTRL, BCM43239_WIFI_GPIO_SUS, 1, 1); */   /* BCM43239_RST_N */
    /* Configure WLAN_WAKE, BCM43239_PA_EN, BCM43239_RST_N as Output */
    /* BSP_REG_SETBITS(IO_ADDRESS(GPIO_0_BASE), 0x04, 22, 1, 1); */   /* WLAN_WAKE */
    /* BSP_REG_SETBITS(IO_ADDRESS(GPIO_1_BASE), 0x04, 0, 1, 1); */   /* BCM43239_PA_EN */
    BSP_REG_SETBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 4, BCM43239_WIFI_GPIO_RST, 1, 1);   /* BCM43239_RST_N */
    /* Configure WLAN_HOST_WAKE as input */
    /* BSP_REG_CLRBITS(IO_ADDRESS(GPIO_0_BASE), 0x04, 26, 1); */   /* WLAN_HOST_WAKE */
    /* Drive High of BCM43239_PA_EN */
    /* BSP_REG_SETBITS(IO_ADDRESS(GPIO_1_BASE), 0x00, 0, 1, 1); */   /* BCM43239_PA_EN */
    /* Drive High of WLAN_WAKE */
    /* BSP_REG_SETBITS(IO_ADDRESS(GPIO_0_BASE), 0x00, 22, 1, 1); */   /* WLAN_WAKE */
    /* Drive Low of BCM43239_RST_N */
    BSP_REG_CLRBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 0, BCM43239_WIFI_GPIO_RST, 1);
    /* Configure BCM43239_RST_N input to save power */
    /* BSP_REG_CLRBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 4, BCM43239_WIFI_GPIO_RST, 1);*/   /* BCM43239_RST_N */
}

void BCM43239_WIFI_PowerDown(void)
{
    /* Configure BCM43239_RST_N output */
    /* BSP_REG_SETBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 4, BCM43239_WIFI_GPIO_RST, 1, 1); */   /* BCM43239_RST_N */
    /* Drive Low of BCM43239_RST_N */
    BSP_REG_CLRBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 0, BCM43239_WIFI_GPIO_RST, 1);
    /* Configure BCM43239_RST_N input to save power */
    /* BSP_REG_CLRBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 4, BCM43239_WIFI_GPIO_RST, 1); */   /* BCM43239_RST_N */
}

void BCM43239_WIFI_PowerOn(void)
{
    /* Delay at least 50ms */
    mdelay(200);
    /* Configure BCM43239_RST_N output from lowpower */
    /* BSP_REG_SETBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 4, BCM43239_WIFI_GPIO_RST, 1, 1); */   /* BCM43239_RST_N */
    /* Drive High of BCM43239_RST_N */
    BSP_REG_SETBITS(IO_ADDRESS(BCM43239_WIFI_GPIO_BASE), 0, BCM43239_WIFI_GPIO_RST, 1, 1);   /* BCM43239_RST_N */
}
#endif

/*#define TEST_DEBUG*/
#include <linux/kthread.h>
static struct task_struct *test_thread;

static int test_thread_entry(void *__unused)
{
    unsigned int i = 0;

    msleep(20000);
	for(;;)
	{        
        printk("%d\r\n",i);
        msleep(1000);
        i++;
	} 

    return OK;
}

BSP_S32 test_thread_init(void)
{
#ifdef TEST_DEBUG
    test_thread = kthread_run(test_thread_entry, NULL, "test_thread");
    if (IS_ERR(test_thread))
    {
    	printk("create kthread test_thread failed!\n");
    	return ERROR;
    }
    printk("create kthread test_thread ok!\n");
#endif
    return OK;
}

EXPORT_SYMBOL(test_thread_init);
EXPORT_SYMBOL(USB_UnReset);
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
EXPORT_SYMBOL(DRV_HSIC_Release);
EXPORT_SYMBOL(DRV_HSIC_ControllerPhyCutOff);
EXPORT_SYMBOL(DRV_HSIC_ControllerPhyRelease);
EXPORT_SYMBOL(BCM43239_WIFI_Release);
EXPORT_SYMBOL(BCM43239_WIFI_PowerDown);
EXPORT_SYMBOL(BCM43239_WIFI_PowerOn);
#endif

