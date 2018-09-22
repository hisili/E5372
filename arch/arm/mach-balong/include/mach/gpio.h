/* arch/arm/mach-balong/include/mach/gpio.h
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	http://armlinux.simtec.co.uk/
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * BALONGV3R2 - GPIO lib support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#ifndef	__BALONG_GPIO_H
#define	__BALONG_GPIO_H

#include <linux/types.h>
#include "BSP.h"
#define gpio_get_value	__gpio_get_value
#define gpio_set_value	__gpio_set_value
#define gpio_cansleep	__gpio_cansleep
#define gpio_to_irq		__gpio_to_irq
//#define gpio_get_direction gpio_chip_get_direction
/*debug info*/
#define HIGPIO_INTERFACE_DEBUG (1)
#define HIGPIO_TRACE_LEVEL  (5)


/*
   0 - all message
   1 - dump all register read/write
   2 - flow trace
   3 - timeouut err and protocol err
   */

#define BALONG_GPIO_DEBUG_TAG	"hi_gpio"
#define higpio_trace(level, format, args...) do { \
	if((level) >= HIGPIO_TRACE_LEVEL) { \
		printk(format""BALONG_GPIO_DEBUG_TAG":%s:%d: \n", ## args, __FUNCTION__,  __LINE__); \
	} \
}while(0)


#define higpio_error(format, args...) do{ \
	printk(KERN_ERR""format""BALONG_GPIO_DEBUG_TAG" error:%s:%d: \n", ##args, __FUNCTION__, __LINE__); \
}while(0)

/*gpio0 register phy address*/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    ||(defined(BOARD_SFT) && defined(VERSION_V7R1)) /*y00186965*/\
    ||((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) \
	 &&(defined(BOARD_ASIC) || defined (BOARD_ASIC_BIGPACK) || defined (BOARD_SFT)))
    /*V3R2 CS f00164371*/
    #if defined(CHIP_BB_6756CS)
	    #define HI_GPIO0_BASE	(0x90011000)
        #define HI_GPIO_REG_OFFSET	(0x1000)
	#elif defined(CHIP_BB_6920CS)
	    #define HI_GPIO0_BASE	(0x90006000)
        #define HI_GPIO1_BASE	(0x90011000)
		#define HI_GPIO2_BASE	(0x9000e000)
		#define HI_GPIO3_BASE	(0x90013000)
        #define HI_GPIO_REG_OFFSET	(0x1000)
	#else
        #define HI_GPIO0_BASE	(0x90006000)
        #define HI_GPIO1_BASE	(0x90011000)
        #define HI_GPIO_REG_OFFSET	(0x1000)
	#endif
#else
    #define HI_GPIO0_BASE	(0x90011000)
    #define HI_GPIO_REG_OFFSET	(0x1000)
#endif

#define GPIO_DATA_OFFSET                         	(0x00)
#define GPIO_DIRECIONT_OFFSET                (0x04)
#define GPIO_INTEN					(0x30)
#define GPIO_INTMASK_OFFSET                   	(0x34)
#define GPIO_INTTYPE_LEVEL			(0x38)
#define GPIO_INT_PLOARITY                 	(0x3C)
#define GPIO_INTSTATUS_OFFSET               	(0x40)
#define GPIO_INTRAWSTATUS_OFFSET        (0x44)
#define GPIO_INTCLEAR_OFFSET                  	(0x4c)
#define GPIO_EXT_PORTA				(0X50)

#define GPIO_REG_READ(reg,result)  ((result) = *(volatile UINT32 *)(reg))
#define GPIO_REG_WRITE(reg,data)  (*((volatile UINT32 *)(reg)) = (data))

typedef s16 INT16;
typedef u16 UINT16;
typedef s32 INT32;
//typedef u32 UINT32;
typedef s8 CHAR8;
//typedef u8 UINT8;

/* GPIO */
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_ASIC) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1)) /*y00186965*/\
    ||((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) \
	&&(defined(BOARD_ASIC) || defined (BOARD_ASIC_BIGPACK) || defined (BOARD_SFT)))
    /*V3R2 CS f00164371*/
    #if defined(CHIP_BB_6756CS)
        #define GPIO0     (0)
        #define GPIO1     (1)
        #define GPIO2     (2)
        #define GPIO3     (3)
        #define GPIO4     (4)
        #define GPIO5     (5)
        #define GPIO6     (6)
        #define GPIO7     (7)
        #define GPIO8     (8)
        #define GPIO9     (9)
        #define GPIO10   (10)
        #define GPIO11   (11)
        
        #define GPIO_MAXIMUM	(12)
        #define GPIO_MAX_PINS 	(8)
	#else
        #define GPIO0     (0)
        #define GPIO1     (1)
        #define GPIO2     (2)
        #define GPIO3     (3)
        #define GPIO4     (4)
        #define GPIO5     (5)
        #define GPIO_MAXIMUM	(6)
        #define GPIO_MAX_PINS 	(32)
	#endif
