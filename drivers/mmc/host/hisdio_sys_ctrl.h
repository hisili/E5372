#ifndef _SYS_CTRL_H_
#define _SYS_CTRL_H_

#define BALONG_PORT_ADP (0)

extern void __iomem  * g_sysctrl_base;

typedef  struct sdio_handle
{
    irqreturn_t (*sdio_oob_irq)(int irq, void *dev_id);
                                                   
}sdio_handle;
#define TRY_COUNT          (0xFFFFFF)

#if (defined (BOARD_SFT) && defined (VERSION_V3R2)) \
    || (defined (BOARD_FPGA) && defined (VERSION_V3R2))
#define MMC0_CLK           30000000   /*SDXC?*/
#define MMC0_CCLK_MAX     100000000
#define MMC0_CCLK_MIN      400000
#define MMC1_CLK			30000000
#define MMC1_CCLK_MAX	30000000
#define MMC1_CCLK_MIN	300000
#elif (defined (BOARD_SFT) && defined (VERSION_V7R1)) \
    || (defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2))
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
			#define MMC0_CLK           30000000   /*SDXC?*/
			#define MMC0_CCLK_MAX     100000000
			#define MMC0_CCLK_MIN      400000
			#define MMC1_CLK			30000000
			#define MMC1_CCLK_MAX	30000000
			#define MMC1_CCLK_MIN	300000
		#else
			#define MMC0_CLK             48000000
			#define MMC0_CCLK_MAX      480000000
			#define MMC0_CCLK_MIN          300000  /*?*/
			#define MMC1_CLK			  48000000
			#define MMC1_CCLK_MAX	     480000000
			#define MMC1_CCLK_MIN	         300000		
		#endif
	#else
		#define MMC0_CLK             48000000//24000000   /*SDXC?*//*y00186965*/
		#define MMC0_CCLK_MAX      100000000  /*y00186965 ?*/
		#define MMC0_CCLK_MIN          400000  /*?*/
		#define MMC1_CLK			48000000
		#define MMC1_CCLK_MAX	48000000
		#define MMC1_CCLK_MIN	300000
	#endif
	/*--by pandong cs*/
#elif((defined (BOARD_ASIC) ||defined(BOARD_SFT))\
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
#define MMC1_CLK			50000000//实际 为48Mhz，为方便配置分频系数，写成 50Mhz
#define MMC1_CCLK_MAX	50000000//实际 为48Mhz，为方便配置分频系数，写成 50Mhz
#define MMC1_CCLK_MIN	300000
#else
#error "Invalid Platform Configure!"
#endif


#define ENABLE            1
#define DISABLE           0
#define POWER_ON          0
#define POWER_OFF         1

/* Base address of SD card register */
#define HI_MCI_IO_SIZE             0x00001000

/* Base address of SDIO card register */
//there are three mmc control in balongv3r2, to make sure use which one

#define HI_MCI_BASE0           0x900ad000   
#define HI_MCI_BASE1			0x900ae000
#define HI_MCI_BASE2			0x900af000
#define HI_MCI_INTR0           107              
#define HI_MCI_INTR1			108
#define HI_MCI_INTR2			109

/* Base address of system control register */
#define SCTL_BASE                   0x90000000//0x20040000//FIXME REG_BASE_SCTL//0x20040000     
#ifdef MMC1_SYSTEM_CTRL
	#define SCTL_BASE_ADDR              IO_ADDRESS(SCTL_BASE)
#else
	#define SCTL_BASE_ADDR              g_sysctrl_base
