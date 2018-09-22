/*
 * balongv3r2_gpio.c - hisilicon balong gpio driver
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include "BSP.h"

#define DRIVER_NAME	"balongv3r2-gpio"
#define LABEL_NAME  "balongv3r2"
static DEFINE_SPINLOCK(gpio_lock); /*lint !e43*/

//static unsigned long gpio_flags;
/*register base address*/
/*y00186965 begin*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_ASIC) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) \
    || ((defined (BOARD_ASIC)||defined (BOARD_SFT)) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) /*y00186965*/ /*sunjian:与芯片确认CS*/
	/*V3R2 CS f00164371*/
    #if defined(CHIP_BB_6756CS)
	    void __iomem *balong_gpio_base;
        #define GPIO0_BASE	(balong_gpio_base + 0x0000)
        #define GPIO1_BASE	(balong_gpio_base + 0x1000)
        #define GPIO2_BASE	(balong_gpio_base + 0x2000)
        #define GPIO3_BASE	(balong_gpio_base + 0x3000)
        #define GPIO4_BASE	(balong_gpio_base + 0x4000)
        #define GPIO5_BASE	(balong_gpio_base + 0x5000)
        #define GPIO6_BASE	(balong_gpio_base + 0x6000)
        #define GPIO7_BASE	(balong_gpio_base + 0x7000)
        #define GPIO8_BASE	(balong_gpio_base + 0x8000)
        #define GPIO9_BASE	(balong_gpio_base + 0x9000)
        #define GPIO10_BASE (balong_gpio_base + 0xa000)
        #define GPIO11_BASE (balong_gpio_base + 0xb000)
	#elif defined(CHIP_BB_6920CS)
	    void __iomem *balong_gpio0_base;/*lint !e129*/
        void __iomem *balong_gpio1_base;
		void __iomem *balong_gpio2_base;
		void __iomem *balong_gpio3_base;
        #define GPIO0_BASE	(balong_gpio0_base + 0x0000)
        #define GPIO1_BASE	(balong_gpio1_base + 0x0000)
        #define GPIO2_BASE	(balong_gpio2_base + 0x0000)
        #define GPIO3_BASE	(balong_gpio3_base + 0x0000)
        #define GPIO4_BASE	(balong_gpio3_base + 0x1000)
        #define GPIO5_BASE	(balong_gpio3_base + 0x2000)
	#else
        void __iomem *balong_gpio0_base;
        void __iomem *balong_gpio1_base;
        #define GPIO0_BASE	(balong_gpio0_base + 0x0000)
        #define GPIO1_BASE	(balong_gpio1_base + 0x0000)
        #define GPIO2_BASE	(balong_gpio1_base + 0x1000)
        #define GPIO3_BASE	(balong_gpio1_base + 0x2000)
        #define GPIO4_BASE	(balong_gpio1_base + 0x3000)
        #define GPIO5_BASE	(balong_gpio1_base + 0x4000)
    #endif
#else
    void __iomem *balong_gpio_base;
    #define GPIO0_BASE	(balong_gpio_base + 0x0000)
    #define GPIO1_BASE	(balong_gpio_base + 0x1000)
    #define GPIO2_BASE	(balong_gpio_base + 0x2000)
    #define GPIO3_BASE	(balong_gpio_base + 0x3000)
    #define GPIO4_BASE	(balong_gpio_base + 0x4000)
    #define GPIO5_BASE	(balong_gpio_base + 0x5000)
    #define GPIO6_BASE	(balong_gpio_base + 0x6000)
    #define GPIO7_BASE	(balong_gpio_base + 0x7000)
    #define GPIO8_BASE	(balong_gpio_base + 0x8000)
    #define GPIO9_BASE	(balong_gpio_base + 0x9000)
    #define GPIO10_BASE (balong_gpio_base + 0xa000)
    #define GPIO11_BASE (balong_gpio_base + 0xb000)
#endif
/*y00186965 end*/

GPIOISR g_gpioisr[GPIO_MAXIMUM][GPIO_MAX_PINS]= {{{NULL},{0}}};   /* modefied by zsc for pclint*/
#if (FEATURE_USB_SWITCH == FEATURE_ON)
#if defined(CHIP_BB_6920ES)
#define EPAD_MODEM_GPIO_GROUPNUM        (2)
#define EPAD_MODEM_GPIO_PINNUM          (0)
#elif defined(CHIP_BB_6920CS)
#define EPAD_MODEM_GPIO_GROUPNUM        (2)
#define EPAD_MODEM_GPIO_PINNUM          (7)
#else
#error "unknown chip type !!"
#endif
#define EPAD_MODEM_SIGNAL_UP            (1)
#define EPAD_MODEM_SIGNAL_DOWN          (0)
#define EPAD_MODEM_GPIO_OUTPUT          (1)
#define EPAD_MODEM_GPIO_INPUT           (0)
#endif

