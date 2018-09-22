/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : BSP_CHGC.h
  �� �� ��   : ����
  ��������   : 2013��1��9��
  ����޸�   :
  ��������   : 
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��1��9��
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __BSP_CHGC_H__
#define __BSP_CHGC_H__


#ifdef __cplusplus
extern "C"
{
#endif



/**************************************************************************
  ͷ�ļ�����                            
**************************************************************************/


/**************************************************************************
  �궨�� 
**************************************************************************/

typedef struct
{
    int charge_state;  
    int battery_percent;  
    int reserve;    
}battery_state_std;


#define CHG_ERROR(fmt, ...) \
do {\
        printk(KERN_ERR "File:%s FUN:%s LINE:%d "fmt"", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }while(0)

#define CHG_INFO(fmt, ...) \
do {\
        printk(KERN_WARNING "File:%s FUN:%s LINE:%d "fmt"", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }while(0)

#define CHG_DEBUG(fmt, ...) \
do {\
        printk(KERN_INFO "File:%s FUN:%s LINE:%d "fmt"", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);\
    }while(0)


#ifdef __cplusplus
}
#endif

#endif

