
#define CY22393_CFG_NUM  4

#define IIC_REGBASE_ADDR            IO_ADDRESS(I2C_BASE)

typedef struct tagI2C_CLKCFG_S
{
    BSP_U8  u8regAddr;        /*�Ĵ�����ַ*/
    BSP_U8  u8regValue;       /*�Ĵ���Ҫ���õ�ֵ*/
} I2C_CLKCFG_S;