/*function declaration*/
static int balongv3r2_gpio_to_irq(struct gpio_chip *chip, unsigned int offset);
static int balongv3r2_gpio_direction_input(struct gpio_chip *chip, unsigned int offset);
static int balongv3r2_gpio_direction_output(struct gpio_chip *chip, unsigned int offset, int value);
static int balongv3r2_gpio_get(struct gpio_chip *chip, unsigned int pin);
static void balongv3r2_gpio_set(struct gpio_chip *chip, unsigned int pin, int value);
static UINT32 gpio_base_addr_get (UINT32 ucId);
static int balongv3r2_gpio_to_chip_num(unsigned int gpio,unsigned *chip_num, unsigned *pin);
#if (FEATURE_USB_SWITCH == FEATURE_ON)
int USB_otg_switch_signal_set(UINT8 group,UINT8 pin, UINT8 value );
#endif
/*Error 135: (Error -- comma assumed in initializer)*/
/*lint -e135*/
/*lint -e156*/
/*lint -e651*/
static struct gpio_chip balongv3r2_gpio_chip[GPIO_MAXIMUM] = {
	[0 ... GPIO_MAXIMUM - 1] = {/*lint !e64*/
	.label			= LABEL_NAME,/*lint !e63 !e35 !e24*/
	.owner			= THIS_MODULE,
	.direction_input	= balongv3r2_gpio_direction_input,
	.get			= balongv3r2_gpio_get,
	.direction_output	= balongv3r2_gpio_direction_output,
	.set			= balongv3r2_gpio_set,
	.to_irq			= balongv3r2_gpio_to_irq,
	}
};
/*lint +e651*/
/*lint +e156*/
/*lint +e135*/

/*set bit pin 0~7 to 1 or 0*/
#undef GPIO_REG_PIN_SET
#define GPIO_REG_PIN_SET(ucId,reg,data,pin)	  \
	 do{ \
	 	UINT32 result; \
	 	UINT32 tmp; \
	 	GPIO_REG_READ(gpio_base_addr_get((ucId)) + (reg), result);\
	 	tmp = (result & ~(1<<(pin))) |( (!!(data) ) << (pin)); \
	 	GPIO_REG_WRITE(gpio_base_addr_get((ucId)) + reg, tmp);\
	 	}while(0)\

/*************************************************
  Function:       gpio_direction_get
  Description:    Get GPIO Direction value
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          UINT8 ucId, UINT8 ucMask, UINT8 *pucDirections
  Output:         NONE
  Return:         GPIO_OK : successfully
                  GPIO_ERROR: fail
  Others:
************************************************/
int gpio_direction_get(UINT32 ucId, UINT32 * pucDirections)
{
    if((ucId >= GPIO_MAXIMUM)||(NULL == pucDirections))
    {
        return GPIO_ERROR;
    }

    GPIO_REG_READ((gpio_base_addr_get(ucId)+GPIO_DIRECIONT_OFFSET), *pucDirections);
    return GPIO_OK;
}

/*************************************************
  Function:       gpio_direction_set
  Description:    Set GPIO Direction value
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          UINT8 ucId, UINT8 ucMask, UINT8 ucDirections
  Output:         NONE
  Return:         GPIO_OK : successfully
                  GPIO_ERROR: fail
  Others:
************************************************/
static int gpio_direction_set(UINT32 ucId, UINT32 ucMask, UINT32 ucDirections)
{
    UINT32 ucRegValue;

    if(ucId >= GPIO_MAXIMUM)
    {
        return GPIO_ERROR;
    }

    GPIO_REG_READ(gpio_base_addr_get(ucId)+GPIO_DIRECIONT_OFFSET, ucRegValue);
    GPIO_REG_WRITE(gpio_base_addr_get(ucId)+GPIO_DIRECIONT_OFFSET, (ucRegValue & (~ucMask))|(ucDirections & ucMask));
    return GPIO_OK;
}

/*************************************************
  Function:      gpio_value_get
  Description:    Get GPIO data value
  Calls:            Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          UINT8 ucId, UINT8 * pucData
  Output:         NONE
  Return:         GPIO_OK : successfully
                  GPIO_ERROR: fail
  Others:
************************************************/
int gpio_value_get(UINT32 ucId, UINT32 * pucData)
{
    if((ucId >= GPIO_MAXIMUM)||(NULL == pucData))
    {
        return GPIO_ERROR;
    }

    GPIO_REG_READ(gpio_base_addr_get(ucId)+GPIO_EXT_PORTA, *pucData);
    return GPIO_OK;
}


