/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  MemoryLayout.h
*
*   ��    �� :  liumengcun
*
*   ��    �� :  ���������ڴ����
*
*   �޸ļ�¼ :  2011��12��20��  v1.00  liumengcun  ����
*
*************************************************************************/
#ifndef __MEMMORY_LAYOUT_H__
#define __MEMMORY_LAYOUT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"


/* ���ַת������ */
#define IO_ADDRESS(x)         ((((x) & 0xF0000000) >> 4) | ((x) & 0x00FFFFFF) | 0xF0000000)

#if (defined(BOARD_ASIC) && defined(VERSION_V3R2))
#if (defined(CHIP_BB_6756CS))
/*CS STICK*/
#if (FEATURE_STICK == FEATURE_ON || FEATURE_CS_PHONE_GU == FEATURE_ON)
/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x04000000)
#define GLOBAL_MEM_BASE_ADDR_VIRT              (0xF3000000)           /* ͨ��IO_ADDRESS������ */

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | A���ڴ�             | 22M     |
               +---------------------+---------+ --> 0x31600000
               | C���ڴ�             | 23M     |               
               +---------------------+---------+ --> 0x32D00000        ���ַͳһ��0xF3000000��ʼ
               | GUPSԤ���ڴ�        | 17M+96KB|  
               +---------------------+---------+ --> 0x33E18000
               | �����ڴ�            | 512K    |
               +---------------------+---------+ --> 0x33E98000
               | �쳣��¼            | 640K    |
               +---------------------+---------+ --> 0x33F38000
               | HIFI�ڴ�            | 800KB   |
               +---------------------+---------+ --> 0x34000000
*/
/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_BASE_ADDR)       /*0x30000000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x01600000)


/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE)    /*0x31A00000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x01700000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)      /*0x33100000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (GLOBAL_MEM_BASE_ADDR_VIRT)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x01118000)


/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_GU_RESERVED_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE)   /*0x33E18000*/
#define GLOBAL_MEM_CORE_SHARE_ADDR_VIRT        (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT + GLOBAL_MEM_GU_RESERVED_SIZE)
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00080000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)     /*0x33E98000*/
#define GLOBAL_MEM_EXCP_BASE_ADDR_VIRT         (GLOBAL_MEM_CORE_SHARE_ADDR_VIRT + GLOBAL_MEM_CORE_SHARE_SIZE)
#define GLOBAL_MEM_EXCP_SIZE                   (0x000A0000)

/* HIFI�ڴ� */
#define GLOBAL_MEM_HIFI_BASE_ADDR              (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE)            /*0x33F38000*/
#define GLOBAL_MEM_HIFI_SIZE                   (0x000C8000)


/*vxworks�ߵ͵�ַ����*/
#define VXWORKS_HIGH_ADDR                      (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE - 6*4096)  /*Ԥ��24K��DMR*/           
#define VXWORKS_LOW_ADDR                       (GLOBAL_MEM_MCORE_BASE_ADDR + 0x4000)


/*Android boot.img ��ʼ��������ַ����*/
#define LINUX_ADDR_TAGS                        (GLOBAL_MEM_ACORE_BASE_ADDR + 0x100)
#define LINUX_PHYS_OFFSET                      (GLOBAL_MEM_ACORE_BASE_ADDR)


#elif (FEATURE_HILINK == FEATURE_ON)  /*cs_hilink*/
/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x04000000)
#define GLOBAL_MEM_BASE_ADDR_VIRT              (0xF3000000)           /* ͨ��IO_ADDRESS������ */

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | A���ڴ�             | 26M     |
               +---------------------+---------+ --> 0x31A00000
               | C���ڴ�             | 23M     |               
               +---------------------+---------+ --> 0x33100000        ���ַͳһ��0xF3000000��ʼ
               | GUPSԤ���ڴ�        | 13M+96KB|  
               +---------------------+---------+ --> 0x33E18000
               | �����ڴ�            | 512K    |
               +---------------------+---------+ --> 0x33E98000
               | �쳣��¼            | 640K    |
               +---------------------+---------+ --> 0x33F38000
               | HIFI�ڴ�            | 800KB   |
               +---------------------+---------+ --> 0x34000000
*/
/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_BASE_ADDR)       /*0x30000000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x01A00000)


/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE)    /*0x31900000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x01700000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)      /*0x33100000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (GLOBAL_MEM_BASE_ADDR_VIRT)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x00D18000)


/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_GU_RESERVED_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE)   /*0x33E18000*/
#define GLOBAL_MEM_CORE_SHARE_ADDR_VIRT        (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT + GLOBAL_MEM_GU_RESERVED_SIZE)
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00080000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)     /*0x33E98000*/
#define GLOBAL_MEM_EXCP_BASE_ADDR_VIRT         (GLOBAL_MEM_CORE_SHARE_ADDR_VIRT + GLOBAL_MEM_CORE_SHARE_SIZE)
#define GLOBAL_MEM_EXCP_SIZE                   (0x000A0000)

/* HIFI�ڴ� */
#define GLOBAL_MEM_HIFI_BASE_ADDR              (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE)            /*0x33F38000*/
#define GLOBAL_MEM_HIFI_SIZE                   (0x000C8000)


/*vxworks�ߵ͵�ַ����*/
#define VXWORKS_HIGH_ADDR                      (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE - 6*4096)  /*Ԥ��24K��DMR*/           
#define VXWORKS_LOW_ADDR                       (GLOBAL_MEM_MCORE_BASE_ADDR + 0x4000)


/*Android boot.img ��ʼ��������ַ����*/
#define LINUX_ADDR_TAGS                        (GLOBAL_MEM_ACORE_BASE_ADDR + 0x100)
#define LINUX_PHYS_OFFSET                      (GLOBAL_MEM_ACORE_BASE_ADDR)


#elif (FEATURE_E5 == FEATURE_ON)  /*cs_e5*/
/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x04000000)
#define GLOBAL_MEM_BASE_ADDR_VIRT              (0xF3000000)           /* ͨ��IO_ADDRESS������ */

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | A���ڴ�             | 27M     |
               +---------------------+---------+ --> 0x31B00000
               | C���ڴ�             | 23M     |               
               +---------------------+---------+ --> 0x33200000        ���ַͳһ��0xF3000000��ʼ
               | GUPSԤ���ڴ�        |12M+864KB|  
               +---------------------+---------+ --> 0x33ED8000
               | �쳣��¼            | 640K    |
               +---------------------+---------+ --> 0x33F78000
               | �����ڴ�            | 512K    | 
               +---------------------+---------+ --> 0x33FF8000
               | HIFI�ڴ�            | 32KB    |
               +---------------------+---------+ --> 0x34000000
*/
/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_BASE_ADDR)       /*0x30000000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x01B00000)


/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE)    /*0x31B00000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x01700000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)      /*0x33200000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (GLOBAL_MEM_BASE_ADDR_VIRT)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x00CD8000)


/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_GU_RESERVED_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE)     /*0x33ED8000*/
#define GLOBAL_MEM_EXCP_BASE_ADDR_VIRT         (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT + GLOBAL_MEM_GU_RESERVED_SIZE)
#define GLOBAL_MEM_EXCP_SIZE                   (0x000A0000)

/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE)   /*0x33F78000*/
#define GLOBAL_MEM_CORE_SHARE_ADDR_VIRT        (GLOBAL_MEM_EXCP_BASE_ADDR_VIRT + GLOBAL_MEM_EXCP_SIZE)
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00080000)