#endif
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))  
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#define IOMG_BASE_ADDR 			0x90004000
		#define IOMG_REG_SIZE				0x0ff
		#define SCTL_SC_RSTCTRL2            (SCTL_BASE_ADDR+0x48)
		#define SCTL_SC_PERCTRL2            (SCTL_BASE_ADDR + 0x4c)
		#define SCTL_SC_SIZE                (0x00001000)

		/* 0:soft reset  1:undo soft reset */
		#define SCTL_SC_PEREN                (SCTL_BASE_ADDR + 0x24)
		#define SCTL_SC_PERDIS               (SCTL_BASE_ADDR + 0x28)
		#define SCTL_SC_PERCLKEN             (SCTL_BASE_ADDR + 0x2C)
		#define SCTL_SC_PERSTAT              (SCTL_BASE_ADDR + 0x30)

		#define SCTL_SC_PERCTRL0	(SCTL_BASE_ADDR + 0x1c)
	
		#define SDIO_MOD_NAME                "HISDIO"

		/*syb*/
		#define  MMC0_CLK_BIT                  (12) 
		#define  MMC1_CLK_BIT                  (13)
		#define  MMC2_CLK_BIT                  (14)
		#define  SCTL_SC_PERCTRL0_RESTEN0      (SCTL_BASE_ADDR + 0x84)
		#define  SCTL_SC_PERCTRL0_RESTDIS0     (SCTL_BASE_ADDR + 0x88)
		#define  SCTL_SC_PERCTRL               (SCTL_BASE_ADDR + 0xD4)
		#define  SCTL_SC_CLKEN1                (SCTL_BASE_ADDR + 0x60)
		#define  SCTL_SC_CLKDIS1               (SCTL_BASE_ADDR + 0x64)
		#define  SCTL_SC_CLKSTAT1              (SCTL_BASE_ADDR + 0x68)
		#define  SCTL_SC_PPLL				   (SCTL_BASE_ADDR + 0x3C)
		#define  SCTL_INT_CLEAR				   (SCTL_BASE_ADDR + 0xCC)
		#define  SCTL_INT_EN1				   (SCTL_BASE_ADDR + 0x160)
		#define  SCTL_INT_STATM1         	   (SCTL_BASE_ADDR + 0x164)
		#define  SDMMC0_WR_OVER_INT_STAT       (0x1 << 0)
		
		#define  MMC0_BUSY_INT_STAT				  0
		#define  MMC0_BUSY_INT_CLEAR              4
		#define  MMC0_BUSY_INT_EN                 0

		#define MMC0_DIV                         3
		#define MMC0_DIV_BIT                     0
		#define MMC0_DIV_NUM                     3    /*10:40M,11:30M;其余:60M*/
		#define MMC0_SRST                        1

		#define SD_GPIO_INT_ID                   2
		#define SD_GPIO_INT_PIN                  5

		#define INT_GPIO_2                       114
		#define INT_GPIO_9						  152
		#define  INT_SCTL						 126


		/*syb end*/
		/* mmc1 div coefficient */
		#define MMC1_DIV				3
		#define MMC1_DIV_BIT			4 
		#define MMC1_DIV_NUM		3
		#define MMC1_SRST			2

		/* mmc2 div coefficient */
		#define MMC2_DIV				3
		#define MMC2_DIV_BIT			7 
		#define MMC2_DIV_NUM		3
		#define MMC2_SRST			3
		#define INT_GPIO_0                      112
		#define IOCFG_BASE                  IO_ADDRESS(REG_BASE_IOCFG)
	#else	
		#define SCTL_IOS_CTRL16 (SCTL_BASE_ADDR+0x0840)    //gpio_1_00 io control
		#define SCTL_IOS_CTRL98 (SCTL_BASE_ADDR+0x0988)    //mmc0 io control
		#define SCTL_CGR_CTRL3 (SCTL_BASE_ADDR+0x000C)	    //mmc0 clock enable
		#define SCTL_CGR_CTRL4 (SCTL_BASE_ADDR+0x0010)	    //mmc0 clock disable
		#define SCTL_CGR_CTRL12 (SCTL_BASE_ADDR+0x0030)	    //mmc0 clk src
		#define SCTL_CGR_CTRL14 (SCTL_BASE_ADDR+0x0038)	    //mmc0 system reset req
		#define SCTL_CGR_CTRL16 (SCTL_BASE_ADDR+0x0040)	    //mmc0 clk div
		#define SCTL_CGR_STAT1 (SCTL_BASE_ADDR+0x0204)    //mmc0 clock state
		
		#define SCTL_IOS_CTRL83 (SCTL_BASE_ADDR+0x094C)    //mmc0 pin ctrl
		
		#define IOS_GMII_CTRL_BIT	21
		#define IOS_MMC0_CTRL_BIT	6
		#define IOS_MMC0_CTRL1_BIT	27
		
		#define MMC0_SRST			21
		#define MMC1_SRST			22
		#define MMC2_SRST			23
		
		#define MMC0_CLK_BIT                  (14) 
		#define MMC1_CLK_BIT                  (15)
		#define MMC2_CLK_BIT                  (16)
		
		#define MMC_CLK_SRC_BIT		2
		#define MMC_CLK_SRC_19_2M		0
		#define MMC_CLK_SRC_384M		1
		
		#define MMC0_CLK_DIV				0x8
		#define MMC0_CLK_DIV_BIT			0
		#define MMC0_CLK_DIV_MASK				0x3F

		#define INT_GPIO_0                      112   /*y00186965*/
		#define SCTL_SC_SIZE                (0x00001000)
	#endif
	/*--by pandong cs*/