/*************************************************
  Function:      gpio_value_set
  Description:    Set GPIO data value
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          UINT8 ucId, UINT8 ucMask, UINT8 ucData
  Output:         NONE
  Return:         GPIO_OK : successfully
                  GPIO_ERROR: fail
  Others:
************************************************/
int gpio_value_set(UINT32 ucId, UINT32 ucMask, UINT32 ucData)
{
    UINT32 ucRegValue;

    if(ucId >= GPIO_MAXIMUM)
    {
        return GPIO_ERROR;
    }

    GPIO_REG_READ(gpio_base_addr_get(ucId)+GPIO_DATA_OFFSET, ucRegValue);
    GPIO_REG_WRITE(gpio_base_addr_get(ucId)+GPIO_DATA_OFFSET, (ucRegValue & (~ucMask)) | (ucData & ucMask));
    return GPIO_OK;
}

/*************************************************
  Function:      gpioBaseAdrrGet
  Description:    Get GPIO 0~11 Base Address
  Calls:          NONE

  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          UINT8 ucId
  Output:         BASE_ADDR_GPIO(i)
  Return:         Base Address
  Others:
************************************************/
static UINT32 gpio_base_addr_get (UINT32 ucId)
{
    /*lint -e124*/
     switch(ucId)
    {
        case GPIO0:
            return (UINT32)GPIO0_BASE;

        case GPIO1:
            return (UINT32)GPIO1_BASE;

        case GPIO2:
            return (UINT32)GPIO2_BASE;

        case GPIO3:
            return (UINT32)GPIO3_BASE;

        case GPIO4:
            return (UINT32)GPIO4_BASE;

        case GPIO5:
            return (UINT32)GPIO5_BASE;

/*y00186965 begin*/
/*V3R2 CS f00164371，与SFT平台一致*/
#if (defined (BOARD_SFT) && defined (VERSION_V3R2)) || \
    (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS))
        case GPIO6:
            return (UINT32)GPIO6_BASE;

        case GPIO7:
            return (UINT32)GPIO7_BASE;

        case GPIO8:
            return (UINT32)GPIO8_BASE;

        case GPIO9:
            return (UINT32)GPIO9_BASE;

        case GPIO10:
            return (UINT32)GPIO10_BASE;

        case GPIO11:
            return (UINT32)GPIO11_BASE;
#endif
/*y00186965 end*/
        default:
            return (UINT32)0xFFFFFFFF;

    }
     /*lint +e124*/


}

/*follow functions were used to adapt to Linux gpio module, refer to balongv3r2_gpio.c */
int gpio_chip_set_direction(unsigned int chip_num, unsigned int pin, unsigned int direction)
{
	int ret = 0;
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	UINT32 ucMask = 1 << pin;
	UINT32  ucDirections = direction << pin;
	higpio_trace(0, "[ucId :0x%x], [ucMask :0x%x], [ucDirections :0x%x]", ucId, ucMask, ucDirections);
	spin_lock_irqsave(&gpio_lock, flags);   /*lint !e26 !e515 */
	ret = gpio_direction_set(ucId, ucMask, ucDirections);
	spin_unlock_irqrestore(&gpio_lock, flags);
	if(GPIO_OK == ret)
		return 0;
	else
		return -EINVAL;
}
EXPORT_SYMBOL_GPL(gpio_chip_set_direction);

int gpio_chip_get_direction(unsigned int chip_num, unsigned int pin, unsigned int * direction)
{
	UINT32 direction_t;
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	if(gpio_direction_get(ucId, &direction_t))
	{
		spin_unlock_irqrestore(&gpio_lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&gpio_lock, flags);
	*direction = (direction_t >> pin);
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_chip_get_direction);

int gpio_chip_value_get(unsigned int chip_num, unsigned int pin,int* value)
{
	int ret = 0;
	UINT32  ucData;
	UINT32 ucId = chip_num;
	UINT32 ucMask = 1 << pin;
	ret = gpio_value_get(ucId, &ucData);
	if(GPIO_OK != ret)
	{
		return -EINVAL;
	}
	*value = (int)((ucData & ucMask) >>pin); /*convert data type,clean lint e713*/
	higpio_trace(0, "[chip_num :%u], [pin :%u], [ucData :0x%x], [value :%d]",
			chip_num, pin ,ucData, *value);
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_chip_value_get);

int gpio_chip_set_function(unsigned int chip_num, unsigned int pin, unsigned int function)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	function = !!function;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	GPIO_REG_PIN_SET(ucId,GPIO_INTEN,function,pin);/*lint !e502 */
	spin_unlock_irqrestore(&gpio_lock, flags);
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_chip_set_function);

int gpio_int_chip_mask_set(unsigned int chip_num, unsigned int pin)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);   /*lint !e26 !e515 */
	GPIO_REG_PIN_SET(ucId,GPIO_INTMASK_OFFSET,GPIO_INT_DISABLE,pin);/*lint !e502 */
	spin_unlock_irqrestore(&gpio_lock, flags);	
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_int_chip_mask_set);