/* HIFI�ڴ� */
#define GLOBAL_MEM_HIFI_BASE_ADDR              (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)     /*0x33FF8000*/
#define GLOBAL_MEM_HIFI_SIZE                   (0x00008000)


/*vxworks�ߵ͵�ַ����*/
#define VXWORKS_HIGH_ADDR                      (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE - 6*4096)  /*Ԥ��24K��DMR*/           
#define VXWORKS_LOW_ADDR                       (GLOBAL_MEM_MCORE_BASE_ADDR + 0x4000)


/*Android boot.img ��ʼ��������ַ����*/
#define LINUX_ADDR_TAGS                        (GLOBAL_MEM_ACORE_BASE_ADDR + 0x100)
#define LINUX_PHYS_OFFSET                      (GLOBAL_MEM_ACORE_BASE_ADDR)


#else   /*cs_udp_stick*/

/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x04000000)
#define GLOBAL_MEM_BASE_ADDR_VIRT              (0xF3000000)           /* ͨ��IO_ADDRESS������ */

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | A���ڴ�             | 26M     |
               +---------------------+---------+ --> 0x31A00000
               | C���ڴ�             | 23M     |               
               +---------------------+---------+ --> 0x33100000        ���ַͳһ��0xF3000000��ʼ
               | GUPSԤ���ڴ�        | 13M+96KB|  
               +---------------------+---------+ --> 0x33E18000
               | �����ڴ�            | 512K    |
               +---------------------+---------+ --> 0x33E98000
               | �쳣��¼            | 640K    |
               +---------------------+---------+ --> 0x33F38000
               | HIFI�ڴ�            | 800KB   |
               +---------------------+---------+ --> 0x34000000
*/
/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_BASE_ADDR)       /*0x30000000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x01A00000)


/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE)    /*0x31A00000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x01700000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)      /*0x33100000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (GLOBAL_MEM_BASE_ADDR_VIRT)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x00D18000)


/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_GU_RESERVED_ADDR + GLOBAL_MEM_GU_RESERVED_SIZE)   /*0x33E18000*/
#define GLOBAL_MEM_CORE_SHARE_ADDR_VIRT        (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT + GLOBAL_MEM_GU_RESERVED_SIZE)
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00080000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)     /*0x33E98000*/
#define GLOBAL_MEM_EXCP_BASE_ADDR_VIRT         (GLOBAL_MEM_CORE_SHARE_ADDR_VIRT + GLOBAL_MEM_CORE_SHARE_SIZE)
#define GLOBAL_MEM_EXCP_SIZE                   (0x000A0000)

/* HIFI�ڴ� */
#define GLOBAL_MEM_HIFI_BASE_ADDR              (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE)            /*0x33F38000*/
#define GLOBAL_MEM_HIFI_SIZE                   (0x000C8000)


/*vxworks�ߵ͵�ַ����*/
#define VXWORKS_HIGH_ADDR                      (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE - 6*4096)  /*Ԥ��24K��DMR*/           
#define VXWORKS_LOW_ADDR                       (GLOBAL_MEM_MCORE_BASE_ADDR + 0x4000)


/*Android boot.img ��ʼ��������ַ����*/
#define LINUX_ADDR_TAGS                        (GLOBAL_MEM_ACORE_BASE_ADDR + 0x100)
#define LINUX_PHYS_OFFSET                      (GLOBAL_MEM_ACORE_BASE_ADDR)


#endif

/*  DDR�ڴ�Խ���� */
#if ((GLOBAL_MEM_HIFI_BASE_ADDR + GLOBAL_MEM_HIFI_SIZE) != (GLOBAL_MEM_BASE_ADDR + GLOBAL_MEM_SIZE))
#error "DDR overflow"
#endif

#else

#if (FEATURE_MEMORY_SIZE == MEMORY_SIZE_64M)
/*ES_STICK*/

/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x04000000)

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | ACP�ڴ�             | 0       | 
               +---------------------+---------+ --> 0x30000000
               | C���ڴ�             | 24M     |               
               +---------------------+---------+ --> 0x31800000
               | A���ڴ�             | 25M     |
               +---------------------+---------+ --> 0x33100000
               | Reserved mem        | 448K    |
               +---------------------+---------+ --> 0x33170000
	           | �����ڴ�            | 512K    |
               +---------------------+---------+ --> 0x331F0000
               | �쳣��¼            | 640K    |
               +---------------------+---------+ --> 0x33290000
               |                     |         |
               | GUPSԤ���ڴ�        | 13M+448K|              
               +---------------------+---------+ --> 0x34000000
*/
/* MCORE ACPʹ�� */
#define GLOBAL_MEM_MCORE_ACP_ADDR              (GLOBAL_MEM_BASE_ADDR)
#define GLOBAL_MEM_MCORE_ACP_SIZE              (0x00000000)

/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_MCORE_ACP_ADDR + GLOBAL_MEM_MCORE_ACP_SIZE)    /*0x30000000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x01800000)

/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)       /*0x31c00000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x01B00000)

/* �����ڴ� */
#define GLOBAL_MEM_RESERVED_ADDR               (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE)       /*0x33100000*/
#define GLOBAL_MEM_RESERVED_SIZE               (0x00070000)

/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_RESERVED_ADDR + GLOBAL_MEM_RESERVED_SIZE)     /*0x33170000*/
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00080000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)  /*0x331F0000*/
#define GLOBAL_MEM_EXCP_SIZE                   (0x000A0000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE)         /*0x33290000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (0xF3400000)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x00B70000)

#elif (FEATURE_E5 == FEATURE_ON)
/* ES_E5 */

/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x08000000)

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | ACP�ڴ�             | 2M      | 
               +---------------------+---------+ --> 0x30200000
               | C���ڴ�             | 61M-24K | 
               +---------------------+---------+ --> 0x33EFA000
               | Debug��Ϣ����       | 4K      | 
               +---------------------+---------+ --> 0x33EFB000
               | �쳣��¼            | 1M      |
               +---------------------+---------+ --> 0x33FFB000
               | UNUSED�ڴ�0         | 20K     |
               +---------------------+---------+ --> 0x34000000
	           | �����ڴ�            | 4M      |
               +---------------------+---------+ --> 0x34400000
               | A���ڴ�             | 32M     |
               +---------------------+---------+ --> 0x36400000
               | GUPSԤ���ڴ�        | 28M     |              
               +---------------------+---------+ --> 0x38000000
*/
/* MCORE ACP */
#define GLOBAL_MEM_MCORE_ACP_ADDR              (GLOBAL_MEM_BASE_ADDR)
#define GLOBAL_MEM_MCORE_ACP_SIZE              (0x00200000)

/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_MCORE_ACP_ADDR + GLOBAL_MEM_MCORE_ACP_SIZE)    /*0x30200000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x03CFA000)

/* Debug�ڴ� */
#define GLOBAL_MEM_DEBUG_BASE_ADDR             (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)       /*0x33EFA000*/
#define GLOBAL_MEM_DEBUG_SIZE                  (0x00001000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_DEBUG_BASE_ADDR + GLOBAL_MEM_DEBUG_SIZE)       /*0x33EFB000*/
#define GLOBAL_MEM_EXCP_SIZE                   (0x00100000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED0_SIZE                (0x00005000)

/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE + GLOBAL_MEM_UNUSED0_SIZE)  /*0x34000000*/
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00400000)

