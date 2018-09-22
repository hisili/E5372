/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : BSP_CHGC.h
  版 本 号   : 初稿
  生成日期   : 2013年1月9日
  最近修改   :
  功能描述   : 
  函数列表   :
  修改历史   :
  1.日    期   : 2013年1月9日
    修改内容   : 创建文件

******************************************************************************/
#ifndef __BSP_CHGC_H__
#define __BSP_CHGC_H__


#ifdef __cplusplus
extern "C"
{
#endif



/**************************************************************************
  头文件包含                            
**************************************************************************/


/**************************************************************************
  宏定义 
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