int gpio_int_chip_unmask_set(unsigned int chip_num, unsigned int pin)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	GPIO_REG_PIN_SET(ucId,GPIO_INTMASK_OFFSET,GPIO_INT_ENABLE,pin);/*lint !e502  */
	spin_unlock_irqrestore(&gpio_lock, flags);	
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_int_chip_unmask_set);

int gpio_int_chip_state_get(unsigned int chip_num, unsigned int pin, unsigned int *state)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	UINT32 pucStat;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	GPIO_REG_READ(gpio_base_addr_get(ucId)+GPIO_INTSTATUS_OFFSET, pucStat);
	spin_unlock_irqrestore(&gpio_lock, flags);	
	*state = ((pucStat & (1 << pin)) >> pin);
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_int_chip_state_get);

int gpio_raw_int_chip_state_get(unsigned int chip_num, unsigned int pin, unsigned int *rawstate)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	UINT32 pucRawStat;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	GPIO_REG_READ(gpio_base_addr_get(ucId)+GPIO_INTRAWSTATUS_OFFSET, pucRawStat);
	spin_unlock_irqrestore(&gpio_lock, flags);
	*rawstate = ((pucRawStat & (1 << pin)) >> pin);
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_raw_int_chip_state_get);

int gpio_int_chip_state_clear(unsigned int chip_num, unsigned int pin)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	GPIO_REG_PIN_SET(ucId,GPIO_INTCLEAR_OFFSET,0x1,pin);/*lint !e502 */
	spin_unlock_irqrestore(&gpio_lock, flags);	
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_int_chip_state_clear);

int gpio_int_chip_trigger_set(unsigned int chip_num, unsigned int pin, unsigned int trigger_type)
{
	unsigned long flags = 0;
	UINT32 ucId = chip_num;
	UINT32 level_t;
	UINT32 polarity_t;
	if(ucId >= GPIO_MAXIMUM || pin >= GPIO_MAX_PINS)
	{
		return -EINVAL;
	}
	switch(trigger_type)
	{
		case IRQ_TYPE_EDGE_RISING:
			level_t = GPIO_INT_TYPE_EDGE;
			polarity_t = GPIO_INT_POLARITY_RIS_HIGH;
			break;
		case IRQ_TYPE_EDGE_FALLING:
			level_t = GPIO_INT_TYPE_EDGE;
			polarity_t = GPIO_INT_POLARITY_FAL_LOW;
			break;
		case IRQ_TYPE_EDGE_BOTH:
			/*do not support both edge trigger*/
			return -EINVAL;
		case IRQ_TYPE_LEVEL_HIGH:
			level_t = GPIO_INT_TYPE_LEVEVL;
			polarity_t = GPIO_INT_POLARITY_RIS_HIGH;
			break;
		case IRQ_TYPE_LEVEL_LOW:
			level_t = GPIO_INT_TYPE_LEVEVL;
			polarity_t = GPIO_INT_POLARITY_FAL_LOW;
			break;
		default:
			return -EINVAL;
	}
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	GPIO_REG_PIN_SET(ucId,GPIO_INTTYPE_LEVEL,level_t,pin);/*lint !e502  */
	GPIO_REG_PIN_SET(ucId,GPIO_INT_PLOARITY,polarity_t,pin);/*lint !e502  */
	spin_unlock_irqrestore(&gpio_lock, flags);	
	return 0;
}
EXPORT_SYMBOL_GPL(gpio_int_chip_trigger_set);

static int balongv3r2_gpio_to_irq(struct gpio_chip *chip, unsigned int offset)
{
	int chip_num = chip - balongv3r2_gpio_chip;
	if (offset >= chip->ngpio)
		return -EINVAL;
	higpio_trace(1, "chip num : %d", chip_num);
	/*segment 1, GPIO 0~5, irq:112 ~ 117*/
	if(chip_num < GPIO_SEPARATE_GPIO + 1)
	{
		return chip_num + GPIO_SEG_IRQ_BASE1;
	}
	/*segment 2, GPIO 6~11, irq: 149 ~154*/
	else if(chip_num > GPIO_SEPARATE_GPIO)
	{
		return (chip_num - GPIO_SEPARATE_GPIO) + GPIO_SEG_IRQ_BASE2;
	}
	else
	{
		return -EINVAL;
	}
}

static int balongv3r2_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	unsigned int pin = offset;/*pin 0 ~ 7*/
	unsigned long flags = 0;  //b00198513 modified for pclint 530 
	/*begin lint-Info 732: (Info -- Loss of sign (initialization) (int to unsigned int))*/
	unsigned int chip_num = (unsigned int)(chip - balongv3r2_gpio_chip);/*chip group num 0~11*/
	/*end*/
	if((pin >= chip->ngpio) || (chip_num >= GPIO_MAXIMUM) ||( chip_num < 0))
		return -EINVAL;
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	if(gpio_chip_set_direction(chip_num, pin, GPIO_INPUT))
	{
		spin_unlock_irqrestore(&gpio_lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&gpio_lock, flags);
	return 0;
}