/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)  /*0x34400000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x02000000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE)  /*0x36400000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (0xF3400000)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x01C00000)

#else
/* UDP_STICK & UDP_E5 */

/* ����DDR�ڴ� */
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x10000000)

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | ACP�ڴ�             | 2M      | 
               +---------------------+---------+ --> 0x30200000
               | C���ڴ�             | 61M-24K | 
               +---------------------+---------+ --> 0x33EFA000
               | Debug��Ϣ����       | 4K      | 
               +---------------------+---------+ --> 0x33EFB000
               | �쳣��¼            | 1M      |
               +---------------------+---------+ --> 0x33FFB000
               | UNUSED�ڴ�0         | 20K     |
               +---------------------+---------+ --> 0x34000000
	           | �����ڴ�            | 4M      |
               +---------------------+---------+ --> 0x34400000
               | A���ڴ�             | 32M     |
               +---------------------+---------+ --> 0x36400000
               | UNUSED�ڴ�          | 12M     |
               +---------------------+---------+ --> 0x37000000
               | GUPSԤ���ڴ�        | 28M     |              
               +---------------------+---------+ --> 0x38C00000
               | UNUSED�ڴ�          | 116M    |
               +---------------------+---------+ --> 0x40000000               
*/
/* MCORE ACP */
#define GLOBAL_MEM_MCORE_ACP_ADDR              (GLOBAL_MEM_BASE_ADDR)
#define GLOBAL_MEM_MCORE_ACP_SIZE              (0x00200000)

/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_MCORE_ACP_ADDR + GLOBAL_MEM_MCORE_ACP_SIZE)    /*0x30200000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x03CFA000)

/* Debug�ڴ� */
#define GLOBAL_MEM_DEBUG_BASE_ADDR             (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)       /*0x33EFA000*/
#define GLOBAL_MEM_DEBUG_SIZE                  (0x00001000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_DEBUG_BASE_ADDR + GLOBAL_MEM_DEBUG_SIZE)       /*0x33EFB000*/
#define GLOBAL_MEM_EXCP_SIZE                   (0x00100000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED0_SIZE                (0x00005000)

/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE + GLOBAL_MEM_UNUSED0_SIZE)  /*0x34000000*/
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00400000)

/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)  /*0x34400000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x02000000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED_SIZE                 (0x00C00000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE + GLOBAL_MEM_UNUSED_SIZE)  /*0x37000000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (0xF3400000)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x01C00000)

#endif /*#if (FEATURE_MEMORY_SIZE == MEMORY_SIZE_64M)*/
#endif /*#if defined(CHIP_BB_6756CS)*/


/* ����AXI�ڴ� */
#if defined(CHIP_BB_6756CS)
#define GLOBAL_AXI_MEM_BASE_ADDR_PHY           (0x2FFE0000)
#define GLOBAL_AXI_MEM_SIZE                    (0x00020000)
#else
#define GLOBAL_AXI_MEM_BASE_ADDR_PHY           (0x2FFC0000)
#define GLOBAL_AXI_MEM_SIZE                    (0x00040000)
#endif
#define GLOBAL_AXI_MEM_BASE_ADDR_VIRT          (IO_ADDRESS(GLOBAL_AXI_MEM_BASE_ADDR_PHY))


#ifdef BSP_CORE_MODEM
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#else
#ifdef __FASTBOOT__
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#else
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_VIRT)
#endif

#define AXI_MEM_BASE_ADDR                      (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#define AXI_MEM_ADDR                           (GLOBAL_AXI_MEM_BASE_ADDR)
#define AXI_MEM_SIZE                           (GLOBAL_AXI_MEM_SIZE)
#endif

#if defined(CHIP_BB_6756CS)
/*                      AXI Memory ����ͼ

               +---------------------+---------+ --> 0x2FFE0000
               | A�˵͹���           | 16KB    |               
               +---------------------+---------+ --> 0x2FFE4000
               | FLASH��Դ��         | 32B     |
               +---------------------+---------+ --> 0x2FFE4020
               | IFC��ʶ             | 32B     |
               +---------------------+---------+ --> 0x2FFE4040
	           | ICC��ʶ             | 1K      |
	           +---------------------+---------+ --> 0x2FFE4440
	           | �ڴ����            | 1K      |
               +---------------------+---------+ --> 0x2FFE4840
               | ��̬�ڴ���          |73K-256B|
               +---------------------+---------+ --> 0x2FFF6B40
               | IPF������           | 30K     |
               +---------------------+---------+ --> 0x2FFFE340
               | �±���              | 128B    |
               +---------------------+---------+ --> 0x2FFFE3C0
               | ���ػ�              | 32B     |
               +---------------------+---------+ --> 0x2FFFE3E0
               | DICC��ʶ            | 32B     |
               +---------------------+---------+ --> 0x2FFFE400
               | HIFI������          | 6K      |              
               +---------------------+---------+ --> 0x2FFFFC00
               | ������              | 1K-256B |
               +---------------------+---------+ --> 0x2FFFFF00
               | ������ʶ            | 256B    |              
               +---------------------+---------+ --> 0x30000000
*/ 
/*FastBoot*/
#define MEMORY_AXI_FASTBOOT_START_ADDR        (GLOBAL_AXI_MEM_BASE_ADDR)
#define MEMORY_AXI_FASTBOOT_START_SIZE        (0x08)

/* ���ػ� */
#define MEMORY_AXI_ONOFF_ADDR               (MEMORY_AXI_FASTBOOT_START_ADDR + MEMORY_AXI_FASTBOOT_START_SIZE + 268)
#define MEMORY_AXI_ONOFF_SIZE               (32)

#define MEMORY_AXI_DLOAD_ADDR               (MEMORY_AXI_ONOFF_ADDR + MEMORY_AXI_ONOFF_SIZE)
#define MEMORY_AXI_DLOAD_SIZE               (256-48)

/* ������ʶ */
#define MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR   (MEMORY_AXI_DLOAD_ADDR + MEMORY_AXI_DLOAD_SIZE)
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (48)

/* DICC��ʶ */
#define MEMORY_AXI_DICC_ADDR                (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR + MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE)
#define MEMORY_AXI_DICC_SIZE                (32)

/* �±��� */
#define MEMORY_AXI_TEMP_PROTECT_ADDR        (MEMORY_AXI_DICC_ADDR + MEMORY_AXI_DICC_SIZE)
#define MEMORY_AXI_TEMP_PROTECT_SIZE        (128)

/* USB�β� */
#define MEMORY_AXI_USB_INOUT_ADDR           (MEMORY_AXI_TEMP_PROTECT_ADDR + MEMORY_AXI_TEMP_PROTECT_SIZE)
#define MEMORY_AXI_USB_INOUT_SIZE           (4)

/*CSHELL*/
#define MEMORY_AXI_USB_CSHELL_ADDR         (MEMORY_AXI_USB_INOUT_ADDR + MEMORY_AXI_USB_INOUT_SIZE)
#define MEMORY_AXI_USB_CSHELL_SIZE         (4)

/* ��������A/C shell���л� */
#define MEMORY_AXI_UART_INOUT_ADDR          (MEMORY_AXI_USB_CSHELL_ADDR + MEMORY_AXI_USB_CSHELL_SIZE)
#define MEMORY_AXI_UART_INOUT_SIZE          (4)