#elif ((defined (BOARD_ASIC) ||defined(BOARD_SFT))\
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#define SCTL_IOS_CTRL16 (SCTL_BASE_ADDR+0x0840)    //gpio_1_00 io control
#define SCTL_IOS_CTRL98 (SCTL_BASE_ADDR+0x0988)    //mmc0 io control
#define SCTL_CGR_CTRL3 (SCTL_BASE_ADDR+0x000C)	    //mmc0 clock enable
#define SCTL_CGR_CTRL4 (SCTL_BASE_ADDR+0x0010)	    //mmc0 clock disable
#define SCTL_CGR_CTRL12 (SCTL_BASE_ADDR+0x0030)	    //mmc0 clk src
#define SCTL_CGR_CTRL14 (SCTL_BASE_ADDR+0x0038)	    //mmc0 system reset req
#define SCTL_CGR_CTRL16 (SCTL_BASE_ADDR+0x0040)	    //mmc0 clk div
#define SCTL_CGR_STAT1 (SCTL_BASE_ADDR+0x0204)    //mmc0 clock state

#define IOS_GMII_CTRL_BIT	21
#define IOS_MMC0_CTRL_BIT	6
#define IOS_MMC0_CTRL1_BIT	27

#define MMC0_SRST			21
#define MMC1_SRST			22
#define MMC2_SRST			23

#define MMC0_CLK_BIT                  (14) 
#define MMC1_CLK_BIT                  (15)
#define MMC2_CLK_BIT                  (16)

#define MMC_CLK_SRC_BIT		2
#define MMC_CLK_SRC_19_2M		0
#define MMC_CLK_SRC_384M		1

#define MMC0_CLK_DIV				0x8
#define MMC0_CLK_DIV_BIT			0
#define MMC0_CLK_DIV_MASK				0x3F

#define INT_GPIO_0                      112   
#define SCTL_SC_SIZE                (0x00001000)

#define INT_GPIO_2						114

#else
#define SCTL_SC_RSTCTRL2            (SCTL_BASE_ADDR+0x48)
#define SCTL_SC_PERCTRL2            (SCTL_BASE_ADDR + 0x4c)
#define SCTL_SC_SIZE                (0x00001000)

/* 0:soft reset  1:undo soft reset */
#define SCTL_SC_PEREN                (SCTL_BASE_ADDR + 0x24)
#define SCTL_SC_PERDIS               (SCTL_BASE_ADDR + 0x28)
#define SCTL_SC_PERCLKEN             (SCTL_BASE_ADDR + 0x2C)
#define SCTL_SC_PERSTAT              (SCTL_BASE_ADDR + 0x30)

#define SCTL_SC_PERCTRL0	(SCTL_BASE_ADDR + 0x1c)

#define SDIO_MOD_NAME                "HISDIO"

/*syb*/
#define  MMC0_CLK_BIT                  (12) 
#define  MMC1_CLK_BIT                  (13)
#define  MMC2_CLK_BIT                  (14)
#define  SCTL_SC_PERCTRL0_RESTEN0      (SCTL_BASE_ADDR + 0x84)
#define  SCTL_SC_PERCTRL0_RESTDIS0     (SCTL_BASE_ADDR + 0x88)
#define  SCTL_SC_PERCTRL               (SCTL_BASE_ADDR + 0xD4)
#define  SCTL_SC_CLKEN1                (SCTL_BASE_ADDR + 0x60)
#define  SCTL_SC_CLKDIS1               (SCTL_BASE_ADDR + 0x64)
#define  SCTL_SC_CLKSTAT1              (SCTL_BASE_ADDR + 0x68)
#define  SCTL_SC_PPLL				   (SCTL_BASE_ADDR + 0x3C)
#define  SCTL_INT_CLEAR				   (SCTL_BASE_ADDR + 0xCC)
#define  SCTL_INT_EN1				   (SCTL_BASE_ADDR + 0x160)
#define  SCTL_INT_STATM1         	   (SCTL_BASE_ADDR + 0x164)
#define  SDMMC0_WR_OVER_INT_STAT       (0x1 << 0)

#define  MMC0_BUSY_INT_STAT				  0
#define  MMC0_BUSY_INT_CLEAR              4
#define  MMC0_BUSY_INT_EN                 0

#define MMC0_DIV                         3
#define MMC0_DIV_BIT                     0
#define MMC0_DIV_NUM                     3    /*10:40M,11:30M;其余:60M*/
#define MMC0_SRST                        1

#define SD_GPIO_INT_ID                   2
#define SD_GPIO_INT_PIN                  5

#define INT_GPIO_2                       114
#define  INT_SCTL						 126


/*syb end*/
/* mmc1 div coefficient */
#define MMC1_DIV				3
#define MMC1_DIV_BIT			4 
#define MMC1_DIV_NUM		3
#define MMC1_SRST			2

/* mmc2 div coefficient */
#define MMC2_DIV				3
#define MMC2_DIV_BIT			7 
#define MMC2_DIV_NUM		3
#define MMC2_SRST			3

#define IOCFG_BASE                  IO_ADDRESS(REG_BASE_IOCFG)
#endif

//set sdio-gpio sd-mode      //mgh iomg021
//set sdio-gpio property	//mgh mmc2 iocfg086---091
//set wlan-gpio gpio-mode  //mgh iomg006
//set wlan-gpio property	 //mgh gpio iocg030 032 033