static int balongv3r2_gpio_direction_output(struct gpio_chip *chip, unsigned int offset, int value)
{
	unsigned long flags = 0;
	unsigned int pin = offset;/*pin 0 ~ 7*/
	/*begin lint-Info 732: (Info -- Loss of sign (initialization) (int to unsigned int))*/
	unsigned int chip_num = (unsigned int)(chip - balongv3r2_gpio_chip);/*chip group num 0~11*/
	/*end*/
	if((pin >= chip->ngpio) || (chip_num >= GPIO_MAXIMUM) ||( chip_num < 0))
		return -EINVAL;
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	if(gpio_chip_set_direction(chip_num, pin, GPIO_OUTPUT))
	{
		spin_unlock_irqrestore(&gpio_lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&gpio_lock, flags);
	/*set default gpio value*/
	chip->set(chip, offset, value);
	return 0;
}

static int balongv3r2_gpio_get(struct gpio_chip *chip, unsigned int pin)
{
	int value;
	unsigned long flags = 0;
	/*begin lint-Info 732: (Info -- Loss of sign (initialization) (int to unsigned int))*/
	unsigned int chip_num = (unsigned int)(chip - balongv3r2_gpio_chip);/*chip group num 0~11*/
	/*end*/
	if((pin >= chip->ngpio) || (chip_num >= GPIO_MAXIMUM) ||( chip_num < 0))
		return -EINVAL;
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	if(gpio_chip_value_get(chip_num, pin,  &value))
	{
		spin_unlock_irqrestore(&gpio_lock, flags);
		return -EINVAL;
	}
	spin_unlock_irqrestore(&gpio_lock, flags);
	return value;
}

static void balongv3r2_gpio_set(struct gpio_chip *chip, unsigned int pin,
			    int value)
{
	unsigned long flags = 0;
	unsigned int direction;
	/*begin lint-Info 732: (Info -- Loss of sign (initialization) (int to unsigned int))*/
	unsigned int chip_num = (unsigned int)(chip - balongv3r2_gpio_chip);/*chip group num 0~11*/
	/*end*/
	value = !!value;
	if((pin >= chip->ngpio) || (chip_num >= GPIO_MAXIMUM) ||( chip_num < 0))
		return ;	
	spin_lock_irqsave(&gpio_lock, flags);/*lint !e26 !e515 */
	gpio_chip_get_direction(chip_num, pin, &direction);
	if(GPIO_INPUT == direction)//FIXME only support output mode
	{
		spin_unlock_irqrestore(&gpio_lock, flags);
		return;
	}
	GPIO_REG_PIN_SET(chip_num, GPIO_DATA_OFFSET, value, pin);/*lint !e502 */
	spin_unlock_irqrestore(&gpio_lock, flags);
	higpio_trace(1, "[chip_num :%d],[pin :%d],[value :%d]", 
				chip_num, pin , value);
}


static int balongv3r2_gpio_to_chip_num(unsigned int gpio,unsigned *chip_num, unsigned *pin)
{
    if(gpio < 0 || gpio > (GPIO_MAXIMUM*GPIO_MAX_PINS - 1))
    {
        return -EINVAL;
    }
    *chip_num   = gpio/GPIO_MAX_PINS ;
    *pin       = gpio%GPIO_MAX_PINS  ;
    higpio_trace(1, "chip no. %d, pin : %d", *chip_num, *pin);
    return 0;
}

int gpio_get_direction(unsigned int gpio, unsigned* direction)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_chip_get_direction(chip_num, pin, direction);
}
EXPORT_SYMBOL_GPL(gpio_get_direction);

int gpio_set_function(unsigned int gpio, unsigned function)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_chip_set_function(chip_num, pin, function);
}
EXPORT_SYMBOL_GPL(gpio_set_function);

int  gpio_int_mask_set(unsigned int gpio)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_int_chip_mask_set(chip_num,pin);
}
EXPORT_SYMBOL_GPL(gpio_int_mask_set);

int gpio_int_unmask_set(unsigned int gpio)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_int_chip_unmask_set(chip_num,pin);
}
EXPORT_SYMBOL_GPL(gpio_int_unmask_set);

int gpio_int_state_get(unsigned int gpio, unsigned * state)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_int_chip_state_get(chip_num, pin, state);
}
EXPORT_SYMBOL_GPL(gpio_int_state_get);

int gpio_raw_int_state_get(unsigned int gpio, unsigned * rawstate)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_raw_int_chip_state_get(chip_num, pin,rawstate);
}
EXPORT_SYMBOL_GPL(gpio_raw_int_state_get);