/* ���־������С��ȥ��8����Ϊ���ĵط������˱�� */
#define MEMORY_AXI_SOFT_FLAG_ADDR           (MEMORY_AXI_UART_INOUT_ADDR + MEMORY_AXI_UART_INOUT_SIZE)
#define MEMORY_AXI_SOFT_FLAG_SIZE           (1292 - (MEMORY_AXI_SOFT_FLAG_ADDR - GLOBAL_AXI_MEM_BASE_ADDR) - 8)

/* A�˵͹��� */
#define MEMORY_AXI_ACPU_DEEPSLEEP_ADDR      (GLOBAL_AXI_MEM_BASE_ADDR + 1292)
#define MEMORY_AXI_ACPU_DEEPSLEEP_SIZE      (0x3000)

/* ICC��ʶ */
#define MEMORY_AXI_ICC_ADDR                 (MEMORY_AXI_ACPU_DEEPSLEEP_ADDR + MEMORY_AXI_ACPU_DEEPSLEEP_SIZE)
#define MEMORY_AXI_ICC_SIZE                 (1024)

/* �ڴ���� */
#define MEMORY_AXI_MEMMGR_FLAG_ADDR         (MEMORY_AXI_ICC_ADDR + MEMORY_AXI_ICC_SIZE)
#define MEMORY_AXI_MEMMGR_FLAG_SIZE         (1024)

/* ��̬�ڴ��� */
#define MEMORY_AXI_DYNAMIC_SEC_ADDR         (MEMORY_AXI_MEMMGR_FLAG_ADDR + MEMORY_AXI_MEMMGR_FLAG_SIZE)
#define MEMORY_AXI_DYNAMIC_SEC_SIZE         (48*1024 - (MEMORY_AXI_DYNAMIC_SEC_ADDR - GLOBAL_AXI_MEM_BASE_ADDR))

/* FLASH��Դ�� */
#define MEMORY_AXI_FLASH_SEM_ADDR           (MEMORY_AXI_DYNAMIC_SEC_ADDR + MEMORY_AXI_DYNAMIC_SEC_SIZE)
#define MEMORY_AXI_FLASH_SEM_SIZE           (4)

/* IFC��ʶ */
#define MEMORY_AXI_IFC_ADDR                 (MEMORY_AXI_FLASH_SEM_ADDR + MEMORY_AXI_FLASH_SEM_SIZE)
#define MEMORY_AXI_IFC_SIZE                 (32)

/* HIFI������ */
#define MEMORY_AXI_HIFI_ADDR                (MEMORY_AXI_IFC_ADDR + MEMORY_AXI_IFC_SIZE)
#define MEMORY_AXI_HIFI_SIZE                (6*1024)

/* ���� */
#define MEMORY_AXI_RESERVE_ADDR             (MEMORY_AXI_HIFI_ADDR + MEMORY_AXI_HIFI_SIZE)
#define MEMORY_AXI_RESERVE_SIZE             (GLOBAL_AXI_MEM_SIZE - (MEMORY_AXI_RESERVE_ADDR - GLOBAL_AXI_MEM_BASE_ADDR) - MEMORY_AXI_PTABLE_SIZE - MEMORY_AXI_FLAG_BAK_SIZE)

/* ��λ��Ǳ����� */
#define MEMORY_AXI_FLAG_BAK_ADDR             (MEMORY_AXI_RESERVE_ADDR + MEMORY_AXI_RESERVE_SIZE)
#define MEMORY_AXI_FLAG_BAK_SIZE             (1024)

/* Flash������ */
#define MEMORY_AXI_PTABLE_ADDR              (MEMORY_AXI_FLAG_BAK_ADDR + MEMORY_AXI_FLAG_BAK_SIZE)
#define MEMORY_AXI_PTABLE_SIZE              (2*1024)

#define MEMORY_AXI_END_ADDR               (MEMORY_AXI_PTABLE_ADDR + MEMORY_AXI_PTABLE_SIZE)


#define MEMORY_AXI_NEED_TO_SAVE_BEFORE_RESET   4
#define MEMORY_AXI_VX_BACK_UP_FLAG1 0x12121212
#define MEMORY_AXI_VX_BACK_UP_FLAG2 0x21212121

#define MEMORY_AXI_BOOTROM_BACK_UP_FLAG1 0x34343434
#define MEMORY_AXI_BOOTROM_BACK_UP_FLAG2 0x43434343
#if 0
/* IPF������ */
#define MEMORY_AXI_IPF_ADDR                 (MEMORY_AXI_SOFT_FLAG_ADDR + MEMORY_AXI_SOFT_FLAG_SIZE)
#define MEMORY_AXI_IPF_SIZE                 (30*1024)
#endif

#else
/*                      AXI Memory ����ͼ

               +---------------------+---------+ --> 0x2FFC0000
               | �쳣����            | 512B    |               
               +---------------------+---------+ --> 0x2FFC0200
               | FLASH��Դ��         | 32B     |
               +---------------------+---------+ --> 0x2FFC0220
               | IFC��ʶ             | 32B     |
               +---------------------+---------+ --> 0x2FFC0240
	           | ICC��ʶ             | 1K      |
	           +---------------------+---------+ --> 0x2FFC0640
	           | �ڴ����            | 1K      |
               +---------------------+---------+ --> 0x2FFC0A40
               | ��̬�ڴ���          |216K+256B|
               +---------------------+---------+ --> 0x2FFF6B40
               | IPF������           | 30K     |
               +---------------------+---------+ --> 0x2FFFE340
               | �±���              | 128B    |
               +---------------------+---------+ --> 0x2FFFE3C0
               | ���ػ�              | 32B     |
               +---------------------+---------+ --> 0x2FFFE3E0
               | DICC��ʶ            | 32B     |
               +---------------------+---------+ --> 0x2FFFE400
               | HIFI������          | 6K      |              
               +---------------------+---------+ --> 0x2FFFFC00
               | ������              | 1K-256B |
               +---------------------+---------+ --> 0x2FFFFF00
               | ������ʶ            | 256B    |              
               +---------------------+---------+ --> 0x30000000
*/ 

/* �쳣���� */
#define MEMORY_AXI_EXEC_VEC_ADDR            (GLOBAL_AXI_MEM_BASE_ADDR)
#define MEMORY_AXI_EXEC_VEC_SIZE            (512)

/* FLASH��Դ�� */
#define MEMORY_AXI_FLASH_SEM_ADDR           (MEMORY_AXI_EXEC_VEC_ADDR + MEMORY_AXI_EXEC_VEC_SIZE)
#define MEMORY_AXI_FLASH_SEM_SIZE           (32)

/* IFC��ʶ */
#define MEMORY_AXI_IFC_ADDR                 (MEMORY_AXI_FLASH_SEM_ADDR + MEMORY_AXI_FLASH_SEM_SIZE)
#define MEMORY_AXI_IFC_SIZE                 (32)

/* ICC��ʶ */
#define MEMORY_AXI_ICC_ADDR                 (MEMORY_AXI_IFC_ADDR + MEMORY_AXI_IFC_SIZE)
#define MEMORY_AXI_ICC_SIZE                 (1024)

/* �ڴ���� */
#define MEMORY_AXI_MEMMGR_FLAG_ADDR         (MEMORY_AXI_ICC_ADDR + MEMORY_AXI_ICC_SIZE)
#define MEMORY_AXI_MEMMGR_FLAG_SIZE         (1024)