#else
    #define GPIO0     (0)
    #define GPIO1     (1)
    #define GPIO2     (2)
    #define GPIO3     (3)
    #define GPIO4     (4)
    #define GPIO5     (5)
    #define GPIO6     (6)
    #define GPIO7     (7)
    #define GPIO8     (8)
    #define GPIO9     (9)
    #define GPIO10   (10)
    #define GPIO11   (11)
    
    #define GPIO_MAXIMUM	(12)
    #define GPIO_MAX_PINS 	(8)
#endif /*y00186965*/
#define ARCH_NR_GPIOS	(GPIO_MAXIMUM*GPIO_MAX_PINS)
#define GPIO_SEG_IRQ_BASE1	(112)
#define GPIO_SEG_IRQ_BASE2	(149)
#define GPIO_SEPARATE_GPIO	(5)


#define GPIO_OK        0
#define GPIO_ERROR  1

/*get gpio num*/
#define BALONG_GPIO_0(_nr)	(_nr)
#define BALONG_GPIO_1(_nr)	(BALONG_GPIO_0(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_2(_nr)	(BALONG_GPIO_1(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_3(_nr)	(BALONG_GPIO_2(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_4(_nr)	(BALONG_GPIO_3(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_5(_nr)	(BALONG_GPIO_4(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_6(_nr)	(BALONG_GPIO_5(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_7(_nr)	(BALONG_GPIO_6(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_8(_nr)	(BALONG_GPIO_7(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_9(_nr)	(BALONG_GPIO_8(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_10(_nr)	(BALONG_GPIO_9(GPIO_MAX_PINS - 1) + (_nr) + 1)
#define BALONG_GPIO_11(_nr)	(BALONG_GPIO_10(GPIO_MAX_PINS - 1) + (_nr) + 1)

/* GPIO TLMM: Function -- GPIO specific */

/* GPIO TLMM: Direction */
enum {
	GPIO_INPUT = 0,
	GPIO_OUTPUT = 1,
};

/* GPIO TLMM: Pullup/Pulldown */
enum {
	GPIO_NO_PULL,
	GPIO_PULL_DOWN,
	GPIO_KEEPER,
	GPIO_PULL_UP,
};

/*GPIO function*/
enum{
	GPIO_NORMAL= 0,
	GPIO_INTERRUPT = 1,
};

/*interrupt mask*/
enum {
	GPIO_INT_ENABLE = 0,
	GPIO_INT_DISABLE = 1,
};

/*interrupt level trigger*/
enum{
	GPIO_INT_TYPE_LEVEVL = 0,
	GPIO_INT_TYPE_EDGE = 1,
};

/*interrupt polarity*/
enum{
	GPIO_INT_POLARITY_FAL_LOW = 0,
	GPIO_INT_POLARITY_RIS_HIGH = 1,
};

/*gpio define from pin 0 ~ 95, group 0~11, pin 0 ~7*/
//#define BALONG_GPIO(group, pin)	BALONG_GPIO_##group(pin)

typedef struct 
{
    void (*gpioisr)(int);
    int para;
} GPIOISR;

/*++by pandong cs*/
int gpio_chip_value_set(unsigned int chip_num, unsigned int pin, unsigned int value);
int gpio_chip_value_get(unsigned int chip_num, unsigned int pin,int* value);
int gpio_chip_set_direction(unsigned int chip_num, unsigned int pin, unsigned int direction);
/*--by pandong cs*/

/*************************************************
  Function:       gpio_chip_get_direction
  Description:    Get GPIO Direction value(GPIO_OUTPUT,GPIO_INPUT)
  Calls:   
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin, 
  unsigned* direction: GPIO_INPUT, GPIO_OUTPUT
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_chip_get_direction(unsigned int chip_num, unsigned int pin, unsigned int * direction);
	
/*************************************************
  Function:       gpio_chip_set_function
  Description:    Set GPIO function
  Calls:   
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin, 
  	unsigned direction:	GPIO_NORMAL,
					GPIO_INTERRUPT,
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_chip_set_function(unsigned int chip_num, unsigned int pin, unsigned int function);


/*************************************************
  Function:       gpio_int_chip_mask_set
  Description:    Mask GPIO interrupt
  Calls:            Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others: mask bit refer to GPIO_INT_ENABLE, GPIO_INT_DISABLE
************************************************/
int  gpio_int_chip_mask_set(unsigned int chip_num, unsigned int pin);

