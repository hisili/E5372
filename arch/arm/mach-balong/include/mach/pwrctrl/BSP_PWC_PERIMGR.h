/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcPeriMgr.h
* Description:
*                
*
* Author:        ¡ı”¿∏ª
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		¡ı”¿∏ª
* Date:			2011-09-20
* Description:	Initial version
*
*******************************************************************************/

#ifndef PWRCTRL_ACPU_PERI_MGR
#define PWRCTRL_ACPU_PERI_MGR

#if(FEATURE_HSIC_SLAVE == FEATURE_ON)

/*****************************************************************************/
/*include headfile */
/*****************************************************************************/

/*****************************************************************************/
/*macro define*/
/*****************************************************************************/
#define OK 0
#define ERROR -1

#define GPIO_LOW                                    0    /*GPIO?a|??®¨???°ß?a|??®¨????*/
#define GPIO_HIGH                                   1    /*GPIO?a??|??®¨????*/
#define GPIO_INVALID                              0xFF
#if defined(CHIP_BB_6920ES)
/*HSIC |??®¨???°ß?a1|o????°ß???°Ï?°Ï1?GPIOo??°ß?o????°Ï???°ß????*/
#define INT_GPIO_0                      112
#define AP_ACTIVEMODEM_INT              INT_GPIO_0
#define SLAVE_WAKE_R_INT                INT_GPIO_0
#define SLAVE_WAKE_F_INT                INT_GPIO_0

#define HOST_WAKEUP_GPIO 2
#define HOST_WAKEUP_PIN 1
#define HOST_WAKEUP BALONG_GPIO_2(HOST_WAKEUP_PIN)

#define SUSPEND_REQUEST_GPIO 2
#define SUSPEND_REQUEST_PIN 4
#define SUSPEND_REQUEST BALONG_GPIO_2(SUSPEND_REQUEST_PIN)

#define SLAVE_WAKEUP_RISING_GPIO 0
#define SLAVE_WAKEUP_RISING_PIN  7
#define SLAVE_WAKEUP_RISING BALONG_GPIO_0(SLAVE_WAKEUP_RISING_PIN)

#define SLAVE_WAKEUP_FALLING_GPIO 0
#define SLAVE_WAKEUP_FALLING_PIN 10
#define SLAVE_WAKEUP_FALLING BALONG_GPIO_0(SLAVE_WAKEUP_FALLING_PIN)

#define HOST_ACTIVE_GPIO 0
#define HOST_ACTIVE_PIN 9
#define HOST_ACTIVE BALONG_GPIO_0(HOST_ACTIVE_PIN)
#elif defined (CHIP_BB_6920CS)
#define INT_GPIO_0                      (112)
#define INT_GPIO_2                      (114)
#define AP_ACTIVEMODEM_INT              INT_GPIO_2
#define SLAVE_WAKE_R_INT                INT_GPIO_2
#define SLAVE_WAKE_F_INT                INT_GPIO_2

#define HOST_WAKEUP_GPIO 2
#define HOST_WAKEUP_PIN 6
#define HOST_WAKEUP BALONG_GPIO_2(HOST_WAKEUP_PIN)

/*leacy config*/
#define SUSPEND_REQUEST_GPIO 0
#define SUSPEND_REQUEST_PIN 12
#define SUSPEND_REQUEST BALONG_GPIO_0(SUSPEND_REQUEST_PIN)

#define SLAVE_WAKEUP_RISING_GPIO 2
#define SLAVE_WAKEUP_RISING_PIN  14
#define SLAVE_WAKEUP_RISING BALONG_GPIO_2(SLAVE_WAKEUP_RISING_PIN)

#define SLAVE_WAKEUP_FALLING_GPIO 2
#define SLAVE_WAKEUP_FALLING_PIN 16
#define SLAVE_WAKEUP_FALLING BALONG_GPIO_2(SLAVE_WAKEUP_FALLING_PIN)

#define HOST_ACTIVE_GPIO 2
#define HOST_ACTIVE_PIN 13
#define HOST_ACTIVE BALONG_GPIO_2(HOST_ACTIVE_PIN)
#else
#error "hsic feature not to be supported on this kind of chip"
#endif

typedef  enum
{
    HSIC_INIT_STATE = 0,
    HSIC_L0_STATE,/*HSIC|°ß??°Ïa1|o?|°ß??L0 L2 L3 ?°Ï?°Ïy????°Ï?°Ï?°Ï??*/
    HSIC_L2_STATE,
    HSIC_L3_STATE,
    HSIC_L02_STATE=4,/*HSIC|°ß??°Ïa1|o?|°ß???Y?°Ï??*/
    HSIC_L23_STATE,
    HSIC_L30_STATE,
    HSIC_L20_STATE,
    HSIC_MAX_STATE,
}HSIC_STATE_TYPE;

typedef struct
{
    s8 state_switch_enable;
    HSIC_STATE_TYPE old_state;
    HSIC_STATE_TYPE current_state;
} HSIC_PWC_STATE;



s8 hsic_pwc_state_set (HSIC_STATE_TYPE new_state);
HSIC_STATE_TYPE hsic_pwc_state_get (void);

void host_active_pwc_isr(void);
s8 host_active_pwc_int_handler(void);
s8 hsic_suspend_pwc_int_handler(void);
s8 hsic_resume_pwc_int_handler(void);


#endif
#endif