/* ��̬�ڴ��� */
#define MEMORY_AXI_DYNAMIC_SEC_ADDR         (MEMORY_AXI_MEMMGR_FLAG_ADDR + MEMORY_AXI_MEMMGR_FLAG_SIZE)
#define MEMORY_AXI_DYNAMIC_SEC_SIZE         (214*1024 + 256 -4-4)

/*CSHELL*/
#define MEMORY_AXI_USB_CSHELL_ADDR         (MEMORY_AXI_DYNAMIC_SEC_ADDR + MEMORY_AXI_DYNAMIC_SEC_SIZE)
#define MEMORY_AXI_USB_CSHELL_SIZE         (4)

/* USB�β� */
#define MEMORY_AXI_USB_INOUT_ADDR          (MEMORY_AXI_USB_CSHELL_ADDR + MEMORY_AXI_USB_CSHELL_SIZE)
#define MEMORY_AXI_USB_INOUT_SIZE          (4)

/* ���־�� */
#define MEMORY_AXI_SOFT_FLAG_ADDR           (MEMORY_AXI_USB_INOUT_ADDR + MEMORY_AXI_USB_INOUT_SIZE) 
#define MEMORY_AXI_SOFT_FLAG_SIZE           (1024)

/* IPF������ */
#define MEMORY_AXI_IPF_ADDR                 (MEMORY_AXI_SOFT_FLAG_ADDR + MEMORY_AXI_SOFT_FLAG_SIZE)
#define MEMORY_AXI_IPF_SIZE                 (30*1024)

/* �±��� */
#define MEMORY_AXI_TEMP_PROTECT_ADDR        (MEMORY_AXI_IPF_ADDR + MEMORY_AXI_IPF_SIZE)
#define MEMORY_AXI_TEMP_PROTECT_SIZE        (128)

/* ���ػ� */
#define MEMORY_AXI_ONOFF_ADDR               (MEMORY_AXI_TEMP_PROTECT_ADDR + MEMORY_AXI_TEMP_PROTECT_SIZE)
#define MEMORY_AXI_ONOFF_SIZE               (32)

/* DICC��ʶ */
#define MEMORY_AXI_DICC_ADDR                (MEMORY_AXI_ONOFF_ADDR + MEMORY_AXI_ONOFF_SIZE)
#define MEMORY_AXI_DICC_SIZE                (32)

/* HIFI������ */
#define MEMORY_AXI_HIFI_ADDR                (MEMORY_AXI_DICC_ADDR + MEMORY_AXI_DICC_SIZE)
#define MEMORY_AXI_HIFI_SIZE                (6*1024)

/* Flash������ */
#define MEMORY_AXI_PTABLE_ADDR              (MEMORY_AXI_HIFI_ADDR + MEMORY_AXI_HIFI_SIZE)
#define MEMORY_AXI_PTABLE_SIZE              (1024)

/* ���� */
#define MEMORY_AXI_RESERVE_ADDR             (MEMORY_AXI_PTABLE_ADDR + MEMORY_AXI_PTABLE_SIZE)
#define MEMORY_AXI_RESERVE_SIZE             (1024 -256)

/* ������ʶ */
#define MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR   (MEMORY_AXI_RESERVE_ADDR + MEMORY_AXI_RESERVE_SIZE)
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (48)

#define MEMORY_AXI_DLOAD_ADDR               (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR + MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE)
#define MEMORY_AXI_DLOAD_SIZE               (256-48)

#define MEMORY_AXI_END_ADDR               (MEMORY_AXI_DLOAD_ADDR + MEMORY_AXI_DLOAD_SIZE)
#endif /*#if defined(CHIP_BB_6756CS)*/

#elif (defined (BOARD_FPGA) && defined(VERSION_V3R2))
/* FPGA P500 */

/* ����DDR�ڴ����ַ����С */
#define GLOBAL_MEM_BASE_ADDR                   (0xC0000000)
#define GLOBAL_MEM_SIZE                        (0x10000000)

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0xC0000000
               | ACP�ڴ�             | 0M      | 
               +---------------------+---------+ --> 0xC0000000
               | C���ڴ�             | 63M-128K| 
               +---------------------+---------+ --> 0xC3EE0000
               | Debug��Ϣ����       | 4K      | 
               +---------------------+---------+ --> 0xC3EE1000
               | �쳣��¼            | 1M      |
               +---------------------+---------+ --> 0xC3FE1000
               | UNUSED�ڴ�0         | 124K    |
               +---------------------+---------+ --> 0xC4000000
	           | �����ڴ�            | 4M      |
               +---------------------+---------+ --> 0xC4400000
               | A���ڴ�             | 32M     |
               +---------------------+---------+ --> 0xC6400000
               | UNUSED�ڴ�1         | 116M     |
               +---------------------+---------+ --> 0xCD800000
               | GUPSԤ���ڴ�        | 28M     |              
               +---------------------+---------+ --> 0xCF400000
               | UNUSED�ڴ�          | 12M     |
               +---------------------+---------+ --> 0xD0000000               
*/
/* MCORE ACPʹ��*/
#define GLOBAL_MEM_MCORE_ACP_ADDR              (GLOBAL_MEM_BASE_ADDR)
#define GLOBAL_MEM_MCORE_ACP_SIZE              (0x00000000)

/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_MCORE_ACP_ADDR + GLOBAL_MEM_MCORE_ACP_SIZE)    /*0xC0200000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x03EE0000)

/* Debug�ڴ� */
#define GLOBAL_MEM_DEBUG_BASE_ADDR             (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)       /*0xC3EE0000*/
#define GLOBAL_MEM_DEBUG_SIZE                  (0x00001000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_DEBUG_BASE_ADDR + GLOBAL_MEM_DEBUG_SIZE)       /*0xC3EE1000*/
#define GLOBAL_MEM_EXCP_SIZE                   (0x00100000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED0_SIZE                (0x0001F000)

/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE + GLOBAL_MEM_UNUSED0_SIZE)  /*0xC4000000*/
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00400000)

/* ACOREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)  /*0xC4400000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x02000000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED1_SIZE                 (0x07400000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE + GLOBAL_MEM_UNUSED1_SIZE)  /*0xCD800000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (0xF3400000)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x01C00000)

/* ����AXI�ڴ� */
#define GLOBAL_AXI_MEM_BASE_ADDR_PHY           (0x30000000)
#define GLOBAL_AXI_MEM_BASE_ADDR_VIRT          (IO_ADDRESS(GLOBAL_AXI_MEM_BASE_ADDR_PHY))
#define GLOBAL_AXI_MEM_SIZE                    (0x00020000)
#ifdef BSP_CORE_MODEM
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#else
#ifdef __FASTBOOT__
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#else
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_VIRT)
#endif

#define AXI_MEM_BASE_ADDR                      (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#define AXI_MEM_ADDR                           (GLOBAL_AXI_MEM_BASE_ADDR)
#define AXI_MEM_SIZE                           (GLOBAL_AXI_MEM_SIZE)
#endif

