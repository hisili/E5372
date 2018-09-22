#include "BSP.h"


#define RESULT_OK		0
#define RESULT_FAIL		1
#define RESULT_UNSUP_HOST	2
#define RESULT_UNSUP_CARD	3

/*低功耗寄存器备份*/
typedef struct ST_SOC_REG_ADDR_INFO_S
{
    unsigned int ulStartAddr;            /*需要备份的寄存器地址信息*/
    unsigned int ulLength;               /*内容长度，需要4字节对齐*/
} ST_SOC_REG_ADDR_INFO;