int gpio_int_state_clear(unsigned int gpio)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_int_chip_state_clear(chip_num, pin);
}
EXPORT_SYMBOL_GPL(gpio_int_state_clear);

int gpio_int_trigger_set(unsigned int gpio, unsigned int trigger_type)
{
    unsigned chip_num, pin;
    int ret ;
    ret = balongv3r2_gpio_to_chip_num(gpio, &chip_num, &pin);
    if(ret)
    {
        return ret;
    }
    return gpio_int_chip_trigger_set(chip_num, pin, trigger_type);
}
EXPORT_SYMBOL_GPL(gpio_int_trigger_set);
#if (FEATURE_PHONE == FEATURE_ON)
#if defined(CHIP_BB_6920CS)
/*  This function is used at C50_bugfix phone gutl, must pull gpio high 
    when system boot up*/
#define MODEM_STATUS_GPIO   BALONG_GPIO_2(0)
#else
#error please define MODEM STATUS GPIO
#endif
#endif
/*******************************************************************************
  Function:     BSP_Modem_OS_Status_Switch
  Description:  指示 Modem Ready 或者 非Ready状态

  Input:        int enable
                非0:设置Modem处于Ready状态 
                0:设置Modem处于非Ready状态

  Output:
  Return:       0:    操作成功
                -1:   操作失败
*******************************************************************************/
int BSP_Modem_OS_Status_Switch(int enable)
{
#if (FEATURE_PHONE == FEATURE_ON)
    int gpio_status = !enable;
    if(WARN_ON(gpio_request(MODEM_STATUS_GPIO, "Modem_OS_Status")))
	{
		printk(KERN_ERR"request gpio %d is busy!\n", MODEM_STATUS_GPIO);
		return -1;
	}
    
    gpio_set_value(MODEM_STATUS_GPIO, gpio_status);
    if(WARN_ON(gpio_get_value(MODEM_STATUS_GPIO) != gpio_status))
    {
        goto err;
    }
    gpio_free(MODEM_STATUS_GPIO);
	return 0;
err:
    gpio_free(MODEM_STATUS_GPIO);
    return -1;
#else
    /*stub interface*/
    return 0;
#endif
    
}
EXPORT_SYMBOL(BSP_Modem_OS_Status_Switch);