/*                      AXI Memory ����ͼ

               +---------------------+---------+ --> 0x30000000
               | �쳣����            | 512B    |               
               +---------------------+---------+ --> 0x30000200
               | FLASH��Դ��         | 32B     |
               +---------------------+---------+ --> 0x30000220
               | IFC��ʶ             | 32B     |
               +---------------------+---------+ --> 0x30000240
	           | ICC��ʶ             | 1K      |
	           +---------------------+---------+ --> 0x30000640
	           | �ڴ����            | 1K      |
               +---------------------+---------+ --> 0x30000A40
               | ��̬�ڴ���          | 30K+256B|
               +---------------------+---------+ --> 0x30008340
               | IPF������           | 30K     |
               +---------------------+---------+ --> 0x3000FB40
               | �±���              | 128B    |
               +---------------------+---------+ --> 0x3000FBC0
               | ���ػ�              | 32B     |
               +---------------------+---------+ --> 0x3000FBE0
               | BBPMaster           | 64K     |
               +---------------------+---------+ --> 0x3001FBE0
               | DICC��ʶ            | 32B     |
               +---------------------+---------+ --> 0x3001FC00
               | HIFI������          | 0       |              
               +---------------------+---------+ --> 0x3001FC00
               | ������              | 1K-256B |
               +---------------------+---------+ --> 0x3001FF00
               | ������ʶ            | 256B    |              
               +---------------------+---------+ --> 0x30020000
*/ 

/* �쳣���� */
#define MEMORY_AXI_EXEC_VEC_ADDR            (GLOBAL_AXI_MEM_BASE_ADDR)
#define MEMORY_AXI_EXEC_VEC_SIZE            (512)

/* FLASH��Դ�� */
#define MEMORY_AXI_FLASH_SEM_ADDR           (MEMORY_AXI_EXEC_VEC_ADDR + MEMORY_AXI_EXEC_VEC_SIZE)
#define MEMORY_AXI_FLASH_SEM_SIZE           (32)

/* IFC��ʶ */
#define MEMORY_AXI_IFC_ADDR                 (MEMORY_AXI_FLASH_SEM_ADDR + MEMORY_AXI_FLASH_SEM_SIZE)
#define MEMORY_AXI_IFC_SIZE                 (32)

/* ICC��ʶ */
#define MEMORY_AXI_ICC_ADDR                 (MEMORY_AXI_IFC_ADDR + MEMORY_AXI_IFC_SIZE)
#define MEMORY_AXI_ICC_SIZE                 (1024)

/* �ڴ���� */
#define MEMORY_AXI_MEMMGR_FLAG_ADDR         (MEMORY_AXI_ICC_ADDR + MEMORY_AXI_ICC_SIZE)
#define MEMORY_AXI_MEMMGR_FLAG_SIZE         (1024)

/* ��̬�ڴ��� */
#define MEMORY_AXI_DYNAMIC_SEC_ADDR         (MEMORY_AXI_MEMMGR_FLAG_ADDR + MEMORY_AXI_MEMMGR_FLAG_SIZE)
#define MEMORY_AXI_DYNAMIC_SEC_SIZE         (92*1024 + 256)

/* ���־�� */
#define MEMORY_AXI_SOFT_FLAG_ADDR           (MEMORY_AXI_DYNAMIC_SEC_ADDR + MEMORY_AXI_DYNAMIC_SEC_SIZE) 
#define MEMORY_AXI_SOFT_FLAG_SIZE           (1024)

/* IPF������ */
#define MEMORY_AXI_IPF_ADDR                 (MEMORY_AXI_SOFT_FLAG_ADDR + MEMORY_AXI_SOFT_FLAG_SIZE)
#define MEMORY_AXI_IPF_SIZE                 (30*1024)

/* �±��� */
#define MEMORY_AXI_TEMP_PROTECT_ADDR        (MEMORY_AXI_IPF_ADDR + MEMORY_AXI_IPF_SIZE)
#define MEMORY_AXI_TEMP_PROTECT_SIZE        (128)

/* ���ػ� */
#define MEMORY_AXI_ONOFF_ADDR               (MEMORY_AXI_TEMP_PROTECT_ADDR + MEMORY_AXI_TEMP_PROTECT_SIZE)
#define MEMORY_AXI_ONOFF_SIZE               (32)

/* DICC��ʶ */
#define MEMORY_AXI_DICC_ADDR                (MEMORY_AXI_ONOFF_ADDR + MEMORY_AXI_ONOFF_SIZE)
#define MEMORY_AXI_DICC_SIZE                (32)

/* HIFI������ */
#define MEMORY_AXI_HIFI_ADDR                (MEMORY_AXI_DICC_ADDR + MEMORY_AXI_DICC_SIZE)
#define MEMORY_AXI_HIFI_SIZE                (0)

/* Flash������ */
#define MEMORY_AXI_PTABLE_ADDR              (MEMORY_AXI_HIFI_ADDR + MEMORY_AXI_HIFI_SIZE)
#define MEMORY_AXI_PTABLE_SIZE              (1024)

/* ���� */
#define MEMORY_AXI_RESERVE_ADDR             (MEMORY_AXI_PTABLE_ADDR + MEMORY_AXI_PTABLE_SIZE)
#define MEMORY_AXI_RESERVE_SIZE             (1024 -256)

/* ������ʶ */
#define MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR   (MEMORY_AXI_RESERVE_ADDR + MEMORY_AXI_RESERVE_SIZE)
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (48)

#define MEMORY_AXI_DLOAD_ADDR               (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR + MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE)
#define MEMORY_AXI_DLOAD_SIZE               (256-48)

#define MEMORY_AXI_END_ADDR               (MEMORY_AXI_DLOAD_ADDR + MEMORY_AXI_DLOAD_SIZE)

#elif defined  (BOARD_SFT) && defined(VERSION_V3R2)
/* SFT */

/* ����DDR�ڴ����ַ����С*/
#define GLOBAL_MEM_BASE_ADDR                   (0x30000000)
#define GLOBAL_MEM_SIZE                        (0x08000000)

/*                      DDR ����ͼ

               +---------------------+---------+ --> 0x30000000
               | ACP�ڴ�             | 1M      | 
               +---------------------+---------+ --> 0x30100000
               | C���ڴ�             | 62M-24K | 
               +---------------------+---------+ --> 0x33EFA000
               | Debug��Ϣ����        | 4K      | 
               +---------------------+---------+ --> 0x33EFB000
               | �쳣��¼            | 1M      |
               +---------------------+---------+ --> 0x33FFB000
               | UNUSED�ڴ�0         | 20K     |
               +---------------------+---------+ --> 0x34000000
	           | �����ڴ�            | 4M      |
               +---------------------+---------+ --> 0x34400000
               | A���ڴ�             | 32M     |
               +---------------------+---------+ --> 0x36400000
               | UNUSED�ڴ�1         | 12M     |
               +---------------------+---------+ --> 0x37000000
               | GUPSԤ���ڴ�        | 16M     |              
               +---------------------+---------+ --> 0x38000000
          
*/
/* MCORE ACP */
#define GLOBAL_MEM_MCORE_ACP_ADDR              (GLOBAL_MEM_BASE_ADDR)
#define GLOBAL_MEM_MCORE_ACP_SIZE              (0x00100000)

/* MCOREʹ���ڴ� */
#define GLOBAL_MEM_MCORE_BASE_ADDR             (GLOBAL_MEM_MCORE_ACP_ADDR + GLOBAL_MEM_MCORE_ACP_SIZE)    /*0x30100000*/
#define GLOBAL_MEM_MCORE_SIZE                  (0x03DFA000)

/* Debug�ڴ� */
#define GLOBAL_MEM_DEBUG_BASE_ADDR             (GLOBAL_MEM_MCORE_BASE_ADDR + GLOBAL_MEM_MCORE_SIZE)    /*0x33EFA000*/
#define GLOBAL_MEM_DEBUG_SIZE                  (0x00001000)