/*************************************************
  Function:       gpio_int_chip_unmask_set
  Description:    unMask GPIO interrupt
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:mask bit refer to GPIO_INT_ENABLE, GPIO_INT_DISABLE
************************************************/
int gpio_int_chip_unmask_set(unsigned int chip_num, unsigned int pin);

/*************************************************
  Function:       gpio_int_chip_state_get
  Description:    get GPIO interrupt  station
  Calls:          
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin, unsigned * state
  Output:          unsigned * state
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_int_chip_state_get(unsigned int chip_num, unsigned int pin, unsigned int * state);

/*************************************************
  Function:       gpio_raw_int_chip_state_get
  Description:    get GPIO raw interrupt  station
  Calls:          
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin, 
  Output:         unsigned * rawstate
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_raw_int_chip_state_get(unsigned int chip_num, unsigned int pin, unsigned int * rawstate);

/*************************************************
  Function:       gpio_int_chip_state_clear
  Description:    Clear GPIO interrupt
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned chip_num, unsigned pin
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:clear refer to 
************************************************/
int gpio_int_chip_state_clear(unsigned int chip_num, unsigned int pin);

/*************************************************
  Function:       gpio_int_chip_trigger_set
  Description:    set GPIO Interrupt  Triger style
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:         unsigned chip_num, unsigned pin, unsigned int trigger_type refer to
  linux/irq.h as:
  IRQ_TYPE_EDGE_RISING,
  IRQ_TYPE_EDGE_FALLING,
  IRQ_TYPE_EDGE_BOTH,
  IRQ_TYPE_LEVEL_HIGH,
  IRQ_TYPE_LEVEL_LOW
  Output:         NONE
  Return:         OSAL_OK : successfully
                  OSAL_ERROR: fail
  Others:
************************************************/
int gpio_int_chip_trigger_set(unsigned int chip_num, unsigned int pin, unsigned int trigger_type) ;

/*************************************************
  Function:       gpio_get_direction
  Description:    Get GPIO Direction value(GPIO_OUTPUT,GPIO_INPUT)
  Calls:   
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio, 
  unsigned* direction: GPIO_INPUT, GPIO_OUTPUT
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_get_direction(unsigned int gpio, unsigned* direction);

/*************************************************
  Function:       gpio_set_function
  Description:    Set GPIO function
  Calls:   
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio, 
  	unsigned direction:	GPIO_NORMAL,
					GPIO_INTERRUPT,
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_set_function(unsigned int gpio, unsigned function);


/*************************************************
  Function:       gpio_int_mask_set
  Description:    Mask GPIO interrupt
  Calls:            Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others: mask bit refer to GPIO_INT_ENABLE, GPIO_INT_DISABLE
************************************************/
int  gpio_int_mask_set(unsigned int gpio);

/*************************************************
  Function:       gpio_int_chip_unmask_set
  Description:    unMask GPIO interrupt
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:mask bit refer to GPIO_INT_ENABLE, GPIO_INT_DISABLE
************************************************/
int gpio_int_unmask_set(unsigned int gpio);

/*************************************************
  Function:       gpio_int_state_get
  Description:    get GPIO interrupt  station
  Calls:          
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio, unsigned * state
  Output:          unsigned * state
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_int_state_get(unsigned int gpio, unsigned * state);

/*************************************************
  Function:       gpio_raw_int_chip_state_get
  Description:    get GPIO raw interrupt  station
  Calls:          
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio 
  Output:         unsigned * rawstate
  Return:         0 : successfully
                  not 0: fail
  Others:
************************************************/
int gpio_raw_int_state_get(unsigned int gpio, unsigned * rawstate);

/*************************************************
  Function:       gpio_raw_int_state_get
  Description:    Clear GPIO interrupt
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:          unsigned int gpio
  Output:         NONE
  Return:         0 : successfully
                  not 0: fail
  Others:clear refer to 
************************************************/
int gpio_int_state_clear(unsigned int gpio);

/*************************************************
  Function:       gpio_int_trigger_set
  Description:    set GPIO Interrupt  Triger style
  Calls:
  Called By:
  Table Accessed: NONE
  Table Updated:  NONE
  Input:         unsigned int gpio, unsigned int trigger_type refer to
  linux/irq.h as:
  IRQ_TYPE_EDGE_RISING,
  IRQ_TYPE_EDGE_FALLING,
  IRQ_TYPE_EDGE_BOTH,
  IRQ_TYPE_LEVEL_HIGH,
  IRQ_TYPE_LEVEL_LOW
  Output:         NONE
  Return:         OSAL_OK : successfully
                  OSAL_ERROR: fail
  Others:
************************************************/
int gpio_int_trigger_set(unsigned int gpio, unsigned int trigger_type) ;

#include <asm-generic/gpio.h>
#endif