/*******************************************************************************
  Function:     USB_otg_switch_signal_set
  Description:  Modem向AP发送消息，释放Modem侧的USB口

  Input:        UINT8 group
                UINT8 pin
                UINT8 value

  Output:
  Return:       GPIO_OK:    操作成功
                GPIO_ERROR: 操作失败
*******************************************************************************/
int USB_otg_switch_signal_set(UINT8 group,UINT8 pin, UINT8 value )
{
#if (FEATURE_USB_SWITCH == FEATURE_ON)
    /*输入参数检测*/
    if((EPAD_MODEM_GPIO_GROUPNUM != group)
       || (EPAD_MODEM_GPIO_PINNUM != pin))
    {
        higpio_error("DRV_GPIO_SET PARA ERROR! group = %d, pin = %d, vlaue = %d.\n", group, pin, value);
        return -EINVAL;
    }

    if (EPAD_MODEM_SIGNAL_UP == value) /*GPIO的电平由低拉高*/
    {
        /*配置GPIO_2_00管脚的输出电平为低电平*/
        if (GPIO_OK != gpio_value_set((unsigned int)group, (unsigned int)(0x1 << pin), (unsigned int)(EPAD_MODEM_SIGNAL_DOWN << pin)))
        {
            higpio_error("gpio_value_set LOW before send ERROR!\n");
            return -EINVAL;
        }

    	/*配置GPIO_2_00管脚为输出；*/
    	if (GPIO_OK != gpio_direction_set((unsigned int)group, (unsigned int)(0x1 << pin), (unsigned int)(EPAD_MODEM_GPIO_OUTPUT << pin)))
        {
            higpio_error("gpio_direction_set ERROR!\n");
            return -EINVAL;
        }

    	/*配置GPIO_2_00管脚的输出电平为高电平；*/
    	if (GPIO_OK != gpio_value_set((unsigned int)group, (unsigned int)(0x1 << pin), (unsigned int)(value << pin)))
        {
            higpio_error("gpio_value_set HIGH ERROR!\n");
            return -EINVAL;
        }

    }
    else if (EPAD_MODEM_SIGNAL_DOWN == value)  /*GPIO的电平由高拉低*/
    {
        /*配置GPIO_2_00管脚为输出；*/
    	if (GPIO_OK != gpio_direction_set((unsigned int)group, (unsigned int)(0x1 << pin), (unsigned int)(EPAD_MODEM_GPIO_OUTPUT << pin)))
        {
            higpio_error("gpio_direction_set ERROR!\n");
            return -EINVAL;
        }

        /*配置GPIO_2_00管脚的输出电平为低电平*/
        if (GPIO_OK != gpio_value_set((unsigned int)group, (unsigned int)(0x1 << pin), (unsigned int)(EPAD_MODEM_SIGNAL_DOWN << pin)))
        {
            higpio_error("gpio_value_set LOW before send ERROR!\n");
            return -EINVAL;
        }
    }
    else
    {
        return GPIO_ERROR;
    }
#endif
    return GPIO_OK;
}
EXPORT_SYMBOL_GPL(USB_otg_switch_signal_set);
/*Error 63: (Error -- Expected an lvalue)*/
/*lint -e63*/
static int __devinit hi_gpio_probe(struct platform_device *pdev)
{
	int i;
	int ret = 0;
#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
    unsigned int state;
#define GPIO_SYNC_TIMEOUT   (10000)
#endif

	higpio_trace(1, "start");

/*++pandong cs*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_ASIC) && defined(VERSION_V3R2)) \
    || ((defined (BOARD_ASIC)||defined(BOARD_SFT)) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) /*y00186965*/ /*sunjian:与芯片确认CS*/
	#if defined(CHIP_BB_6756CS)
	struct resource *res;	
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -EBUSY;

	balong_gpio_base = ioremap(res->start, res->end - res->start + 1);
	if (!balong_gpio_base)
		return -ENOMEM;

	higpio_trace(1, "[base:0x%x],[start:0x%x],[end:0x%x]", (unsigned int)balong_gpio_base,
				res->start, res->end - res->start + 1);
	#elif defined(CHIP_BB_6920CS)
	balong_gpio0_base = ioremap(HI_GPIO0_BASE, HI_GPIO_REG_OFFSET);
    balong_gpio1_base = ioremap(HI_GPIO1_BASE, HI_GPIO_REG_OFFSET);
	balong_gpio2_base = ioremap(HI_GPIO2_BASE, HI_GPIO_REG_OFFSET);
	balong_gpio3_base = ioremap(HI_GPIO3_BASE, HI_GPIO_REG_OFFSET * 3);
    if ((!balong_gpio0_base) || (!balong_gpio1_base) || (!balong_gpio2_base) || (!balong_gpio3_base))
		return -ENOMEM;
    higpio_trace(1, "balong_gpio0_base = 0x%X,phy base= 0x%X, size = 0x%X\n", (unsigned int)balong_gpio0_base,
				HI_GPIO0_BASE, HI_GPIO_REG_OFFSET);
    higpio_trace(1, "balong_gpio1_base = 0x%X,phy base= 0x%X, size = 0x%X\n", (unsigned int)balong_gpio1_base,
				HI_GPIO1_BASE, HI_GPIO_REG_OFFSET);
	higpio_trace(1, "balong_gpio2_base = 0x%X,phy base= 0x%X, size = 0x%X\n", (unsigned int)balong_gpio2_base,
				HI_GPIO0_BASE, HI_GPIO_REG_OFFSET);
    higpio_trace(1, "balong_gpio3_base = 0x%X,phy base= 0x%X, size = 0x%X\n", (unsigned int)balong_gpio3_base,
				HI_GPIO1_BASE, HI_GPIO_REG_OFFSET * 3);
	#else
    balong_gpio0_base = ioremap(HI_GPIO0_BASE, HI_GPIO_REG_OFFSET);
    balong_gpio1_base = ioremap(HI_GPIO1_BASE, HI_GPIO_REG_OFFSET * 5);
    if ((!balong_gpio0_base) || (!balong_gpio1_base))
		return -ENOMEM;
    higpio_trace(1, "balong_gpio0_base = 0x%X,phy base= 0x%X, size = 0x%X\n", (unsigned int)balong_gpio0_base,
				HI_GPIO0_BASE, HI_GPIO_REG_OFFSET);
    higpio_trace(1, "balong_gpio1_base = 0x%X,phy base= 0x%X, size = 0x%X\n", (unsigned int)balong_gpio1_base,
				HI_GPIO1_BASE, HI_GPIO_REG_OFFSET * 5);
	#endif
#else
	struct resource *res;
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -EBUSY;

	balong_gpio_base = ioremap(res->start, res->end - res->start + 1);
	if (!balong_gpio_base)
		return -ENOMEM;

	higpio_trace(1, "[base:0x%x],[start:0x%x],[end:0x%x]", (unsigned int)balong_gpio_base,
				res->start, res->end - res->start + 1);
#endif

#if (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
    ret = BSP_SYNC_Lock(SYNC_MODULE_GPIO, &state, GPIO_SYNC_TIMEOUT);
    if(ret)
    {
        printk("hi_gpio_probe sync lock timeout ret=0x%x, line:%d\n", ret, __LINE__);
    }

    if(0 == state)
    {
        printk("hi_gpio_probe:gpio sync in acore.\n");
        for(i = 0; i < GPIO_MAXIMUM; i++)
        {
        	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int)))*/
            gpio_int_state_clear((unsigned int)i);

            gpio_int_mask_set((unsigned int)i);
			/*end*/
        }       
    }

    state = 1;
    (void)BSP_SYNC_UnLock(SYNC_MODULE_GPIO, state);

    printk("hi_gpio_probe:gpio sync over.\n");