/* �쳣��¼�ڴ� */
#define GLOBAL_MEM_EXCP_BASE_ADDR              (GLOBAL_MEM_DEBUG_BASE_ADDR + GLOBAL_MEM_DEBUG_SIZE)       /*0x33EFB000*/
#define GLOBAL_MEM_EXCP_SIZE                   (0x00100000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED0_SIZE                (0x00005000)

/* �˼乲���ڴ� */
#define GLOBAL_MEM_CORE_SHARE_ADDR             (GLOBAL_MEM_EXCP_BASE_ADDR + GLOBAL_MEM_EXCP_SIZE + GLOBAL_MEM_UNUSED0_SIZE)  /*0x34000000*/
#define GLOBAL_MEM_CORE_SHARE_SIZE             (0x00400000)

/* A COREʹ���ڴ� */
#define GLOBAL_MEM_ACORE_BASE_ADDR             (GLOBAL_MEM_CORE_SHARE_ADDR + GLOBAL_MEM_CORE_SHARE_SIZE)  /*0x34400000*/
#define GLOBAL_MEM_ACORE_SIZE                  (0x02000000)

/* δʹ���ڴ��С */
#define GLOBAL_MEM_UNUSED1_SIZE                 (0x00A00000)

/* ��GUԤ�����ڴ� */
#define GLOBAL_MEM_GU_RESERVED_ADDR            (GLOBAL_MEM_ACORE_BASE_ADDR + GLOBAL_MEM_ACORE_SIZE + GLOBAL_MEM_UNUSED1_SIZE)  /*0x37000000*/
#define GLOBAL_MEM_GU_RESERVED_ADDR_VIRT       (0xF3400000)
#define GLOBAL_MEM_GU_RESERVED_SIZE            (0x01200000)

/* ����AXI�ڴ� */
#define GLOBAL_AXI_MEM_BASE_ADDR_PHY           (0x2FFE0000)
#define GLOBAL_AXI_MEM_BASE_ADDR_VIRT          (IO_ADDRESS(GLOBAL_AXI_MEM_BASE_ADDR_PHY))
#define GLOBAL_AXI_MEM_SIZE                    (0x00020000)
#ifdef BSP_CORE_MODEM
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#else
#ifdef __FASTBOOT__
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#else
#define GLOBAL_AXI_MEM_BASE_ADDR               (GLOBAL_AXI_MEM_BASE_ADDR_VIRT)
#endif

#define AXI_MEM_BASE_ADDR                      (GLOBAL_AXI_MEM_BASE_ADDR_PHY)
#define AXI_MEM_ADDR                           (GLOBAL_AXI_MEM_BASE_ADDR)
#define AXI_MEM_SIZE                           (GLOBAL_AXI_MEM_SIZE)
#endif


/*                      AXI Memory ����ͼ

               +---------------------+---------+ --> 0x2FFE0000
               | A�˵͹���           | 16KB    |               
               +---------------------+---------+ --> 0x2FFE4000
               | FLASH��Դ��         | 32B     |
               +---------------------+---------+ --> 0x2FFE4020
               | IFC��ʶ             | 32B     |
               +---------------------+---------+ --> 0x2FFE4040
	           | ICC��ʶ             | 1K      |
	           +---------------------+---------+ --> 0x2FFE4440
	           | �ڴ����            | 1K      |
               +---------------------+---------+ --> 0x2FFE4840
               | ��̬�ڴ���          |73K-256B|
               +---------------------+---------+ --> 0x2FFF6B40
               | IPF������           | 30K     |
               +---------------------+---------+ --> 0x2FFFE340
               | �±���              | 128B    |
               +---------------------+---------+ --> 0x2FFFE3C0
               | ���ػ�              | 32B     |
               +---------------------+---------+ --> 0x2FFFE3E0
               | DICC��ʶ            | 32B     |
               +---------------------+---------+ --> 0x2FFFE400
               | HIFI������          | 6K      |              
               +---------------------+---------+ --> 0x2FFFFC00
               | ������              | 1K-256B |
               +---------------------+---------+ --> 0x2FFFFF00
               | ������ʶ            | 256B    |              
               +---------------------+---------+ --> 0x30000000
*/ 

/* A�˵͹��� */
#define MEMORY_AXI_ACPU_DEEPSLEEP_ADDR      (GLOBAL_AXI_MEM_BASE_ADDR)
#define MEMORY_AXI_ACPU_DEEPSLEEP_SIZE      (0x4000)

/* FLASH��Դ�� */
#define MEMORY_AXI_FLASH_SEM_ADDR           (MEMORY_AXI_ACPU_DEEPSLEEP_ADDR + MEMORY_AXI_ACPU_DEEPSLEEP_SIZE)
#define MEMORY_AXI_FLASH_SEM_SIZE           (32)

/* IFC��ʶ */
#define MEMORY_AXI_IFC_ADDR                 (MEMORY_AXI_FLASH_SEM_ADDR + MEMORY_AXI_FLASH_SEM_SIZE)
#define MEMORY_AXI_IFC_SIZE                 (32)

/* ICC��ʶ */
#define MEMORY_AXI_ICC_ADDR                 (MEMORY_AXI_IFC_ADDR + MEMORY_AXI_IFC_SIZE)
#define MEMORY_AXI_ICC_SIZE                 (1024)

/* �ڴ���� */
#define MEMORY_AXI_MEMMGR_FLAG_ADDR         (MEMORY_AXI_ICC_ADDR + MEMORY_AXI_ICC_SIZE)
#define MEMORY_AXI_MEMMGR_FLAG_SIZE         (1024)

/* ��̬�ڴ��� */
#define MEMORY_AXI_DYNAMIC_SEC_ADDR         (MEMORY_AXI_MEMMGR_FLAG_ADDR + MEMORY_AXI_MEMMGR_FLAG_SIZE)
#define MEMORY_AXI_DYNAMIC_SEC_SIZE         (71*1024 - 256)

/* ���־�� */
#define MEMORY_AXI_SOFT_FLAG_ADDR           (MEMORY_AXI_DYNAMIC_SEC_ADDR + MEMORY_AXI_DYNAMIC_SEC_SIZE) 
#define MEMORY_AXI_SOFT_FLAG_SIZE           (1024)

/* IPF������ */
#define MEMORY_AXI_IPF_ADDR                 (MEMORY_AXI_SOFT_FLAG_ADDR + MEMORY_AXI_SOFT_FLAG_SIZE)
#define MEMORY_AXI_IPF_SIZE                 (30*1024)

/* �±��� */
#define MEMORY_AXI_TEMP_PROTECT_ADDR        (MEMORY_AXI_IPF_ADDR + MEMORY_AXI_IPF_SIZE)
#define MEMORY_AXI_TEMP_PROTECT_SIZE        (128)

/* ���ػ� */
#define MEMORY_AXI_ONOFF_ADDR               (MEMORY_AXI_TEMP_PROTECT_ADDR + MEMORY_AXI_TEMP_PROTECT_SIZE)
#define MEMORY_AXI_ONOFF_SIZE               (32)

/* DICC��ʶ */
#define MEMORY_AXI_DICC_ADDR                (MEMORY_AXI_ONOFF_ADDR + MEMORY_AXI_ONOFF_SIZE)
#define MEMORY_AXI_DICC_SIZE                (32)

