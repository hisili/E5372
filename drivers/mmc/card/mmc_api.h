#include "BSP.h"


#define RESULT_OK		0
#define RESULT_FAIL		1
#define RESULT_UNSUP_HOST	2
#define RESULT_UNSUP_CARD	3

/*�͹��ļĴ�������*/
typedef struct ST_SOC_REG_ADDR_INFO_S
{
    unsigned int ulStartAddr;            /*��Ҫ���ݵļĴ�����ַ��Ϣ*/
    unsigned int ulLength;               /*���ݳ��ȣ���Ҫ4�ֽڶ���*/
} ST_SOC_REG_ADDR_INFO;