#endif

	for(i = 0; i < GPIO_MAXIMUM; i++)
	{
		balongv3r2_gpio_chip[i].dev = &pdev->dev;
		balongv3r2_gpio_chip[i].base = (i * GPIO_MAX_PINS);
		balongv3r2_gpio_chip[i].ngpio = GPIO_MAX_PINS;
		ret |= gpiochip_add(&balongv3r2_gpio_chip[i]);
		higpio_trace(0, "[group %d, base %d]", i, balongv3r2_gpio_chip[i].base);
	}
	if(ret)
	{
		higpio_error("gpiochip add error = %d", ret);
	}
	higpio_trace(1, "end");
	return ret;
}/*lint !e529*/

//add remove function
static int __devexit hi_gpio_remove(struct platform_device *pdev)
{
/*++pandong cs */
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_ASIC) && defined(VERSION_V3R2)) \
    || ((defined (BOARD_ASIC)||defined(BOARD_SFT)) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) /*y00186965*/ /*sunjian:与芯片确认CS*/
	#if defined(CHIP_BB_6756CS)
	if(!balong_gpio_base)
	{
		iounmap(balong_gpio_base);
		balong_gpio_base = NULL;
	}
	#elif defined(CHIP_BB_6920CS)
	if(!balong_gpio0_base)
	{
		iounmap(balong_gpio0_base);
		balong_gpio0_base = NULL;
	}
    if(!balong_gpio1_base)
	{
		iounmap(balong_gpio1_base);
		balong_gpio1_base = NULL;
	}
	if(!balong_gpio2_base)
	{
		iounmap(balong_gpio2_base);
		balong_gpio2_base = NULL;
	}
    if(!balong_gpio3_base)
	{
		iounmap(balong_gpio3_base);
		balong_gpio3_base = NULL;
	}
	#else
	if(!balong_gpio0_base)
	{
		iounmap(balong_gpio0_base);
		balong_gpio0_base = NULL;
	}
    if(!balong_gpio1_base)
	{
		iounmap(balong_gpio1_base);
		balong_gpio1_base = NULL;
	}
	#endif
#else
    if(!balong_gpio_base)
	{
		iounmap(balong_gpio_base);
		balong_gpio_base = NULL;
	}
#endif
	return 0;
}
/*lint +e63*/



#if (defined(BOARD_SFT) && defined(VERSION_V3R2)) || \
    (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS))
static struct resource hi_gpio_io_resources[] = {
	[0] = {
		.start          = HI_GPIO0_BASE,
		.end            = HI_GPIO0_BASE + HI_GPIO_REG_OFFSET*GPIO_MAXIMUM- 1,
		.flags          = IORESOURCE_MEM,
	},
};
#endif  

static void hi_gpio_platdev_release(struct device *dev)
{
}

static struct platform_device hi_gpio_device = {
	.name           = DRIVER_NAME,
	.id             = 1,
	.dev = {
		.release      = hi_gpio_platdev_release,
	},


#if (defined(BOARD_SFT) && defined(VERSION_V3R2)) || \
    (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS))
	.num_resources  = ARRAY_SIZE(hi_gpio_io_resources),
	.resource       = hi_gpio_io_resources,
#endif  
};


static struct platform_driver hi_gpio_driver = {
	.probe           = hi_gpio_probe,
	.remove 	     =__devexit_p(hi_gpio_remove),
//	.shutdown        = hi_sdio_shutdown,
//	.suspend        = hi_sdio_suspend,
//	.resume        = hi_sdio_resume,
	.driver        =
	{
		.name          = DRIVER_NAME,
	},
};

static int __init hi_gpio_init(void)
{
	int ret = 0;

	higpio_trace(1,"begin");

	ret = platform_device_register(&hi_gpio_device);
	if(ret)
	{

		higpio_error("Platform device register is failed!");
		return ret;
	}

	ret = platform_driver_register(&hi_gpio_driver);
	if(ret)
	{
		platform_device_unregister(&hi_gpio_device);

		higpio_error("Platform driver register is failed!");
		return ret;
	}

	higpio_trace(1,"end");
	return ret;
}

static void __exit hi_gpio_exit(void)
{
	higpio_trace(1,"begin");
	platform_driver_unregister(&hi_gpio_driver);
	platform_device_unregister(&hi_gpio_device);

	higpio_trace(1,"end");
}

subsys_initcall(hi_gpio_init);
module_exit(hi_gpio_exit);

MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION("Balong GPIO driver for Hisilicon");
MODULE_LICENSE("GPL");