/* HIFI������ */
#define MEMORY_AXI_HIFI_ADDR                (MEMORY_AXI_DICC_ADDR + MEMORY_AXI_DICC_SIZE)
#define MEMORY_AXI_HIFI_SIZE                (6*1024)

/* Flash������ */
#define MEMORY_AXI_PTABLE_ADDR              (MEMORY_AXI_HIFI_ADDR + MEMORY_AXI_HIFI_SIZE)
#define MEMORY_AXI_PTABLE_SIZE              (1024)

/* ���� */
#define MEMORY_AXI_RESERVE_ADDR             (MEMORY_AXI_PTABLE_ADDR + MEMORY_AXI_PTABLE_SIZE)
#define MEMORY_AXI_RESERVE_SIZE             (1024 -256)

/* ������ʶ */
#define MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR   (MEMORY_AXI_RESERVE_ADDR + MEMORY_AXI_RESERVE_SIZE)
#define MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE   (48)

#define MEMORY_AXI_DLOAD_ADDR               (MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR + MEMORY_AXI_DLOAD_AUTOINSTALL_SIZE)
#define MEMORY_AXI_DLOAD_SIZE               (256-48)

#define MEMORY_AXI_END_ADDR               (MEMORY_AXI_DLOAD_ADDR + MEMORY_AXI_DLOAD_SIZE)

#endif

/*Memory size check*/
/*#if (MEMORY_AXI_END_ADDR != (AXI_MEM_ADDR + AXI_MEM_SIZE))
#error AXI(MEMORY_AXI_END_ADDR) USED beyond the mark (AXI_MEM_ADDR+AXI_MEM_SIZE)
#endif*/

/*-------------------------------------------------------------------------------
                            ˫�˹����ڴ�
  -------------------------------------------------------------------------------
                         ----------------------- 
                         |  SYNC(0x60)                  |
                         ----------------------- 
                         |  MEM spinlock(0x20)       |
                         ----------------------- 
                         |  ICC(0x80)                    |
                         ----------------------- 
                         | TENCILICA_TEXT(0)       |
                         ----------------------- 
                         |  WAN0x8000)                 |
                         ----------------------- 
                         |  MEM MGR M                  |
                         ----------------------- 
-------------------------------------------------------------------------------*/
/* �Ӷ����·��䣬���µ�ַ�ʹ�С���ɸı� */
#define MEMORY_RAM_CORESHARE_SIZE                   (GLOBAL_MEM_CORE_SHARE_SIZE)

#define MEMORY_RAM_CORESHARE_SYNC_RESV              (96)
#define MEMORY_RAM_CORESHARE_MEM_RESV               (32)
#define MEMORY_RAM_CORESHARE_ICC_RESV               (128)
#define MEMORY_RAM_CORESHARE_TENCILICA_TEXT_RESV    (0)
#define MEMORY_RAM_CORESHARE_MEM_WAN_SIZE           (0x8000)
#if (defined(BOARD_ASIC) && defined(VERSION_V3R2) && (defined(CHIP_BB_6756CS)))
#define MEMORY_RAM_CORESHARE_MEM_IPF_SIZE            (30*1024)
#define MEMORY_RAM_CORESHARE_MEMMGR_SIZE            (MEMORY_RAM_CORESHARE_SIZE \
                                                    - MEMORY_RAM_CORESHARE_ICC_RESV \
                                                    - MEMORY_RAM_CORESHARE_MEM_RESV \
                                                    - MEMORY_RAM_CORESHARE_SYNC_RESV \
                                                    - MEMORY_RAM_CORESHARE_TENCILICA_TEXT_RESV\
                                                    - MEMORY_RAM_CORESHARE_MEM_WAN_SIZE\
                                                    - MEMORY_RAM_CORESHARE_MEM_IPF_SIZE)
#else
#define MEMORY_RAM_CORESHARE_MEMMGR_SIZE            (MEMORY_RAM_CORESHARE_SIZE \
                                                    - MEMORY_RAM_CORESHARE_ICC_RESV \
                                                    - MEMORY_RAM_CORESHARE_MEM_RESV \
                                                    - MEMORY_RAM_CORESHARE_SYNC_RESV \
                                                    - MEMORY_RAM_CORESHARE_TENCILICA_TEXT_RESV\
                                                    - MEMORY_RAM_CORESHARE_MEM_WAN_SIZE)
#endif

#ifdef BSP_CORE_MODEM
#define MEMORY_RAM_CORESHARE_ADDR                   (GLOBAL_MEM_CORE_SHARE_ADDR)
#else
#define MEMORY_RAM_CORESHARE_ADDR                   (GLOBAL_MEM_CORE_SHARE_ADDR_VIRT)
#endif
#define MEMORY_RAM_CORESHARE_MEMMGR_ADDR            (MEMORY_RAM_CORESHARE_ADDR)
#if (defined(BOARD_ASIC) && defined(VERSION_V3R2) && (defined(CHIP_BB_6756CS)))
#define MEMORY_RAM_CORESHARE_IPF_ADDR               (MEMORY_RAM_CORESHARE_MEMMGR_ADDR + MEMORY_RAM_CORESHARE_MEMMGR_SIZE)
#define MEMORY_RAM_CORESHARE_MEM_WAN_ADDR           (MEMORY_RAM_CORESHARE_IPF_ADDR + MEMORY_RAM_CORESHARE_MEM_IPF_SIZE)
#else
#define MEMORY_RAM_CORESHARE_MEM_WAN_ADDR           (MEMORY_RAM_CORESHARE_MEMMGR_ADDR+MEMORY_RAM_CORESHARE_MEMMGR_SIZE)
#endif
#define MEMORY_RAM_CORESHARE_TENCILICA_TEXT_ADDR    (MEMORY_RAM_CORESHARE_MEM_WAN_ADDR+MEMORY_RAM_CORESHARE_MEM_WAN_SIZE)
#define MEMORY_RAM_CORESHARE_ICC_ADDR               (MEMORY_RAM_CORESHARE_TENCILICA_TEXT_ADDR+MEMORY_RAM_CORESHARE_TENCILICA_TEXT_RESV)           
#define MEMORY_RAM_CORESHARE_MEM_ADDR               (MEMORY_RAM_CORESHARE_ICC_ADDR + MEMORY_RAM_CORESHARE_ICC_RESV) 
#define MEMORY_RAM_CORESHARE_SYNC_ADDR              (MEMORY_RAM_CORESHARE_MEM_ADDR + MEMORY_RAM_CORESHARE_MEM_RESV)

#if (defined(BOARD_ASIC) && defined(VERSION_V3R2) && (defined(CHIP_BB_6756CS)))
#define MEMORY_AXI_IPF_ADDR MEMORY_RAM_CORESHARE_IPF_ADDR
#define MEMORY_AXI_IPF_SIZE MEMORY_RAM_CORESHARE_MEM_IPF_SIZE
#endif

/* ʹ�������ַ */
#define CORESHARE_MEM_WAN_ADDR                      (MEMORY_RAM_CORESHARE_MEM_WAN_ADDR - MEMORY_RAM_CORESHARE_ADDR + GLOBAL_MEM_CORE_SHARE_ADDR)
#define CORESHARE_MEM_WAN_SIZE                      (MEMORY_RAM_CORESHARE_MEM_WAN_SIZE)


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of __MEMMORY_LAYOUT_H__ */

