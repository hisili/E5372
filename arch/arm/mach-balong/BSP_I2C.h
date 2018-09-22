
#define CY22393_CFG_NUM  4

#define IIC_REGBASE_ADDR            IO_ADDRESS(I2C_BASE)

typedef struct tagI2C_CLKCFG_S
{
    BSP_U8  u8regAddr;        /*寄存器地址*/
    BSP_U8  u8regValue;       /*寄存器要配置的值*/
} I2C_CLKCFG_S;