#define pin_config(x) do{        \
	writel(0x0 ,x + 0x0054); \
	writel(0x14,x + 0x0958); \
	writel(0x17,x + 0x095c); \
	writel(0x17,x + 0x0960); \
	writel(0x17,x + 0x0964); \
	writel(0x17,x + 0x0968); \
	writel(0x17,x + 0x096c); \
	writel(0x01,x + 0x0018); \
	writel(0x10,x + 0x0878); \
	writel(0x17,x + 0x0880); \
}while(0)

#define pin_config_4329(x) do{        \
	writel(0x0 ,x + 0x0054); \
	writel(0x14,x + 0x0958); \
	writel(0x17,x + 0x095c); \
	writel(0x17,x + 0x0960); \
	writel(0x17,x + 0x0964); \
	writel(0x17,x + 0x0968); \
	writel(0x17,x + 0x096c); \
	writel(0x01,x + 0x0018); \
	writel(0x10,x + 0x0878); \
	writel(0x17,x + 0x0880); \
	writel(0x10,x + 0x088C); \
}while(0)


//mgh 100527 add begin
#define pin_reset(x) do{        \
	writel(0x1 ,x + 0x0054); \
	writel(0x05,x + 0x0958); \
	writel(0x05,x + 0x095c); \
	writel(0x05,x + 0x0960); \
	writel(0x05,x + 0x0964); \
	writel(0x05,x + 0x0968); \
	writel(0x05,x + 0x096c); \
}while(0)
//mgh 100527 add end

void sys_ctrl_balong_sdio_init_io(void);
void sys_ctrl_balong_sdio_power_io(unsigned int flag);
unsigned int sys_ctrl_balong_sdio_card_detect_io(void);
unsigned int sys_ctrl_balong_sdio_card_readonly_io(void);
void sys_ctrl_balong_sdio_exit_io(void);//mark to NULL


#ifndef MMC1_SYSTEM_CTRL
#if (BALONG_PORT_ADP)
void sys_ctrl_hisdio_power_io(unsigned int flag);
unsigned int sys_ctrl_hisdio_card_detect_io(void);
unsigned int sys_ctrl_hisdio_card_readonly_io(void);
#else
void sys_ctrl_hisdio_power_io(unsigned int flag)
{
	return;
}

unsigned int sys_ctrl_hisdio_card_detect_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}

unsigned int sys_ctrl_hisdio_card_readonly_io(void)
{
	unsigned int card_status = 0;

	return card_status;
}
#endif

#if (BALONG_PORT_ADP)
void sys_ctrl_hisdio_exit_io(void);//mark to NULL
void sys_ctrl_hisdio_init_io(void);
void wlan_clock_ctl(int fg);
void wlan_input_clock_ctl(int fg);
void wlan_gpio_low_power(void);
#else
void sys_ctrl_hisdio_exit_io(void){return;};//mark to NULL
/*void sys_ctrl_hisdio_init_io(void){return;};*/
/*void wlan_clock_ctl(int fg){return;};*/
void wlan_input_clock_ctl(int fg){return;};
void wlan_gpio_low_power(void){return;};
#endif
#endif

#if defined (BOARD_SFT) && defined (VERSION_V3R2) || defined (BOARD_FPGA) && defined (VERSION_V3R2)
	/*GPIO2_0 sft wifi reset pin*/
	#define BALONG_GPIO_WIFI_RESET_PIN     0
	#define BALONG_GPIO_WIFI_RESET         BALONG_GPIO_2(BALONG_GPIO_WIFI_RESET_PIN)
#elif defined (BOARD_SFT) && defined (VERSION_V7R1) || defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#if defined(CS_SFT)
			/*GPIO2_0 sft wifi reset pin*/
			#define BALONG_GPIO_WIFI_RESET_PIN     0
			#define BALONG_GPIO_WIFI_RESET         BALONG_GPIO_2(BALONG_GPIO_WIFI_RESET_PIN)
		#else
			/*GPIO2_0 sft wifi reset pin*/
			#define BALONG_GPIO_WIFI_RESET_PIN     6
			#define BALONG_GPIO_WIFI_RESET         BALONG_GPIO_5(BALONG_GPIO_WIFI_RESET_PIN)		
		#endif
	#else
		/*GPIO1_00 V7R1 wifi reset pin*/
		#define BALONG_GPIO_WIFI_RESET_PIN     0
		#define BALONG_GPIO_WIFI_RESET         BALONG_GPIO_1(BALONG_GPIO_WIFI_RESET_PIN)
	#endif
	/*--by pandong cs*/
	
#elif ((defined (BOARD_ASIC) ||defined(BOARD_SFT))\
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))

#else
#error "there is no file included!"
#endif



#endif


